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
#include <stdbool.h>

CY_ISR(motor1_quaddec_overflow) {
    
}

CY_ISR(motor2_quaddec_overflow) {
    
}

size_t FormatI8(int8_t num, char* buffer);

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
    
    QuadDec_1_Start();
    QuadDec_2_Start();
    
    PWM_1_WritePeriod(255);
    PWM_1_Start();
    PWM_1_WriteCompare(5); // writecompare value / write period = Duty cycle percentage
    
    PWM_2_WritePeriod(255);
    PWM_2_Start();
    PWM_2_WriteCompare(5);
    
    QuadDec1Interrupt_StartEx(motor1_quaddec_overflow);
    QuadDec2Interrupt_StartEx(motor2_quaddec_overflow);
    
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;) {
        int8_t q1 = QuadDec_1_GetCounter();
        int8_t q2 = QuadDec_2_GetCounter();
        
        char usbBuffer[12];
        char* buff = usbBuffer;
        buff += FormatI8(q1, buff);
        *(buff++) = ','; *(buff++) = ' ';
        buff += FormatI8(q2, buff);
        *(buff++) = '\r'; *(buff++) = '\n';
        
        while(USBUART_1_CDCIsReady() == 0) {}
        USBUART_1_PutData(usbBuffer, buff - usbBuffer);
    }
}

size_t FormatI8(int8_t num, char* buffer) {
    size_t length = 0;
    int8_t num2 = num;
    do { length++; num2 /= 10; } while(num2 != 0);
    
    bool isNegative = (num < 0);
    char* writeLoc = buffer;
    if(isNegative) *(writeLoc++) = '-';
    
    for(size_t i = 1; i <= length; i++) {
        writeLoc[length - i] = "9876543210123456789"[9 + num % 10];
        num /= 10;
    }
    
    return length + isNegative;
}

/* [] END OF FILE */
