#include "G8RTOS_Semaphores.h"

semaphore_t i2cSemaphore;
semaphore_t ledSemaphore;


void Thread0();
void Thread1();
void Thread2();
