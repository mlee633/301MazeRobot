#ifndef _ALGO_H
#define _ALGO_H

#include "defs.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct AlgoResult {
  size_t pathLength;

#ifdef TRACK_STATS
  struct Stats {
    size_t maxMemUsage;
  } stats;
#endif
} AlgoResult;

AlgoResult run_algo(const uint8_t map[MAP_HEIGHT][MAP_WIDTH],
                    Point start,
                    const Point *ends,
                    size_t endCount,
                    Point *result,
                    size_t resultCount);

#endif // _ALGO_H
