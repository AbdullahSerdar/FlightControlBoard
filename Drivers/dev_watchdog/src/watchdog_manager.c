/*
 * watchdog_manager.c
 *
 *  Created on: May 13, 2026
 *      Author: serda
 */


#include "watchdog_manager.h"
#include "cmsis_os.h"

typedef struct
{
    uint32_t last_report_tick;
    uint32_t max_allowed_delay_ms;
    uint32_t report_count;
    uint32_t missed_deadline_count;
    uint8_t started;
} WatchdogTaskStatus_t;

static volatile WatchdogTaskStatus_t g_wd_tasks[WD_TASK_COUNT];

void Watchdog_Init(void)
{
    g_wd_tasks[WD_TASK_BME].last_report_tick = 0U;
    g_wd_tasks[WD_TASK_BME].max_allowed_delay_ms = 2500U;
    g_wd_tasks[WD_TASK_BME].report_count = 0U;
    g_wd_tasks[WD_TASK_BME].missed_deadline_count = 0U;
    g_wd_tasks[WD_TASK_BME].started = 0U;

    g_wd_tasks[WD_TASK_MPU].last_report_tick = 0U;
    g_wd_tasks[WD_TASK_MPU].max_allowed_delay_ms = 1500U;
    g_wd_tasks[WD_TASK_MPU].report_count = 0U;
    g_wd_tasks[WD_TASK_MPU].missed_deadline_count = 0U;
    g_wd_tasks[WD_TASK_MPU].started = 0U;

    g_wd_tasks[WD_TASK_GPS].last_report_tick = 0U;
    g_wd_tasks[WD_TASK_GPS].max_allowed_delay_ms = 500U;
    g_wd_tasks[WD_TASK_GPS].report_count = 0U;
    g_wd_tasks[WD_TASK_GPS].missed_deadline_count = 0U;
    g_wd_tasks[WD_TASK_GPS].started = 0U;

    g_wd_tasks[WD_TASK_LORA].last_report_tick = 0U;
    g_wd_tasks[WD_TASK_LORA].max_allowed_delay_ms = 1500U;
    g_wd_tasks[WD_TASK_LORA].report_count = 0U;
    g_wd_tasks[WD_TASK_LORA].missed_deadline_count = 0U;
    g_wd_tasks[WD_TASK_LORA].started = 0U;
}

void Watchdog_Report(WatchdogTaskId_t task_id)
{
    if (task_id >= WD_TASK_COUNT)
    {
        return;
    }

    g_wd_tasks[task_id].last_report_tick = osKernelSysTick();
    g_wd_tasks[task_id].report_count++;
    g_wd_tasks[task_id].started = 1U;
}

void Watchdog_ReportBme(void)
{
    Watchdog_Report(WD_TASK_BME);
}

void Watchdog_ReportGps(void)
{
    Watchdog_Report(WD_TASK_GPS);
}

void Watchdog_ReportMpu(void)
{
    Watchdog_Report(WD_TASK_MPU);
}

void Watchdog_ReportLora(void)
{
    Watchdog_Report(WD_TASK_LORA);
}

uint8_t Watchdog_AllTasksHealthy(void)
{
    uint32_t now = osKernelSysTick();

    for (uint8_t i = 0U; i < WD_TASK_COUNT; i++)
    {
        if (g_wd_tasks[i].started == 0U)
        {
            return 0U;
        }

        uint32_t elapsed = now - g_wd_tasks[i].last_report_tick;

        if (elapsed > g_wd_tasks[i].max_allowed_delay_ms)
        {
            g_wd_tasks[i].missed_deadline_count++;
            return 0U;
        }
    }

    return 1U;
}

uint32_t Watchdog_GetLastReportTick(WatchdogTaskId_t task_id)
{
    if (task_id >= WD_TASK_COUNT)
    {
        return 0U;
    }

    return g_wd_tasks[task_id].last_report_tick;
}

uint32_t Watchdog_GetMissedDeadlineCount(WatchdogTaskId_t task_id)
{
    if (task_id >= WD_TASK_COUNT)
    {
        return 0U;
    }

    return g_wd_tasks[task_id].missed_deadline_count;
}
