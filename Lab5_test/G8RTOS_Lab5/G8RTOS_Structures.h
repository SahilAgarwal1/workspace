/*
 * G8RTOS_Structure.h
 *
 *  Created on: Jan 12, 2017
 *      Author: Raz Aloni
 */

#ifndef G8RTOS_STRUCTURES_H_
#define G8RTOS_STRUCTURES_H_

#include <G8RTOS_Lab5/G8RTOS.h>
#include <G8RTOS_Lab5/G8RTOS_Semaphores.h>
#include <stdbool.h>

/*********************************************** Data Structure Definitions ***********************************************************/

/*
 *  Thread Control Block:
 *      - Every thread has a Thread Control Block
 *      - The Thread Control Block holds information about the Thread Such as the Stack Pointer, Priority Level, and Blocked Status
 *      - For Lab 2 the TCB will only hold the Stack Pointer, next TCB and the previous TCB (for Round Robin Scheduling)
 */

/* Create tcb struct here */

typedef struct tcb_t{
    struct tcb_t* prev;
    struct tcb_t* next;
    int32_t* sp;
    semaphore_t *blocked;
    bool asleep;
    uint32_t sleepCount;
    bool isAlive;
    uint8_t priority;
   uint32_t ThreadID;
   char Threadname[16];

}tcb_t;


typedef struct ptcb_t{
    void (*Handler)(void);
    uint32_t period;
    uint32_t execTime;
    uint32_t curTime;
    struct ptcb_t * prev;
    struct ptcb_t * next;
} ptcb_t;

/** Error Codes for Scheduler*/

typedef enum sched_ErrCode_t{
    NO_ERROR = 0,
    THREAD_LIMIT_REACHED        =   -1,
    NO_THREADS_SCHEDULED       =   -2,
    THREADS_INCORRECTLY_ALIVE =   -3,
    THREAD_DOES_NOT_EXIST      =   -4,
    CANNOT_KILL_LAST_THREAD    =   -5,
    IRQn_INVALID              =  -6,
    HWI_PRIORITY_INVALID     =   -7} sched_ErrCode_t;



/*********************************************** Data Structure Definitions ***********************************************************/


/*********************************************** Public Variables *********************************************************************/

tcb_t * CurrentlyRunningThread;


/*********************************************** Public Variables *********************************************************************/




#endif /* G8RTOS_STRUCTURES_H_ */
