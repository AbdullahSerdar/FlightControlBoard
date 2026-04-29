/*
 * gps_task.c
 *
 *  Created on: Apr 22, 2026
 *      Author: serda
 */

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "gps_driver.h"
#include "gps_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "gps_task.h"

extern uint8_t gps_rx_buf[GPS_RX_BUFFER_SIZE];
uint16_t gps_rx_size = 0;
extern osThreadId gpsTaskHandle;
GpsParsedData_t gps_data;

static uint8_t gps_process_buf[GPS_RX_BUFFER_SIZE];
static volatile uint8_t  gps_buf_ready = 0;


static double Nmea_ToDegrees(const char *str)
{
    double raw = atof(str);
    int    degrees = (int)(raw / 100);
    double minutes = raw - (degrees * 100.0);
    return degrees + (minutes / 60.0);
}

static uint8_t Nmea_GetField(const char *src, uint16_t src_len, uint8_t field_idx, char *dst, uint8_t dst_size)
{
    uint8_t  field = 0;
    uint16_t d     = 0;

    for (uint16_t i = 0; i < src_len; i++)
    {
        if (src[i] == ',' || src[i] == '*')
        {
            if (field == field_idx)
            {
                dst[d] = '\0';
                return 1; // bulundu
            }
            field++;
            d = 0;
            if (src[i] == '*') break;
            continue;
        }

        if (field == field_idx && d < dst_size - 1)
            dst[d++] = src[i];
    }

    return 0; // bulunamadı
}

void Gps_TakeGPGGA(uint8_t *raw_gps_data, uint16_t size)
{
    uint16_t i    = 0;

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
                const char *sentence = (const char *)&raw_gps_data[start];

                char field[20];
                GpsParsedData_t parsed = {0};

                if (Nmea_GetField(sentence, len, 6, field, sizeof(field)))
                    parsed.fix_quality = (uint8_t)atoi(field);

                if (parsed.fix_quality == 0)
                {
                    HAL_UART_Transmit(GPS_DEBUG_UART,
                        (uint8_t *)"NO FIX\r\n", 8, HAL_MAX_DELAY);
                    return;
                }

                char lat_dir[4] = {0};
                if (Nmea_GetField(sentence, len, 2, field, sizeof(field)) &&
                    Nmea_GetField(sentence, len, 3, lat_dir, sizeof(lat_dir)))
                {
                    parsed.latitude = Nmea_ToDegrees(field);
                    if (lat_dir[0] == 'S') parsed.latitude = -parsed.latitude;
                }

                char lon_dir[4] = {0};
                if (Nmea_GetField(sentence, len, 4, field, sizeof(field)) &&
                    Nmea_GetField(sentence, len, 5, lon_dir, sizeof(lon_dir)))
                {
                    parsed.longitude = Nmea_ToDegrees(field);
                    if (lon_dir[0] == 'W') parsed.longitude = -parsed.longitude;
                }

                if (Nmea_GetField(sentence, len, 9, field, sizeof(field)))
                    parsed.altitude = atof(field);

                if (Nmea_GetField(sentence, len, 7, field, sizeof(field)))
                    parsed.satellites = (uint8_t)atoi(field);

                char out[80];
                int  out_len = snprintf(out, sizeof(out),
                    "LAT:%.6f LON:%.6f ALT:%.1fm SAT:%d\r\n",
                    parsed.latitude,
                    parsed.longitude,
                    parsed.altitude,
                    parsed.satellites);

                HAL_UART_Transmit(GPS_DEBUG_UART,
                    (uint8_t *)out, (uint16_t)out_len, HAL_MAX_DELAY);
            }
            return;
        }
        i++;
    }
}

void Gps_RxCallback(UART_HandleTypeDef* huart, uint16_t data_size)
{
	if (huart != GPS_DEVICE_UART) return;

	    memcpy(gps_process_buf, gps_rx_buf, data_size);
	    gps_rx_size = data_size;

	    HAL_UARTEx_ReceiveToIdle_DMA(GPS_DEVICE_UART, gps_rx_buf, sizeof(gps_rx_buf));
	    __HAL_DMA_DISABLE_IT(GPS_DEVICE_UART->hdmarx, DMA_IT_HT);

	    // ISR'dan FreeRTOS task notify — FromISR versiyonu kullan
	    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	    vTaskNotifyGiveFromISR(gpsTaskHandle, &xHigherPriorityTaskWoken);
	    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void StartGpsTask(void const * argument)
{

	Gps_Open(NULL);

    for(;;)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        Gps_TakeGPGGA(gps_process_buf, gps_rx_size);
    }
}

