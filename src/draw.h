//
// Created by HichTala on 24/06/25.
//

#ifndef DRAW_H
#define DRAW_H

#include <obs-module.h>

enum input_type {
	INPUT_TYPE_SOURCE,
	INPUT_TYPE_SCENE
};

struct draw_source_data {
	enum input_type input_type;
	obs_weak_source_t *source;

	gs_texrender_t *render;
	bool rendering;
};
typedef struct draw_source_data draw_source_data_t;

extern struct obs_source_info draw_source;


#endif //DRAW_H
