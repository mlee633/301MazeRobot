#include "algo.h"
#include "defs.h"
#include "queue.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

#define IDX_POINT(map, p) ((map)[(p).y][(p).x])

#define LEFT(p) ((Point){.x = (p).x - 1, .y = (p).y})
#define RIGHT(p) ((Point){.x = (p).x + 1, .y = (p).y})
#define ABOVE(p) ((Point){.x = (p).x, .y = (p).y - 1})
#define BELOW(p) ((Point){.x = (p).x, .y = (p).y + 1})

#define LEFT_FLAG (1 << 4)
#define RIGHT_FLAG (1 << 5)
#define UP_FLAG (1 << 6)
#define DOWN_FLAG (1 << 7)

// INPUTS
// map:
//   Outer pointer points to rows
//   Inner pointer points to each column inside the row
// result:
//   The output of your path should be put here. It is
//   an array of MAX_PATH_SIZE points.
//
// RETURNS:
//  The length of the path
int run_algo(uint8_t map[MAP_HEIGHT][MAP_WIDTH], Point start, Point end,
             Point *result) {
  // The following algorithm finds a random possible path
  // uses the result array as a stack

  // Setup variables
  static uint8_t visitAndParentMap[MAP_HEIGHT][MAP_WIDTH];
  static Point
      queueMem[MAP_HEIGHT * MAP_WIDTH]; // Upper limit for max num vals in queue
  PointQueue queue;

  // Initialise
  memset(visitAndParentMap, 0, sizeof(visitAndParentMap));
  IDX_POINT(visitAndParentMap, start) |= 1;
  point_queue_init(&queue, queueMem, 512);
  enqueue_point(&queue, start);

  // Perform BFS
  Point curr;
  while (point_queue_size(&queue) != 0) {
    curr = dequeue_point(&queue);
    assert(IDX_POINT(map, curr) != POINT_WALL);

    if (point_eq(curr, end)) {
      break;
    }

    // clang-format off
    if (curr.x - 1 >= 0 
				&& !IDX_POINT(map, LEFT(curr)) 
				&& !IDX_POINT(visitAndParentMap, LEFT(curr))) {
			IDX_POINT(visitAndParentMap, LEFT(curr)) |= RIGHT_FLAG | 1;
			enqueue_point(&queue, LEFT(curr));
    }

    if (curr.x + 1 < MAP_WIDTH 
				&& !IDX_POINT(map, RIGHT(curr)) 
				&& !IDX_POINT(visitAndParentMap, RIGHT(curr))) {
			IDX_POINT(visitAndParentMap, RIGHT(curr)) |= LEFT_FLAG | 1;
			enqueue_point(&queue, RIGHT(curr));
    }

    if (curr.y - 1 >= 0 
				&& !IDX_POINT(map, ABOVE(curr))
				&& !IDX_POINT(visitAndParentMap, ABOVE(curr))) {
			IDX_POINT(visitAndParentMap, ABOVE(curr)) |= DOWN_FLAG | 1;
			enqueue_point(&queue, ABOVE(curr));
    }

    if (curr.y + 1 < MAP_HEIGHT 
				&& !IDX_POINT(map, BELOW(curr))
				&& !IDX_POINT(visitAndParentMap, BELOW(curr))) {
			IDX_POINT(visitAndParentMap, BELOW(curr)) |= UP_FLAG | 1;
			enqueue_point(&queue, BELOW(curr));
    }
    // clang-format on
  }

  if (point_eq(curr, end)) {
    // Calculate the size of the path
    int pathSize = 0;
    for (pathSize = 0; pathSize < MAX_PATH_SIZE; pathSize++) {
      uint8_t dirFlags = IDX_POINT(visitAndParentMap, curr);
      assert(dirFlags & 1); /* Assert that we have visited a node */
      if (dirFlags & LEFT_FLAG)
        curr = LEFT(curr);
      else if (dirFlags & RIGHT_FLAG)
        curr = RIGHT(curr);
      else if (dirFlags & UP_FLAG)
        curr = ABOVE(curr);
      else if (dirFlags & DOWN_FLAG)
        curr = BELOW(curr);
      else /* If there any direction flags, we must have reached the start */
        break;
    }

    // This means we didn't reach the end of the path in time
    assert(pathSize != MAX_PATH_SIZE);
    curr = end;
    pathSize++;
    for (int i = 0; i <= pathSize; i++) {
      result[pathSize - 1 - i] = curr;
      uint8_t dirFlags = IDX_POINT(visitAndParentMap, curr);
      if (dirFlags & LEFT_FLAG)
        curr = LEFT(curr);
      else if (dirFlags & RIGHT_FLAG)
        curr = RIGHT(curr);
      else if (dirFlags & UP_FLAG)
        curr = ABOVE(curr);
      else if (dirFlags & DOWN_FLAG)
        curr = BELOW(curr);
      else
        break;
    }

    return pathSize;
  }

  return 0;
}
