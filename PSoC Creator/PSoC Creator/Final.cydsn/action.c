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
#include "action.h"
#include "project.h"
#include "uart.h"
#include "motor.h"
#include "maze.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define PD_GET(s, n) ((s) & (1 << ((n) - 1)))
#define PD_ON(s, n) (((s) & (1 << ((n) - 1))) ? 1 : 0)
#define ASSERT(c) Assert(c, #c "\r\n")
#define ASSERT_MSG(c, msg) Assert(c, #c "; " msg "\r\n")
#define XOR(a, b) (!(a) != !(b))

typedef enum TimerDo {
    TIMER_DO_NOTHING,
    TIMER_DO_IGNORE_SENSORS,
    TIMER_DO_NEXT_ACTION,
} TimerDo;

volatile static bool ignore = false; //to help ignore certain sensors 
volatile static TimerDo timerDo = TIMER_DO_NOTHING;
volatile static bool runningTimer = false;

//static Action _actions[] = {
////    {ACTION_TURN_LEFT, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0},
////    {ACTION_TURN_LEFT, -1, 0},
////    {ACTION_TURN_LEFT, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0},
////    {ACTION_TURN_LEFT, -1, 0},
////    {ACTION_TURN_LEFT,-1, 0}, //Start of entering 360 loop at 2
////    {ACTION_IGNORE_INTERSECTION, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0},
////    {ACTION_IGNORE_INTERSECTION, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0},
////    {ACTION_TURN_LEFT, -1, 0},
////    {ACTION_TURN_LEFT, -1, 0},//Start of straight line
////    {ACTION_IGNORE_INTERSECTION, -1, 0},
////    {ACTION_TURN_LEFT, -1, 0}, //Start of at the bottom
////    {ACTION_TURN_LEFT, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0},
////    {ACTION_TURN_LEFT, -1, 0},
////    {ACTION_IGNORE_INTERSECTION, -1, 0},
////    {ACTION_TURN_LEFT, -1, 0}, //Reached bottom right corner
////    {ACTION_TURN_LEFT, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0},
////    {ACTION_TURN_LEFT, -1, 0}, //Reach 3
////    {ACTION_TURN_LEFT, -1, 0},
////    {ACTION_IGNORE_INTERSECTION, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0},
////    {ACTION_TURN_LEFT, -1, 0},
////    {ACTION_TURN_LEFT, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0},
////    {ACTION_TURN_LEFT, -1, 0},//Start of straight line
////    {ACTION_IGNORE_INTERSECTION, -1, 0},
////    {ACTION_IGNORE_INTERSECTION, -1, 0},
////    {ACTION_TURN_LEFT, -1, 0}, //Start of at the bottom
////    {ACTION_TURN_LEFT, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0},
////    {ACTION_TURN_LEFT, -1, 0},
////    {ACTION_TURN_LEFT, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0}, //Reached 4
////    {ACTION_IGNORE_INTERSECTION, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0},
////    {ACTION_IGNORE_INTERSECTION, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0},
////    {ACTION_TURN_LEFT, -1, 0},
////    {ACTION_TURN_LEFT, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0},
////    {ACTION_IGNORE_INTERSECTION, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0},
////    {ACTION_TURN_LEFT, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0},
////    {ACTION_TURN_RIGHT, -1, 0}
//    {ACTION_TURN_LEFT, -1, 0},
//    {ACTION_180, -1, FLAG_180_EXPECT_RIGHT | FLAG_180_EXPECT_LEFT} // right intersection
//    
//};
volatile static size_t actionIndex = 0;

Action GetAction() {   
    return GetActionList()[actionIndex];
}

void NextAction() {
    if(actionIndex + 1 < GetActionCount())
        actionIndex++;
}
//----------------------------------------------------------------------------
CY_ISR(StateMachineTimerInterrupt) {
    if(timerDo == TIMER_DO_IGNORE_SENSORS) {
        ignore = false;
        StateMachineTimer_Stop();
    } else if(timerDo == TIMER_DO_NEXT_ACTION) {
        NextAction();
        StateMachineTimer_Stop();
    }
    
    runningTimer = false;
}

typedef enum {
    STRAIGHT,
    TURN_LEFT_START,
    TURN_LEFT_END,
    TURN_RIGHT_START,
    TURN_RIGHT_END,
    START_180,
    NEXT_180_ONE,
    NEXT_180_TWO,
    END_180
    
} State;

static State debugStateLog[256];
static size_t debugStateLogCount = 0;

#define LOG_STATE(s) debugStateLog[debugStateLogCount++] = s;

void Assert(bool cond, const char* msg) {
    if(!cond) {
        WriteUARTString("ASSERT: ", sizeof("ASSERT: "));
        WriteUARTString((char *)msg, strlen(msg));
        while(1) {
            CyDelay(100);
        }
    }
}

#define ROTATE_SPEED 35

void InitLeftTurn() {
    DisableSpeedISR();
    UpdatePWMLeft(127 - ROTATE_SPEED);
    UpdatePWMRight(127 + ROTATE_SPEED);
}

void InitRightTurn() {
    DisableSpeedISR();
    UpdatePWMLeft(127 + ROTATE_SPEED);
    UpdatePWMRight(127 - ROTATE_SPEED);
}

void TimerDoStuff(TimerDo timerAction) {
    if(runningTimer) return;
    
    timerDo = timerAction;
    if(timerDo == TIMER_DO_IGNORE_SENSORS) {
        ignore = true;
        StateMachineTimer_WritePeriod(4500);
    } else {
        StateMachineTimer_WritePeriod(4500);
    }
    
    StateMachineTimer_Start();
    StateMachineTimerReset_Write(1);
    CyDelay(1);
    StateMachineTimerReset_Write(0);
    runningTimer = true;
}

void StateMachine(bool _reset) {
    static State current_state = STRAIGHT;
    static bool hasInit = false;
    static int32_t lastActionLeftMotorQuadEnc = 0;
    static int32_t lastActionRightMotorQuadEnc = 0;
    static bool isTurn180 = false;
    
    if(!hasInit) {
        StateMachineTimerInterrupt_StartEx(StateMachineTimerInterrupt);
        hasInit = true;
        StateMachineTimer_WritePeriod(4500);
    }
    
    // Updated at end of function
    uint8_t sensors = PD_Read();
    float distanceSinceLastAction = (CalcDistanceLeftMotorCm(lastActionLeftMotorQuadEnc) +
                                     CalcDistanceRightMotorCm(lastActionRightMotorQuadEnc)) / 2;
    
    float x = distanceSinceLastAction;
    float D = GetAction().distance < 0 ? -GetAction().distance : GetAction().distance;
    float speed = ((D - x > 3.0f && D - x < (D - 1)) || GetAction().type == ACTION_IGNORE_INTERSECTION) ? MOTOR_SPEED : 15.0f;
    if(D < 13.5f) speed = 15.0f;
    
    switch (current_state) {
        case STRAIGHT:
            if(0) {} // Need this for some reason
            
            // **** PID Controller Starts Here ****
            //int8_t pd1Cap = (255 - SensTimer1_ReadCapture()) / 4;
            //int8_t pd2Cap = (255 - SensTimer2_ReadCapture()) / 4;

            // Change speed based on distance till next point
            // Something like
            //          ________
            // speed   /        \
            // ^      /          \
            // |     /            \ 
            // ---> time
            
            if(!PD_GET(sensors, 1) && PD_GET(sensors, 5)) {
               BoostRightMotor(7);
               SetTargetSpeeds(speed, speed + 7);
            }
            
            else if(!PD_GET(sensors, 2)  && PD_GET(sensors, 7)) {
                BoostLeftMotor(7);
                SetTargetSpeeds(speed + 7, speed);
            } else {
                SetTargetSpeeds(speed, speed); 
            }

            //BoostRightMotor(-pid);
            //BoostLeftMotor(pid);
            // **** PID Controller Ends Here ****
            

            
            // If we see PD4 in the dark, and we aren't ignoring
            // the sensors after a turn, check the current action,
            // and perform it
            if (!PD_GET(sensors, 4) && !ignore) {
                if(GetAction().type == ACTION_IGNORE_INTERSECTION) {
                    TimerDoStuff(TIMER_DO_NEXT_ACTION);
                    
                    // Save quad encoders, so we can calculate the distance after this intersection
                    lastActionLeftMotorQuadEnc = GetQuadDecCountLeftMotor();
                    lastActionRightMotorQuadEnc = GetQuadDecCountRightMotor();
                    
                    break;
                } else if (GetAction().type == ACTION_TURN_RIGHT) {
                    NextAction();
                    InitRightTurn();
                    current_state = TURN_RIGHT_START;
                    LOG_STATE(TURN_RIGHT_START);
                    break;
                } else if(GetAction().type == ACTION_180) {
                    
                    isTurn180 = true;
                    
                    if(GetAction().flags180 & FLAG_180_EXPECT_LEFT && GetAction().flags180 & FLAG_180_EXPECT_RIGHT) {
                        NextAction();
                        InitLeftTurn();
                        current_state = START_180;
                        LOG_STATE(START_180);
                    } else if (GetAction().flags180 & FLAG_180_EXPECT_LEFT) {
                        NextAction();
                        InitRightTurn();
                        current_state = TURN_RIGHT_START;
                        LOG_STATE(TURN_RIGHT_START);
                    } else if (GetAction().flags180 & FLAG_180_EXPECT_RIGHT) {
                        NextAction();
                        InitLeftTurn();
                        current_state = TURN_LEFT_START;
                        LOG_STATE(TURN_LEFT_START);
                    }   
                } else if(GetAction().type == ACTION_STOP) {
                    SetStopMotors(1,1);   
                }
            }
            
            // If we see PD3 in the dark, and we aren't ignoring
            // the sensors after a turn, check the current action,
            // and perform it
            if (!PD_GET(sensors, 3) && !ignore) {
                if(GetAction().type == ACTION_IGNORE_INTERSECTION) {
                    TimerDoStuff(TIMER_DO_NEXT_ACTION);
                    
                    // Save quad encoders, so we can calculate the distance after this intersection
                    lastActionLeftMotorQuadEnc = GetQuadDecCountLeftMotor();
                    lastActionRightMotorQuadEnc = GetQuadDecCountRightMotor();
                    
                    break;              
                } else if (GetAction().type == ACTION_TURN_LEFT) {
                    NextAction();
                    InitLeftTurn();
                    current_state = TURN_LEFT_START;
                    LOG_STATE(TURN_LEFT_START);
                    break;
                }  else if(GetAction().type == ACTION_180) {
                    
                    isTurn180 = true;
                    
                    if(GetAction().flags180 & FLAG_180_EXPECT_LEFT && GetAction().flags180 & FLAG_180_EXPECT_RIGHT) {
                        NextAction();
                        InitLeftTurn();
                        current_state = START_180;
                        LOG_STATE(START_180);
                    } else if (GetAction().flags180 & FLAG_180_EXPECT_LEFT) {
                        NextAction();
                        InitRightTurn();
                        current_state = TURN_RIGHT_START;
                        LOG_STATE(TURN_RIGHT_START);
                    } else if (GetAction().flags180 & FLAG_180_EXPECT_RIGHT) {
                        NextAction();
                        InitLeftTurn();
                        current_state = TURN_LEFT_START;
                        LOG_STATE(TURN_LEFT_START);
                    }
                } else if(GetAction().type == ACTION_STOP) {
                    SetStopMotors(1,1);   
                }
             }
            
            // Check how far we've travelled, if the current action requires us to travel a certain distance
            if(GetAction().distance > 0) {
                if(distanceSinceLastAction >= GetAction().distance) {
                    if(GetAction().type == ACTION_180) {
                        NextAction();
                        InitLeftTurn();
                        current_state = TURN_LEFT_START;
                        LOG_STATE(TURN_LEFT_START);
                    } else if(GetAction().type == ACTION_STOP) {
                        SetStopMotors(1, 1);   
                    }
                }
            }
            
            
            break;
        case TURN_LEFT_START:
            if(PD_GET(sensors, 5)) break;
            current_state = TURN_LEFT_END;
        case TURN_LEFT_END:
            if(PD_GET(sensors, 6)) break;
            
            UpdatePWMLeft(127);
            UpdatePWMRight(127);
            
            CyDelay(50);
            
            // Save quad encoders, so we can calculate the distance after this intersection
            lastActionLeftMotorQuadEnc = GetQuadDecCountLeftMotor();
            lastActionRightMotorQuadEnc = GetQuadDecCountRightMotor();
            
            current_state = STRAIGHT;
            EnableSpeedISR();
            LOG_STATE(STRAIGHT);
            SetTargetSpeeds(15, 15);

            TimerDoStuff(TIMER_DO_IGNORE_SENSORS);
                
            break;
            
        case TURN_RIGHT_START:
            if(PD_GET(sensors, 7)) break;
            current_state = TURN_RIGHT_END;
            LOG_STATE(TURN_RIGHT_END);
            
        case TURN_RIGHT_END:
            if(PD_GET(sensors, 6)) break;
            
            UpdatePWMLeft(127);
            UpdatePWMRight(127);
            
            CyDelay(50);

            // Save quad encoders, so we can calculate the distance after this intersection
            lastActionLeftMotorQuadEnc = GetQuadDecCountLeftMotor();
            lastActionRightMotorQuadEnc = GetQuadDecCountRightMotor();
            
            current_state = STRAIGHT;
            EnableSpeedISR();
            LOG_STATE(STRAIGHT);
            SetTargetSpeeds(15, 15);
            
            TimerDoStuff(TIMER_DO_IGNORE_SENSORS);
            break;
        
        case START_180:
            if (PD_GET(sensors, 5)) break;
            current_state = NEXT_180_ONE;
            LOG_STATE(NEXT_180_ONE);
            
        case NEXT_180_ONE:
            if(PD_GET(sensors,6)) break;
            current_state = NEXT_180_TWO;
            LOG_STATE(NEXT_180_TWO);
            
        case NEXT_180_TWO:
            if (PD_GET(sensors,5)) break;
            current_state = END_180;
            LOG_STATE(END_180);
            
        case END_180:
            if (PD_GET(sensors,6)) break;         
            
            UpdatePWMLeft(127);
            UpdatePWMRight(127);
            
            CyDelay(50);
                        
            // Save quad encoders, so we can calculate the distance after this intersection
            lastActionLeftMotorQuadEnc = GetQuadDecCountLeftMotor();
            lastActionRightMotorQuadEnc = GetQuadDecCountRightMotor();
            
            current_state = STRAIGHT;
            EnableSpeedISR();
            LOG_STATE(STRAIGHT);
            SetTargetSpeeds(15, 15);
            
            TimerDoStuff(TIMER_DO_IGNORE_SENSORS);
            break;
            
        default:
            ASSERT_MSG(false, "Invalid state reached");
    }
    
    if(current_state != TURN_LEFT_START && 
       current_state != TURN_LEFT_END && 
       current_state != TURN_RIGHT_START && 
       current_state != TURN_RIGHT_END && 
       current_state != START_180 &&
       current_state != NEXT_180_ONE &&
       current_state != NEXT_180_TWO &&
       current_state != END_180) MotorController();
}
