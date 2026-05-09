/*
 * gps_driver.h
 *
 *  Created on: Apr 14, 2026
 *      Author: serda
 */

#ifndef INC_GPS_DRIVER_H_
#define INC_GPS_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>
#include "gps_hal.h"

typedef enum
{
    E_GPS_ERR_NONE = 0,
    E_GPS_ERR_UART,
    E_GPS_ERR_BUFFER_OVERFLOW,
    E_GPS_ERR_INVALID_PARAM,
    E_GPS_ERR_WRONG_IOCTL_CMD,
    E_GPS_ERR_UNKNOWN
} GPSErrorCodes_t;

typedef enum
{
    E_GPS_IOCTL_NONE = 0,
    E_GPS_IOCTL_GET_VERSION,
    E_GPS_IOCTL_GET_VALID_STATUS,
    E_GPS_IOCTL_RESET_BUFFER,
    E_GPS_IOCTL_PRINT_LAST_RAW
} GPS_IOCTL_COMMANDS_T;


void Gps_RxCallback(UART_HandleTypeDef* huart, uint16_t Size);


GPSErrorCodes_t Gps_Open(void* vpParam);
GPSErrorCodes_t Gps_Close(void* vpParam);

void Gps_ResetBuffer(void);

uint8_t Gps_ReadLine(char *line, uint16_t max_len);
uint32_t Gps_GetOverflowCount(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_GPS_DRIVER_H_ */
