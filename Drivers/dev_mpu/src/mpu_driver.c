#include "cmsis_os.h"
#include <math.h>
#include <mpu_driver.h>

static int16_t acc_raw[3];
static int16_t gyro_raw[3];

static float acc_g[3];
static float gyro_dps[3];

static float gyro_offset_dps[3] = {0.0f, 0.0f, 0.0f};

static float roll_acc = 0.0f;
static float pitch_acc = 0.0f;

static mpu_degree degree_mpu = {
    .angle_pitch = 0.0f,
    .angle_roll = 0.0f
};

MPU_ErrorCodes MPU_config(uint8_t pwr_mgmt, uint8_t config, uint8_t gyro, uint8_t accel)
{
    HAL_StatusTypeDef status;

    uint8_t who_I;
    HAL_I2C_Mem_Read(MPU_I2C_PORT,
                     MPU6050_ADRESS << 1,
                     MPU6050_WHO_AM_I,
                     I2C_MEMADD_SIZE_8BIT,
					 &who_I,
                     1,
                     100);

    if(who_I != MPU6050_CHIP_ID){ return E_MPU_ERR_WRONG_ID; }

    status = HAL_I2C_Mem_Write(MPU_I2C_PORT,
                               MPU6050_ADRESS << 1,
                               PWR_MGMT_1_REG,
                               I2C_MEMADD_SIZE_8BIT,
                               &pwr_mgmt,
                               1,
                               100);
    osDelay(50);
    if (status != HAL_OK) { return E_MPU_ERR_HAL; }

    status = HAL_I2C_Mem_Write(MPU_I2C_PORT,
                               MPU6050_ADRESS << 1,
                               CONFIG_ADD,
                               I2C_MEMADD_SIZE_8BIT,
                               &config,
                               1,
                               100);
    if (status != HAL_OK) return E_MPU_ERR_HAL;
    osDelay(50);

    status = HAL_I2C_Mem_Write(MPU_I2C_PORT,
                               MPU6050_ADRESS << 1,
                               GYRO_CONFIG,
                               I2C_MEMADD_SIZE_8BIT,
                               &gyro,
                               1,
                               100);
    if (status != HAL_OK) return E_MPU_ERR_HAL;
    osDelay(50);

    status = HAL_I2C_Mem_Write(MPU_I2C_PORT,
                               MPU6050_ADRESS << 1,
                               ACCEL_CONFIG,
                               I2C_MEMADD_SIZE_8BIT,
                               &accel,
                               1,
                               100);
    if (status != HAL_OK) return E_MPU_ERR_HAL;
    osDelay(50);

    return E_MPU_ERR_NONE;
}

MPU_ErrorCodes MPU_ReadRaw(void)
{
    HAL_StatusTypeDef status;
    uint8_t mpu_raw_data[14];

    status = HAL_I2C_Mem_Read(MPU_I2C_PORT,
                              MPU6050_ADRESS << 1,
                              MPU_READ_REG,
                              I2C_MEMADD_SIZE_8BIT,
                              mpu_raw_data,
                              14,
                              1000);

    if (status != HAL_OK)
    {
        return E_MPU_ERR_HAL;
    }

    acc_raw[0] = (int16_t)((mpu_raw_data[0] << 8) | mpu_raw_data[1]);
    acc_raw[1] = (int16_t)((mpu_raw_data[2] << 8) | mpu_raw_data[3]);
    acc_raw[2] = (int16_t)((mpu_raw_data[4] << 8) | mpu_raw_data[5]);

    gyro_raw[0] = (int16_t)((mpu_raw_data[8]  << 8) | mpu_raw_data[9]);
    gyro_raw[1] = (int16_t)((mpu_raw_data[10] << 8) | mpu_raw_data[11]);
    gyro_raw[2] = (int16_t)((mpu_raw_data[12] << 8) | mpu_raw_data[13]);

    acc_g[0] = acc_raw[0] / ACCEL_SENS_4G;
    acc_g[1] = acc_raw[1] / ACCEL_SENS_4G;
    acc_g[2] = acc_raw[2] / ACCEL_SENS_4G;

    gyro_dps[0] = (gyro_raw[0] / GYRO_SENS_500) - gyro_offset_dps[0];
    gyro_dps[1] = (gyro_raw[1] / GYRO_SENS_500) - gyro_offset_dps[1];
    gyro_dps[2] = (gyro_raw[2] / GYRO_SENS_500) - gyro_offset_dps[2];

    return E_MPU_ERR_NONE;
}

static void MPU_CalculateAccAngles(void)
{
    roll_acc = atan2f(acc_g[1], acc_g[2]) * RAD_TO_DEG;

    pitch_acc = atan2f(
        -acc_g[0],
        sqrtf((acc_g[1] * acc_g[1]) + (acc_g[2] * acc_g[2]))
    ) * RAD_TO_DEG;
}

void MPU_CalibrateGyro(uint16_t sample_count)
{
    float sum_x = 0.0f;
    float sum_y = 0.0f;
    float sum_z = 0.0f;

    gyro_offset_dps[0] = 0.0f;
    gyro_offset_dps[1] = 0.0f;
    gyro_offset_dps[2] = 0.0f;

    for (uint16_t i = 0; i < sample_count; i++)
    {
        if (MPU_ReadRaw() == E_MPU_ERR_NONE)
        {
            sum_x += gyro_raw[0] / GYRO_SENS_500;
            sum_y += gyro_raw[1] / GYRO_SENS_500;
            sum_z += gyro_raw[2] / GYRO_SENS_500;
        }

        osDelay(5);
    }

    gyro_offset_dps[0] = sum_x / sample_count;
    gyro_offset_dps[1] = sum_y / sample_count;
    gyro_offset_dps[2] = sum_z / sample_count;
}

void MPU_UpdateAngles(float dt)
{
    if (dt <= 0.0f || dt > 1.0f)
    {
        return;
    }

    MPU_CalculateAccAngles();

    degree_mpu.angle_roll =
        COMPLEMENTARY_ALPHA * (degree_mpu.angle_roll + gyro_dps[0] * dt)
        + (1.0f - COMPLEMENTARY_ALPHA) * roll_acc;

    degree_mpu.angle_pitch =
        COMPLEMENTARY_ALPHA * (degree_mpu.angle_pitch + gyro_dps[1] * dt)
        + (1.0f - COMPLEMENTARY_ALPHA) * pitch_acc;
}

mpu_degree MPU_GetDegree(void)
{
    return degree_mpu;
}


#if MPU_TEST_ENABLE

int8_t MPU_TEST(uint32_t timeout_ms)
{
	uint32_t elapsed = 0;

    if(MPU_config(0x09, 0x03, 0x08, 0x08) != E_MPU_ERR_NONE)
    {
    	return -1;
    }

    osDelay(500);
    MPU_CalibrateGyro(500);

    while(elapsed < timeout_ms)
    {
		if(MPU_ReadRaw() == E_MPU_ERR_NONE)
		{
			MPU_UpdateAngles(0.01f);
			osDelay(10);

			mpu_degree degree = {0, 0};
			degree = MPU_GetDegree();

			if(degree.angle_pitch < -90.0f || degree.angle_pitch > 90.0f)
			{
				return -3;
			}

			if(degree.angle_roll < -180.0f || degree.angle_roll > 180.0f)
			{
				return -4;
			}

		}
		else
		{
			return -2;
		}

		osDelay(1000);
		elapsed += 1000;
    }

    return 0;
}

#endif






















