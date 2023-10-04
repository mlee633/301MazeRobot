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
#define PD_ON(s, n) (((s) & (1 << ((n) - 1))) ? 1 : 0)
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
    
    // NOTE: Trying to correct for overturning when off the line!
    // might remove
    if(PD_GET(PD_Read(), 2)) PWM_1_WriteCompare(92);
    else PWM_1_WriteCompare(99);
    PWM_2_WriteCompare(155);
}

void InitRightTurn() {

    DisableSpeedISR();
    PWM_1_WriteCompare(155);
    // NOTE: Trying to correct for overturning when off the line!
    // might remove
    if(PD_GET(PD_Read(), 1)) PWM_2_WriteCompare(92);
    else PWM_2_WriteCompare(99);
}

void StateMachine(bool reset) {
    static State current_state = STRAIGHT;
//    
//    static struct {
//        bool pd3, pd4;
//    } noSensDriftDir = { false, false };
//    
    static float driftErrorPrev = 0.0f;
    
    if(reset) {
        current_state = STRAIGHT;
        PRINT_STATE(STRAIGHT);
        EnableSpeedISR();
        return;
    }
    
    // Updated at end of function
    uint8_t sensors = PD_Read();
    
    static char usbBuffer[255];
    static int count = 0;
    
    switch (current_state) {
        case STRAIGHT:
            if(0) {}
            
            int8_t driftErrorApprox = -1 * (-PD_ON(sensors, 1) + PD_ON(sensors, 2)); 
            int8_t diff = (driftErrorApprox - driftErrorPrev);
            int8_t pid = 2 * driftErrorApprox + diff;
            
            BoostRightMotor(-pid);
            BoostLeftMotor(pid);
            driftErrorPrev = driftErrorApprox;

            break;
        case TURN_LEFT:
            
            if(PD_GET(sensors, 6)) break; 
            
            PWM_1_WriteCompare(127);
            PWM_2_WriteCompare(127);
            
            current_state = STRAIGHT;
            EnableSpeedISR();
            PRINT_STATE(STRAIGHT);
            SetTargetSpeeds(15.0f, 15.0f);
            
            break;

        case TURN_RIGHT:
            
            if(PD_GET(sensors, 6)) break;
            
            PWM_1_WriteCompare(127);
            PWM_2_WriteCompare(127);
            
            current_state = STRAIGHT;
            EnableSpeedISR();
            PRINT_STATE(STRAIGHT);
            SetTargetSpeeds(15.0f, 15.0f);
            
            break;
            
        default:
            ASSERT_MSG(false, "Invalid state reached");
    }
    
    if(current_state != TURN_LEFT && current_state != TURN_RIGHT) MotorController();
}
