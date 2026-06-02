/*
 * watchdog_manager.h
 *
 *  Created on: May 13, 2026
 *      Author: serda
 */

#ifndef INC_WATCHDOG_MANAGER_H_
#define INC_WATCHDOG_MANAGER_H_

#include <stdint.h>

typedef enum
{
    WD_TASK_BME = 0,
    WD_TASK_MPU,
    WD_TASK_GPS,
    WD_TASK_LORA,
    WD_TASK_COUNT
} WatchdogTaskId_t;

void Watchdog_Init(void);

void Watchdog_Report(WatchdogTaskId_t task_id);

void Watchdog_ReportBme(void);
void Watchdog_ReportGps(void);
void Watchdog_ReportMpu(void);
void Watchdog_ReportLora(void);

uint8_t Watchdog_AllTasksHealthy(void);

uint32_t Watchdog_GetLastReportTick(WatchdogTaskId_t task_id);
uint32_t Watchdog_GetMissedDeadlineCount(WatchdogTaskId_t task_id);

#endif
