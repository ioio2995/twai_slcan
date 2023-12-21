idf_component_register(SRCS "cdc_driver.cpp" "twai_slcan.cpp" "twai_driver.cpp" "slcan.cpp" "slcan_helper.cpp" "version.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver led_rgb
                    PRIV_REQUIRES esp_timer) 