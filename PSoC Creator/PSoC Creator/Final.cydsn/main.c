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
#include "sensors.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

char* AppendStrToStr(char *result, const char* src, size_t len);

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    USBUART_1_Start(0, USBUART_1_5V_OPERATION);
    SetupMotors();
    InitSensors();
    
    SensTimer1_Start();
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    char usbBuffer[1024];
    
    // SetStopMotors(1, 1);
    SetTargetSpeeds(20.0f, 20.0f);
    for(;;) {
        float q1 = CalcMotor1Speed();
        float q2 = CalcMotor2Speed();
        
        char* buff = usbBuffer;
        
        uint8_t pd = PD_Read();
        SetTargetLeftSpeed((pd & (1 << 0) ? 22.0f : 20.0f));
        SetTargetRightSpeed((pd & (1 << 1)) ? 22.0f : 20.0f);
        
//        int count = snprintf(buff, sizeof(usbBuffer),
//            "Speed: %dcm/s, %dcm/s\r\n%d  %d  %d\r\n%d %d %d %d\r\n", 
//            (int) q1,
//            (int) q2,
//            (bool)(pd & (1 << 4)),
//            (bool)(pd & (1 << 5)),
//            (bool)(pd & (1 << 6)),
//            (bool)(pd & (1 << 2)),
//            (bool)(pd & (1 << 0)),
//            (bool)(pd & (1 << 1)),
//            (bool)(pd & (1 << 3)));
//      
//        WriteUARTString(buff, count);
    }
    
}

#define crBegin static int state=0; switch(state) { case 0:
#define crReturn(x) do { state=__LINE__; return x; \
                         case __LINE__:; } while (0)
#define crFinish }

#define sensML 1
#define sensMR 1

void StateMachine() {
    crBegin;
    
    // Start of co-routine
    while(!(sensML && sensMR)) {
        crReturn();
    }
    
    crFinish;
}

char* AppendStrToStr(char *result, const char* src, size_t len) {
    memcpy(result, src, len);
    return result + len;
}


/* [] END OF FILE */
