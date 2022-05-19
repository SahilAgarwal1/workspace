/*
 * RGBLEDS.c
 *
 *  Created on: Jan 22, 2021
 *      Author: sahil
 */

#include "msp.h"
#include "driverlib.h"
#include "RGBLEDS.h"

static void LP3943_ColorSet(uint32_t unit, uint32_t PWM_DATA){}

void LP3943_LedModeSet(uint32_t unit, uint16_t LED_DATA){

    // data for each of the registers
    uint8_t LSO_0 = 0;
    uint8_t LSO_1 = 0;
    uint8_t LSO_2 = 0;
    uint8_t LSO_3 = 0;


    // bit manipulation for the registers
    // note that the LED DATA is in little ENDIAN FORMAT
    int i;
    for(i = 0; i < 4; i++){
        LSO_0 = (LSO_0 | (((LED_DATA >> i) & (0b00000001)) << 2*i));
    }
    for(i = 0; i < 4; i++){
        LSO_1 = (LSO_1 | (((LED_DATA >> (i+4)) & (0b00000001)) << 2*i));
    }
    for(i = 0; i < 4; i++){
        LSO_2 = (LSO_2 | (((LED_DATA >> (i+8)) & (0b00000001)) << 2*i));
    }
    for(i = 0; i < 4; i++){
        LSO_3 = (LSO_3 | (((LED_DATA >> (i+12)) & (0b00000001)) << 2*i));
    }

    // start the actual transmission of data

    // the slave address is loaded into the register needed first
    UCB2I2CSA = 0x60 | unit;
    // modify the ctl register for a 7 bit address
    UCB2CTLW0 &= (~UCSLA10);
    //uctr is already in transmitter mode
    // generate a start condition, send the slave
    while((UCB2IFG & UCTXIFG0) == 0);

    // wait for the start bit to go low to indicate data can be written to the transmit buffer
    while(UCB2CTLW0 & UCTXSTT);

    // once the flag is shown we can write the register address with autoincrement
    UCB2TXBUF = 0x16;

    // wait for the flag again
    while(!(UCB2IFG & UCTXIFG0));

    // load data 1

    UCB2TXBUF = LSO_0;

    // wait for the flag again
    while(!(UCB2IFG & UCTXIFG0));

    // load data 2

    UCB2TXBUF = LSO_1;

    // wait for the flag again
    while(!(UCB2IFG & UCTXIFG0));

    // load data 3

    UCB2TXBUF = LSO_2;

    // wait for the flag again
    while(!(UCB2IFG & UCTXIFG0));

    // load data 3

    UCB2TXBUF = LSO_3;

    // end the transmission
    while(!(UCB2IFG & UCTXIFG0));
    UCB2CTLW0 |= UCTXSTP;
    // wait for the stop bit to end
    while((UCB2CTLW0 & UCTXSTP));

}



void init_RGBLEDS(){
    uint16_t UNIT_OFF = 0x0000;

    // sodtware reset enable
    UCB2CTLW0 = UCSWRST;

    // init  i2c master
    // set master, i2c mode, clock synck, smclk source, transmitter

    UCB2CTLW0 |= UCMST | UCMODE_3 | UCSYNC | UCSSEL_2 | UCTR;

    // set the Fclk as 400khz
    // Presumes that smclk is selected as source and fsmclk is 12MHz

    UCB2BRW = 30;

    // set pins to I2c mode

    P3SEL0 |= 0b11000000;
    P3SEL1 &= 0b00000000;


    // bit wise and all bits except UCSWRST

    UCB2CTLW0 &= ~UCSWRST;


    LP3943_LedModeSet(RED, UNIT_OFF);
    LP3943_LedModeSet(BLUE, UNIT_OFF);
    LP3943_LedModeSet(GREEN, UNIT_OFF);

}






