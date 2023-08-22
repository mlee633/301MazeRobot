#include "algo.h"
#include "defs.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#define HEURISTIC_WEIGHT 10

// Macros that allow easy creation of adjacent points
// i.e. LEFT(start), will get you the position that is
// left from the starting point.
#define LEFT(p) ((Point){.x = (p).x - 1, .y = (p).y})
#define RIGHT(p) ((Point){.x = (p).x + 1, .y = (p).y})
#define ABOVE(p) ((Point){.x = (p).x, .y = (p).y - 1})
#define BELOW(p) ((Point){.x = (p).x, .y = (p).y + 1})

void calculate_heuristic(double heuristic[MAP_HEIGHT][MAP_WIDTH], const Point goal) {
  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      double dx = goal.x - x;
      double dy = goal.y - y;
      heuristic[y][x] = HEURISTIC_WEIGHT * sqrt(dx * dx + dy * dy);
    }
  }
}

// INPUTS
// map:
//	 The map to be used to run BFS on
// start:
//	 Start points for the algorithm
// ends:
//   The end points which the algorithm should search for
// endCount:
//	 The number of possible end points we have (size of array ends points to)
// out:
//   Where your final path should be written. It is an array.
//
// RETURNS:
//  The length of the path
AlgoResult run_algo(const uint8_t map[MAP_HEIGHT][MAP_WIDTH],
                    Point start,
                    const Point *ends,
                    size_t endCount,
                    Point *out) {

  // Calculate heuristic values based on the goal
  // 2280 bytes
  double heuristic[MAP_HEIGHT][MAP_WIDTH];
  calculate_heuristic(heuristic, ends[0]);

  // Initialise distance, priority and visited arrays
  int distance[MAP_HEIGHT][MAP_WIDTH];
  // 2280 bytes
  double priorities[MAP_HEIGHT][MAP_WIDTH];
  // 1140 bytes
  int previous[MAP_HEIGHT][MAP_WIDTH];
  // 285 bytes
  bool visited[MAP_HEIGHT][MAP_WIDTH];

  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      distance[y][x] = INT_MAX;
      priorities[y][x] = INFINITY;
      visited[y][x] = false;
    }
  }

  // Turn start and end point into coordinates
  int start_x = start.x;
  int start_y = start.y;
  int goal_x = ends[0].x;
  int goal_y = ends[0].y;

  distance[start_y][start_x] = 0;
  priorities[start_y][start_x] = heuristic[start_y][start_x];


  // While there are nodes left to visit
  while (1) {
  
    // Find lowest priority node
    double lowestPriority = INFINITY;
    int lowestPriority_x = -1;
    int lowestPriority_y = -1;
    for (int y = 0; y < MAP_HEIGHT; y++) {
      for (int x = 0; x < MAP_WIDTH; x++) {
        if (priorities[y][x] < lowestPriority && !visited[y][x]) {
          lowestPriority = priorities[y][x];
          lowestPriority_x = x;
          lowestPriority_y = y;          
        }   
      }
    
    }
  
    
    if (lowestPriority_x == -1) {
        // Node not found
      return(AlgoResult){.pathLength = -1};
    } else if (lowestPriority_x == goal_x && lowestPriority_y == goal_y) {
        // Goal found - return path length and path
      int pathLength = 0;
      int current_x = goal_x;
      int current_y = goal_y;

      Point tempPath[MAP_HEIGHT * MAP_WIDTH];

      while (current_x != start_x || current_y != start_y) {
        tempPath[pathLength++] = (Point){.x = current_x, .y = current_y};
        int prev_direction = previous[current_y][current_x];

        if (prev_direction == 0) { // Move left
          current_x++;
        } else if (prev_direction == 1) { // Move right
          current_x--;
        } else if (prev_direction == 2) { // Move up
          current_y++;
        } else if (prev_direction == 3) { // Move down
          current_y--;
        }
      }
    
      tempPath[pathLength++] = start;

      // Reverse order of path into out
      for (int i = 0; i < pathLength; i++) {
        out[i] = tempPath[pathLength - 1 - i];
      }
    
      return (AlgoResult){.pathLength = pathLength};

    } else {
      // Update distances and priorities for neighboring nodes
      int dx[] = {-1, 1, 0, 0}; // left, right, up, down
      int dy[] = {0, 0, -1, 1};

      for (int i = 0; i < 4; i++) {
        int neighbor_x = lowestPriority_x + dx[i];
        int neighbor_y = lowestPriority_y + dy[i];

        if (neighbor_x >= 0 && neighbor_x < MAP_WIDTH && neighbor_y >= 0 &&
            neighbor_y < MAP_HEIGHT && map[neighbor_y][neighbor_x] == 0) {

          int newDistance = distance[lowestPriority_y][lowestPriority_x] + 1;

          if (newDistance < distance[neighbor_y][neighbor_x]) {
            distance[neighbor_y][neighbor_x] = newDistance;
            priorities[neighbor_y][neighbor_x] =
                newDistance + heuristic[neighbor_y][neighbor_x];
            previous[neighbor_y][neighbor_x] = i; // Store the direction
          }
        }
      }

      visited[lowestPriority_y][lowestPriority_x] = true;
    }
  
  
  }

}
// Don't need to modify this
  uint8_t point_eq(Point a, Point b) { return a.x == b.x && a.y == b.y; }
