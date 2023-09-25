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

typedef struct Action {
    enum {
        ACTION_NOTHING,
        ACTION_CHANGE_SPEED,
        ACTION_TURN_LEFT,
        ACTION_TURN_RIGHT
    } actionType;
    
    float leftSpeed, rightSpeed;
    float motorBoostLeft, motorBoostRight;
} Action;

void StateMachine(bool reset);