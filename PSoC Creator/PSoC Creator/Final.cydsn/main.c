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
#include "project.h"
#include "uart.h"
#include "motor.h"
#include "action.h"
#include "sensors.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static bool motorLengthFin = false;

CY_ISR(MotorLengthFin) {
    motorLengthFin = true;
}

char* AppendStrToStr(char *result, const char* src, size_t len);

void normal() {
     char usbBuffer[1024];
    bool active = true;
    
    SetTargetSpeeds(15.0f, 15.0f);
    for(;;) {
        StateMachine(false);
    }
    
}

void speed_run() {
    MotorLengthFinish_StartEx(MotorLengthFin);
    
    motorLengthFin = false;
    
    SetStopMotors(0, 0);
    EnableSpeedISR();
    motorBoostLeft = 0.0f;
    motorBoostRight = 0.0f;
    
    SetTargetSpeeds(40.0f, 40.0f);
    Timer_1_Start();
    Timer_1_WritePeriod(2000 + 1000);
    Timer_1_WriteCounter(2000 + 1000);
    
    while(motorLengthFin == false) {
        MotorController();
    }
    
    SetStopMotors(1, 1);
    Timer_1_Stop();
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    InitSensors();
    USBUART_1_Start(0, USBUART_1_5V_OPERATION);
        
    SetupMotors();
    CyDelay(1000);
    
    DisableSpeedISR();
    EnableSpeedISR();
    
    
#define SPEED_RUN 0

#if SPEED_RUN
    speed_run();        
#else
    normal();
#endif
    
}

char* AppendStrToStr(char *result, const char* src, size_t len) {
    memcpy(result, src, len);
    return result + len;
}


/* [] END OF FILE */
