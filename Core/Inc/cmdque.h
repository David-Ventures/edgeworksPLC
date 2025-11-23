/*
 * cmdque.h
 *
 *  Created on: Apr 27, 2025
 *      Author: usuario
 */
#include <stdbool.h>
#ifndef CMDQUE_H
#define CMDQUE_H

#ifdef __cplusplus
 extern "C" {
#endif

#define MAX_QUEUE 20
#define PROC_RUN 1
#define PROC_PAUSE 2

 typedef struct
 {
     void *ParamIn;
     void *ParamOut;
     uint32_t tim;
 }ItfcStruct;

 typedef struct
 {
	 uint32_t Time;
	 uint8_t SubCmd;
 }StatLed_Params;

typedef struct
{
	uint32_t Cmd[MAX_QUEUE];
	void *ParamIn[MAX_QUEUE];
	void *ParamOut[MAX_QUEUE];
	uint32_t tim[MAX_QUEUE];
	uint16_t IdxIn;
	uint16_t IdxOut;
}CmdQueue;


typedef enum
{
    NULLCMD = 0,
    FLASH_LED,
	OVEN_CONTROL
}CmdCode;

void InitQueue(void);
bool CommandQueue(uint32_t, void *, uint32_t);
ItfcStruct *GetQueue(uint32_t *);
bool ProcQueue(uint32_t, void *, void *);

#endif /* INC_CMDQUE_H_ */
