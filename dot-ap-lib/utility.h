#ifndef DOTAP_UTILITY_H
#define DOTAP_UTILITY_H

#include <bit>
#include <concepts>
#include <cstddef>
#include <span>

namespace marinedalek::dotap {
template <std::integral T, bool source_is_big_endian = false>
T constexpr copy_blob_as(std::span<std::byte> blob, std::size_t offset)
{
	auto constexpr extent = sizeof(T);
	std::array<std::byte, extent> buffer;
	if constexpr ((std::endian::native == std::endian::big) == source_is_big_endian) {
		std::copy(blob.begin() + offset, blob.begin() + offset + extent, buffer.begin());
	}
	else {
		std::copy(blob.rend() - offset - extent, blob.rend() - offset, buffer.begin());
	}
	return std::bit_cast<T>(buffer);
}
}

#endif
