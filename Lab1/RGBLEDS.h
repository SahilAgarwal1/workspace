/*
 * RGBLEDS.h
 *
 *  Created on: Jan 22, 2021
 *      Author: sahil
 */

#ifndef RGBLEDS_H_
#define RGBLEDS_H_


// enums for leds

typedef enum device
{
    BLUE = 0,
    GREEN = 1,
    RED = 2
} unit_desig;

// LP COLOR SET
// function to set the frequencies and PW< duty cycle for the registers

static void LP3943_ColorSet(uint32_t unit, uint32_t PWM_DATA);

// LP LED MODE SET
// setz the mode between on, off, PWM1, and PWM2

void LP3943_LedModeSet(uint32_t unit, uint16_t LED_DATA);

// init the RGB LEDS

void init_RGBLEDS();





#endif /* RGBLEDS_H_ */
