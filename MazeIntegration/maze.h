/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

/* [] END OF FILE */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAP_WIDTH 19
#define MAP_HEIGHT 15

typedef struct Point {
    int8_t x, y;
} Point;

typedef struct AlgoResult {
  int pathLength;
    
#ifdef TRACK_STATS
  struct Stats {
    size_t maxMemUsage;
  } stats;
#endif
} AlgoResult;

AlgoResult RunAStar(const uint8_t map[MAP_HEIGHT][MAP_WIDTH],
                    Point start,
                    Point food,
                    Point *result);

bool point_eq(Point a, Point b);
