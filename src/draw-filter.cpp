//
// Created by HichTala on 23/06/25.
//

#include "draw-filter.hpp"


struct obs_source_info draw_filter = {
	.id = "draw_filter",
	.type = OBS_SOURCE_TYPE_FILTER,
	.output_flags = OBS_SOURCE_VIDEO,
	.get_name= draw_filter_get_name,
	.get_width = draw_filter_get_width,
	.get_height = draw_filter_get_height,
};

const char* draw_filter_get_name(void *type_data)
{
	return obs_module_text("Draw Filter");
}

void* draw_filter_create(obs_data_t *settings, obs_source_t *source)
{
	return nullptr;
}
void draw_filter_destroy(void *data)
{

}

uint32_t draw_filter_get_width(void *data)
{
	return 0;
}
uint32_t draw_filter_get_height(void *data)
{
	return 0;
}
void draw_filter_get_defaults(obs_data_t *settings){

}