#ifndef INC_TELEMETRY_DATA_H_
#define INC_TELEMETRY_DATA_H_

#include "main.h"
#include <stdint.h>

/*
 * This struct is NOT the LoRa packet.
 * It is the shared latest sensor state used inside the firmware.
 */
typedef struct
{
    float bme_altitude;
    float bme_temperature;
    uint8_t bme_valid;

    double gps_latitude;
    double gps_longitude;
    float gps_altitude;
    uint8_t gps_satellites;
    uint8_t gps_valid;

    float mpu_pitch;
    float mpu_roll;
    uint8_t mpu_valid;

    uint32_t bme_tick;
    uint32_t gps_tick;
    uint32_t mpu_tick;
} TelemetryData_t;

void TelemetryData_Init(void);

void TelemetryData_UpdateBme(float altitude, float temperature);

void TelemetryData_UpdateGps(double latitude,
                             double longitude,
                             float altitude,
                             uint8_t satellites,
                             uint8_t valid);

void TelemetryData_UpdateMpu(float pitch, float roll);

void TelemetryData_GetSnapshot(TelemetryData_t *out);

#endif /* INC_TELEMETRY_DATA_H_ */
