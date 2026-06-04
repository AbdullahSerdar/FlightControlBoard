#ifndef MOCK_I2C_H_
#define MOCK_I2C_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>

/* I2C okuma ve yazma islemleri icin taklit (mock) degiskenler */
extern uint8_t mock_i2c_read_data[256];
extern uint8_t mock_i2c_write_data[256];
extern HAL_StatusTypeDef mock_i2c_return_status;

void MockI2C_Init(void);
void MockI2C_SetReadData(uint8_t *data, uint16_t len);
HAL_StatusTypeDef MockI2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef MockI2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);

#endif /* MOCK_I2C_H_ */
