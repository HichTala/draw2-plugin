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

	obs_source_t *source = obs_weak_source_get_source(context->clone);
	if (source) {
		if (obs_source_showing(context->source))
			obs_source_dec_showing(source);
		if (context->active_clone && obs_source_active(context->source))
			obs_source_dec_active(source);
		obs_source_release(source);
	}
	obs_weak_source_release(context->clone);
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
	if (!context->clone)
		return 1;
	if (context->buffer_frame > 0)
		return context->cy;
	obs_source_t *source = obs_weak_source_get_source(context->clone);
	if (!source)
		return 1;
	uint32_t height = obs_source_get_height(source);
	obs_source_release(source);
	if (context->buffer_frame > 1)
		height /= context->buffer_frame;
	return height;
}
uint32_t draw_source_get_width(void *data)
{
	draw_source_data_t *context = data;
	if (!context->clone)
		return 1;
	if (context->buffer_frame > 0)
		return context->cx;
	obs_source_t *source = obs_weak_source_get_source(context->clone);
	if (!source)
		return 1;
	uint32_t width = obs_source_get_width(source);
	obs_source_release(source);
	if (context->buffer_frame > 1)
		width /= context->buffer_frame;
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
	if (context->clone_type == CLONE_SOURCE && !context->clone)
		return;

	if (context->buffer_frame > 0 && context->processed_frame) {
		draw_source_draw_frame(context);
		return;
	}
	if (context->rendering)
		return;
	context->rendering = true;
	obs_source_t *source = obs_weak_source_get_source(context->clone);
	if (!source) {
		context->rendering = false;
		return;
	}
	if (context->buffer_frame == 0) {
		obs_source_video_render(source);
		obs_source_release(source);
		context->rendering = false;
		return;
	}

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

bool draw_source_list_add_source(void *data, obs_source_t *source)
{
	obs_property_t *prop = data;

	const char *name = obs_source_get_name(source);
	size_t count = obs_property_list_item_count(prop);
	size_t idx = 0;
	while (idx < count && strcmp(name, obs_property_list_item_string(prop, idx)) > 0)
		idx++;
	obs_property_list_insert_string(prop, idx, name, name);
	return true;
}
struct same_clones {
	obs_data_t *settings;
	DARRAY(const char *) clones;
};
bool find_clones(void *data, obs_source_t *source)
{
	if (strcmp(obs_source_get_unversioned_id(source), "source-clone") != 0) {

		return true;
	}
	obs_data_t *settings = obs_source_get_settings(source);
	if (!settings)
		return true;
	struct same_clones *sc = data;
	if (settings == sc->settings) {
		obs_data_release(settings);
		return true;
	}
	if (obs_data_get_int(sc->settings, "clone_type") == CLONE_SOURCE) {
		if (obs_data_get_int(settings, "clone_type") == CLONE_SOURCE &&
		    strcmp(obs_data_get_string(sc->settings, "clone"), obs_data_get_string(settings, "clone")) == 0) {
			const char *name = obs_source_get_name(source);
			da_push_back(sc->clones, &name);
		}
	} else if (obs_data_get_int(sc->settings, "clone_type") == obs_data_get_int(settings, "clone_type")) {
		const char *name = obs_source_get_name(source);
		da_push_back(sc->clones, &name);
	}
	obs_data_release(settings);
	return true;
}
void find_same_clones(obs_properties_t *props, obs_data_t *settings)
{
	struct same_clones sc;
	sc.settings = settings;
	da_init(sc.clones);
	obs_enum_sources(find_clones, &sc);
	obs_property_t *prop = obs_properties_get(props, "same_clones");
	if (sc.clones.num) {
		struct dstr names;
		dstr_init_copy(&names, sc.clones.array[0]);
		for (size_t i = 1; i < sc.clones.num; i++) {
			dstr_cat(&names, "\n");
			dstr_cat(&names, sc.clones.array[i]);
		}
		obs_data_set_string(settings, "same_clones", names.array);
		dstr_free(&names);
		obs_property_set_visible(prop, true);
	} else {
		obs_data_unset_user_value(settings, "same_clones");
		obs_property_set_visible(prop, false);
	}
	da_free(sc.clones);
}
bool draw_source_source_changed(void *priv, obs_properties_t *props, obs_property_t *property, obs_data_t *settings)
{
	UNUSED_PARAMETER(property);
	draw_source_data_t *context = priv;
	const char *source_name = obs_data_get_string(settings, "clone");
	bool async = true;
	obs_source_t *source = obs_get_source_by_name(source_name);
	if (source == context->source) {
		obs_source_release(source);
		source = NULL;
	}
	if (source) {
		async = (obs_source_get_output_flags(source) & OBS_SOURCE_ASYNC) != 0;
		obs_source_release(source);
	}

	obs_property_t *no_filters = obs_properties_get(props, "no_filters");
	obs_property_set_visible(no_filters, !async);

	find_same_clones(props, settings);
	return true;
}
bool draw_source_type_changed(void *priv, obs_properties_t *props, obs_property_t *property, obs_data_t *settings)
{
	UNUSED_PARAMETER(priv);
	UNUSED_PARAMETER(property);
	obs_property_t *clone = obs_properties_get(props, "clone");
	const bool clone_source = obs_data_get_int(settings, "clone_type") == CLONE_SOURCE;
	obs_property_set_visible(clone, clone_source);
	if (clone_source) {
		draw_source_source_changed(priv, props, NULL, settings);
	} else {
		find_same_clones(props, settings);
	}
	return true;
}
obs_properties_t *draw_source_get_properties(void *data)
{
	obs_properties_t *props = obs_properties_create();
	obs_property_t *p = obs_properties_add_list(props, "clone_type", obs_module_text("CloneType"),
						    OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_INT);
	obs_property_list_add_int(p, obs_module_text("Source"), CLONE_SOURCE);
	obs_property_list_add_int(p, obs_module_text("CurrentScene"), CLONE_CURRENT_SCENE);
	obs_property_list_add_int(p, obs_module_text("PreviousScene"), CLONE_PREVIOUS_SCENE);

	obs_property_set_modified_callback2(p, draw_source_type_changed, data);

	p = obs_properties_add_list(props, "clone", obs_module_text("Clone"), OBS_COMBO_TYPE_EDITABLE,
				    OBS_COMBO_FORMAT_STRING);
	obs_enum_sources(draw_source_list_add_source, p);
	obs_enum_scenes(draw_source_list_add_source, p);
	obs_property_list_insert_string(p, 0, "", "");
	//add global audio sources
	for (uint32_t i = 1; i < 7; i++) {
		obs_source_t *s = obs_get_output_source(i);
		if (!s)
			continue;
		draw_source_list_add_source(p, s);
		obs_source_release(s);
	}
	obs_property_set_modified_callback2(p, draw_source_source_changed, data);

	p = obs_properties_add_list(props, "buffer_frame", obs_module_text("VideoBuffer"), OBS_COMBO_TYPE_LIST,
				    OBS_COMBO_FORMAT_INT);
	obs_property_list_add_int(p, obs_module_text("None"), 0);
	obs_property_list_add_int(p, obs_module_text("Full"), 1);
	obs_property_list_add_int(p, obs_module_text("Half"), 2);
	obs_property_list_add_int(p, obs_module_text("Third"), 3);
	obs_property_list_add_int(p, obs_module_text("Quarter"), 4);

	obs_properties_add_bool(props, "active_clone", obs_module_text("ActiveClone"));

	p = obs_properties_add_text(props, "same_clones", obs_module_text("SameClones"), OBS_TEXT_INFO);
	obs_property_set_visible(p, false);

	return props;
}

void draw_source_switch_source(draw_source_data_t *context, obs_source_t *source)
{
	obs_source_t *prev_source = obs_weak_source_get_source(context->clone);
	if (prev_source) {
		if (obs_source_showing(context->source))
			obs_source_dec_showing(prev_source);
		if (context->active_clone && obs_source_active(context->source))
			obs_source_dec_active(source);
		obs_source_release(prev_source);
	}
	obs_weak_source_release(context->clone);
	context->clone = obs_source_get_weak_source(source);
	if (obs_source_showing(context->source))
		obs_source_inc_showing(source);
	if (context->active_clone && obs_source_active(context->source))
		obs_source_inc_active(source);
}
void draw_source_update(void *data, obs_data_t *settings)
{
	draw_source_data_t *context = data;
	bool active_clone = obs_data_get_bool(settings, "active_clone");
	context->clone_type = obs_data_get_int(settings, "clone_type");
	bool async = true;
	if (context->clone_type == CLONE_SOURCE) {
		const char *source_name = obs_data_get_string(settings, "clone");
		obs_source_t *source = obs_get_source_by_name(source_name);
		if (source == context->source) {
			obs_source_release(source);
			source = NULL;
		}
		if (source) {
			async = (obs_source_get_output_flags(source) & OBS_SOURCE_ASYNC) != 0;
			if (!obs_weak_source_references_source(context->clone, source) ||
			    context->active_clone != active_clone) {
				context->active_clone = active_clone;
				draw_source_switch_source(context, source);
			}
			obs_source_release(source);
		}
	}
	context->active_clone = active_clone;
	context->buffer_frame = (uint8_t)obs_data_get_int(settings, "buffer_frame");
}
void draw_source_save(void *data, obs_data_t *settings)
{
	draw_source_data_t *context = data;
	if (context->clone_type != CLONE_SOURCE) {
		obs_data_set_string(settings, "clone", "");
		return;
	}
	if (!context->clone)
		return;
	obs_source_t *source = obs_weak_source_get_source(context->clone);
	if (!source)
		return;
	obs_data_set_string(settings, "clone", obs_source_get_name(source));
	obs_source_release(source);
}

void draw_source_video_tick(void *data, float seconds)
{
	UNUSED_PARAMETER(seconds);
	draw_source_data_t *context = data;
	context->processed_frame = false;

	if (context->clone_type == CLONE_CURRENT_SCENE) {
		obs_source_t *source = obs_frontend_get_current_scene();
		if (!obs_weak_source_references_source(context->clone, source)) {
			draw_source_switch_source(context, source);
		}
		obs_source_release(source);
	} else if (context->clone_type == CLONE_PREVIOUS_SCENE) {
		obs_source_t *current_scene = obs_frontend_get_current_scene();
		if (!obs_weak_source_references_source(context->current_scene, current_scene)) {
			obs_source_t *source = obs_weak_source_get_source(context->current_scene);
			draw_source_switch_source(context, source);
			obs_source_release(source);

			obs_weak_source_release(context->current_scene);
			context->current_scene = obs_source_get_weak_source(current_scene);
		}
		obs_source_release(current_scene);
	}
	if (context->buffer_frame > 0) {
		uint32_t cx = context->buffer_frame;
		uint32_t cy = context->buffer_frame;
		if (context->clone) {
			obs_source_t *s = obs_weak_source_get_source(context->clone);
			if (s) {
				context->source_cx = obs_source_get_width(s);
				context->source_cy = obs_source_get_height(s);

				cx = context->source_cx;
				cy = context->source_cy;
				obs_source_release(s);
			}
		}
		if (context->buffer_frame > 1) {
			cx /= context->buffer_frame;
			cy /= context->buffer_frame;
		}
		if (cx != context->cx || cy != context->cy) {
			context->cx = cx;
			context->cy = cy;
			obs_enter_graphics();
			gs_texrender_destroy(context->render);
			context->render = NULL;
			obs_leave_graphics();
		}
	}
}
struct obs_source_info draw_source = {.id = "draw_source",
				      .type = OBS_SOURCE_TYPE_INPUT,
				      .output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_CUSTOM_DRAW,
				      .get_name = draw_source_get_name,
				      .create = draw_source_create,
				      .destroy = draw_source_destroy,
				      .load = draw_source_update,
				      .update = draw_source_update,
				      .save = draw_source_save,
				      .get_width = draw_source_get_width,
				      .get_height = draw_source_get_height,
				      .video_tick = draw_source_video_tick,
				      .get_defaults = draw_source_get_defaults,
				      .video_render = draw_source_video_render,
				      .get_properties = draw_source_get_properties,
				      .icon_type = OBS_ICON_TYPE_COLOR};