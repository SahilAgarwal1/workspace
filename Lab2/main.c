#include "msp.h"
#include "BSP.h"
#include "G8RTOS.h"
#include "Threads.h"
#include "driverlib.h"
#include "quiz.h"


/**
 * main.c
 */





void main(void)
{
    G8RTOS_Init();



    G8RTOS_InitSemaphore(&cnt_sem_0, 1);
    G8RTOS_InitSemaphore(&cnt_sem_1, 1);
    G8RTOS_InitSemaphore(&cnt_sem_2, 1);
    G8RTOS_InitSemaphore(&cnt_sem_3, 1);
    G8RTOS_InitSemaphore(&ledSem, 1);


    G8RTOS_AddThread(&Thread0_quiz);
    G8RTOS_AddThread(&Thread1_quiz);
    G8RTOS_AddThread(&Thread2_quiz);
    G8RTOS_AddThread(&Thread3_quiz);
    G8RTOS_AddThread(&resetThread);
    G8RTOS_AddThread(&ledThread);


    // comment out above and uncomment below for the lab demo;

    /*

    G8RTOS_InitSemaphore(&ledSemaphore, 1);
    G8RTOS_InitSemaphore(&i2cSemaphore, 1);

    G8RTOS_AddThread(&Thread0);
    G8RTOS_AddThread(&Thread1);
    G8RTOS_AddThread(&Thread2);

*/

    G8RTOS_Launch();



}
