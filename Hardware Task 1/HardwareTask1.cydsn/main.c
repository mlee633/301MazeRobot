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

void uart_send_str(const char* data);
void uart_conv_uint16(uint16_t x);
volatile static uint16 ADCresult;
volatile static uint16 count = 0;

CY_ISR(adc_finished_interrupt) {
    ADCresult = ADC_SAR_1_GetResult16();
    LED_Write(~LED_Read());
    count++;
    //uart_conv_uint16(ADCresult);
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Timer_1_Start();   
    ADC_SAR_1_Start();
    ADC_Finished_Interrupt_StartEx(adc_finished_interrupt);
    
    // INITIALIZE READING:
    USBUART_1_Start(0, USBUART_1_5V_OPERATION);

    uart_send_str("Hello World ");
    uart_conv_uint16(0);
    
    

    uint16_t prevAdcVal = ADCresult;
    for(;;)
    {
        /* Place your application code here. */
        if(prevAdcVal != count) {
            uart_conv_uint16(count);
            uart_send_str(",");
            uart_conv_uint16(ADC_SAR_1_CountsTo_mVolts(ADCresult));
            uart_send_str(",\r\n");
            prevAdcVal = count;
        }
        
    }
}

void uart_send_str(const char* data) {
    while(USBUART_1_CDCIsReady() == 0) {}
    USBUART_1_PutString(data);
}

void uart_conv_uint16(uint16_t data) {
    char digits[5] = {'?'};

    size_t i;
    for(i = 0; data != 0 && i < 5; i++) {
        digits[4 - i] = '0' + (data % 10);
        data /= 10;
    }
    
    while(USBUART_1_CDCIsReady() == 0) {}
    USBUART_1_PutData((const uint8_t*)digits + (5 - i), i);
}


/* [] END OF FILE */
