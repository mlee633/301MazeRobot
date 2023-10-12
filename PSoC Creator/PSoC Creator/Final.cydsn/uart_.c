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

/* [] END OF FILE */

#define USE_UART 0

#include "uart.h"
#include <project.h>
#include <stdbool.h>

volatile static bool useUSB = true;

void SetUseUSB(bool val) {
    useUSB = val;
}

bool UARTIsReady() {
    return USBUART_1_CDCIsReady();   
}

void WriteUARTString(char* string, size_t length) {
    if(!useUSB) return;
    
    while(USBUART_1_CDCIsReady() == 0) {};
    USBUART_1_PutData((uint8_t *)string, length);
}

size_t FormatInt(int32_t num, char* buffer) {
    size_t length = 0;
    int32_t num2 = num;
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

char ReadUARTChar() {
    static bool hasConfigured = false;

    if(!useUSB) return 0;
    
    if(!hasConfigured && USBUART_1_GetConfiguration()) {
        USBUART_1_CDC_Init();
        hasConfigured = true;
    } else if(hasConfigured && USBUART_1_DataIsReady()) {
        return USBUART_1_GetChar();
    }
    
    return 0;
}