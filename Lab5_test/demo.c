/*
 * demo.c
 *
 *  Created on: Mar 30, 2021
 *      Author: sahil
 */

#include <G8RTOS_Lab5/G8RTOS.h>
#include <stdlib.h>
#include "demo.h"
#include "BSP.h"
#include "msp.h"


#define maxSpeed 5


int16_t initXaccel, initYaccel,xAccel, yAccel;
bool TP_FLAG = false;
static ball_t ballControlBlocks[MAX_BALLS];
int NumberOfBalls=0;

int generate_random(int l, int r) { //this will generate random number in range l and r
      return (rand() % (r - l + 1)) + l;
}

static void Delay(unsigned long interval)
{
    while(interval > 0)
    {
        __delay_cycles(48000);
        interval--;
    }
}

uint32_t distanceFunction(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2){
    return ((x1-x2)*(x1-x2)) - ((y1-y2)*(y1-y2));
}


void Read_Accel(){ // read the accellerations
    bmi160_read_accel_x(&initXaccel);
    bmi160_read_accel_y(&initYaccel);
    while(1){
        // if we have calibrated the board then start reading
            bmi160_read_accel_x(&xAccel);
            bmi160_read_accel_y(&yAccel);
            OS_Sleep(33);
    }
}

void LCDTap(){ // set flag for a touch
    Delay(10);
    if(P4->IFG & BIT0){
        P4->IFG &= ~BIT0;
        TP_FLAG = true;
    }
}

void tester(){
    while(1){
        TP_FLAG = true;
        OS_Sleep(1200);
    }
}

void WaitForTap(){
    int i;
    while(1){
        if(TP_FLAG){
            G8RTOS_WaitSemaphore(&lcd_sem); // read the touched areas

            Point p = TP_ReadXY();
            G8RTOS_SignalSemaphore(&lcd_sem);


            // check if we need to kill or create a ball
            int kball = MAX_BALLS;
            for(i = 0; i < MAX_BALLS; i++){
                if(ballControlBlocks[i].alive && (distanceFunction(ballControlBlocks[i].x, ballControlBlocks[i].y, p.x, p.y) < threshold)){
                    kball = i;
                    break;
                }
            } // for loop checking to create ball

            if(kball == MAX_BALLS & NumberOfBalls < MAX_BALLS){ // check for max balls;
                write_FIfo(ball_fifo ,p.x << 16 | p.y);
                G8RTOS_AddThread(&ball, 20, "ball");
                NumberOfBalls++;

            }

            else if(kball != MAX_BALLS){

                G8RTOS_WaitSemaphore(&lcd_sem);
                G8RTOS_KillThread(ballControlBlocks[kball].threadID);
                LCD_DrawRectangle(ballControlBlocks[kball].x, ballControlBlocks[kball].x+4, ballControlBlocks[kball].y, ballControlBlocks[kball].y+4, LCD_BLACK);
                G8RTOS_SignalSemaphore(&lcd_sem);


                ballControlBlocks[kball].alive = false;
                NumberOfBalls--;


            }

            OS_Sleep(500);
            TP_FLAG = false;
        }
    }
}

void ball(){
    int deadball = MAX_BALLS;
    int i;

    for(i = 0; i < MAX_BALLS; i++){
        if(!ballControlBlocks[i].alive){
            deadball = i;
            break;
        }
    }

    int32_t spawn = read_Fifo(ball_fifo);

    if(deadball == MAX_BALLS) G8RTOS_KillSelf();

    // initialize the ball

    ballControlBlocks[deadball].x = spawn >> 16;
    ballControlBlocks[deadball].y = spawn & 0xFFFF;
    ballControlBlocks[deadball].x_v = 1;
    ballControlBlocks[deadball].y_v = 1;
    ballControlBlocks[deadball].threadID = G8RTOS_GetThreadID();
    ballControlBlocks[deadball].alive = true;
    ballControlBlocks[deadball].scalar = generate_random(1, maxSpeed);
    ballControlBlocks[deadball].color = (uint16_t) rand();

    // update the frames

    while(1){
        G8RTOS_WaitSemaphore(&lcd_sem);
        LCD_DrawRectangle(ballControlBlocks[deadball].x, ballControlBlocks[deadball].x+4, ballControlBlocks[deadball].y, ballControlBlocks[deadball].y+4, LCD_BLACK);


        if(xAccel >= initXaccel) ballControlBlocks[deadball].x_v = 1 * ballControlBlocks[deadball].scalar;
        else ballControlBlocks[deadball].x_v = -1 * ballControlBlocks[deadball].scalar;
        if(yAccel >= initYaccel) ballControlBlocks[deadball].y_v = -1 * ballControlBlocks[deadball].scalar;
        else ballControlBlocks[deadball].y_v = 1 * ballControlBlocks[deadball].scalar;


        // update positions


        ballControlBlocks[deadball].x = ballControlBlocks[deadball].x + ballControlBlocks[deadball].x_v;
        ballControlBlocks[deadball].y = ballControlBlocks[deadball].y + ballControlBlocks[deadball].y_v;

        if(ballControlBlocks[deadball].x >= MAX_SCREEN_X) ballControlBlocks[deadball].x = 0;
        else if(ballControlBlocks[deadball].x <= 0) ballControlBlocks[deadball].x = MAX_SCREEN_X;
        if(ballControlBlocks[deadball].y >= MAX_SCREEN_Y) ballControlBlocks[deadball].y = 0;
        else if(ballControlBlocks[deadball].y <= 0) ballControlBlocks[deadball].y = MAX_SCREEN_Y;


        LCD_DrawRectangle(ballControlBlocks[deadball].x, ballControlBlocks[deadball].x+4, ballControlBlocks[deadball].y, ballControlBlocks[deadball].y+4, ballControlBlocks[deadball].color);
        G8RTOS_SignalSemaphore(&lcd_sem);


        OS_Sleep(33);// frame rate ;

    }



}

void idle(){
    while(1);
}




