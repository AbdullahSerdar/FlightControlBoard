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
#include "bme_driver.h"
#include "watchdog_manager.h"

extern osThreadId bmeTaskHandle;
volatile int8_t g_bme_test_result = 0;

void StartBmeTask(void const * argument)
{

#if BME_TEST_ENABLE

	g_bme_test_result = BME_TEST(5000);

    for (;;)
    {
        osDelay(1000);
    }

#else

    while (BME_Open(OSRS_2, OSRS_16, BME_NORMAL_MODE, T_SB1000, IIR_16) != E_BME_ERR_NONE)
    {
        osDelay(500);
    }

	for(;;)
	{
	    BME_Measure();
	    TelemetryData_UpdateBme(BME_GetAltitude(), BME_GetTemperature());

	    Watchdog_ReportBme();

	  	osDelay(1000);
	}

#endif
}
