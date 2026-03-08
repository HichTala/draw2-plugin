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
#ifdef _WIN32
#include <boost/interprocess/windows_shared_memory.hpp>
#endif

struct shared_frame_header {
	uint32_t width;
	uint32_t height;
};
typedef struct shared_frame_header shared_frame_header_t;
extern "C" void write_message_to_shared_memory(draw_source_data_t *context, uint8_t *frame, uint32_t linesize,
					       uint32_t width, uint32_t height)
{
	if (!context->shared_frame)
		return;

	auto *header = static_cast<shared_frame_header_t *>(context->shared_frame);

	header->width = width;
	header->height = height;
	blog(LOG_INFO, "writing frame to shared memory, width: %u, height: %u, linesize: %u", width, height, linesize);

	uint8_t *frame_data = static_cast<uint8_t *>(context->shared_frame) + sizeof(shared_frame_header_t);

	for (uint32_t y = 0; y < height; y++) {
		memcpy(frame_data + size_t(y) * width * 4, frame + size_t(y) * linesize, size_t(width) * 4);
	}
}

extern "C" void init_shared_memory(draw_source_data_t *context)
{
	using namespace boost::interprocess;

	if (context->source_width == 0 || context->source_height == 0) {
		blog(LOG_WARNING, "Shared memory not initialized: invalid frame size");
		return;
	}

	size_t pixel_bytes = size_t(context->source_width) * size_t(context->source_height) * 4;
	size_t required_size = sizeof(shared_frame_header_t) + pixel_bytes;

	if (context->region) {
		delete static_cast<mapped_region *>(context->region);
		context->region = nullptr;
		context->shared_frame = nullptr;
	}

	try {
#ifdef _WIN32
		windows_shared_memory shm(create_only, OBS_SHM_NAME, read_write, required_size);
#else
		shared_memory_object shm(open_or_create, OBS_SHM_NAME, read_write);
		shm.truncate(static_cast<offset_t>(required_size));
#endif
		auto *region = new mapped_region(shm, read_write);
		context->region = region;
		context->shared_frame = region->get_address();

		memset(context->shared_frame, 0, sizeof(shared_frame_header_t));
	} catch (const interprocess_exception &e) {
#ifdef _WIN32
		UNUSED_PARAMETER(e);
		windows_shared_memory shm(open_only, OBS_SHM_NAME, read_write);

		auto *region = new mapped_region(shm, read_write);
		context->region = region;
		context->shared_frame = region->get_address();

		memset(context->shared_frame, 0, sizeof(shared_frame_header_t));
#else
		blog(LOG_ERROR, "Failed to open or create shared memory: %s", e.what());
#endif
	}
}

extern "C" void destroy_shared_memory(draw_source_data_t *context)
{
	using namespace boost::interprocess;
#ifdef _WIN32
	shared_memory_object::remove(OBS_SHM_NAME);
	shared_memory_object::remove(PYTHON_SHM_NAME);
#else
	delete static_cast<mapped_region *>(context->region);
#endif
	context->region = nullptr;
	context->shared_frame = nullptr;
}

extern "C" bool read_shared_memory(draw_source_data_t *context)
{
	using namespace boost::interprocess;

	try {
#ifdef _WIN32
		windows_shared_memory shm(open_only, PYTHON_SHM_NAME, read_only);
#else
		shared_memory_object shm(open_only, PYTHON_SHM_NAME, read_only);
#endif

		mapped_region region(shm, read_only);

		auto *python_header = static_cast<shared_frame_header_t *>(region.get_address());

		if (python_header->width == 0 || python_header->height == 0) {
			blog(LOG_ERROR, "Size of the image in the header are null");
			context->processing = false;
			return false;
		}

		context->display_width = python_header->width;
		context->display_height = python_header->height;

		if (context->display_texture)
			gs_texture_destroy(context->display_texture);

		context->display_texture = gs_texture_create(context->display_width, context->display_height, GS_RGBA,
							     1, nullptr, GS_DYNAMIC);

		uint8_t *image_data = static_cast<uint8_t *>(region.get_address()) + sizeof(shared_frame_header_t);

		gs_texture_set_image(context->display_texture, image_data, context->display_width * 4, false);
	} catch (const interprocess_exception &) {
		context->processing = false;
		return false;
	}

	return true;
}

extern "C" void ensure_shared_memory_exists(draw_source_data_t *context, uint32_t width, uint32_t height)
{
	using namespace boost::interprocess;

	if (width != context->source_width || height != context->source_height) {
		context->source_width = width;
		context->source_height = height;
		blog(LOG_INFO, "context source size changed, reinitializing shared memory, new size: %ux%u", width,
		     height);

		destroy_shared_memory(context);
		init_shared_memory(context);
		return;
	}

	try {
		shared_memory_object shm(open_only, OBS_SHM_NAME, read_only);
	} catch (const interprocess_exception &ex) {
		(void)ex;
		init_shared_memory(context);
	}
}
