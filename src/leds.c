#include "leds.h"
#include "nrf_gpio.h"
#include "high_res_timer.h"

void led_on(int c)
{
    // inverted logic
    nrf_gpio_pin_clear(c);
}

void led_off(int c)
{
    // inverted logic
    nrf_gpio_pin_set(c);
}

void leds_init(void)
{
    for (int i = LED_0; i <= LED_2; i++) {
        led_off(i);
        nrf_gpio_cfg_output(i);
    }
}

void led_blink(int c, uint16_t ms)
{
    led_on(c);
    nrf_timer_delay_ms(ms);
    led_off(c);
    nrf_timer_delay_ms(ms);
}

