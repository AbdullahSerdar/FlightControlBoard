#include "unity.h"
#include "mpu_unit_test.h"
#include "mpu_driver.h"
#include "mock_i2c.h"

extern I2C_HandleTypeDef hi2c1;

static void Test_MPU_Open_Success(void)
{
    MPU_OpenConfig_t config = {0};
    config.hi2c = &hi2c1;
    config.i2c_addr_7bit = MPU_DEFAULT_I2C_ADDR_7BIT;
    config.sample_rate_hz = 100U; /* Cok onemli: Sifir kalirsa Invalid_Param(3) doner! */
    config.auto_calibrate = 0U;   /* Testi hizlandirmak icin kalibrasyonu kapattik */

    uint8_t fake_who_am_i = 0x68; /* MPU_WHO_AM_I_EXPECTED */
    MockI2C_SetReadData(&fake_who_am_i, 1);

    MPU_ErrorCode_t err = MPU_Open(&config);
    TEST_ASSERT_EQUAL(E_MPU_ERR_NONE, err);

    /* Bir sonraki test icin surucuyu kapatip temizliyoruz */
    (void)MPU_Close(NULL);
}

static void Test_MPU_Open_WrongId(void)
{
    MPU_OpenConfig_t config = {0};
    config.hi2c = &hi2c1;
    config.i2c_addr_7bit = MPU_DEFAULT_I2C_ADDR_7BIT;
    config.sample_rate_hz = 100U;

    uint8_t fake_who_am_i = 0x99; /* Yanlis ID */
    MockI2C_SetReadData(&fake_who_am_i, 1);

    MPU_ErrorCode_t err = MPU_Open(&config);
    TEST_ASSERT_EQUAL(E_MPU_ERR_WRONG_ID, err);
}

void RunMpuUnitTests(void)
{
    RUN_TEST(Test_MPU_Open_Success);
    RUN_TEST(Test_MPU_Open_WrongId);
}
