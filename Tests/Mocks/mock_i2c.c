#include "mock_i2c.h"
#include <string.h>

uint8_t mock_i2c_read_data[256];
uint8_t mock_i2c_write_data[256];
HAL_StatusTypeDef mock_i2c_return_status = HAL_OK;

void MockI2C_Init(void)
{
    /* Testler arasi mock verilerini temizle */
    memset(mock_i2c_read_data, 0, sizeof(mock_i2c_read_data));
    memset(mock_i2c_write_data, 0, sizeof(mock_i2c_write_data));
    mock_i2c_return_status = HAL_OK;
}

void MockI2C_SetReadData(uint8_t *data, uint16_t len)
{
    if(data != NULL && len <= 256)
    {
        memcpy(mock_i2c_read_data, data, len);
    }
}

/* Orijinal HAL_I2C_Mem_Read fonksiyonunu ezen (wrap) sahte fonksiyon */
HAL_StatusTypeDef __wrap_HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    if(mock_i2c_return_status == HAL_OK)
    {
        memcpy(pData, mock_i2c_read_data, Size);
    }
    return mock_i2c_return_status;
}

/* Orijinal HAL_I2C_Mem_Write fonksiyonunu ezen (wrap) sahte fonksiyon */
HAL_StatusTypeDef __wrap_HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    if(mock_i2c_return_status == HAL_OK)
    {
        memcpy(mock_i2c_write_data, pData, Size);
    }
    return mock_i2c_return_status;
}
