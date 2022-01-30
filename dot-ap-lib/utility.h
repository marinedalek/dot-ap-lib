#ifndef DOTAP_UTILITY_H
#define DOTAP_UTILITY_H

#include <array>
#include <bit>
#include <concepts>
#include <cstddef>
#include <istream>
#include <span>

namespace marinedalek::dotap {
template <std::integral T, std::endian source_endianness = std::endian::native>
T constexpr copy_blob_as(std::span<std::byte const> blob, std::size_t const offset = 0)
{
	auto constexpr extent = sizeof(T);
	std::array<std::byte, extent> buffer;
	// NOTE: comparison of endianness is used here rather than shifting values in 
	//		 due to sub-optimal code generation on certain platforms
	if constexpr (source_endianness == std::endian::native) {
		std::copy(blob.begin() + offset, blob.begin() + offset + extent, buffer.begin());
	}
	else {
		std::copy(blob.rend() - offset - extent, blob.rend() - offset, buffer.begin());
	}
	return std::bit_cast<T>(buffer);
}

template <std::integral T, std::endian source_endianness = std::endian::native>
T constexpr read_as(std::istream& stream)
{
	auto constexpr extent = sizeof(T);
	std::array<std::byte, extent> buffer;
	stream.read(reinterpret_cast<char*>(buffer.data()), extent);
	return copy_blob_as<T, source_endianness>(buffer);
}

struct bit_reader {
	int cached_bit_count{ 0 };
	std::byte cached_byte;
	template <std::integral T = uint16_t>
	T read_bits(std::istream& stream, int const num_bits)
	{
		T output_bits{ 0 };
		auto wanted_bits = num_bits;
		while (wanted_bits > 0) {
			if (cached_bit_count == 0) {
				// we want bits, so load a new byte from the stream
				stream.read(reinterpret_cast<char*>(&cached_byte), 1);
				cached_bit_count = 8;
			}
			if (cached_bit_count <= wanted_bits) {
				// In this case all of the cached byte will be used up.
				// Get remaining bits from cached byte and place them appropriately
				output_bits |= std::to_integer<T>(cached_byte) << (num_bits - wanted_bits);
				wanted_bits -= cached_bit_count;
				cached_bit_count = 0;
				cached_byte = std::byte{ 0 };
			}
			else {
				// In this case part of the cached byte will be used.
				// It needs to be masked off and shifted into place onto the output.
				auto nibble = cached_byte & std::byte((1 << wanted_bits) - 1);
				output_bits |= std::to_integer<T>(nibble) << (num_bits - wanted_bits);
				cached_byte >>= wanted_bits;
				cached_bit_count -= wanted_bits;
				wanted_bits = 0;
			}
		}
		return output_bits;
	}
};

}

#endif
