#include "msp.h"
#include "G8RTOS_Lab3/G8RTOS.h"
#include "G8RTOS_Lab3/Threads.h"

/**
 * main.c
 */



void main(void)
{

    G8RTOS_Init();
    // init ports
    P4->DIR |= BIT7;

    init_Fifo(JOYSTICK_FIFO);
    init_Fifo(TEMP_FIFO);
    init_Fifo(LIGHT_FIFO);

    G8RTOS_InitSemaphore(&LEDMutex, 1);
    G8RTOS_InitSemaphore(&sensorMutex, 1);

    // add our normal threads
    G8RTOS_AddThread(&BackgroundThread0);
    G8RTOS_AddThread(&BackgroundThread1);
    G8RTOS_AddThread(&BackgroundThread2);
    G8RTOS_AddThread(&BackgroundThread3);
    G8RTOS_AddThread(&BackgroundThread4);
    G8RTOS_AddThread(&YoloSwag);

    // add our p events
    G8RTOS_AddPThread(&pthread0, 100);
    G8RTOS_AddPThread(&pthread1, 1000);

    G8RTOS_Launch();
}




