/*
 * quiz.c
 *
 *  Created on: Apr 1, 2021
 *      Author: sahil
 */
#include <G8RTOS_Lab5/G8RTOS.h>
#include <G8RTOS_Lab5/IPC.h>
#include <stdlib.h>
#include "demo.h"
#include "BSP.h"
#include "msp.h"
#include "quiz.h"
#include "stdint.h"


int direction = 0;
bool directionChange = false;
snake snakeBlock;

void port4_setUp(){

    P4->DIR &= ~(BIT4 | BIT5);//set direction
    P4->IFG &= ~(BIT4 | BIT5); // clear flag
    P4->IE |= (BIT4 | BIT5); // enable interrupt
    P4->IES |= (BIT4 | BIT5); // high to low transition
    P4->REN |= (BIT4 | BIT5); // PULL up resistor
    P4->OUT |= (BIT4 | BIT5); // set res to pull up

    P5->DIR &= ~(BIT4 | BIT5);//set direction
    P5->IFG &= ~(BIT4 | BIT5); // clear flag
    P5->IE |= (BIT4 | BIT5); // enable interrupt
    P5->IES |= (BIT4 | BIT5); // high to low transition
    P5->REN |= (BIT4 | BIT5); // PULL up resistor
    P5->OUT |= (BIT4 | BIT5); // set res to pull up
    snakeBlock.x = 50;
    snakeBlock.y = 50;


}

void ButtonTap_port4(){ // set flag for a touch
    if(P4->IFG & BIT4){
        P4->IFG &= ~BIT4;
        direction = 0;
    }
    else if(P4->IFG & BIT5){
        P4->IFG &= ~BIT5;
        direction = 1;
    }
    directionChange = true;
}

void ButtonTap_port5(){ // set flag for a touch
    if(P5->IFG & BIT4){
        P5->IFG &= ~BIT4;
        direction = 2;
    }
    else if(P5->IFG & BIT5){
        P5->IFG &= ~BIT5;
        direction = 3;
    }
    directionChange = true;
}

void drawSnake(uint16_t x, uint16_t y, uint16_t color){
    G8RTOS_WaitSemaphore(&lcd_sem);
    LCD_DrawRectangle(x, x+4, y, y+4, color);
    G8RTOS_SignalSemaphore(&lcd_sem);
}

void updateSnake(){
    LCD_Clear(LCD_BLACK);
    while(1){
        if(directionChange){
            int tempDirection = direction;
            snake tempSnake = snakeBlock;

            switch(direction){
            case 0 :
                if(tempDirection != 2)snakeBlock.y -= 4;
                else {
                    direction = tempDirection;
                    snakeBlock.y += 4;
                }
                break;
            case 1 :
                if(tempDirection != 3) snakeBlock.x += 4;
                else {
                    direction = tempDirection;
                    snakeBlock.x -=4;
                }
                break;
            case 2 :
                if(tempDirection != 0) snakeBlock.y += 4;
                else {
                    direction = tempDirection;
                    snakeBlock.y -= 4;
                }
                break;
            case 3 :
                if(tempDirection != 1) snakeBlock.x -= 4;
                else{
                    direction = tempDirection;
                    snakeBlock.x += 4;
                }
                break;
            }

            if(snakeBlock.x >= MAX_SCREEN_X) snakeBlock.x = 0;
            else if(snakeBlock.x <= 0) snakeBlock.x = MAX_SCREEN_X;
            if(snakeBlock.y >= MAX_SCREEN_Y) snakeBlock.y = 0;
            else if(snakeBlock.y <= 0) snakeBlock.y = MAX_SCREEN_Y;

            /*

            // write to the fifo
            if(fifoControlBlocks[snake_fifo].CurrentSize == 256){
                uint32_t toDelete = read_Fifo(snake_fifo);
                drawSnake(toDelete >> 16, toDelete & 0xFFFF, LCD_BLACK);
            }
            write_FIfo(snake_fifo, snakeBlock.x << 16 | snakeBlock.y);

            //

            int i;
            for(i = 0; i < fifoControlBlocks[snake_fifo].CurrentSize; i ++ ){
                drawSnake(fifoControlBlocks[snake_fifo].Buffer[i] >> 16, fifoControlBlocks[snake_fifo].Buffer[i] & 0xFFFF , LCD_WHITE);
            }
            */

            drawSnake(tempSnake.x, tempSnake.y, LCD_BLACK);
            drawSnake(snakeBlock.x, snakeBlock.y, LCD_WHITE);


            OS_Sleep(30);
            directionChange = false;
        }
        else{
            snake tempSnake = snakeBlock;

            switch(direction){
            case 0 :
                snakeBlock.y -= 4;
                break;
            case 1 :
                snakeBlock.x += 4;
                break;
            case 2 :
                snakeBlock.y += 4;
                break;
            case 3 :
                snakeBlock.x -= 4;
                break;
            }

            if(snakeBlock.x >= MAX_SCREEN_X) snakeBlock.x = 0;
            else if(snakeBlock.x <= 0) snakeBlock.x = MAX_SCREEN_X;
            if(snakeBlock.y >= MAX_SCREEN_Y) snakeBlock.y = 0;
            else if(snakeBlock.y <= 0) snakeBlock.y = MAX_SCREEN_Y;

            /*
            // write to the fifo
            if(fifoControlBlocks[snake_fifo].CurrentSize == 256){
                uint32_t toDelete = read_Fifo(snake_fifo);
                drawSnake(toDelete >> 16, toDelete & 0xFFFF, LCD_BLACK);
            }
            write_FIfo(snake_fifo, snakeBlock.x << 16 | snakeBlock.y);

            //

            int i;
            for(i = 0; i < fifoControlBlocks[snake_fifo].CurrentSize; i ++ ){
                drawSnake(fifoControlBlocks[snake_fifo].Buffer[i] >> 16, fifoControlBlocks[snake_fifo].Buffer[i] & 0xFFFF , LCD_WHITE);
            }
            */

            drawSnake(tempSnake.x, tempSnake.y, LCD_BLACK);
            drawSnake(snakeBlock.x, snakeBlock.y, LCD_WHITE);


            OS_Sleep(30);
            directionChange = false;

        }

    }
}






