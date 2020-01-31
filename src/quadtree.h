#ifndef QUADTREE_H
#define QUADTREE_H

#include "vec.h"

#include <SDL2/SDL_shape.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifndef BUCKET_SIZE
#	define BUCKET_SIZE 4
#endif

typedef SDL_FPoint Point;
typedef SDL_FRect Rect;
typedef struct
{
	float x, y;
	float r;
} Circle;

typedef vec_t(Point) vec_p_t;
typedef vec_t(Point) vec_point_t;

Point makePoint(float x, float y)
{
	return (Point){x, y};
}

Rect makeRect(const float x, const float y, const float w, const float h)
{
	Rect temp;
	temp.x = x;
	temp.y = y;
	temp.w = w;
	temp.h = h;
	return temp;
}

float dist(float x, float y, float z, float w)
{
	return sqrtf((x - z) * (x - z) + (y - w) * (y - w));
}

bool pointInRect(const Point* const p, const Rect* const r)
{
	return ((p->x >= r->x) && (p->x < r->x + r->w) && (p->y >= r->y) &&
			(p->y < r->y + r->h));
}

bool pointInCircle(const Point* const p, const Circle* const c)
{
	return (dist(p->x, p->y, c->x, c->y) < c->r);
}

bool intersects(const Rect* const r1, const Rect* const r2)
{
	if(r1->x + r1->w < r2->x)	 // r1 is left of r2
		return false;
	if(r1->x > r2->x + r2->w)	 // r1 is right of r2
		return false;

	if(r1->y + r1->h < r2->y)	 // r1 is above r2
		return false;
	if(r1->y > r2->y + r2->h)	 // r1 is below r2
		return false;

	return true;
}

bool intersectCircle(const Rect* const _r, const Circle* const c)
{
	const Rect temp = makeRect(_r->x + _r->w / 2.f, _r->y + _r->h / 2.f, _r->w, _r->h);
	const Rect* const r = &temp;

	Point circle_dist = makePoint(fabs(c->x - r->x), fabs(c->y - r->y));
	if((circle_dist.x > (r->w / 2.f + c->r)) || (circle_dist.y > (r->h / 2.f + c->r)))
		return false;
	if((circle_dist.x <= (r->w / 2.f)) || (circle_dist.y <= (r->h / 2.f)))
		return true;
	float corner =
		powf(circle_dist.x - r->w / 2.f, 2.f) + powf(circle_dist.y - r->h / 2.f, 2.f);
	return (corner <= (c->r * c->r));
}

typedef struct QuadTree
{
	vec_p_t points;

	Rect boundary;

	struct QuadTree* north_west;
	struct QuadTree* north_east;
	struct QuadTree* south_west;
	struct QuadTree* south_east;
} QuadTree;

bool qt_init(QuadTree** qt, const Rect rect)
{
	QuadTree* temp = (QuadTree*)calloc(1U, sizeof(struct QuadTree));
	if(temp)
	{
		temp->boundary = rect;
		vec_init(&temp->points);
		vec_reserve(&temp->points, BUCKET_SIZE);
		temp->north_west = NULL;
		temp->north_east = NULL;
		temp->south_west = NULL;
		temp->south_east = NULL;
		*qt = temp;
		return true;
	}
	return false;
}

bool qt_destroy(QuadTree* qt)
{
	if(!qt)
		return false;

	vec_deinit(&qt->points);
	if(!qt->north_west)
	{
		free(qt);
		return true;
	}
	qt_destroy(qt->north_west);
	qt_destroy(qt->north_east);
	qt_destroy(qt->south_west);
	qt_destroy(qt->south_east);
	return true;
}

bool qt_insert(QuadTree* const, const Point);

bool qt_subdivide(QuadTree* const qt)
{
	if(qt->north_west)
	{
		qt_subdivide(qt->north_west);
		qt_subdivide(qt->north_east);
		qt_subdivide(qt->south_west);
		qt_subdivide(qt->south_east);
		return false;
	}

	Rect rect = makeRect(
		qt->boundary.x, qt->boundary.y, qt->boundary.w / 2.f, qt->boundary.h / 2.f);

	qt_init(&qt->north_west, rect);
	rect.x = qt->boundary.x + qt->boundary.w / 2.f;
	qt_init(&qt->north_east, rect);
	rect.x = qt->boundary.x;
	rect.y = qt->boundary.y + qt->boundary.h / 2.f;
	qt_init(&qt->south_west, rect);
	rect.x = qt->boundary.x + qt->boundary.w / 2.f;
	rect.y = qt->boundary.y + qt->boundary.h / 2.f;
	qt_init(&qt->south_east, rect);

	// After initializing the children 4 nodes, put the data in qt->points to
	// the correct nodes.
	for(uint8_t i = 0U; i < qt->points.length; ++i)
	{
		if(qt_insert(qt->north_west, qt->points.data[i]))
		{
			vec_splice(&qt->points, i, 1);
			--i;
		}
		else if(qt_insert(qt->north_east, qt->points.data[i]))
		{
			vec_splice(&qt->points, i, 1);
			--i;
		}
		else if(qt_insert(qt->south_west, qt->points.data[i]))
		{
			vec_splice(&qt->points, i, 1);
			--i;
		}
		else if(qt_insert(qt->south_east, qt->points.data[i]))
		{
			vec_splice(&qt->points, i, 1);
			--i;
		}
	}
	return true;
}

// Either store Point* or whenever you insert a new point balance the tree,
// which sucks It should be a seperate function which the user can run or maybe
// call it in subdivide();
bool qt_insert(QuadTree* const qt, const Point p)
{
	if(!pointInRect(&p, &qt->boundary))
		return false;

	if(qt->points.length < BUCKET_SIZE && !qt->north_west)
	{
		vec_push(&qt->points, p);
		return true;
	}

	if(!qt->north_west)
	{
		if(qt_subdivide(qt))
		{
			if(qt_insert(qt->north_west, p))
				return true;
			if(qt_insert(qt->north_east, p))
				return true;
			if(qt_insert(qt->south_west, p))
				return true;
			if(qt_insert(qt->south_east, p))
				return true;
		}
	}
	else
	{
		if(qt_insert(qt->north_west, p))
			return true;
		if(qt_insert(qt->north_east, p))
			return true;
		if(qt_insert(qt->south_west, p))
			return true;
		if(qt_insert(qt->south_east, p))
			return true;
	}

	return false;
}

uint32_t count = 0U;

void qt_getPointsInRect(QuadTree* const qt, const Rect* const rect, vec_p_t* const vec)
{
	++count;
	if(!intersects(&qt->boundary, rect))
		return;

	for(uint8_t i = 0U; i < qt->points.length; ++i)
		if(pointInRect(&qt->points.data[i], rect))
			vec_push(vec, qt->points.data[i]);

	if(!qt->north_west)
		return;

	qt_getPointsInRect(qt->north_west, rect, vec);
	qt_getPointsInRect(qt->north_east, rect, vec);
	qt_getPointsInRect(qt->south_west, rect, vec);
	qt_getPointsInRect(qt->south_east, rect, vec);
}

void qt_getPointsInCircle(QuadTree* const qt,
						  const Circle* const circle,
						  vec_p_t* const vec)
{
	++count;
	// OOF TODO Add Circle-Rectangle intersection
	if(!intersectCircle(&qt->boundary, circle))
		return;

	for(uint8_t i = 0U; i < qt->points.length; ++i)
		if(pointInCircle(&qt->points.data[i], circle))
			vec_push(vec, qt->points.data[i]);

	if(!qt->north_west)
		return;

	qt_getPointsInCircle(qt->north_west, circle, vec);
	qt_getPointsInCircle(qt->north_east, circle, vec);
	qt_getPointsInCircle(qt->south_west, circle, vec);
	qt_getPointsInCircle(qt->south_east, circle, vec);
}

void qt_clear(QuadTree* const qt, uint16_t level)
{
	if(!qt || level == UINT16_MAX)	  // Somehow qt == NULL
		return;

	if(!qt->north_west)	   // If qt doesn't have a child
	{
		/* if(level)	 // Child QT */
		/* { */
		/* 	qt_destroy(qt); */
		/* } */
		/* else	// Root QT */
		vec_clear(&qt->points);
		return;
	}
	qt_clear(qt->north_west, level + 1);
	qt_clear(qt->north_east, level + 1);
	qt_clear(qt->south_west, level + 1);
	qt_clear(qt->south_east, level + 1);

	/* qt_destroy(qt->north_west); */
	/* qt_destroy(qt->north_east); */
	/* qt_destroy(qt->south_west); */
	/* qt_destroy(qt->south_east); */
}

#endif

