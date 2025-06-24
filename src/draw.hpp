//
// Created by HichTala on 23/06/25.
//

#ifndef DRAW_FILTER_HPP
#define DRAW_FILTER_HPP

#include <obs-module.h>
#include <obs-source.h>
#include <graphics/graphics.h>
#include <graphics/effect.h>

struct draw_filter_data {
	obs_source_t *source;
};
typedef struct draw_filter_data draw_filter_data_t;

struct draw_source_data {
	obs_source_t *source;

	uint32_t width;
	uint32_t height;
};
typedef struct draw_source_data draw_source_data_t;

static const char *draw_filter_get_name(void *type_data);
static const char *draw_source_get_name(void *type_data);
static void *draw_filter_create(obs_data_t *settings, obs_source_t *source);
static void *draw_source_create(obs_data_t *settings, obs_source_t *source);
static void draw_filter_destroy(void *data);
uint32_t draw_source_get_height(void *data);
uint32_t draw_source_get_width(void *data);
void draw_filter_get_defaults(obs_data_t *settings);
void draw_source_video_render(void *data, gs_effect_t *effect);

#endif //DRAW_FILTER_HPP
