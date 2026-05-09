/*
 * gps_driver.c
 *
 *  Created on: Apr 14, 2026
 *      Author: serda
 */

#include "gps_driver.h"
#include <string.h>

static uint8_t gps_rx_buf[GPS_RX_BUFFER_SIZE];

static uint8_t gps_ring_buf[GPS_RING_BUFFER_SIZE];
static volatile uint16_t gps_ring_head = 0U;
static volatile uint16_t gps_ring_tail = 0U;

static volatile uint32_t gps_overflow_count = 0U;

static void GPS_RingWriteByte(uint8_t byte)
{
    uint16_t next_head = (uint16_t)((gps_ring_head + 1U) % GPS_RING_BUFFER_SIZE);

    if (next_head != gps_ring_tail)
    {
        gps_ring_buf[gps_ring_head] = byte;
        gps_ring_head = next_head;
    }
    else
    {
        gps_overflow_count++;
    }
}

static uint8_t GPS_RingReadByte(uint8_t *byte)
{
    if (byte == NULL)
    {
        return 0U;
    }

    if (gps_ring_tail == gps_ring_head)
    {
        return 0U;
    }

    *byte = gps_ring_buf[gps_ring_tail];
    gps_ring_tail = (uint16_t)((gps_ring_tail + 1U) % GPS_RING_BUFFER_SIZE);

    return 1U;
}

void Gps_RxCallback(UART_HandleTypeDef* huart, uint16_t data_size)
{
    if (huart != GPS_DEVICE_UART)
    {
        return;
    }

    if (data_size > GPS_RX_BUFFER_SIZE)
    {
        data_size = GPS_RX_BUFFER_SIZE;
    }

    for (uint16_t i = 0U; i < data_size; i++)
    {
        GPS_RingWriteByte(gps_rx_buf[i]);
    }

    HAL_UARTEx_ReceiveToIdle_DMA(GPS_DEVICE_UART, gps_rx_buf, sizeof(gps_rx_buf));
    __HAL_DMA_DISABLE_IT(GPS_DEVICE_UART->hdmarx, DMA_IT_HT);
}

uint8_t Gps_ReadLine(char *line, uint16_t max_len)
{
    static uint16_t index = 0U;
    uint8_t byte;

    if ((line == NULL) || (max_len < 2U))
    {
        return 0U;
    }

    while (GPS_RingReadByte(&byte))
    {
        if (byte == '$')
        {
            index = 0U;
            line[index++] = (char)byte;
        }
        else if (index > 0U)
        {
            if (index < (max_len - 1U))
            {
                line[index++] = (char)byte;

                if (byte == '\n')
                {
                    line[index] = '\0';
                    index = 0U;
                    return 1U;
                }
            }
            else
            {
                index = 0U;
            }
        }
    }

    return 0U;
}

void Gps_ResetBuffer(void)
{
    memset(gps_rx_buf, 0, sizeof(gps_rx_buf));
    memset(gps_ring_buf, 0, sizeof(gps_ring_buf));

    gps_ring_head = 0U;
    gps_ring_tail = 0U;
    gps_overflow_count = 0U;
}

uint32_t Gps_GetOverflowCount(void)
{
    return gps_overflow_count;
}

GPSErrorCodes_t Gps_Open(void* vpParam)
{
    (void)vpParam;

    Gps_ResetBuffer();

    if (HAL_UARTEx_ReceiveToIdle_DMA(GPS_DEVICE_UART, gps_rx_buf, sizeof(gps_rx_buf)) != HAL_OK)
    {
        return E_GPS_ERR_UART;
    }

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
