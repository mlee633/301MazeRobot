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
#include "action.h"

typedef enum {
    STRAIGHT,
    LEFT_FLAGGED,
    RIGHT_FLAGGED,
    BOTH_FLAGGED,
    TURN_LEFT,
    TURN_RIGHT
} State;


int top_left_sensor_active();
int top_right_sensor_active();
int middle_left_sensor_active();
int middle_right_sensor_active();
void execute_turn_left();
void execute_turn_right();

Action StateMachine() {
    static State current_state = STRAIGHT;

    switch (current_state) {
        case STRAIGHT:
            if (top_left_sensor_active() && top_right_sensor_active()) {
                current_state = BOTH_FLAGGED;
            } else if (top_left_sensor_active()) {
                current_state = LEFT_FLAGGED;
            } else if (top_right_sensor_active()) {
                current_state = RIGHT_FLAGGED;
            }
            break;

        case LEFT_FLAGGED:
            if (middle_left_sensor_active()) {
                current_state = TURN_LEFT;
            } else if (!top_left_sensor_active()) {
                current_state = STRAIGHT;
            }
            break;

        case RIGHT_FLAGGED:
            if (middle_right_sensor_active()) {
                current_state = TURN_RIGHT;
            } else if (!top_right_sensor_active()) {
                current_state = STRAIGHT;
            }
            break;

        case BOTH_FLAGGED:
            if (middle_left_sensor_active()) {
                current_state = TURN_LEFT;
            } else if (middle_right_sensor_active()) {
                current_state = TURN_RIGHT;
            } else if (!top_left_sensor_active() && !top_right_sensor_active()) {
                current_state = STRAIGHT;
            }
            break;

        case TURN_LEFT:
            execute_turn_left();
            current_state = STRAIGHT;
            break;

        case TURN_RIGHT:
            execute_turn_right();
            current_state = STRAIGHT;
            break;
    }
    
    
    switch (current_state) {
        case STRAIGHT: return //go forward action;
        case TURN_LEFT: return // turn left action;
        case TURN_RIGHT: return // turn right action;
        default: return // stay still action;
    }

