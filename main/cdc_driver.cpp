#include <stdint.h>
#include "esp_log.h"
#include "sdkconfig.h"

#include "cdc_driver.hpp"
#include "slcan.hpp" 

#define APP_TAG "CDC_MODULE"


void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event)
{
    /* initialization */
    size_t rx_size = 0;
    uint8_t buf[CONFIG_TINYUSB_CDC_RX_BUFSIZE + 1];
    char cmdbuf[CONFIG_TINYUSB_CDC_RX_BUFSIZE + 1];
    int cmdidx = 0;

    /* read */
    esp_err_t ret = tinyusb_cdcacm_read((tinyusb_cdcacm_itf_t)itf, buf, CONFIG_TINYUSB_CDC_RX_BUFSIZE, &rx_size);
    if (ret == ESP_OK) {
        ESP_LOGD(APP_TAG, "Read successful: %d bytes", rx_size);

        for (int i = 0; i < rx_size; i++) {
            char val = buf[i];
            cmdbuf[cmdidx++] = val;

            if (cmdidx == sizeof(cmdbuf)) {
                ESP_LOGE(APP_TAG, "buffer overflow");
                cmdidx = 0;
                abort();
            } else if ((val == CR) || (val == BELL)) {
                cmdbuf[cmdidx] = '\0';
                ESP_LOGD(APP_TAG, "Received command: %s", cmdbuf);
                esp_log_buffer_hexdump_internal("Received command: ", cmdbuf, strlen(cmdbuf), ESP_LOG_DEBUG);
                slcan_decode_line(cmdbuf);
                esp_log_buffer_hexdump_internal("Send command: ", cmdbuf, strlen(cmdbuf), ESP_LOG_DEBUG);
                slcan_serial_write(itf, cmdbuf, strlen(cmdbuf));
                cmdidx = 0;
            }
        }
    } else {
        ESP_LOGE(APP_TAG, "Read error: %s", esp_err_to_name(ret));
    }
}

void tinyusb_cdc_write(int itf, const char* buf, size_t len)
{
    for (int i = 0; i < len; i++) {
        tinyusb_cdcacm_write_queue_char((tinyusb_cdcacm_itf_t)itf, buf[i]);
    }
    tinyusb_cdcacm_write_flush((tinyusb_cdcacm_itf_t)itf, 0);
}

void tinyusb_cdc_line_state_changed_callback(int itf, cdcacm_event_t *event)
{
    int dtr = event->line_state_changed_data.dtr;
    int rts = event->line_state_changed_data.rts;
    ESP_LOGI(APP_TAG, "Line state changed on channel %d: DTR:%d, RTS:%d", itf, dtr, rts);
}

void tinyusb_cdc_init(int itf)
{
ESP_LOGI(APP_TAG, "USB initialization");
    const tinyusb_config_t tusb_cfg = {
        .device_descriptor = NULL,
        .string_descriptor = NULL,
        .external_phy = false,
        .configuration_descriptor = NULL,
    };

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    tinyusb_config_cdcacm_t acm_cfg = {
        .usb_dev = TINYUSB_USBDEV_0,
        .cdc_port = (tinyusb_cdcacm_itf_t)itf,
        .rx_unread_buf_sz = 64,
        .callback_rx = &tinyusb_cdc_rx_callback, // the first way to register a callback
        .callback_rx_wanted_char = NULL,
        .callback_line_state_changed = NULL,
        .callback_line_coding_changed = NULL
    };

    ESP_ERROR_CHECK(tusb_cdc_acm_init(&acm_cfg));
    /* the second way to register a callback */
    ESP_ERROR_CHECK(tinyusb_cdcacm_register_callback(
                        (tinyusb_cdcacm_itf_t)itf,
                        CDC_EVENT_LINE_STATE_CHANGED,
                        &tinyusb_cdc_line_state_changed_callback));

    ESP_LOGI(APP_TAG, "USB initialization DONE");
} 
