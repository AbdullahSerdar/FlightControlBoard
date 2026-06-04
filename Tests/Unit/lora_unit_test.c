#include "unity.h"
#include "lora_unit_test.h"
#include "lora_driver.h"
#include "mock_uart.h"

static void Test_LoRa_Transmit_Success(void)
{
    LoRaPacket_t test_packet = {0};
    test_packet.start_byte = 0xAB;

    mock_uart_return_status = HAL_OK;
    Lora_ErrorCode err = LoRa_Transmit(&test_packet);

    TEST_ASSERT_EQUAL(E_LORA_ERR_NONE, err);
    TEST_ASSERT_EQUAL(sizeof(LoRaPacket_t), mock_uart_tx_len);
    TEST_ASSERT_EQUAL_HEX8(0xAB, mock_uart_tx_data[0]); /* Gonderilen ilk byte kontrolu */
}

static void Test_LoRa_Transmit_NullPacket(void)
{
    Lora_ErrorCode err = LoRa_Transmit(NULL);
    TEST_ASSERT_EQUAL(E_LORA_ERR_NULL, err);
}

void RunLoraUnitTests(void)
{
    RUN_TEST(Test_LoRa_Transmit_Success);
    RUN_TEST(Test_LoRa_Transmit_NullPacket);
}
