#include "game.h"

// Define an SDL_FRect vector
typedef vec_t(SDL_FRect) vec_frect_t;

static const Rect QT_RECT = {0.f, 0.f, WIDTH, HEIGHT};

// Function to get the rectangular boundaries from QT and put them into a
// SDL_FRect vector
void getFRect(vec_frect_t* const vec, QuadTree* const qt)
{
	if(!qt)
		return;

	vec_push(vec, qt->boundary);

	if(!qt->north_west)
		return;

	getFRect(vec, qt->north_west);
	getFRect(vec, qt->north_east);
	getFRect(vec, qt->south_west);
	getFRect(vec, qt->south_east);
}

void insertPointsIntoQT(QuadTree* const qt, const SDL_FPoint* const point_array)
{
	for(uint32_t i = 0U; i < POINT_COUNT; ++i)
	{
		qt_insert(qt, point_array[i]);
	}
}

Uint32 getFPS(Uint32 start, Uint32 end)
{
	return 1000 / (end - start);
}

int main(void)
{
	printf("\033[3J\033[2J\033[H");

	if(SDL_Init(SDL_INIT_VIDEO))
	{
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	game_t game;
	initGame(&game);
	generatePoints();

	vec_frect_t qt_frect_vec;
	vec_init(&qt_frect_vec);
	vec_reserve(&qt_frect_vec, 1024U);

	vec_p_t queried_points;
	vec_init(&queried_points);
	vec_reserve(&queried_points, 64U);

	Uint16 fps = 0U;
	Uint64 frame_count = 0ULL;
	Uint32 fps_timer = SDL_GetTicks();

	// Query boundary
	Circle query_circle = (Circle){WIDTH / 2.f, HEIGHT / 2.f, 125};

	bool draw_grid = true;

	SDL_Window* window = SDL_CreateWindow("Grid Particle System",
										  SDL_WINDOWPOS_UNDEFINED,
										  SDL_WINDOWPOS_UNDEFINED,
										  WIDTH,
										  HEIGHT,
										  SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS);
	if(!window)
	{
		fprintf(stderr, "Couldn't create window: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_Renderer* render = SDL_CreateRenderer(
		window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

	if(!render)
	{
		fprintf(stderr, "Couldn't create renderer: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		exit(1);
	}

	SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);

	while(game.state != QUITTING)
	{
		SDL_SetRenderDrawColor(render, 12U, 12U, 32U, 255U);
		SDL_RenderClear(render);

		fps_timer = SDL_GetTicks();
		++frame_count;

		SDL_Event e;
		while(SDL_PollEvent(&e))
		{
			switch(e.type)
			{
				case SDL_QUIT: game.state = QUITTING; break;
				case SDL_KEYDOWN:
					switch(e.key.keysym.sym)
					{
						case SDLK_ESCAPE:
						case SDLK_q: game.state = QUITTING; break;
						default: break;
					}
					break;
				case SDL_KEYUP:
					switch(e.key.keysym.sym)
					{
						case SDLK_r:
							randomizePoints();
							vec_clear(&queried_points);
							break;
						case SDLK_g: draw_grid = !draw_grid; break;
					}
					break;
				case SDL_MOUSEMOTION: mouse = (SDL_Point){e.motion.x, e.motion.y}; break;
				default: break;
			}
		}

		for(uint16_t i = 0U; i < POINT_COUNT; ++i)
		{
			filledCircleRGBA(render, points[i].x, points[i].y, 2U, 255U, 24U, 24U, 222U);
		}

		if(draw_grid)
		{
			SDL_SetRenderDrawColor(render, 183U, 183U, 64U, 99U);
			SDL_RenderDrawRectsF(render, qt_frect_vec.data, qt_frect_vec.length);
		}

		// Draw a circle at the mouse as the query boundary
		// TODO Add life to this part
		// Make every particle check every other particle

		// Clear the vector and the quadtree, insert points every frame.
		// FIXME Now it just deinits the qt and inits again, make a qt_clear() function
		// which leaves the first node intact, deletes just the children
		qt_destroy(game.qt);
		qt_init(&game.qt, QT_RECT);
		vec_clear(&qt_frect_vec);

		insertPointsIntoQT(game.qt, points);
		getFRect(&qt_frect_vec, game.qt);
		query_circle.x = mouse.x;
		query_circle.y = mouse.y;
		filledCircleRGBA(render, query_circle.x, query_circle.y, 5U, 8U, 236U, 18U, 204U);

		vec_clear(&queried_points);
		qt_getPointsInCircle(game.qt, &query_circle, &queried_points);
		for(int j = 0U; j < queried_points.length; ++j)
		{
			Point diff = makePoint(mouse.x - queried_points.data[j].x,
								   mouse.y - queried_points.data[j].y);
			applyForce(&queried_points.data[j], &diff);
		}

		SDL_RenderPresent(render);

		// Show FPS
		if(frame_count % 6 == 0)
		{
			fps = getFPS(fps_timer, SDL_GetTicks());
			printf("\033[1HFPS:%3d\n", fps);
		}
	}

	vec_deinit(&qt_frect_vec);
	vec_deinit(&queried_points);
	deinitGame(&game);

	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
