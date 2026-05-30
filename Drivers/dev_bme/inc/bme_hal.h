/*
 * bme_hal.h
 *
 *  Created on: May 18, 2026
 *      Author: serda
 */

#ifndef DEV_BME_INC_BME_HAL_H_
#define DEV_BME_INC_BME_HAL_H_

extern I2C_HandleTypeDef hi2c2;

#define BME_TRANSFER_PORT   (&hi2c2)

#endif /* DEV_BME_INC_BME_HAL_H_ */
