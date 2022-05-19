#include "quiz.h"
#include "BSP.h"
#include "stdint.h"
#include "msp.h"
#include "driverlib.h"

void Thread0_quiz(void){
    while(1){
        G8RTOS_WaitSemaphore(&cnt_sem_0); // wait for the cnt because we dont want any other thread messing with it
        counter0++; // increment counter
        if (counter0 == 8) {counter0 = 0;} // check if 8, 0 if 8;
        G8RTOS_SignalSemaphore(&cnt_sem_0); // release the semaphore
        DelayMs(500);
    }
}

void Thread1_quiz(void){
    while(1){
        G8RTOS_WaitSemaphore(&cnt_sem_1); // wait for the cnt because we dont want any other thread messing with it
        counter1++;
        if (counter1 == 8) counter1= 0;
        G8RTOS_SignalSemaphore(&cnt_sem_1); // release the semaphore
        DelayMs(600);
    }
}

void Thread2_quiz(void){
    while(1){
        G8RTOS_WaitSemaphore(&cnt_sem_2); // wait for the cnt because we dont want any other thread messing with it
        counter2++;
        if (counter2 == 8) counter2 = 0;
        G8RTOS_SignalSemaphore(&cnt_sem_2); // release the semaphore
        DelayMs(700);
    }
}

void Thread3_quiz(void){
    while(1){
        G8RTOS_WaitSemaphore(&cnt_sem_3); // wait for the cnt because we dont want any other thread messing with it
        counter3++;
        if (counter3 == 8) counter3 = 0;
        G8RTOS_SignalSemaphore(&cnt_sem_3); // release the semaphore
    DelayMs(800);
}
}

void resetThread(void){
    while(1){
        G8RTOS_WaitSemaphore(&cnt_sem_0); // wait for all the semaphores to stop the threads from iterating the counters
        G8RTOS_WaitSemaphore(&cnt_sem_1);
        G8RTOS_WaitSemaphore(&cnt_sem_2);
        G8RTOS_WaitSemaphore(&cnt_sem_3);

        sum = counter0 + counter1 + counter2 + counter3; // sum the counts

    if (sum >= 14){ // check for reset condition
            counter0 = 0;
            counter1 = 0;
            counter2 = 0;
            counter3 = 0;

    }
    sum = 0;

        G8RTOS_SignalSemaphore(&cnt_sem_0); // release all the semaphores
        G8RTOS_SignalSemaphore(&cnt_sem_1);
        G8RTOS_SignalSemaphore(&cnt_sem_2);
        G8RTOS_SignalSemaphore(&cnt_sem_3);
    }


}

void ledThread(void){

    // initialize the button, same way as lab one, only needs to be done once so not in the loop
        P4->DIR &= ~BIT5;//set direction
        P4->IFG &= ~BIT5; // clear flag
        P4->IE |= BIT5; // enable interrupt
        P4->IES |= BIT5; // high to low transition
        P4->REN |= BIT5; // PULL up resistor
        P4->OUT |= BIT5; // set res to pull up



        while(1){

            uint8_t lowest = 0; //lowestcounter
            uint16_t LEDDATA = 0; // data for the LEDS

            while(!(P4->IN & BIT5)); // loop here and dont update if the button is pressed;

            G8RTOS_WaitSemaphore(&cnt_sem_0);// wait for all semaphores to capture all the counters in one "frame of time"
            G8RTOS_WaitSemaphore(&cnt_sem_1);
            G8RTOS_WaitSemaphore(&cnt_sem_2);
            G8RTOS_WaitSemaphore(&cnt_sem_3);


            if((counter0 <= counter1) & (counter0 <= counter2) &  (counter0 <= counter3)){
                lowest = 0;
            }
            else if((counter1 <= counter2) & (counter1 <= counter3)){
                lowest = 1;
            }
            else if(counter2 <= counter3){
                lowest = 2;
            }
            else{
                lowest = 3;
            }

            /// update leds in semaphore

            LEDDATA = (counter3 << 11) | (counter2 << 8) | (counter1 << 5) | (counter0 << 2) | (lowest);

            // release all the semaphores
            G8RTOS_SignalSemaphore(&cnt_sem_0); // release counter
            G8RTOS_SignalSemaphore(&cnt_sem_1); // release counter
            G8RTOS_SignalSemaphore(&cnt_sem_2); // release counter
            G8RTOS_SignalSemaphore(&cnt_sem_3); // release counter


            // wait for LED semaphore and release
            G8RTOS_WaitSemaphore(&ledSem);
            LP3943_LedModeSet(RED, LEDDATA); // send LED data
            G8RTOS_SignalSemaphore(&ledSem);
        }

}




