#include "queue.h"
#include <assert.h>

void point_queue_init(PointQueue *queue, Point *mem, size_t memSize) {
  queue->mem = mem;
  queue->memSize = memSize;
  queue->front = 0;
  queue->back = 0;
}

void enqueue_point(PointQueue *queue, Point p) {
  queue->mem[queue->back] = p;
  queue->back = (queue->back + 1) % queue->memSize;
  assert(queue->back != queue->front);
}

Point dequeue_point(PointQueue *queue) {
  assert(queue->front != queue->back);
  Point result = queue->mem[queue->front];
  queue->front = (queue->front + 1) % queue->memSize;
  return result;
}

size_t point_queue_size(PointQueue *queue) {
  if (queue->back >= queue->front)
    return queue->back - queue->front;

  return (queue->memSize - queue->front) + queue->back;
}
