/*
 * button.c
 *
 *  Created on: 06-04-2025
 *      Author: David Pawlak
 */
#include "main.h"
#include "button.h"

#define DEBOUNCE_DELAY 200  // 200 ms debounce delay
#define BUT_UNKNOWN 0



// Button state variables
volatile uint8_t buttonState = BUT_RELEASED;
volatile uint32_t lastDebounceTime = 0;

void buttonPress(uint8_t btn)
{
	uint8_t val = 0;
	val = btn;
	// Button Press Logic
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_2);  // Example: Toggle output PA2

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_0) {  // Adjust according to your button pin
        uint32_t currentTime = HAL_GetTick();
        if ((currentTime - lastDebounceTime) > DEBOUNCE_DELAY) {
        	uint8_t pin = HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_0);
        	if (pin == GPIO_PIN_RESET) {
        		if (buttonState == BUT_RELEASED) buttonState = BUT_PRESSED;  // Toggle button state
        		else buttonState = BUT_PULSED;
        		buttonPress(buttonState);
        	}
        lastDebounceTime = currentTime;
        }
    }
}



