/*
 * bme_driver.c
 *
 *  Created on: Apr 24, 2026
 *      Author: serda
 */

#include "bme_driver.h"
#include <math.h>
#include "cmsis_os.h"

#define ALTITUDE(press)   	(44330.0 * (1.0 - pow( (press) / 101325.0 , 0.1903)))

//int32_t T;
int32_t t_fine;
double altitude;
double temperature_c;
int32_t adc_p;
int32_t adc_t;

uint8_t trim_prmt[24];
uint16_t dig_P1;
int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;

uint16_t dig_T1;
int16_t dig_T2, dig_T3;

int read_trim_values()
{
	HAL_StatusTypeDef status;
	status = HAL_I2C_Mem_Read(BME_TRANSFER_PORT, BME_ADRESS << 1, BME_CONST, I2C_MEMADD_SIZE_8BIT, trim_prmt, 24, 1000);

	if(status != HAL_OK)
	{
		return status;
	}

	dig_T1 = (uint16_t)((trim_prmt[1] << 8) | trim_prmt[0]);
	dig_T2 = (int16_t)((trim_prmt[3]  << 8) | trim_prmt[2]);
	dig_T3 = (int16_t)((trim_prmt[5]  << 8) | trim_prmt[4]);

	dig_P1 = (uint16_t)((trim_prmt[7] << 8) | trim_prmt[6]);
	dig_P2 = (int16_t)((trim_prmt[9]  << 8) | trim_prmt[8]);
	dig_P3 = (int16_t)((trim_prmt[11] << 8) | trim_prmt[10]);
	dig_P4 = (int16_t)((trim_prmt[13] << 8) | trim_prmt[12]);
	dig_P5 = (int16_t)((trim_prmt[15] << 8) | trim_prmt[14]);
	dig_P6 = (int16_t)((trim_prmt[17] << 8) | trim_prmt[16]);
	dig_P7 = (int16_t)((trim_prmt[19] << 8) | trim_prmt[18]);
	dig_P8 = (int16_t)((trim_prmt[21] << 8) | trim_prmt[20]);
	dig_P9 = (int16_t)((trim_prmt[23] << 8) | trim_prmt[22]);

	return HAL_OK;
}




uint8_t readChipID(uint8_t *chip_id)
{
	HAL_I2C_Mem_Read(BME_TRANSFER_PORT, BME_ADRESS << 1, BMECHIP_ID_REGISTER, I2C_MEMADD_SIZE_8BIT, chip_id, 1, 1000);
	osDelay(50);
	return *chip_id;
}



int read_raw_data()
{
	HAL_StatusTypeDef status;
	uint8_t raw_sensor_data[6];
	status = HAL_I2C_Mem_Read(BME_TRANSFER_PORT, BME_ADRESS << 1, BME_PRESS_DATA, I2C_MEMADD_SIZE_8BIT, raw_sensor_data, 6, 1000);

	if (status != HAL_OK){ return status; }

	adc_p = ((uint32_t)raw_sensor_data[0] << 12) | ((uint32_t)raw_sensor_data[1] << 4) | ((uint32_t)raw_sensor_data[2] >> 4);
	adc_t = ((int32_t)raw_sensor_data[3] << 12) | ((int32_t)raw_sensor_data[4] << 4) | ((int32_t)raw_sensor_data[5] >> 4);

	return HAL_OK;
}


void compansate_temp(int32_t adc_T)
{
	int32_t var1, var2;
    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;

    t_fine = var1 + var2;
//    T = (t_fine * 5 + 128) >> 8;
    temperature_c = (float)(((t_fine * 5 + 128) >> 8) / 100.0);
//    return T;
}

uint32_t compensate_pressure(int32_t adc_P)
{
    int64_t var1, var2, p;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dig_P6;
    var2 = var2 + ((var1 *(int64_t)dig_P5) << 17);
    var2 = var2 + (((int64_t)dig_P4) << 35);
    var1 = ((var1 * var1 *(int64_t)dig_P3) >> 8) +((var1 * (int64_t)dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;
    if(var1 == 0)
    {
    	return 0;
    }

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);

    return (uint32_t)p;
}




int BME_Config(uint8_t osrs_t, uint8_t osrs_p, uint8_t mode, uint8_t t_sb, uint8_t filter)
{
    HAL_StatusTypeDef status;

    uint8_t reset_val = 0xB6;
    HAL_I2C_Mem_Write(BME_TRANSFER_PORT, BME_ADRESS << 1, BME_RESET, I2C_MEMADD_SIZE_8BIT, &reset_val, 1, 100);
    osDelay(10);

    if (read_trim_values() != HAL_OK) { return HAL_ERROR; }

    uint8_t dataowrite = (t_sb << 5) | (filter << 2);
    status = HAL_I2C_Mem_Write(BME_TRANSFER_PORT, BME_ADRESS << 1, BME_CONFIG,
                                I2C_MEMADD_SIZE_8BIT, &dataowrite, 1, 100);
    if (status != HAL_OK) { return status; }
    osDelay(10);

    dataowrite = (osrs_t << 5) | (osrs_p << 2) | mode;
    status = HAL_I2C_Mem_Write(BME_TRANSFER_PORT, BME_ADRESS << 1, BME_CTRL_MEAS,
                                I2C_MEMADD_SIZE_8BIT, &dataowrite, 1, 100);
    if (status != HAL_OK) { return status; }
    osDelay(10);

    return HAL_OK;
}

void BME_Measure()
{
	if(!read_raw_data())
	{
		if(adc_t == 0x800000)
        {
			altitude = 0;
            temperature_c = 0;
        }
		else
        {
			compansate_temp(adc_t);
        }

        if (adc_p == 0x800000)
        	altitude = 0;
        else
        {
        	 altitude = ALTITUDE(((compensate_pressure(adc_p)) / 256.0f));
        }

	}
	else
	{
		altitude  = 0;
        temperature_c = 0;
	}

}

float BME_GetAltitude(void)
{
    return (float)altitude;
}

float BME_GetTemperature(void)
{
    return (float)temperature_c;
}



