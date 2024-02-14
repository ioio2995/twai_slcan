#ifndef ESP32_LED_RGB_CONTROL_HPP_
#define ESP32_LED_RGB_CONTROL_HPP_

#include "sdkconfig.h"

#include "driver/ledc.h"

#define LEDRGB_RED (CONFIG_APP_RGB_RED)
#define LEDRGB_GREEN  (CONFIG_APP_RGB_GREEN)
#define LEDRGB_BLUE  (CONFIG_APP_RGB_BLUE)


typedef struct
{
    uint8_t r;   // Red component of the color (0 to 255)
    uint8_t g;   // Green component of the color (0 to 255)
    uint8_t b;   // Blue component of the color (0 to 255)
    uint16_t t;  // Duration of the pulse in time units (16 bits)
    uint8_t i;   // Intensity of the color in percentage (0 to 100)
    uint8_t o;   // Offset of the color (0 to 255)
} color_t;


class LEDRGB
{
public:
    color_t rgb;
    LEDRGB(int LED_RED_PIN, int LED_GREEN_PIN, int LED_BLUE_PIN);
    void error();
    void waiting();
    void idle();
private:
    ledc_timer_config_t ledc_timer;
    ledc_channel_config_t ledc_channel[3];
    void setColor(color_t color, float in);
    static void LEDPulse(void* pvParameters);
};

#endif // ESP32_LED_RGB_CONTROL_HPP_
