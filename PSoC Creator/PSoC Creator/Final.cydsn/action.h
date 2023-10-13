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

/* [] END OF FILE */
#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef enum ActionType {
  ACTION_IGNORE_INTERSECTION,
  ACTION_TURN_LEFT,
  ACTION_TURN_RIGHT,
  ACTION_180,
  ACTION_STOP,
} ActionType;

typedef struct Action {
  ActionType type;
  // distance == -1 if we continue until next
  // intersection. Otherwise, distance = num cms
  // to travel.
  int8_t distance;
  // contains information about surrounding paths when we do
  // a 180. If the FLAG_180_EXPECT_LEFT bit is set, we should expect a left path
  // when we do a 180 If the FLAG_
  uint8_t flags180;
} Action;


void StateMachine(bool reset);