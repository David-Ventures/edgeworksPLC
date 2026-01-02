/*
 * motor_drv8833.h
 *
 *  Created on: 25-12-2025
 *      Author: usuario
 */

#ifndef DRIVERS_MOTOR_MOTOR_DRV8833_H_
#define DRIVERS_MOTOR_MOTOR_DRV8833_H_

// motor_drv8833.h
#pragma once
#include "stm32f1xx_hal.h"
#include <stdint.h>
#include <stdbool.h>


typedef enum {
  MOTOR_ZERO_COAST = 0,
  MOTOR_ZERO_BRAKE = 1
} MotorZeroMode;

typedef struct {
  uint8_t id;
  TIM_HandleTypeDef *htim;
  uint32_t ch_in1;
  uint32_t ch_in2;
  uint32_t ch_in3;
  uint32_t ch_in4;

  int16_t target;     // signed target command
  int16_t current;    // signed current command (after slew)

  int16_t max;        // command max magnitude (e.g. 1000)
  int16_t deadband;   // around zero
  int16_t min_start;  // minimum magnitude once moving (optional)

  uint16_t arr;       // cached timer ARR (period)
  MotorZeroMode zero_mode;

  // Slew limiting: max change per update tick (in command units)
  int16_t slew_step;

  // Optional safety timeout (ms)
  uint32_t timeout_ms;
  uint32_t last_cmd_tick;
} MotorDRV8833;

typedef struct {
	MotorDRV8833 *m;
	uint8_t sel;
	uint32_t Time;
}Motor_Params;

void Motor_Init(MotorDRV8833 *m);
void Motor_Set(MotorDRV8833 *m, int16_t signed_cmd);
void Motor_SetZeroMode(MotorDRV8833 *m, MotorZeroMode mode);
void Motor_Stop(MotorDRV8833 *m);
void Motor_Update(MotorDRV8833 *m);
void Motor_ForceOutputs(MotorDRV8833 *m, int16_t signed_cmd); // immediate apply (no slew)
void triggerChng(uint8_t, uint8_t);
void triggerAIChng(MotorDRV8833 *m, uint8_t, int16_t);

#endif /* DRIVERS_MOTOR_MOTOR_DRV8833_H_ */
