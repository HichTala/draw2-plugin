//
// Created by HichTala on 24/06/25.
//

#ifndef DRAW_H
#define DRAW_H

#include <obs-module.h>

enum input_type { INPUT_TYPE_SOURCE, INPUT_TYPE_SCENE };

struct draw_source_data {
	enum input_type input_type;

	// Shared-memory pair id this source reads from. Part of the
	// plugin<->backend addressing; defaults to 1 (single detector).
	int channel;

	// Crop applied to the captured input before it is sent to the detector
	// (pixels removed from each edge). Lets the user focus detection on a
	// region of the camera without affecting the source elsewhere.
	uint32_t crop_left;
	uint32_t crop_top;
	uint32_t crop_right;
	uint32_t crop_bottom;

	// Rotate the captured input 180° before sending it to the detector
	// (useful when the camera is mounted upside down). Detector-only.
	bool rotate_180;

	// Tuning aid: when true, this source renders the cropped/rotated input it
	// feeds the detector (instead of the detected-card overlay), so the crop
	// can be dialed in from the source's own preview. Detection keeps running.
	bool preview_input;

	obs_weak_source_t *source;

	void *region;
	void *shared_frame;
	size_t shared_frame_size;
	uint32_t source_width;
	uint32_t source_height;

	gs_texrender_t *render;
	gs_stagesurf_t *stage;
	uint32_t stage_width;
	uint32_t stage_height;
	gs_texture_t *display_texture;
	uint32_t display_width;
	uint32_t display_height;
	bool processing;
};
typedef struct draw_source_data draw_source_data_t;

extern struct obs_source_info draw_source;

#endif //DRAW_H
