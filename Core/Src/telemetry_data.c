#include "telemetry_data.h"
#include "cmsis_os.h"
#include <string.h>

static TelemetryData_t gTelemetryData;
extern osMutexId telemetryMutexHandle;

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

    osMutexWait(telemetryMutexHandle, osWaitForever);

    gTelemetryData.bme_altitude = altitude;
    gTelemetryData.bme_temperature = temperature;
    gTelemetryData.bme_valid = 1U;
    gTelemetryData.bme_tick = osKernelSysTick();

    osMutexRelease(telemetryMutexHandle);
}

void TelemetryData_UpdateGps(double latitude,
                             double longitude,
                             float altitude,
                             uint8_t satellites,
                             uint8_t valid)
{
    if (telemetryMutexHandle == NULL)
    {
        return;
    }

    osMutexWait(telemetryMutexHandle, osWaitForever);

    gTelemetryData.gps_latitude = latitude;
    gTelemetryData.gps_longitude = longitude;
    gTelemetryData.gps_altitude = altitude;
    gTelemetryData.gps_satellites = satellites;
    gTelemetryData.gps_valid = valid ? 1U : 0U;
    gTelemetryData.gps_tick = osKernelSysTick();

    osMutexRelease(telemetryMutexHandle);
}

void TelemetryData_UpdateMpu(float pitch, float roll)
{
    if (telemetryMutexHandle == NULL)
    {
        return;
    }

    osMutexWait(telemetryMutexHandle, osWaitForever);

    gTelemetryData.mpu_pitch = pitch;
    gTelemetryData.mpu_roll = roll;
    gTelemetryData.mpu_valid = 1U;
    gTelemetryData.mpu_tick = osKernelSysTick();

    osMutexRelease(telemetryMutexHandle);
}

void TelemetryData_GetSnapshot(TelemetryData_t *out)
{
    if (out == NULL || telemetryMutexHandle == NULL)
    {
        return;
    }

    osMutexWait(telemetryMutexHandle, osWaitForever);
    memcpy(out, &gTelemetryData, sizeof(TelemetryData_t));
    osMutexRelease(telemetryMutexHandle);
}
