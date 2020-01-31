#ifndef GAME_H
#define GAME_H

#include "quadtree.h"
#include "vec.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH  (1280)
#define HEIGHT (720)

#define GRID_W (40)
#define GRID_H (40)

#define GRID_ROW (HEIGHT / GRID_H)
#define GRID_COL (WIDTH / GRID_W)

#define GRID_LENGTH (GRID_COL * GRID_ROW)

enum state_t
{
	RUNNING,
	QUITTING,
	INPUT,
	OUTPUT
};

SDL_Point mouse;

typedef struct game_t
{
	enum state_t state;
	/* struct Grid grid; */
	struct QuadTree* qt;
} game_t;

void initGame(game_t* game)
{
	game->state = RUNNING;
	game->qt = NULL;
	// TODO Change the first HEIGHT to WIDTH
	Uint32 start_time = SDL_GetTicks();
	const Rect qt_rect = makeRect(0.f, 0.f, WIDTH, HEIGHT);
	if(!qt_init(&game->qt, qt_rect))
	{
		fprintf(stderr, "Couldn't Create QuadTree\n");
		exit(1);
	}
	printf("qt_init() took %u milliseconds\n", SDL_GetTicks() - start_time);
}

void deinitGame(game_t* game)
{
	game->state = QUITTING;
	qt_destroy(game->qt);
}

#define POINT_COUNT 1024U
SDL_FPoint points[POINT_COUNT];

void generatePoints(void)
{
	for(uint16_t i = 0; i < POINT_COUNT; ++i)
	{
		points[i] = (SDL_FPoint){rand() % WIDTH + 0.f, rand() % HEIGHT + 0.f};
	}
}

void randomizePoints(void)
{
	for(uint16_t i = 0; i < POINT_COUNT; ++i)
	{
		points[i].x = rand() % WIDTH;
		points[i].y = rand() % HEIGHT;
	}
}

#endif

