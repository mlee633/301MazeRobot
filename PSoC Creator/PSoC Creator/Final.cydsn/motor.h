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

extern volatile float motorBoostLeft, motorBoostRight;

void SetupMotors();
void SetStopMotors(bool m1, bool m2);
int16_t GetQuadDecCountMotor1();
int16_t GetQuadDecCountMotor2();
float CalcMotor1Speed();
float CalcMotor2Speed();

float GetTargetLeftSpeed();
float GetTargetRightSpeed();
void SetTargetSpeeds(float left, float right);
void SetTargetLeftSpeed(float);
void SetTargetRightSpeed(float);

void MotorController();
