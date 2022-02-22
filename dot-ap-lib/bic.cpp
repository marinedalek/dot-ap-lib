#include "bic.h"

void marinedalek::dotap::bic::read_frame(framebuffer& buffer, std::istream& stream, uint32_t size)
{
	//auto frame_size = read_as<uint32_t>(stream);
	auto end_pos = int(stream.tellg()) + size;
	int y{ 0 };
	bool good_frame = true;
	while (stream.tellg() < end_pos && good_frame) {
		auto flags = read_as<uint8_t>(stream);

		for (int flag_bit{ 0 }; flag_bit < 4; ++flag_bit) {
			if ((flags >> flag_bit) & 0x01 && good_frame) { // if flag [flag_bit] is set
				int x{ 0 };
				auto chunk_count = read_as<int16_t>(stream);
				int debug_pos = stream.tellg();
				bool read_offset_first = chunk_count < 0;
				chunk_count = std::abs(chunk_count);
				good_frame = chunk_count < 160;
				if (!good_frame) { 
					break; 
				}
				while (chunk_count > 0) {
					if (read_offset_first) {
						auto offset = read_as<uint8_t>(stream);
						x += offset * 4;
						--chunk_count;
					}
					read_offset_first = true;
					auto block_count = read_as<uint8_t>(stream);
					switch (flag_bit) {
					case 0: // Draw solid blocks
						copy_n_solid_blocks(stream, block_count, buffer, x, y);
						break;
					case 1:
						copy_n_8bpp_pattern_blocks(stream, block_count, buffer, x, y);
						break;
					case 2:
						copy_n_16_byte_blocks(stream, block_count, buffer, x, y);
						break;
					case 3: 
						copy_n_4bpp_pattern_blocks(stream, block_count, buffer, x, y);
						break;
					default:
						break;
					}
					x += block_count * 4;
					--chunk_count;

				}
			}
		}
		y += 4;
	}
}

void marinedalek::dotap::bic::draw_solid_block(uint8_t c, framebuffer& buffer, int x, int y)
{
	//if (x >= buffer.width || y >= buffer.height) { return; }
	for (int y_offset{ 0 }; y_offset < 4; ++y_offset) {
		auto y_buffer_pos = buffer.width * (y + y_offset);
		for (int x_offset{ 0 }; x_offset < 4; ++x_offset) {
			buffer.pixels.at(x + x_offset + y_buffer_pos) = c;
		}
	}
}

void marinedalek::dotap::bic::draw_8bpp_pattern_block(uint8_t c1, uint8_t c2, uint16_t pattern, framebuffer& buffer, int x, int y)
{
	for (int y_offset{ 0 }; y_offset < 4; ++y_offset) {
		auto y_buffer_pos = buffer.width * (y + y_offset);
		for (int x_offset{ 0 }; x_offset < 4; ++x_offset) {
			buffer.pixels.at(x + x_offset + y_buffer_pos) =
				(pattern & 0x1) ? c2 : c1;
			pattern >>= 1;
		}
	}
}

void marinedalek::dotap::bic::draw_16_byte_block(std::istream& stream, framebuffer& buffer, int x, int y)
{
	for (int y_offset{ 0 }; y_offset < 4; ++y_offset) {
		auto y_buffer_pos = buffer.width * (y + y_offset);
		for (int x_offset{ 0 }; x_offset < 4; ++x_offset) {
			buffer.pixels.at(x + x_offset + y_buffer_pos) =
				read_as<uint8_t>(stream);
		}
	}
}

void marinedalek::dotap::bic::copy_n_solid_blocks(std::istream& stream, int n, framebuffer& buffer, int x, int y)
{
	for (int block{ 0 }; block < n; ++block) {
		auto color = read_as<uint8_t>(stream);
		draw_solid_block(color, buffer, x + block * 4, y);
	}
}

void marinedalek::dotap::bic::copy_n_4bpp_pattern_blocks(std::istream& stream, int n, framebuffer& buffer, int x, int y)
{
	for (int block{ 0 }; block < n; ++block) {
		auto packed_colors = read_as<uint8_t>(stream);
		auto color_a = packed_colors & 0x0F;
		auto color_b = packed_colors >> 4;
		auto pattern = read_as<uint16_t>(stream);
		draw_8bpp_pattern_block(color_a, color_b, pattern, buffer, x + block * 4, y);
	}
}

void marinedalek::dotap::bic::copy_n_8bpp_pattern_blocks(std::istream& stream, int n, framebuffer& buffer, int x, int y)
{
	for (int block{ 0 }; block < n; ++block) {
		auto color_a = read_as<uint8_t>(stream);
		auto color_b = read_as<uint8_t>(stream);
		auto pattern = read_as<uint16_t>(stream);
		draw_8bpp_pattern_block(color_a, color_b, pattern, buffer, x + block * 4, y);
	}
}

void marinedalek::dotap::bic::copy_n_16_byte_blocks(std::istream& stream, int n, framebuffer& buffer, int x, int y)
{
	for (int block{ 0 }; block < n; ++block) {
		draw_16_byte_block(stream, buffer, x + block * 4, y);
	}
}
