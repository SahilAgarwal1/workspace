/*
 * demo.h
 *
 *  Created on: Mar 30, 2021
 *      Author: sahil
 */

#ifndef DEMO_H_
#define DEMO_H_

#include <G8RTOS_Lab5/G8RTOS_Structures.h>

#define MAX_BALLS 20
#define threshold 1000
#define ball_fifo 0

semaphore_t sensor_mutex;
semaphore_t lcd_sem;



typedef struct ball_t{
    int32_t x;
    int32_t y;
    int32_t x_v;
    int32_t y_v;
    bool alive;
    threadId_t threadID;
    uint16_t color;
    uint16_t scalar;
}ball_t;


void tester();
static void Delay(unsigned long interval);
int generate_random(int l, int r);
uint32_t distanceFunction(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
int16_t mapToRange(int16_t X);
void calibrate_sensors();
void Read_Accel();
void LCDTap();
void WaitForTap();
void ball();
void idle();





#endif /* DEMO_H_ */
