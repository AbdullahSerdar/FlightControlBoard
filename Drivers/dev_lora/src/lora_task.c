/*
 * lora_task.c
 *
 *  Created on: May 6, 2026
 *      Author: serda
 */

#include "lora_task.h"
#include "cmsis_os.h"
#include "lora_driver.h"
#include "lora_security.h"
#include "telemetry_data.h"
#include <string.h>
#include <stddef.h>
#include "watchdog_manager.h"

#define LORA_SEND_PERIOD_MS        500U

static uint8_t Lora_IsGpsValid(const TelemetryData_t *data)
{
    if (data == NULL)
    {
        return 0U;
    }

    if (data->gps_satellites == 0U)
    {
        return 0U;
    }

    if ((data->gps_latitude > -0.000000001) && (data->gps_latitude < 0.000000001) &&
        (data->gps_longitude > -0.000000001) && (data->gps_longitude < 0.000000001))
    {
        return 0U;
    }

    return 1U;
}

static uint16_t Lora_BuildSensorStatus(const TelemetryData_t *data)
{
    uint16_t status = 0U;

    if (data->bme_valid != 0U)
    {
        status |= LORA_STATUS_BME_VALID;
    }

    if (Lora_IsGpsValid(data) != 0U)
    {
        status |= LORA_STATUS_GPS_VALID;
    }

    if (data->mpu_valid != 0U)
    {
        status |= LORA_STATUS_MPU_VALID;
    }

    return status;
}

static float Lora_CalculateFusionAltitude(const TelemetryData_t *data)
{
    if (data->bme_valid && data->gps_satellites >= 6U)
    {
        return (data->bme_altitude + data->gps_altitude) * 0.5f;
    }

    if (data->bme_valid)
    {
        return data->bme_altitude;
    }

    return 0.0f;
}

static void Lora_BuildPacket(const TelemetryData_t *snapshot, LoRaPacket_t *packet)
{
    memset(packet, 0, sizeof(LoRaPacket_t));

    packet->start_byte = LORA_PACKET_START_BYTE_SECURE;
    packet->version = LORA_SECURITY_VERSION;
    packet->msg_type = LORA_SECURITY_MSG_TELEMETRY;
    packet->payload_len = LORA_PAYLOAD_LEN_TELEMETRY;
    packet->sequence = LoraSecurity_NextSequence();
    packet->session_id = LoraSecurity_GetSessionId();
    packet->sensor_status = Lora_BuildSensorStatus(snapshot);
    packet->timestamp_ms = osKernelSysTick();

    packet->pitch = snapshot->mpu_pitch;
    packet->roll = snapshot->mpu_roll;
    packet->fusion_alt = Lora_CalculateFusionAltitude(snapshot);
    packet->temp = snapshot->bme_temperature;
    packet->gps_lat = snapshot->gps_latitude;
    packet->gps_lon = snapshot->gps_longitude;

    packet->crc16 = 0U;
    packet->auth_tag = 0U;

    packet->crc16 = LoraSecurity_Crc16Ccitt(
        (const uint8_t *)packet,
        (uint16_t)offsetof(LoRaPacket_t, crc16)
    );

    packet->auth_tag = LoraSecurity_AuthTag32(
        (const uint8_t *)packet,
        (uint16_t)offsetof(LoRaPacket_t, auth_tag)
    );
}

void StartLoraTask(void const * argument)
{
    (void)argument;

    TelemetryData_t snapshot;
    LoRaPacket_t packet;

    TelemetryData_Init();
    LoraSecurity_Init();
    Lora_Init(0x00, 0x61, 0x62, 0x00, 0x01);
    osDelay(500);

    for (;;)
    {
        uint32_t tickStart = osKernelSysTick();

        TelemetryData_GetSnapshot(&snapshot);
        Lora_BuildPacket(&snapshot, &packet);
        LoRa_Transmit(&packet);

        Watchdog_ReportLora();

        uint32_t elapsed = osKernelSysTick() - tickStart;

        if (elapsed < LORA_SEND_PERIOD_MS)
            osDelay(LORA_SEND_PERIOD_MS - elapsed);
    }
}
