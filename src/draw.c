//
// Created by HichTala on 24/06/25.
//

#include "draw.h"
#include "shared_memory_wrapper.h"

#include <errno.h>

const char *draw_source_get_name(void *type_data)
{
	UNUSED_PARAMETER(type_data);
	return obs_module_text("Draw Display");
}

void *draw_source_create(obs_data_t *settings, obs_source_t *source)
{
	UNUSED_PARAMETER(settings);
	draw_source_data_t *context = bzalloc(sizeof(draw_source_data_t));
	context->shared_frame = NULL;
	context->display_texture = NULL;
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
	if (context->render) {
		obs_enter_graphics();
		gs_texrender_destroy(context->render);
		obs_leave_graphics();
	}
	if (context->shared_frame) {
		destroy_shared_memory(context);
	}
	bfree(context);
}

uint32_t draw_source_get_height(void *data)
{
	draw_source_data_t *context = data;
	if (!context->display_height)
		return 391;
	return context->display_height;
}
uint32_t draw_source_get_width(void *data)
{
	draw_source_data_t *context = data;
	if (!context->display_width)
		return 268;
	return context->display_width;
}

void draw_source_get_defaults(obs_data_t *settings)
{
	UNUSED_PARAMETER(settings);
}

void draw_source_video_render(void *data, gs_effect_t *effect)
{
	UNUSED_PARAMETER(effect);
	draw_source_data_t *context = data;
	if (!context->source)
		return;

	if (context->processing)
		return;
	context->processing = true;
	obs_source_t *source = obs_weak_source_get_source(context->source);
	if (!source) {
		context->processing = false;
		return;
	}

	uint32_t width = context->source_width;
	uint32_t height = context->source_height;

	gs_texrender_t *render = gs_texrender_create(GS_RGBA, GS_ZS_NONE);
	if (!gs_texrender_begin(render, width, height)) {
		obs_source_release(source);
		context->processing = false;
		gs_texrender_destroy(render);
		return;
	}

	ensure_shared_memory_exists(context);

	struct vec4 clear_color;
	vec4_set(&clear_color, 0.0f, 0.0f, 0.0f, 1.0f); // black
	gs_clear(GS_CLEAR_COLOR, &clear_color, 0.0f, 0);
	gs_ortho(0.0f, (float)width, 0.0f, (float)height, -100.0f, 100.0f);

	obs_source_video_render(source);
	gs_texrender_end(render);

	gs_texture_t *texture = gs_texrender_get_texture(render);
	if (texture) {
		gs_stagesurf_t *stage = gs_stagesurface_create(width, height, GS_RGBA);
		gs_stage_texture(stage, texture);
		if (stage) {
			uint8_t *frame = NULL;
			uint32_t linesize = 0;

			if (gs_stagesurface_map(stage, &frame, &linesize)) {
				if (context->shared_frame) {
					write_message_to_shared_memory(context, frame, linesize, width, height);
				}
				gs_stagesurface_unmap(stage);
			}
			gs_stagesurface_destroy(stage);

		} else {
			blog(LOG_ERROR, "Failed to capture stage surface");
		}
	}

	gs_texrender_destroy(render);
	obs_source_release(source);

	if (!read_shared_memory(context)) {
		return;
	}

	gs_effect_t *default_effect = obs_get_base_effect(OBS_EFFECT_DEFAULT);
	gs_eparam_t *image = gs_effect_get_param_by_name(default_effect, "image");
	gs_effect_set_texture(image, context->display_texture);
	while (gs_effect_loop(default_effect, "Draw"))
		gs_draw_sprite(context->display_texture, 0, context->display_width, context->display_height);

	context->processing = false;
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
		context->source_width = obs_source_get_width(source);
		context->source_height = obs_source_get_height(source);
		init_shared_memory(context);
		obs_source_release(source);
	}
}

struct obs_source_info draw_source = {.id = "draw_source",
				      .type = OBS_SOURCE_TYPE_INPUT,
				      .output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_CUSTOM_DRAW,
				      .get_name = draw_source_get_name,
				      .create = draw_source_create,
				      .destroy = draw_source_destroy,
				      .update = draw_source_update,
				      .get_width = draw_source_get_width,
				      .get_height = draw_source_get_height,
				      .get_defaults = draw_source_get_defaults,
				      .video_render = draw_source_video_render,
				      .get_properties = draw_source_get_properties,
				      .icon_type = OBS_ICON_TYPE_COLOR};
