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

#define GPS_TASK_PERIOD_MS          50U
#define GPS_RX_RECOVERY_TIMEOUT_MS  10000U

void StartGpsTask(void const * argument)
{
    (void)argument;

    GPS_NmeaLine_t gpsLine;
    GpsParsedData_t gpsData;

    uint32_t lastWakeTime;
    uint32_t lastRxTick;

    while (Gps_Open(NULL) != E_GPS_ERR_NONE)
    {
        Watchdog_ReportGps();
        osDelay(500);
    }

    lastWakeTime = osKernelSysTick();
    lastRxTick = osKernelSysTick();

    for (;;)
    {
        if (Gps_Read(&gpsLine, sizeof(gpsLine)) == E_GPS_ERR_NONE)
        {
            lastRxTick = osKernelSysTick();

            if (Nmea_ParseLine(gpsLine.line, gpsLine.length) == E_NMEA_ERR_NONE)
            {
                gpsData = Nmea_GetData();

                if (gpsData.is_valid)
                {
                    TelemetryData_UpdateGps(gpsData);
                }
            }
        }
        else
        {
            uint32_t now = osKernelSysTick();

            if ((now - lastRxTick) > GPS_RX_RECOVERY_TIMEOUT_MS)
            {
                (void)Gps_Ioctl(E_GPS_IOCTL_RESET_BUFFER, NULL);
                (void)Gps_Ioctl(E_GPS_IOCTL_STOP_RX, NULL);
                (void)Gps_Ioctl(E_GPS_IOCTL_START_RX, NULL);

                lastRxTick = now;
            }
        }

        Watchdog_ReportGps();

        osDelayUntil(&lastWakeTime, GPS_TASK_PERIOD_MS);
    }
}
