#include "SDL.h"
#include <iostream>

#include "Platform.h"

#define WINDOW_TITLE "cchip8"

Platform::Platform(int windowWidth, int windowHeight, int textureWidth, int textureHeight) {
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow(WINDOW_TITLE, 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
}

Platform::~Platform() {
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Platform::update(void const* buffer, int pitch) {
	SDL_UpdateTexture(texture, nullptr, buffer, pitch);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}

bool Platform::handleInput(uint8_t* keys) {
	SDL_Event event;
	SDL_Keycode key;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
			{
				return true;
			}
			case SDL_KEYDOWN:
			{
				// Mark the key as pressed.
				key = event.key.keysym.sym;
				if (keymap.count(key) > 0) {
					keys[keymap.at(key)] = 1;
				}
				break;
			}
			case SDL_KEYUP:
			{
				// Mark the key as not pressed.
				key = event.key.keysym.sym;
				if (keymap.count(key) > 0) keys[keymap.at(key)] = 0;
				break;
			}
		}
	}

	return false;
}