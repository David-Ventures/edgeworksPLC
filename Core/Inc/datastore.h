/*
 * datastore.c
 *
 *  Created on: Oct 21, 2025
 *      Author: D. Pawlak
 */

#ifndef INC_DATASTORE_H_
#define INC_DATASTORE_H_

#include <stdbool.h>

uint16_t ascToU16(char []);
bool getDO(uint8_t, uint8_t );
bool getDI(uint8_t, uint8_t);
bool getHR(uint8_t, uint8_t); // Reg Out
bool getHI(uint8_t, uint8_t);

#endif /* INC_DATASTORE_H_ */
