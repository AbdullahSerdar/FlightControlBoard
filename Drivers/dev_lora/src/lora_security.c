/*
 * lora_security.c
 *
 *  Created on: May 13, 2026
 *      Author: serda
 */


#include "lora_security.h"
#include "cmsis_os.h"
#include "main.h"

#if (LORA_SECURITY_USE_HAL_RNG == 1U)
extern RNG_HandleTypeDef hrng;
#endif

static const uint8_t LORA_PSK[16] = {
    0x53, 0x45, 0x52, 0x44, 0x41, 0x52, 0x2D, 0x4C,
    0x4F, 0x52, 0x41, 0x2D, 0x50, 0x53, 0x4B, 0x31
};

static uint32_t g_session_id = 0U;
static uint32_t g_sequence = 0U;

static uint32_t LoraSecurity_FallbackRandom(void)
{
    uint32_t x = osKernelSysTick();
    x ^= (uint32_t)(uintptr_t)&x;
    x ^= 0xA5A55A5AU;
    x ^= (x << 13);
    x ^= (x >> 17);
    x ^= (x << 5);
    return x;
}

void LoraSecurity_Init(void)
{
    uint32_t rnd = 0U;

#if (LORA_SECURITY_USE_HAL_RNG == 1U)
    if (HAL_RNG_GenerateRandomNumber(&hrng, &rnd) != HAL_OK)
    {
        rnd = LoraSecurity_FallbackRandom();
    }
#else
    rnd = LoraSecurity_FallbackRandom();
#endif

    if (rnd == 0U)
    {
        rnd = 0x1A2B3C4DU;
    }

    g_session_id = rnd;
    g_sequence = 0U;
}

uint32_t LoraSecurity_GetSessionId(void)
{
    return g_session_id;
}

uint32_t LoraSecurity_NextSequence(void)
{
    g_sequence++;
    if (g_sequence == 0U)
    {
        g_sequence = 1U;
    }
    return g_sequence;
}

uint16_t LoraSecurity_Crc16Ccitt(const uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFFU;

    if (data == NULL)
    {
        return 0U;
    }

    for (uint16_t i = 0U; i < len; i++)
    {
        crc ^= ((uint16_t)data[i] << 8);
        for (uint8_t bit = 0U; bit < 8U; bit++)
        {
            if ((crc & 0x8000U) != 0U)
            {
                crc = (uint16_t)((crc << 1) ^ 0x1021U);
            }
            else
            {
                crc = (uint16_t)(crc << 1);
            }
        }
    }

    return crc;
}

uint32_t LoraSecurity_AuthTag32(const uint8_t *data, uint16_t len)
{
    uint32_t h = 2166136261UL;

    if (data == NULL)
    {
        return 0U;
    }

    for (uint8_t i = 0U; i < sizeof(LORA_PSK); i++)
    {
        h ^= LORA_PSK[i];
        h *= 16777619UL;
    }

    for (uint16_t i = 0U; i < len; i++)
    {
        h ^= data[i];
        h *= 16777619UL;
    }

    for (uint8_t i = 0U; i < sizeof(LORA_PSK); i++)
    {
        h ^= (uint8_t)(LORA_PSK[i] + i);
        h *= 16777619UL;
    }

    return h;
}
