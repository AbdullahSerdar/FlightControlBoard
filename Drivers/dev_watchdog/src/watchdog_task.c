/*
 * watchdog_task.c
 *
 *  Created on: May 13, 2026
 *      Author: serda
 */

#include "watchdog_task.h"
#include "watchdog_manager.h"
#include "cmsis_os.h"
#include "main.h"

extern IWDG_HandleTypeDef hiwdg;

void StartWatchdogTask(void const * argument)
{
    for (;;)
    {
        if (Watchdog_AllTasksAlive())
        {
            Watchdog_ClearFlags();
            HAL_IWDG_Refresh(&hiwdg);
        }

        osDelay(1000);
    }
}
