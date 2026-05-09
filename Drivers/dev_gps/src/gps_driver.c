/*
 * gps_driver.c
 *
 *  Created on: Apr 14, 2026
 *      Author: serda
 */
#include "gps_driver.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t gps_rx_buf[GPS_RX_BUFFER_SIZE];
extern uint8_t gps_rx_buf[GPS_RX_BUFFER_SIZE];
uint8_t gps_process_buf[GPS_RX_BUFFER_SIZE];
static volatile uint8_t  gps_buf_ready = 0;
uint16_t gps_rx_size = 0;

void Gps_RxCallback(UART_HandleTypeDef* huart, uint16_t data_size)
{
	if (huart != GPS_DEVICE_UART) return;

	    memcpy(gps_process_buf, gps_rx_buf, data_size);
	    gps_rx_size = data_size;

	    HAL_UARTEx_ReceiveToIdle_DMA(GPS_DEVICE_UART, gps_rx_buf, sizeof(gps_rx_buf));
	    __HAL_DMA_DISABLE_IT(GPS_DEVICE_UART->hdmarx, DMA_IT_HT);

}

void Gps_ResetBuffer(void)
{
    memset(gps_rx_buf, 0, sizeof(gps_rx_buf));
}

GPSErrorCodes_t Gps_Open(void* vpParam)
{
    Gps_ResetBuffer();
    if (HAL_UARTEx_ReceiveToIdle_DMA(GPS_DEVICE_UART, gps_rx_buf, sizeof(gps_rx_buf)) != HAL_OK)
        return E_GPS_ERR_UART;

    __HAL_DMA_DISABLE_IT(GPS_DEVICE_UART->hdmarx, DMA_IT_HT);
    return E_GPS_ERR_NONE;
}

GPSErrorCodes_t Gps_Close(void* vpParam)
{
    (void)vpParam;

    if (HAL_UART_DMAStop(GPS_DEVICE_UART) != HAL_OK)
    {
        return E_GPS_ERR_UART;
    }

    return E_GPS_ERR_NONE;
}

