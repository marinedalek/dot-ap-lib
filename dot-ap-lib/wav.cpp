#include "wav.h"

bool marinedalek::dotap::wav::is_header(std::istream& stream)
{
	uint32_t constexpr riff_id{ 0x52494646 };
	uint32_t constexpr wave_id{ 0x57415645 };
	auto chunk_id = read_as<uint32_t, std::endian::big>(stream);
	stream.seekg(4, std::ios::cur);
	auto subchunk_id = read_as<uint32_t, std::endian::big>(stream);
	stream.seekg(-12, std::ios::cur);
	return ((chunk_id == riff_id) && (subchunk_id == wave_id));
}
