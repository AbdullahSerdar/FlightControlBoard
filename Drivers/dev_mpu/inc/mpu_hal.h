/*
 * mpu_hal.h
 *
 *  Created on: May 30, 2026
 *      Author: serda
 */

#ifndef DEV_MPU_INC_MPU_HAL_H_
#define DEV_MPU_INC_MPU_HAL_H_

extern I2C_HandleTypeDef hi2c1;

#define MPU_TRANSFER_PORT   (&hi2c1)

#endif /* DEV_MPU_INC_MPU_HAL_H_ */
