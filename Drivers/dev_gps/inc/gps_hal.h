#ifndef INC_GPS_HAL_H_
#define INC_GPS_HAL_H_

#include "main.h"
#include <stdint.h>

#define GPS_RX_BUFFER_SIZE        128U
#define GPS_RING_BUFFER_SIZE      512U
#define GPS_NMEA_MAX_LINE_SIZE    128U

extern UART_HandleTypeDef huart3;

#define GPS_DEVICE_UART           (&huart3)

#endif /* INC_GPS_HAL_H_ */
