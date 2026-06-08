#include "unity.h"
#include "mock_i2c.h"
#include "mock_uart.h"
#include "test_runner.h"

#include "bme_unit_test.h"
#include "gps_unit_test.h"
#include "lora_unit_test.h"
#include "mpu_unit_test.h"

#include <stdio.h>

void setUp(void)
{
    MockI2C_Init();
    MockUART_Init();
}

void tearDown(void)
{
	// Bosluk
}

void RunAllTests(void)
{
    UNITY_BEGIN();

    RunBmeUnitTests();
    RunGpsUnitTests();
    RunLoraUnitTests();
    RunMpuUnitTests();

    int result = UNITY_END();

}
