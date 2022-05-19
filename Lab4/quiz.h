/*
 * quiz.h
 *
 *  Created on: Apr 1, 2021
 *      Author: sahil
 */


#ifndef QUIZ_H_
#define QUIZ_H_

#include <stdlib.h>
#include "demo.h"
#include "BSP.h"
#include "G8RTOS_Lab4/G8RTOS.h"
#include "msp.h"

#define snake_fifo 0


void port4_setUp();
void ButtonTap_port4();
void ButtonTap_port5();
void drawSnake(uint16_t x, uint16_t y, uint16_t color);
void updateSnake();




typedef struct snake{
    uint16_t x;
    uint16_t y;
}snake;







#endif /* QUIZ_H_ */

