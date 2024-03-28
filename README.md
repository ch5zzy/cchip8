# cchip8

A CHIP-8 emulator written in C++.

## Compiling

First, install [SDL2](https://www.libsdl.org/). Then, clone the repo and run `make`.

## Usage

To play a ROM, run

`./cchip8 <ROM> <delay> <scale>`

where `<ROM>` is the path to the ROM, `<delay>` is the delay between cycles in millisceonds, and `<scale>` is how much the display output should be scaled by.
