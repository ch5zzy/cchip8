#include <cstdint>
#include <stack>
#include <string>

#define NUM_REGISTERS 16
#define MEM_SIZE 4096
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define KEYPAD_SIZE 16

#define FONTSET_START_ADDR 0x50
#define ROM_START_ADDR 0x200

#define FONTSET_SIZE 80

uint8_t fontset[FONTSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0
    0x20, 0x60, 0x20, 0x20, 0x70,   // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,   // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,   // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,   // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,   // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,   // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,   // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,   // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,   // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,   // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,   // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,   // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,   // E
    0xF0, 0x80, 0xF0, 0x80, 0x80    // F
};

class Chip8 {
    public:
        Chip8();
        uint8_t registers[NUM_REGISTERS];
        uint16_t index;
        uint16_t pc = ROM_START_ADDR;
        uint8_t memory[MEM_SIZE];
        std::stack<uint16_t> callStack;
        uint8_t keypad[KEYPAD_SIZE];
        uint32_t display[DISPLAY_WIDTH][DISPLAY_HEIGHT];
        uint8_t delayTimer;
        uint8_t soundTimer;
    
    private:
        void loadRom(const std::string path);
        void draw(uint8_t x, uint8_t y, uint8_t n);
        void cycle();
};