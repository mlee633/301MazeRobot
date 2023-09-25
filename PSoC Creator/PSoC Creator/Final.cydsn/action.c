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
        WriteUARTString(msg, strlen(msg));
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
    } correctDiftParams = { false, false };
    
    uint8_t sensors = PD_Read();
    
    Action action = {
      .leftSpeed = 0.0f,
      .rightSpeed = 0.0f,
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
                
                correctDiftParams.goLeft = PD_GET(sensors, 1);
                correctDiftParams.goRight = PD_GET(sensors, 2);
            }
            
            action = (Action) {
                .leftSpeed = 20.0f,
                .rightSpeed = 20.0f,
            };
            
            break;
            
        case CORRECT_DRIFT:
            ASSERT(!(correctDiftParams.goLeft && correctDiftParams.goRight));
            
            if((!PD_GET(sensors, 1) && correctDiftParams.goLeft) || (!PD_GET(sensors, 2) && correctDiftParams.goRight)) {
                current_state = STRAIGHT;
                PRINT_STATE(STRAIGHT);
                break;
            }
            
            if(correctDiftParams.goLeft) {
                action = (Action) {
                    .leftSpeed = 20.0f,
                    .rightSpeed = 10.0f
                };
            } else {
                action = (Action) {
                    .leftSpeed = 10.0f,
                    .rightSpeed = 20.0f
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
