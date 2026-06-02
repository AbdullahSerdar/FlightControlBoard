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

GPS_NmeaLine_t gpsLine;
GpsParsedData_t gpsData;

volatile uint32_t g_gps_read_ok_count = 0U;
volatile uint32_t g_gps_no_data_count = 0U;
volatile uint32_t g_gps_parse_ok_count = 0U;
volatile uint32_t g_gps_parse_err_count = 0U;
volatile uint32_t g_gps_valid_count = 0U;
volatile GPSErrorCodes_t g_gps_last_read_err = E_GPS_ERR_NONE;
volatile NMEA_ErrorCode_t g_gps_last_parse_err = E_NMEA_ERR_NONE;


void StartGpsTask(void const * argument)
{
    (void)argument;

//    GPS_NmeaLine_t gpsLine;
//    GpsParsedData_t gpsData;

    while (Gps_Open(NULL) != E_GPS_ERR_NONE)
    {
    	Watchdog_ReportGps();
        osDelay(500);
    }

    uint32_t lastWakeTime = osKernelSysTick();

    for (;;)
    {
        g_gps_last_read_err = Gps_Read(&gpsLine, sizeof(gpsLine));

        if (g_gps_last_read_err == E_GPS_ERR_NONE)
        {
            g_gps_read_ok_count++;

            g_gps_last_parse_err = Nmea_ParseLine(gpsLine.line, gpsLine.length);

            if (g_gps_last_parse_err == E_NMEA_ERR_NONE)
            {
                g_gps_parse_ok_count++;
                gpsData = Nmea_GetData();

                if (gpsData.is_valid)
                {
                    g_gps_valid_count++;
                    TelemetryData_UpdateGps(gpsData);
                }
            }
            else
            {
                g_gps_parse_err_count++;
            }
        }
        else if (g_gps_last_read_err == E_GPS_ERR_NO_DATA)
        {
            g_gps_no_data_count++;
        }

        Watchdog_ReportGps();

        osDelayUntil(&lastWakeTime, 50U);
    }
}
