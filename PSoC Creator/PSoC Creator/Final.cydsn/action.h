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

typedef struct Action {
    enum {
        ACTION_NOTHING,
        ACTION_CHANGE_SPEED
    } actionType;
    
    float leftSpeed, rightSpeed;
    float motorBoostLeft, motorBoostRight;
} Action;

Action StateMachine();