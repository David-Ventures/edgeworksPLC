/*
 * cmdque.c
 *
 *  Created on: Apr 27, 2025
 *      Author: usuario
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>


#include "cmdque.h"
//#include "heaterControl.h"
#include "../Drivers/Gpio/system.h"

/*#define true 1
#define false 0*/

extern void *ParamOut;

CmdQueue Queue;
ItfcStruct IsRet;


void InitQueue()
{
	CmdQueue *pQ = &Queue;
    pQ->IdxIn = 0;
    pQ->IdxOut = 0;


    for( int8_t j = 0; j < MAX_QUEUE; j++)
    {
        pQ->Cmd[j] = 0;
    }
}

ItfcStruct *GetQueue(uint32_t *Cmd)
{
	CmdQueue *pQ = &Queue;
//	uint8_t err = 0;

	for( uint8_t i = 0; i < MAX_QUEUE; i++)
	{
		if ((pQ->Cmd[i] != 0) && (pQ->tim[i] <= HAL_GetTick())) // SystemTickCnt
		{
			ItfcStruct *pIs = &IsRet;
			*Cmd = pQ->Cmd[i];
			pIs->ParamIn = pQ->ParamIn[i];
			pIs->ParamOut = pQ->ParamOut[i];
			pQ->Cmd[i] = 0;
			pQ->tim[i] = 0; // This is new
			return pIs;
		}
	}


	return NULL;
}

bool CommandQueue(uint32_t Cmd, void *pIn, uint32_t tim)
{
	CmdQueue *pQ = &Queue;

	for (uint8_t i = 0; i < MAX_QUEUE; i++)
	{
		if ((pQ->Cmd[i] == 0) || (pQ->tim[i] == 0)) // was only (pQ->Cmd[i] == 0)
		{
			pQ->Cmd[i] = Cmd;
			pQ->ParamIn[i] = pIn;
			pQ->ParamOut[i] = ParamOut;
			pQ->tim[i] = HAL_GetTick() + tim;
			return true;
		}
	}


	return false;
}

bool ProcQueue(uint32_t Cmd, void *pPIn, void *pPOut)
{
    // Param will normally be a pointer to a structure with data needed to execute the command
    switch(Cmd)
    {
		case FLASH_LED:
		{
			StatLed_Params *slp = (StatLed_Params *)pPIn;

			if (slp->SubCmd == 1)
			{
				STAT_OFF(); //Led On
				slp->Time = 1000; // 1 sec
				slp->SubCmd = 0;
				CommandQueue(FLASH_LED, (void *)slp, slp->Time);
			}
			else
			{
				STAT_ON(); // Led Off
				slp->Time = 1000; // 1 sec
				slp->SubCmd = 1;
				CommandQueue(FLASH_LED, (void *)slp, slp->Time);
			}

			return false;
		}
		{
			default:
			return true;
		}

//		case OVEN_CONTROL:
//		{
			/* Main Kiln Control, Get's or Simulates temperature
			 * Compares it to target, generating PID output
			 * then controls heaters to follow *
			 * Find in heaterControl.c */

//			tempControl();


//			CommandQueue( OVEN_CONTROL, NULL, 2000);
//			return false;
//		}
/*		case READ_TEMP:
		{
			uint8_t pwm_duty = 0;
	        ovenTemp = SENSOR_ReadTemperature();
	        if (ovenTemp <= 0)
	        {
	        	pwm_duty = ((heaterAllowed & 0x02) ? 0.5 : 0.0) + ((heaterAllowed & 0x01)? on_time_limit/10 : 0);
	        	ovenTemp = update_temperature(pwm_duty, ovenTemp, 4600);
	        	sprintf(buff, "Sim Temp: %.2fC\n", ovenTemp);
	        }
	        else sprintf(buff, "Temp: %.2fC\n", ovenTemp);
            UART_Debug_Print(buff);
			CommandQueue( READ_TEMP, NULL, 10000);
			return false;
		}*/
	}

	return true;

}
