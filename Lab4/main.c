#include <G8RTOS_Lab4/G8RTOS.h>
#include "msp.h"
#include "test.h"
#include "demo.h"
#include "quiz.h"

/**
 * main.c
 */



void main(void)
{
    /*

        G8RTOS_Init(true);

        // generate touch interrupt on button press
        /*
        P4->DIR &= ~BIT4;//set direction
        P4->IFG &= ~BIT4; // clear flag
        P4->IE |= BIT4; // enable interrupt
        P4->IES |= BIT4; // high to low transition
        P4->REN |= BIT4; // PULL up resistor
        P4->OUT |= BIT4; // set res to pull up



       srand(time(0));

       // initialize our FIFOs
       init_Fifo(ball_fifo);

       // initialize the semaphore protecting the LCD
       G8RTOS_InitSemaphore(&lcd_sem, 1);
       G8RTOS_InitSemaphore(&sensor_mutex, 1);

       // add our normal threads
       G8RTOS_AddThread(&Read_Accel, 25, "Read_Accel");
       G8RTOS_AddThread(&WaitForTap,   25,       "WaitForTap");
       G8RTOS_AddThread(&idle,              255,                   "idle");
       //G8RTOS_AddThread(&tester, 10, "test");

       // add the aperiodic event
       G8RTOS_AddAPeriodicEvent(&LCDTap, 4, PORT4_IRQn);
       P4->IFG &= ~BIT0;

        // and launch the OS!
        G8RTOS_Launch();
        */

    G8RTOS_Init(false);
    port4_setUp();

    // generate touch interrupt on button press
    /*
    P4->DIR &= ~BIT4;//set direction
    P4->IFG &= ~BIT4; // clear flag
    P4->IE |= BIT4; // enable interrupt
    P4->IES |= BIT4; // high to low transition
    P4->REN |= BIT4; // PULL up resistor
    P4->OUT |= BIT4; // set res to pull up
    */

    //init_Fifo(snake_fifo);

   // initialize the semaphore protecting the LCD
   G8RTOS_InitSemaphore(&lcd_sem, 1);

   // add our normal threads
   G8RTOS_AddThread(&updateSnake, 25, "updateSnake");
   //G8RTOS_AddThread(&WaitForTap,   25,       "WaitForTap");
   //G8RTOS_AddThread(&idle,              255,                   "idle");
   //G8RTOS_AddThread(&tester, 10, "test");

   // add the aperiodic event
   G8RTOS_AddAPeriodicEvent(&ButtonTap_port4, 4, PORT4_IRQn);
   G8RTOS_AddAPeriodicEvent(&ButtonTap_port5, 4, PORT5_IRQn);

    // and launch the OS!
    G8RTOS_Launch();



}




