#ifndef GAME_H
#define GAME_H

#include "doub_link_list.h"
#include "quadtree.h"
#include "vec.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

/* typedef struct grid_t */
/* { */
/* 	vec_point_t v; */
/* } grid_t; */

/* struct Grid */
/* { */
/* 	grid_t grids[GRID_ROW][GRID_COL]; */
/* 	SDL_Rect rect_array[GRID_LENGTH]; */
/* }; */

typedef struct game_t
{
	enum state_t state;
	/* struct Grid grid; */
	struct QuadTree* qt;
} game_t;

void initGame(game_t* game)
{
	/* for(uint16_t i = 0U; i < GRID_ROW; ++i) */
	/* 	for(uint16_t j = 0U; j < GRID_COL; ++j) */
	/* 	{ */
	/* 		vec_init(&game->grid.grids[i][j].v); */
	/* 		vec_reserve(&game->grid.grids[i][j].v, 4U); */
	/* 		game->grid.rect_array[j + i * GRID_COL] = */
	/* 			(SDL_Rect){j * GRID_W, i * GRID_H, GRID_W, GRID_H}; */
	/* 	} */

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
	/* for(uint16_t i = 0U; i < GRID_ROW; ++i) */
	/* 	for(uint16_t j = 0U; j < GRID_COL; ++j) */
	/* 		vec_deinit(&game->grid.grids[i][j].v); */

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

