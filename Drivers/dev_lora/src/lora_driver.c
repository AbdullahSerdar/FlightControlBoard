/*
 * lora_driver.c
 *
 *  Created on: May 5, 2026
 *      Author: serda
 */

#include "lora_driver.h"
#include "cmsis_os.h"

Lora_ErrorCode Lora_Init(uint8_t adressH, uint8_t adressL, uint8_t reg0, uint8_t reg1, uint8_t channel)
{
    HAL_StatusTypeDef status;

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);

    osDelay(50);

    uint8_t configPacket[8];

    configPacket[0] = 0xC0;
    configPacket[1] = 0x00;
    configPacket[2] = 0x05;
    configPacket[3] = adressH;
    configPacket[4] = adressL;
    configPacket[5] = reg0;
    configPacket[6] = reg1;
    configPacket[7] = channel;

    status = HAL_UART_Transmit(LORA_TRANSMIT, configPacket, sizeof(configPacket), 1000);
    if(status != HAL_OK) { return E_LORA_ERR_HAL; }

    osDelay(100);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);

    osDelay(50);

    return E_LORA_ERR_NONE;
}

Lora_ErrorCode LoRa_Transmit(LoRaPacket_t *packet)
{
    if (packet == NULL)
    {
        return E_LORA_ERR_NULL;
    }

    return HAL_UART_Transmit(LORA_TRANSMIT, (uint8_t *)packet, sizeof(LoRaPacket_t), 100);
}
