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

#define LORA_PACKET_START_BYTE_SECURE  0xABU
#define LORA_PAYLOAD_LEN_TELEMETRY     32U

#define LORA_STATUS_BME_VALID          (1U << 0)
#define LORA_STATUS_GPS_VALID          (1U << 1)
#define LORA_STATUS_MPU_VALID          (1U << 2)

typedef struct __attribute__((packed))
{
    uint8_t  start_byte;
    uint8_t  version;
    uint8_t  msg_type;
    uint8_t  payload_len;

    uint32_t sequence;
    uint32_t session_id;
    uint16_t sensor_status;
    uint32_t timestamp_ms;

    float    pitch;
    float    roll;
    float    fusion_alt;
    float    temp;
    double   gps_lat;
    double   gps_lon;

    uint16_t crc16;
    uint32_t auth_tag;
} LoRaPacket_t;

typedef enum{
    E_LORA_ERR_NONE,
    E_LORA_ERR_HAL,
    E_LORA_ERR_NULL,
    E_LORA_ERR_BUSY,
    E_LORA_ERR_TIMEOUT,
    E_LORA_ERR_UNKNOWN
}Lora_ErrorCode;

Lora_ErrorCode Lora_Init(uint8_t adressH, uint8_t adressL, uint8_t reg0, uint8_t reg1, uint8_t channel);
Lora_ErrorCode LoRa_Transmit(LoRaPacket_t *packet);

#endif /* INC_LORA_DRIVER_H_ */
