#include "algo.h"
#include "defs.h"
#include <assert.h>
#include <raylib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GRID_SCALE 30
#define T(x) ((x)*GRID_SCALE)
#define COUNT_OF(x)                                                                    \
  ((sizeof(x) / sizeof(0 [x])) / ((size_t)(!(sizeof(x) % sizeof(0 [x])))))

static uint8_t map[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

/* static uint8_t map[MAP_HEIGHT][MAP_WIDTH] = { */
/*     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, */
/*     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, */
/*     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, */
/*     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, */
/*     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, */
/*     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, */
/*     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, */
/*     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, */
/*     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, */
/*     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, */
/*     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, */
/*     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, */
/*     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, */
/*     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, */
/*     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, */
/* }; */

static Point food_list[] = {{1, 9}, {5, 5}, {7, 1}, {13, 5}, {9, 9}};

// Pre-declaration for functions defined later on
void visualise_map(uint8_t map[MAP_HEIGHT][MAP_WIDTH], int startx, int starty);
void draw_path(Point *path, int pathLength, int startx, int starty);
AlgoResult find_full_path(Point start, Point *out, size_t outCap);

int main(int argc, char **argv) {

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(40 + MAP_WIDTH * GRID_SCALE,
             60 + MAP_HEIGHT * GRID_SCALE,
             "Robot Visualiser - 301");

  static const Point start = (Point){.x = 7, .y = MAP_HEIGHT - 2};
  static Point pathArray[128];

  // Find the quickest path to the first piece of food
  AlgoResult result = find_full_path(start, pathArray, COUNT_OF(pathArray));

  // Event loop
  while (!WindowShouldClose()) {
    ClearBackground(BLACK);

    BeginDrawing();

    visualise_map(map, 20, 20);

    // Draw start and end points
    DrawCircle(T(start.x) + 20 + 5, T(start.y) + 20 + 5, 2, ORANGE);
    for (int i = 0; i < COUNT_OF(food_list); i++) {
      Point food = food_list[i];
      DrawCircle(T(food.x) + 20 + 5, T(food.y) + 20 + 5, 2, GREEN);
    }
    // Display path length
    char buffer[255];
    snprintf(buffer,
             sizeof(buffer),
             "Path Length: %ld, Memory Usage: %ld bytes",
             result.pathLength,
             result.stats.maxMemUsage);
    DrawText(buffer, 20, 40 + T(MAP_HEIGHT), 15, WHITE);

    draw_path(pathArray, result.pathLength, 20, 20);

    EndDrawing();
  }
}

void visualise_map(uint8_t map[MAP_HEIGHT][MAP_WIDTH], int sx, int sy) {
  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      if (map[y][x])
        DrawRectangle(T(x) + sx, T(y) + sy, GRID_SCALE, GRID_SCALE, WHITE);
      else
        DrawRectangleLines(T(x) + sx, T(y) + sy, GRID_SCALE, GRID_SCALE, WHITE);
    }
  }
}

void draw_path(Point *path, int pathLength, int sx, int sy) {

  Color drawColor;
  for (int i = 0; i < (pathLength - 1); i++) {
    Point curr = path[i];
    Point next = path[i + 1];

    float percent = (((float)i) / ((float)pathLength));
    drawColor = (Color){
        .a = 150,
        .g = (unsigned char)(255 * percent),
        .b = 255,
    };

    DrawLine(T(curr.x) + sx + GRID_SCALE / 2,
             T(curr.y) + sy + GRID_SCALE / 2,
             T(next.x) + sx + GRID_SCALE / 2,
             T(next.y) + sy + GRID_SCALE / 2,
             drawColor);

    DrawCircle(
        T(curr.x) + sx + GRID_SCALE / 2, T(curr.y) + sy + GRID_SCALE / 2, 3, drawColor);
  }

  // If we have a non-zero length path, draw the
  // last circle
  if (pathLength > 0) {
    Point last = path[pathLength - 1];
    DrawCircle(
        T(last.x) + sx + GRID_SCALE / 2, T(last.y) + sy + GRID_SCALE / 2, 3, drawColor);
  }
}

AlgoResult find_full_path(Point start, Point *out, size_t outCap) {
  Point food[COUNT_OF(food_list)];
  size_t foodLeft = COUNT_OF(food);
  memcpy(food, food_list, sizeof(food_list));

  AlgoResult result = {
      .pathLength = 0,
#ifdef TRACK_STATS
      .stats.maxMemUsage = 0,
#endif
  };

  *out = start;
  while (foodLeft > 0) {
    AlgoResult runResult = run_algo(map, *out, food, foodLeft, out, outCap);
		assert(outCap >= runResult.pathLength);

    Point last = out[runResult.pathLength - 1];
    bool found = false;
    for (int i = 0; i < foodLeft; i++) {
      if (point_eq(food[i], last)) {
        Point temp = food[foodLeft - 1];
        food[foodLeft - 1] = food[i];
        food[i] = temp;
        found = true;
        break;
      }
    }

    printf("Food Left: %ld\n", foodLeft);
    fflush(stdout);
    assert(found);

    result.pathLength += runResult.pathLength - 1;
    out += runResult.pathLength - 1;
    outCap -= runResult.pathLength - 1;
    foodLeft -= 1;

#ifdef TRACK_STATS
    if (runResult.stats.maxMemUsage > result.stats.maxMemUsage)
      result.stats.maxMemUsage = runResult.stats.maxMemUsage;
#endif
  }

  result.pathLength++;
  return result;
}

uint8_t point_eq(Point a, Point b) { return a.x == b.x && a.y == b.y; }
