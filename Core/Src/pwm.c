#include "pwm.h"

TIM_HandleTypeDef htim2;

static uint16_t s_arr = 0;

uint16_t PWM_GetARR(void) { return s_arr; }

void PWM_Init_TIM2(uint32_t pwm_hz)
{
  __HAL_RCC_TIM2_CLK_ENABLE();

  // Timer clock on F103 typically 72 MHz if APB1 prescaler != 1 (common Cube setup).
  // We'll compute from HAL:
  uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
  uint32_t timclk = pclk1;

  // If APB1 prescaler > 1, timer clock is doubled
  if ((RCC->CFGR & RCC_CFGR_PPRE1) != RCC_CFGR_PPRE1_DIV1) {
    timclk = pclk1 * 2;
  }

  // We want: timclk / (PSC+1) / (ARR+1) = pwm_hz
  // Choose PSC to keep ARR in a nice range (e.g., ARR ~ 3600 for good resolution)
  uint32_t psc = (timclk / (pwm_hz * 3600UL));
  if (psc < 1) psc = 1;
  psc -= 1;

  uint32_t arr = (timclk / ((psc + 1) * pwm_hz)) - 1;
  if (arr > 0xFFFF) arr = 0xFFFF;

  s_arr = (uint16_t)arr;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = (uint16_t)psc;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = (uint16_t)arr;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

  HAL_TIM_PWM_Init(&htim2);

  TIM_OC_InitTypeDef sConfig = {0};
  sConfig.OCMode = TIM_OCMODE_PWM1;
  sConfig.Pulse = 0;
  sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfig.OCFastMode = TIM_OCFAST_DISABLE;

  HAL_TIM_PWM_ConfigChannel(&htim2, &sConfig, TIM_CHANNEL_1); // PA0
  HAL_TIM_PWM_ConfigChannel(&htim2, &sConfig, TIM_CHANNEL_2); // PA1

  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

  // Start at 0% duty
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
}

void PWM_SetA(uint16_t duty_0_to_arr)
{
  if (duty_0_to_arr > s_arr) duty_0_to_arr = s_arr;
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, duty_0_to_arr);
}

void PWM_SetB(uint16_t duty_0_to_arr)
{
  if (duty_0_to_arr > s_arr) duty_0_to_arr = s_arr;
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, duty_0_to_arr);
}
