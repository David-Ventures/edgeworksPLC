#include "main.h"
#include "sensor.h"

/***********************************************************************/
//
// SPECIAL NOTE: This is modified for the "drop-in replacement" max31855
//
/***********************************************************************/

// SPI handle, usually from generated code
extern SPI_HandleTypeDef hspi1;

// SENSOR Chip Select pin
#define SENSOR_CS_PORT GPIOA
#define SENSOR_CS_PIN  GPIO_PIN_4
#define max31855


// Delay between CS assert and read (microseconds)
#define SENSOR_DELAY 1

float ovenTemp = 0.0;
float lastTemp = 25.0;

// Initialize SENSOR
void SENSOR_Init(void) {
    HAL_GPIO_WritePin(SENSOR_CS_PORT, SENSOR_CS_PIN, GPIO_PIN_SET); // Deassert CS
}

// Read temperature from SENSOR
float SENSOR_ReadTemperature(void) {
	uint8_t *data;
//	uint8_t data1[2] = {0,0};
	uint8_t data2[4] = {0,0,0,0};

	float temperature = 25;

/*	#if (SER_SPI == max6675)
	{
		uint8_t data1[2] = {0};
		data = data1;
	}
	#else if (SER_SPI == max31855)
	{
		uint8_t data2[4] = {0};
		data = data2;
	}
	#endif*/


    uint32_t rawData = 0;

    // Assert CS
    HAL_GPIO_WritePin(SENSOR_CS_PORT, SENSOR_CS_PIN, GPIO_PIN_RESET);
//    HAL_Delay(SENSOR_DELAY);  // Short delay

    // Receive 16 bits from SENSOR
	#ifdef  max6675
    {
        if (HAL_SPI_Receive(&hspi1, data2, 1, HAL_MAX_DELAY) != HAL_OK)
        {
        	HAL_GPIO_WritePin(SENSOR_CS_PORT, SENSOR_CS_PIN, GPIO_PIN_SET); // Deassert CS
        	return -1.0;  // Error value
        }
    }

	#else max31855
    {
        if (HAL_SPI_Receive(&hspi1, data2, 2, HAL_MAX_DELAY) != HAL_OK)
        {
            HAL_GPIO_WritePin(SENSOR_CS_PORT, SENSOR_CS_PIN, GPIO_PIN_SET); // Deassert CS
            return -1.0;  // Error value
        }
    }
	#endif

    // Deassert CS
    HAL_GPIO_WritePin(SENSOR_CS_PORT, SENSOR_CS_PIN, GPIO_PIN_SET);

    // Combine bytes into a single 16-bit value
	#ifdef max6675
    {
    rawData = ((uint16_t)data2[1] << 8) + data2[0];
    // Check for thermocouple connection error
    }
	#else max31855
    {
        rawData = (data2[1] << 24) | (data2[0] << 16) | (data2[3] << 8) | data2[2];
        // Check for thermocouple connection error
        if (rawData & 0x4) {
            return -1.0;  // No thermocouple connected
        }
    }
	#endif
    if (rawData & 0x4)
    {
         return lastTemp;  // No thermocouple connected
    }

    // Convert raw data to temperature (in Celsius)
    #ifdef max6675
    {
    	temperature = ((rawData >> 3) * 0.25 ); // 0.00001025/0.000041 = 0.25
    	lastTemp = temperature;
//    	return temperature; // + internal;
    }

	#else max31855
    {
    	temperature = ((rawData >> 18) *0.25 ); // 0.00001025/0.000041 = 0.25
    }
	#endif

	return temperature; // + internal;
}

