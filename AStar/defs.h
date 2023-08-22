#ifndef _MAP_H
#define _MAP_H

#include <stddef.h>
#include <stdint.h>

#define TRACK_STATS 1

#define MAP_WIDTH 19
#define MAP_HEIGHT 15

typedef struct Point {
  uint8_t x, y;
} Point;

uint8_t point_eq(Point a, Point b);

#endif // _MAP_H
