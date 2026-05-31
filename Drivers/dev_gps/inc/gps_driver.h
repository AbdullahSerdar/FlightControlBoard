#ifndef INC_GPS_DRIVER_H_
#define INC_GPS_DRIVER_H_

#include "main.h"
#include "gps_hal.h"
#include <stdint.h>

typedef enum
{
    E_GPS_ERR_NONE = 0,
    E_GPS_ERR_UART,
    E_GPS_ERR_BUFFER_OVERFLOW,
    E_GPS_ERR_INVALID_PARAM,
    E_GPS_ERR_NOT_OPEN,
    E_GPS_ERR_ALREADY_OPEN,
    E_GPS_ERR_NO_DATA,
    E_GPS_ERR_LINE_TOO_LONG,
    E_GPS_ERR_WRONG_IOCTL_CMD,
    E_GPS_ERR_UNKNOWN
} GPSErrorCodes_t;

typedef enum
{
    GPS_STATE_CLOSED = 0,
    GPS_STATE_OPEN
} GPS_State_t;

typedef enum
{
    E_GPS_IOCTL_GET_STATE = 0,
    E_GPS_IOCTL_GET_OVERFLOW_COUNT,
    E_GPS_IOCTL_RESET_BUFFER,
    E_GPS_IOCTL_START_RX,
    E_GPS_IOCTL_STOP_RX
} GPS_IoctlCommand_t;

typedef struct
{
    UART_HandleTypeDef *huart;
} GPS_OpenConfig_t;

typedef struct
{
    char line[GPS_NMEA_MAX_LINE_SIZE];
    uint16_t length;
} GPS_NmeaLine_t;

void Gps_RxCallback(UART_HandleTypeDef *huart, uint16_t Size);

GPSErrorCodes_t Gps_Open(void *vpParam);
GPSErrorCodes_t Gps_Read(void *pvBuffer, uint32_t xBytes);
GPSErrorCodes_t Gps_Write(const void *pvBuffer, uint32_t xBytes);
GPSErrorCodes_t Gps_Ioctl(GPS_IoctlCommand_t command, void *vpParam);
GPSErrorCodes_t Gps_Close(void *vpParam);

#endif /* INC_GPS_DRIVER_H_ */
