/*
 * bme_driver.h
 *
 *  Created on: Apr 24, 2026
 *      Author: serda
 */

#ifndef INC_BME_DRIVER_H_
#define INC_BME_DRIVER_H_

#include "main.h"
#include <stdint.h>

#define BME_CHIP_ID 		0x60

#define BME_RESET		     0xE0
#define BME_CHIP_ID_REGISTER 0xD0
#define BME_ADRESS           0x76

#define BME_STATUS			 0xF3
#define BME_CTRL_MEAS		 0xF4
#define BME_CONFIG			 0xF5
#define BME_PRESS_DATA		 0xF7
#define BME_TEMP_DATA		 0xFA
#define BME_CONST			 0x88

#define OSRS_OFF    	0x00
#define OSRS_1      	0x01
#define OSRS_2      	0x02
#define OSRS_4      	0x03
#define OSRS_8      	0x04
#define OSRS_16     	0x05

#define T_SB0_5 	0x00
#define T_SB62_5 	0x01
#define T_SB125 	0x02
#define T_SB250 	0x03
#define T_SB500 	0x04
#define T_SB1000 	0x05
#define T_SB2000 	0x06
#define T_SB4000 	0x07

#define BME_SLEEP_MODE       0x00
#define BME_FORCED_MODE		 0x01
#define BME_NORMAL_MODE	     0x03

#define IIR_OFF	 0x00
#define IIR_2	 0x01
#define IIR_4	 0x02
#define IIR_8	 0x03
#define IIR_16	 0x04

typedef enum {
    E_BME_ERR_NONE = 0,
    E_BME_ERR_HAL,
    E_BME_ERR_WRONG_ID,
    E_BME_ERR_INVALID_PARAM,
    E_BME_ERR_NOT_OPEN,
    E_BME_ERR_ALREADY_OPEN,
    E_BME_ERR_UNSUPPORTED,
    E_BME_ERR_UNKNOWN
} BME_ErrorCode_t;

typedef enum {
	E_BME_IOCTL_GET_CHIP_ID = 0,
	E_BME_IOCTL_RESET,
	E_BME_IOCTL_GET_STATUS,
	E_BME_IOCTL_SET_CONFIG,
	E_BME_IOCTL_GET_ALTITUDE,
	E_BME_IOCTL_GET_TEMP,
	E_BME_IOCTL_GET_TRIM,
	E_BME_IOCTL_CALIBRATE
} BME_IoctlCommand_t;

typedef struct
{
    uint8_t reg;
    const uint8_t *data;
    uint16_t len;
} BME_RegisterWrite_t;

typedef enum {
	BME_STATE_CLOSED = 0,
	BME_STATE_OPEN
} BME_State_t;

typedef struct {
	int32_t adc_p;
	int32_t adc_t;

	double altitude;
	double temperature_c;
} BME_Data_t;

typedef struct {
	I2C_HandleTypeDef *hi2c;
	uint8_t i2c_addr_7bit;

	uint8_t reset_val;
	uint8_t osrs_t;
	uint8_t osrs_p;
	uint8_t mode;
	uint8_t t_sb;
	uint8_t filter;
} BME_OpenConfig_t;

typedef struct {
	uint8_t reset_val;
	uint8_t osrs_t;
	uint8_t osrs_p;
	uint8_t mode;
	uint8_t t_sb;
	uint8_t filter;
} BME_Config_t;

BME_ErrorCode_t BME_Open(void *vpParam);
BME_ErrorCode_t BME_Ioctl(BME_IoctlCommand_t command, void *vpParam);
BME_ErrorCode_t BME_Write(const void *pvBuffer, uint32_t xBytes);
BME_ErrorCode_t BME_Read(void *pvBuffer, uint32_t xBytes);
BME_ErrorCode_t BME_Close(void *vpParam);

#endif /* INC_BME_DRIVER_H_ */




