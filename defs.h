#ifndef _MAP_H
#define _MAP_H

#include <stdint.h>

#define MAP_WIDTH 19
#define MAP_HEIGHT 15

#define POINT_WALL 1
#define POINT_EMPTY 0

typedef struct Point {
  uint8_t x, y;
} Point;

uint8_t point_eq(Point a, Point b);

#endif // _MAP_H
