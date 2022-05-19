/*
 * RAM_DRIVER.h
 *
 *  Created on: Oct 2, 2021
 *      Author: sahil
 */

#ifndef RAM_DRIVER_H_
#define RAM_DRIVER_H_

#include <F2837xD_device.h>
#include <gpio.h>
#include <pin_map.h>
#include <device.h>
#include <driverlib.h>

void InitSpib(void);

void InitGpioSpib(void);

void SpibWriteData_SRAM_8bit(uint32_t address, uint16_t data, uint8_t CSpinNum);

uint16_t SpibReadData_SRAM_8bit(uint32_t address, uint8_t CSpinNum);

void SpibWriteData_SRAM_16bit(uint32_t address, uint16_t data, uint8_t CSpinNum);
uint16_t SpibReadData_SRAM_16bit(uint32_t address, uint8_t CSpinNum);

void SpibWriteData_dualSram(uint32_t address, uint16_t data);

#endif /* RAM_DRIVER_H_ */
