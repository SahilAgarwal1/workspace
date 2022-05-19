/*
 * G8RTOS_Semaphores.c
 */

/*********************************************** Dependencies and Externs *************************************************************/

#include "G8RTOS_CriticalSection.h"
#include "G8RTOS_Semaphores.h"
#include "G8RTOS_Structures.h"
#include <stdint.h>
#include "msp.h"

/*********************************************** Dependencies and Externs *************************************************************/


/*********************************************** Public Functions *********************************************************************/

/*
 * Initializes a semaphore to a given value
 * Param "s": Pointer to semaphore
 * Param "value": Value to initialize semaphore to
 * THIS IS A CRITICAL SECTION
 */
void G8RTOS_InitSemaphore(semaphore_t *s, int32_t value)
{
	int32_t i; // the return value of the critical section function
	i = StartCriticalSection(); // start critical section
	(*s) = value; // set val
	EndCriticalSection(i); // end section

}

/*
 * Waits for a semaphore to be available (value greater than 0)
 * 	- Decrements semaphore when available
 * 	- Spinlocks to wait for semaphore
 * Param "s": Pointer to semaphore to wait on
 * THIS IS A CRITICAL SECTION
 */
void G8RTOS_WaitSemaphore(semaphore_t *s)
{
    int32_t i;
    i = StartCriticalSection(); // start critical section
    (*s)--; // decrement semaphore

    if((*s) < 0){
        // Block Thread
        CurrentlyRunningThread->blocked = s;
        // Start the context Switch
        EndCriticalSection(i); // possibly not needed
        SCB -> ICSR |= SCB_ICSR_PENDSVSET_Msk;
    }
    else{

        EndCriticalSection(i);
    }

}

/*
 * Signals the completion of the usage of a semaphore
 * 	- Increments the semaphore value by 1
 * Param "s": Pointer to semaphore to be signalled
 * THIS IS A CRITICAL SECTION
 */
void G8RTOS_SignalSemaphore(semaphore_t *s)
{
	int32_t i;
	i = StartCriticalSection();

	(*s)++;

	if((*s) <= 0){
	    tcb_t *pt = CurrentlyRunningThread -> next;
	    while(pt->blocked != s){
	        pt = pt-> next;
	    }

	    pt -> blocked = 0;
	}

	EndCriticalSection(i);

}

/*********************************************** Public Functions *********************************************************************/


