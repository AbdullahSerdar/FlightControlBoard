#ifndef INC_IMU_DRIVER_H_
#define INC_IMU_DRIVER_H_

#include "main.h"
#include <stdint.h>

extern I2C_HandleTypeDef hi2c1;

#define MPU_I2C_PORT          (&hi2c1)
#define MPU6050_ADRESS        0x68

#define MPU6050_WHO_AM_I      0x75
#define MPU_READ_REG          0x3B

#define CONFIG_ADD            0x1A
#define GYRO_CONFIG           0x1B
#define ACCEL_CONFIG          0x1C
#define PWR_MGMT_1_REG        0x6B

#define RAD_TO_DEG            57.2957795f

#define ACCEL_SENS_4G         8192.0f
#define GYRO_SENS_500         65.5f

#define COMPLEMENTARY_ALPHA   0.98f

typedef struct
{
    float angle_pitch;
    float angle_roll;
} degree;

uint8_t WhoAmI(uint8_t *buffer_who);

int MPU_config(uint8_t pwr_mgmt, uint8_t config, uint8_t gyro, uint8_t accel);

int MPU_ReadRaw(void);
void MPU_CalibrateGyro(uint16_t sample_count);
void MPU_UpdateAngles(float dt);

HAL_StatusTypeDef MPU_ReadRaw_DMA(void);

degree MPU_GetDegree(void);

#endif /* INC_IMU_DRIVER_H_ */
