#include <mpu_driver.h>
#include "cmsis_os.h"
#include "mpu_task.h"
#include "telemetry_data.h"

extern osThreadId mpuTaskHandle;

void StartMpuTask(void const * argument)
{
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
        }

        osDelayUntil(&lastWakeTime, 10U);
    }
}
