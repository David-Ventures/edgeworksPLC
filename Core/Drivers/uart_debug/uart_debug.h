/*
 * uart_debug.h
 *
 *  Created on: 30-04-2025
 *      Author: David Pawlak
 */

#ifndef UART_DEBUG_H_
#define UART_DEBUG_H_

#define UART_RX_BUFFER_SIZE 100
UART_HandleTypeDef huart1;

//uint8_t rxIndex = 0;

void UART_Debug_Init(UART_HandleTypeDef *);
void USART1_Init(void);
void UART_Debug_Print(const char *);
void UART_Debug_Process(char *);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *);
#endif /* UART_DEBUG_H_ */
