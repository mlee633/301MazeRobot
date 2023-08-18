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

volatile static uint8_t light1Val, light2Val;

CY_ISR(light_sens1_interrupt) {
    
    LightSens1_ClearInterrupt();
}

CY_ISR(light_sens2_interrupt) {
    
    LightSens2_ClearInterrupt();
}

CY_ISR(light_sens1_overflow_interrupt) {

}

CY_ISR(light_sens2_overflow_interrupt) {

}


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    LightSens1Edge_StartEx(light_sens1_interrupt);
    LightSens1Edge_StartEx(light_sens2_interrupt);
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
