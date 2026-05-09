/*
 * gps_task.c
 *
 *  Created on: Apr 22, 2026
 *      Author: serda
 */

#include "FreeRTOS.h"
#include <cmsis_os.h>
#include "gps_driver.h"
#include "gps_task.h"
#include "nmea_parser.h"
#include "telemetry_data.h"
#include <string.h>

void StartGpsTask(void const * argument)
{
	char gps_line[128];
	Gps_Open(NULL);

    for(;;)
    {
        while (Gps_ReadLine(gps_line, sizeof(gps_line)))
        {
        	Nmea_ParseGPGGA((uint8_t *)gps_line, strlen(gps_line));
            TelemetryData_UpdateGps(Gps_Datas());
        }

        osDelay(100);
    }
}
