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

char* AppendStrToStr(char *result, const char* src, size_t len);

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    InitSensors();
    CyDelay(100);
    
    USBUART_1_Start(0, USBUART_1_5V_OPERATION);
    SetupMotors();
    

    char usbBuffer[1024];
    // SetStopMotors(1, 1);
    SetTargetSpeeds(20.0f, 20.0f);
    for(;;) {
        float q1 = CalcMotor1Speed();
        float q2 = CalcMotor2Speed();
        
        Action action = StateMachine();
        SetTargetSpeeds(action.leftSpeed, action.rightSpeed);
        
        char c = ReadUARTChar();
        if(c == 's') {
            uint8_t pd = PD_Read();
            int count = snprintf(usbBuffer, sizeof(usbBuffer),
                "Speed: %dcm/s, %dcm/s\r\n%d  %d  %d\r\n%d %d %d %d\r\n", 
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

char* AppendStrToStr(char *result, const char* src, size_t len) {
    memcpy(result, src, len);
    return result + len;
}


/* [] END OF FILE */
