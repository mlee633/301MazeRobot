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

#if QuadDec_1_COUNTER_RESOLUTION != QuadDec_2_COUNTER_RESOLUTION
#error The quadrature decoders for both wheels must have the same resolution
#endif

// in ms
#define MOTOR_SPEED_CALC_PERIOD_MS 100
#define MOTOR_SPEED_CALC_PERIOD_S (((float) MOTOR_SPEED_CALC_PERIOD_MS) / 1000.0f)

#define MOTOR_GEAR_RATIO 19
#define PULSES_PER_ROTATION ((MOTOR_GEAR_RATIO) * 3 * (QuadDec_1_COUNTER_RESOLUTION))

#define WHEEL_1_RADIUS_CM (3.25f)
#define WHEEL_2_RADIUS_CM (3.25f)

volatile static int16_t motor1Count = 0, motor2Count = 0;
volatile static bool shouldFixMotorSpeeds = false;

CY_ISR(MotorSpeedTimerOverflow) {
    motor1Count = QuadDec_1_GetCounter();
    motor2Count = QuadDec_2_GetCounter();
    
    QuadDec_1_SetCounter(0);
    QuadDec_2_SetCounter(0);
    
    shouldFixMotorSpeeds = true;
}

void SetupMotors() {
    PWM_1_WritePeriod(255);
    PWM_1_Start();
    PWM_1_WriteCompare(200); // writecompare value / write period = Duty cycle percentage
    
    PWM_2_WritePeriod(255);
    PWM_2_Start();
    PWM_2_WriteCompare(200);
    
    QuadDec_1_Start();
    QuadDec_2_Start();
    MotorSpeedTimer_Start();
    MotorUpdateSpeed_StartEx(MotorSpeedTimerOverflow);
    
    MotorSpeedTimer_WritePeriod(MOTOR_SPEED_CALC_PERIOD_MS - 1);
    QuadDec_1_SetCounter(0);
    QuadDec_2_SetCounter(0);
}

int16_t GetQuadDecCountMotor1() {
    return motor1Count;
}

int16_t GetQuadDecCountMotor2() {
    return motor2Count;
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

void MotorController(float speedLeft, float speedRight) {
    if(!shouldFixMotorSpeeds) return;
    shouldFixMotorSpeeds = false;
    
    float mot1Speed = CalcMotor1Speed();
    float mot2Speed = CalcMotor2Speed();
    
    float mot1Diff = speedLeft - mot1Speed;
    float mot2Diff = speedRight - mot2Speed;
    
    //mot1Diff = (mot1Diff > MAX_ERROR) ? MAX_ERROR : (mot1Diff < -MAX_ERROR) ? -MAX_ERROR : 0;
    //mot2Diff = (mot2Diff > MAX_ERROR) ? MAX_ERROR : (mot2Diff < -MAX_ERROR) ? -MAX_ERROR : 0;
    
    float mot1Cmp = PWM_1_ReadCompare();
    float mot2Cmp = PWM_2_ReadCompare();
    
    float mot1Target = mot1Cmp + 0.5 * mot1Diff;
    float mot2Target = mot2Cmp + 0.5 * mot2Diff;
    
//    static char usbBuffer[128];
//    char* buff = usbBuffer;
//    int count;
//    
//    count = sprintf(buff, "Current speed: %d mm/s, %d mm/s \r\n", (int)(mot1Speed * 10), (int)(mot2Speed * 10));
//    WriteUARTString(usbBuffer, count);
//    
//    count = sprintf(buff, "Current error: %d mm/s, %d mm/s \r\n", (int)(mot1Diff * 10), (int)(mot2Diff * 10));
//    WriteUARTString(usbBuffer, count);
//    
//    count = sprintf(buff, "Current compare value: %d, %d \r\n", (int)(mot1Cmp), (int)(mot2Cmp));
//    WriteUARTString(usbBuffer, count);
//    
//    count = sprintf(buff, "Target compare value: %d, %d \r\n\r\n", (int)(mot1Target), (int)(mot2Target));
//    WriteUARTString(usbBuffer, count);
    
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