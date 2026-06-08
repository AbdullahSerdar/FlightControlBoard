#include "unity.h"
#include "gps_unit_test.h"
#include "gps_driver.h"
#include "mock_uart.h"

extern UART_HandleTypeDef huart3;

static void Test_GPS_Open_Success(void)
{
    GPS_OpenConfig_t config = {0};
    config.huart = &huart3;

    mock_uart_return_status = HAL_OK;
    GPSErrorCodes_t err = Gps_Open(&config);

    TEST_ASSERT_EQUAL(E_GPS_ERR_NONE, err);

    /* Bir sonraki test icin surucuyu kapatip temizliyoruz */
    (void)Gps_Close(NULL);
}

static void Test_GPS_Open_HalError(void)
{
    GPS_OpenConfig_t config = {0};
    config.huart = &huart3;

    mock_uart_return_status = HAL_ERROR; /* DMA baslatilamayacak */
    GPSErrorCodes_t err = Gps_Open(&config);

    TEST_ASSERT_EQUAL(E_GPS_ERR_UART, err);
}

void RunGpsUnitTests(void)
{
    RUN_TEST(Test_GPS_Open_Success);
    RUN_TEST(Test_GPS_Open_HalError);
}
