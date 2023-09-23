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

    // Enable pin interrupts
//    LightSens1Edge_StartEx(light_sens1_interrupt);
//    LightSens2Edge_StartEx(light_sens2_interrupt);
//    
//    // Enable timer interrupts
//    LightSens1Overflow_StartEx(light_sens1_overflow_interrupt);
//    LightSens2Overflow_StartEx(light_sens2_overflow_interrupt);
//    
//    // Enable timers
//    LightSens1_TimeOut_Start();
//    LightSens2_TimeOut_Start();
    
    USBUART_1_Start(0, USBUART_1_5V_OPERATION);
    SetupMotors();
    InitSensors();
    
    SensTimer1_Start();
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    char usbBuffer[512];
    
    SetStopMotors(1, 1);
    for(;;) {
        float q1 = CalcMotor1Speed();
        float q2 = CalcMotor2Speed();
        
        MotorController(10.0f, -10.0f);
        
        char* buff = usbBuffer;
        uint8_t pd = PD_Read();
        int count = sprintf(buff, "Counters: %d, %d, %d, %d, %d, %d, %d\r\n%d  %d  %d\r\n%d %d %d %d\r\n", 
            SensTimer1_ReadCounter(),
            SensTimer2_ReadCounter(),
            SensTimer3_ReadCounter(),
            SensTimer4_ReadCounter(),
            SensTimer5_ReadCounter(),
            SensTimer6_ReadCounter(),
            SensTimer7_ReadCounter(),
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
