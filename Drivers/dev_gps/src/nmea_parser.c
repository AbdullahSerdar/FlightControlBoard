/*
 * nmea_parser.c
 *
 *  Created on: May 9, 2026
 *      Author: serda
 */

#include <stdlib.h>
#include "nmea_parser.h"


GpsParsedData_t parsed = {0};

static double Nmea_ToDegrees(const char *str)
{
    double raw = atof(str);
    int    degrees = (int)(raw / 100);
    double minutes = raw - (degrees * 100.0);
    return degrees + (minutes / 60.0);
}

static uint8_t Nmea_GetField(const char *src, uint16_t src_len, uint8_t field_idx, char *dst, uint8_t dst_size)
{
    uint8_t  field = 0;
    uint16_t d     = 0;

    for (uint16_t i = 0; i < src_len; i++)
    {
        if (src[i] == ',' || src[i] == '*')
        {
            if (field == field_idx)
            {
                dst[d] = '\0';
                return 1;
            }
            field++;
            d = 0;
            if (src[i] == '*') break;
            continue;
        }

        if (field == field_idx && d < dst_size - 1)
            dst[d++] = src[i];
    }

    return 0;
}

void Gps_TakeGPGGA(uint8_t *raw_gps_data, uint16_t size)
{
    uint16_t i    = 0;

    if (raw_gps_data == NULL || size == 0)
        return;

    while (i + 7 < size)
    {
        if (raw_gps_data[i]   == '$' &&
            raw_gps_data[i+1] == 'G' &&
            raw_gps_data[i+2] == 'P' &&
            raw_gps_data[i+3] == 'G' &&
            raw_gps_data[i+4] == 'G' &&
            raw_gps_data[i+5] == 'A' &&
            raw_gps_data[i+6] == ',')
        {
            uint16_t start = i;
            uint16_t end   = i;

            while (end < size && raw_gps_data[end] != '*')
                end++;

            if (end + 5 <= size)
            {
                end += 3;
                if (raw_gps_data[end] == '\r' && raw_gps_data[end + 1] == '\n')
                    end += 2;

                uint16_t len = end - start;
                const char *sentence = (const char *)&raw_gps_data[start];

                char field[20];

                if (Nmea_GetField(sentence, len, 6, field, sizeof(field)))
                    parsed.fix_quality = (uint8_t)atoi(field);

                char lat_dir[4] = {0};
                if (Nmea_GetField(sentence, len, 2, field, sizeof(field)) &&
                    Nmea_GetField(sentence, len, 3, lat_dir, sizeof(lat_dir)))
                {
                    parsed.latitude = Nmea_ToDegrees(field);
                    if (lat_dir[0] == 'S') parsed.latitude = -parsed.latitude;
                }

                char lon_dir[4] = {0};
                if (Nmea_GetField(sentence, len, 4, field, sizeof(field)) &&
                    Nmea_GetField(sentence, len, 5, lon_dir, sizeof(lon_dir)))
                {
                    parsed.longitude = Nmea_ToDegrees(field);
                    if (lon_dir[0] == 'W') parsed.longitude = -parsed.longitude;
                }

                if (Nmea_GetField(sentence, len, 9, field, sizeof(field)))
                    parsed.altitude = atof(field);

                if (Nmea_GetField(sentence, len, 7, field, sizeof(field)))
                    parsed.satellites = (uint8_t)atoi(field);

            }
            return;
        }
        i++;
    }
}


GpsParsedData_t Gps_Datas(void){
	return parsed;
}
















