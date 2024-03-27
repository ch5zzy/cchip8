CC := g++

CFLAGS := $(shell sdl2-config --cflags --libs) --std=c++11 -Wall -lm

SRCS := Chip8.cc Platform.cc Main.cc

EXEC := cchip8

all: $(EXEC)

$(EXEC): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS)

clean:
	rm -f $(EXEC)

.PHONY: all clean