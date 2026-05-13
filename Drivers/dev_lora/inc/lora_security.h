/*
 * lora_security.h
 *
 *  Created on: May 13, 2026
 *      Author: serda
 */

#ifndef DEV_LORA_INC_LORA_SECURITY_H_
#define DEV_LORA_INC_LORA_SECURITY_H_

#include <stdint.h>
#include <stddef.h>

#ifndef LORA_SECURITY_USE_HAL_RNG
#define LORA_SECURITY_USE_HAL_RNG   0U
#endif

#define LORA_SECURITY_VERSION       0x01U
#define LORA_SECURITY_MSG_TELEMETRY 0x01U

void     LoraSecurity_Init(void);
uint32_t LoraSecurity_GetSessionId(void);
uint32_t LoraSecurity_NextSequence(void);
uint16_t LoraSecurity_Crc16Ccitt(const uint8_t *data, uint16_t len);
uint32_t LoraSecurity_AuthTag32(const uint8_t *data, uint16_t len);


#endif /* DEV_LORA_INC_LORA_SECURITY_H_ */
