#include "unity.h"
#include "bme_unit_test.h"
#include "bme_driver.h"
#include "mock_i2c.h"

extern I2C_HandleTypeDef hi2c2;

static void Test_BME_Open_Success(void)
{
    BME_OpenConfig_t config = {0};
    config.hi2c = &hi2c2;
    config.i2c_addr_7bit = BME_ADRESS;

    uint8_t fake_chip_id = 0x60; /* BME_CHIP_ID */
    MockI2C_SetReadData(&fake_chip_id, 1);

    BME_ErrorCode_t err = BME_Open(&config);
    TEST_ASSERT_EQUAL(E_BME_ERR_NONE, err);

    /* Bir sonraki test icin surucuyu kapatip temizliyoruz */
    (void)BME_Close(NULL);
}

static void Test_BME_Open_WrongChipId(void)
{
    BME_OpenConfig_t config = {0};
    config.hi2c = &hi2c2;
    config.i2c_addr_7bit = BME_ADRESS;

    uint8_t fake_chip_id = 0x55; /* Yanlis ID */
    MockI2C_SetReadData(&fake_chip_id, 1);

    BME_ErrorCode_t err = BME_Open(&config);
    TEST_ASSERT_EQUAL(E_BME_ERR_WRONG_ID, err);
}

void RunBmeUnitTests(void)
{
    RUN_TEST(Test_BME_Open_Success);
    RUN_TEST(Test_BME_Open_WrongChipId);
}
