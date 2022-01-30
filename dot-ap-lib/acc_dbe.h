#ifndef DOTAP_ACC_DBE_H
#define DOTAP_ACC_DBE_H

#include "utility.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <istream>

namespace marinedalek::dotap {
struct acc_dbe_header {
	uint32_t decompressed_length;
	uint32_t compressed_length;
};

std::istream& operator>>(std::istream& stream, acc_dbe_header& header);
void init_lzw_dictionary(std::vector<std::vector<std::byte>>& dictionary);
auto decompress_lzw(std::istream& stream, std::size_t uncompressed_size)
->std::vector<std::byte>;
}

#endif