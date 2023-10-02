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
    TURN_LEFT,
    TURN_RIGHT,
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
    PWM_1_WriteCompare(99);
    PWM_2_WriteCompare(155);
}

void InitRightTurn() {           
    DisableSpeedISR();
    PWM_1_WriteCompare(155);
    PWM_2_WriteCompare(99);
}

void StateMachine(bool reset) {
    static State current_state = STRAIGHT;
    static struct {
        bool pd3, pd4;
    } postTurnIgnore = {false, false};
    
    
    if(reset) {
        current_state = STRAIGHT;
        PRINT_STATE(STRAIGHT);
        EnableSpeedISR();
        return;
    }
    
    // Updated at end of function
    uint8_t sensors = PD_Read();
    
    switch (current_state) {
        case STRAIGHT:
           
            if(PD_GET(sensors, 3)) postTurnIgnore.pd3 = false;
            if(PD_GET(sensors, 4)) postTurnIgnore.pd4 = false;
            
            // Disable boost by default and then boost if 
            // necessary if one sensor is off
            BoostLeftMotor(0);
            BoostRightMotor(0);
            if (XOR(PD_GET(sensors,1), PD_GET(sensors,2))) {
                if (PD_GET(sensors, 2)) BoostRightMotor(2);
                else BoostLeftMotor(2);
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

        case TURN_LEFT:
            
            if(PD_GET(sensors, 6)) break; 
            
            PWM_1_WriteCompare(127);
            PWM_2_WriteCompare(127);
            
            current_state = STRAIGHT;
            EnableSpeedISR();
            PRINT_STATE(STRAIGHT);
            SetTargetSpeeds(15.0f, 15.0f);
            
            postTurnIgnore.pd3 = true;
            
            break;

        case TURN_RIGHT:
            
            if(PD_GET(sensors, 6)) break;
            
            PWM_1_WriteCompare(127);
            PWM_2_WriteCompare(127);
            
            current_state = STRAIGHT;
            EnableSpeedISR();
            PRINT_STATE(STRAIGHT);
            SetTargetSpeeds(15.0f, 15.0f);
            
            postTurnIgnore.pd4 = true;
            
            break;
            
        default:
            ASSERT_MSG(false, "Invalid state reached");
    }
    
    if(current_state != TURN_LEFT && current_state != TURN_RIGHT) MotorController();
}
