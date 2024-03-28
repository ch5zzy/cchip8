#include <getopt.h>

#include <chrono>
#include <iostream>

#include "Chip8.h"
#include "Platform.h"

using namespace std;

int main(int argc, char** argv) {
	if (argc < 4) {
		cerr << "usage: " << argv[0] << " <ROM> <delay> <scale>\n";
		exit(EXIT_FAILURE);
	}

	// Parse arguments.
	char const* romFilename = argv[1];
	int cycleDelay = stoi(argv[2]);
	int displayScale = stoi(argv[3]);

	Platform platform(DISPLAY_WIDTH * displayScale, DISPLAY_HEIGHT * displayScale, DISPLAY_WIDTH, DISPLAY_HEIGHT);

	Chip8 chip8;
	chip8.loadRom(romFilename);

	int displayPitch = sizeof(chip8.display[0]) * DISPLAY_WIDTH;

	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;
	while (!quit) {
		quit = platform.handleInput(chip8.keypad);

		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		if (dt > cycleDelay) {
			lastCycleTime = currentTime;
			chip8.cycle();
			platform.update(chip8.display, displayPitch);
		}
	}

	return 0;
}