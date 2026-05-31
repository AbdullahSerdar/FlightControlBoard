/*
 * gps_task.c
 *
 *  Created on: Apr 22, 2026
 *      Author: serda
 */

#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "gps_driver.h"
#include "nmea_parser.h"
#include "telemetry_data.h"
#include "watchdog_manager.h"
#include <string.h>

void StartGpsTask(void const * argument)
{
    (void)argument;

    GPS_NmeaLine_t gpsLine;
    GpsParsedData_t gpsData;

    while (Gps_Open(NULL) != E_GPS_ERR_NONE)
    {
        osDelay(500);
    }

    for (;;)
    {

        if (Gps_Read(&gpsLine, sizeof(gpsLine)) == E_GPS_ERR_NONE)
        {
            if (Nmea_ParseLine(gpsLine.line, gpsLine.length) == E_NMEA_ERR_NONE)
            {
                gpsData = Nmea_GetData();

                if (gpsData.is_valid)
                {

                    TelemetryData_UpdateGps(gpsData);

                    Watchdog_ReportGps();
                }
            }
        }

        osDelay(20);
    }
}
