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
#include "project.h"
#include "uart.h"
#include "motor.h"
#include "action.h"
#include "sensors.h"
#include "maze.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define COUNT_OF(x) (sizeof(x)/sizeof((x)[0]))

// static uint8_t map[MAP_HEIGHT][MAP_WIDTH] = {
//     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
//     {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//     {1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1},
//     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
//     {1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1},
//     {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1},
//     {1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1},
//     {1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1},
//     {1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1},
//     {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1},
//     {1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1},
//     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1},
//     {1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1},
//     {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
//     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
// };

/* static uint8_t map[15][19] = { */
/*     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, */
/*     {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, */
/*     {1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1}, */
/*     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1}, */
/*     {1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1}, */
/*     {1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1}, */
/*     {1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1}, */
/*     {1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1}, */
/*     {1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1}, */
/*     {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1}, */
/*     {1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1}, */
/*     {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1}, */
/*     {1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1}, */
/*     {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, */
/*     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, */
/* }; */

static uint8_t map[15][19] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1},
    {1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

// Point food_list[5] = {{1, 9}, {7, 1}, {5, 5}, {13, 5}, {9, 9}};
// Point food_list[5] = {{1, 9}, {9, 9}, {5, 5}, {7, 1}, {13, 5}};
// Point food_list[5] = {{15, 13}, {11, 1}, {7, 9}, {9, 2}, {9, 9}};
// Point food_list[5] = {{9, 1}, {6, 13}, {5, 3}, {8, 1}, {9, 9}};
// Point food_list[5] = {{16, 7}, {11, 4}, {5, 7}, {7, 6}, {9, 9}};
// Point food_list[5] = {{17, 11}, {5, 1}, {9, 9}, {7, 11}, {9, 9}}; 
// Point food_list[5] = {{8, 11}, {4, 11}, {9, 5}, {11, 5}, {9, 9}}; 
// Point food_list[5] = {{9, 9}, {1, 1}, {14, 13}, {1, 5}, {14, 3}}; 
// Point food_list[5] = {{2, 5}, {5, 13}, {5, 3}, {1, 9}, {1, 6}}; 
// Point food_list[5] = {{14, 3}, {13, 8}, {8, 11}, {1, 3}, {9, 1}}; 
Point food_list[5] = {
	{5,3},
	{1,3},
	{11,7},
	{17,1},
	{15,13}
};
Point start = {7,9};

char* AppendStrToStr(char *result, const char* src, size_t len);

void normal() {
    char usbBuffer[1024];
    
    if(GenerateActionList(map, start, food_list, COUNT_OF(food_list)) == false) {
        TrackLED1_Write(0xff);
        return;
    }   
    
    if(UARTIsReady() == 0) {
        SetUseUSB(false);
    } else {
        WriteUARTString("UART Is Connected! \r\n", sizeof("UART Is Connected! \r\n"));
    }
    
    SetTargetSpeeds(MOTOR_SPEED, MOTOR_SPEED);
    for(;;) {
        StateMachine(false);
        
        if(ReadUARTChar() == 's') {
            uint8_t q1 = PWM_1_ReadCompare();
            uint8_t q2 = PWM_2_ReadCompare();
            uint8_t pd = PD_Read();
            int count = snprintf(usbBuffer, sizeof(usbBuffer),
                "PWM: %d, %d\r\nSpeed: %dcm/s, %dcm/s\r\n%d  %d  %d\r\n%d %d %d %d\r\n", 
                PWM_1_ReadCompare(), PWM_2_ReadCompare(),
                (int) q1,
                (int) q2,
                (bool)(pd & (1 << 4)),
                (bool)(pd & (1 << 5)),
                (bool)(pd & (1 << 6)),
                (bool)(pd & (1 << 2)),
                (bool)(pd & (1 << 0)),
                (bool)(pd & (1 << 1)),
                (bool)(pd & (1 << 3)));
  
            WriteUARTString(usbBuffer, count);      
        }
    }
    
}

void speed_run() {
    
    SetStopMotors(0, 0);
    EnableSpeedISR();
    SetTargetSpeeds(39.0f + 1.0f, 39.0f + 1.0f);
    
    while(1) {
        MotorController();
    }
    
    SetStopMotors(1, 1);
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    TrackLED1_Write(0xff);
    TrackLED2_Write(0xff);
    TrackLED3_Write(0xff);
    
    InitSensors();
    USBUART_1_Start(0, USBUART_1_5V_OPERATION);
        
    SetupMotors();
    CyDelay(1000);
    
    DisableSpeedISR();
    EnableSpeedISR();
    
#define SPEED_RUN 0

#if SPEED_RUN
    speed_run();        
#else
    normal();
#endif
    
}

char* AppendStrToStr(char *result, const char* src, size_t len) {
    memcpy(result, src, len);
    return result + len;
}


/* [] END OF FILE */
