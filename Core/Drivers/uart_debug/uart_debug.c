/*
 * uart_debug.c
 *
 *  Created on: Apr 27, 2025
 *      Author: usuario
 */
#include "main.h"
#include "uart_debug.h"
#include "datastore.h"
#include <string.h>


static UART_HandleTypeDef *debug_uart = NULL;
static uint8_t rxByte;
static uint8_t rxBuffer[UART_RX_BUFFER_SIZE];
static volatile uint16_t rxIndex = 0;

extern uint8_t procState;
extern float ovenTemp;

void UART_Debug_Init(UART_HandleTypeDef *huart) {
    debug_uart = huart;
    rxIndex = 0;
    memset(rxBuffer, 0, sizeof(rxBuffer));
    debug_uart->RxState = HAL_UART_STATE_READY;
    HAL_UART_Receive_IT(debug_uart, &rxByte, 1);
}

void USART1_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}


void UART_Debug_Print(const char *msg) {
    if (debug_uart) {
        HAL_UART_Transmit(debug_uart, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
    }
}

// Call this periodically or from main loop
void UART_Debug_Process(char *in) {
    // Example: look for newline and echo message
    if (rxIndex > 0 && (rxBuffer[rxIndex - 1] == '\n' || rxBuffer[rxIndex - 1] == '\r')) {
        rxBuffer[rxIndex] = '\0'; // Null-terminate
        UART_Debug_Print("[RX]: ");
        UART_Debug_Print((char *)rxBuffer);
        rxIndex = 0;
        strcpy(in, rxBuffer);
    }
    else strcpy(in, "\0");
}

// HAL callback implementation
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == debug_uart->Instance) {
/*        if (rxIndex < UART_RX_BUFFER_SIZE - 1) {
            rxBuffer[rxIndex++] = rxByte;
        }*/
        HAL_UART_Receive_IT(debug_uart, &rxByte, 1);
        if (rxByte == '\n') {
            rxBuffer[rxIndex] = '\0';  // Null-terminate string
            int8_t bufDI = 0x03;
            int8_t bufDO;
            int16_t bufAI[8] = {0,122,2319,10,0,0,0,0};
            int16_t bufAO[8];

            char dir[6] = {0,0,0,0,0,0};
            char space[3] = {0,0,0};
            char addr[5] = {0,0,0,0,0};
            char val[6] = {0,0,0,0,0,0};
            char id[2] = {0,0};

        	char *pos;
            if ((pos = strstr(rxBuffer, "PING") != NULL))
            {
            	char newstr[7] = {0,0,0,0,0,0,0};
            	uint16_t num;
            	pos = strstr(rxBuffer, "PING,");
            	num = ascToU8((char *)(pos+5));
            	sprintf(newstr, "OK,%d\n", num);
            	HAL_UART_Transmit(huart, (uint8_t*)newstr, strlen(newstr), HAL_MAX_DELAY);
//            	OK,<id>\n
            	num = 0;
            }
            else
            {
				strcpy(&dir, strtok(rxBuffer, ","));
				strcpy(&space, strtok(NULL, ","));
				if(strstr(dir, "RD"))
				{
					strcpy(&addr, strtok(NULL, ","));
					strcpy(&id, strtok(NULL, ","));
				}
				else if (strstr(dir, "WR"))
				{
					strcpy(&addr, strtok(NULL, ","));
					strcpy(&val, strtok(NULL, ","));
					strcpy(&id, strtok(NULL, ","));
				}

				// dir is W=Write, R=Read
				// space = 'C'|'DI'|'IR'|'HR';
				// addr = 4 byte number in ascii
				// id = 2 byte sequence number 01 to 99

				if (strcmp(dir, "RD") == 0)
				{
					if (strcmp(space, "0") == 0) // DI
					{
						uint8_t addr8; // 0 @ 0x03
						char newstr[10] = {0,0,0,0,0,0,0,0,0,0};

						if ((addr8 = ascToU8(addr)) > 7) // Error
						{
							// Max 8 bits
							sprintf(newstr, "ERR,%d,%d\n", id[0], 1); // Code for Out of Range
							HAL_UART_Transmit(huart, (uint8_t*)newstr, strlen(newstr), HAL_MAX_DELAY);
						}
						else
						{

							sprintf(&newstr[0], "OK,%s,%d\n", &id[0], ((bufDI) >> (addr8-1)) & 0x01);
							HAL_UART_Transmit(huart, (uint8_t*)newstr, strlen(newstr), HAL_MAX_DELAY);
						}
					}
					else if(strcmp(space, "3") == 0) // DO
					{
						// WR,1,0,1,8\n
						// WR,<space>,<reg>,<value>,<tagId>\n
						uint16_t addr8; // 0 @ 0x03
						char newstr[10] = {0,0,0,0,0,0,0,0,0,0};

						if ((addr8 = ascToU8(addr)) > 7) // Error
						{
							// Max 8 bits
							sprintf(newstr, "ERR,%d,%d\n", &id[0], 1); // Code for Out of Range
							HAL_UART_Transmit(huart, (uint8_t*)newstr, strlen(newstr), HAL_MAX_DELAY);
						}
						else
						{
							sprintf(&newstr[0], "OK,%s,%d\n", &id[0], bufAI[addr8]);
							HAL_UART_Transmit(huart, (uint8_t*)newstr, strlen(newstr), HAL_MAX_DELAY);
						}
					}
				}
				else if (strcmp(dir, "WR") == 0)
				{
					if (strcmp(space, "1") == 0) // DO
					{
						uint16_t addr8; // 0 @ 0x03

						char newstr[10] = {0,0,0,0,0,0,0,0,0,0};

						if ((addr8 = ascToU8(addr)) > 7) // Error
						{
							// Max 8 regs
							sprintf(newstr, "ERR,%d,%d\n", &id[0], 1); // Code for Addr Out of Range
							HAL_UART_Transmit(huart, (uint8_t*)newstr, strlen(newstr), HAL_MAX_DELAY);
						}
						else if ((val[0] < 0x30) || (val[0] > 0x31))
						{
							sprintf(newstr, "ERR,%d,%d\n", &id[0], 2); // Code for Val Out of Range
							HAL_UART_Transmit(huart, (uint8_t*)newstr, strlen(newstr), HAL_MAX_DELAY);
						}
						else
						{
							sprintf(&newstr[0], "OK,%s\n", &id[0]);
							int8_t diVal = (int8_t)((val[0] - 0x30) << addr8);
							if (diVal > 0) bufDO |= diVal;
							else bufDO &= ~diVal;
							HAL_UART_Transmit(huart, (uint8_t*)newstr, strlen(newstr), HAL_MAX_DELAY);
						}
					}
					else if (strcmp(space, "4") == 0) // AO
					{
						uint16_t addr8; // 0 @ 0x03

						char newstr[10] = {0,0,0,0,0,0,0,0,0,0};

						if ((addr8 = ascToU8(addr)) > 7) // Error
						{
							// Max 8 regs
							sprintf(newstr, "ERR,%d,%d\n", &id[0], 1); // Code for Out of Range
							HAL_UART_Transmit(huart, (uint8_t*)newstr, strlen(newstr), HAL_MAX_DELAY);
						}
						else
						{
							sprintf(&newstr[0], "OK,%s\n", &id[0]);
							bufAO[addr8] = ascToU16(&val[0]);
							HAL_UART_Transmit(huart, (uint8_t*)newstr, strlen(newstr), HAL_MAX_DELAY);
						}					}
				}
            }

            rxIndex = 0;  // Reset buffer for next command

        } else {
            if (rxIndex < UART_RX_BUFFER_SIZE - 1) {
                rxBuffer[rxIndex++] = rxByte;
            } else {
                // Buffer overflow safety
                rxIndex = 0;
            }
        }
    }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */


