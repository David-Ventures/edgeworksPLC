/*
 * motor_drv8833.c
 *
 *  Created on: 25-12-2025
 *      Author: usuario
 */

// motor_drv8833.c
#include "motor_drv8833.h"

static inline int16_t clamp16(int16_t x, int16_t lo, int16_t hi) {
  if (x < lo) return lo;
  if (x > hi) return hi;
  return x;
}

static inline int16_t abs16(int16_t x) { return (x < 0) ? (int16_t)(-x) : x; }

static inline uint16_t cmd_to_ccr(const MotorDRV8833 *m, int16_t mag) {
  // mag is 0..m->max
  // Convert to 0..ARR. Keep it integer and stable.
  if (mag <= 0) return 0;
  if (mag >= m->max) return m->arr;
  // scale: mag/max * arr
  // use 32-bit to avoid overflow
  uint32_t v = (uint32_t)mag * (uint32_t)m->arr;
  v /= (uint32_t)m->max;
  return (uint16_t)v;
}

static void apply_pwm(MotorDRV8833 *m, int16_t signed_cmd) {
  int16_t cmd = signed_cmd;

  // deadband
  if (abs16(cmd) <= m->deadband) cmd = 0;

  // Optional: stiction helper (min_start)
  // If command is non-zero but small, bump up to min_start
  if (cmd != 0 && m->min_start > 0) {
    int16_t mag = abs16(cmd);
    if (mag < m->min_start) {
      cmd = (cmd > 0) ? m->min_start : (int16_t)(-m->min_start);
    }
  }

  if (cmd == 0) {
    if (m->zero_mode == MOTOR_ZERO_BRAKE) {
      // Brake: IN1=IN2=100% (logic high)
      __HAL_TIM_SET_COMPARE(m->htim, m->ch_in1, m->arr);
      __HAL_TIM_SET_COMPARE(m->htim, m->ch_in3, m->arr);
    } else {
      // Coast: both low
      __HAL_TIM_SET_COMPARE(m->htim, m->ch_in1, 0);
      __HAL_TIM_SET_COMPARE(m->htim, m->ch_in3, 0);
    }
    return;
  }

  uint16_t ccr = cmd_to_ccr(m, abs16(cmd));

  if (cmd > 0) {
    // Forward: IN1 = PWM, IN2 = 0
    __HAL_TIM_SET_COMPARE(m->htim, m->ch_in1, ccr);
    __HAL_TIM_SET_COMPARE(m->htim, m->ch_in3, 0);
  } else {
    // Reverse: IN1 = 0, IN2 = PWM
    __HAL_TIM_SET_COMPARE(m->htim, m->ch_in1, 0);
    __HAL_TIM_SET_COMPARE(m->htim, m->ch_in3, ccr);
  }
}

void Motor_Init(MotorDRV8833 *m) {
  if (!m || !m->htim) return;

  // Cache ARR for scaling
  m->arr = (uint16_t)__HAL_TIM_GET_AUTORELOAD(m->htim);

  if (m->max <= 0) m->max = 1000;
  if (m->slew_step <= 0) m->slew_step = m->max; // default: no slew
  m->deadband = clamp16(m->deadband, 0, m->max);
  m->min_start = clamp16(m->min_start, 0, m->max);

  m->target = 0;
  m->current = 0;
  m->zero_mode = MOTOR_ZERO_COAST;

  m->timeout_ms = 0;
  m->last_cmd_tick = HAL_GetTick();

  // Start PWM channels if not already started
  HAL_TIM_PWM_Start(m->htim, m->ch_in1);
  HAL_TIM_PWM_Start(m->htim, m->ch_in2);
  HAL_TIM_PWM_Start(m->htim, m->ch_in3);
  HAL_TIM_PWM_Start(m->htim, m->ch_in4);

  apply_pwm(m, 0);
}

void Motor_Set(MotorDRV8833 *m, int16_t signed_cmd) {
  if (!m) return;
  m->target = clamp16(signed_cmd, (int16_t)(-m->max), m->max);
  m->last_cmd_tick = HAL_GetTick();
}

void Motor_SetZeroMode(MotorDRV8833 *m, MotorZeroMode mode) {
  if (!m) return;
  m->zero_mode = mode;
}

void Motor_Stop(MotorDRV8833 *m) {
  if (!m) return;
  m->target = 0;
  m->current = 0;
  apply_pwm(m, 0);
}

void Motor_ForceOutputs(MotorDRV8833 *m, int16_t signed_cmd) {
  if (!m) return;
  int16_t cmd = clamp16(signed_cmd, (int16_t)(-m->max), m->max);
  m->target = cmd;
  m->current = cmd;
  apply_pwm(m, cmd);
}

void Motor_Update(MotorDRV8833 *m) {
  if (!m) return;

  // Optional safety timeout
  if (m->timeout_ms > 0) {
    uint32_t now = HAL_GetTick();
    if ((now - m->last_cmd_tick) > m->timeout_ms) {
      m->target = 0;
    }
  }

  // Slew-limit toward target
  int16_t diff = (int16_t)(m->target - m->current);
  if (diff > m->slew_step) diff = m->slew_step;
  else if (diff < (int16_t)(-m->slew_step)) diff = (int16_t)(-m->slew_step);

  m->current = (int16_t)(m->current + diff);

  apply_pwm(m, m->current);
}

