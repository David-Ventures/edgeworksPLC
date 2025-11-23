/*
 * system.h
 *
 *  Created on: Apr 27, 2025
 *      Author: usuario
 */

#ifndef INC_SYSTEM_H_
#define INC_SYSTEM_H_

#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "stm32f1xx_hal.h"


void StatLED( int8_t );


// GPIO Bits
#define STAT_LED 13
#define PA2_PIN 2
#define PA3_PIN 3


// Bit ON_OFF Macros
#define STAT_OFF() GPIOC->BSRR = (1 << STAT_LED)
#define STAT_ON() GPIOC->BSRR = (1 << (STAT_LED + 16))

#define PA2_OFF() GPIOA->BSRR = (1 << PA2_PIN)
#define PA2_ON() GPIOA->BSRR = (1 << (PA2_PIN + 16))

#define PA3_OFF() GPIOA->BSRR = (1 << PA3_PIN)
#define PA3_ON() GPIOA->BSRR = (1 << (PA3_PIN + 16))



#endif

#endif /* INC_SYSTEM_H_ */
