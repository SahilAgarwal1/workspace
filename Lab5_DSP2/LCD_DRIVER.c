/*
 * LCD_DRIVER.c
 *
 *  Created on: Oct 1, 2021
 *      Author: sahil
 */

#include <LCD_DRIVER.h>
#include <sysctl.h>
#include <i2c.h>

#define LCD_ADDRESS 0x27
#define I2C_CLK_SPD_Khz 12000

// function to split a byte into 4 bytes to send to the lcd
Uint16 * splitByte(char inputChar, bool address){
    Uint16 outputchar[4];
    uint8_t upperNibble = (inputChar & 0xF0) | 0b00001100 | address;
    uint8_t lowerNibble = ((inputChar & 0x0F) << 4) | 0b00001100 | address;

    outputchar[0] = upperNibble; // upper nibble enable = 1
    outputchar[1] = upperNibble & 0b11111011;
    outputchar[2] = lowerNibble; // upper nibble enable = 1
    outputchar[3] = lowerNibble & 0b11111011;

    return outputchar;
}

void init_LCD(){
    I2cAGpioConfig(5);
    I2C_disableModule(I2CA_BASE);
    I2C_initMaster(I2CA_BASE, SysCtl_getClock(10E6), 12E6, I2C_DUTYCYCLE_50);
    I2C_enableModule(I2CA_BASE);
    I2C_setConfig(I2CA_BASE, I2C_MASTER_SEND_MODE);
    I2C_setSlaveAddress(I2CA_BASE, 0x27);

    //I2C_O2O_Master_Init(0x27, SysCtl_getClock(10E6) * 1E-6 , 12000);


    Uint16 initCommands[5] = {0x33, 0x32, 0x28,0x0F, 0x01};
    for(int i = 0; i < 5; i++){
        write_char_LCD(initCommands[i], 0);
    //    I2C_O2O_SendBytes(initCommands, 5);
        //delayMS(5);
    }
}

void write_char_LCD(char input , bool address_RS){
    Uint16 outputcharUpper[1];
    Uint16 outputcharLower[1];
    Uint16 outputcharUpper1[1];
    Uint16 outputcharLower1[1];

    uint8_t upperNibble = (input & 0xF0) | 0b00001100 | address_RS;
    uint8_t lowerNibble = ((input & 0x0F) << 4) | 0b00001100 | address_RS;

    outputcharUpper[0] = upperNibble; // upper nibble enable = 1
    outputcharUpper1[0] = upperNibble & 0b11111011;
    outputcharLower[0] = lowerNibble; // upper nibble enable = 1
    outputcharLower1[0] = lowerNibble & 0b11111011;

    //I2C_O2O_SendBytes(splitByte(input, address_RS), 4);
    I2C_O2O_SendBytes(outputcharUpper, 1);
    I2C_O2O_SendBytes(outputcharUpper1, 1);
    I2C_O2O_SendBytes(outputcharLower, 1);
    I2C_O2O_SendBytes(outputcharLower1, 1);


}

void write_string_LCD(Uint16 * outputString, uint8_t size){
    for(int k = 0; k < size ; k++){
        write_char_LCD(outputString[k], 1);
    }
    write_char_LCD(0x00, 1);
    //write_char_LCD(0x00, 1);
}

void delayMS(uint32_t MS){
    SysCtl_delay(((SysCtl_getClock(10E6))/1000)*MS);
}

void clearScreen(){
    write_char_LCD(0x01, 0);
}

