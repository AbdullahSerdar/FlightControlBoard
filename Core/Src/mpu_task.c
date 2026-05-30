#include "mpu_driver.h"
#include "cmsis_os.h"
#include "telemetry_data.h"
#include "watchdog_manager.h"

volatile int8_t g_mpu_test_result = 0;

void StartMpuTask(void const * argument)
{
    (void)argument;

    MPU_Data_t mpuData;

    while (MPU_Open(NULL) != E_MPU_ERR_NONE)
    {
        osDelay(500);
    }

    uint32_t lastWakeTime = osKernelSysTick();

    for (;;)
    {
        if (MPU_Read(&mpuData, sizeof(mpuData)) == E_MPU_ERR_NONE)
        {
            TelemetryData_UpdateMpu(MPU_GetDegree());
            Watchdog_ReportMpu();
        }

        osDelayUntil(&lastWakeTime, 10U);
    }

}
