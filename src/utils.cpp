//
// Created by HichTala on 01/07/25.
//
#include "utils.hpp"

#include "draw.h"

#include <mutex>
#include <opencv2/core.hpp>

extern "C" {
bool capture_source_frame(void *data, obs_source_t *source)
{
	auto *source_data = static_cast<draw_source_data_t *>(data);

	uint32_t width = source_data->width;
	uint32_t height = source_data->height;

	if (!source) {
		return false;
	}
	if (width == 0 || height == 0) {
		return false;
	}

	gs_texrender_reset(source_data->texrender);
	if (!gs_texrender_begin(source_data->texrender, width, height)) {
		return false;
	}
	struct vec4 background{};
	vec4_zero(&background);
	gs_clear(GS_CLEAR_COLOR, &background, 0.0f, 0);
	gs_ortho(0.0f, static_cast<float>(width), 0.0f, (float)height, -100.0f, 100.0f);
	gs_blend_state_push();
	gs_blend_function(GS_BLEND_ONE, GS_BLEND_ZERO);
	obs_source_video_render(source);
	gs_blend_state_pop();
	gs_texrender_end(source_data->texrender);

	if (source_data->stagesurface) {
		uint32_t stagesurf_width = gs_stagesurface_get_width(source_data->stagesurface);
		uint32_t stagesurf_height = gs_stagesurface_get_height(source_data->stagesurface);
		if (stagesurf_width != width || stagesurf_height != height) {
			gs_stagesurface_destroy(source_data->stagesurface);
			source_data->stagesurface = NULL;
		}
	}
	if (!source_data->stagesurface) {
		source_data->stagesurface = gs_stagesurface_create(width, height, GS_BGRA);
	}
	gs_stage_texture(source_data->stagesurface, gs_texrender_get_texture(source_data->texrender));
	uint8_t *video_data;
	uint32_t linesize;
	if (!gs_stagesurface_map(source_data->stagesurface, &video_data, &linesize)) {
		return false;
	}
	{
		std::lock_guard<std::mutex> lock(source_data->inputBGRALock);
		source_data->inputBGRA = cv::Mat(height, width, CV_8UC4, video_data, linesize);
	}
	gs_stagesurface_unmap(source_data->stagesurface);
	return true;
}
}