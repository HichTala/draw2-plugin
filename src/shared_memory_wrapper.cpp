//
// Created by HichTala on 23/07/25.
//

// #include <boost/interprocess/shared_memory_object.hpp>
// #include <boost/interprocess/mapped_region.hpp>

#define OBS_SHM_NAME "obs_shared_memory"
#define PYTHON_SHM_NAME "python_shared_memory"

#include "shared_memory_wrapper.h"

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

struct shared_frame_header {
	uint32_t width;
	uint32_t height;
};
typedef struct shared_frame_header shared_frame_header_t;
extern "C" void write_message_to_shared_memory(draw_source_data_t *context, uint8_t *frame, uint32_t linesize,
					       uint32_t width, uint32_t height)
{
	auto *header = static_cast<shared_frame_header_t *>(context->shared_frame);
	header->width = width;
	header->height = height;

	uint8_t *frame_data = static_cast<uint8_t *>(context->shared_frame) + sizeof(shared_frame_header_t);
	for (uint32_t y = 0; y < height; y++) {
		memcpy(frame_data + y * width * 4, frame + y * linesize, width * 4);
	}
}

extern "C" void init_shared_memory(draw_source_data_t *context)
{
	using namespace boost::interprocess;
	size_t required_size =
		sizeof(shared_frame_header_t) + (size_t)context->source_width * context->source_height * 4;

	if (context->shared_frame) {
		shared_memory_object::remove(OBS_SHM_NAME);
		context->shared_frame = nullptr;
		context->region = nullptr;
	}

	shared_memory_object shm(open_or_create, OBS_SHM_NAME, read_write);
	shm.truncate(static_cast<offset_t>(required_size));

	context->region = static_cast<void *>(new mapped_region(shm, read_write));
	context->shared_frame = static_cast<mapped_region *>(context->region)->get_address();
}

extern "C" void destroy_shared_memory(draw_source_data_t *context)
{
	using namespace boost::interprocess;
	shared_memory_object::remove(OBS_SHM_NAME);
	shared_memory_object::remove(PYTHON_SHM_NAME);
	context->region = nullptr;
	context->shared_frame = nullptr;
}

extern "C" bool read_shared_memory(draw_source_data_t *context)
{
	using namespace boost::interprocess;

	try {
		shared_memory_object shm(open_only, PYTHON_SHM_NAME, read_only);
		mapped_region region(shm, read_only);

		auto *python_header = static_cast<shared_frame_header_t *>(region.get_address());

		context->display_width = python_header->width;
		context->display_height = python_header->height;

		uint8_t *image_data = static_cast<uint8_t *>(region.get_address()) + sizeof(shared_frame_header_t);
		if (context->display_texture)
			gs_texture_destroy(context->display_texture);
		context->display_texture = gs_texture_create(context->display_width, context->display_height, GS_RGBA,
							     1, nullptr, GS_DYNAMIC);
		gs_texture_set_image(context->display_texture, image_data, context->display_width * 4, false);

	} catch (const interprocess_exception &ex) {
		(void)ex;
		context->processing = false;
		return false;
	}

	return true;
}

extern "C" void ensure_shared_memory_exists(draw_source_data_t *context)
{
	using namespace boost::interprocess;
	try {
		shared_memory_object shm(open_only, OBS_SHM_NAME, read_only);
	} catch (const interprocess_exception &ex) {
		(void)ex;
		init_shared_memory(context);
	}
}
