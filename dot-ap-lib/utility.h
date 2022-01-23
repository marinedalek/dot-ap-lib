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
}

#endif
