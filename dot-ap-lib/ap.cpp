#include "ap.h"
#include <algorithm>
#include <iostream>
#include <iterator>

namespace marinedalek::dotap {
std::istream& operator>> (std::istream& stream, ap_header& ap)
{
	// how many offsets are there in this .AP?
	auto offset_count{ read_as<uint16_t>(stream) };

	// grab the number of offsets specified 
	ap.element_offsets.clear();
	std::generate_n(std::back_inserter(ap.element_offsets), offset_count, 
					[&](){return read_as<uint32_t>(stream); });
	return stream;
}
}