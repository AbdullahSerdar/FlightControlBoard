/*
 * nmea_parser.c
 * NMEA middleware parser. Currently parses GGA sentences.
 */

#include "nmea_parser.h"
#include <stdlib.h>
#include <string.h>

#define NMEA_FIELD_MAX_LEN 24U

static GpsParsedData_t g_parsed = {0};

static uint8_t Nmea_HexToNibble(char c)
{
    if ((c >= '0') && (c <= '9')) { return (uint8_t)(c - '0'); }
    if ((c >= 'A') && (c <= 'F')) { return (uint8_t)(c - 'A' + 10); }
    if ((c >= 'a') && (c <= 'f')) { return (uint8_t)(c - 'a' + 10); }
    return 0xFFU;
}

static uint8_t Nmea_ChecksumIsValid(const char *line, uint16_t length)
{
    uint8_t checksum = 0U;
    uint16_t star_idx = 0U;
    uint8_t expected;
    uint8_t high;
    uint8_t low;

    if ((line == NULL) || (length < 5U) || (line[0] != '$'))
    {
        return 0U;
    }

    for (uint16_t i = 1U; i < length; i++)
    {
        if (line[i] == '*')
        {
            star_idx = i;
            break;
        }

        checksum ^= (uint8_t)line[i];
    }

    if ((star_idx == 0U) || ((star_idx + 2U) >= length))
    {
        return 0U;
    }

    high = Nmea_HexToNibble(line[star_idx + 1U]);
    low  = Nmea_HexToNibble(line[star_idx + 2U]);

    if ((high == 0xFFU) || (low == 0xFFU))
    {
        return 0U;
    }

    expected = (uint8_t)((high << 4U) | low);
    return (checksum == expected) ? 1U : 0U;
}

static double Nmea_ToDegrees(const char *str)
{
    double raw;
    int degrees;
    double minutes;

    if ((str == NULL) || (str[0] == '\0'))
    {
        return 0.0;
    }

    raw = atof(str);
    degrees = (int)(raw / 100.0);
    minutes = raw - ((double)degrees * 100.0);

    return (double)degrees + (minutes / 60.0);
}

static uint8_t Nmea_GetField(const char *src, uint16_t src_len, uint8_t field_idx,
                             char *dst, uint8_t dst_size)
{
    uint8_t field = 0U;
    uint8_t d = 0U;

    if ((src == NULL) || (dst == NULL) || (dst_size == 0U))
    {
        return 0U;
    }

    dst[0] = '\0';

    for (uint16_t i = 0U; i < src_len; i++)
    {
        char c = src[i];

        if ((c == '$') && (i == 0U))
        {
            continue;
        }

        if ((c == ',') || (c == '*') || (c == '\r') || (c == '\n') || (c == '\0'))
        {
            if (field == field_idx)
            {
                dst[d] = '\0';
                return 1U;
            }

            field++;
            d = 0U;

            if ((c == '*') || (c == '\r') || (c == '\n') || (c == '\0'))
            {
                break;
            }

            continue;
        }

        if (field == field_idx)
        {
            if (d < (uint8_t)(dst_size - 1U))
            {
                dst[d++] = c;
            }
        }
    }

    return 0U;
}

static uint8_t Nmea_IsGgaSentence(const char *line)
{
    if (line == NULL)
    {
        return 0U;
    }

    /* Supports $GPGGA, $GNGGA, $BDGGA etc. Check sentence formatter positions. */
    return ((line[0] == '$') &&
            (line[3] == 'G') &&
            (line[4] == 'G') &&
            (line[5] == 'A') &&
            (line[6] == ',')) ? 1U : 0U;
}

static NMEA_ErrorCode_t Nmea_ParseGgaLine(const char *line, uint16_t length)
{
    char field[NMEA_FIELD_MAX_LEN];
    char lat_dir[4] = {0};
    char lon_dir[4] = {0};
    GpsParsedData_t parsed = g_parsed;

    if (!Nmea_IsGgaSentence(line))
    {
        return E_NMEA_ERR_UNSUPPORTED_SENTENCE;
    }

    if (Nmea_GetField(line, length, 6U, field, sizeof(field)))
    {
        parsed.fix_quality = (uint8_t)atoi(field);
        parsed.is_valid = (parsed.fix_quality > 0U) ? 1U : 0U;
    }

    if (Nmea_GetField(line, length, 2U, field, sizeof(field)) &&
        Nmea_GetField(line, length, 3U, lat_dir, sizeof(lat_dir)))
    {
        if (field[0] != '\0')
        {
            parsed.latitude = Nmea_ToDegrees(field);
            if (lat_dir[0] == 'S') { parsed.latitude = -parsed.latitude; }
        }
    }

    if (Nmea_GetField(line, length, 4U, field, sizeof(field)) &&
        Nmea_GetField(line, length, 5U, lon_dir, sizeof(lon_dir)))
    {
        if (field[0] != '\0')
        {
            parsed.longitude = Nmea_ToDegrees(field);
            if (lon_dir[0] == 'W') { parsed.longitude = -parsed.longitude; }
        }
    }

    if (Nmea_GetField(line, length, 7U, field, sizeof(field)))
    {
        parsed.satellites = (uint8_t)atoi(field);
    }

    if (Nmea_GetField(line, length, 9U, field, sizeof(field)))
    {
        parsed.altitude = (float)atof(field);
    }

    g_parsed = parsed;
    return E_NMEA_ERR_NONE;
}

void Nmea_Reset(void)
{
    memset(&g_parsed, 0, sizeof(g_parsed));
}

NMEA_ErrorCode_t Nmea_ParseLine(const char *line, uint16_t length)
{
    if ((line == NULL) || (length == 0U))
    {
        return E_NMEA_ERR_INVALID_PARAM;
    }

    if (!Nmea_IsGgaSentence(line))
    {
        return E_NMEA_ERR_UNSUPPORTED_SENTENCE;
    }

    if (Nmea_ChecksumIsValid(line, length) == 0U)
    {
        return E_NMEA_ERR_CHECKSUM;
    }

    return Nmea_ParseGgaLine(line, length);
}

GpsParsedData_t Nmea_GetData(void)
{
    return g_parsed;
}

