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
#define MOTOR_SPEED_CALC_PERIOD_MS 50
#define MOTOR_SPEED_CALC_PERIOD_S (((float) MOTOR_SPEED_CALC_PERIOD_MS) / 1000.0f)

#define MOTOR_GEAR_RATIO 19
#define PULSES_PER_ROTATION ((MOTOR_GEAR_RATIO) * 3 * (QuadDec_1_COUNTER_RESOLUTION))

#define WHEEL_1_RADIUS_CM (3.25f)
#define WHEEL_2_RADIUS_CM (3.25f)

#define MOTOR_SPEED (34.0f)

void SetupMotors();
void SetStopMotors(bool m1, bool m2);

int32_t GetQuadDecCountLeftMotor();
int32_t GetQuadDecCountRightMotor();
void DisableSpeedISR();
void EnableSpeedISR();

float CalcMotor1Speed();
float CalcMotor2Speed();

float CalcDistanceLeftMotorCm(int32_t compareCount);
float CalcDistanceRightMotorCm(int32_t compareCount);
float CalcTotalDistanceMeter();

float GetTargetLeftSpeed();
float GetTargetRightSpeed();
void SetTargetSpeeds(float left, float right);
void SetTargetLeftSpeed(float);
void SetTargetRightSpeed(float);

void BoostLeftMotor(int8_t);
void BoostRightMotor(int8_t);

void UpdatePWMLeft(uint8_t);
void UpdatePWMRight(uint8_t);

void MotorController();
