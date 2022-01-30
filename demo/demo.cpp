#include "dot-ap-lib.h"
#include <format>
#include <fstream>
#include <iostream>

int main()
{
	auto alley = std::ifstream("C:\\GOG Games\\Under a Killing Moon\\DMAP.LZ", std::ios::binary); // MMMM Hardcoded paths
	if (!alley.is_open()) {
		std::cerr << "Failed to open ALLEY.AP\n";
		return 0;
	}
	auto header = dotap::ap_header{};
	if (alley >> header) {
		std::cout << std::format("There are {} entries in ALLEY.AP.\n", header.element_offsets.size());
	}
	else {
		std::cerr << "Error reading header from ALLEY.AP.\n";
	}
	for (auto offset : header.element_offsets) {
		std::cout << std::format("Offset {:#08x}\n", offset);
	}
}