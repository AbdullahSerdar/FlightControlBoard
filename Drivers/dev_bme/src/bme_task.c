/*
 * bme_task.c
 *
 *  Created on: Apr 29, 2026
 *      Author: serda
 */

#include "bme_task.h"
#include "cmsis_os.h"
#include "bme_driver.h"
#include "telemetry_data.h"

extern osThreadId bmeTaskHandle;

void StartBmeTask(void const * argument)
{
    while (BME_Config(OSRS_2, OSRS_16, BME_NORMAL_MODE, T_SB1000, IIR_16) != HAL_OK)
    {
        osDelay(500);
    }

	for(;;)
	{
	    BME_Measure();
	    TelemetryData_UpdateBme(BME_GetAltitude(), BME_GetTemperature());
	  	osDelay(1000);
	}

  /* USER CODE END StartBmeTask */
}
