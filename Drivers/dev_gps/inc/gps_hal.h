#ifndef INC_GPS_HAL_H_
#define INC_GPS_HAL_H_

#include "main.h"
#include <stdint.h>

/*
 * DMA receive chunk size. GPS NMEA line is usually < 82 chars,
 * but 128 keeps enough margin for ReceiveToIdle chunks.
 */
#define GPS_RX_BUFFER_SIZE        128U

/* Ring buffer should hold multiple NMEA sentences. */
#define GPS_RING_BUFFER_SIZE      512U

/* Maximum single NMEA sentence length including CR/LF and \0. */
#define GPS_NMEA_MAX_LINE_SIZE    128U

extern UART_HandleTypeDef huart3;

/* GPS physical UART. Change only this macro if GPS moves to another UART. */
#define GPS_DEVICE_UART           (&huart3)

#endif /* INC_GPS_HAL_H_ */
