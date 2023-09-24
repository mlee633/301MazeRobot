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
#include <stdint.h>

#define PD_GET(s, n) ((s) & (1 << ((n) - 1)))

typedef enum {
    STRAIGHT,
    LEFT_FLAGGED,
    RIGHT_FLAGGED,
    BOTH_FLAGGED,
    CORRECT_DRIFT,
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

#define PD_Read() 1
    uint8_t sensors = PD_Read();
    
    switch (current_state) {
        case STRAIGHT:
//            // Top right and Top left sensors
//            if(PD_GET(sensors, 5) && PD_GET(sensors, 7)) {
//                current_state = BOTH_FLAGGED;
//            // Top left sensor
//            } else if (PD_GET(sensors, 5)) {
//                current_state = LEFT_FLAGGED;
//            // Top right sensor
//            } else if (PD_GET(sensors, 7)) {
//                current_state = RIGHT_FLAGGED;
            if ( !PD_GET(sensors, 1) || !PD_GET(sensors, 2) ) {
                current_state = CORRECT_DRIFT;
            }
            break;
            
        case CORRECT_DRIFT:
            if (!PD_GET(sensors, 1)) {
                current_state = TURN_RIGHT;
            } else if (!PD_GET(sensors, 2)) {
                current_state = TURN_LEFT;
            }
            break;
            

//        case LEFT_FLAGGED:
//            if (middle_left_sensor_active()) {
//                current_state = TURN_LEFT;
//            } else if (!top_left_sensor_active()) {
//                current_state = STRAIGHT;
//            }
//            break;
//
//        case RIGHT_FLAGGED:
//            if (middle_right_sensor_active()) {
//                current_state = TURN_RIGHT;
//            } else if (!top_right_sensor_active()) {
//                current_state = STRAIGHT;
//            }
//            break;
//
//        case BOTH_FLAGGED:
//            if (middle_left_sensor_active()) {
//                current_state = TURN_LEFT;
//            } else if (middle_right_sensor_active()) {
//                current_state = TURN_RIGHT;
//            } else if (!top_left_sensor_active() && !top_right_sensor_active()) {
//                current_state = STRAIGHT;
//            }
//            break;
//
//        case TURN_LEFT:
//            execute_turn_left();
//            current_state = STRAIGHT;
//            break;
//
//        case TURN_RIGHT:
//            execute_turn_right();
//            current_state = STRAIGHT;
//            break;
    }
    
    
    switch (current_state) {
        case STRAIGHT: return (Action) {
            .leftSpeed = 10.0f,
            .rightSpeed = 10.0f
        }; //go forward action;
        case TURN_LEFT: return // turn left action;
        case TURN_RIGHT: return // turn right action;
        case CORRECT_DRIFT: return // correct drift action
    }
}
