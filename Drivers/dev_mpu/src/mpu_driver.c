#include "mpu_driver.h"
#include "cmsis_os.h"
#include <math.h>
#include <string.h>
#include "mpu_hal.h"

extern I2C_HandleTypeDef hi2c1;

typedef struct
{
    I2C_HandleTypeDef *hi2c;
    uint8_t i2c_addr_7bit;
    MPU_State_t state;

    MPU_Config_t config;
    MPU_Data_t last_data;
    MPU_Degree_t degree;

    float gyro_offset_dps[3];
    uint32_t last_update_tick;
} MPU_Context_t;

static MPU_Context_t g_mpu = {0};

static uint16_t MPU_DevAddr8(void)
{
    return (uint16_t)(g_mpu.i2c_addr_7bit << 1U);
}

static void MPU_DelayMs(uint32_t delay_ms)
{
    osDelay(delay_ms);
}

static MPU_ErrorCode_t MPU_ReadRegister(uint8_t reg, uint8_t *data, uint16_t len)
{
    if ((g_mpu.hi2c == NULL) || (data == NULL) || (len == 0U))
    {
        return E_MPU_ERR_INVALID_PARAM;
    }

    if (HAL_I2C_Mem_Read(g_mpu.hi2c,
                         MPU_DevAddr8(),
                         reg,
                         I2C_MEMADD_SIZE_8BIT,
                         data,
                         len,
                         1000U) != HAL_OK)
    {
        return E_MPU_ERR_HAL;
    }

    return E_MPU_ERR_NONE;
}

static MPU_ErrorCode_t MPU_WriteRegister(uint8_t reg, const uint8_t *data, uint16_t len)
{
    if ((g_mpu.hi2c == NULL) || (data == NULL) || (len == 0U))
    {
        return E_MPU_ERR_INVALID_PARAM;
    }

    if (HAL_I2C_Mem_Write(g_mpu.hi2c,
                          MPU_DevAddr8(),
                          reg,
                          I2C_MEMADD_SIZE_8BIT,
                          (uint8_t *)data,
                          len,
                          1000U) != HAL_OK)
    {
        return E_MPU_ERR_HAL;
    }

    return E_MPU_ERR_NONE;
}

static MPU_ErrorCode_t MPU_WriteRegister8(uint8_t reg, uint8_t value)
{
    return MPU_WriteRegister(reg, &value, 1U);
}

static MPU_ErrorCode_t MPU_SetSampleRate(uint16_t sample_rate_hz)
{
    uint8_t divider;

    if ((sample_rate_hz == 0U) || (sample_rate_hz > 1000U))
    {
        return E_MPU_ERR_INVALID_PARAM;
    }

    /* With DLPF enabled, internal sample rate is generally 1 kHz. */
    divider = (uint8_t)((1000U / sample_rate_hz) - 1U);

    if (MPU_WriteRegister8(MPU_REG_SMPLRT_DIV, divider) != E_MPU_ERR_NONE)
    {
        return E_MPU_ERR_HAL;
    }

    g_mpu.config.sample_rate_hz = sample_rate_hz;
    return E_MPU_ERR_NONE;
}

static MPU_ErrorCode_t MPU_ApplyConfig(const MPU_Config_t *config)
{
    MPU_ErrorCode_t err;

    if (config == NULL)
    {
        return E_MPU_ERR_INVALID_PARAM;
    }

    err = MPU_WriteRegister8(MPU_REG_PWR_MGMT_1, config->pwr_mgmt_1);
    if (err != E_MPU_ERR_NONE) { return err; }
    MPU_DelayMs(50U);

    err = MPU_WriteRegister8(MPU_REG_CONFIG, config->config);
    if (err != E_MPU_ERR_NONE) { return err; }
    MPU_DelayMs(10U);

    err = MPU_WriteRegister8(MPU_REG_GYRO_CONFIG, config->gyro_config);
    if (err != E_MPU_ERR_NONE) { return err; }
    MPU_DelayMs(10U);

    err = MPU_WriteRegister8(MPU_REG_ACCEL_CONFIG, config->accel_config);
    if (err != E_MPU_ERR_NONE) { return err; }
    MPU_DelayMs(10U);

    err = MPU_SetSampleRate(config->sample_rate_hz);
    if (err != E_MPU_ERR_NONE) { return err; }

    g_mpu.config = *config;
    return E_MPU_ERR_NONE;
}

static MPU_ErrorCode_t MPU_HardwareInit(const MPU_OpenConfig_t *open_config)
{
    uint8_t who_am_i = 0U;
    MPU_Config_t config;
    MPU_ErrorCode_t err;

    g_mpu.hi2c = open_config->hi2c;
    g_mpu.i2c_addr_7bit = open_config->i2c_addr_7bit;

    err = MPU_ReadRegister(MPU_REG_WHO_AM_I, &who_am_i, 1U);
    if (err != E_MPU_ERR_NONE)
    {
        return err;
    }

    if (who_am_i != MPU_WHO_AM_I_EXPECTED)
    {
        return E_MPU_ERR_WRONG_ID;
    }

    config.pwr_mgmt_1 = open_config->pwr_mgmt_1;
    config.config = open_config->config;
    config.gyro_config = open_config->gyro_config;
    config.accel_config = open_config->accel_config;
    config.sample_rate_hz = open_config->sample_rate_hz;

    err = MPU_ApplyConfig(&config);
    if (err != E_MPU_ERR_NONE)
    {
        return err;
    }

    memset(&g_mpu.last_data, 0, sizeof(g_mpu.last_data));
    memset(&g_mpu.degree, 0, sizeof(g_mpu.degree));
    memset(g_mpu.gyro_offset_dps, 0, sizeof(g_mpu.gyro_offset_dps));
    g_mpu.last_update_tick = osKernelSysTick();

    if (open_config->auto_calibrate != 0U)
    {
        err = MPU_CalibrateGyro(open_config->calibration_samples);
        if (err != E_MPU_ERR_NONE)
        {
            return err;
        }
    }

    return E_MPU_ERR_NONE;
}

static void MPU_UpdateAnglesFromLastRead(void)
{
    uint32_t now_tick = osKernelSysTick();
    uint32_t elapsed_tick = now_tick - g_mpu.last_update_tick;
    float dt = ((float)elapsed_tick) / 1000.0f;

    float roll_acc;
    float pitch_acc;

    if ((dt <= 0.0f) || (dt > 1.0f))
    {
        dt = 0.01f;
    }

    roll_acc = atan2f(g_mpu.last_data.acc_g[1], g_mpu.last_data.acc_g[2]) * MPU_RAD_TO_DEG;
    pitch_acc = atan2f(-g_mpu.last_data.acc_g[0],
                       sqrtf((g_mpu.last_data.acc_g[1] * g_mpu.last_data.acc_g[1]) +
                             (g_mpu.last_data.acc_g[2] * g_mpu.last_data.acc_g[2]))) * MPU_RAD_TO_DEG;

    g_mpu.degree.angle_roll =
        MPU_COMPLEMENTARY_ALPHA * (g_mpu.degree.angle_roll + g_mpu.last_data.gyro_dps[0] * dt) +
        (1.0f - MPU_COMPLEMENTARY_ALPHA) * roll_acc;

    g_mpu.degree.angle_pitch =
        MPU_COMPLEMENTARY_ALPHA * (g_mpu.degree.angle_pitch + g_mpu.last_data.gyro_dps[1] * dt) +
        (1.0f - MPU_COMPLEMENTARY_ALPHA) * pitch_acc;

    g_mpu.last_data.angle_roll = g_mpu.degree.angle_roll;
    g_mpu.last_data.angle_pitch = g_mpu.degree.angle_pitch;
    g_mpu.last_update_tick = now_tick;
}

MPU_ErrorCode_t MPU_Open(void *vpParam)
{
    MPU_OpenConfig_t default_config;
    MPU_OpenConfig_t *open_config;
    MPU_ErrorCode_t err;

    if (g_mpu.state == MPU_STATE_OPEN)
    {
        return E_MPU_ERR_ALREADY_OPEN;
    }

    /* MPU_Open ile eğer MPU değerleri boş ise ilk değer atamalarını yapıyorum. */
    if (vpParam == NULL)
    {
        default_config.hi2c = MPU_TRANSFER_PORT;
        default_config.i2c_addr_7bit = MPU_DEFAULT_I2C_ADDR_7BIT;
        default_config.pwr_mgmt_1 = 0x01U;
        default_config.config = 0x03U;
        default_config.gyro_config = 0x08U;
        default_config.accel_config = 0x08U;
        default_config.sample_rate_hz = 100U;
        default_config.auto_calibrate = 1U;
        default_config.calibration_samples = 500U;
        open_config = &default_config;
    }
    else
    {
        open_config = (MPU_OpenConfig_t *)vpParam;
    }

    if ((open_config->hi2c == NULL) || (open_config->i2c_addr_7bit == 0U))
    {
        return E_MPU_ERR_INVALID_PARAM;
    }

    err = MPU_HardwareInit(open_config);
    if (err != E_MPU_ERR_NONE)
    {
        g_mpu.state = MPU_STATE_CLOSED;
        return err;
    }

    g_mpu.state = MPU_STATE_OPEN;
    return E_MPU_ERR_NONE;
}

MPU_ErrorCode_t MPU_Read(void *pvBuffer, uint32_t xBytes)
{
    uint8_t raw[14];
    MPU_Data_t *out_data;
    MPU_ErrorCode_t err;

    if (g_mpu.state != MPU_STATE_OPEN)
    {
        return E_MPU_ERR_NOT_OPEN;
    }

    if ((pvBuffer == NULL) || (xBytes < sizeof(MPU_Data_t)))
    {
        return E_MPU_ERR_INVALID_PARAM;
    }

    err = MPU_ReadRegister(MPU_REG_ACCEL_XOUT_H, raw, sizeof(raw));
    if (err != E_MPU_ERR_NONE)
    {
        return err;
    }

    g_mpu.last_data.acc_raw[0] = (int16_t)((raw[0] << 8) | raw[1]);
    g_mpu.last_data.acc_raw[1] = (int16_t)((raw[2] << 8) | raw[3]);
    g_mpu.last_data.acc_raw[2] = (int16_t)((raw[4] << 8) | raw[5]);

    g_mpu.last_data.gyro_raw[0] = (int16_t)((raw[8]  << 8) | raw[9]);
    g_mpu.last_data.gyro_raw[1] = (int16_t)((raw[10] << 8) | raw[11]);
    g_mpu.last_data.gyro_raw[2] = (int16_t)((raw[12] << 8) | raw[13]);

    g_mpu.last_data.acc_g[0] = ((float)g_mpu.last_data.acc_raw[0]) / MPU_ACCEL_SENS_4G;
    g_mpu.last_data.acc_g[1] = ((float)g_mpu.last_data.acc_raw[1]) / MPU_ACCEL_SENS_4G;
    g_mpu.last_data.acc_g[2] = ((float)g_mpu.last_data.acc_raw[2]) / MPU_ACCEL_SENS_4G;

    g_mpu.last_data.gyro_dps[0] = (((float)g_mpu.last_data.gyro_raw[0]) / MPU_GYRO_SENS_500DPS) - g_mpu.gyro_offset_dps[0];
    g_mpu.last_data.gyro_dps[1] = (((float)g_mpu.last_data.gyro_raw[1]) / MPU_GYRO_SENS_500DPS) - g_mpu.gyro_offset_dps[1];
    g_mpu.last_data.gyro_dps[2] = (((float)g_mpu.last_data.gyro_raw[2]) / MPU_GYRO_SENS_500DPS) - g_mpu.gyro_offset_dps[2];

    MPU_UpdateAnglesFromLastRead();

    out_data = (MPU_Data_t *)pvBuffer;
    *out_data = g_mpu.last_data;

    return E_MPU_ERR_NONE;
}

MPU_ErrorCode_t MPU_Write(const void *pvBuffer, uint32_t xBytes)
{
    const MPU_RegisterWrite_t *write_request;

    if (g_mpu.state != MPU_STATE_OPEN)
    {
        return E_MPU_ERR_NOT_OPEN;
    }

    if ((pvBuffer == NULL) || (xBytes != sizeof(MPU_RegisterWrite_t)))
    {
        return E_MPU_ERR_INVALID_PARAM;
    }

    write_request = (const MPU_RegisterWrite_t *)pvBuffer;
    return MPU_WriteRegister(write_request->reg, write_request->data, write_request->len);
}

MPU_ErrorCode_t MPU_Ioctl(MPU_IoctlCommand_t command, void *vpParam)
{
    uint8_t who_am_i;
    MPU_ErrorCode_t err;

    if (g_mpu.state != MPU_STATE_OPEN)
    {
        return E_MPU_ERR_NOT_OPEN;
    }

    switch (command)
    {
        case E_MPU_IOCTL_GET_WHO_AM_I:
            if (vpParam == NULL) { return E_MPU_ERR_INVALID_PARAM; }
            err = MPU_ReadRegister(MPU_REG_WHO_AM_I, &who_am_i, 1U);
            if (err != E_MPU_ERR_NONE) { return err; }
            *(uint8_t *)vpParam = who_am_i;
            return E_MPU_ERR_NONE;

        case E_MPU_IOCTL_SET_CONFIG:
            if (vpParam == NULL) { return E_MPU_ERR_INVALID_PARAM; }
            return MPU_ApplyConfig((const MPU_Config_t *)vpParam);

        case E_MPU_IOCTL_SET_SAMPLE_RATE:
            if (vpParam == NULL) { return E_MPU_ERR_INVALID_PARAM; }
            return MPU_SetSampleRate(*(uint16_t *)vpParam);

        case E_MPU_IOCTL_CALIBRATE_GYRO:
            if (vpParam == NULL) { return E_MPU_ERR_INVALID_PARAM; }
            return MPU_CalibrateGyro(*(uint16_t *)vpParam);

        case E_MPU_IOCTL_GET_DEGREE:
            if (vpParam == NULL) { return E_MPU_ERR_INVALID_PARAM; }
            *(MPU_Degree_t *)vpParam = g_mpu.degree;
            return E_MPU_ERR_NONE;

        case E_MPU_IOCTL_SLEEP:
            return MPU_WriteRegister8(MPU_REG_PWR_MGMT_1, 0x40U);

        case E_MPU_IOCTL_WAKEUP:
            return MPU_WriteRegister8(MPU_REG_PWR_MGMT_1, g_mpu.config.pwr_mgmt_1);

        case E_MPU_IOCTL_GET_STATE:
            if (vpParam == NULL) { return E_MPU_ERR_INVALID_PARAM; }
            *(MPU_State_t *)vpParam = g_mpu.state;
            return E_MPU_ERR_NONE;

        default:
            return E_MPU_ERR_UNSUPPORTED;
    }
}

MPU_ErrorCode_t MPU_Close(void *vpParam)
{
    (void)vpParam;

    if (g_mpu.state != MPU_STATE_OPEN)
    {
        return E_MPU_ERR_NOT_OPEN;
    }

    (void)MPU_WriteRegister8(MPU_REG_PWR_MGMT_1, 0x40U); /* Sleep mode alıyoruz. */
    g_mpu.state = MPU_STATE_CLOSED;
    g_mpu.hi2c = NULL;

    return E_MPU_ERR_NONE;
}

MPU_Degree_t MPU_GetDegree(void)
{
    return g_mpu.degree;
}

MPU_ErrorCode_t MPU_CalibrateGyro(uint16_t sample_count)
{
    uint8_t raw[14];
    int16_t gyro_raw[3];
    float sum[3] = {0.0f, 0.0f, 0.0f};
    MPU_ErrorCode_t err;

    if (g_mpu.hi2c == NULL)
    {
        return E_MPU_ERR_INVALID_PARAM;
    }

    if (sample_count == 0U)
    {
        return E_MPU_ERR_INVALID_PARAM;
    }

    g_mpu.gyro_offset_dps[0] = 0.0f;
    g_mpu.gyro_offset_dps[1] = 0.0f;
    g_mpu.gyro_offset_dps[2] = 0.0f;

    for (uint16_t i = 0; i < sample_count; i++)
    {
        err = MPU_ReadRegister(MPU_REG_ACCEL_XOUT_H, raw, sizeof(raw));
        if (err != E_MPU_ERR_NONE)
        {
            return err;
        }

        gyro_raw[0] = (int16_t)((raw[8]  << 8) | raw[9]);
        gyro_raw[1] = (int16_t)((raw[10] << 8) | raw[11]);
        gyro_raw[2] = (int16_t)((raw[12] << 8) | raw[13]);

        sum[0] += ((float)gyro_raw[0]) / MPU_GYRO_SENS_500DPS;
        sum[1] += ((float)gyro_raw[1]) / MPU_GYRO_SENS_500DPS;
        sum[2] += ((float)gyro_raw[2]) / MPU_GYRO_SENS_500DPS;

        MPU_DelayMs(5U);
    }

    g_mpu.gyro_offset_dps[0] = sum[0] / (float)sample_count;
    g_mpu.gyro_offset_dps[1] = sum[1] / (float)sample_count;
    g_mpu.gyro_offset_dps[2] = sum[2] / (float)sample_count;

    return E_MPU_ERR_NONE;
}
