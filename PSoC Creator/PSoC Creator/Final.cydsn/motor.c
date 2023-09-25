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
#define MOTOR_SPEED_CALC_PERIOD_MS 40
#define MOTOR_SPEED_CALC_PERIOD_S (((float) MOTOR_SPEED_CALC_PERIOD_MS) / 1000.0f)

#define MOTOR_GEAR_RATIO 19
#define PULSES_PER_ROTATION ((MOTOR_GEAR_RATIO) * 3 * (QuadDec_1_COUNTER_RESOLUTION))

#define WHEEL_1_RADIUS_CM (3.25f)
#define WHEEL_2_RADIUS_CM (3.25f)

volatile static int16_t motor1Count = 0, motor2Count = 0;
volatile static float leftSpeedTarget = 0, rightSpeedTarget = 0;
volatile float motorBoostLeft = 2.2f, motorBoostRight = 1.4f;
volatile static bool shouldUpdateSpeed = false;

CY_ISR(MotorSpeedTimerOverflow) {
    motor1Count = QuadDec_1_GetCounter();
    motor2Count = QuadDec_2_GetCounter();
    
    QuadDec_1_SetCounter(0);
    QuadDec_2_SetCounter(0);
    
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

void SetTargetSpeeds(float left, float right) {
    if(motorBoostLeft != 0.0f) {
        float approxPWM = 127 + left * motorBoostLeft; 
        PWM_1_WriteCompare((approxPWM < 0) ? 0 : (approxPWM > 255) ? 255 : approxPWM);
    }
    leftSpeedTarget = left;
    
    if(motorBoostRight != 0.0f) {
        float approxPWM2 = 127 + right * motorBoostRight;   
        PWM_2_WriteCompare((approxPWM2 < 0) ? 0 : (approxPWM2 > 255) ? 255 : approxPWM2);
    }
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
    
    float mot1Target = mot1Cmp + 0.5 * mot1Diff;
    float mot2Target = mot2Cmp + 0.5 * mot2Diff;
    
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