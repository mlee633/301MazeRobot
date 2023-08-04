#ifndef _ALGO_H
#define _ALGO_H

#include "defs.h"
#include <stdbool.h>
#include <stddef.h>

size_t run_algo(uint8_t map[MAP_HEIGHT][MAP_WIDTH],
                Point start,
                Point *ends,
                size_t endCount,
                Point *result,
                size_t resultCount,
                size_t *maxInternalMem);

#endif // _ALGO_H
