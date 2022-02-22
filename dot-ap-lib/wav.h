#ifndef DOTAP_WAV_H
#define DOTAP_WAV_H

#include "utility.h"
#include <istream>

namespace marinedalek::dotap::wav {
bool is_header(std::istream& stream);
}

#endif