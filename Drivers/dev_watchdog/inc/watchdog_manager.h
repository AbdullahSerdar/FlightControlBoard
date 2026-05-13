/*
 * watchdog_manager.h
 *
 *  Created on: May 13, 2026
 *      Author: serda
 */

#ifndef INC_WATCHDOG_MANAGER_H_
#define INC_WATCHDOG_MANAGER_H_

#include <stdint.h>

void Watchdog_ReportBme(void);
void Watchdog_ReportGps(void);
void Watchdog_ReportMpu(void);
void Watchdog_ReportLora(void);

uint8_t Watchdog_AllTasksAlive(void);
void Watchdog_ClearFlags(void);

#endif /* INC_WATCHDOG_MANAGER_H_ */
