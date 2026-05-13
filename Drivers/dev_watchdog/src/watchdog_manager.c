/*
 * watchdog_manager.c
 *
 *  Created on: May 13, 2026
 *      Author: serda
 */


#include "watchdog_manager.h"
#include "cmsis_os.h"

#define WD_BME_ALIVE    (1U << 0)
#define WD_GPS_ALIVE    (1U << 1)
#define WD_MPU_ALIVE    (1U << 2)
#define WD_LORA_ALIVE   (1U << 3)

#define WD_ALL_ALIVE    (WD_BME_ALIVE | WD_GPS_ALIVE | WD_MPU_ALIVE | WD_LORA_ALIVE)

static volatile uint32_t watchdog_flags = 0;

volatile uint32_t g_wd_flags_snapshot = 0;
volatile uint32_t g_wd_bme_count = 0;
volatile uint32_t g_wd_gps_count = 0;
volatile uint32_t g_wd_mpu_count = 0;
volatile uint32_t g_wd_lora_count = 0;

void Watchdog_ReportBme(void)
{
    watchdog_flags |= WD_BME_ALIVE;
    g_wd_bme_count++;
}

void Watchdog_ReportGps(void)
{
    watchdog_flags |= WD_GPS_ALIVE;
    g_wd_gps_count++;
}

void Watchdog_ReportMpu(void)
{
    watchdog_flags |= WD_MPU_ALIVE;
    g_wd_mpu_count++;
}

void Watchdog_ReportLora(void)
{
    watchdog_flags |= WD_LORA_ALIVE;
    g_wd_lora_count++;
}

uint8_t Watchdog_AllTasksAlive(void)
{
    g_wd_flags_snapshot = watchdog_flags;
    return ((watchdog_flags & WD_ALL_ALIVE) == WD_ALL_ALIVE);
}

void Watchdog_ClearFlags(void)
{
    watchdog_flags = 0;
}
