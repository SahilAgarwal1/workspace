/*
 * G8RTOS_Scheduler.h
 */

#ifndef G8RTOS_SCHEDULER_H_
#define G8RTOS_SCHEDULER_H_

/*********************************************** Sizes and Limits *********************************************************************/
#define MAX_THREADS 30 // was 6
#define STACKSIZE 256
#define OSINT_PRIORITY 7
/*********************************************** Sizes and Limits *********************************************************************/
#include "stdint.h"
#include "stdbool.h"
#include <G8RTOS_Lab5/G8RTOS_Structures.h>
#include "msp.h"
/*********************************************** Public Variables *********************************************************************/

/* Holds the current time for the whole System */
extern uint32_t SystemTime;

typedef uint32_t threadId_t;

/*********************************************** Public Variables *********************************************************************/


/*********************************************** Public Functions *********************************************************************/

/*
 * Initializes variables and hardware for G8RTOS usage
 */
void G8RTOS_Init(bool usingTP);

/*
 * Starts G8RTOS Scheduler
 * 	- Initializes Systick Timer
 * 	- Sets Context to first thread
 * Returns: Error Code for starting scheduler. This will only return if the scheduler fails
 */
int32_t G8RTOS_Launch();

/*
 * Adds threads to G8RTOS Scheduler
 * 	- Checks if there are stil available threads to insert to scheduler
 * 	- Initializes the thread control block for the provided thread
 * 	- Initializes the stack for the provided thread
 * 	- Sets up the next and previous tcb pointers in a round robin fashion
 * Param "threadToAdd": Void-Void Function to add as preemptable main thread
 * Returns: Error code for adding threads
 */
int G8RTOS_AddThread(void (*threadToAdd)(void), uint8_t priority, char* thread_name);

// Add p thread to the list

int32_t G8RTOS_AddPThread(void (*PthreadToAdd)(void), uint32_t period);

// OS sleep function, puts a thread to sleep for the amount of time duration

void OS_Sleep(uint32_t duration);

threadId_t G8RTOS_GetThreadID();

int G8RTOS_KillThread(threadId_t threadID);

int G8RTOS_AddAPeriodicEvent(void (*AthreadToAdd)(void), uint8_t priority, IRQn_Type IRQn);

/*********************************************** Public Functions *********************************************************************/

#endif /* G8RTOS_SCHEDULER_H_ */
