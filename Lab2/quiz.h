/*
 * quiz.h
 *
 *  Created on: Feb 18, 2021
 *      Author: sahil
 */

#include "stdint.h"
#include "stdbool.h"
#include "G8RTOS_Semaphores.h"



#ifndef QUIZ_H_
#define QUIZ_H_


// variables
uint8_t counter0; // accessed in 3 functions, needs semaphore
uint8_t counter1; // accessed in 3 functions needs semaphore
uint8_t counter2; // accessed in three functions needs semaphore;
uint8_t counter3; // accessed in three functions needs semaphore
int sum;
bool reset;
// create semaphores

semaphore_t cnt_sem_0;
semaphore_t cnt_sem_1;
semaphore_t cnt_sem_2;
semaphore_t cnt_sem_3;
semaphore_t ledSem; // need sem for led so that the communication is not interrupted;


void Thread0_quiz(void);
void Thread1_quiz(void);
void Thread2_quiz(void);
void Thread3_quiz(void);
void resetThread(void);
void ledThread(void);





#endif /* QUIZ_H_ */
