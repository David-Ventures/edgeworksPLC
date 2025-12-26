/*
 * tim_driver.h
 *
 *  Created on: 21-12-2025
 *      Author: usuario
 */

#ifndef DRIVERS_TIM_TIM_DRIVER_H_
#define DRIVERS_TIM_TIM_DRIVER_H_

typedef enum {
  MOTOR_COAST = 0,
  MOTOR_FORWARD,
  MOTOR_REVERSE,
  MOTOR_BRAKE
} MotorDir;

extern void PWM_SetDuty_0_1000(uint8_t, uint16_t);

extern void Motor_SetStandby(uint8_t);
extern void MotorA_SetDir(MotorDir, uint16_t);
extern void MotorB_SetDir(MotorDir, uint16_t);


#endif /* DRIVERS_TIM_TIM_DRIVER_H_ */
