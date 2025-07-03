//
// Created by HichTala on 24/06/25.
//

#ifndef DRAW_H
#define DRAW_H

#include <obs-module.h>
#include <obs-source.h>
#include <graphics/graphics.h>

struct draw_filter_data {
	obs_source_t *source;
};
typedef struct draw_filter_data draw_filter_data_t;

struct draw_source_data {
	obs_source_t *source;

	uint32_t width;
	uint32_t height;

	char *input_type;
	char *input_selection;

	uint32_t minimum_screen_time;
	uint32_t minimum_out_of_screen_time;
	uint32_t threshold_confidence_score;

	gs_texture_t *output_texture;
};
typedef struct draw_source_data draw_source_data_t;

extern struct obs_source_info draw_filter;
extern struct obs_source_info draw_source;
static const char *draw_filter_get_name(void *type_data);

const char *draw_source_get_name(void *type_data);
void *draw_filter_create(obs_data_t *settings, obs_source_t *source);
void *draw_source_create(obs_data_t *settings, obs_source_t *source);
void draw_filter_destroy(void *data);
void draw_source_destroy(void *data);
uint32_t draw_source_get_height(void *data);
uint32_t draw_source_get_width(void *data);
void draw_filter_get_defaults(obs_data_t *settings);
void draw_source_video_render(void *data, gs_effect_t *effect);
bool enum_cb(obs_scene_t *scene, obs_sceneitem_t *item, void *param);
bool scene_contains_source(obs_source_t *source);
bool add_source_to_list(void *data, obs_source_t *source);
static bool draw_source_type_changed(obs_properties_t *props, obs_property_t *list, obs_data_t *settings);
obs_properties_t *draw_source_get_properties(void *data);
void draw_source_update(void *data, obs_data_t *settings);
gs_texture_t* capture_source_frame(obs_source_t *source);

#endif //DRAW_H
