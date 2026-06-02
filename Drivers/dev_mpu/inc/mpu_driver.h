#ifndef INC_MPU_DRIVER_H_
#define INC_MPU_DRIVER_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stddef.h>

#define MPU_DEFAULT_I2C_ADDR_7BIT    0x68U
#define MPU_WHO_AM_I_EXPECTED        0x68U

#define MPU_REG_SMPLRT_DIV           0x19U
#define MPU_REG_CONFIG               0x1AU
#define MPU_REG_GYRO_CONFIG          0x1BU
#define MPU_REG_ACCEL_CONFIG         0x1CU
#define MPU_REG_ACCEL_XOUT_H         0x3BU
#define MPU_REG_PWR_MGMT_1           0x6BU
#define MPU_REG_WHO_AM_I             0x75U

#define MPU_RAD_TO_DEG               57.2957795f
#define MPU_COMPLEMENTARY_ALPHA      0.98f

/* accel = +-4g, gyro = +-500 dps. */
#define MPU_ACCEL_SENS_4G            8192.0f
#define MPU_GYRO_SENS_500DPS         65.5f

typedef enum
{
    E_MPU_ERR_NONE = 0,
    E_MPU_ERR_HAL,
    E_MPU_ERR_WRONG_ID,
    E_MPU_ERR_INVALID_PARAM,
    E_MPU_ERR_NOT_OPEN,
    E_MPU_ERR_ALREADY_OPEN,
    E_MPU_ERR_UNSUPPORTED,
    E_MPU_ERR_UNKNOWN
} MPU_ErrorCode_t;

typedef enum
{
    E_MPU_IOCTL_GET_WHO_AM_I = 0,
    E_MPU_IOCTL_SET_CONFIG,
    E_MPU_IOCTL_SET_SAMPLE_RATE,
    E_MPU_IOCTL_CALIBRATE_GYRO,
    E_MPU_IOCTL_GET_DEGREE,
    E_MPU_IOCTL_SLEEP,
    E_MPU_IOCTL_WAKEUP,
    E_MPU_IOCTL_GET_STATE
} MPU_IoctlCommand_t;

typedef enum
{
    MPU_STATE_CLOSED = 0,
    MPU_STATE_OPEN
} MPU_State_t;

typedef struct
{
    float angle_pitch;
    float angle_roll;
} MPU_Degree_t;

typedef MPU_Degree_t mpu_degree;

typedef struct
{
    int16_t acc_raw[3];
    int16_t gyro_raw[3];

    float acc_g[3];
    float gyro_dps[3];

    float angle_pitch;
    float angle_roll;
} MPU_Data_t;

typedef struct
{
    I2C_HandleTypeDef *hi2c;
    uint8_t i2c_addr_7bit;

    uint8_t pwr_mgmt_1;
    uint8_t config;
    uint8_t gyro_config;
    uint8_t accel_config;
    uint16_t sample_rate_hz;

    uint8_t auto_calibrate;
    uint16_t calibration_samples;
} MPU_OpenConfig_t;

typedef struct
{
    uint8_t pwr_mgmt_1;
    uint8_t config;
    uint8_t gyro_config;
    uint8_t accel_config;
    uint16_t sample_rate_hz;
} MPU_Config_t;

typedef struct
{
    uint8_t reg;
    const uint8_t *data;
    uint16_t len;
} MPU_RegisterWrite_t;

MPU_ErrorCode_t MPU_Open(void *vpParam);
MPU_ErrorCode_t MPU_Ioctl(MPU_IoctlCommand_t command, void *vpParam);
MPU_ErrorCode_t MPU_Write(const void *pvBuffer, uint32_t xBytes);
MPU_ErrorCode_t MPU_Read(void *pvBuffer, uint32_t xBytes);
MPU_ErrorCode_t MPU_Close(void *vpParam);

MPU_Degree_t MPU_GetDegree(void);
MPU_ErrorCode_t MPU_CalibrateGyro(uint16_t sample_count);

#endif /* INC_MPU_DRIVER_H_ */
