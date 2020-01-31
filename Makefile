CC=gcc
CFLAGS= -O3 -g -std=c11 -Wall -Wextra -Wshadow -Wpedantic -Wno-unused-value
CLIBS= `sdl2-config --cflags --libs` -lSDL2_gfx -lm
EXE=main

.PHONY: all vec build clean

all: clean build

clean:
	rm -f bin/*

bin/vec.o: src/vec.c src/vec.h
	$(CC) -c -o bin/vec.o src/vec.c -O3 -g

build: bin/vec.o
	$(CC) -o bin/$(EXE) src/grid.c bin/vec.o $(CFLAGS) $(CLIBS)
