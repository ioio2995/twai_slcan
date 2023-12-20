idf_component_register(SRCS "TWAI_slcan.cpp" "slcan.cpp" "slcan_helper.cpp" "twai_driver.cpp" "cdc_driver.cpp" "version.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver led_rgb
                    PRIV_REQUIRES esp_timer) 