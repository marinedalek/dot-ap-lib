#include "flic.h"
#include <algorithm>
#include <iostream>

void marinedalek::dotap::flic::read_frame(framebuffer& buffer, std::istream& stream)
{
	uint16_t constexpr type_delta_flc{ 7 };
	uint16_t constexpr type_color_64{ 11 };
	uint16_t constexpr type_black{ 13 };
	uint16_t constexpr type_byte_run{ 15 };
	auto chunk_count = read_as<uint16_t>(stream);
	if (chunk_count == 0) {
		// TODO: Find out what this weirdness is all about!
		stream.seekg(9, std::ios::cur);
		return;
	}
	stream.seekg(8, std::ios::cur);
	for (int i{ 0 }; i < int{ chunk_count }; ++i) {
		auto chunk_size = read_as<uint32_t>(stream);
		auto chunk_type = read_as<uint16_t>(stream);
		switch (chunk_type) {
		case type_delta_flc:
			read_delta_flc(buffer, stream);
			break;
		case type_color_64:
			read_color_64(buffer.palette, stream);
			break;
		case type_black:
			std::fill(buffer.pixels.begin(), buffer.pixels.end(), 0);
			break;
		case type_byte_run:
			read_byte_run(buffer, stream);
			break;
		default:
			// TODO: handle error case
			break;
		}
	} 
}

void marinedalek::dotap::flic::read_delta_flc(framebuffer& buffer, std::istream& stream)
{
	auto lines = read_as<int16_t>(stream);
	auto line = int{ 0 };
	while (line < buffer.height) {
		auto last_byte = int{ -1 };
		auto packet_count = int{ -1 };
		// Read the preamble for each line.
		while (packet_count < 0) {
			auto word = read_as<int16_t>(stream);
			if (word >= 0) {
				// This word describes the packet count.
				packet_count = word;
			}
			else {
				if (word & 0x4000) {
					// This word describes a line skip.
					line += -word;
					// TODO: CHECK THIS!!!
					if (line >= buffer.height) {
						return;
					}
				}
				else {
					// This word describes a final byte.
					last_byte = word & 0x00FF;
				}
			}
		}
		auto column = int{ 0 };
		for (int p{ 0 }; p < packet_count; ++p) {
			//auto skip = read_as<uint8_t>(stream);
			auto skip = uint8_t( stream.get() );
			column += skip;
			//auto packet_type = read_as<int8_t>(stream);
			auto packet_type = int8_t( stream.get() );
			if (packet_type >= 0) {
				// This packet represents the number of following words
				// to copy to the output image.
				for (int w{ 0 }; w < packet_type; ++w) {
					auto offset = line * buffer.width + column;
					//buffer.pixels.at(offset) = read_as<uint8_t>(stream);
					buffer.pixels.at(offset) = uint8_t( stream.get() );
					//buffer.pixels.at(offset + 1) = read_as<uint8_t>(stream);
					buffer.pixels.at(offset + 1) = uint8_t( stream.get() );
					column += 2;
				}
			}
			else {
				// This packet represents the number of times to
				// copy the following word into the output image
				//auto byte_a = read_as<uint8_t>(stream);
				auto byte_a = uint8_t( stream.get() );
				//auto byte_b = read_as<uint8_t>(stream);
				auto byte_b = uint8_t( stream.get() );
				for (int w{ 0 }; w < (0 - packet_type); ++w) {
					auto offset = line * buffer.width + column;
					buffer.pixels.at(offset) = byte_a;
					buffer.pixels.at(offset + 1) = byte_b;
					column += 2;
				}
			}
		}
		if (last_byte >= 0) {
			auto offset = (line + 1) * buffer.width - 1;
			buffer.pixels.at(offset) = last_byte & 0xFF;
		}
		++line;
	}
}

void marinedalek::dotap::flic::read_color_64(framebuffer_palette& palette, std::istream& stream)
{
	int color_index{ 0 };
	auto packet_count = read_as<uint16_t>(stream);
	for (int p{ 0 }; p < packet_count; ++p) {
		auto skip_count = read_as<uint8_t>(stream);
		int color_count = read_as<uint8_t>(stream);
		if (color_count == 0) { color_count = 256; }
		color_index += skip_count;
		/*
		for (int c{ 0 }; c < color_count; ++c) {
			stream.read(reinterpret_cast<char*>(palette.data() + color_index), 3);
			palette.at(color_index).r <<= 2;
			palette.at(color_index).g <<= 2;
			palette.at(color_index).b <<= 2;
			++color_index;
		}*/
		
		if (color_count > 0) {
			stream.read(reinterpret_cast<char*>(palette.data() + color_index), color_count * 3);
			std::for_each_n(palette.begin() + color_index,color_count, [&stream]( color& c) {
					c.r *= 4;
					c.g *= 4;
					c.b *= 4;
				});
		}
		color_index += color_count;
	}

}

void marinedalek::dotap::flic::read_byte_run(framebuffer& buffer, std::istream& stream)
{
	for (int y{ 0 }; y < buffer.height; ++y) {
		// skip the first byte; unused
		stream.seekg(1, std::ios::cur);
		int x{ 0 };
		while (x < buffer.width) {
			auto command = read_as<int8_t>(stream);
			if (command < 0) {
				for (int b{ 0 }; b < (0 - command); ++b) {
					buffer.pixels.at(x + y * buffer.width) = read_as<uint8_t>(stream);
					++x;
				}
			}
			else {
				auto pixel = read_as<uint8_t>(stream);
				for (int b{ 0 }; b < command; ++b) {
					buffer.pixels.at(x + y * buffer.width) = pixel;
					++x;
				}
			}
		}
	}
}
