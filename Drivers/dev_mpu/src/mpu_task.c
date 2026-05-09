#include "cmsis_os.h"
#include "mpu_task.h"
#include "imu_driver.h"
#include "telemetry_data.h"

extern osThreadId mpuTaskHandle;

void StartMpuTask(void const * argument)
{
    while (MPU_config(0x09, 0x03, 0x08, 0x08) != HAL_OK)
        osDelay(500);

    osDelay(500);
    MPU_CalibrateGyro(500);

    uint32_t lastWakeTime = osKernelSysTick();

    for (;;)
    {
        if (MPU_ReadRaw() == HAL_OK)
        {
            MPU_UpdateAngles(0.01f);
            degree d = MPU_GetDegree();
            TelemetryData_UpdateMpu(d.angle_pitch, d.angle_roll);
        }

        osDelayUntil(&lastWakeTime, 10U);
    }
}
