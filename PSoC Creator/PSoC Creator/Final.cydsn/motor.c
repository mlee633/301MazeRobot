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

#include "motor.h"
#include "project.h"
#include "uart.h"
#include <math.h>
#include <stdio.h>

volatile static int16_t motor1Count = 0, motor2Count = 0, oldCount1 = 0, oldCount2 = 0;
volatile static float leftSpeedTarget = 0, rightSpeedTarget = 0;
volatile static bool shouldUpdateSpeed = false;

CY_ISR(MotorSpeedTimerOverflow) {
    motor1Count = QuadDec_1_GetCounter() - oldCount1;
    motor2Count = QuadDec_2_GetCounter() - oldCount2;
    
    
    oldCount1 = QuadDec_1_GetCounter();
    oldCount2 = QuadDec_2_GetCounter();
    shouldUpdateSpeed = true;
}

void SetupMotors() {
    PWM_1_WritePeriod(255);
    PWM_1_Start();
    PWM_1_WriteCompare(127); // writecompare value / write period = Duty cycle percentage
    
    PWM_2_WritePeriod(255);
    PWM_2_Start();
    PWM_2_WriteCompare(127);
    
    QuadDec_1_Start();
    QuadDec_2_Start();
    MotorSpeedTimer_Start();
    MotorUpdateSpeed_StartEx(MotorSpeedTimerOverflow);
    
    MotorSpeedTimer_WritePeriod(MOTOR_SPEED_CALC_PERIOD_MS - 1);
    QuadDec_1_SetCounter(0);
    QuadDec_2_SetCounter(0);
}

void DisableSpeedISR() {
    shouldUpdateSpeed = false;
    MotorUpdateSpeed_Disable();
}

void EnableSpeedISR() {
    shouldUpdateSpeed = false;
    MotorUpdateSpeed_Enable();   
}

void BoostLeftMotor(int8_t pwmCount) {
    uint8_t current = PWM_1_ReadCompare();
    if(pwmCount >= 0 && (uint8_t)pwmCount >= 255 - current) {
        PWM_1_WriteCompare(255);
    } else if(pwmCount < 0 && (uint8_t)(-pwmCount) >= current) {
        PWM_1_WriteCompare(0);
    } else {
        if(pwmCount >= 0) PWM_1_WriteCompare(current + pwmCount);
        else PWM_1_WriteCompare(current - (uint8_t)(-pwmCount));
    }
}


void BoostRightMotor(int8_t pwmCount) {
    uint8_t current = PWM_2_ReadCompare();
    if(pwmCount >= 0 && (uint8_t)pwmCount >= 255 - current) {
        PWM_2_WriteCompare(255);
    } else if(pwmCount < 0 && (uint8_t)(-pwmCount) >= current) {
        PWM_2_WriteCompare(0);
    } else {
        if(pwmCount >= 0) PWM_2_WriteCompare(current + pwmCount);
        else PWM_2_WriteCompare(current - (uint8_t)(-pwmCount));
    }
}


void SetTargetSpeeds(float left, float right) {
    leftSpeedTarget = left;
    rightSpeedTarget = right;
}

void SetTargetLeftSpeed(float l) {
    leftSpeedTarget = l;
}

void SetTargetRightSpeed(float r) {
    rightSpeedTarget = r;
}

float GetTargetLeftSpeed() {
    return leftSpeedTarget;   
}

float GetTargetRightSpeed() {
    return rightSpeedTarget;    
}

int16_t GetQuadDecCountMotor1() {
    return motor1Count;
}

int16_t GetQuadDecCountMotor2() {
    return motor2Count;
}

float CalcDistance1Meter() {
    float numRots = (float) QuadDec_1_GetCounter() / (float)PULSES_PER_ROTATION;
    float rads = 2.0 * M_PI * numRots;
    return ((rads * WHEEL_1_RADIUS_CM) ); // Distance in meters
}

float CalcDistance2Meter() {
    float numRots = (float) QuadDec_2_GetCounter() / (float)PULSES_PER_ROTATION;
    float rads = 2.0 * M_PI * numRots;
    return ((rads * WHEEL_1_RADIUS_CM) ); // Distance in meters
}

float CalcTotalDistanceMeter() {
    float total = CalcDistance1Meter() + CalcDistance2Meter();
    float average = total / 2.0;
    return average;  
}

float CalcMotor1Speed() {
    float numRots = (float)motor1Count / (float)PULSES_PER_ROTATION;
    float radsPerSec = 2 * M_PI * numRots / MOTOR_SPEED_CALC_PERIOD_S;
    return -radsPerSec * WHEEL_1_RADIUS_CM;
}

float CalcMotor2Speed() {
    float numRots = (float)motor2Count / (float)PULSES_PER_ROTATION;
    float radsPerSec = 2 * M_PI * numRots / MOTOR_SPEED_CALC_PERIOD_S;
    return -radsPerSec * WHEEL_2_RADIUS_CM;
}

void SetStopMotors(bool m1, bool m2) {
    MotorStopReg_Write(m1 | (m2 << 1));
}


void MotorController() {  
    if(!shouldUpdateSpeed) return;
    shouldUpdateSpeed = false;
    
    float mot1Speed = CalcMotor1Speed();
    float mot2Speed = CalcMotor2Speed();
    
    float mot1Diff = leftSpeedTarget - mot1Speed;
    float mot2Diff = rightSpeedTarget - mot2Speed;
    
    //mot1Diff = (mot1Diff > MAX_ERROR) ? MAX_ERROR : (mot1Diff < -MAX_ERROR) ? -MAX_ERROR : 0;
    //mot2Diff = (mot2Diff > MAX_ERROR) ? MAX_ERROR : (mot2Diff < -MAX_ERROR) ? -MAX_ERROR : 0;
    
    float mot1Cmp = PWM_1_ReadCompare();
    float mot2Cmp = PWM_2_ReadCompare();
    
    float mot1Target = mot1Cmp + /* 0.5 * */ mot1Diff;
    float mot2Target = mot2Cmp + /* 0.5 * */ mot2Diff;
    
    static char usbBuffer[128];
    char* buff = usbBuffer;
    int count;
    
    //count = sprintf(buff, "Current quad encs: %d, %d\r\n", (int)(motor1Count), (int)(motor2Count));
    //WriteUARTString(usbBuffer, count);
    
    if(mot1Target > 255) {
        PWM_1_WriteCompare(255);    
    } else if(mot1Target < 0) {
        PWM_1_WriteCompare(0);
    } else {
        PWM_1_WriteCompare((uint8_t) mot1Target);
    }
    
    if(mot2Target > 255) {
        PWM_2_WriteCompare(255);    
    } else if(mot2Target < 0) {
        PWM_2_WriteCompare(0);
    } else {
        PWM_2_WriteCompare((uint8_t) mot2Target);
    }
}