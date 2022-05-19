/*
 * LCD_DRIVER.h
 *
 *  Created on: Oct 1, 2021
 *      Author: sahil
 */

#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

#include <LCD_DRIVER.h>
#include <OneToOneI2CDriver.h>
#include <sysctl.h>
#include <i2c.h>


// driver file for LCD functions

Uint16 * splitByte(char inputChar, bool address);

void init_LCD();

void write_char_LCD(char input , bool address_RS);

void write_string_LCD(Uint16 * outputString, uint8_t size);

void clearScreen();







#endif /* LCD_DRIVER_H_ */
