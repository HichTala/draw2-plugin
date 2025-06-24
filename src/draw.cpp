//
// Created by HichTala on 23/06/25.
//

#include "draw.hpp"

#include "plugin-support.h"

struct obs_source_info draw_filter = {.id = "draw_filter",
				      .type = OBS_SOURCE_TYPE_FILTER,
				      .output_flags = OBS_SOURCE_VIDEO,
				      .get_name = draw_filter_get_name,
				      .create = draw_filter_create,
				      .destroy = draw_filter_destroy,
				      .get_defaults = draw_filter_get_defaults};

const char *draw_filter_get_name(void *type_data)
{
	UNUSED_PARAMETER(type_data);
	obs_log(LOG_INFO, "draw_filter_get_name");
	return obs_module_text("Draw Filter");
}

void *draw_filter_create(obs_data_t *settings, obs_source_t *source)
{
	UNUSED_PARAMETER(settings);
	auto *filter = static_cast<draw_filter_data_t *>(bzalloc(sizeof(draw_filter_data_t)));

	filter->source = source;
	obs_log(LOG_INFO, "draw_filter_create");

	return filter;
}
void draw_filter_destroy(void *data)
{
	auto *filter = static_cast<draw_filter_data_t *>(data);

	obs_log(LOG_INFO, "draw_filter_destroy");
	bfree(filter);
}

void draw_filter_get_defaults(obs_data_t *settings)
{
	UNUSED_PARAMETER(settings);
	obs_log(LOG_INFO, "draw_filter_get_defaults");
}