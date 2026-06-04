#include "mock_uart.h"
#include <string.h>

uint8_t mock_uart_tx_data[512];
uint16_t mock_uart_tx_len = 0U;
HAL_StatusTypeDef mock_uart_return_status = HAL_OK;

void MockUART_Init(void)
{
    memset(mock_uart_tx_data, 0, sizeof(mock_uart_tx_data));
    mock_uart_tx_len = 0U;
    mock_uart_return_status = HAL_OK;
}

HAL_StatusTypeDef __wrap_HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    if (mock_uart_return_status == HAL_OK)
    {
        if (Size <= 512U)
        {
            memcpy(mock_uart_tx_data, pData, Size);
            mock_uart_tx_len = Size;
        }
    }
    return mock_uart_return_status;
}

HAL_StatusTypeDef __wrap_HAL_UARTEx_ReceiveToIdle_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    /* Sadece durum dondurmesi yeterlidir, donanimin DMA islemini taklit eder */
    return mock_uart_return_status;
}

HAL_StatusTypeDef __wrap_HAL_UART_DMAStop(UART_HandleTypeDef *huart)
{
    return mock_uart_return_status;
}
