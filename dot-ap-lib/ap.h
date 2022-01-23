#ifndef AP_H
#define AP_H

#include "utility.h"
#include <istream>
#include <vector>

namespace marinedalek::dotap {
struct ap_header {
	std::vector<int32_t> element_offsets;
};

std::istream& operator>>(std::istream& stream, ap_header& ap);
}

#endif
