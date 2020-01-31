CC=gcc
CFLAGS= -O3 -g -std=c11 -Wall -Wextra -Wshadow -Wpedantic -Wno-unused-value
CLIBS= `sdl2-config --cflags --libs` -lSDL2_gfx -lm
EXE=main

.PHONY: all build clean run

all: build

clean:
	rm -f ../bin/*

../bin/vec.o: vec.c vec.h
	$(CC) -c -o ../bin/vec.o vec.c -O3 -g

build: ../bin/vec.o
	$(CC) -o ../bin/$(EXE) grid.c ../bin/vec.o $(CFLAGS) $(CLIBS)

run: ../bin/$(EXE)
	../bin/$(EXE)
