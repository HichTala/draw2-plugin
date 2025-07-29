//
// Created by HichTala on 23/07/25.
//

#ifndef SHARED_MEMORY_WRAPPER_H
#define SHARED_MEMORY_WRAPPER_H
#pragma once
#include "draw.h"

#ifdef __cplusplus
extern "C" {
#endif

void write_message_to_shared_memory(draw_source_data_t *context, uint8_t *frame, uint32_t linesize, uint32_t width,
				    uint32_t height);
void init_shared_memory(draw_source_data_t *context);
void destroy_shared_memory(draw_source_data_t *context);
bool read_shared_memory(draw_source_data_t *context);
void ensure_shared_memory_exists(draw_source_data_t *context);

#ifdef __cplusplus
}
#endif
#endif //SHARED_MEMORY_WRAPPER_H
