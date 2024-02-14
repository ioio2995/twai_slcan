#include <math.h>
#include "freertos/FreeRTOS.h"

#include "led_rgb.hpp"
#define LED_TASK_PRIO 8

/**
 * @brief Constructor for the LEDRGB class.
 *
 * Initializes timers and LEDC channels to control an RGB LED.
 *
 * @param LED_RED_PIN   GPIO pin for the red LED.
 * @param LED_GREEN_PIN GPIO pin for the green LED.
 * @param LED_BLUE_PIN  GPIO pin for the blue LED.
 */
LEDRGB::LEDRGB(int LED_RED_PIN,
               int LED_GREEN_PIN,
               int LED_BLUE_PIN)
{
    ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 1000,
        .clk_cfg = LEDC_AUTO_CLK,
        .deconfigure = false};

    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel[0].channel = LEDC_CHANNEL_0;
    ledc_channel[0].gpio_num = LED_RED_PIN;

    ledc_channel[1].channel = LEDC_CHANNEL_1;
    ledc_channel[1].gpio_num = LED_GREEN_PIN;

    ledc_channel[2].channel = LEDC_CHANNEL_2;
    ledc_channel[2].gpio_num = LED_BLUE_PIN;

    for (int i = 0; i < 3; i++)
    {
        ledc_channel[i].speed_mode = LEDC_LOW_SPEED_MODE;
        ledc_channel[i].timer_sel = LEDC_TIMER_0;
        ledc_channel[i].intr_type = LEDC_INTR_DISABLE;
        ledc_channel[i].duty = 0;
        ledc_channel[i].hpoint = 0;

        ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel[i]));
    }
    rgb.t = 40;
    rgb.i = 15;
    rgb.o = 55;
    xTaskCreate(LEDPulse, "LED_task",   configMINIMAL_STACK_SIZE + 1024, this, LED_TASK_PRIO + 1, NULL);
}

/**
 * @brief Sets the color of the LED based on the specified color parameters.
 *
 * Calculates pulse parameters based on color and input value.
 *
 * @param color Color specified for the LED.
 * @param in    Input value for LED pulsation.
 */
void LEDRGB::setColor(color_t color, float in)
{
    float out;
    // Calculate pulse parameters based on color and in value
    float minOut = color.o;
    float amplitude = 100 - color.o;
    float intensity = color.i / 100.0;
    out = cos(in) * amplitude + minOut;
    uint8_t red = color.r * out / 100 * intensity;
    uint8_t green = color.g * out / 100 * intensity;
    uint8_t blue = color.b * out / 100 * intensity;

     // Convert color values to LED duties (duty cycle)
    uint32_t red_duty = 8191 * red / 255;
    uint32_t green_duty = 8191 * green / 255;
    uint32_t blue_duty = 8191 * blue / 255;

    // Update LED channel duties
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, red_duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, green_duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1));

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, blue_duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2));
}

/**
 * @brief FreeRTOS task for LED pulsation.
 *
 * Performs LED pulsation using the passed instance.
 *
 * @param pvParameters Task parameters (LEDRGB instance).
 */
void LEDRGB::LEDPulse(void *pvParameters)
{
    // Convert the generic pointer to a pointer of type LEDRGB
    LEDRGB *led = static_cast<LEDRGB *>(pvParameters);
    float in;
    float maxIn = 10.995;
    float step = 0.1;
    for (;;)
    {
        // Perform LED pulsation using the passed instance
        for (in = 4.712; in < maxIn; in += step)
        {
            led->setColor(led->rgb, in);
            vTaskDelay(pdMS_TO_TICKS(led->rgb.t));
        }
    }
}
/**
 * @brief Set the LED state to indicate an error condition.
 *
 * This function sets the LED state to a predefined RGB value indicating an error condition.
 */
void LEDRGB::error()
{
    rgb.r = 10;
    rgb.g = 0;
    rgb.b = 0;
    rgb.t = 10;
    rgb.o = 55;
}

/**
 * @brief Set the LED state to indicate an idle status.
 *
 * This function sets the LED state to a predefined RGB value indicating an idle status.
 */
void LEDRGB::idle()
{
    rgb.r = 0;
    rgb.g = 255;
    rgb.b = 255;
    rgb.t = 40;
    rgb.o = 55;
}

/**
 * @brief Set the LED state to indicate a waiting status.
 *
 * This function sets the LED state to a predefined RGB value indicating a waiting status.
 */
void LEDRGB::waiting()
{
    rgb.r = 255;
    rgb.g = 0;
    rgb.b = 127;
    rgb.t = 50;
}