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

#include "project.h"
#include <stdbool.h>

inline void InitSensors() {
    SensTimer1_Start();
    SensTimer2_Start();
    SensTimer3_Start();
    SensTimer4_Start();
    SensTimer5_Start();
    SensTimer6_Start();
    SensTimer7_Start();
}

inline uint8_t GetSensors() {
    return PD_Read();
}