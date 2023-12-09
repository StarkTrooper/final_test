#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/device.h>
#include <hal/nrf_gpio.h>

// With just one RGB LED
/*
#define RGB_LED_NODE DT_ALIAS(gpio0) //rgb_led|gpio0   

#define SLEEP_TIME_MS   1000

void set_rgb_color(const struct device *dev, gpio_pin_t pin_r, gpio_pin_t pin_g, gpio_pin_t pin_b, uint8_t r, uint8_t g, uint8_t b)
{
    gpio_pin_set(dev, pin_r, r);
    gpio_pin_set(dev, pin_g, g);
    gpio_pin_set(dev, pin_b, b);
}

void main(void)
void LED3()
{
	
    const struct device *rgb_led = device_get_binding(DT_LABEL(RGB_LED_NODE));
	if (!rgb_led)
    {
        printk("RGB LED device not ready\n");
        return;
    }

    gpio_pin_configure(rgb_led, DT_GPIO_PIN(RGB_LED_NODE, r_gpios), GPIO_OUTPUT);
    gpio_pin_configure(rgb_led, DT_GPIO_PIN(RGB_LED_NODE, g_gpios), GPIO_OUTPUT);
    gpio_pin_configure(rgb_led, DT_GPIO_PIN(RGB_LED_NODE, b_gpios), GPIO_OUTPUT);

    while (1)
    {
        // red
        set_rgb_color(rgb_led, DT_GPIO_PIN(RGB_LED_NODE, r_gpios), DT_GPIO_PIN(RGB_LED_NODE, g_gpios), DT_GPIO_PIN(RGB_LED_NODE, b_gpios), 1, 0, 0);
        k_sleep(K_MSEC(SLEEP_TIME_MS));

        // green
        set_rgb_color(rgb_led, DT_GPIO_PIN(RGB_LED_NODE, r_gpios), DT_GPIO_PIN(RGB_LED_NODE, g_gpios), DT_GPIO_PIN(RGB_LED_NODE, b_gpios), 0, 1, 0);
        k_sleep(K_MSEC(SLEEP_TIME_MS));

        // blue
        set_rgb_color(rgb_led, DT_GPIO_PIN(RGB_LED_NODE, r_gpios), DT_GPIO_PIN(RGB_LED_NODE, g_gpios), DT_GPIO_PIN(RGB_LED_NODE, b_gpios), 0, 0, 1);
        k_sleep(K_MSEC(SLEEP_TIME_MS));
    }
}
*/

// In case having 3 different LEDs (easiest scenario)

#define LED_GREEN_NODE DT_ALIAS(led0)  // Just change pins
#define LED_YELLOW_NODE DT_ALIAS(led1)
#define LED_RED_NODE DT_ALIAS(led2)

#define SLEEP_TIME_MS   1000

void turn_on_led(const struct device *dev, gpio_pin_t pin)
{
    gpio_pin_set(dev, pin, 1);
}

void turn_off_led(const struct device *dev, gpio_pin_t pin)
{
    gpio_pin_set(dev, pin, 0);
}

//void main(void)   // LEDS3
void LED3()
{
    const struct device *led_green_dev = device_get_binding(DT_LABEL(LED_GREEN_NODE));
    const struct device *led_yellow_dev = device_get_binding(DT_LABEL(LED_YELLOW_NODE));
    const struct device *led_red_dev = device_get_binding(DT_LABEL(LED_RED_NODE));

    if (!led_green_dev || !led_yellow_dev || !led_red_dev)
    {
        printk("Error obtaining LED devices\n");
        return;
    }

    gpio_pin_configure(led_green_dev, DT_GPIO_PIN(LED_GREEN_NODE, gpios), GPIO_OUTPUT);
    gpio_pin_configure(led_yellow_dev, DT_GPIO_PIN(LED_YELLOW_NODE, gpios), GPIO_OUTPUT);
    gpio_pin_configure(led_red_dev, DT_GPIO_PIN(LED_RED_NODE, gpios), GPIO_OUTPUT);

    while (1)
    {
        turn_on_led(led_green_dev, DT_GPIO_PIN(LED_GREEN_NODE, gpios));
        k_sleep(K_MSEC(SLEEP_TIME_MS));

        turn_off_led(led_green_dev, DT_GPIO_PIN(LED_GREEN_NODE, gpios));
        turn_on_led(led_yellow_dev, DT_GPIO_PIN(LED_YELLOW_NODE, gpios));
        k_sleep(K_MSEC(SLEEP_TIME_MS));

        turn_off_led(led_yellow_dev, DT_GPIO_PIN(LED_YELLOW_NODE, gpios));
        turn_on_led(led_red_dev, DT_GPIO_PIN(LED_RED_NODE, gpios));
        k_sleep(K_MSEC(SLEEP_TIME_MS));

        turn_off_led(led_red_dev, DT_GPIO_PIN(LED_RED_NODE, gpios));
    }
}

