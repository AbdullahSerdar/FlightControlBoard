/**
  ******************************************************************************
  * @file    mock_uart.h
  * @brief   UART donanimini taklit eden mock fonksiyonlarinin baslik dosyasi.
  ******************************************************************************
  */
#ifndef MOCK_UART_H_
#define MOCK_UART_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>

extern uint8_t mock_uart_tx_data[512];
extern uint16_t mock_uart_tx_len;
extern HAL_StatusTypeDef mock_uart_return_status;

/**
  * @brief  UART mock degiskenlerini sifirlar.
  * @retval None
  */
void MockUART_Init(void);

/* Linker sarmalama (wrap) prototipleri */
HAL_StatusTypeDef __wrap_HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef __wrap_HAL_UARTEx_ReceiveToIdle_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef __wrap_HAL_UART_DMAStop(UART_HandleTypeDef *huart);

#endif /* MOCK_UART_H_ */
