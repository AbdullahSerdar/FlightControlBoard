/*
 * watchdog_task.c
 *
 *  Created on: May 13, 2026
 *      Author: serda
 */

#include "watchdog_manager.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

extern IWDG_HandleTypeDef hiwdg;

void StartWatchdogTask(void const * argument)
{
    (void)argument;

    Watchdog_Init();

    for (;;)
    {
        if (Watchdog_AllTasksHealthy())
        {
            HAL_IWDG_Refresh(&hiwdg);
        }

        osDelay(500);
    }
}
