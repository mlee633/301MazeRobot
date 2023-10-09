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
    
    if(UARTIsReady() == 0) {
        SetUseUSB(false);
    } else {
        WriteUARTString("UART Is Connected! \r\n", sizeof("UART Is Connected! \r\n"));
    }
    
    SetTargetSpeeds(MOTOR_SPEED, MOTOR_SPEED);
    for(;;) {
        StateMachine(false);
        
        if(ReadUARTChar() == 's') {
            uint8_t q1 = PWM_1_ReadCompare();
            uint8_t q2 = PWM_2_ReadCompare();
            uint8_t pd = PD_Read();
            int count = snprintf(usbBuffer, sizeof(usbBuffer),
                "PWM: %d, %d\r\nSpeed: %dcm/s, %dcm/s\r\n%d  %d  %d\r\n%d %d %d %d\r\n", 
                PWM_1_ReadCompare(), PWM_2_ReadCompare(),
                (int) q1,
                (int) q2,
                (bool)(pd & (1 << 4)),
                (bool)(pd & (1 << 5)),
                (bool)(pd & (1 << 6)),
                (bool)(pd & (1 << 2)),
                (bool)(pd & (1 << 0)),
                (bool)(pd & (1 << 1)),
                (bool)(pd & (1 << 3)));
  
            WriteUARTString(usbBuffer, count);      
        }
    }
    
}

void speed_run() {
    MotorLengthFinish_StartEx(MotorLengthFin);
    
    motorLengthFin = false;
    
    SetStopMotors(0, 0);
    EnableSpeedISR();
    
    SetTargetSpeeds(39.0f + 1.0f, 39.0f + 1.0f);
    Timer_1_Start();
    Timer_1_WritePeriod(2500);
    Timer_1_WriteCounter(2500);
    
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
