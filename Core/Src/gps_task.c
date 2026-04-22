/*
 * gps_task.c
 *
 *  Created on: Apr 22, 2026
 *      Author: serda
 */

#include "cmsis_os.h"
#include "gps_driver.h"
#include "gps_hal.h"
#include <string.h>
#include <stdlib.h>
#include "gps_task.h"

extern uint8_t gps_rx_buf[GPS_RX_BUFFER_SIZE];
static volatile uint16_t gps_rx_size = 0;
extern osThreadId gpsTaskHandle;
GpsParsedData_t gps_data;

static uint8_t gps_process_buf[GPS_RX_BUFFER_SIZE];
static volatile uint8_t  gps_buf_ready = 0;

void Gps_TakeGPGGA(uint8_t *raw_gps_data)
{
    uint16_t size = gps_rx_size; // Lokal kopyaya al — volatile race önlemi
    uint16_t i = 0;

    if (raw_gps_data == NULL || size == 0)
        return;

    while (i + 7 < size)
    {
        if (raw_gps_data[i]   == '$' &&
            raw_gps_data[i+1] == 'G' &&
            raw_gps_data[i+2] == 'P' &&
            raw_gps_data[i+3] == 'G' &&
            raw_gps_data[i+4] == 'G' &&
            raw_gps_data[i+5] == 'A' &&
            raw_gps_data[i+6] == ',')
        {
            uint16_t start = i;
            uint16_t end   = i;

            while (end < size && raw_gps_data[end] != '*')
                end++;

            if (end + 5 <= size)
            {
                end += 3;

                if (raw_gps_data[end] == '\r' && raw_gps_data[end + 1] == '\n')
                    end += 2;

                uint16_t len = end - start;
                HAL_UART_Transmit(GPS_DEBUG_UART, &raw_gps_data[start], len, HAL_MAX_DELAY);
            }
            return;
        }
        i++;
    }
}

void Gps_RxCallback(UART_HandleTypeDef* huart, uint16_t data_size)
{
    if (huart == GPS_DEVICE_UART)
    {
    	memcpy(gps_process_buf, gps_rx_buf, data_size);
    	gps_rx_size = data_size;

    	HAL_UARTEx_ReceiveToIdle_DMA(GPS_DEVICE_UART, gps_rx_buf, sizeof(gps_rx_buf));
    	__HAL_DMA_DISABLE_IT(GPS_DEVICE_UART->hdmarx, DMA_IT_HT);

        osSignalSet(gpsTaskHandle, 0x01);
    }
}

void StartGpsTask(void const * argument)
{

	HAL_UARTEx_ReceiveToIdle_DMA(GPS_DEVICE_UART, gps_rx_buf, sizeof(gps_rx_buf));
	__HAL_DMA_DISABLE_IT(GPS_DEVICE_UART->hdmarx, DMA_IT_HT);

    for(;;)
    {
    	osEvent evt = osSignalWait(0x01, osWaitForever);
        if (evt.status == osEventSignal)
        {
        	Gps_TakeGPGGA(gps_rx_buf);
        }
    }
}

