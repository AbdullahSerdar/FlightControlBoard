/*
 * nmea_parser.h
 *
 *  Created on: May 9, 2026
 *      Author: serda
 */

#ifndef DEV_GPS_INC_NMEA_PARSER_H_
#define DEV_GPS_INC_NMEA_PARSER_H_

#include <stdint.h>

typedef struct
{
    double latitude;
    double longitude;
    uint8_t fix_quality;
    uint8_t satellites;
    float altitude;
} GpsParsedData_t;

GpsParsedData_t Gps_Datas(void);

#endif /* DEV_GPS_INC_NMEA_PARSER_H_ */
