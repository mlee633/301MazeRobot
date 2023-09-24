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
    UART_Start();
    SetupMotors();
    InitSensors();
    
    SensTimer1_Start();
    
    UART_PutString("I am iron man I am iron man I am iron man I am iron man I am iron man I am iron man I am iron man I am iron man I am iron man I am iron man I am iron man I am iron man I am iron man I am iron man I am iron man I am iron man ");
    UART_PutString("AT+NAMETeam 11 BT");
    CyDelay(1001);
    
    char uartChar;
    while((uartChar = UART_GetChar())) {
        USBUART_1_PutChar(uartChar);
    }
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    char usbBuffer[512];
    
    SetStopMotors(1, 1);
    for(;;) {
        float q1 = CalcMotor1Speed();
        float q2 = CalcMotor2Speed();
        
        MotorController(10.0f, -10.0f);
        
        char* buff = usbBuffer;
        uint8_t pd = PD_Read();
        int count = snprintf(buff, sizeof(usbBuffer),
            "Counters: %d, %d, %d, %d, %d, %d, %d\r\n%d  %d  %d\r\n%d %d %d %d\r\n", 
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
      
        UART_PutArray((const uint8_t *) usbBuffer, count);
        
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
