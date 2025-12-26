#pragma once
#include "stm32f1xx_hal.h"

extern TIM_HandleTypeDef htim2;

void PWM_Init_TIM2(uint32_t pwm_hz);
void PWM_SetA(uint16_t duty_0_to_arr);
void PWM_SetB(uint16_t duty_0_to_arr);
uint16_t PWM_GetARR(void);
