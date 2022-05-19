/*
 * G8RTOS_Scheduler.c
 */

/*********************************************** Dependencies and Externs *************************************************************/
#include <stdint.h>
#include <string.h>
#include "msp.h"
#include <BSP.h>
#include <G8RTOS_Lab4/G8RTOS_Scheduler.h>
#include <G8RTOS_Lab4/G8RTOS_Structures.h>

/*
 * G8RTOS_Start exists in asm
 */
extern void G8RTOS_Start();

/* System Core Clock From system_msp432p401r.c */
extern uint32_t SystemCoreClock;
static uint16_t IDCounter;

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

// pthreads Control Blocks
static ptcb_t pthreadControlBlocks[MAX_THREADS];

//



/*********************************************** Data Structures Used *****************************************************************/


/*********************************************** Private Variables ********************************************************************/

/*
 * Current Number of Threads currently in the scheduler
 */
static uint32_t NumberOfThreads;

static uint32_t pNumberOfThreads;

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
    uint16_t nextThreadPriority = 256;
    tcb_t* tempNextThread = CurrentlyRunningThread->next;
    int i;

    for(i = 0; i < NumberOfThreads; i++){ // for every thread might need to change
        if(tempNextThread->isAlive && (tempNextThread->blocked == 0) && !tempNextThread->asleep){ // if next thread is not blocked or asleep
            if(tempNextThread->priority < nextThreadPriority){ // check priority
                CurrentlyRunningThread = tempNextThread;
                nextThreadPriority = CurrentlyRunningThread->priority;
            }
        } // check for blocked or asleep
        tempNextThread = tempNextThread->next;
    } // thread iterator

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
    int i;
    // handle periodic threads
    ptcb_t * Pptr = &pthreadControlBlocks[0]; // the first p thread

    for(i = 0; i < pNumberOfThreads; i++){
        if(Pptr->execTime == SystemTime){
            Pptr->execTime = SystemTime + Pptr->period;
            (Pptr->Handler)();
        }
        Pptr = Pptr-> next;
    }

    // handle the sleeping threads

    tcb_t* thread_ptr = CurrentlyRunningThread;

    for(i = 0; i < NumberOfThreads; i++){
        if(thread_ptr->asleep && thread_ptr->sleepCount == SystemTime){
            thread_ptr -> asleep = false;
        }
        thread_ptr = thread_ptr-> next;
    }

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
void G8RTOS_Init(bool usingTP)
{
	/* Implement this */
    SystemTime = 0;
    pNumberOfThreads = 0;

    // Relocate the VTOR table to SRAM
    uint32_t newVTORTable = 0x20000000;
    // 57 interrupt vectors to copy
    memcpy((uint32_t *)newVTORTable, (uint32_t *)SCB->VTOR, 57*4);
    SCB->VTOR = newVTORTable;

    BSP_InitBoard(usingTP);
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





int G8RTOS_AddThread(void (*threadToAdd)(void), uint8_t priority, char* thread_name)
{
    int i;
    int tcbToInitialize = MAX_THREADS;
	//Implement this
    int32_t ibit = StartCriticalSection();

    //max number of threads has been exceeded
    if(NumberOfThreads >= MAX_THREADS){
        EndCriticalSection(ibit);
        return THREAD_LIMIT_REACHED;
    }

    //if the max number of threads hasnt been reached, we must check to see if their are dead threads
    for(i = 0; i < MAX_THREADS; i++){
        if(!threadControlBlocks[i].isAlive){ // checks for dead threads or threads that havent been initialized
            tcbToInitialize = i;
            break;
        }
    }

    if(tcbToInitialize == MAX_THREADS){
        EndCriticalSection(ibit);
        return THREADS_INCORRECTLY_ALIVE;
    }

    // if there are no threads then point the thread at itself
    if(NumberOfThreads == 0){
        threadControlBlocks[tcbToInitialize].next = &threadControlBlocks[tcbToInitialize];
        threadControlBlocks[tcbToInitialize].prev = &threadControlBlocks[tcbToInitialize];
    }
    else{
        for(i = 0; i < MAX_THREADS; i++){
            if(threadControlBlocks[i].isAlive){ // if there is an alive thread
                threadControlBlocks[tcbToInitialize].next = threadControlBlocks[i].next;
                threadControlBlocks[tcbToInitialize].prev = &threadControlBlocks[i];
                threadControlBlocks[i].next->prev = &threadControlBlocks[tcbToInitialize];
                threadControlBlocks[i].next = &threadControlBlocks[tcbToInitialize];
                break;
            }
        }
    }


    // init stack to hold fake content

    threadStacks[tcbToInitialize][STACKSIZE - 1] = THUMBBIT; // The STATUS REG IS FIRST
    threadStacks[tcbToInitialize][STACKSIZE - 2] = (uint32_t)threadToAdd; // The PC
    // all the variable and scratch registers are 0

    for(i = 3; i <= 16; i ++) threadStacks[tcbToInitialize][STACKSIZE - i] = 0;

    // set up the stack pointer for the thread control block;
    threadControlBlocks[tcbToInitialize].sp = &threadStacks[tcbToInitialize][STACKSIZE-16];

    // set up the tcb properties
    strcpy(threadControlBlocks[tcbToInitialize].Threadname, thread_name);
    threadControlBlocks[tcbToInitialize].ThreadID = ((IDCounter++) << 16) | tcbToInitialize;
    threadControlBlocks[tcbToInitialize].blocked = 0;
    threadControlBlocks[tcbToInitialize].asleep = false;
    threadControlBlocks[tcbToInitialize].isAlive = true;
    threadControlBlocks[tcbToInitialize].priority = priority;




    // increment number of threads
    NumberOfThreads = NumberOfThreads + 1;
    EndCriticalSection(ibit);

    return NO_ERROR;

}




// this function mirrors the Add thread function



int32_t G8RTOS_AddPThread(void (*PthreadToAdd)(void), uint32_t period){

    if(pNumberOfThreads == MAX_THREADS){
        return -1;
    }
    else{
       if(pNumberOfThreads == 0){
           pthreadControlBlocks[pNumberOfThreads].next = &pthreadControlBlocks[pNumberOfThreads];
           pthreadControlBlocks[pNumberOfThreads].prev = &pthreadControlBlocks[pNumberOfThreads];
       }
       else{
           // Round Robin
           pthreadControlBlocks[pNumberOfThreads-1].next = &pthreadControlBlocks[pNumberOfThreads];
           pthreadControlBlocks[pNumberOfThreads].next = &pthreadControlBlocks[0];
           pthreadControlBlocks[pNumberOfThreads].prev = &pthreadControlBlocks[pNumberOfThreads-1];
           pthreadControlBlocks[0].prev = &pthreadControlBlocks[pNumberOfThreads];
       }
    }

    pthreadControlBlocks[pNumberOfThreads].execTime = SystemTime + period;
    pthreadControlBlocks[pNumberOfThreads].period = period;
    pthreadControlBlocks[pNumberOfThreads].Handler = PthreadToAdd;

    pNumberOfThreads++;

}


void OS_Sleep(uint32_t duration){
    // set the time
    CurrentlyRunningThread -> sleepCount = duration + SystemTime;
    // set the thread to sleep
    CurrentlyRunningThread -> asleep = true;
    // yield thread
    SCB -> ICSR |= SCB_ICSR_PENDSVSET_Msk;
}

threadId_t G8RTOS_GetThreadID(){
    return CurrentlyRunningThread->ThreadID;
}


int G8RTOS_KillThread(threadId_t threadID){
    int32_t ibit = StartCriticalSection();
    int i;
    // check for one thread left
    if(NumberOfThreads == 1){
        EndCriticalSection(ibit);
        return CANNOT_KILL_LAST_THREAD;
    }

    int kThread = MAX_THREADS;
    for(i = 0; i < MAX_THREADS; i++){
        if(threadControlBlocks[i].ThreadID == threadID){
            kThread = i;
            break;
        }
    }
    if(kThread == MAX_THREADS){
        return THREAD_DOES_NOT_EXIST;
    }

    threadControlBlocks[kThread].isAlive = false;

    // switch the next and prevs around
    threadControlBlocks[kThread].prev->next = threadControlBlocks[kThread].next;
    threadControlBlocks[kThread].next->prev = threadControlBlocks[kThread].prev;

    if(&threadControlBlocks[kThread] == CurrentlyRunningThread){ // check
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    }

    NumberOfThreads--;

    EndCriticalSection(ibit);

    return NO_ERROR;


}

int G8RTOS_KillSelf(){
    return G8RTOS_KillThread(CurrentlyRunningThread->ThreadID);
}

int G8RTOS_AddAPeriodicEvent(void (*AthreadToAdd)(void), uint8_t priority, IRQn_Type IRQn){
    int32_t ibit = StartCriticalSection();

    if(IRQn > PORT6_IRQn || IRQn < PSS_IRQn){
        EndCriticalSection(ibit);
        return IRQn_INVALID;
    }

    if(priority > 6){
        EndCriticalSection(ibit);
        return HWI_PRIORITY_INVALID;
    }

    __NVIC_SetVector(IRQn, (uint32_t)AthreadToAdd);
    __NVIC_SetPriority(IRQn,(uint32_t)priority);
    __NVIC_EnableIRQ(IRQn);

    EndCriticalSection(ibit);
    return NO_ERROR;
}


/*********************************************** Public Functions *********************************************************************/
