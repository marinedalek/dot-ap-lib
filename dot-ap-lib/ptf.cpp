#include "ptf.h"

std::istream& marinedalek::dotap::ptf::operator>>(std::istream& stream, file_header& header)
{
	// TODO: handle invalid file
	header.given_length = read_as<uint32_t>(stream);
	stream.read(reinterpret_cast<char*>(header.magic_number.data()), 4);
	header.unknown_a = read_as<uint16_t>(stream);
	header.frame_count = read_as<uint16_t>(stream);
	header.video_width = read_as<uint16_t>(stream);
	header.video_height = read_as<uint16_t>(stream);
	header.bpp = read_as<uint16_t>(stream);
	header.ticks_per_frame = read_as<uint16_t>(stream);
	stream.read(reinterpret_cast<char*>(header.unknown_b.data()), 8);
	header.unknown_c = read_as<uint32_t>(stream);
	stream.seekg(0x20, std::ios::cur);
	return stream;
}

std::istream& marinedalek::dotap::ptf::operator>>(std::istream& stream, frame_header& header)
{
	uint16_t constexpr flic_magic{ 0xF1FA };
	uint16_t constexpr bic_magic{ 0x0B1C };
	uint16_t constexpr wave_magic{ 0x5657 }; // little endian equiv. of "WV"
	header.given_length = read_as<int32_t>(stream);
	auto magic_number = read_as<uint16_t>(stream);
	switch (magic_number) {
	case flic_magic:
		header.type = frame_type::flic;
		break;
	case bic_magic:
		header.type = frame_type::bic;
		break;
	case wave_magic:
		header.type = frame_type::wave;
		break;
	default:
		// not a valid frame header, so reset the stream position
		// TODO: report an error
		stream.seekg(-6, std::ios::cur);

	}

	return stream;
}

int32_t marinedalek::dotap::ptf::frame_header::overall_length()
{
	using enum frame_type;
	return 6 + data_length() + (has_palette() ? 0x300 : 0);
}

int32_t marinedalek::dotap::ptf::frame_header::data_length()
{
	using enum frame_type;
	int32_t absolute_length = has_palette() ? 0 - given_length : given_length;
	return (type == flic) ? absolute_length - 6 : absolute_length;
}

bool marinedalek::dotap::ptf::frame_header::has_palette()
{
	return given_length < 0;
}

auto marinedalek::dotap::ptf::parse_frames(std::istream& stream, int eof_offset)
-> std::vector<dotap::ptf::logical_frame>
{
	// expects stream to point to the first frame upon entry
	// otherwise, snake demons will blow up the moon
	auto frames = std::vector<dotap::ptf::logical_frame>{};
	using enum dotap::ptf::frame_type;
	while (stream.tellg() < eof_offset) {
		auto header = dotap::ptf::frame_header{};
		stream >> header;
		switch (header.type) {
		case flic:
		case bic:
			frames.push_back({ static_cast<int>(stream.tellg()) - 6, -1, header.type });
			break;
		case wave:
			if (!frames.empty() && frames.back().audio_offset == -1) {
				frames.back().audio_offset = (static_cast<int>(stream.tellg()) - 6);
			}
			else {
				frames.push_back({ -1, static_cast<int>(stream.tellg()) - 6 , header.type});
			}
			break;
		default:
			break;
		}

		stream.seekg(header.overall_length() - 6, std::ios::cur);
	}
	return frames;
}

void marinedalek::dotap::ptf::read_palette(framebuffer& buffer, std::istream& stream)
{
	stream.read(reinterpret_cast<char*>(buffer.palette.data()), 0x300);
	std::for_each_n(buffer.palette.begin(), 0x100, [&stream](color& c) {
		c.r *= 4;
		c.g *= 4;
		c.b *= 4;
		});
}
