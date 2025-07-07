//
// Created by HichTala on 24/06/25.
//

#include <obs-frontend-api.h>
#include "draw.h"

#include "plugin-support.h"
#include "util/dstr.h"

const char *draw_source_get_name(void *type_data)
{
	UNUSED_PARAMETER(type_data);
	return obs_module_text("Draw Display");
}

void *draw_source_create(obs_data_t *settings, obs_source_t *source)
{
	UNUSED_PARAMETER(settings);
	draw_source_data_t *context = bzalloc(sizeof(draw_source_data_t));
	context->cx = 1;
	context->cy = 1;
	obs_source_update(source, NULL);
	return context;
}

void draw_source_destroy(void *data)
{
	draw_source_data_t *context = data;

	obs_source_t *source = obs_weak_source_get_source(context->source);
	if (source) {
		obs_source_release(source);
	}
	obs_weak_source_release(context->source);
	obs_weak_source_release(context->current_scene);
	if (context->render) {
		obs_enter_graphics();
		gs_texrender_destroy(context->render);
		obs_leave_graphics();
	}
	bfree(context);
}

uint32_t draw_source_get_height(void *data)
{
	draw_source_data_t *context = data;
	if (!context->source)
		return 1;
	obs_source_t *source = obs_weak_source_get_source(context->source);
	if (!source)
		return 1;
	uint32_t height = obs_source_get_height(source);
	obs_source_release(source);
	return height;
}
uint32_t draw_source_get_width(void *data)
{
	draw_source_data_t *context = data;
	if (!context->source)
		return 1;
	obs_source_t *source = obs_weak_source_get_source(context->source);
	if (!source)
		return 1;
	uint32_t width = obs_source_get_width(source);
	obs_source_release(source);
	return width;
}

void draw_source_get_defaults(obs_data_t *settings)
{
	UNUSED_PARAMETER(settings);
}
static const char *get_tech_name_and_multiplier(enum gs_color_space current_space, enum gs_color_space source_space,
						float *multiplier)
{
	const char *tech_name = "Draw";
	*multiplier = 1.f;

	switch (source_space) {
	case GS_CS_SRGB:
	case GS_CS_SRGB_16F:
		switch (current_space) {
		case GS_CS_709_SCRGB:
			tech_name = "DrawMultiply";
			*multiplier = obs_get_video_sdr_white_level() / 80.0f;
		default:;
		}
		break;
	case GS_CS_709_EXTENDED:
		switch (current_space) {
		case GS_CS_SRGB:
		case GS_CS_SRGB_16F:
			tech_name = "DrawTonemap";
			break;
		case GS_CS_709_SCRGB:
			tech_name = "DrawMultiply";
			*multiplier = obs_get_video_sdr_white_level() / 80.0f;
		default:;
		}
		break;
	case GS_CS_709_SCRGB:
		switch (current_space) {
		case GS_CS_SRGB:
		case GS_CS_SRGB_16F:
			tech_name = "DrawMultiplyTonemap";
			*multiplier = 80.0f / obs_get_video_sdr_white_level();
			break;
		case GS_CS_709_EXTENDED:
			tech_name = "DrawMultiply";
			*multiplier = 80.0f / obs_get_video_sdr_white_level();
		default:;
		}
	}

	return tech_name;
}
static void draw_source_draw_frame(draw_source_data_t *context)
{

	const enum gs_color_space current_space = gs_get_color_space();
	float multiplier;
	const char *technique = get_tech_name_and_multiplier(current_space, context->space, &multiplier);

	gs_effect_t *effect = obs_get_base_effect(OBS_EFFECT_DEFAULT);
	gs_texture_t *tex = gs_texrender_get_texture(context->render);
	if (!tex)
		return;
	const bool previous = gs_framebuffer_srgb_enabled();
	gs_enable_framebuffer_srgb(true);

	gs_effect_set_texture_srgb(gs_effect_get_param_by_name(effect, "image"), tex);
	gs_effect_set_float(gs_effect_get_param_by_name(effect, "multiplier"), multiplier);

	while (gs_effect_loop(effect, technique))
		gs_draw_sprite(tex, 0, context->cx, context->cy);

	gs_enable_framebuffer_srgb(previous);
}
void draw_source_video_render(void *data, gs_effect_t *effect)
{
	UNUSED_PARAMETER(effect);
	draw_source_data_t *context = data;
	if (context->input_type == INPUT_TYPE_SOURCE && !context->source)
		return;

	if (context->rendering)
		return;
	context->rendering = true;
	obs_source_t *source = obs_weak_source_get_source(context->source);
	if (!source) {
		context->rendering = false;
		return;
	}

	obs_source_video_render(source);
	obs_source_release(source);
	context->rendering = false;
	return;

	if (!context->source_cx || !context->source_cy) {
		obs_source_release(source);
		context->rendering = false;
		return;
	}

	const enum gs_color_space preferred_spaces[] = {
		GS_CS_SRGB,
		GS_CS_SRGB_16F,
		GS_CS_709_EXTENDED,
	};
	const enum gs_color_space space =
		obs_source_get_color_space(source, OBS_COUNTOF(preferred_spaces), preferred_spaces);
	const enum gs_color_format format = gs_get_format_from_space(space);
	if (!context->render || gs_texrender_get_format(context->render) != format) {
		gs_texrender_destroy(context->render);
		context->render = gs_texrender_create(format, GS_ZS_NONE);
	} else {
		gs_texrender_reset(context->render);
	}

	gs_blend_state_push();
	gs_blend_function(GS_BLEND_ONE, GS_BLEND_ZERO);
	if (gs_texrender_begin_with_color_space(context->render, context->cx, context->cy, space)) {

		struct vec4 clear_color;

		vec4_zero(&clear_color);
		gs_clear(GS_CLEAR_COLOR, &clear_color, 0.0f, 0);
		if (context->source_cx && context->source_cy) {
			gs_ortho(0.0f, (float)context->source_cx, 0.0f, (float)context->source_cy, -100.0f, 100.0f);
			obs_source_video_render(source);
		}
		gs_texrender_end(context->render);

		context->space = space;
	}

	gs_blend_state_pop();

	context->processed_frame = true;
	obs_source_release(source);
	context->rendering = false;
	draw_source_draw_frame(context);
}
bool enum_cb(obs_scene_t *scene, obs_sceneitem_t *item, void *param)
{
	UNUSED_PARAMETER(scene);
	obs_source_t *source = obs_sceneitem_get_source(item);
	bool *found = param;
	const char *source_id = obs_source_get_id(source);

	if (strcmp(source_id, "draw_source") == 0) {
		*found = true;
		return false;
	}
	return true;
}
bool scene_contains_source(obs_source_t *source)
{
	if (!source)
		return false;
	if (strcmp(obs_source_get_id(source), "scene") != 0)
		return false;

	bool found = false;

	obs_scene_t *scene_data = obs_scene_from_source(source);
	obs_scene_enum_items(scene_data, enum_cb, &found);
	return found;
}
bool add_source_to_list(void *data, obs_source_t *source)
{
	if (scene_contains_source(source))
		return true;
	obs_property_t *prop = data;

	const char *name = obs_source_get_name(source);
	size_t count = obs_property_list_item_count(prop);
	size_t idx = 0;
	while (idx < count && strcmp(name, obs_property_list_item_string(prop, idx)) > 0)
		idx++;

	uint32_t flags = obs_source_get_output_flags(source);
	const char *source_id = obs_source_get_id(source);

	if (flags & OBS_SOURCE_VIDEO & (strcmp(source_id, "draw_source") != 0)) {
		obs_property_list_insert_string(prop, idx, name, name);
	}
	return true;
}
bool draw_source_type_changed(void *priv, obs_properties_t *props, obs_property_t *property, obs_data_t *settings)
{
	UNUSED_PARAMETER(property);
	UNUSED_PARAMETER(priv);
	obs_property_t *input_selection = obs_properties_get(props, "input_selection");
	obs_property_list_clear(input_selection);

	const bool selected_type = obs_data_get_int(settings, "input_type") == INPUT_TYPE_SOURCE;
	if (selected_type) {
		obs_enum_sources(add_source_to_list, input_selection);
	} else {
		obs_enum_scenes(add_source_to_list, input_selection);
	}
	return true;
}
obs_properties_t *draw_source_get_properties(void *data)
{
	obs_properties_t *props = obs_properties_create();
	obs_property_t *p = obs_properties_add_list(props, "input_type", obs_module_text("InputType"),
						    OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_INT);
	obs_property_list_add_int(p, obs_module_text("Source"), INPUT_TYPE_SOURCE);
	obs_property_list_add_int(p, obs_module_text("Scene"), INPUT_TYPE_SCENE);

	obs_property_set_modified_callback2(p, draw_source_type_changed, data);

	p = obs_properties_add_list(props, "input_selection", obs_module_text("InputSelection"), OBS_COMBO_TYPE_LIST,
				    OBS_COMBO_FORMAT_STRING);

	obs_property_list_insert_string(p, 0, "", "");

	return props;
}

void switch_source(draw_source_data_t *context, obs_source_t *source)
{
	obs_source_t *prev_source = obs_weak_source_get_source(context->source);
	if (prev_source) {
		obs_source_release(prev_source);
	}
	obs_weak_source_release(context->source);
	context->source = obs_source_get_weak_source(source);
}
void draw_source_update(void *data, obs_data_t *settings)
{
	draw_source_data_t *context = data;
	context->input_type = obs_data_get_int(settings, "input_type");
	const char *source_name = obs_data_get_string(settings, "input_selection");
	obs_source_t *source = obs_get_source_by_name(source_name);
	if (source) {
		if (!obs_weak_source_references_source(context->source, source)) {
			switch_source(context, source);
		}
		obs_source_release(source);
	}
}
struct obs_source_info draw_source = {.id = "draw_source",
				      .type = OBS_SOURCE_TYPE_INPUT,
				      .output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_CUSTOM_DRAW,
				      .get_name = draw_source_get_name, //ok
				      .create = draw_source_create,
				      .destroy = draw_source_destroy,
				      .update = draw_source_update,         //ok
				      .get_width = draw_source_get_width,   //ok
				      .get_height = draw_source_get_height, //ok
				      .get_defaults = draw_source_get_defaults,
				      .video_render = draw_source_video_render,
				      .get_properties = draw_source_get_properties, //ok
				      .icon_type = OBS_ICON_TYPE_COLOR};