#include "gps_driver.h"
#include <string.h>

#define GPS_UART_TIMEOUT_MS 100U

typedef struct
{
    GPS_State_t state;
    UART_HandleTypeDef *huart;
    volatile uint16_t ring_head;
    volatile uint16_t ring_tail;
    volatile uint32_t overflow_count;
} GPS_Context_t;

static GPS_Context_t g_gps =
{
    .state = GPS_STATE_CLOSED,
    .huart = NULL,
    .ring_head = 0U,
    .ring_tail = 0U,
    .overflow_count = 0U
};

static uint8_t gps_rx_buf[GPS_RX_BUFFER_SIZE];
static uint8_t gps_ring_buf[GPS_RING_BUFFER_SIZE];

static GPSErrorCodes_t GPS_StartRx(void);
static GPSErrorCodes_t GPS_StopRx(void);
static void GPS_RingReset(void);
static void GPS_RingWriteByte(uint8_t byte);
static uint8_t GPS_RingReadByte(uint8_t *byte);
static uint8_t GPS_InternalReadLine(char *line, uint16_t max_len, uint16_t *out_len);

static GPSErrorCodes_t GPS_StartRx(void)
{
    if ((g_gps.huart == NULL) || (g_gps.huart->hdmarx == NULL))
    {
        return E_GPS_ERR_INVALID_PARAM;
    }

    if (HAL_UARTEx_ReceiveToIdle_DMA(g_gps.huart,
                                     gps_rx_buf,
                                     sizeof(gps_rx_buf)) != HAL_OK)
    {
        return E_GPS_ERR_UART;
    }

    /*
     * Half-transfer interrupt is not needed for ReceiveToIdle.
     * It can create unnecessary callbacks with half packets.
     */
    __HAL_DMA_DISABLE_IT(g_gps.huart->hdmarx, DMA_IT_HT);

    return E_GPS_ERR_NONE;
}

static GPSErrorCodes_t GPS_StopRx(void)
{
    if (g_gps.huart == NULL)
    {
        return E_GPS_ERR_INVALID_PARAM;
    }

    if (HAL_UART_DMAStop(g_gps.huart) != HAL_OK)
    {
        return E_GPS_ERR_UART;
    }

    return E_GPS_ERR_NONE;
}

static void GPS_RingReset(void)
{
    memset(gps_rx_buf, 0, sizeof(gps_rx_buf));
    memset(gps_ring_buf, 0, sizeof(gps_ring_buf));

    g_gps.ring_head = 0U;
    g_gps.ring_tail = 0U;
    g_gps.overflow_count = 0U;
}

static void GPS_RingWriteByte(uint8_t byte)
{
    uint16_t next_head;

    next_head = (uint16_t)((g_gps.ring_head + 1U) % GPS_RING_BUFFER_SIZE);

    if (next_head != g_gps.ring_tail)
    {
        gps_ring_buf[g_gps.ring_head] = byte;
        g_gps.ring_head = next_head;
    }
    else
    {
        g_gps.overflow_count++;
    }
}

static uint8_t GPS_RingReadByte(uint8_t *byte)
{
    if (byte == NULL)
    {
        return 0U;
    }

    if (g_gps.ring_tail == g_gps.ring_head)
    {
        return 0U;
    }

    *byte = gps_ring_buf[g_gps.ring_tail];
    g_gps.ring_tail = (uint16_t)((g_gps.ring_tail + 1U) % GPS_RING_BUFFER_SIZE);

    return 1U;
}

static uint8_t GPS_InternalReadLine(char *line, uint16_t max_len, uint16_t *out_len)
{
    static uint16_t index = 0U;
    uint8_t byte;

    if ((line == NULL) || (max_len < 2U))
    {
        return 0U;
    }

    while (GPS_RingReadByte(&byte) != 0U)
    {
        /*
         * NMEA sentence starts with '$'. Drop everything before '$'.
         */
        if (byte == (uint8_t)'$')
        {
            index = 0U;
            line[index++] = (char)byte;
            continue;
        }

        /*
         * If a sentence has not started yet, ignore incoming bytes.
         */
        if (index == 0U)
        {
            continue;
        }

        /*
         * Prevent line buffer overflow.
         */
        if (index >= (uint16_t)(max_len - 1U))
        {
            index = 0U;
            g_gps.overflow_count++;
            continue;
        }

        line[index++] = (char)byte;

        /*
         * NMEA normally ends with CRLF. Some paths can deliver only CR or LF.
         * Accept both '\r' and '\n' as a line terminator.
         */
        if ((byte == (uint8_t)'\n') || (byte == (uint8_t)'\r'))
        {
            line[index] = '\0';

            if (out_len != NULL)
            {
                *out_len = index;
            }

            index = 0U;
            return 1U;
        }
    }

    return 0U;
}

void Gps_RxCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if ((g_gps.state != GPS_STATE_OPEN) || (huart != g_gps.huart))
    {
        return;
    }

    if (Size == 0U)
    {
        (void)GPS_StartRx();
        return;
    }

    if (Size > GPS_RX_BUFFER_SIZE)
    {
        Size = GPS_RX_BUFFER_SIZE;
    }

    for (uint16_t i = 0U; i < Size; i++)
    {
        GPS_RingWriteByte(gps_rx_buf[i]);
    }

    (void)GPS_StartRx();
}

void Gps_RxErrorCallback(UART_HandleTypeDef *huart)
{
    if ((g_gps.state != GPS_STATE_OPEN) || (huart != g_gps.huart))
    {
        return;
    }

    (void)HAL_UART_DMAStop(g_gps.huart);

    __HAL_UART_CLEAR_OREFLAG(g_gps.huart);
    __HAL_UART_CLEAR_FEFLAG(g_gps.huart);
    __HAL_UART_CLEAR_NEFLAG(g_gps.huart);

    (void)GPS_StartRx();
}

GPSErrorCodes_t Gps_Open(void *vpParam)
{
    GPS_OpenConfig_t default_config;
    GPS_OpenConfig_t *open_config;
    GPSErrorCodes_t err;

    if (g_gps.state == GPS_STATE_OPEN)
    {
        return E_GPS_ERR_NONE;
    }

    if (vpParam == NULL)
    {
        default_config.huart = GPS_DEVICE_UART;
        open_config = &default_config;
    }
    else
    {
        open_config = (GPS_OpenConfig_t *)vpParam;
    }

    if (open_config->huart == NULL)
    {
        return E_GPS_ERR_INVALID_PARAM;
    }

    g_gps.huart = open_config->huart;
    GPS_RingReset();

    /*
     * Set state before starting DMA so that an early RxEvent callback is not
     * discarded while DMA is already receiving data.
     */
    g_gps.state = GPS_STATE_OPEN;

    err = GPS_StartRx();
    if (err != E_GPS_ERR_NONE)
    {
        g_gps.huart = NULL;
        g_gps.state = GPS_STATE_CLOSED;
        GPS_RingReset();
        return err;
    }

    return E_GPS_ERR_NONE;
}

GPSErrorCodes_t Gps_Read(void *pvBuffer, uint32_t xBytes)
{
    GPS_NmeaLine_t *out_line;

    if (g_gps.state != GPS_STATE_OPEN)
    {
        return E_GPS_ERR_NOT_OPEN;
    }

    if ((pvBuffer == NULL) || (xBytes < sizeof(GPS_NmeaLine_t)))
    {
        return E_GPS_ERR_INVALID_PARAM;
    }

    out_line = (GPS_NmeaLine_t *)pvBuffer;
    memset(out_line, 0, sizeof(GPS_NmeaLine_t));

    if (GPS_InternalReadLine(out_line->line,
                             sizeof(out_line->line),
                             &out_line->length) == 0U)
    {
        return E_GPS_ERR_NO_DATA;
    }

    return E_GPS_ERR_NONE;
}

GPSErrorCodes_t Gps_Write(const void *pvBuffer, uint32_t xBytes)
{
    if (g_gps.state != GPS_STATE_OPEN)
    {
        return E_GPS_ERR_NOT_OPEN;
    }

    if ((pvBuffer == NULL) || (xBytes == 0U) || (xBytes > 0xFFFFU))
    {
        return E_GPS_ERR_INVALID_PARAM;
    }

    if (HAL_UART_Transmit(g_gps.huart,
                          (uint8_t *)pvBuffer,
                          (uint16_t)xBytes,
                          GPS_UART_TIMEOUT_MS) != HAL_OK)
    {
        return E_GPS_ERR_UART;
    }

    return E_GPS_ERR_NONE;
}

GPSErrorCodes_t Gps_Ioctl(GPS_IoctlCommand_t command, void *vpParam)
{
    switch (command)
    {
    case E_GPS_IOCTL_GET_STATE:
        if (vpParam == NULL)
        {
            return E_GPS_ERR_INVALID_PARAM;
        }

        *(GPS_State_t *)vpParam = g_gps.state;
        return E_GPS_ERR_NONE;

    case E_GPS_IOCTL_GET_OVERFLOW_COUNT:
        if (vpParam == NULL)
        {
            return E_GPS_ERR_INVALID_PARAM;
        }

        *(uint32_t *)vpParam = g_gps.overflow_count;
        return E_GPS_ERR_NONE;

    case E_GPS_IOCTL_RESET_BUFFER:
        GPS_RingReset();
        return E_GPS_ERR_NONE;

    case E_GPS_IOCTL_START_RX:
        if (g_gps.state != GPS_STATE_OPEN)
        {
            return E_GPS_ERR_NOT_OPEN;
        }

        return GPS_StartRx();

    case E_GPS_IOCTL_STOP_RX:
        if (g_gps.state != GPS_STATE_OPEN)
        {
            return E_GPS_ERR_NOT_OPEN;
        }

        return GPS_StopRx();

    default:
        return E_GPS_ERR_WRONG_IOCTL_CMD;
    }
}

GPSErrorCodes_t Gps_Close(void *vpParam)
{
    GPSErrorCodes_t err;

    (void)vpParam;

    if (g_gps.state != GPS_STATE_OPEN)
    {
        return E_GPS_ERR_NOT_OPEN;
    }

    err = GPS_StopRx();
    if (err != E_GPS_ERR_NONE)
    {
        return err;
    }

    GPS_RingReset();
    g_gps.huart = NULL;
    g_gps.state = GPS_STATE_CLOSED;

    return E_GPS_ERR_NONE;
}
