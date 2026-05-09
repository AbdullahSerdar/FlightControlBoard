/*
 * gps_task.c
 *
 *  Created on: Apr 22, 2026
 *      Author: serda
 */

#include "FreeRTOS.h"
#include "gps_driver.h"
#include "gps_task.h"
#include "nmea_parser.h"
#include "telemetry_data.h"

extern uint8_t gps_process_buf[GPS_RX_BUFFER_SIZE];
extern uint16_t gps_rx_size;

void StartGpsTask(void const * argument)
{

	Gps_Open(NULL);

    for(;;)
    {
        Gps_TakeGPGGA(gps_process_buf, gps_rx_size);
        TelemetryData_UpdateGps(Gps_Datas());

    }
}
