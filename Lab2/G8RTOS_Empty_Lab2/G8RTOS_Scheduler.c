/*
 * G8RTOS_Scheduler.c
 */

/*********************************************** Dependencies and Externs *************************************************************/
#include <stdint.h>
#include "msp.h"
#include <G8RTOS_Scheduler.h>
#include "G8RTOS_Structures.h"
#include <BSP.h>

/*
 * G8RTOS_Start exists in asm
 */
extern void G8RTOS_Start();

/* System Core Clock From system_msp432p401r.c */
extern uint32_t SystemCoreClock;

/*
 * Pointer to the currently running Thread Control Block
 */
extern tcb_t * CurrentlyRunningThread;

/*********************************************** Dependencies and Externs *************************************************************/


/*********************************************** Defines ******************************************************************************/

/* Status Register with the Thumb-bit Set */
#define THUMBBIT 0x01000000

/*********************************************** Defines ******************************************************************************/


/*********************************************** Data Structures Used *****************************************************************/

/* Thread Control Blocks
 *	- An array of thread control blocks to hold pertinent information for each thread
 */
static tcb_t threadControlBlocks[MAX_THREADS];

/* Thread Stacks
 *	- An array of arrays that will act as invdividual stacks for each thread
 */
static int32_t threadStacks[MAX_THREADS][STACKSIZE];


/*********************************************** Data Structures Used *****************************************************************/


/*********************************************** Private Variables ********************************************************************/

/*
 * Current Number of Threads currently in the scheduler
 */
static uint32_t NumberOfThreads;

/*********************************************** Private Variables ********************************************************************/


/*********************************************** Private Functions ********************************************************************/

/*
 * Initializes the Systick and Systick Interrupt
 * The Systick interrupt will be responsible for starting a context switch between threads
 * Param "numCycles": Number of cycles for each systick interrupt
 */
static void InitSysTick(uint32_t numCycles)
{
	/* Implement this */
    SysTick_Config(numCycles);
}

/*
 * Chooses the next thread to run.
 * Lab 2 Scheduling Algorithm:
 * 	- Simple Round Robin: Choose the next running thread by selecting the currently running thread's next pointer
 */
void G8RTOS_Scheduler()
{
    CurrentlyRunningThread = CurrentlyRunningThread->next;
}

/*
 * SysTick Handler
 * Currently the Systick Handler will only increment the system time
 * and set the PendSV flag to start the scheduler
 *
 * In the future, this function will also be responsible for sleeping threads and periodic threads
 */
void SysTick_Handler()
{
	/* Implement this */
    SystemTime++;
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

/*********************************************** Private Functions ********************************************************************/


/*********************************************** Public Variables *********************************************************************/

/* Holds the current time for the whole System */
uint32_t SystemTime;

/*********************************************** Public Variables *********************************************************************/


/*********************************************** Public Functions *********************************************************************/

/*
 * Sets variables to an initial state (system time and number of threads)
 * Enables board for highest speed clock and disables watchdog
 */
void G8RTOS_Init()
{
	/* Implement this */
    SystemTime = 0;
    NumberOfThreads = 0;
    BSP_InitBoard();
}

/*
 * Starts G8RTOS Scheduler
 * 	- Initializes the Systick
 * 	- Sets Context to first thread
 * Returns: Error Code for starting scheduler. This will only return if the scheduler fails
 */
int G8RTOS_Launch()
{
	/* Implement this */
    // set the current running thread pointer
    CurrentlyRunningThread = &threadControlBlocks[0];
    // initialize systick and set to 1 ms
    InitSysTick(ClockSys_GetSysFreq()/1000);
    // set systick and pendsv to lowest priorities
    __NVIC_SetPriority(PendSV_IRQn,OSINT_PRIORITY);
    __NVIC_SetPriority(SysTick_IRQn,OSINT_PRIORITY);

    //start os
    G8RTOS_Start();

    return -3;


}


/*
 * Adds threads to G8RTOS Scheduler
 * 	- Checks if there are stil available threads to insert to scheduler
 * 	- Initializes the thread control block for the provided thread
 * 	- Initializes the stack for the provided thread to hold a "fake context"
 * 	- Sets stack tcb stack pointer to top of thread stack
 * 	- Sets up the next and previous tcb pointers in a round robin fashion
 * Param "threadToAdd": Void-Void Function to add as preemptable main thread
 * Returns: Error code for adding threads
 */
int G8RTOS_AddThread(void (*threadToAdd)(void))
{
	/* Implement this */

    if(NumberOfThreads == MAX_THREADS){
        return -1;
    }
    else{
       if(NumberOfThreads == 0){
           threadControlBlocks[NumberOfThreads].next = &threadControlBlocks[NumberOfThreads];
           threadControlBlocks[NumberOfThreads].prev = &threadControlBlocks[NumberOfThreads];
       }
       else{
           // Round Robin
           threadControlBlocks[NumberOfThreads-1].next = &threadControlBlocks[NumberOfThreads];
           threadControlBlocks[NumberOfThreads].next = &threadControlBlocks[0];
           threadControlBlocks[NumberOfThreads].prev = &threadControlBlocks[NumberOfThreads-1];
           threadControlBlocks[0].prev = &threadControlBlocks[NumberOfThreads];
       }
    }

    // init stack to hold fake content

    threadStacks[NumberOfThreads][STACKSIZE - 1] = THUMBBIT; // The STATUS REG IS FIRST
    threadStacks[NumberOfThreads][STACKSIZE - 2] = (uint32_t)threadToAdd; // The PC
    // all the variable and scratch registers are 0

    int i;
    for(i = 3; i <= 16; i ++) threadStacks[NumberOfThreads][STACKSIZE - i] = 0;

    // set up the stack pointer for the thread control block;
    threadControlBlocks[NumberOfThreads].sp = &threadStacks[NumberOfThreads][STACKSIZE-16];

    // increment number of threads
    NumberOfThreads = NumberOfThreads + 1;

    return 0;

}

/*********************************************** Public Functions *********************************************************************/
