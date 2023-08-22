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
//	 The map to be used to run BFS on
// start:
//	 Start points for the algorithm
// ends:
//   The end points which the algorithm should search for
// endCount:
//	 The number of possible end points we have
// out:
//   The output of your path should be put here.
// outCap:
//   The capacity of result array
//   memory used by the algorithm. N.B. This should be the in-use memory.
// RETURNS:
//  The length of the path
AlgoResult run_algo(const uint8_t map[MAP_HEIGHT][MAP_WIDTH],
                    Point start,
                    const Point *ends,
                    size_t endCount,
                    Point *out,
                    size_t outCap) {
  // The following algorithm finds a random possible path
  // uses the result array as a stack
  // [50 1 10];
  //                                      have we been visited
  // Setup variables                      V
  // each point is a set of flags: 00000000
  static uint8_t visitAndParentMap[MAP_HEIGHT][MAP_WIDTH];
  static Point queueMem[32]; // Upper limit for max num vals in queue
  PointQueue queue;
  AlgoResult result;

  // Initialise
  memset(visitAndParentMap, 0, sizeof(visitAndParentMap));
  IDX_POINT(visitAndParentMap, start) |= 1;
  point_queue_init(&queue, queueMem, sizeof(queueMem) / sizeof(Point));
  enqueue_point(&queue, start);
  result = (AlgoResult){
      .pathLength = 0,
#ifdef TRACK_STATS
      .stats.maxMemUsage = 0,
#endif
  };

  // Perform BFS
  Point curr;
  while (point_queue_size(&queue) != 0) {
    curr = dequeue_point(&queue);
    assert(!IDX_POINT(map, curr)); // Assert that we're looking at an empty space

    for (int i = 0; i < endCount; i++) {
      if (point_eq(curr, ends[i])) {
        goto found;
      }
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

#ifdef TRACK_STATS
    size_t queueMem = point_queue_size(&queue) * sizeof(Point);
    result.stats.maxMemUsage =
        (queueMem > result.stats.maxMemUsage) ? queueMem : result.stats.maxMemUsage;
#endif
  }

  // If we exit out of the loop without finding
  // anything, we return a path length of 0
  result.pathLength = 0;
  return result;

found:;
  Point end = curr;
  int pathSize = 0;
  for (pathSize = 0; pathSize < outCap; pathSize++) {
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
  assert(pathSize != outCap);
  curr = end;
  pathSize++;
  for (int i = 0; i < pathSize; i++) {
    out[pathSize - 1 - i] = curr;
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

  result.pathLength = pathSize;
  return result;
}
