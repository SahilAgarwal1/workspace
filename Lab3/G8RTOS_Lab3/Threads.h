/*
 * Threads.h
 *
 *  Created on: Mar 11, 2021
 *      Author: sahil
 */

#ifndef G8RTOS_LAB3_THREADS_H_
#define G8RTOS_LAB3_THREADS_H_

#include <G8RTOS_Lab3/G8RTOS.h>

#define JOYSTICK_FIFO 0
#define TEMP_FIFO 1
#define LIGHT_FIFO 2

semaphore_t sensorMutex;
semaphore_t LEDMutex;

void BackgroundThread0();
void BackgroundThread1();
void BackgroundThread2();
void BackgroundThread3();
void BackgroundThread4();
void YoloSwag();

void pthread0();
void pthread1();




#endif /* G8RTOS_LAB3_THREADS_H_ */


