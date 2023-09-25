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
#include "motor.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

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

void Assert(bool cond, const char* msg) {
    if(!cond) {
        WriteUARTString("ASSERT: ", sizeof("ASSERT: "));
        WriteUARTString((char *)msg, strlen(msg));
        while(1) {
            CyDelay(100);
        }
    }
}

void InitLeftTurn() {
    DisableSpeedISR();
    PWM_1_WriteCompare(94);
    PWM_2_WriteCompare(160);
}

void InitRightTurn() {           
    DisableSpeedISR();
    PWM_1_WriteCompare(160);
    PWM_2_WriteCompare(94);
}

void StateMachine(bool reset) {
    static State current_state = STRAIGHT;
    static struct {
        bool goLeft;
        bool goRight;
    } driftParams = { false, false };
    
    static struct {
        bool pd3, pd4;
    } postTurnIgnore = {false, false};
    
    if(reset) {
        current_state = STRAIGHT;
        postTurnIgnore.pd3 = false;
        postTurnIgnore.pd4 = false;
        driftParams.goLeft = false;
        driftParams.goRight = false;
        PRINT_STATE(STRAIGHT);
        EnableSpeedISR();
        return;
    }
    
    // Updated at end of function
    static uint8_t prevSensors = 0;
    uint8_t sensors = PD_Read();
    
    if(PD_GET(sensors, 3) && !PD_GET(prevSensors, 3)) {
        postTurnIgnore.pd3 = false;   
    }
    
    if(PD_GET(sensors, 4) && !PD_GET(prevSensors, 4)) {
        postTurnIgnore.pd4 = false;   
    }
    
    
    switch (current_state) {
        case STRAIGHT:
        
            if(XOR(PD_GET(sensors, 1), PD_GET(sensors, 2))) {
                current_state = CORRECT_DRIFT;
                PRINT_STATE(CORRECT_DRIFT);
                driftParams.goLeft = PD_GET(sensors, 2);
                driftParams.goRight = PD_GET(sensors, 1);
                    
                motorBoostLeft = PD_GET(sensors, 1) ? 2.6f : 0.0f;
                motorBoostRight = PD_GET(sensors, 2) ? 2.3f : 0.0f;
                SetTargetSpeeds(15.0f + (PD_GET(sensors, 1) ? 3.0f : 0.0f), 16.0f + (PD_GET(sensors, 2) ? 3.0f : 0.0f));  
            
            } 
            
            if (!PD_GET(sensors, 3) && !postTurnIgnore.pd3) {
                current_state = TURN_LEFT;
                PRINT_STATE(TURN_LEFT);
                
                InitLeftTurn();
            } 
            
            if (!PD_GET(sensors, 4) && !postTurnIgnore.pd4) {
                current_state = TURN_RIGHT;
                PRINT_STATE(TURN_RIGHT);
                
                InitRightTurn();
            }
            
            
            break;
            
        case CORRECT_DRIFT:
            ASSERT(!(driftParams.goLeft && driftParams.goRight));
            
            if((!PD_GET(sensors, 1) && driftParams.goRight) || (!PD_GET(sensors, 2) && driftParams.goLeft)) {
                current_state = STRAIGHT;
                PRINT_STATE(STRAIGHT);
                
                motorBoostLeft = driftParams.goLeft ? 2.6f : 0.0f;
                motorBoostRight = driftParams.goRight ? 2.3f : 0.0f;
                SetTargetSpeeds(15.0f, 15.0f);
            }
            
            if(!PD_GET(sensors, 3) && !postTurnIgnore.pd3) {
                current_state = TURN_LEFT;
                PRINT_STATE(TURN_LEFT);
                
                InitLeftTurn();
            }
            
            if(!PD_GET(sensors, 4) && !postTurnIgnore.pd4) {
                current_state = TURN_RIGHT;
                PRINT_STATE(TURN_RIGHT);
                
                InitRightTurn();
            }
            
            break;
        case TURN_LEFT:
            
            if(PD_GET(sensors, 6)) break; 
            
            PWM_1_WriteCompare(127);
            PWM_2_WriteCompare(127);
            
            current_state = STRAIGHT;
            EnableSpeedISR();
            PRINT_STATE(STRAIGHT);
            motorBoostLeft = 0.0f;
            motorBoostRight = 0.0f;
            SetTargetSpeeds(12.0f, 12.0f);
            
            postTurnIgnore.pd3 = true;
            
            break;

        case TURN_RIGHT:
            
            if(PD_GET(sensors, 6)) break;
            
            current_state = STRAIGHT;
            EnableSpeedISR();
            PRINT_STATE(STRAIGHT);
            motorBoostLeft = 0.0f;
            motorBoostRight = 0.0f;
            SetTargetSpeeds(12.0f, 12.0f);
            
            postTurnIgnore.pd4 = true;
            
            break;
            
        default:
            ASSERT_MSG(false, "Invalid state reached");
    }
    
    prevSensors = sensors;
    if(current_state != TURN_LEFT && current_state != TURN_RIGHT) MotorController();
}
