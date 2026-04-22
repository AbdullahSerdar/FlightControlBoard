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

#define GPS_MODULE_SW_VERSION   (1.0f)

typedef enum
{
    E_GPS_ERR_NONE = 0,
    E_GPS_ERR_UART,
    E_GPS_ERR_BUFFER_OVERFLOW,
    E_GPS_ERR_INVALID_PARAM,
    E_GPS_ERR_WRONG_IOCTL_CMD,
    E_GPS_ERR_UNKNOWN
} GPSErrorCodes_t;

typedef struct
{
    double latitude;
    double longitude;
    float altitude;
    uint8_t sats_in_use;
    uint8_t valid;
} GPSReadData_t;

typedef enum
{
    E_GPS_IOCTL_NONE = 0,
    E_GPS_IOCTL_GET_VERSION,
    E_GPS_IOCTL_GET_VALID_STATUS,
    E_GPS_IOCTL_RESET_BUFFER,
    E_GPS_IOCTL_PRINT_LAST_RAW
} GPS_IOCTL_COMMANDS_T;

GPSErrorCodes_t Gps_Open(void* vpParam);
GPSErrorCodes_t Gps_Close(void* vpParam);
void Gps_ResetBuffer(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_GPS_DRIVER_H_ */
