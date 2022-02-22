#ifndef DOTAP_BIC_H
#define DOTAP_BIC_H

#include "flic.h"
#include "utility.h"
#include <istream>

namespace marinedalek::dotap::bic {

void read_frame(framebuffer& buffer, std::istream& stream, uint32_t size);
void draw_solid_block(uint8_t c, framebuffer& buffer, int x, int y);
void draw_8bpp_pattern_block(uint8_t c1, uint8_t c2, uint16_t pattern, framebuffer& buffer, int x, int y);
void draw_16_byte_block(std::istream& stream, framebuffer& buffer, int x, int y);
void copy_n_solid_blocks(std::istream& stream, int n, framebuffer& buffer, int x, int y);
void copy_n_4bpp_pattern_blocks(std::istream& stream, int n, framebuffer& buffer, int x, int y);
void copy_n_8bpp_pattern_blocks(std::istream& stream, int n, framebuffer& buffer, int x, int y);
void copy_n_16_byte_blocks(std::istream& stream, int n, framebuffer& buffer, int x, int y);
}

#endif