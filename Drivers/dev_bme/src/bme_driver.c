/*
 * bme_driver.c
 *
 *  Created on: Apr 24, 2026
 *      Author: serda
 */

#include "bme_driver.h"
#include <math.h>
#include <cmsis_os.h>
#include <stdio.h>
#include "bme_hal.h"

#define ALTITUDE(press)   	(44330.0 * (1.0 - pow( (press) / 101325.0 , 0.1903)))

typedef struct {
	I2C_HandleTypeDef *hi2c;
	uint8_t i2c_addr_7bit;
	BME_State_t state;

	BME_Config_t config;
	BME_Data_t last_data;
} BME_Context_t;

static BME_Context_t g_bme = {0};

//int32_t T;
static int32_t t_fine;
static double temperature_c;

static uint16_t dig_P1;
static int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;

static uint16_t dig_T1;
static int16_t dig_T2, dig_T3;

/* BME I2C OKUMA YAZMA KISIMLARI*/
static uint16_t BME_DevAddr8(void)
{
    return (uint16_t)(g_bme.i2c_addr_7bit << 1U);
}

static BME_ErrorCode_t BME_WriteRegister(uint8_t reg, const uint8_t *data, uint16_t len)
{
    if ((g_bme.hi2c == NULL) || (data == NULL) || (len == 0U))
    {
        return E_BME_ERR_INVALID_PARAM;
    }

    if (HAL_I2C_Mem_Write(g_bme.hi2c,
                          BME_DevAddr8(),
                          reg,
                          I2C_MEMADD_SIZE_8BIT,
						  (uint8_t *)data,
                          len,
                          1000U) != HAL_OK)
    {
        return E_BME_ERR_HAL;
    }

    return E_BME_ERR_NONE;
}

static BME_ErrorCode_t BME_ReadRegister(uint8_t reg, uint8_t *data, uint16_t len)
{
    if ((g_bme.hi2c == NULL) || (data == NULL) || (len == 0U))
    {
        return E_BME_ERR_INVALID_PARAM;
    }

    if (HAL_I2C_Mem_Read(g_bme.hi2c,
                         BME_DevAddr8(),
                         reg,
                         I2C_MEMADD_SIZE_8BIT,
                         data,
                         len,
                         1000U) != HAL_OK)
    {
        return E_BME_ERR_HAL;
    }

    return E_BME_ERR_NONE;
}

/* TRIM DEGERLERİNİN OKUNMASI*/
static BME_ErrorCode_t read_trim_values()
{
	BME_ErrorCode_t err;
	static uint8_t trim_prmt[24];
	err = BME_ReadRegister(BME_CONST, trim_prmt, 24);

	if(err != E_BME_ERR_NONE)
	{
		return E_BME_ERR_HAL;
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

	return E_BME_ERR_NONE;
}

/* BME OPEN YAPISININ FONKSİYONLARI*/
static BME_ErrorCode_t BME_ApplyConfig(const BME_Config_t *config)
{
	BME_ErrorCode_t err;
	uint8_t datawrite = 0;

	if(config == NULL)
	{
		return E_BME_ERR_INVALID_PARAM;
	}

//	datawrite = config->reset_val;
//	err = BME_WriteRegister(BME_RESET, &datawrite, 1U);
//	if (err != E_BME_ERR_NONE) { return err; }
//	osDelay(50);

	if(read_trim_values() != E_BME_ERR_NONE)
	{
		return E_BME_ERR_HAL;
	}

	datawrite = (config->t_sb << 5) | (config->filter << 2);
	err = BME_WriteRegister(BME_CONFIG, &datawrite, 1U);
	if (err != E_BME_ERR_NONE) { return err; }
	osDelay(20);

	datawrite = (config->osrs_t << 5) | (config->osrs_p << 2) | config->mode;
	err = BME_WriteRegister(BME_CTRL_MEAS, &datawrite, 1U);
	if (err != E_BME_ERR_NONE) { return err; }
	osDelay(20);

	g_bme.config = *config;
	return E_BME_ERR_NONE;
}

static BME_ErrorCode_t BME_HardwareInit(const BME_OpenConfig_t *open_config)
{
	uint8_t chip_id = 0U;
	BME_Config_t config;
	BME_ErrorCode_t err;

	g_bme.hi2c = open_config->hi2c;
	g_bme.i2c_addr_7bit = open_config->i2c_addr_7bit;

	err = BME_ReadRegister(BME_CHIP_ID_REGISTER, &chip_id, 1U);
	if(err != E_BME_ERR_NONE)
	{
		return err;
	}

	if(chip_id != BME_CHIP_ID)
	{
		return E_BME_ERR_WRONG_ID;
	}

	config.filter    = open_config->filter;
	config.mode      = open_config->mode;
	config.osrs_p    = open_config->osrs_p;
	config.osrs_t    = open_config->osrs_t;
	config.reset_val = open_config->reset_val;
	config.t_sb      = open_config->t_sb;

	err = BME_ApplyConfig(&config);
    if (err != E_BME_ERR_NONE)
    {
        return err;
    }

    return E_BME_ERR_NONE;
}

BME_ErrorCode_t BME_Open(void *vpParam)
{
	BME_OpenConfig_t default_config;
	BME_OpenConfig_t *open_config;
	BME_ErrorCode_t err;

	if(g_bme.state == BME_STATE_OPEN)
	{
		return E_BME_ERR_ALREADY_OPEN;
	}

	if(vpParam == NULL)
	{
		default_config.hi2c = BME_TRANSFER_PORT;
		default_config.i2c_addr_7bit = BME_ADRESS;
		default_config.filter = IIR_16;
		default_config.mode = BME_NORMAL_MODE;
		default_config.osrs_p = OSRS_16;
		default_config.osrs_t = OSRS_2;
		default_config.reset_val = 0xB6;
		default_config.t_sb = T_SB1000;
		open_config = &default_config;
	}
	else
	{
		open_config = (BME_OpenConfig_t *)vpParam;
	}

    if ((open_config->hi2c == NULL) || (open_config->i2c_addr_7bit == 0U))
    {
        return E_BME_ERR_INVALID_PARAM;
    }

    err = BME_HardwareInit(open_config);
    if (err != E_BME_ERR_NONE)
    {
        g_bme.state = BME_STATE_CLOSED;
        return err;
    }

    g_bme.state = BME_STATE_OPEN;
    return E_BME_ERR_NONE;
}


/* BME READ FONKSIYONLARI */
static float compansate_temp(int32_t adc_T)
{
	int32_t var1, var2;
    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;

    t_fine = var1 + var2;
//    T = (t_fine * 5 + 128) >> 8;
    temperature_c = (double)(((t_fine * 5 + 128) >> 8) / 100.0);
    return temperature_c;
}

static uint32_t compensate_pressure(int32_t adc_P)
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

BME_ErrorCode_t BME_Read(void *pvBuffer, uint32_t xBytes)
{
	uint8_t raw[6];
	BME_Data_t *out_data;
	BME_ErrorCode_t err;

	if(g_bme.state != BME_STATE_OPEN)
	{
		return E_BME_ERR_NOT_OPEN;
	}

    if ((pvBuffer == NULL) || (xBytes < sizeof(BME_Data_t)))
    {
        return E_BME_ERR_INVALID_PARAM;
    }

    out_data = (BME_Data_t *)pvBuffer;

    err = BME_ReadRegister(BME_PRESS_DATA, raw, 6);
    if(err != E_BME_ERR_NONE)
    {
    	return err;
    }

    out_data->adc_p = ((uint32_t)raw[0] << 12) | ((uint32_t)raw[1] << 4) | ((uint32_t)raw[2] >> 4);
	out_data->adc_t = ((int32_t)raw[3] << 12) | ((int32_t)raw[4] << 4) | ((int32_t)raw[5] >> 4);

	if(out_data->adc_t == 0x800000)
	{
		out_data->altitude      = 0;
		out_data->temperature_c = 0;
	}
	else
	{
		out_data->temperature_c = compansate_temp(out_data->adc_t);
	}

	if(out_data->adc_p == 0x800000)
	{
		out_data->altitude = 0;
	}
	else
	{
		out_data->altitude = ALTITUDE(((compensate_pressure(out_data->adc_p)) / 256.0f));
	}

	g_bme.last_data = *out_data;
	return E_BME_ERR_NONE;
}

BME_ErrorCode_t BME_Ioctl(BME_IoctlCommand_t command, void *vpParam)
{
	uint8_t chip_id;
	BME_ErrorCode_t err;

	if(g_bme.state != BME_STATE_OPEN)
	{
		return E_BME_ERR_NOT_OPEN;
	}

	switch(command)
	{
	case E_BME_IOCTL_GET_CHIP_ID:
		if (vpParam == NULL) { return E_BME_ERR_INVALID_PARAM; }
		err = BME_ReadRegister(BME_CHIP_ID_REGISTER, &chip_id, 1U);
		if (err != E_BME_ERR_NONE) { return err; }
		*(uint8_t *)vpParam = chip_id;
		return E_BME_ERR_NONE;

	case E_BME_IOCTL_RESET:
		if (vpParam == NULL) { return E_BME_ERR_INVALID_PARAM; }
		err = BME_WriteRegister(BME_RESET, &g_bme.config.reset_val, 1U);
		if (err != E_BME_ERR_NONE) { return err; }
		osDelay(50);
		return BME_ApplyConfig(&g_bme.config);

	case E_BME_IOCTL_GET_STATUS:
		if (vpParam == NULL) { return E_BME_ERR_INVALID_PARAM; }
		*(BME_State_t *)vpParam = g_bme.state;
		return E_BME_ERR_NONE;

	case E_BME_IOCTL_SET_CONFIG:
		if (vpParam == NULL) { return E_BME_ERR_INVALID_PARAM; }
		return BME_ApplyConfig((const BME_Config_t *)vpParam);

	case E_BME_IOCTL_GET_ALTITUDE:
	    if (vpParam == NULL) { return E_BME_ERR_INVALID_PARAM; }
	    *(double *)vpParam = g_bme.last_data.altitude;
	    return E_BME_ERR_NONE;

	case E_BME_IOCTL_GET_TEMP:
	    if (vpParam == NULL) { return E_BME_ERR_INVALID_PARAM; }
	    *(double *)vpParam = g_bme.last_data.temperature_c;
	    return E_BME_ERR_NONE;

	case E_BME_IOCTL_GET_TRIM:
		if (vpParam == NULL) { return E_BME_ERR_INVALID_PARAM; }
		return read_trim_values();

	case E_BME_IOCTL_CALIBRATE:
		return E_BME_ERR_NONE;

	default:
		return E_BME_ERR_UNSUPPORTED;
	}
}

BME_ErrorCode_t BME_Write(const void *pvBuffer, uint32_t xBytes)
{
    const BME_RegisterWrite_t *write_request;

    if (g_bme.state != BME_STATE_OPEN)
    {
        return E_BME_ERR_NOT_OPEN;
    }

    if ((pvBuffer == NULL) || (xBytes != sizeof(BME_RegisterWrite_t)))
    {
        return E_BME_ERR_INVALID_PARAM;
    }

    write_request = (const BME_RegisterWrite_t *)pvBuffer;
    return BME_WriteRegister(write_request->reg, write_request->data, write_request->len);
}

BME_ErrorCode_t BME_Close(void *vpParam)
{
	(void)vpParam;
	BME_ErrorCode_t err;
	uint8_t datawrite;

	if(g_bme.state != BME_STATE_OPEN)
	{
		return E_BME_ERR_NOT_OPEN;
	}

	datawrite = (g_bme.config.osrs_t << 5) | (g_bme.config.osrs_p << 2) | BME_SLEEP_MODE;
	err = BME_WriteRegister(BME_CTRL_MEAS, &datawrite, 1U);

	if (err != E_BME_ERR_NONE)
	{
	    return err;
	}

	g_bme.state = BME_STATE_CLOSED;
	g_bme.hi2c  = NULL;

	return E_BME_ERR_NONE;
}









