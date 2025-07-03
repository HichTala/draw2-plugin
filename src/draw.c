//
// Created by HichTala on 24/06/25.
//

#include "draw.h"

#include "plugin-support.h"
#include <graphics/graphics.h>

struct obs_source_info draw_filter = {.id = "draw_filter",
				      .type = OBS_SOURCE_TYPE_FILTER,
				      .output_flags = OBS_SOURCE_VIDEO,
				      .get_name = draw_filter_get_name,
				      .create = draw_filter_create,
				      .destroy = draw_filter_destroy,
				      .get_defaults = draw_filter_get_defaults};

struct obs_source_info draw_source = {.id = "draw_source",
				      .type = OBS_SOURCE_TYPE_INPUT,
				      .output_flags = OBS_SOURCE_VIDEO,
				      .get_name = draw_source_get_name,
				      .create = draw_source_create,
				      .destroy = draw_source_destroy,
				      .get_width = draw_source_get_width,
				      .get_height = draw_source_get_height,
				      .get_defaults = draw_filter_get_defaults,
				      .video_render = draw_source_video_render,
				      .get_properties = draw_source_get_properties,
				      .update = draw_source_update,
				      .icon_type = OBS_ICON_TYPE_COLOR};

const char *draw_filter_get_name(void *type_data)
{
	UNUSED_PARAMETER(type_data);
	return obs_module_text("Draw Filter");
}
const char *draw_source_get_name(void *type_data)
{
	UNUSED_PARAMETER(type_data);
	return obs_module_text("Draw Display");
}

void *draw_filter_create(obs_data_t *settings, obs_source_t *source)
{
	UNUSED_PARAMETER(settings);
	draw_filter_data_t *filter = bzalloc(sizeof(draw_filter_data_t));

	filter->source = source;

	return filter;
}
void *draw_source_create(obs_data_t *settings, obs_source_t *source)
{
	UNUSED_PARAMETER(settings);
	draw_source_data_t *source_data = bzalloc(sizeof(draw_source_data_t));

	source_data->source = source;
	source_data->height = 391;
	source_data->width = 268;

	source_data->input_type = NULL;
	source_data->input_selection = NULL;

	source_data->minimum_screen_time = 0;
	source_data->minimum_out_of_screen_time = 0;
	source_data->threshold_confidence_score = 0;

	return source_data;
}
void draw_filter_destroy(void *data)
{
	draw_filter_data_t *filter = data;

	bfree(filter);
}
void draw_source_destroy(void *data)
{
	draw_source_data_t *source_data = data;

	if (source_data->input_type)
		bfree(source_data->input_type);

	if (source_data->input_selection)
		bfree(source_data->input_selection);

	bfree(source_data);
}

uint32_t draw_source_get_height(void *data)
{
	draw_source_data_t *source_data = data;
	return source_data->height;
}
uint32_t draw_source_get_width(void *data)
{
	draw_source_data_t *source_data = data;
	return source_data->width;
}

void draw_filter_get_defaults(obs_data_t *settings)
{
	UNUSED_PARAMETER(settings);
}

void draw_source_video_render(void *data, gs_effect_t *effect)
{
	UNUSED_PARAMETER(effect);
	draw_source_data_t *source_data = data;
	if (!source_data)
		return;

	if (!source_data->input_selection)
		return;

	obs_source_t *selected_source = obs_get_source_by_name(source_data->input_selection);
	if (!selected_source) {
		return;
	}

	gs_texture_t *captured_frame = capture_source_frame(selected_source);
	source_data->height = obs_source_get_height(selected_source);
	source_data->width = obs_source_get_width(selected_source);
	obs_source_release(selected_source);

	if (!captured_frame)
		return;

	gs_effect_t *default_effect = obs_get_base_effect(OBS_EFFECT_DEFAULT);
	if (!default_effect)
		return;

	gs_effect_set_texture(gs_effect_get_param_by_name(default_effect, "image"), captured_frame);

	gs_technique_t *tech = gs_effect_get_technique(default_effect, "Draw");
	if (!tech)
		return;

	size_t passes = gs_technique_begin(tech);
	for (size_t i = 0; i < passes; i++) {
		if (gs_technique_begin_pass(tech, i)) {
			gs_draw_sprite(captured_frame, 0, source_data->width, source_data->height);
			gs_technique_end_pass(tech);
		}
	}
	gs_technique_end(tech);
	// gs_texture_destroy(captured_frame);
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
	obs_property_t *p = data;
	const char *name = obs_source_get_name(source);
	size_t count = obs_property_list_item_count(p);
	size_t idx = 0;
	while (idx < count && strcmp(name, obs_property_list_item_string(p, idx)) > 0)
		idx++;

	uint32_t flags = obs_source_get_output_flags(source);
	const char *source_id = obs_source_get_id(source);

	if (flags & OBS_SOURCE_VIDEO & (strcmp(source_id, "draw_source") != 0)) {
		obs_property_list_insert_string(p, idx, name, name);
	}
	return true;
}
static bool draw_source_type_changed(obs_properties_t *props, obs_property_t *list, obs_data_t *settings)
{
	UNUSED_PARAMETER(list);

	const char *selected_type = obs_data_get_string(settings, "input_type");

	obs_property_t *input_selection = obs_properties_get(props, "input_selection");

	obs_property_list_clear(input_selection);

	if (strcmp(selected_type, "source") == 0) {
		obs_enum_sources(add_source_to_list, input_selection);
	} else if (strcmp(selected_type, "scene") == 0) {
		obs_enum_scenes(add_source_to_list, input_selection);
	}

	return true; // Refresh UI
}
obs_properties_t *draw_source_get_properties(void *data)
{
	draw_source_data_t *source_data = data;
	obs_properties_t *source_properties = obs_properties_create();
	obs_properties_set_param(source_properties, source_data, NULL);

	obs_property_t *input_type = obs_properties_add_list(source_properties, "input_type", "Input Type",
							     OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_STRING);
	obs_property_list_add_string(input_type, "Scene", "scene");
	obs_property_list_add_string(input_type, "Source", "source");

	obs_properties_add_list(source_properties, "input_selection", "Input Selection", OBS_COMBO_TYPE_LIST,
				OBS_COMBO_FORMAT_STRING);
	obs_property_set_modified_callback(input_type, draw_source_type_changed);

	obs_properties_add_int(source_properties, "minimum_screen_time",
			       obs_module_text("Minimum Card Display Time (in sec)"), 1, 4096, 1);
	obs_properties_add_int(source_properties, "minimum_out_of_screen_time",
			       obs_module_text("Minimum Card Out Of Screen Time (in sec)"), 1, 4096, 1);
	obs_properties_add_int(source_properties, "threshold_confidence_score",
			       obs_module_text("Threshold for confidence score (in %)"), 1, 4096, 1);

	return source_properties;
}
void draw_source_update(void *data, obs_data_t *settings)
{
	draw_source_data_t *source_data = data;
	const char *input_type = obs_data_get_string(settings, "input_type");
	const char *input_selection = obs_data_get_string(settings, "input_selection");

	if (source_data->input_type)
		bfree(source_data->input_type);
	if (source_data->input_selection)
		bfree(source_data->input_selection);

	source_data->input_type = input_type ? bstrdup(input_type) : NULL;
	source_data->input_selection = input_selection ? bstrdup(input_selection) : NULL;

	source_data->minimum_screen_time = (uint32_t)obs_data_get_int(settings, "minimum_screen_time");
	source_data->minimum_out_of_screen_time = (uint32_t)obs_data_get_int(settings, "minimum_out_of_screen_time");
	source_data->threshold_confidence_score = (uint32_t)obs_data_get_int(settings, "threshold_confidence_score");
}
gs_texture_t *capture_source_frame(obs_source_t *source)
{
	if (!source)
		return NULL;

	const char *source_id = obs_source_get_id(source);

	if (!source_id || strcmp(source_id, "draw_source") == 0)
		return NULL;

	uint32_t width = obs_source_get_width(source);
	uint32_t height = obs_source_get_height(source);

	if (width == 0 || height == 0)
		return NULL;

	gs_texrender_t *texrender = gs_texrender_create(GS_RGBA, GS_ZS_NONE);
	if (!texrender)
		return NULL;

	gs_texrender_reset(texrender);

	gs_texture_t *texture = NULL;

	if (gs_texrender_begin(texrender, width, height)) {
		struct vec4 background;
		vec4_zero(&background);

		gs_clear(GS_CLEAR_COLOR, &background, 0.0f, 0);
		gs_ortho(0.0f, (float)width, 0.0f, (float)height, -100.0f, 100.0f);
		obs_source_video_render(source);

		gs_texrender_end(texrender);

		gs_texture_t *src_tex = gs_texrender_get_texture(texrender);
		if (src_tex) {
			texture = gs_texture_create(width, height, GS_RGBA, 1, NULL, GS_DYNAMIC);
			if (texture) {
				gs_copy_texture(texture, src_tex);
			}
		}
	}

	gs_texrender_destroy(texrender);
	return texture;
}