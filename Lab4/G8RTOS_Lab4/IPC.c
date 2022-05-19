/*
 * IPC.c
 *
 *  Created on: Mar 11, 2021
 *      Author: sahil
 */

#include <G8RTOS_Lab4/IPC.h>

// Array of Fifos

//static FIFO_t fifoControlBlocks[MAX_FIFOS];

int init_Fifo(uint32_t index){
    if(index >= MAX_FIFOS){
        return -1;
    }

    // initialize the struct data
    fifoControlBlocks[index].Head = &fifoControlBlocks[index].Buffer[0];
    fifoControlBlocks[index].Tail = &fifoControlBlocks[index].Buffer[0];
    fifoControlBlocks[index].LostData = 0;
    G8RTOS_InitSemaphore(&fifoControlBlocks[index].CurrentSize, 0); // this semaphore will be signalled when the first write happens
    G8RTOS_InitSemaphore(&fifoControlBlocks[index].Mutex, 1); // this one is just a mutex


}

int32_t read_Fifo(uint32_t index){

    int32_t readData;

    // wait for the semaphores
    G8RTOS_WaitSemaphore(&fifoControlBlocks[index].CurrentSize);
    G8RTOS_WaitSemaphore(&fifoControlBlocks[index].Mutex);

    readData = *(fifoControlBlocks[index].Head);

    ++fifoControlBlocks[index].Head;

    if(fifoControlBlocks[index].Head >= &fifoControlBlocks[index].Buffer[FIFOSIZE]){
        fifoControlBlocks[index].Head = &fifoControlBlocks[index].Buffer[0];
    }

    G8RTOS_SignalSemaphore(&fifoControlBlocks[index].Mutex);

    return readData;
}


int write_FIfo(uint32_t index, int32_t data){
    G8RTOS_WaitSemaphore(&fifoControlBlocks[index].Mutex);

    if(fifoControlBlocks[index].CurrentSize > FIFOSIZE -1){
        fifoControlBlocks[index].LostData++;
        return -1;
    }

    (*fifoControlBlocks[index].Tail) = data;
    // increment tail.
    ++fifoControlBlocks[index].Tail;

    if (fifoControlBlocks[index].Tail >= &fifoControlBlocks[index].Buffer[FIFOSIZE]){
        fifoControlBlocks[index].Tail = &fifoControlBlocks[index].Buffer[0];
    }

    G8RTOS_SignalSemaphore(&fifoControlBlocks[index].CurrentSize);
    G8RTOS_SignalSemaphore(&fifoControlBlocks[index].Mutex);

    return 0;

}


