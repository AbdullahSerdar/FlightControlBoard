/*
 * nmea_parser.h
 * GPS middleware parser. Driver gives raw NMEA lines; this layer parses them.
 */

#ifndef MIDDLEWARE_NMEA_PARSER_H_
#define MIDDLEWARE_NMEA_PARSER_H_

#include <stdint.h>

typedef enum
{
    E_NMEA_ERR_NONE = 0,
    E_NMEA_ERR_INVALID_PARAM,
    E_NMEA_ERR_UNSUPPORTED_SENTENCE,
    E_NMEA_ERR_CHECKSUM,
    E_NMEA_ERR_PARSE
} NMEA_ErrorCode_t;

typedef struct
{
    double latitude;
    double longitude;
    uint8_t fix_quality;
    uint8_t satellites;
    float altitude;
    uint8_t is_valid;
} GpsParsedData_t;

void Nmea_Reset(void);
NMEA_ErrorCode_t Nmea_ParseLine(const char *line, uint16_t length);
GpsParsedData_t Nmea_GetData(void);

#endif /* MIDDLEWARE_NMEA_PARSER_H_ */
