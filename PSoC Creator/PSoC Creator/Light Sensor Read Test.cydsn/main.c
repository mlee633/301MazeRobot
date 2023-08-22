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

volatile static uint16_t light1Val, light2Val;

CY_ISR(light_sens1_interrupt) {
    // Grab timer value and reset
    light1Val = LightSens1_TimeOut_ReadCapture();
    
    // Reset the counter
    uint8_t curr = TimeOutResetReg_Read();
    TimeOutResetReg_Write(curr ^ (1 << 0));
    
    // Reset the interrupt flag on the pins
    LightSens1_ClearInterrupt();
}

CY_ISR(light_sens2_interrupt) {
    // Grab timer value and reset
    light2Val = LightSens2_TimeOut_ReadCapture();
    // Reset the counter
    uint8_t curr = TimeOutResetReg_Read();
    TimeOutResetReg_Write(curr ^ (1 << 1));
    
    // Reset the interrupt flag on the pins
    LightSens2_ClearInterrupt();
}

CY_ISR(light_sens1_overflow_interrupt) {
    light1Val = 0;
}

CY_ISR(light_sens2_overflow_interrupt) {
    light2Val = 0;
}


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    // Enable pin interrupts
    LightSens1Edge_StartEx(light_sens1_interrupt);
    LightSens2Edge_StartEx(light_sens2_interrupt);
    
    // Enable timer interrupts
    LightSens1Overflow_StartEx(light_sens1_overflow_interrupt);
    LightSens2Overflow_StartEx(light_sens2_overflow_interrupt);
    
    // Enable timers
    LightSens1_TimeOut_Start();
    LightSens2_TimeOut_Start();
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        if(light1Val) {
            LSDebug1_Write(0xff);
        } else {
            LSDebug1_Write(0x00);   
        }
        
        if(light2Val) {
            LSDebug2_Write(0xff);
        } else {
            LSDebug2_Write(0x00);   
        }
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
