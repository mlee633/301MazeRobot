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

#define FLAG_180_EXPECT_LEFT (1 << 0)
#define FLAG_180_EXPECT_RIGHT (1 << 1)

typedef enum ActionType {
  ACTION_IGNORE_INTERSECTION,
  ACTION_TURN_LEFT,
  ACTION_TURN_RIGHT,
  ACTION_180,
  ACTION_WAIT,
} ActionType;

typedef struct Action {
  ActionType type;
  // distance == -1 if we continue until next
  // intersection. Otherwise, distance = num cms
  // to travel.
  int8_t distance;
  // contains information about surrounding paths when we do
  // a 180. If the FLAG_180_EXPECT_LEFT bit is set, we should expect a left path
  // when we do a 180 If the FLAG_
  uint8_t flags180;
} Action;

typedef struct Point {
    int8_t x, y;
} Point;

typedef struct AlgoResult {
  size_t pathLength;
    
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

bool GenerateActionList(const uint8_t map[MAP_HEIGHT][MAP_WIDTH], Point start,
                        Point *food, int foodCount);

Action* GetActionList();
size_t GetActionCount();

bool point_eq(Point a, Point b);
