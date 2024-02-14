#ifndef TWAI_DRIVER_H
#define TWAI_DRIVER_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define TWAI_TX static_cast<gpio_num_t>(CONFIG_APP_TWAI_TX)
#define TWAI_RX static_cast<gpio_num_t>(CONFIG_APP_TWAI_RX)
#define TWAI_TX_TIMEOUT (CONFIG_APP_TWAI_TX_TIMEOUT)
#define TWAI_RX_TIMEOUT (CONFIG_APP_TWAI_RX_TIMEOUT)
#define TWAI_VIO static_cast<gpio_num_t>(42)



extern int twai_running;

struct can_frame_s {
    uint32_t timestamp;
    uint32_t id : 29;
    uint32_t extended : 1;
    uint32_t remote : 1;
    uint8_t length;
    uint8_t data[8];
};

struct can_statusFlags{
    uint8_t fifo_queue_full_rx : 1;
    uint8_t fifo_queue_full_tx : 1;
    uint8_t error_warning : 1;
    uint8_t data_overrun : 1;
    uint8_t not_used : 1;
    uint8_t error_passive : 1;
    uint8_t arbitration_lost : 1;
    uint8_t bus_error : 1;
};

enum {
    CAN_MODE_NORMAL,
    CAN_MODE_LOOPBACK,
    CAN_MODE_SILENT
};

/* non-blocking CAN frame receive, NULL if nothing received */
bool can_receive(struct can_frame_s * frame);


/* blocking CAN frame send */
bool can_send(struct can_frame_s * frame);

/* returns true on success, must be called before can_open */
bool can_set_bitrate(uint32_t bitrate);

/* returns true on success */
bool can_open(int mode);
bool can_close(void);
void can_init(void);

uint32_t can_errorRX();
uint32_t can_errorTX();
bool can_state(struct can_statusFlags * states);


#endif // TWAI_DRIVER_H 