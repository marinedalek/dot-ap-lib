#include "dot-ap-lib.h"
#include "raylib.h"
#include <cstring>
#include <algorithm>
#include <execution>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <spanstream>
#include <string>

char constexpr frag_code[] =
	R"(
#version 330
in vec3 vertexPos;
in vec2 fragTexCoord;
in vec4 fragColor;
uniform sampler2D texture0;
uniform sampler2D texture1;
out vec4 finalColor;
void main()
{
	float index = texture(texture0, fragTexCoord).r * 255.0 / 256.0;
	finalColor = texture(texture1, vec2(index, 0.0));
}
)";

void convert_frame(dotap::framebuffer& source, Image& dest)
{
	auto data = static_cast<uint32_t*>(dest.data);
	std::transform(std::execution::par_unseq,
		source.pixels.begin(), source.pixels.end(), data, [&source]
		(const uint8_t elem) {
			auto color = source.palette[elem];
			return 0xFF000000 | color.b << 16 | color.g << 8 | color.r;
			}
	);
}


auto ray_8bpp_image(dotap::framebuffer& source)
->Image
{
	return Image{
		.data = source.pixels.data(),
		.width = source.width,
		.height = source.height,
		.mipmaps = 1,
		.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE 
	};
}

auto ray_palette_image(dotap::framebuffer& source)
->Image
{
	return Image{
		.data = source.palette.data(),
		.width = 256,
		.height = 1,
		.mipmaps = 1,
		.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8
	};
}


int main()
{
	auto path = std::filesystem::path("C12.AP");
	auto file = std::ifstream(path, std::ios::binary);
	auto filesize = std::filesystem::file_size(path);
	auto buf = std::make_unique<char[]>(filesize);
	file.read(buf.get(), filesize);
	auto spn = std::span{ buf.get(), filesize };
	auto c00 = std::ispanstream{ spn };
	auto header = dotap::ap_header{};
	c00 >> header;
	auto ptf_offset = header.element_offsets.at(1);
	c00.seekg(ptf_offset, std::ios::beg);
	auto ptf_header = dotap::ptf::file_header{};
	c00 >> ptf_header;
	auto framebuffer = dotap::framebuffer(ptf_header.video_width, ptf_header.video_height);

	auto frames = dotap::ptf::parse_frames(c00, ptf_offset + ptf_header.given_length);
	
	int const screen_width = ptf_header.video_width > 0 ? ptf_header.video_width * 2 : 640;
	int const screen_height = ptf_header.video_height > 0 ? ptf_header.video_height * 2 : 480;
	//SetConfigFlags(FLAG_VSYNC_HINT);
	InitWindow(screen_width, screen_height, "dotap demo");
	// SetTargetFPS(500);
	auto video_image = ray_8bpp_image(framebuffer);
	auto video_texture = LoadTextureFromImage(video_image);
	auto palette_image = ray_palette_image(framebuffer);
	auto palette_texture = LoadTextureFromImage(palette_image);
	auto shader = LoadShaderFromMemory(0, frag_code);
	auto palette_loc = GetShaderLocation(shader, "texture1");
	auto frame = int{ 0 };
	while (!IsKeyPressed(KEY_A)) {
		BeginDrawing(); 
		ClearBackground(DARKGRAY);
		DrawFPS(5, 5);
		EndDrawing();
	}
	InitAudioDevice();
	SetAudioStreamBufferSizeDefault(2205);
	auto audio_stream = LoadAudioStream(22050, 16, 1);
	//PlayAudioStream(audio_stream);

	auto start_time = GetTime();
	int old_frame{ 0 };
	bool audio_loaded{ false };
	bool update_frame{ true };
	int current_frame{ 0 };
	while (!WindowShouldClose()) {
		if (IsKeyPressed(KEY_R)) {
			start_time = GetTime();
			current_frame = 0;
			old_frame = 0;
			audio_loaded = false;
			update_frame = true;
			StopAudioStream(audio_stream);
			UnloadAudioStream(audio_stream);
			audio_stream = LoadAudioStream(22050, 16, 1);
		}
		/*
		int current_frame = floor((GetTime() - start_time) / (ptf_header.ticks_per_frame / 60.0));
		
		if (current_frame >= frames.size() && IsAudioStreamPlaying(audio_stream)) {
			auto audio_buffer = std::array<char, 4410>{};
			audio_buffer.fill(0);
			UpdateAudioStream(audio_stream, audio_buffer.data(), 2205);
			StopAudioStream(audio_stream);

		}
		*/
		if (current_frame > old_frame && current_frame < frames.size()) {
			update_frame = true;
		}
		/*
		if (update_frame && (IsAudioStreamPlaying(audio_stream) || !audio_loaded)) {
			auto audio_buffer = std::array<char, 4410>{};
			audio_buffer.fill(0);
			if (auto audio_frame = frames.at(current_frame).audio_offset; audio_frame != -1) {
				c00.seekg(audio_frame, std::ios::beg);
				auto frame_header = dotap::ptf::frame_header{};
				c00 >> frame_header;
				//auto magic_number = dotap::read_as<uint32_t, std::endian::big>(c00);
				//auto riff_present = magic_number == 0x52494646;
				auto skip_header = dotap::wav::is_header(c00);
				c00.seekg(audio_frame + (skip_header ? 50 : 6), std::ios::beg);
				auto data_to_load = frame_header.data_length() - (skip_header ? 44 : 0);
				c00.read(audio_buffer.data(), data_to_load);
				UpdateAudioStream(audio_stream, audio_buffer.data(), 2205);
				if (!audio_loaded) { PlayAudioStream(audio_stream); }
				audio_loaded = true;
				//std::cout << current_frame << '\n';
			}
			else {
				audio_loaded = false;
			}
		}
		*/
		if (update_frame && current_frame < frames.size() && frames.at(current_frame).video_offset != -1) {
			c00.seekg(frames.at(current_frame).video_offset);
			auto frame_header = dotap::ptf::frame_header{};
			c00 >> frame_header;
			if (frame_header.has_palette()) {
				dotap::ptf::read_palette(framebuffer, c00);
			}
			if (frame_header.type == dotap::ptf::frame_type::flic) {
				dotap::flic::read_frame(framebuffer, c00);
				// convert_frame(framebuffer, video_image);
				// UpdateTexture(video_texture, video_image.data);
				UpdateTexture(video_texture, framebuffer.pixels.data());
				UpdateTexture(palette_texture, framebuffer.palette.data());
			}
			if (frame_header.type == dotap::ptf::frame_type::bic) {
				dotap::bic::read_frame(framebuffer, c00, frame_header.data_length());
				//convert_frame(framebuffer, video_image);
				//UpdateTexture(video_texture, video_image.data);
				UpdateTexture(video_texture, framebuffer.pixels.data());
				UpdateTexture(palette_texture, framebuffer.palette.data());
			}
		}

		old_frame = current_frame;
		++current_frame;
		if (current_frame == frames.size()) {
			current_frame = 0;
			old_frame = -1;
		}
		BeginDrawing();
		ClearBackground(DARKGRAY);
		BeginShaderMode(shader);
		SetShaderValueTexture(shader, palette_loc, palette_texture);
			DrawTextureEx(video_texture, { 0,0 }, 0.0, 2.0, WHITE);
		EndShaderMode();

		DrawFPS(5, 5);
		auto frame_num = std::format("{}", current_frame);
		DrawText(frame_num.c_str(), 8, 28, 30, DARKGRAY);
		DrawText(frame_num.c_str(), 5, 25, 30, RAYWHITE);
		EndDrawing();
		update_frame = false;
		}
	return 0;
}