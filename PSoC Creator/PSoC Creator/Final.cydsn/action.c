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
#include "project.h"
#include "uart.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define PD_GET(s, n) ((s) & (1 << ((n) - 1)))
#define ASSERT(c) Assert(c, #c "\r\n")
#define ASSERT_MSG(c, msg) Assert(c, #c "; " msg "\r\n")
#define PRINT_STATE(s) WriteUARTString("State: " #s "\r\n", sizeof("State: " #s "\r\n"))
#define XOR(a, b) (!(a) != !(b))

#define LBOOST 2.4f
#define RBOOST 1.4f

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

void Assert(bool cond, const char* msg) {
    if(!cond) {
        WriteUARTString("ASSERT: ", sizeof("ASSERT: "));
        WriteUARTString((char *)msg, strlen(msg));
        while(1) {
            CyDelay(100);
        }
    }
}

Action StateMachine() {
    static State current_state = STRAIGHT;
    static struct {
        bool goLeft;
        bool goRight;
    } driftParams = { false, false };
    static bool hasInit = false;
    if(!hasInit) {
        hasInit = true;   
        return (Action) {
            .actionType = ACTION_CHANGE_SPEED,
            .leftSpeed = 20.0f,
            .rightSpeed = 20.0f
        };
    }
    
    
    uint8_t sensors = PD_Read();
    Action action = {
      .actionType = ACTION_NOTHING
    };
    
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
            if (XOR(PD_GET(sensors, 1), PD_GET(sensors, 2))) {
                current_state = CORRECT_DRIFT;
                PRINT_STATE(CORRECT_DRIFT);
                driftParams.goLeft = PD_GET(sensors, 2);
                driftParams.goRight = PD_GET(sensors, 1);
                
                
                action = (Action) {
                    .leftSpeed = 20.0f + PD_GET(sensors, 1) * 3.0f,
                    .rightSpeed = 20.0f + PD_GET(sensors, 2) * 3.0f,
                    .motorBoostLeft =  PD_GET(sensors, 1) ? 2.8f :  0.0f,
                    .motorBoostRight =  PD_GET(sensors, 2) ? 2.2f :  0.0f,
                    .actionType = ACTION_CHANGE_SPEED
                };
            } else if (PD_GET(sensors, 5) {
                current_state = TURN_LEFT;
                PRINT_STATE(TURN_LEFT);
                
            }
            
            
            
            break;
            
        case CORRECT_DRIFT:
            ASSERT(!(driftParams.goLeft && driftParams.goRight));
            
            if((!PD_GET(sensors, 1) && driftParams.goRight) || (!PD_GET(sensors, 2) && driftParams.goLeft)) {
                current_state = STRAIGHT;
                PRINT_STATE(STRAIGHT);
                
                action = (Action) {
                  .actionType = ACTION_CHANGE_SPEED,
                    .leftSpeed = 20.0f,
                    .rightSpeed = 20.0f,
                    .motorBoostLeft = driftParams.goLeft ? 2.8f : 0.0f,
                    .motorBoostRight = driftParams.goRight ? 2.2f : 0.0f,
                };
            }
            break;
        default:
            ASSERT_MSG(false, "Invalid state reached");

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
    
    
//    switch (current_state) {
//        case STRAIGHT: return (Action) {
//            .leftSpeed = 10.0f,
//            .rightSpeed = 10.0f
//        }; //go forward action;
//        break;
//        case TURN_LEFT: return (Action) {
//            .leftSpeed = 10.0f,
//            .rightSpeed = 5.0f
//        };
//        break;// turn left action;
//        case TURN_RIGHT: return (Action) {
//            .leftSpeed = 5.0f,
//            .rightSpeed = 10.0f
//        };
//        break;// turn right action;
//        case CORRECT_DRIFT: return (Action) {
//            
//        }// correct drift action
//    }
    
    return action;
}
