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

#include <stdbool.h>
#include <stdint.h>

void SetupMotors();
void SetStopMotors(bool m1, bool m2);
int16_t GetQuadDecCountMotor1();
int16_t GetQuadDecCountMotor2();
float CalcMotor1Speed();
float CalcMotor2Speed();
void MotorController(float leftSpeed, float rightSpeed);
