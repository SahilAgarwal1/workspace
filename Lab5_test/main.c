/*
 * main.c
 *
 *  Created on: Feb 6, 2021
 *      Author: Kyle
 */

#include "G8RTOS_Lab5/G8RTOS.h"
#include <BSP.h>
#include <LCDLib.h>
#include <Game.h>


#define IS_HOST 0


//uint8_t test;

void main() {
    G8RTOS_Init(0);
    LCD_Init(0);

    if(IS_HOST)
        G8RTOS_AddThread(CreateGame, 2, "Create Game");
    else
        G8RTOS_AddThread(JoinGame, 2, "Join Game");


    sched_ErrCode_t error_code = G8RTOS_Launch();

//    initCC3100(Host);
//
//    // LED indicator
//    initLEDs();
//    turnLedOn(LED2);
//
//    while (1) {
//        int32_t retValtest = NOTHING_RECEIVED;
//
//        // Wait for data
//        while (retValtest == NOTHING_RECEIVED){
//            retValtest = ReceiveData(&test, sizeof(test));
//            Delay(50);
//        }
//
//        LCD_Clear(LCD_RED);
//        LCD_Clear(LCD_BLACK);
//    }
}

