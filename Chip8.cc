#include "Chip8.h"

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

Chip8::Chip8() {
	// Load the fontset into memory.
	for (int i = 0; i < FONTSET_SIZE; i++)
		memory[FONTSET_START_ADDR + i] = fontset[i];
	
	// Set the display to zeroes.
	memset(display, 0, sizeof(display));

	// Mark all keys as not pressed.
	memset(keypad, 0, sizeof(keypad));
}

void Chip8::loadRom(const string path) {
	ifstream file (path, ios::binary | ios::ate);

	if (file.is_open()) {
		// Get the ROM size and validate that it can fit in memory.
		streampos size = file.tellg();
		if (size > (MEM_SIZE - ROM_START_ADDR) * sizeof(uint8_t)) {
			cerr << "ROM size is too large.\n";
			exit(EXIT_FAILURE);
		}

		// Read the ROM into memory.
		file.seekg(0, ios::beg);
		file.read((char *) memory + ROM_START_ADDR, size);
		file.close();
	} else {
		cerr << "Unable to open ROM.\n";
		exit(EXIT_FAILURE);
	}
}

void Chip8::draw(uint8_t x, uint8_t y, uint8_t height) {
	// Reset the flag register.
	registers[0xF] = 0;

	unsigned int xx = registers[x] % DISPLAY_WIDTH;
	unsigned int yy = registers[y] % DISPLAY_HEIGHT;

	for (int row = 0; row < height; ++row) {
		for (unsigned int col = 0; col < 8; ++col) {
			uint8_t spriteBit = memory[index + row] >> (7 - col) & 1;
			uint32_t* screenBit = &display[(yy + row) * DISPLAY_WIDTH + (xx + col)];

			if (spriteBit) {
				registers[0xF] |= *screenBit == 0xFFFFFFFF;  // Mark flag register if this pixel is being flipped off.
				*screenBit ^= 0xFFFFFFFF;
			}
		}
	}

}

void Chip8::cycle() {
	// Read the current instruction (2 bytes) and increment the program counter.
	uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
	pc += 2;

	uint8_t i = (opcode >> 12) & 0xF;   // First nibble, used for register lookups.
	uint8_t x = (opcode >> 8) & 0xF;    // Second nibble, used for register lookups.
	uint8_t y = (opcode >> 4) & 0xF;    // Third nibble.
	uint8_t n = opcode & 0xF;           // Fourth nibble.
	uint8_t nn = opcode & 0xFF;         // Third and fourth nibble.
	uint16_t nnn = opcode & 0xFFF;      // Second, third, and fourth nibble.

	switch (i) {
		case 0:
			switch (nnn) {
				case 0x0E0: // Clear screen.
					memset(display, 0, sizeof(display));
					break;
				case 0x0EE: // Return from subroutine.
					pc = callStack.top();
					callStack.pop();
					break;
			}
			break;
		case 1: // Jump.
			pc = nnn;
			break;
		case 2: // Subroutine.
			callStack.push(pc);
			pc = nnn;
			break;
		case 3: // Skip if Vx == nn.
			if (registers[x] == nn) pc += 2;
			break;
		case 4: // Skip if Vx != nn.
			if (registers[x] != nn) pc += 2;
			break;
		case 5: // Skip if Vx = Vy.
			if (registers[x] == registers[y]) pc += 2;
			break;
		case 6: // Set Vx to nn.
			registers[x] = nn;
			break;
		case 7: // Add nn to Vx.
			{
				uint16_t sum = registers[x] + nn;
				registers[x] = (uint8_t) sum;
				registers[0xF] = sum >= UINT8_MAX;  // Carry.
				break;
			}
			break;
		case 8: // Arithmetic.
			switch (n) {
				case 0: // Set Vx to Vy.
					registers[x] = registers[y];
					break;
				case 1: // Set Vx to Vx | Vy.
					registers[x] |= registers[y];
					break;
				case 2: // Set Vx to Vx & Vy.
					registers[x] &= registers[y];
					break;
				case 3: // Set Vx to Vx ^ Vy.
					registers[x] ^= registers[y];
					break;
				case 4: // Set Vx to Vx + Vy.
				{
					uint16_t sum = registers[x] + registers[y];
					registers[x] = (uint8_t) sum;
					registers[0xF] = sum >= UINT8_MAX;  // Carry.
					break;
				}
				case 5: // Set Vx to Vx - Vy.
					registers[0xF] = registers[y] > registers[x];   // Borrow.
					registers[x] = (uint8_t) (registers[x] - registers[y]);
					break;
				case 7: // Set Vx to Vy - Vx.
					registers[0xF] = registers[x] > registers[y];   // Borrow.
					registers[x] = (uint8_t) (registers[y] - registers[x]);
					break;
				case 6: // Shift Vx right.
					registers[0xF] = registers[x] & 1; // Carry.
					registers[x] >>= 1;
					break;
				case 0xE: // Shift Vx left.
					registers[0xF] = registers[x] >> 7; // Carry.
					registers[x] <<= 1;
					break;
			}
		case 9: // Skip if Vx != Vy.
			if (registers[x] != registers[y]) pc += 2;
			break;
		case 0xA:   // Set index register.
			index = nnn;
			break;
		case 0xB:   // Jump to V0 + offset.
			pc = registers[0] + nnn;
			break;
		case 0xC:   // Set Vx to a random value.
			registers[x] = (rand() % UINT8_MAX) & nn;
			break;
		case 0xD:   // Draw.
			draw(x, y, n);
			break;
		case 0xE:   // Skip if key.
			switch (nn) {
				case 0x9E:  // Skip if key Vx is pressed.
					if (keypad[registers[x]]) pc += 2;
					break;
				case 0xA1:  // Skip if key Vx is not pressed.
					if (!keypad[registers[x]]) pc += 2;
					break;
			}
			break;
		case 0xF:
			switch (nn) {
				case 0x07:  // Set Vx to delay timer.
					registers[x] = delayTimer;
					break;
				case 0x15:  // Set delay timer to Vx.
					delayTimer = registers[x];
					break;
				case 0x18:  // Set sound timer to Vx.
					soundTimer = registers[x];
					break;
				case 0x1E:  // Add Vx to index.
				{
					unsigned int sum = index + registers[x];
					index = (uint16_t) sum;
					registers[0xF] = sum >= UINT16_MAX; // Carry.
					break;
				}
				case 0x0A:  // Wait until a key is pressed.
				{
					bool keyPressed = false;
					for (uint8_t i = 0; i < KEYPAD_SIZE; i++) {
						if (keypad[i]) {
							keyPressed = true;
							break;
						}
					}
					if (!keyPressed) pc -= 2;
					break;
				}
				case 0x29:  // Set index to address of font character Vx.
					index = FONTSET_START_ADDR + FONTSET_CHAR_SIZE * registers[x];
					break;
				case 0x33:  // Store decimal digits of x at index, index + 1, and index + 2.
				{
					uint8_t d1 = floor(registers[x] / 100 % 10);
					uint8_t d2 = floor(registers[x] / 10 % 10);
					uint8_t d3 = floor(registers[x] % 10);
					memory[index] = d1;
					memory[index + 1] = d2;
					memory[index + 2] = d3;
					break;
				}
				case 0x55:  // Store V0, V1, ..., Vx in memory starting at index.
					for (int j = index; j <= index + x; ++j) memory[j] = registers[j - index];
					break;
				case 0x65:  // Load index, index + 1, ..., index + x from memory into registers.
					for (int j = 0; j <= x; ++j) registers[j] = memory[index + j];
					break;
			}
			break;
		default:
			// Unknown instruction.
			break;
	}

	// Update timers.
	if (delayTimer > 0) --delayTimer;
	if (soundTimer > 0) --soundTimer;
};