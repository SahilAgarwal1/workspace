#include "Threads.h"
#include "stdint.h"
#include "G8RTOS.h"
#include "G8RTOS_Semaphores.h"
#include <BSP.h>
#include "msp.h"

void Thread0(){
    int16_t accelerometerX;
    int16_t ledData;
    int32_t ibit;

    while(1){

        G8RTOS_WaitSemaphore(&i2cSemaphore); // wait for i2c semaphore

        while(bmi160_read_accel_x(&accelerometerX)); // read sensor

        G8RTOS_SignalSemaphore(&i2cSemaphore); // release tge i2c semaphore

        // load data for the LEDs
        // if else statement for all the data

        if      (accelerometerX >  14000) ledData = 0xFF00;

        else if (accelerometerX >  12000) ledData = 0x7F00;

        else if (accelerometerX >  10000) ledData = 0x3F00;

        else if (accelerometerX >   8000) ledData = 0x1F00;

        else if (accelerometerX >   6000) ledData = 0x0F00;

        else if (accelerometerX >   4000) ledData = 0x0700;

        else if (accelerometerX >   2000) ledData = 0x0300;

        else if (accelerometerX >      0) ledData = 0x0100;

        else if (accelerometerX >  -2000) ledData = 0x0080;

        else if (accelerometerX >  -4000) ledData = 0x00C0;

        else if (accelerometerX >  -6000) ledData = 0x00E0;

        else if (accelerometerX >  -8000) ledData = 0x00F0;

        else if (accelerometerX > -10000) ledData = 0x00F8;

        else if (accelerometerX > -12000) ledData = 0x00FC;

        else if (accelerometerX > -14000) ledData = 0x00FE;

        else                              ledData = 0x00FF;




        G8RTOS_WaitSemaphore(&ledSemaphore); // wait for led semaphore

        LP3943_LedModeSet(RED, ledData); // update the LEDS

        G8RTOS_SignalSemaphore(&ledSemaphore); // release the led semaphore

    }


}



void Thread1(){
    uint16_t lightData;
    uint16_t ledData= 0;
    int32_t ibit;

    while(1){

        G8RTOS_WaitSemaphore(&i2cSemaphore); // wait for i2c semaphore

        while(!sensorOpt3001Read(&lightData)); // read sensor

        G8RTOS_SignalSemaphore(&i2cSemaphore); // release the i2c semaphore

        // update the led data
        // if else for all the data
        if      (lightData > 49000) ledData = 0xFFFF;
        else if (lightData > 45500) ledData = 0xFFFE;
        else if (lightData > 42000) ledData = 0xFFFC;
        else if (lightData > 38500) ledData = 0xFFF8;
        else if (lightData > 35500) ledData = 0xFFF0;
        else if (lightData > 31500) ledData = 0xFFE0;
        else if (lightData > 28000) ledData = 0xFFC0;
        else if (lightData > 26000) ledData = 0xFF80;
        else if (lightData > 24500) ledData = 0xFF00;
        else if (lightData > 21000) ledData = 0xFE00;
        else if (lightData > 17500) ledData = 0xFC00;
        else if (lightData > 14000) ledData = 0xF800;
        else if (lightData > 10500) ledData = 0xF000;
        else if (lightData >  7000) ledData = 0xE000;
        else if (lightData >  3500) ledData = 0xC000;
        else                        ledData = 0x8000;



        G8RTOS_WaitSemaphore(&ledSemaphore); // wait for led semaphore

        LP3943_LedModeSet(GREEN, ledData); // update the LEdS

        G8RTOS_SignalSemaphore(&ledSemaphore); // signal the semaphore

    }

}


void Thread2(){
    int16_t gyroZ;
    uint16_t ledData;
    int32_t ibit;

    while(1){

        G8RTOS_WaitSemaphore(&i2cSemaphore); // wait for i2c semaphore

        while(bmi160_read_gyro_y(&gyroZ)); // read sensor

        G8RTOS_SignalSemaphore(&i2cSemaphore); // release the semaphore

        // update data
        // if else for all the data
        if      (gyroZ >  7000) ledData = 0xFF00;
        else if (gyroZ >  6000) ledData = 0x7F00;
        else if (gyroZ >  5000) ledData = 0x3F00;
        else if (gyroZ >  4000) ledData = 0x1F00;
        else if (gyroZ >  3000) ledData = 0x0F00;
        else if (gyroZ >  2000) ledData = 0x0700;
        else if (gyroZ >  1000) ledData = 0x0300;
        else if (gyroZ >     0) ledData = 0x0100;
        else if (gyroZ > -1000) ledData = 0x0080;
        else if (gyroZ > -2000) ledData = 0x00C0;
        else if (gyroZ > -3000) ledData = 0x00E0;
        else if (gyroZ > -4000) ledData = 0x00F0;
        else if (gyroZ > -5000) ledData = 0x00F8;
        else if (gyroZ > -6000) ledData = 0x00FC;
        else if (gyroZ > -7000) ledData = 0x00FE;
        else                    ledData = 0x00FF;

        G8RTOS_WaitSemaphore(&ledSemaphore); // wait for led semaphore

        LP3943_LedModeSet(0, ledData); // update the LEDS


        G8RTOS_SignalSemaphore(&ledSemaphore); // release the semaphore


    }


}
