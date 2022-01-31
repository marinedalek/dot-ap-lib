#include "acc_dbe.h"

std::istream& marinedalek::dotap::operator>>(std::istream& stream, acc_dbe_header& header)
{
	std::array<std::byte, 4> constexpr acc_number{
		std::byte{'A'},
		std::byte{'C'},
		std::byte{'C'},
		std::byte{0x01}
	};
	std::array<std::byte, 4> constexpr dbe_number{
		std::byte{'D'},
		std::byte{'B'},
		std::byte{'E'},
		std::byte{0x01}
	};
	std::array<std::byte, 4> magic_number{};
	stream.read(reinterpret_cast<char*>(magic_number.data()), 4);

	if ((magic_number != acc_number) || (magic_number != dbe_number)) {
		// TODO: handle this awful predicament better
		// Also, be nice and fix the stream
		std::for_each(magic_number.rbegin(), magic_number.rend(),
			[&stream](std::byte b) {stream.putback(std::to_integer<char>(b)); });
		return stream;
	}
	
	header.decompressed_length = read_as<uint32_t>(stream);
	header.compressed_length = read_as<uint32_t>(stream);
	return stream;
}

void marinedalek::dotap::init_lzw_dictionary(std::vector<std::vector<std::byte>>& dictionary)
{
	dictionary.clear();
	// 258 below is 256 + 2 placeholder codes for RESET and STOP
	for (int i{ 0 }; i < 258; ++i) {
		dictionary.emplace_back(std::vector<std::byte>{std::byte(i)});
	}
}

auto marinedalek::dotap::decompress_lzw(std::istream& stream, std::size_t uncompressed_size)
-> std::vector<std::byte>
{
	int16_t constexpr reset_code{ 0x100 };
	int16_t constexpr stop_code{ 0x101 };
	int code_length{ 9 };
	auto dictionary = std::vector<std::vector<std::byte>>();
	auto output_buffer = std::vector<std::byte>();
	auto reader = bit_reader();

	while (output_buffer.size() <= uncompressed_size) {
		switch (auto code = reader.read_bits(stream, code_length)) {
		case reset_code:
			init_lzw_dictionary(dictionary);
			code_length = 9;
			break;
		case stop_code:
			return output_buffer;
		default:
			auto const& temp_output = dictionary.at(code);
			dictionary.back().push_back(temp_output.front());
			std::copy(temp_output.begin(), temp_output.end(), std::back_inserter(output_buffer));
			dictionary.emplace_back(temp_output);
			if (dictionary.size() > uint16_t(char{ 1 } << code_length)) {
				++code_length;
			}
		}
	}
	return output_buffer;
}
