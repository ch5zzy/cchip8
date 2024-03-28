#ifndef PLATFORM_H
#define PLATFORM_H

#include <cstdint>
#include <SDL_video.h>
#include <SDL_render.h>
#include <map>
#include <SDL_keycode.h>

class Platform {
	public:
		Platform(int windowWidth, int windowHeight, int textureWidth, int textureHeight);
		~Platform();
		void update(void const* buffer, int pitch);
		bool handleInput(uint8_t* keys);

	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
		SDL_Texture* texture;
		const std::map<SDL_Keycode, uint8_t> keymap = {
			{SDLK_x, 0},   {SDLK_1, 1},   {SDLK_2, 2},   {SDLK_3, 3},
			{SDLK_q, 4},   {SDLK_w, 5},   {SDLK_e, 6},   {SDLK_a, 7},
			{SDLK_s, 8},   {SDLK_d, 9},   {SDLK_z, 0xA}, {SDLK_c, 0xB},
			{SDLK_4, 0xC}, {SDLK_r, 0xD}, {SDLK_f, 0xE}, {SDLK_v, 0xF}};
};

#endif