#ifndef _QUEUE_H
#define _QUEUE_H

#include "defs.h"
#include <stddef.h>

typedef struct PointQueue {
  // Underlying memory to use
  Point *mem;
  // Number of points that can be accessed
  // through mem.
  size_t memSize;

  size_t front;
  size_t back;

} PointQueue;

void point_queue_init(PointQueue *queue, Point *mem, size_t memSize);
void enqueue_point(PointQueue *queue, Point p);
Point dequeue_point(PointQueue *queue);
size_t point_queue_size(PointQueue *queue);

#endif // _QUEUE_H
