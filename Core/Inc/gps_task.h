/*
 * gps_task.h
 *
 *  Created on: Apr 22, 2026
 *      Author: serda
 */

#ifndef INC_GPS_TASK_H_
#define INC_GPS_TASK_H_

typedef struct
{
    double latitude;
    double longitude;
    uint8_t fix_quality;
    uint8_t satellites;
    float altitude;
} GpsParsedData_t;


void Gps_RxCallback(UART_HandleTypeDef* huart, uint16_t Size);
void StartGpsTask(void const * argument);
void Gps_TakeGPGGA(uint8_t *datas, uint16_t size);

#endif /* INC_GPS_TASK_H_ */
