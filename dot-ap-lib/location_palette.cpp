#include "location_palette.h"

std::istream& marinedalek::dotap::operator>>(std::istream& stream, location_palette& palette)
{
	if (auto entries = read_as<uint16_t>(stream); entries != 0x300) {
		// TODO: handle this better
		// push read bytes back onto stream
		stream.putback(signed char{ (entries >> 8) & 0xFF });
		stream.putback(signed char{ entries & 0xFF });
		return stream;
	}
	stream.read(reinterpret_cast<char*>(palette.data()), 256 * 3);
	return stream;
}
