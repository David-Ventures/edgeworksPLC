/*
 * datastore.c
 *
 *  Created on: Oct 21, 2025
 *      Author: D. Pawlak
 *
 *  Store all I/O data for retrieval and use
 */
#include "main.h"
#include "datastore.h"
#include <math.h>
#include <stdbool.h>


static uint8_t DIBuf[2] ={0,0};
static uint8_t DOBuf[2] ={0,0};
static uint16_t AIBuf[2] ={0,0};
static uint16_t AOBuf[2] ={0,0};

void initStore()
{

}

uint16_t ascToU16(char data[])
{
	uint16_t tmp = 0;
	uint8_t len = strlen(data);
	for (uint8_t i = 0; i < len; i++)
	{
		tmp += ((data[i] - 0x30) * pow(10,len-i-1));
	}
//	tmp = (((data[3] - 0x30) * 1000) + ((data[2] - 0x30) * 100) + ((data[1] - 0x30) * 10) + (data[0] - 0x30));
	return tmp;
}

uint16_t ascToU8(char data[])
{
	uint16_t tmp = 0;
	if (data[1] == 0x00) tmp = (data[0] - 0x30);
	else tmp = (((data[1] - 0x30) * 10) + ((data[0] - 0x30)));
	return tmp;
}

bool getDO(uint8_t addr, uint8_t mask ) //'C'
{

}

bool getDI( uint8_t addr, uint8_t mask) // 'DI'
{

}

bool setDO( uint8_t addr, uint8_t mask, uint8_t val) //'C'
{

}

bool getHR(uint8_t addr, uint8_t qty) // Reg Out
{

}

bool getHI(uint8_t addr, uint8_t qty) // Reg In
{

}

bool setHR() // Reg Out
{

}

bool setHI() // Reg In
{

}
