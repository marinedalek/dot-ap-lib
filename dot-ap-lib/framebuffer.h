#ifndef DOTAP_FRAMEBUFFER_H
#define DOTAP_FRAMEBUFFER_H

#include "color.h"

#include <array>
#include <vector>

namespace marinedalek::dotap {
using framebuffer_palette = std::array<dotap::color, 256>;

struct framebuffer {
	framebuffer(int w, int h) : width{ w }, height{ h }, pixels(w*h, 0) {
		palette.fill({ 0,0,0 });
	}
	int const width;
	int const height;
	framebuffer_palette palette;
	std::byte spacer {0};
	std::vector<uint8_t> pixels;
};
}

#endif