//
// Created by HichTala on 24/06/25.
//

#define DEFAULT_DISPLAY_CARD_HEIGHT 1195
#define DEFAULT_DISPLAY_CARD_WIDTH 813

#include "draw.h"
#include "shared_memory_wrapper.h"
#include "feature_flags.h"

const char *draw_source_get_name(void *type_data)
{
	UNUSED_PARAMETER(type_data);
	return obs_module_text("draw_display");
}

void *draw_source_create(obs_data_t *settings, obs_source_t *source)
{
	UNUSED_PARAMETER(settings);
	draw_source_data_t *context = bzalloc(sizeof(draw_source_data_t));
	context->shared_frame = NULL;
	context->display_texture = NULL;
	context->render = NULL;
	obs_source_update(source, NULL);
	return context;
}

void draw_source_destroy(void *data)
{
	draw_source_data_t *context = data;
	if (context->source) {
		obs_source_t *source = obs_weak_source_get_source(context->source);
		if (source) {
			obs_source_release(source);
		}
		obs_weak_source_release(context->source);
	}

	obs_enter_graphics();
	if (context->render) {
		gs_texrender_destroy(context->render);
	}
	if (context->stage) {
		gs_stagesurface_destroy(context->stage);
	}
	if (context->display_texture)
		gs_texture_destroy(context->display_texture);
	obs_leave_graphics();
	if (context->shared_frame)
		destroy_shared_memory(context);

	bfree(context);
}

uint32_t draw_source_get_height(void *data)
{
	draw_source_data_t *context = data;
	if (!context->display_height)
		return DEFAULT_DISPLAY_CARD_HEIGHT;
	return context->display_height;
}
uint32_t draw_source_get_width(void *data)
{
	draw_source_data_t *context = data;
	if (!context->display_width)
		return DEFAULT_DISPLAY_CARD_WIDTH;
	return context->display_width;
}

void draw_source_get_defaults(obs_data_t *settings)
{
	obs_data_set_default_int(settings, "crop_left", 0);
	obs_data_set_default_int(settings, "crop_top", 0);
	obs_data_set_default_int(settings, "crop_right", 0);
	obs_data_set_default_int(settings, "crop_bottom", 0);
	obs_data_set_default_bool(settings, "rotate_180", false);
	obs_data_set_default_bool(settings, "preview_input", false);
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

	uint32_t src_width = obs_source_get_width(source);
	uint32_t src_height = obs_source_get_height(source);

	if (src_width == 0 || src_height == 0) {
		obs_source_release(source);
		context->processing = false;
		return;
	}

	// Apply the user-defined crop (pixels removed from each edge). Ignore the
	// crop on an axis if it would leave nothing to capture.
	uint32_t crop_l = context->crop_left, crop_t = context->crop_top;
	uint32_t crop_r = context->crop_right, crop_b = context->crop_bottom;
	if (crop_l + crop_r >= src_width) {
		crop_l = 0;
		crop_r = 0;
	}
	if (crop_t + crop_b >= src_height) {
		crop_t = 0;
		crop_b = 0;
	}
	uint32_t width = src_width - crop_l - crop_r;
	uint32_t height = src_height - crop_t - crop_b;

	if (!context->render || width != context->source_width || height != context->source_height) {
		obs_enter_graphics();
		if (context->render)
			gs_texrender_destroy(context->render);
		context->render = gs_texrender_create(GS_RGBA, GS_ZS_NONE);
		obs_leave_graphics();
	}

	gs_texrender_reset(context->render);
	if (!gs_texrender_begin(context->render, width, height)) {
		blog(LOG_INFO, "Failed to begin texture rendering");
		obs_source_release(source);
		context->processing = false;
		return;
	}

	ensure_shared_memory_exists(context, width, height);

	struct vec4 clear_color;
	vec4_set(&clear_color, 0.0f, 0.0f, 0.0f, 1.0f); // black
	gs_clear(GS_CLEAR_COLOR, &clear_color, 0.0f, 0);
	// Map the cropped region into the render target. For a 180° rotation we
	// simply reverse both axes of the orthographic projection.
	if (context->rotate_180) {
		gs_ortho((float)(crop_l + width), (float)crop_l, (float)(crop_t + height), (float)crop_t, -100.0f,
			 100.0f);
	} else {
		gs_ortho((float)crop_l, (float)(crop_l + width), (float)crop_t, (float)(crop_t + height), -100.0f,
			 100.0f);
	}

	obs_source_video_render(source);
	gs_texrender_end(context->render);

	gs_texture_t *texture = gs_texrender_get_texture(context->render);
	if (texture) {
		if (!context->stage || width != context->stage_width || height != context->stage_height) {
			obs_enter_graphics();
			if (context->stage)
				gs_stagesurface_destroy(context->stage);
			context->stage = gs_stagesurface_create(width, height, GS_RGBA);
			context->stage_width = width;
			context->stage_height = height;
			obs_leave_graphics();
		}

		if (context->stage) {
			gs_stage_texture(context->stage, texture);
			uint8_t *frame = NULL;
			uint32_t linesize = 0;

			if (gs_stagesurface_map(context->stage, &frame, &linesize)) {
				if (context->shared_frame) {
					write_message_to_shared_memory(context, frame, linesize, width, height);
				}
				gs_stagesurface_unmap(context->stage);
			}

		} else {
			blog(LOG_ERROR, "Failed to capture stage surface");
		}
	}

	obs_source_release(source);

	// Tuning aid: render the cropped/rotated input this source feeds the
	// detector (instead of the detected-card overlay), so the crop can be
	// dialed in from this source's own preview. Detection still runs above.
	if (context->preview_input) {
		context->display_width = width;
		context->display_height = height;
		if (texture) {
			gs_effect_t *preview_effect = obs_get_base_effect(OBS_EFFECT_DEFAULT);
			gs_eparam_t *preview_image = gs_effect_get_param_by_name(preview_effect, "image");
			gs_effect_set_texture(preview_image, texture);
			while (gs_effect_loop(preview_effect, "Draw"))
				gs_draw_sprite(texture, 0, width, height);
		}
		context->processing = false;
		return;
	}

	if (!read_shared_memory(context)) {
		context->processing = false;
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

	if ((flags & OBS_SOURCE_VIDEO) && (strcmp(source_id, "draw_source") != 0)) {
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

	// Crop (pixels removed from each edge) to focus detection on a region.
	// Opt-in: hidden unless enabled in settings.
	if (draw_feature_enabled(FEATURE_CROP)) {
		obs_properties_add_int(props, "crop_left", obs_module_text("crop_left"), 0, 7680, 1);
		obs_properties_add_int(props, "crop_top", obs_module_text("crop_top"), 0, 4320, 1);
		obs_properties_add_int(props, "crop_right", obs_module_text("crop_right"), 0, 7680, 1);
		obs_properties_add_int(props, "crop_bottom", obs_module_text("crop_bottom"), 0, 4320, 1);
	}

	// Rotate input 180°. Opt-in: hidden unless enabled in settings.
	if (draw_feature_enabled(FEATURE_ROTATE))
		obs_properties_add_bool(props, "rotate_180", obs_module_text("rotate_180"));

	// Tuning aid: preview the cropped/rotated input in this source. Shown only
	// when crop or rotation is enabled (it's pointless otherwise).
	if (draw_feature_enabled(FEATURE_CROP) || draw_feature_enabled(FEATURE_ROTATE))
		obs_properties_add_bool(props, "preview_input", obs_module_text("preview_input"));

	return props;
}

void switch_source(draw_source_data_t *context, obs_source_t *source)
{
	if (context->source) {
		obs_source_t *prev_source = obs_weak_source_get_source(context->source);
		if (prev_source) {
			obs_source_release(prev_source);
		}
		obs_weak_source_release(context->source);
	}
	context->source = obs_source_get_weak_source(source);
}
void draw_source_update(void *data, obs_data_t *settings)
{
	draw_source_data_t *context = data;
	context->input_type = obs_data_get_int(settings, "input_type");

	// Optional features force a neutral value when disabled, so turning a
	// feature off in settings reverts behaviour even if the source still has
	// stored values from when it was enabled.
	if (draw_feature_enabled(FEATURE_CROP)) {
		context->crop_left = (uint32_t)obs_data_get_int(settings, "crop_left");
		context->crop_top = (uint32_t)obs_data_get_int(settings, "crop_top");
		context->crop_right = (uint32_t)obs_data_get_int(settings, "crop_right");
		context->crop_bottom = (uint32_t)obs_data_get_int(settings, "crop_bottom");
	} else {
		context->crop_left = context->crop_top = context->crop_right = context->crop_bottom = 0;
	}
	context->rotate_180 = draw_feature_enabled(FEATURE_ROTATE) && obs_data_get_bool(settings, "rotate_180");
	context->preview_input = (draw_feature_enabled(FEATURE_CROP) || draw_feature_enabled(FEATURE_ROTATE)) &&
				 obs_data_get_bool(settings, "preview_input");
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

	// Debug mode: log the resolved capture config to the OBS log (Help > Log
	// Files), so framing/crop/rotation issues are easy to spot.
	if (draw_feature_enabled(FEATURE_DEBUG)) {
		blog(LOG_INFO,
		     "[draw2] debug: input='%s' source=%ux%u crop(l=%u t=%u r=%u b=%u) rotate_180=%d channel=%d",
		     source_name ? source_name : "", context->source_width, context->source_height, context->crop_left,
		     context->crop_top, context->crop_right, context->crop_bottom, context->rotate_180,
		     context->channel);
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
				      .icon_type = OBS_ICON_TYPE_IMAGE};
