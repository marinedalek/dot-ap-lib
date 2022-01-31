#ifndef DOTAP_LOCATION_PALETTE_H
#define DOTAP_LOCATION_PALETTE_H

#include "color.h"

#include <utility.h>
#include <algorithm>
#include <array>
#include <cstdint>
#include <istream>

namespace marinedalek::dotap {
using location_palette = std::array<dotap::color, 256>;

std::istream& operator>> (std::istream& stream, location_palette& palette);
}

#endif
