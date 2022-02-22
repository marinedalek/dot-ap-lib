#ifndef DOTAP_FLIC_H
#define DOTAP_FLIC_H

#include "framebuffer.h"
#include "utility.h"
#include <cstdint>
#include <istream>

namespace marinedalek::dotap::flic {

void read_frame(framebuffer& buffer, std::istream& stream);
void read_delta_flc(framebuffer& buffer, std::istream& stream);
void read_color_64(framebuffer_palette& palette, std::istream& stream);
void read_byte_run(framebuffer& buffer, std::istream& stream);

}

#endif
