#include "cmsis_os.h"
#include "mpu_task.h"
#include "imu_driver.h"

extern osThreadId mpuTaskHandle;

void StartMpuTask(void const * argument)
{
    while (MPU_config(0x09, 0x03, 0x08, 0x08) != HAL_OK)
    {
        osDelay(500);
    }

    osDelay(500);
    MPU_CalibrateGyro(500);

    uint32_t last_tick = HAL_GetTick();

    for (;;)
    {
        uint32_t now = HAL_GetTick();
        float dt = (now - last_tick) / 1000.0f;
        last_tick = now;

        if (MPU_ReadRaw() == HAL_OK)
        {
            MPU_UpdateAngles(dt);
        }

        osDelay(10);
    }
}
