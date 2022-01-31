#include "dot-ap-lib.h"
#include "raylib.h"
#include <format>
#include <fstream>
#include <iostream>

int main()
{
	auto alley = std::ifstream("C:\\GOG Games\\Under a Killing Moon\\ALLEY.AP", std::ios::binary); // MMMM Hardcoded paths
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
	
	alley.seekg(header.element_offsets.at(2), std::ios::beg);
	auto palette = dotap::location_palette();
	alley >> palette;
	
	int constexpr screen_width{ 512 };
	int constexpr screen_height{ 512 };
	InitWindow(screen_width, screen_height, "dotap demo");
	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(DARKGRAY);
		for (int i{ 0 }; i < 256; ++i) {
			int x{ (i % 16) * 32 };
			int y{ (i / 16) * 32 };
			auto temp_color = palette.at(i);
			Color draw_color{
				temp_color.r,
				temp_color.g,
				temp_color.b,
				uint8_t{255}
			};
			DrawRectangle(x, y, 32, 32, draw_color);
		}
		EndDrawing();
	}
	return 0;
}