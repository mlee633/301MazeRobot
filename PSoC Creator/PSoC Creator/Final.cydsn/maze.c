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

#include "maze.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define HEURISTIC_WEIGHT 10

// Macros that allow easy creation of adjacent points
// i.e. LEFT(start), will get you the position that is
// left from the starting point.
#define LEFT(p) ((Point){.x = (p).x - 1, .y = (p).y})
#define RIGHT(p) ((Point){.x = (p).x + 1, .y = (p).y})
#define ABOVE(p) ((Point){.x = (p).x, .y = (p).y - 1})
#define BELOW(p) ((Point){.x = (p).x, .y = (p).y + 1})

#define FLAG_180_EXPECT_LEFT (1 << 0)
#define FLAG_180_EXPECT_RIGHT (1 << 1)

#define SQUARE_WIDTH_CM (12.5f)
#define SQUARE_HEIGHT_CM (8.0f)

typedef enum RobotDirection {
  ROBOT_INVALID_DIR,
  ROBOT_UP,
  ROBOT_RIGHT,
  ROBOT_DOWN,
  ROBOT_LEFT,
} RobotDirection;

static Action actionList[256];
static size_t actionCount = 0;

Action *GetActionList() { return actionList; }

size_t GetActionCount() { return actionCount; }

RobotDirection GetDirectionFromPoints(Point first, Point second) {
  int8_t xdiff = second.x - first.x;
  int8_t ydiff = second.y - first.y;
  if (xdiff == 1)
    return ROBOT_RIGHT;
  else if (xdiff == -1)
    return ROBOT_LEFT;

  if (ydiff == 1)
    return ROBOT_DOWN;
  else if (ydiff == -1)
    return ROBOT_UP;

  return ROBOT_INVALID_DIR;
}
// helper function to generate turns
static void CreateTurnAction(const uint8_t map[MAP_HEIGHT][MAP_WIDTH],
                             Point curr, Point next,
                             RobotDirection currDirection,
                             Point lastActionPos) {
  RobotDirection nextDir = GetDirectionFromPoints(curr, next);
  if (nextDir == currDirection) {
    return;
  }

  // Calculate clockwise distance
  int diff = nextDir - currDirection;
  if (diff == 1 || diff == -3)
    actionList[actionCount++] = (Action){ACTION_TURN_RIGHT, -1, 0};
  else if (diff == -1 || diff == 3)
    actionList[actionCount++] = (Action){ACTION_TURN_LEFT, -1, 0};
  else {
    actionList[actionCount] = (Action){ACTION_180, -1, 0};
    switch (currDirection) {
    case ROBOT_UP:
      if (curr.x >= 1 && map[curr.y][curr.x - 1] == 0)
        actionList[actionCount].flags180 |= FLAG_180_EXPECT_LEFT;
      if (curr.x + 1 < MAP_WIDTH && map[curr.y][curr.x + 1] == 0)
        actionList[actionCount].flags180 |= FLAG_180_EXPECT_RIGHT;

      break;
    case ROBOT_DOWN:
      if (curr.x >= 1 && map[curr.y][curr.x - 1] == 0)
        actionList[actionCount].flags180 |= FLAG_180_EXPECT_RIGHT;
      if (curr.x + 1 < MAP_WIDTH && map[curr.y][curr.x + 1] == 0)
        actionList[actionCount].flags180 |= FLAG_180_EXPECT_LEFT;

      break;

    case ROBOT_LEFT:
      if (curr.y >= 1 && map[curr.y - 1][curr.x] == 0)
        actionList[actionCount].flags180 |= FLAG_180_EXPECT_RIGHT;
      if (curr.y + 1 < MAP_HEIGHT && map[curr.y + 1][curr.x] == 0)
        actionList[actionCount].flags180 |= FLAG_180_EXPECT_LEFT;

    case ROBOT_RIGHT:
      if (curr.y >= 1 && map[curr.y - 1][curr.x] == 0)
        actionList[actionCount].flags180 |= FLAG_180_EXPECT_LEFT;
      if (curr.y + 1 < MAP_HEIGHT && map[curr.y + 1][curr.x] == 0)
        actionList[actionCount].flags180 |= FLAG_180_EXPECT_RIGHT;

      break;

    default:
      break;
    }

    if (actionList[actionCount].flags180 == 0) {
      // Edit previous command to make it go a certain distance
      // instead of waiting for an intersection
      actionList[actionCount].distance =
          abs(lastActionPos.y - curr.y) * SQUARE_HEIGHT_CM +
          abs(lastActionPos.x - curr.x) * SQUARE_WIDTH_CM;
    }

    actionCount++;
  }
}

static void CreateIntersectionAction(const uint8_t map[MAP_HEIGHT][MAP_WIDTH],
                                     Point curr, RobotDirection currDirection,
                                     ActionType type) {
  // We're going straight so we need to detect
  // if there is an intersection we need to ignore
  if (curr.x >= 1 && map[curr.y][curr.x - 1] == 0 &&
      currDirection != ROBOT_LEFT && currDirection != ROBOT_RIGHT)
    actionList[actionCount++] = (Action){type, -1, 0};
  else if (curr.x + 1 < MAP_WIDTH && map[curr.y][curr.x + 1] == 0 &&
           currDirection != ROBOT_LEFT && currDirection != ROBOT_RIGHT)
    actionList[actionCount++] = (Action){type, -1, 0};
  else if (curr.y >= 1 && map[curr.y - 1][curr.x] == 0 &&
           currDirection != ROBOT_DOWN && currDirection != ROBOT_UP)
    actionList[actionCount++] = (Action){type, -1, 0};
  else if (curr.y + 1 < MAP_HEIGHT && map[curr.y + 1][curr.x] == 0 &&
           currDirection != ROBOT_DOWN && currDirection != ROBOT_UP)
    actionList[actionCount++] = (Action){type, -1, 0};
}

bool GenerateActionList(const uint8_t map[MAP_HEIGHT][MAP_WIDTH], Point start,
                        Point *food, int foodCount) {
  Point path[256];
  Point lastFoodPosition = start;
  RobotDirection currDirection = ROBOT_INVALID_DIR;
  bool firstRun = true;

  Point lastActionPoint;

  for (int foodIndex = 0; foodIndex < foodCount; foodIndex++) {
    Point currFood = food[foodIndex];
    AlgoResult result = RunAStar(map, lastFoodPosition, currFood, path);

    // Get the starting direction if this is the first run
    if (firstRun) {
      if (result.pathLength < 2)
        return false;
      currDirection = GetDirectionFromPoints(path[0], path[1]);
    }

    // Now we can deal with our path
    for (int i = 1; i < result.pathLength; i++) {
      RobotDirection nextDir = GetDirectionFromPoints(path[i - 1], path[i]);
      Point curr = path[i - 1];

      size_t preGenSize = actionCount;
      if (nextDir != currDirection) {
        CreateTurnAction(map, curr, path[i], currDirection, lastActionPoint);
        currDirection = nextDir;
      } else {
        CreateIntersectionAction(map, curr, currDirection, ACTION_IGNORE_INTERSECTION);
      }

      // We generated an action,
      // update the last action location
      if (preGenSize != actionCount) {
        lastActionPoint = curr;
      }
    }
    lastFoodPosition = path[result.pathLength - 1];

    // Always set first run to false
    firstRun = false;
  }

  size_t preGenSize = actionCount;
  CreateIntersectionAction(map, lastFoodPosition, currDirection, ACTION_STOP);
  if(preGenSize == actionCount) {
    actionList[actionCount] = (Action){ACTION_STOP, -1, 0};
    actionList[actionCount++].distance =
          abs(lastActionPoint.y - lastFoodPosition.y) * SQUARE_HEIGHT_CM +
          abs(lastActionPoint.x - lastFoodPosition.x) * SQUARE_WIDTH_CM;
  }

  return true;
}

int8_t Heuristic(Point start, Point end) {
  int8_t xdiff = end.x - start.x;
  if (xdiff < 0)
    xdiff = -xdiff;

  int8_t ydiff = end.y - start.y;
  if (ydiff < 0)
    ydiff = -ydiff;

  return xdiff + ydiff;
}

AlgoResult RunAStar(const uint8_t map[MAP_HEIGHT][MAP_WIDTH], Point start,
                    Point food, Point *result) {

  // Calculate heuristic values based on the goal
  // 2280 bytes

  // Initialise distance, priority and visited arrays
  static int8_t distance[MAP_HEIGHT][MAP_WIDTH];
  // 2280 bytes
  static int8_t priorities[MAP_HEIGHT][MAP_WIDTH];
  // 1140 bytes
  static int8_t previous[MAP_HEIGHT][MAP_WIDTH];
  // 285 bytes
  static bool visited[MAP_HEIGHT][MAP_WIDTH];

  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      distance[y][x] = 127;
      priorities[y][x] = 127;
      visited[y][x] = false;
    }
  }

  // Turn start and end point into coordinates
  // TODO: Figure out which order to get to the food in
  distance[start.y][start.x] = 0;
  priorities[start.y][start.x] = Heuristic(start, food);

  // While there are nodes left to visit
  while (1) {

    // Find lowest priority node
    int8_t lowestPriority = 127;
    Point next = {.x = -1, .y = -1};
    for (int8_t y = 0; y < MAP_HEIGHT; y++) {
      for (int8_t x = 0; x < MAP_WIDTH; x++) {
        if (priorities[y][x] < lowestPriority && !visited[y][x]) {
          lowestPriority = priorities[y][x];
          next = (Point){.x = x, .y = y};
        }
      }
    }

    if (next.x == -1 || next.y == -1) {
      // Node not found
      return (AlgoResult){.pathLength = -1};
    }

    if (point_eq(next, food)) {
      // Goal found - return path length and path

      int pathLength = 0;
      Point current = next;

      static Point tempPath[MAP_HEIGHT * MAP_WIDTH];

      while (current.x != start.x || current.y != start.y) {
        tempPath[pathLength++] = current;
        int prev_direction = previous[current.y][current.x];

        if (prev_direction == 0) { // Move right
          current.x++;
        } else if (prev_direction == 1) { // Move left
          current.x--;
        } else if (prev_direction == 2) { // Move down
          current.y++;
        } else if (prev_direction == 3) { // Move up
          current.y--;
        }
      }

      tempPath[pathLength++] = start;

      // Reverse order of path into out
      for (int i = 0; i < pathLength; i++) {
        result[i] = tempPath[pathLength - 1 - i];
      }

      return (AlgoResult){.pathLength = pathLength};
    }

    // Update distances and priorities for neighboring nodes
    static const int8_t dx[] = {-1, 1, 0, 0}; // left, right, up, down
    static const int8_t dy[] = {0, 0, -1, 1};

    for (int8_t i = 0; i < 4; i++) {
      Point neighbour = {.x = next.x + dx[i], .y = next.y + dy[i]};

      if (neighbour.x >= 0 && neighbour.x < MAP_WIDTH && neighbour.y >= 0 &&
          neighbour.y < MAP_HEIGHT && map[neighbour.y][neighbour.x] == 0) {

        int8_t newDistance = distance[next.y][next.x] + 1;

        if (newDistance < distance[neighbour.y][neighbour.x]) {
          distance[neighbour.y][neighbour.x] = newDistance;
          priorities[neighbour.y][neighbour.x] =
              newDistance + Heuristic(neighbour, food);
          previous[neighbour.y][neighbour.x] =
              i; // Store the direction we came from
        }
      }
    }

    visited[next.y][next.x] = true;
  }
}
// Don't need to modify this
bool point_eq(Point a, Point b) { return a.x == b.x && a.y == b.y; }
