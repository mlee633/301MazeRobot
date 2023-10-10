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
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define PD_GET(s, n) ((s) & (1 << ((n) - 1)))
#define PD_ON(s, n) (((s) & (1 << ((n) - 1))) ? 1 : 0)
#define ASSERT(c) Assert(c, #c "\r\n")
#define ASSERT_MSG(c, msg) Assert(c, #c "; " msg "\r\n")
#define PRINT_STATE(s) WriteUARTString("State: " #s "\r\n", sizeof("State: " #s "\r\n"))
#define XOR(a, b) (!(a) != !(b))

typedef enum TimerDo {
    TIMER_DO_NOTHING,
    TIMER_DO_IGNORE_SENSORS,
    TIMER_DO_NEXT_ACTION,
} TimerDo;

volatile static bool ignore = false; //to help ignore certain sensors 
volatile static TimerDo timerDo = TIMER_DO_NOTHING;
volatile static bool runningTimer = false;

//-------------------TODO: Remove these, because they'll be in maze.h---------
typedef enum ActionType {
    ACTION_IGNORE_INTERSECTION,
    ACTION_TURN_LEFT,
    ACTION_TURN_RIGHT,
    ACTION_THREE_SIXTY,
} ActionType;

typedef struct Action {
     ActionType type;
} Action;

static Action _actions[256] = {
    { ACTION_TURN_RIGHT },
    { ACTION_TURN_RIGHT },
    { ACTION_TURN_LEFT },
    { ACTION_TURN_LEFT },
    { ACTION_TURN_RIGHT },
    { ACTION_TURN_RIGHT },
    { ACTION_TURN_LEFT },
    { ACTION_IGNORE_INTERSECTION },
    { ACTION_TURN_LEFT },
};
volatile static size_t _actionIndex = 0;

Action GetAction() {   
    return _actions[_actionIndex];
}

void NextAction() {
    _actionIndex++;   
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
    TrackLED2_Write(0x00);
}

typedef enum {
    STRAIGHT,
    TURN_LEFT_START,
    TURN_LEFT_END,
    TURN_RIGHT_START,
    TURN_RIGHT_END,
} State;

void Assert(bool cond, const char* msg) {
    if(!cond) {
        WriteUARTString("ASSERT: ", sizeof("ASSERT: "));
        WriteUARTString((char *)msg, strlen(msg));
        while(1) {
            CyDelay(100);
        }
    }
}

#define MOVE_SPEED 35

void InitLeftTurn() {
    DisableSpeedISR();
    UpdatePWMLeft(127 - MOVE_SPEED);
    UpdatePWMRight(127 + MOVE_SPEED);
}

void InitRightTurn() {
    DisableSpeedISR();
    UpdatePWMLeft(127 + MOVE_SPEED);
    UpdatePWMRight(127 - MOVE_SPEED);
}

void TimerDoStuff(TimerDo timerAction) {
    if(runningTimer) return;
    
    timerDo = timerAction;
    if(timerDo == TIMER_DO_IGNORE_SENSORS) {
        ignore = true;
    }
    
    TrackLED2_Write(0xff);
    
    StateMachineTimer_Start();
    runningTimer = true;
}

void StateMachine(bool reset) {
    static State current_state = STRAIGHT;
    static float driftErrorPrev = 0.0f;
    static int8_t prevVals[10];
    static bool hasInit = false;
    
    if(!hasInit) {
        StateMachineTimerInterrupt_StartEx(StateMachineTimerInterrupt);
        hasInit = true;
    }
    
    // Updated at end of function
    uint8_t sensors = PD_Read();
    
    //static char usbBuffer[255];
    //static int count = 0;+
    
    switch (current_state) {
        case STRAIGHT:
            if(0) {}
            
            int16_t pd1Drift = (255 - SensTimer1_ReadCapture()) / 5;
            int16_t pd2Drift = (255 - SensTimer2_ReadCapture()) / 5;
            
            int16_t driftErrorApprox = -1 * (-PD_ON(sensors, 1) * pd1Drift  + PD_ON(sensors, 2) * pd2Drift); 
            int8_t diff = (driftErrorApprox - driftErrorPrev);
            int8_t pid =  driftErrorApprox + diff;
            
            if(!ignore && PD_GET(sensors, 4) && PD_GET(sensors, 3)) {
                BoostRightMotor(-pid);
                BoostLeftMotor(pid);
            }
            
            driftErrorPrev = driftErrorApprox;
            
            if (!PD_GET(sensors, 4) && !ignore) {
                if(GetAction().type == ACTION_IGNORE_INTERSECTION) {
                    TimerDoStuff(TIMER_DO_NEXT_ACTION);
                    break;
                } else if (GetAction().type == ACTION_TURN_RIGHT) {
                    NextAction();
                    InitRightTurn();
                    current_state = TURN_RIGHT_START;
                    PRINT_STATE(TURN_RIGHT_START);
                    break;
                }
            }
            if (!PD_GET(sensors, 3) && !ignore) {
                if(GetAction().type == ACTION_IGNORE_INTERSECTION) {
                    TimerDoStuff(TIMER_DO_NEXT_ACTION);
                    break;              
                } else if (GetAction().type == ACTION_TURN_LEFT) {
                    NextAction();
                    InitLeftTurn();
                    current_state = TURN_LEFT_START;
                    PRINT_STATE(TURN_LEFT_START);
                    break;
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
            
            current_state = STRAIGHT;
            EnableSpeedISR();
            PRINT_STATE(STRAIGHT);
            SetTargetSpeeds(MOTOR_SPEED, MOTOR_SPEED);

            TimerDoStuff(TIMER_DO_IGNORE_SENSORS);
            
            break;
        case TURN_RIGHT_START:
            if(PD_GET(sensors, 7)) break;
            current_state = TURN_RIGHT_END;
        case TURN_RIGHT_END:
            if(PD_GET(sensors, 6)) break;
            
            UpdatePWMLeft(127);
            UpdatePWMRight(127);
            
            CyDelay(50);
            
            current_state = STRAIGHT;
            EnableSpeedISR();
            PRINT_STATE(STRAIGHT);
            SetTargetSpeeds(MOTOR_SPEED, MOTOR_SPEED);
            
            TimerDoStuff(TIMER_DO_IGNORE_SENSORS);
            break;
            
        default:
            ASSERT_MSG(false, "Invalid state reached");
    }
    
    if(current_state != TURN_LEFT_START && current_state != TURN_LEFT_END && current_state != TURN_RIGHT_START && current_state != TURN_RIGHT_END) MotorController();
}
