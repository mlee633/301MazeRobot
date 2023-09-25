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
#include "project.h"

#if QuadDec_1_COUNTER_RESOLUTION != QuadDec_2_COUNTER_RESOLUTION
#error The quadrature decoders for both wheels must have the same resolution
#endif

// in ms
#define MOTOR_SPEED_CALC_PERIOD_MS 40
#define MOTOR_SPEED_CALC_PERIOD_S (((float) MOTOR_SPEED_CALC_PERIOD_MS) / 1000.0f)

#define MOTOR_GEAR_RATIO 19
#define PULSES_PER_ROTATION ((MOTOR_GEAR_RATIO) * 3 * (QuadDec_1_COUNTER_RESOLUTION))

#define WHEEL_1_RADIUS_CM (3.25f)
#define WHEEL_2_RADIUS_CM (3.25f)

extern volatile float motorBoostLeft, motorBoostRight;

void SetupMotors();
void SetStopMotors(bool m1, bool m2);
int16_t GetQuadDecCountMotor1();
int16_t GetQuadDecCountMotor2();

void DisableSpeedISR();
void EnableSpeedISR();

float CalcMotor1Speed();
float CalcMotor2Speed();

float GetTargetLeftSpeed();
float GetTargetRightSpeed();
void SetTargetSpeeds(float left, float right);
void SetTargetLeftSpeed(float);
void SetTargetRightSpeed(float);

void MotorController();
