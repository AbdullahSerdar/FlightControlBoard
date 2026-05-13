#include <mpu_driver.h>
#include "cmsis_os.h"
#include "mpu_task.h"
#include "telemetry_data.h"
#include "watchdog_manager.h"

volatile int8_t g_mpu_test_result = 0;

void StartMpuTask(void const * argument)
{
#if MPU_TEST_ENABLE

	g_mpu_test_result = MPU_TEST(8000);

	for(;;)
	{
		osDelay(1000);
	}

#else
    while (MPU_config(0x09, 0x03, 0x08, 0x08) != E_MPU_ERR_NONE)
        osDelay(500);

    osDelay(500);
    MPU_CalibrateGyro(500);

    uint32_t lastWakeTime = osKernelSysTick();

    for (;;)
    {
        if (MPU_ReadRaw() == E_MPU_ERR_NONE)
        {
            MPU_UpdateAngles(0.01f);
            TelemetryData_UpdateMpu(MPU_GetDegree());

            Watchdog_ReportMpu();
        }

        osDelayUntil(&lastWakeTime, 10U);
    }
#endif
}
