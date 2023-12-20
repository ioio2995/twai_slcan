#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "freertos/FreeRTOS.h"

#include "esp_log.h"

#include "slcan.hpp"
#include "slcan_helper.hpp"
#include "twai_driver.hpp"
#include "cdc_driver.hpp"
#include "version.h"
#include "led_rgb.hpp"

#define APP_TAG "SLCAN_MODULE"

LEDRGB LRGB(LEDRGB_RED,
            LEDRGB_GREEN,
            LEDRGB_BLUE);

/** wirtes a NULL terminated ACK response */
static void slcan_ack(char *buf)
{
    *buf++ = CR; // CR
    *buf = 0;
}

/** wirtes a NULL terminated NACK response */
static void slcan_nack(char *buf)
{
    *buf++ = BELL; // BELL
    *buf = 0;
}

size_t slcan_frame_to_ascii(char *buf, const struct can_frame_s *f, bool timestamp)
{
    char *p = buf;
    uint32_t id = f->id;

    // type
    *p++ = (f->remote ? (f->extended ? 'R' : 'r') : (f->extended ? 'T' : 't'));

    // ID
    if (f->extended)
    {
        int i;
        for (i = 3; i >= 0; i--)
        {
            uint8_t b = id >> (8 * i);
            hex_write(&p, &b, 1);
        }
    }
    else
    {
        *p++ = hex_digit(id >> 8);
        *p++ = hex_digit(id >> 4);
        *p++ = hex_digit(id);
    }

    // DLC
    *p++ = hex_digit(f->length);

    // data
    if (!f->remote)
    {
        hex_write(&p, f->data, f->length);
    }

    // timestamp
    if (timestamp)
    {
        uint16_t t = f->timestamp;
        uint8_t b[2] = {(uint8_t)(t >> 8), (uint8_t)t};
        hex_write(&p, b, 2);
    }

    *p++ = '\r';
    *p = 0;

    return (size_t)(p - buf);
}

void slcan_send_frame(char *line)
{
    char *out = line;
    can_frame_s frame;
    frame.remote = false;
    frame.extended = false;
    switch (*line++)
    {
    case 'r':
        frame.remote = true;
        /* FALLTHROUGH */
    case 't':
        frame.id = hex_to_u32(line, SLC_STD_ID_LEN);
        line += SLC_STD_ID_LEN;
        break;
    case 'R':
        frame.remote = true;
        /* FALLTHROUGH */
    case 'T':
        frame.extended = true;
        frame.id = hex_to_u32(line, SLC_EXT_ID_LEN);
        line += SLC_EXT_ID_LEN;
        break;
    default:
        slcan_nack(out);
        return;
    };

    frame.length = hex_val(*line++);

    if (frame.length > 8)
    {
        slcan_nack(out);
        return;
    }

    if (!frame.remote)
    {
        hex_to_u8_array(line, frame.data, frame.length);
    }

    if (can_send(&frame))
    {
        slcan_ack(out);
    }
    else
    {
        slcan_nack(out);
        LRGB.error();
    }
}

static void set_bitrate(char *line)
{
    static const uint32_t br_tbl[10] = {10000, 20000, 50000, 100000, 125000,
                                        250000, 500000, 800000, 1000000};
    unsigned char i = line[1];
    if (i < '0' || i > '8')
    {
        slcan_nack(line);
        return;
    }
    i -= '0';
    if (can_set_bitrate(br_tbl[i]))
    {
        slcan_ack(line);
        LRGB.waiting();
    }
    else
    {
        slcan_nack(line);
        LRGB.error();
    }
}

static void slcan_open(char *line, int mode)
{
    if (can_open(mode))
    {
        slcan_ack(line);
        LRGB.idle();
    }
    else
    {
        slcan_nack(line);
        LRGB.error();
    }
}

static void slcan_close(char *line)
{
    if (can_close())
    {
        slcan_ack(line);
        LRGB.idle();
    }
    else
    {
        slcan_nack(line);
        LRGB.waiting();
    }
}

int slcan_serial_write(int itf, const char *buf, size_t len)
{
    if (len == 0)
    {
        return 0;
    }

    int ret = tinyusb_cdcacm_write_queue((tinyusb_cdcacm_itf_t)itf, (const uint8_t *)buf, len);
    tinyusb_cdcacm_write_flush((tinyusb_cdcacm_itf_t)itf, 0);
    return ret;
}

static void slcan_status(char *line)
{

    if (twai_running != 0)
    {
        can_statusFlags status_flags;
        if (can_state(&status_flags))
        {
            snprintf(line, strlen(line) + 1, "F%02X", *((uint8_t *)&status_flags));
            slcan_ack(line);
            return;
        }
    }
    else
    {
        slcan_nack(line);
    }
}

/*
reference:
http://www.fischl.de/usbtin/
http://www.can232.com/docs/canusb_manual.pdf
*/
void slcan_decode_line(char *line)
{
    switch (*line)
    {
    case 'T': // extended frame
    case 't': // standard frame
    case 'R': // extended remote frame
    case 'r': // standard remote frame
        slcan_send_frame(line);
        break;
    case 'S': // set baud rate, S0-S9
        set_bitrate(line);
        break;
    case 'O': // open CAN channel
        slcan_open(line, CAN_MODE_NORMAL);
        break;
    case 'l': // open in loop back mode
        slcan_open(line, CAN_MODE_LOOPBACK);
        break;
    case 'L': // open in silent mode (listen only)
        slcan_open(line, CAN_MODE_SILENT);
        break;
    case 'C': // close CAN channel
        slcan_close(line);
        break;
    case 'V': // hardware version
        line = stpcpy(line, hardware_version_str);
        slcan_ack(line);
        break;
    case 'v': // firmware version
        line = stpcpy(line, software_version_str);
        slcan_ack(line);
        break;
    case 'F': // read & clear status/error flags
        slcan_status(line);
        break;
    case '\0': // Empty line, requires an ACK to be sent back
        slcan_ack(line);
        break;
    // 'N': // serial number
    // 'F': // read status byte
    // 'Z': // timestamp on/off, Zx[CR]
    // 'm': // acceptance mask, mxxxxxxxx[CR]
    // 'M': // acceptance code, Mxxxxxxxx[CR]
    default:
        slcan_nack(line);
        LRGB.error();
        break;
    };
}

void slcan_rx_spin(void *arg)
{
    while (1)
    {
        if (twai_running > 1)
        {
            int *_arg = (int *)arg;
            int ift = *_arg;
            struct can_frame_s rxf;

            if ( can_receive(&rxf))
            {
                static char txbuf[MAX_FRAME_LEN];
                size_t len = slcan_frame_to_ascii(txbuf, &rxf, false);
                slcan_serial_write(ift, txbuf, len);
            }
        }
        else
        {
            LRGB.waiting();
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }
}