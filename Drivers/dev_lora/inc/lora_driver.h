/*
 * lora_driver.h
 *
 *  Created on: May 5, 2026
 *      Author: serda
 */

#ifndef INC_LORA_DRIVER_H_
#define INC_LORA_DRIVER_H_

#include "main.h"
#include <stdint.h>

extern UART_HandleTypeDef huart2;

#define LORA_TRANSMIT    (&huart2)

typedef struct __attribute__((packed))
{
    uint8_t start_byte;
    float pitch;
    float roll;
    float fusion_alt;
    float temp;
    double gps_lat;
    double gps_lon;
    uint8_t crc;
} LoRaPacket_t;

typedef enum{
	E_LORA_ERR_NONE,
	E_LORA_ERR_HAL,
	E_LORA_ERR_NULL,
	E_LORA_ERR_UNKNOWN
}Lora_ErrorCode;

Lora_ErrorCode Lora_Init(uint8_t adressH, uint8_t adressL, uint8_t reg0, uint8_t reg1, uint8_t channel);
Lora_ErrorCode LoRa_Transmit(LoRaPacket_t *packet);

#endif /* INC_LORA_DRIVER_H_ */
