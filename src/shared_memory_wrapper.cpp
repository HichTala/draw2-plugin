//
// Created by HichTala on 23/07/25.
//

#define MAX_FRAME_WIDTH 3840
#define MAX_FRAME_HEIGHT 2160
#define BYTES_PER_PIXEL 4

#include "shared_memory_wrapper.h"

#define OBS_SHM_NAME "/obs_shared_memory"
#define PYTHON_SHM_NAME "/python_shared_memory"
#ifdef _WIN32
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/windows_shared_memory.hpp>
#else
// POSIX path (macOS / Linux). The Python backend uses
// multiprocessing.shared_memory, which maps to POSIX shm_open with a
// leading-slash name (e.g. "/obs_shared_memory"). We MUST use the exact same
// mechanism and name here; boost::interprocess on macOS falls back to a
// file in /tmp, which the backend never finds.
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#endif

#include <string>

struct shared_frame_header {
	uint32_t width;
	uint32_t height;
};
typedef struct shared_frame_header shared_frame_header_t;

// Per-channel shared-memory names so multiple detectors can coexist. Must match
// what the Python backend computes (multiprocessing.shared_memory prepends "/").
static std::string obs_shm_name(draw_source_data_t *context)
{
	int ch = context->channel ? context->channel : 1;
	return std::string(OBS_SHM_NAME) + std::string("_") + std::to_string(ch);
}

static std::string py_shm_name(draw_source_data_t *context)
{
	int ch = context->channel ? context->channel : 1;
	return std::string(PYTHON_SHM_NAME) + std::string("_") + std::to_string(ch);
}

extern "C" void write_message_to_shared_memory(draw_source_data_t *context, uint8_t *frame, uint32_t linesize,
					       uint32_t width, uint32_t height)
{
	if (!context->shared_frame)
		return;

	auto *header = static_cast<shared_frame_header_t *>(context->shared_frame);

	header->width = width;
	header->height = height;

	uint8_t *frame_data = static_cast<uint8_t *>(context->shared_frame) + sizeof(shared_frame_header_t);

	for (uint32_t y = 0; y < height; y++) {
		memcpy(frame_data + size_t(y) * width * 4, frame + size_t(y) * linesize, size_t(width) * 4);
	}
}

extern "C" void init_shared_memory(draw_source_data_t *context)
{
	if (context->source_width == 0 || context->source_height == 0) {
		blog(LOG_WARNING, "Shared memory not initialized: invalid frame size");
		return;
	}

	size_t pixel_bytes = MAX_FRAME_WIDTH * MAX_FRAME_HEIGHT * BYTES_PER_PIXEL;
	size_t required_size = sizeof(shared_frame_header_t) + pixel_bytes;

#ifdef _WIN32
	using namespace boost::interprocess;

	if (context->region) {
		delete static_cast<mapped_region *>(context->region);
		context->region = nullptr;
		context->shared_frame = nullptr;
	}

	std::string oname = obs_shm_name(context);
	try {
		windows_shared_memory shm(create_only, oname.c_str(), read_write, required_size);
		auto *region = new mapped_region(shm, read_write);
		context->region = region;
		context->shared_frame = region->get_address();
		memset(context->shared_frame, 0, sizeof(shared_frame_header_t));
	} catch (const interprocess_exception &e) {
		UNUSED_PARAMETER(e);
		windows_shared_memory shm(open_only, oname.c_str(), read_write);
		auto *region = new mapped_region(shm, read_write);
		context->region = region;
		context->shared_frame = region->get_address();
		memset(context->shared_frame, 0, sizeof(shared_frame_header_t));
	}
#else
	if (context->shared_frame) {
		munmap(context->shared_frame, context->shared_frame_size);
		context->shared_frame = nullptr;
		context->shared_frame_size = 0;
	}

	// Start from a fresh segment: on macOS a POSIX shm object can only be
	// ftruncate()d once, so unlink any stale one first.
	std::string oname = obs_shm_name(context);
	shm_unlink(oname.c_str());

	int fd = shm_open(oname.c_str(), O_CREAT | O_RDWR, 0666);
	if (fd < 0) {
		blog(LOG_ERROR, "Failed to shm_open %s: %s", oname.c_str(), strerror(errno));
		return;
	}
	if (ftruncate(fd, static_cast<off_t>(required_size)) != 0) {
		blog(LOG_ERROR, "Failed to size shared memory: %s", strerror(errno));
		close(fd);
		shm_unlink(oname.c_str());
		return;
	}
	void *addr = mmap(nullptr, required_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
	if (addr == MAP_FAILED) {
		blog(LOG_ERROR, "Failed to mmap shared memory: %s", strerror(errno));
		return;
	}
	context->region = nullptr;
	context->shared_frame = addr;
	context->shared_frame_size = required_size;
	memset(context->shared_frame, 0, sizeof(shared_frame_header_t));
#endif
}

extern "C" void destroy_shared_memory(draw_source_data_t *context)
{
#ifdef _WIN32
	using namespace boost::interprocess;
	delete static_cast<mapped_region *>(context->region);
#else
	if (context->shared_frame) {
		munmap(context->shared_frame, context->shared_frame_size);
	}
	{
		std::string oname = obs_shm_name(context);
		std::string pname = py_shm_name(context);
		shm_unlink(oname.c_str());
		shm_unlink(pname.c_str());
	}
	context->shared_frame_size = 0;
#endif
	context->region = nullptr;
	context->shared_frame = nullptr;
}

extern "C" bool read_shared_memory(draw_source_data_t *context)
{
	std::string pname = py_shm_name(context);
#ifdef _WIN32
	using namespace boost::interprocess;

	try {
		windows_shared_memory shm(open_only, pname.c_str(), read_only);
		mapped_region region(shm, read_only);

		auto *python_header = static_cast<shared_frame_header_t *>(region.get_address());

		if (python_header->width == 0 || python_header->height == 0) {
			blog(LOG_ERROR, "Size of the image in the header are null");
			context->processing = false;
			return false;
		}

		if (!context->display_texture || context->display_width != python_header->width ||
		    context->display_height != python_header->height) {
			context->display_width = python_header->width;
			context->display_height = python_header->height;
			if (context->display_texture) {
				gs_texture_destroy(context->display_texture);
			}
			context->display_texture = gs_texture_create(context->display_width, context->display_height,
								     GS_RGBA, 1, nullptr, GS_DYNAMIC);
		}

		uint8_t *image_data = static_cast<uint8_t *>(region.get_address()) + sizeof(shared_frame_header_t);
		gs_texture_set_image(context->display_texture, image_data, context->display_width * 4, false);
	} catch (const interprocess_exception &) {
		context->processing = false;
		return false;
	}

	return true;
#else
	int fd = shm_open(pname.c_str(), O_RDONLY, 0666);
	if (fd < 0) {
		context->processing = false;
		return false;
	}

	struct stat st;
	if (fstat(fd, &st) != 0 || static_cast<size_t>(st.st_size) < sizeof(shared_frame_header_t)) {
		close(fd);
		context->processing = false;
		return false;
	}
	size_t map_size = static_cast<size_t>(st.st_size);

	void *addr = mmap(nullptr, map_size, PROT_READ, MAP_SHARED, fd, 0);
	close(fd);
	if (addr == MAP_FAILED) {
		context->processing = false;
		return false;
	}

	auto *python_header = static_cast<shared_frame_header_t *>(addr);
	uint32_t width = python_header->width;
	uint32_t height = python_header->height;

	if (width == 0 || height == 0 || sizeof(shared_frame_header_t) + size_t(width) * height * 4 > map_size) {
		munmap(addr, map_size);
		context->processing = false;
		return false;
	}

	if (!context->display_texture || context->display_width != width || context->display_height != height) {
		context->display_width = width;
		context->display_height = height;
		if (context->display_texture) {
			gs_texture_destroy(context->display_texture);
		}
		context->display_texture = gs_texture_create(context->display_width, context->display_height, GS_RGBA,
							     1, nullptr, GS_DYNAMIC);
	}

	uint8_t *image_data = static_cast<uint8_t *>(addr) + sizeof(shared_frame_header_t);
	gs_texture_set_image(context->display_texture, image_data, context->display_width * 4, false);

	munmap(addr, map_size);
	return true;
#endif
}

extern "C" void ensure_shared_memory_exists(draw_source_data_t *context, uint32_t width, uint32_t height)
{
	if (width != context->source_width || height != context->source_height) {
		context->source_width = width;
		context->source_height = height;

		destroy_shared_memory(context);
		init_shared_memory(context);
		return;
	}

#ifdef _WIN32
	using namespace boost::interprocess;
	try {
		std::string oname = obs_shm_name(context);
		shared_memory_object shm(open_only, oname.c_str(), read_only);
	} catch (const interprocess_exception &ex) {
		(void)ex;
		init_shared_memory(context);
	}
#else
	// Recreate the segment if it is missing. The Python backend's
	// multiprocessing resource_tracker unlinks shared memory it merely
	// *opened* when it exits, which can delete the segment we own out from
	// under us; detect that (the name no longer resolves) and re-create it.
	bool exists = false;
	if (context->shared_frame) {
		std::string oname = obs_shm_name(context);
		int fd = shm_open(oname.c_str(), O_RDONLY, 0666);
		if (fd >= 0) {
			exists = true;
			close(fd);
		}
	}
	if (!exists)
		init_shared_memory(context);
#endif
}
