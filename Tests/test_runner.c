#include "unity.h"
#include "mock_i2c.h"
#include "mock_uart.h"

/* Tum test suite basliklari */
#include "bme_unit_test.h"
#include "gps_unit_test.h"
#include "lora_unit_test.h"
#include "mpu_unit_test.h"

/* * Unity tarafindan her test fonksiyonundan once cagirilir.
 * Multiple definition hatasini engellemek icin sadece burada tanimlanmistir.
 */
void setUp(void)
{
    MockI2C_Init();
    MockUART_Init();
}

/* Unity tarafindan her test fonksiyonundan sonra cagirilir. */
void tearDown(void)
{
    /* Gerekirse temizleme islemleri yapilabilir */
}

/* Projenin main.c dosyasindan cagrilacak ana test fonksiyonu */
void RunAllTests(void)
{
    UNITY_BEGIN();

    RunBmeUnitTests();
    RunGpsUnitTests();
    RunLoraUnitTests();
    RunMpuUnitTests();

    (void)UNITY_END();
}
