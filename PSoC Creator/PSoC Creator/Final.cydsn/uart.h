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
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void SetUseUSB(bool val);
bool UARTIsReady();

size_t FormatInt(int32_t num, char* buffer);
void WriteUARTString(char* string, size_t length);
char ReadUARTChar();