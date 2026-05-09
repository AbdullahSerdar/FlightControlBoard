#include "telemetry_data.h"
#include "cmsis_os.h"
#include <string.h>

static TelemetryData_t gTelemetryData;
extern osMutexId telemetryMutexHandle;

static void lock_mutex(osMutexId mutex_id)
{
	osMutexWait(mutex_id, osWaitForever);
}

static void unlock_mutex(osMutexId mutex_id)
{
	osMutexRelease(mutex_id);
}

void TelemetryData_Init(void)
{
    memset(&gTelemetryData, 0, sizeof(gTelemetryData));
}

void TelemetryData_UpdateBme(float altitude, float temperature)
{
    if (telemetryMutexHandle == NULL)
    {
        return;
    }

    lock_mutex(telemetryMutexHandle);

    gTelemetryData.bme_altitude = altitude;
    gTelemetryData.bme_temperature = temperature;
    gTelemetryData.bme_valid = 1U;
    gTelemetryData.bme_tick = osKernelSysTick();

    unlock_mutex(telemetryMutexHandle);
}

void TelemetryData_UpdateGps(GpsParsedData_t gps_parser)
{
    if (telemetryMutexHandle == NULL)
    {
        return;
    }

    lock_mutex(telemetryMutexHandle);

    gTelemetryData.gps_latitude = gps_parser.latitude;
    gTelemetryData.gps_longitude = gps_parser.longitude;
    gTelemetryData.gps_altitude = gps_parser.altitude;
    gTelemetryData.gps_satellites = gps_parser.satellites;
    gTelemetryData.gps_tick = osKernelSysTick();

    unlock_mutex(telemetryMutexHandle);
}

void TelemetryData_UpdateMpu(mpu_degree degree)
{
    if (telemetryMutexHandle == NULL)
    {
        return;
    }

    lock_mutex(telemetryMutexHandle);

    gTelemetryData.mpu_pitch = degree.angle_pitch;
    gTelemetryData.mpu_roll = degree.angle_roll;
    gTelemetryData.mpu_valid = 1U;
    gTelemetryData.mpu_tick = osKernelSysTick();

    unlock_mutex(telemetryMutexHandle);
}

void TelemetryData_GetSnapshot(TelemetryData_t *out)
{
    if (out == NULL || telemetryMutexHandle == NULL)
    {
        return;
    }

    lock_mutex(telemetryMutexHandle);
    memcpy(out, &gTelemetryData, sizeof(TelemetryData_t));
    unlock_mutex(telemetryMutexHandle);
}
