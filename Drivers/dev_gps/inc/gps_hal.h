/*
 * gps_hal.h
 *
 *  Created on: Apr 14, 2026
 *      Author: serda
 */

#ifndef INC_GPS_HAL_H_
#define INC_GPS_HAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

#define GPS_RX_BUFFER_SIZE   256U

extern UART_HandleTypeDef huart3;

#define GPS_DEVICE_UART      (&huart3)

#ifdef __cplusplus
}
#endif

#endif /* INC_GPS_HAL_H_ */
