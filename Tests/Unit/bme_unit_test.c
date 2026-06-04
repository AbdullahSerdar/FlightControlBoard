#include "unity.h"
#include "bme_unit_test.h"
#include "bme_driver.h"
#include "mock_i2c.h"

I2C_HandleTypeDef hi2c2_mock;


static void Test_BME_Open_Success(void)
{
    BME_OpenConfig_t config = {0};
    config.hi2c = &hi2c2_mock;
    config.i2c_addr_7bit = BME_ADRESS;
    config.filter = IIR_16;
    config.mode = BME_NORMAL_MODE;
    config.osrs_p = OSRS_16;
    config.osrs_t = OSRS_2;
    config.reset_val = 0xB6;
    config.t_sb = T_SB1000;

    /* Sahte Chip ID dondurulmesi ayarlaniyor */
    uint8_t fake_chip_id = 0x60;
    MockI2C_SetReadData(&fake_chip_id, 1);

    BME_ErrorCode_t err = BME_Open(&config);

    TEST_ASSERT_EQUAL(E_BME_ERR_NONE, err);
}

static void Test_BME_Open_WrongChipId(void)
{
    BME_OpenConfig_t config = {0};
    config.hi2c = &hi2c2_mock;
    config.i2c_addr_7bit = BME_ADRESS;

    /* Yanlis chip ID dondurulmesi ayarlaniyor (0x55) */
    uint8_t fake_chip_id = 0x55;
    MockI2C_SetReadData(&fake_chip_id, 1);

    BME_ErrorCode_t err = BME_Open(&config);

    TEST_ASSERT_EQUAL(E_BME_ERR_WRONG_ID, err);
}

void RunBmeUnitTests(void)
{
    RUN_TEST(Test_BME_Open_Success);
    RUN_TEST(Test_BME_Open_WrongChipId);
}
