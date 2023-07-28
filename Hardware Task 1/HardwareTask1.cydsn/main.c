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
volatile static uint8_t adc_flag = 0;

CY_ISR(adc_finished_interrupt) {
    ADCresult = ADC_SAR_1_GetResult16();
    adc_flag = !adc_flag;
    //uart_conv_uint16(ADCresult);
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Timer_1_Start();   
    ADC_SAR_1_Start();
    ADC_Finished_Interrupt_StartEx(adc_finished_interrupt);
    VDAC8_1_Start();
    
    // INITIALIZE READING:
    USBUART_1_Start(0, USBUART_1_5V_OPERATION);

    uart_send_str("Hello World ");
    uart_conv_uint16(0);
    
    

    uint8_t prev_adc_flag = adc_flag;
    for(;;)
    {
        /* Place your application code here. */
        if(prev_adc_flag != adc_flag) {
            uint16_t mV = ADC_SAR_1_CountsTo_mVolts(ADCresult);
            uint8_t dacVal = (ADCresult >> 4 /* Account for difference between 12bit ADC and 8bit DAC*/) >> 1 /* Compensate for different voltage ranges */;
            VDAC8_1_SetValue(dacVal);
//            uart_conv_uint16(mV);
//            uart_send_str(", ");
//            uart_conv_uint16(dacVal);
//            uart_send_str("\r\n");
            prev_adc_flag = adc_flag;
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
