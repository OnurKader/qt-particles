all: main

.PHONY: main

main:
	clear && rm -f ./main && gcc -o main main.c vec.o -O2 -g -lm -std=c11 -Wall -Wextra -Wshadow -Wpedantic -Wno-unused-value && ./main

sdl:
	clear && gcc -o main grid.c vec.o -O2 -g -lm -std=c11 -Wall -Wextra -Wshadow -Wpedantic -Wno-unused-value `sdl2-config --cflags --libs` -lSDL2_gfx && ./main
