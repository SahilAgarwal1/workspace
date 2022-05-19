/*
 * IPC.h
 *
 *  Created on: Mar 11, 2021
 *      Author: sahil
 */

#ifndef G8RTOS_LAB5_IPC_H_
#define G8RTOS_LAB5_IPC_H_

#include <G8RTOS_Lab5/G8RTOS.h>
#include "stdint.h"

#define FIFOSIZE 256
#define MAX_FIFOS 4

static struct FIFO_t fifoControlBlocks[MAX_FIFOS];


typedef struct FIFO_t{
    int32_t Buffer[FIFOSIZE];
    int32_t *Head;
    int32_t *Tail;
    uint32_t LostData;
    semaphore_t CurrentSize;
    semaphore_t Mutex;
}FIFO_t;


// functions

int init_Fifo(uint32_t index);
int32_t read_Fifo(uint32_t index);
int write_FIfo(uint32_t index, int32_t data);




#endif /* G8RTOS_LAB5_IPC_H_ */
