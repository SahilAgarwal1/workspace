/*
 * test.c
 *
 *  Created on: Mar 30, 2021
 *      Author: sahil
 */

#include <G8RTOS_Lab5/G8RTOS.h>
#include <stdlib.h>
#include "test.h"
#include "BSP.h"

semaphore_t lcd_mutex;
semaphore_t sensor_mutex;
bool LCDTapy = false;

void test(){
    int xStart = 50;
    int yStart = 50;
    int xEnd = 100;
    int yEnd = 100;
    while(1){
        G8RTOS_WaitSemaphore(&lcd_mutex);
        LCD_DrawRectangle(xStart, xEnd, yStart, yEnd, LCD_BLUE);
        G8RTOS_SignalSemaphore(&lcd_mutex);

    }
}

void test2(){
    int xStart = 50;
    int yStart = 50;
    int xEnd = 100;
    int yEnd = 100;
    while(1){
        G8RTOS_WaitSemaphore(&lcd_mutex);
        LCD_DrawRectangle(xStart, xEnd, yStart, yEnd, LCD_RED);
        G8RTOS_SignalSemaphore(&lcd_mutex);
    }
}

void test3(){
    Point p;
    while(1){
        G8RTOS_WaitSemaphore(&sensor_mutex);
        p = TP_ReadXY();
        LP3943_LedModeSet(BLUE, p.x);
        LP3943_LedModeSet(RED, p.y << 7);
        G8RTOS_SignalSemaphore(&sensor_mutex);
    }
}

void LCDTPIRQ(void)
{
    // If the interrupt flag for BIT0 is set
    if (P4->IFG & BIT0)
    {
        // Set the LCD flag
        LCDTapy = !LCDTapy;

        // And clear the flag
        P4->IFG &= ~BIT0;
    }
}


