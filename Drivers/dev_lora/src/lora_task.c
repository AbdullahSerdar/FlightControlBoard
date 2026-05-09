/*
 * lora_task.c
 *
 *  Created on: May 6, 2026
 *      Author: serda
 */

#include "lora_task.h"
#include "cmsis_os.h"
#include "lora_driver.h"
#include "telemetry_data.h"
#include <string.h>

#define LORA_PACKET_START_BYTE     0xAAU
#define LORA_SEND_PERIOD_MS        500U

static uint8_t Lora_Crc8_Xor(const uint8_t *data, uint16_t len)
{
    uint8_t crc = 0U;

    for (uint16_t i = 0; i < len; i++)
    {
        crc ^= data[i];
    }

    return crc;
}

static float Lora_CalculateFusionAltitude(const TelemetryData_t *data)
{
    if (data->bme_valid && data->gps_valid && data->gps_satellites >= 6U)
    {
        return (data->bme_altitude + data->gps_altitude) * 0.5f;
    }

    if (data->bme_valid)
    {
        return data->bme_altitude;
    }

    if (data->gps_valid)
    {
        return data->gps_altitude;
    }

    return 0.0f;
}

static void Lora_BuildPacket(const TelemetryData_t *snapshot, LoRaPacket_t *packet)
{
    memset(packet, 0, sizeof(LoRaPacket_t));

    packet->start_byte = LORA_PACKET_START_BYTE;
    packet->pitch = snapshot->mpu_pitch;
    packet->roll = snapshot->mpu_roll;
    packet->fusion_alt = Lora_CalculateFusionAltitude(snapshot);
    packet->temp = snapshot->bme_temperature;
    packet->gps_lat = snapshot->gps_latitude;
    packet->gps_lon = snapshot->gps_longitude;

    packet->crc = 0U;
    packet->crc = Lora_Crc8_Xor((const uint8_t *)packet, sizeof(LoRaPacket_t) - 1U);
}

void StartLoraTask(void const * argument)
{
    (void)argument;

    TelemetryData_t snapshot;
    LoRaPacket_t packet;

    TelemetryData_Init();
    Lora_Init(0x00, 0x61, 0x62, 0x00, 0x01);
    osDelay(500);

    for (;;)
    {
        uint32_t tickStart = osKernelSysTick();

        TelemetryData_GetSnapshot(&snapshot);
        Lora_BuildPacket(&snapshot, &packet);
        LoRa_Transmit(&packet);

        uint32_t elapsed = osKernelSysTick() - tickStart;

        if (elapsed < LORA_SEND_PERIOD_MS)
            osDelay(LORA_SEND_PERIOD_MS - elapsed);
    }
}
