//
// Created by HichTala on 24/06/25.
//

#include "draw.h"

#include "plugin-support.h"
#include <graphics/graphics.h>
#include <graphics/effect.h>

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
				      .destroy = draw_filter_destroy,
				      .get_width = draw_source_get_width,
				      .get_height = draw_source_get_height,
				      .get_defaults = draw_filter_get_defaults,
				      // .video_render = draw_source_video_render,
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
	draw_filter_data_t *filter = (draw_filter_data_t *)(bzalloc(sizeof(draw_filter_data_t)));

	filter->source = source;

	return filter;
}
void *draw_source_create(obs_data_t *settings, obs_source_t *source)
{
	UNUSED_PARAMETER(settings);
	draw_source_data_t *source_data = (draw_source_data_t *)(bzalloc(sizeof(draw_source_data_t)));

	source_data->source = source;
	source_data->height = 391;
	source_data->width = 268;

	return source_data;
}
void draw_filter_destroy(void *data)
{
	draw_filter_data_t *filter = (draw_filter_data_t *)(data);

	bfree(filter);
}

uint32_t draw_source_get_height(void *data)
{
	draw_source_data_t *source_data = (draw_source_data_t *)(data);
	return source_data->height;
}
uint32_t draw_source_get_width(void *data)
{
	draw_source_data_t *source_data = (draw_source_data_t *)(data);
	return source_data->width;
}

void draw_filter_get_defaults(obs_data_t *settings)
{
	UNUSED_PARAMETER(settings);
}

void draw_source_video_render(void *data, gs_effect_t *effect)
{
	UNUSED_PARAMETER(data);
	UNUSED_PARAMETER(effect);
}
