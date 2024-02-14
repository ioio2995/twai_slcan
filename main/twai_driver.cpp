#include <cstring>

#include "esp_log.h"
#include "driver/twai.h"

#include "twai_driver.hpp"

#define APP_TAG "TWAI_MODULE"

/** \var twai_running
 *  \brief
 *       Variable to track the TWAI module state (0: closed, 1: closing, 2: open)
 */
int  twai_running = 0;

/** \var t_config
 *  \brief
 *       Configuration for CAN Speed
 */
static twai_timing_config_t t_config;  // CAN Speed Config

/** \fn can_set_bitrate
 *  \brief
 *       Sets the bitrate for the CAN bus
 * 
 *  \param bitrate
 *       Desired bitrate value
 *  \return
 *       True if successful, false if the bitrate is not supported
 */
bool can_set_bitrate(uint32_t bitrate)
{
    switch (bitrate)
    {
    case 10000:
        t_config = TWAI_TIMING_CONFIG_10KBITS();
        break;
    case 20000:
        t_config = TWAI_TIMING_CONFIG_20KBITS();
        break;
    case 50000:
        t_config = TWAI_TIMING_CONFIG_50KBITS();
        break;
    case 100000:
        t_config = TWAI_TIMING_CONFIG_100KBITS();
        break;
    case 125000:
        t_config = TWAI_TIMING_CONFIG_125KBITS();
        break;
    case 250000:
        t_config = TWAI_TIMING_CONFIG_250KBITS();
        break;
    case 500000:
        t_config = TWAI_TIMING_CONFIG_500KBITS();
        break;
    case 800000:
        t_config = TWAI_TIMING_CONFIG_800KBITS();
        break;
    case 1000000:
        t_config = TWAI_TIMING_CONFIG_1MBITS();
        break;
    default:
        return false;
    }

    return true;
};

/** \fn can_init
 *  \brief
 *       Initializes the TWAI module
 */
void can_init(void){
        gpio_set_direction(TWAI_VIO, GPIO_MODE_OUTPUT);
        gpio_set_level(TWAI_VIO, 1);
}

/** \fn can_open
 *  \brief
 *       Opens the TWAI channel with the specified mode
 * 
 *  \param mode
 *       CAN mode (CAN_MODE_NORMAL or CAN_MODE_SILENT)
 *  \return
 *       True if successful, false otherwise
 */
bool can_open(int mode)
{
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    twai_general_config_t g_config;
    switch (mode)
    {
    case CAN_MODE_NORMAL:
        g_config = TWAI_GENERAL_CONFIG_DEFAULT(TWAI_TX, TWAI_RX, TWAI_MODE_NORMAL);
        break;
    case CAN_MODE_SILENT:
        g_config = TWAI_GENERAL_CONFIG_DEFAULT(TWAI_TX, TWAI_RX, TWAI_MODE_LISTEN_ONLY);
        break;
    default:
        return false;
    }

    if (twai_running == 0)
    {
        ESP_LOGI(APP_TAG, "Initializing...");
        if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK)
        {
            ESP_LOGI(APP_TAG, "Starting...");
            if (twai_start() != ESP_OK)
            {
                ESP_LOGE(APP_TAG, "twai_start() failed");
                return false;
            }
            else
            {
                twai_reconfigure_alerts(TWAI_ALERT_ALL, NULL);
                twai_running = 2;
                ESP_LOGI(APP_TAG, "Channel open");
                return true;
            }
        }
        else
        {
            ESP_LOGE(APP_TAG, "twai_driver_install() failed");
            return false;
        }
    }
    ESP_LOGE(APP_TAG, "Channel is not close");
    return false;
};

/** \fn can_close
 *  \brief
 *       Closes the TWAI channel
 * 
 *  \return
 *       True if successful, false otherwise
 */
bool can_close(void)
{
    if (twai_running != 0)
    {
        twai_running = 1;
        vTaskDelay(100 / portTICK_PERIOD_MS);
        ESP_LOGI(APP_TAG, "Stopping...");
        if (twai_stop() == ESP_OK)
        {
            ESP_LOGI(APP_TAG, "Uninstall the TWAI driver");
            if (twai_driver_uninstall() != ESP_OK)
            {
                ESP_LOGE(APP_TAG, "twai_stop() failed");
                return false;
            }
            else
            {
                twai_running = 0;
                ESP_LOGI(APP_TAG, "Channel close");
                return true;
            }
        }
        else
        {
            ESP_LOGE(APP_TAG, "twai_driver_uninstall() failed");
            return false;
        }
    }
    ESP_LOGE(APP_TAG, "Channel is not open");
    return false;
};

/** \fn can_getStatus
 *  \brief
 *       Retrieves the status information of the TWAI module
 * 
 *  \return
 *       Status information structure
 */
twai_status_info_t can_getStatus()
{
    twai_status_info_t status;
    ESP_ERROR_CHECK(twai_get_status_info(&status));
    return status;
}

/** \fn can_errorRX
 *  \brief
 *       Retrieves the number of receive errors on the CAN bus
 * 
 *  \return
 *       Number of receive errors
 */
uint32_t can_errorRX()
{
    twai_status_info_t status = can_getStatus();
    return status.rx_error_counter;
}

/** \fn can_errorTX
 *  \brief
 *       Retrieves the number of transmit errors on the CAN bus
 * 
 *  \return
 *       Number of transmit errors
 */
uint32_t can_errorTX()
{
    twai_status_info_t status = can_getStatus();
    return status.tx_error_counter;
}

/** \fn can_state
 *  \brief
 *       Retrieves the state flags of the TWAI module
 * 
 *  \param states
 *       Pointer to the structure to store state flags
 *  \return
 *       True if successful, false otherwise
 */
bool can_state(struct can_statusFlags * states)
{
    uint32_t alerts;
    if (twai_read_alerts(&alerts, pdMS_TO_TICKS(100)) == ESP_OK)
   {
    states->fifo_queue_full_rx = (alerts & TWAI_ALERT_RX_QUEUE_FULL) != 0;
    states->fifo_queue_full_tx = (alerts & TWAI_ALERT_TX_FAILED) != 0;
    states->error_warning = (alerts & (TWAI_ALERT_ABOVE_ERR_WARN | TWAI_ALERT_BELOW_ERR_WARN)) != 0;
    states->data_overrun = (alerts & TWAI_ALERT_RX_FIFO_OVERRUN) != 0;
    states->error_passive = (alerts & TWAI_ALERT_ERR_PASS) != 0;
    states->arbitration_lost = (alerts & TWAI_ALERT_ARB_LOST) != 0;
    states->bus_error = (alerts & TWAI_ALERT_BUS_ERROR) != 0;
    return true;
   }
    return false;
}

/** \fn can_receive
 *  \brief
 *       Receives a CAN frame from the TWAI module
 * 
 *  \param frame
 *       Pointer to the structure to store the received CAN frame
 *  \return
 *       True if successful, false otherwise
 */
bool can_receive(struct can_frame_s * frame)
{
   //ToDo Timestamp 
    twai_message_t rx_frame;
    esp_err_t ret = twai_receive(&rx_frame, pdMS_TO_TICKS(100));
    if (ret == ESP_OK) {
        frame->extended = rx_frame.extd;
        frame->remote = rx_frame.rtr;
        frame->length = rx_frame.data_length_code;
        frame->id = rx_frame.identifier;
        memcpy(frame->data, rx_frame.data, sizeof(frame->data));
        return true;
    }
    return false;
};

/** \fn can_send
 *  \brief
 *       Transmits a CAN frame using the TWAI module
 * 
 *  \param frame
 *       Pointer to the CAN frame to be transmitted
 *  \return
 *       True if successful, false otherwise
 */
bool can_send(struct can_frame_s * frame)
{
    twai_message_t tx_frame;
    tx_frame.identifier = frame->id;
    tx_frame.extd = frame->extended ? true : false;
    tx_frame.rtr = frame->remote ? true : false;
    tx_frame.data_length_code = frame->length;
    memcpy(tx_frame.data, frame->data, frame->length);

    esp_err_t ret = twai_transmit(&tx_frame, pdMS_TO_TICKS(100));
    if (ret != ESP_OK) 
      {
        ESP_LOGE(APP_TAG,"twai_transmit() failed");
        return false;
      }
    return true;
}
