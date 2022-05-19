/*
 * test.h
 *
 *  Created on: Mar 30, 2021
 *      Author: sahil
 */

#ifndef TEST_H_
#define TEST_H_

#include "G8RTOS_Lab4/G8RTOS_Structures.h"

semaphore_t sensor_mutex;
semaphore_t lcd_mutex;

void test();
void test2();
void test3();
void test4();
void LCDTPIRQ(void);



#endif /* TEST_H_ */
