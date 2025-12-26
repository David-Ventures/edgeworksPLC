/*
 * tim_driver.c
 *
 *  Created on: 21-12-2025
 *      Author: usuario
 */
#include "main.h"
#include "tim_driver.h"

extern TIM_HandleTypeDef htim3;


static inline uint32_t pwm_arr(void)
{
  return __HAL_TIM_GET_AUTORELOAD(&htim3); // ARR value (Period)
}

inline void PWM_SetDuty_0_1000(uint8_t channel, uint16_t duty1000)
{

  if (duty1000 > 1000) duty1000 = 1000;

  uint32_t arr = pwm_arr();                         // e.g. 49
  uint32_t ccr = (arr + 1) * duty1000 / 1000;       // 0..(arr+1)

  if (ccr > arr) ccr = arr;                         // clamp to 0..arr

/*  if (channel == 1) __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, ccr);
  else              __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, ccr);*/

  uint32_t chan = TIM_CHANNEL_1;

  switch (channel)
  {
    case 1:
    {
    	chan =TIM_CHANNEL_1;
    	break;
    }
    case 2:
    {
    	chan =TIM_CHANNEL_2;
    	break;
    }
    case 3:
    {
    	chan =TIM_CHANNEL_3;
    	break;
    }
    case 4:
    {
    	chan =TIM_CHANNEL_4;
    	break;
    }
    default:
    {
    	chan =TIM_CHANNEL_1;
    	break;
    }
  }

  __HAL_TIM_SET_COMPARE(&htim3, chan, ccr);
}



void Motor_SetStandby(uint8_t enable)
{
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, enable ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void MotorA_SetDir(MotorDir d, uint16_t speed)
{
  switch (d)
  {
    case MOTOR_FORWARD:
      PWM_SetDuty_0_1000( 1, speed);
      PWM_SetDuty_0_1000( 3, 0);
//      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET); // No - output PWM
//      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); // Output PWM = 0%
      break;
    case MOTOR_REVERSE:
        PWM_SetDuty_0_1000( 1, 0);
      PWM_SetDuty_0_1000( 3, speed);
//      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
//      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
      break;
    case MOTOR_BRAKE:
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
      break;
    case MOTOR_COAST:
    default:
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
      break;
  }
}

void MotorB_SetDir(MotorDir d, uint16_t speed)
{
  switch (d)
  {
  case MOTOR_FORWARD:
    PWM_SetDuty_0_1000( 2, speed);
    PWM_SetDuty_0_1000( 4, 0);
    break;
  case MOTOR_REVERSE:
    PWM_SetDuty_0_1000( 2, 0);
    PWM_SetDuty_0_1000( 4, speed);
    break;
  case MOTOR_BRAKE:
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
    break;
  case MOTOR_COAST:
  default:
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    break;
  }
}

void TIM3_Init(void)
{
  __HAL_RCC_TIM3_CLK_ENABLE();

  // Base timer: 1 MHz timer tick
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 72 - 1;                 // 72MHz / 72 = 1MHz
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 50 - 1;                    // 1MHz / 50 = 20kHz
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }

  TIM_OC_InitTypeDef sConfigOC = {0};
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;                           // start at 0% duty
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }

  // Optional but nice: force update so ARR/PSC load immediately
  __HAL_TIM_SET_COUNTER(&htim3, 0);
  __HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE);
}

