/*
 * main_quiz.c
 *
 *  Created on: Oct 22, 2021
 *      Author: sahil
 */

#include <F28x_Project.h>
#include <initAIC23.h>
#include <device.h>

#define BUFF_SIZE 2048
#define BUFF_MASK BUFF_SIZE - 1
#pragma DATA_SECTION(delayLineOne, "ramgs0");
int delayLineOne[BUFF_SIZE];

int ch_sel = 0;
float delayLineOne_head, delayLineOne_tail = 0.0;
volatile float speed = 1;
volatile int record = 0;




int main(void){

    InitSysCtrl();      // Set SYSCLK to 200 MHz, disables watchdog timer, turns on peripheral clocks

    DINT;               // Disable CPU interrupts on startup

    // Init PIE
    InitPieCtrl();      // Initialize PIE -> disable PIE and clear all PIE registers
    IER = 0x0000;       // Clear CPU interrupt register
    IFR = 0x0000;       // Clear CPU interrupt flag register
    InitPieVectTable(); // Initialize PIE vector table to known state

    EALLOW;             // EALLOW for rest of program (unless later function disables it)

    InitSPIA();        // Initialize the SPI
    InitMcBSPb();      // Init McBSPb
    InitAIC23();       // Init AIC23

    initBreakoutBoardGPIO();

    initMRINTB();
    initGPIOINT();

    EALLOW;
    IER |= M_INT6;
    IER |= M_INT13;

    EnableInterrupts();

    while(1);

    return 0;

}


void initBreakoutBoardGPIO(){
    // the push buttons
    GPIO_setPinConfig(GPIO_10_GPIO10);
    GPIO_setPinConfig(GPIO_9_GPIO9);
    GPIO_setPinConfig(GPIO_8_GPIO8);
    // the switches
    GPIO_setPinConfig(GPIO_16_GPIO16);
    GPIO_setPinConfig(GPIO_15_GPIO15);
    GPIO_setPinConfig(GPIO_14_GPIO14);
    GPIO_setPinConfig(GPIO_11_GPIO11);
    // the leds

    GPIO_setPinConfig(GPIO_7_GPIO7);
    GPIO_setPinConfig(GPIO_6_GPIO6);
    GPIO_setPinConfig(GPIO_5_GPIO5);
    GPIO_setPinConfig(GPIO_4_GPIO4);
    GPIO_setPinConfig(GPIO_3_GPIO3);
    GPIO_setPinConfig(GPIO_2_GPIO2);
    GPIO_setPinConfig(GPIO_1_GPIO1);
    GPIO_setPinConfig(GPIO_0_GPIO0);

    // configure the pull ups for the push buttons and switches
    GPIO_setPadConfig(10, GPIO_PIN_TYPE_PULLUP);
    GPIO_setPadConfig(9, GPIO_PIN_TYPE_PULLUP);
    GPIO_setPadConfig(8, GPIO_PIN_TYPE_PULLUP);

    GPIO_setPadConfig(16, GPIO_PIN_TYPE_PULLUP);
    GPIO_setPadConfig(15, GPIO_PIN_TYPE_PULLUP);
    GPIO_setPadConfig(14, GPIO_PIN_TYPE_PULLUP);
    GPIO_setPadConfig(11, GPIO_PIN_TYPE_PULLUP);

    // configure the input qualifications
    // should help with bouncing!
    GPIO_setQualificationMode(10, GPIO_QUAL_6SAMPLE);
    GPIO_setQualificationMode(9, GPIO_QUAL_6SAMPLE);
    GPIO_setQualificationMode(8, GPIO_QUAL_6SAMPLE);

    GPIO_setQualificationPeriod(10, 510); // large divider.

    // configure directions

    GPIO_setDirectionMode(10, GPIO_DIR_MODE_IN);
    GPIO_setDirectionMode(9, GPIO_DIR_MODE_IN);
    GPIO_setDirectionMode(8, GPIO_DIR_MODE_IN);


    GPIO_setDirectionMode(16, GPIO_DIR_MODE_IN);
    GPIO_setDirectionMode(15, GPIO_DIR_MODE_IN);
    GPIO_setDirectionMode(14, GPIO_DIR_MODE_IN);
    GPIO_setDirectionMode(11, GPIO_DIR_MODE_IN);

    GPIO_setDirectionMode(7, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(6, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(5, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(4, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(3, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(2, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(1, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(0, GPIO_DIR_MODE_OUT);
}





interrupt void Button3_isr(void){
    speed += 0.5;
    if(speed > 2){
        speed = 2;
    }

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

}

interrupt void Button1_isr(void){
    speed += 0.5;
    if(speed < 0.5){
        speed = 0.5;
    }
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

interrupt void MRINTB_isr(void) {
       uint32_t sample_L;
       uint32_t sample_R;
       uint32_t temp;

           if(ch_sel == 0){
               ch_sel = 1; // next recieve will be right sample
               McBSP_receive32BitDataNonBlocking(MCBSPB_BASE, &sample_L); // recieveData
               delayLineOne[(uint32_t)delayLineOne_head & BUFF_MASK] = (sample_L);

               // iterate the circle buffer head
               delayLineOne_head += 1;
               //delayLineOne_head = (uint32_t)delayLineOne_head % BUFF_MASK;

               // check if circle buff head is equal to circle buff tail, and move tail if so
               if(delayLineOne_head == delayLineOne_tail) delayLineOne_tail += 1 ;
               //delayLineOne_tail = (uint32_t) delayLineOne_tail % BUFF_MASK;

               temp = (uint32_t) delayLineOne[((uint32_t)(delayLineOne_tail)) & BUFF_MASK];
               //McBSP_transmit32BitDataNonBlocking(MCBSPB_BASE, temp);
               delayLineOne_tail += 1.0 * speed;
               //delayLineOne_tail = (uint32_t) delayLineOne_tail % BUFF_MASK;

           }
           else{
               ch_sel = 0; // next recieve will be right sample
               McBSP_receive32BitDataNonBlocking(MCBSPB_BASE, &sample_R); // recieveData
           }

           McBSP_transmit32BitDataNonBlocking(MCBSPB_BASE, temp);

           PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;

}

void initGPIOINT(){
    EALLOW;
    GPIO_setInterruptType(GPIO_INT_XINT1, GPIO_INT_TYPE_FALLING_EDGE);
    GPIO_setInterruptType(GPIO_INT_XINT2, GPIO_INT_TYPE_FALLING_EDGE);
    GPIO_setInterruptPin(8,GPIO_INT_XINT1);
    GPIO_setInterruptPin(10,GPIO_INT_XINT2);

    EALLOW;
    PieVectTable.XINT1_INT = &Button1_isr;
    PieVectTable.XINT2_INT = &Button3_isr;
    PieCtrlRegs.PIEIER1.bit.INTx4 = 1;
    PieCtrlRegs.PIEIER1.bit.INTx5 = 1;
    IER |= M_INT1;

    GPIO_enableInterrupt(GPIO_INT_XINT1);
    GPIO_enableInterrupt(GPIO_INT_XINT2);
}

void initMRINTB(){
    DINT;
    EALLOW;
    PieVectTable.MCBSPB_RX_INT = &MRINTB_isr;
    PieCtrlRegs.PIEIER6.bit.INTx7 = 1;
    IER |= M_INT6;
}
