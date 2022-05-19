/*
 * Threads.c
 *
 *  Created on: Mar 11, 2021
 *      Author: sahil
 */


#include <stdint.h>
#include "G8RTOS.h"
#include <BSP.h>
#include "Threads.h"
#include "msp.h"
#include "stdbool.h"

int32_t avg = 0;
bool flag = false;
int32_t temp_f;


static int squareRoot(int n){
    int xk1 = n;
    int xk;

    do{
        xk = xk1;
        xk1 = (xk + (n/xk))/2;
    } while(abs(xk1 - xk) >= 1);

    return xk1;
}


void BackgroundThread0(){
    int32_t temp;
    while(1){

        // Wait semaphore
        G8RTOS_WaitSemaphore(&sensorMutex); // wait for the semaphore to be free

        //Get the temp
        bme280_read_uncomp_temperature(&temp); // read temp
        temp = bme280_compensate_temperature_int32(temp); // get temp in cels
        temp = (((temp/100) * 9)/5) + 32; //change to F

        // release the semaphore
        G8RTOS_SignalSemaphore(&sensorMutex);

        // write to FIFO
        write_FIfo(TEMP_FIFO, temp);

        //toggle pin
        BITBAND_PERI(P4->OUT,7) = ~BITBAND_PERI(P4->OUT,7);

        //Sleep
        OS_Sleep(500);

    }
}

void BackgroundThread1(){
    uint16_t light;
    while(1){
        // get data
        G8RTOS_WaitSemaphore(&sensorMutex);
        sensorOpt3001Read(&light);
        G8RTOS_SignalSemaphore(&sensorMutex);

        write_FIfo(LIGHT_FIFO, light);

        // toggle pin
        BITBAND_PERI(P4->OUT,7) = ~BITBAND_PERI(P4->OUT,7);

        // sleep
        OS_Sleep(200);


    }
}

void BackgroundThread2(){
    int32_t data;
    int i;
    int RMS;


    while(1){
        RMS = 0;
        // get new data
        G8RTOS_WaitSemaphore(&sensorMutex);
        sensorOpt3001Read(&data);
        G8RTOS_SignalSemaphore(&sensorMutex);

        // if there is no data to read the thread will block anyways
        RMS = (data * data);
        for(i = 0; i < FIFOSIZE; i++){
            data = read_Fifo(LIGHT_FIFO);
            RMS +=(data*data);
        }
        RMS /= 17;
        RMS = squareRoot(RMS);

        if(RMS < 5000){
            flag = true;
        }
        else{
            flag = false;
        }

    }
}

void BackgroundThread3(){
    int32_t temp_out;
    uint8_t led_data;

    while(1){
        led_data = 0;
        temp_out = read_Fifo(TEMP_FIFO);
        temp_f = temp_out;
        temp_out = temp_out - 63;
        temp_out /= 3;

        for(temp_out; temp_out >= 0; temp_out--){
            led_data |= (1 << temp_out);
        }

        G8RTOS_WaitSemaphore(&LEDMutex);
        LP3943_LedModeSet(BLUE, 0x00FF & (~led_data));
        LP3943_LedModeSet(RED, led_data);
        G8RTOS_SignalSemaphore(&LEDMutex);

    }
}

void BackgroundThread4(){
    uint16_t led_data;
    while(1){
        avg = (avg + read_Fifo(JOYSTICK_FIFO)) >> 1;

        if (avg > 6000)
        {
            led_data = 0xF000;
        }
        else if (avg > 4000)
        {
            led_data = 0x7000;
        }
        else if (avg > 2000)
        {
            led_data = 0x3000;
        }
        else if (avg > 500)
        {
            led_data = 0x1000;
        }
        else if (avg > -500)
        {
            led_data = 0x0000;
        }
        else if (avg > -2000)
        {
            led_data = 0x0800;
        }
        else if (avg > -4000)
        {
            led_data = 0x0C00;
        }
        else if (avg > -6000)
        {
            led_data = 0x0E00;
        }
        else
        {
            led_data = 0x0F00;
        }

        G8RTOS_WaitSemaphore(&LEDMutex);
        LP3943_LedModeSet(GREEN, led_data);
        G8RTOS_SignalSemaphore(&LEDMutex);

    }


}

void YoloSwag(){
        while(1);
}

void pthread0(){
    int16_t xcor;
    int16_t blank;

    GetJoystickCoordinates(&xcor, &blank);

    write_FIfo(JOYSTICK_FIFO, xcor);

}

void pthread1(){
    if(flag){
        BackChannelPrintIntVariable("avg", avg);
        BackChannelPrintIntVariable("temp", temp_f);
    }

}

