#include <stdio.h>
#include "twai_driver.hpp"
#include "cdc_driver.hpp"
#include "slcan.hpp"

/** \var itf
 *  \brief
 *       Variable representing the interface (itf_default is assumed to be defined elsewhere)
 */
extern "C" int itf = itf_default;

/** \fn app_main
 *  \brief
 *       Main function of the application
 */
extern "C" void app_main(void)
{
    can_init();
    tinyusb_cdc_init(itf);
    xTaskCreatePinnedToCore(slcan_rx_spin, "transfer_can2uart", 4096, &itf, 8, NULL, 0);
}