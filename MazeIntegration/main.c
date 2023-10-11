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

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <locale.h>
#include <wchar.h>

#define HEURISTIC_WEIGHT 10

// Macros that allow easy creation of adjacent points
// i.e. LEFT(start), will get you the position that is
// left from the starting point.
#define LEFT(p) ((Point){.x = (p).x - 1, .y = (p).y})
#define RIGHT(p) ((Point){.x = (p).x + 1, .y = (p).y})
#define ABOVE(p) ((Point){.x = (p).x, .y = (p).y - 1})
#define BELOW(p) ((Point){.x = (p).x, .y = (p).y + 1})

typedef enum ActionType
{
    ACTION_IGNORE_INTERSECTION,
    ACTION_TURN_LEFT,
    ACTION_TURN_RIGHT,
    ACTION_180,
    ACTION_WAIT,
} ActionType;

typedef struct Action
{
    ActionType type;
} Action;

typedef enum RobotDirection
{
    ROBOT_INVALID_DIR,
    ROBOT_UP,
    ROBOT_RIGHT,
    ROBOT_DOWN,
    ROBOT_LEFT,
} RobotDirection;

static Action actionList[256];
static size_t actionCount = 0;

RobotDirection GetDirectionFromPoints(Point first, Point second)
{
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

bool GenerateActionList(const uint8_t map[MAP_HEIGHT][MAP_WIDTH],
                        Point start,
                        Point *food,
                        int foodCount)
{
    Point path[256];
    Point currentPos = start;
    RobotDirection currDirection = ROBOT_INVALID_DIR;
    bool firstRun = true;

    for (int foodIndex = 0; foodIndex < foodCount; foodIndex++) {
        Point currFood = food[foodIndex];
        AlgoResult result = RunAStar(map, currentPos, currFood, path);

        // Get the starting direction if this is the first run
        if (firstRun) {
            if (result.pathLength < 2)
                return false;
            currDirection = GetDirectionFromPoints(path[0], path[1]);
        }

        // Now we can deal with our path
        for (int i = 1; i < result.pathLength; i++) {
            RobotDirection nextDir = GetDirectionFromPoints(path[i - 1], path[i]);
            if (nextDir != currDirection) {
                // Calculate clockwise distance
                int diff = nextDir - currDirection;
                if (diff == 1 || diff == -3)
                    actionList[actionCount++] = (Action){ACTION_TURN_RIGHT};
                else if (diff == -1 || diff == 3)
                    actionList[actionCount++] = (Action){ACTION_TURN_LEFT};
                else
                    actionList[actionCount++] = (Action){ACTION_180};

                currDirection = nextDir;
            } else {
                // We're going straight so we need to detect
                // if there is an intersection we need to ignore
                if (path[i - 1].x - 1 >= 0 && map[path[i - 1].y][path[i - 1].x - 1] == 0 && currDirection != ROBOT_LEFT  && currDirection != ROBOT_RIGHT)
                    actionList[actionCount++] = (Action){ACTION_IGNORE_INTERSECTION};
                else if (path[i - 1].x + 1 < MAP_WIDTH && map[path[i - 1].y][path[i - 1].x + 1] == 0 && currDirection != ROBOT_LEFT && currDirection != ROBOT_RIGHT)
                    actionList[actionCount++] = (Action){ACTION_IGNORE_INTERSECTION};
                else if (path[i - 1].y - 1 >= 0 && map[path[i - 1].y - 1][path[i - 1].x] == 0 && currDirection != ROBOT_DOWN && currDirection != ROBOT_UP)
                    actionList[actionCount++] = (Action){ACTION_IGNORE_INTERSECTION};
                else if (path[i - 1].y + 1 < MAP_HEIGHT && map[path[i - 1].y + 1][path[i - 1].x] == 0  && currDirection != ROBOT_DOWN && currDirection != ROBOT_UP)
                    actionList[actionCount++] = (Action){ACTION_IGNORE_INTERSECTION};
            }
        }

        actionList[actionCount++] = (Action){ACTION_WAIT};
        currentPos = path[result.pathLength - 1];

        // Always set first run to false
        firstRun = false;
    }

    return true;
}

int8_t Heuristic(Point start, Point end)
{
    int8_t xdiff = end.x - start.x;
    if (xdiff < 0)
        xdiff = -xdiff;

    int8_t ydiff = end.y - start.y;
    if (ydiff < 0)
        ydiff = -ydiff;

    return xdiff + ydiff;
}

AlgoResult RunAStar(const uint8_t map[MAP_HEIGHT][MAP_WIDTH],
                    Point start,
                    Point food,
                    Point *result)
{

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
                }
                else if (prev_direction == 1) { // Move left
                    current.x--;
                }
                else if (prev_direction == 2) { // Move down
                    current.y++;
                }
                else if (prev_direction == 3) { // Move up
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
                    priorities[neighbour.y][neighbour.x] = newDistance + Heuristic(neighbour, food);
                    previous[neighbour.y][neighbour.x] = i; // Store the direction we came from
                }
            }
        }

        visited[next.y][next.x] = true;
    }
}
// Don't need to modify this
bool point_eq(Point a, Point b) { return a.x == b.x && a.y == b.y; }

// ************* TESTING **************

// static uint8_t map[MAP_HEIGHT][MAP_WIDTH] = {
//     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
//     {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//     {1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1},
//     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
//     {1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1},
//     {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1},
//     {1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1},
//     {1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1},
//     {1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1},
//     {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1},
//     {1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1},
//     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1},
//     {1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1},
//     {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
// };

static uint8_t map[15][19] = {
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
{1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1},
{1,1,1,1,1,0,1,0,1,1,1,1,1,1,1,0,1,0,1},
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1},
{1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1},
{1,0,1,0,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1},
{1,0,1,1,1,0,1,0,1,0,1,0,1,0,1,1,1,0,1},
{1,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,0,0,1},
{1,0,1,1,1,1,1,0,1,0,1,1,1,0,1,0,1,1,1},
{1,0,0,0,0,0,0,0,1,0,0,0,1,0,1,0,0,0,1},
{1,1,1,1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1},
{1,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,1},
{1,0,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,0,1},
{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
};

static Point food_list[5]= {{1,9},
{5,5},
{7,1},
{13,5},
{9,9}};

int main() {

    // Print map
    for(int i = 0; i < MAP_HEIGHT; i++) {
        for(int j = 0; j < MAP_WIDTH; j++) {
            bool isFood = false;
            for(int f = 0; f < sizeof(food_list)/sizeof(Point); f++)
                if(food_list[f].x == j && food_list[f].y == i) isFood = true;
            
            if(isFood) printf("o ");
            else if(map[i][j]) printf("X ");
            else printf("_ ");
        }

        printf("\r\n");
    }

    printf("\r\n");

    Point start = {1, 1};
    bool success = GenerateActionList(map, start, food_list, sizeof(food_list) / sizeof(Point));
    if (!success)
    {
        printf("! > Failed while generating action list\r\n");
    }
    printf(":: %d Actions Generated\r\n", actionCount);

#define CASE_AND_PRINT(x) case x: \
                          printf("-> " #x "\r\n"); \
                          break; 
    for(size_t i = 0; i < actionCount; i++) {
        switch(actionList[i].type) {
        CASE_AND_PRINT(ACTION_IGNORE_INTERSECTION)
        CASE_AND_PRINT(ACTION_180)
        CASE_AND_PRINT(ACTION_TURN_LEFT)
        CASE_AND_PRINT(ACTION_TURN_RIGHT)
        case ACTION_WAIT:
            printf("<+> Wait on food\r\n\r\n");
            break;
        }
    }
}