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

#define BME_TEST_ENABLE 		0

extern I2C_HandleTypeDef hi2c2;

#define BME_CHIP_ID 		0x60
#define BME_TRANSFER_PORT   (&hi2c2)

#define BME_RESET		     0xE0
#define BMECHIP_ID_REGISTER  0xD0
#define BME_CHIP_ID          0x60
#define BME_ADRESS           0x76

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
	E_BME_ERR_NONE,
	E_BME_ERR_WRONG_ID,
	E_BME_ERR_HAL,
	E_BME_ERR_UNKNOWN,
}Bme_ErrorCodes;

Bme_ErrorCodes BME_Open(uint8_t osrs_t, uint8_t osrs_p, uint8_t mode, uint8_t t_sb, uint8_t filter);

void BME_Measure(void);

float BME_GetAltitude(void);
float BME_GetTemperature(void);

int8_t BME_TEST(uint32_t timeout);

#endif /* INC_BME_DRIVER_H_ */




