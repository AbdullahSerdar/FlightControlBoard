/*
 * gps_driver.c
 *
 *  Created on: Apr 14, 2026
 *      Author: serda
 */

#include "gps_driver.h"
#include "gps_hal.h"
#include "lwgps/lwgps.h"
#include "string.h"
#include "stdio.h"

static lwgps_t gps;
static uint8_t rx_buffer[GPS_RX_BUFFER_SIZE];
static uint16_t rx_index = 0;
static uint8_t rx_data = 0;

static void Gps_ResetBuffer(void)
{
    rx_index = 0;
    rx_data = 0;
    memset(rx_buffer, 0, sizeof(rx_buffer));
}

GPSErrorCodes_t Gps_Open(void* vpParam)
{
    (void)vpParam;

    lwgps_init(&gps);
    Gps_ResetBuffer();

    if (HAL_UART_Receive_DMA(GPS_DEVICE_UART, &rx_data, 1) != HAL_OK)
    {
        return E_GPS_ERR_UART;
    }

    return E_GPS_ERR_NONE;
}

GPSErrorCodes_t Gps_Read(void* pvBuffer, uint32_t xBytes)
{
    GPSReadData_t* out = NULL;

    if (pvBuffer == NULL)
    {
        return E_GPS_ERR_INVALID_PARAM;
    }

    if (xBytes < sizeof(GPSReadData_t))
    {
        return E_GPS_ERR_INVALID_PARAM;
    }

    out = (GPSReadData_t*)pvBuffer;

    out->latitude    = gps.latitude;
    out->longitude   = gps.longitude;
    out->altitude    = gps.altitude;
    out->sats_in_use = (uint8_t)gps.sats_in_use;
    out->valid       = (uint8_t)gps.is_valid;

    return E_GPS_ERR_NONE;
}

GPSErrorCodes_t Gps_Write(const void* pvBuffer, uint32_t xBytes)
{
    (void)pvBuffer;
    (void)xBytes;

    return E_GPS_ERR_NONE;
}

GPSErrorCodes_t Gps_Ioctl(GPS_IOCTL_COMMANDS_T eCommand, void* vpParam)
{
    switch (eCommand)
    {
        case E_GPS_IOCTL_GET_VERSION:
            if (vpParam == NULL)
            {
                return E_GPS_ERR_INVALID_PARAM;
            }
            *(float*)vpParam = GPS_MODULE_SW_VERSION;
            break;

        case E_GPS_IOCTL_GET_VALID_STATUS:
            if (vpParam == NULL)
            {
                return E_GPS_ERR_INVALID_PARAM;
            }
            *(uint8_t*)vpParam = (uint8_t)gps.is_valid;
            break;

        case E_GPS_IOCTL_RESET_BUFFER:
            Gps_ResetBuffer();
            break;

        default:
            return E_GPS_ERR_WRONG_IOCTL_CMD;
    }

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

void Gps_RxCallback(UART_HandleTypeDef* huart)
{
    if (huart != GPS_DEVICE_UART)
    {
        return;
    }

    if ((rx_data != '\n') && (rx_index < (GPS_RX_BUFFER_SIZE - 1U)))
    {
        rx_buffer[rx_index++] = rx_data;
    }
    else
    {
        rx_buffer[rx_index] = '\0';

//        HAL_UART_Transmit(GPS_DEBUG_UART, rx_buffer, sizeof(rx_buffer) - 1, HAL_MAX_DELAY);
//        Burada işlem tamamlanınca diyeceğim ki bunu pars et  ve doğru veriyi gönder
        (void)lwgps_process(&gps, rx_buffer, rx_index);

        Gps_ResetBuffer();
    }

    (void)HAL_UART_Receive_DMA(GPS_DEVICE_UART, &rx_data, 1);
}
