#ifndef CDC_DRIVER_H
#define CDC_DRIVER_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "tinyusb.h"
#include "tusb_cdc_acm.h"

#define itf_default TINYUSB_CDC_ACM_0
#define usb_default TINYUSB_CDC_ACM_0

void tinyusb_cdc_init(int itf);

#endif // CDC_DRIVER_H 