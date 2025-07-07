//
// Created by HichTala on 24/06/25.
//

#ifndef DRAW_H
#define DRAW_H

#include <obs-module.h>
#include <obs-source.h>
#include <graphics/graphics.h>

enum clone_type {
	CLONE_SOURCE,
	CLONE_CURRENT_SCENE,
	CLONE_PREVIOUS_SCENE,
};

struct draw_source_data {
	obs_source_t *source;
	enum clone_type clone_type;
	obs_weak_source_t *clone;
	obs_weak_source_t *current_scene;

	gs_texrender_t *render;
	bool processed_frame;
	uint8_t buffer_frame;
	uint32_t cx;
	uint32_t cy;
	uint32_t source_cx;
	uint32_t source_cy;
	enum gs_color_space space;
	bool rendering;
	bool active_clone;
};
typedef struct draw_source_data draw_source_data_t;

extern struct obs_source_info draw_source;


#endif //DRAW_H
