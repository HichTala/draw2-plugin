//
// Created by HichTala on 24/06/25.
//

#ifndef DRAW_H
#define DRAW_H

#include <obs-module.h>

enum input_type { INPUT_TYPE_SOURCE, INPUT_TYPE_SCENE };

struct draw_source_data {
	enum input_type input_type;

	obs_weak_source_t *source;

	void *region;
	void *shared_frame;
	size_t shared_frame_size;
	uint32_t source_width;
	uint32_t source_height;

	gs_texrender_t *render;
	gs_texture_t *display_texture;
	uint32_t display_width;
	uint32_t display_height;
	bool processing;
};
typedef struct draw_source_data draw_source_data_t;

extern struct obs_source_info draw_source;

#endif //DRAW_H
