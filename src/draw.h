//
// Created by HichTala on 24/06/25.
//

#ifndef DRAW_H
#define DRAW_H

#include <obs-module.h>
#include <obs-source.h>
#include <graphics/graphics.h>

enum input_type {
	INPUT_TYPE_SOURCE,
	INPUT_TYPE_SCENE
};

struct draw_source_data {
	// obs_source_t *source;
	enum input_type input_type;
	obs_weak_source_t *source;
	obs_weak_source_t *current_scene;

	gs_texrender_t *render;
	bool processed_frame;
	uint32_t cx;
	uint32_t cy;
	uint32_t source_cx;
	uint32_t source_cy;
	enum gs_color_space space;
	bool rendering;
};
typedef struct draw_source_data draw_source_data_t;

extern struct obs_source_info draw_source;


#endif //DRAW_H
