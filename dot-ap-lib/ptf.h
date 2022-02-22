#ifndef DOTAP_PTF_H
#define DOTAP_PTF_H

#include "flic.h"
#include "framebuffer.h"
#include "utility.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <istream>

namespace marinedalek::dotap::ptf {
struct file_header {
	uint32_t given_length;
	std::array<std::byte, 4> magic_number;
	uint16_t unknown_a;
	uint16_t frame_count;
	uint16_t video_width;
	uint16_t video_height;
	uint16_t bpp;
	uint16_t ticks_per_frame;
	std::array<std::byte, 8> unknown_b;
	uint32_t unknown_c;
};

enum class frame_type {
	invalid,
	flic,
	bic,
	wave
};

struct logical_frame {
	std::ptrdiff_t video_offset{ -1 };
	std::ptrdiff_t audio_offset{ -1 };
	frame_type video_type;
};

struct frame_header {
	int32_t given_length;
	frame_type type;
	int32_t overall_length();
	int32_t data_length();
	bool has_palette();
};

std::istream& operator>> (std::istream& stream, file_header& header);
std::istream& operator>> (std::istream& stream, frame_header& header);

auto parse_frames(std::istream& stream, int eof_offset)
->std::vector<dotap::ptf::logical_frame>;

void read_palette(framebuffer& buffer, std::istream& stream);

}

#endif