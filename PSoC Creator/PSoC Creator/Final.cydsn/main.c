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

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    InitSensors();
    
    USBUART_1_Start(0, USBUART_1_5V_OPERATION);
    SetupMotors();
    MotorLengthFinish_StartEx(MotorLengthFin);
    

    char usbBuffer[1024];
    bool active = true;
    
    SetTargetSpeeds(15.0f, 15.0f);
    for(;;) {
        float q1 = CalcMotor1Speed();
        float q2 = CalcMotor2Speed();
        char c = ReadUARTChar();
        
        if(c == ' ') {
            active = !active;
            if(active) {
                SetStopMotors(0, 0);
                WriteUARTString("Robot Active\r\n", sizeof("Robot Active\r\n"));
            }
            else {
                SetStopMotors(1, 1);
                WriteUARTString("Robot Inactive\r\n", sizeof("Robot Inactive\r\n"));
            }
        }
        
        if(c == 's') {
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
        
        if(c == '2') {
            motorLengthFin = false;
            int count = sprintf(usbBuffer, "Running speed mode\r\n");
            WriteUARTString(usbBuffer, count);
            
            SetStopMotors(0, 0);
            EnableSpeedISR();
            motorBoostLeft = 0.0f;
            motorBoostRight = 0.0f;
            
            SetTargetSpeeds(30.0f, 30.0f);
            Timer_1_WritePeriod(3333 + 300);
            Timer_1_WriteCounter(3333 + 300);
            Timer_1_Start();
            while(motorLengthFin == false) {
                MotorController();
            }
            SetStopMotors(1, 1);
            Timer_1_Stop();
            
            count = sprintf(usbBuffer, "Finished speed mode\r\n");
            WriteUARTString(usbBuffer, count);
            while(ReadUARTChar() != 'q') {}
            
            SetStopMotors(0, 0);
            StateMachine(true);
        }
        
        if(c == 'r') {
            StateMachine(true);
        }
        
        if(active) {
            StateMachine(false);
        }
    }
    
}

char* AppendStrToStr(char *result, const char* src, size_t len) {
    memcpy(result, src, len);
    return result + len;
}


/* [] END OF FILE */
