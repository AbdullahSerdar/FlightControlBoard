/*
 * bme_task.c
 *
 *  Created on: Apr 29, 2026
 *      Author: serda
 */

#include "cmsis_os.h"
#include "bme_driver.h"
#include "telemetry_data.h"
#include "bme_driver.h"
#include "watchdog_manager.h"

extern osThreadId bmeTaskHandle;
volatile int8_t g_bme_test_result = 0;

void StartBmeTask(void const * argument)
{
	(void) argument;

    BME_Data_t bmeData;

    osDelay(1000U);

    while (BME_Open(NULL) != E_BME_ERR_NONE)
    {
    	Watchdog_ReportBme();
        osDelay(500);
    }

    uint32_t lastWakeTime = osKernelSysTick();

	for(;;)
	{
		if (BME_Read(&bmeData, sizeof(bmeData)) == E_BME_ERR_NONE)
		{
			TelemetryData_UpdateBme((float)bmeData.altitude, (float)bmeData.temperature_c);
		}
		Watchdog_ReportBme();
		osDelayUntil(&lastWakeTime, 1000U);
	}

}
