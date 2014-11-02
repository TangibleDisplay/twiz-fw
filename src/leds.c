#include "leds.h"
#include "nrf_gpio.h"
#include "high_res_timer.h"

void led_on(color_t c)
{
    nrf_gpio_pin_set(c);
}

void led_off(color_t c)
{
    nrf_gpio_pin_clear(c);
}

void leds_init(void)
{
    nrf_gpio_cfg_output(GREEN);
    nrf_gpio_pin_clear(GREEN);

    nrf_gpio_cfg_output(RED);
    nrf_gpio_pin_clear(RED);
}

void led_blink(color_t c, uint16_t ms)
{
    nrf_gpio_pin_set(c);
    nrf_timer_delay_ms(ms);
    nrf_gpio_pin_clear(c);
    nrf_timer_delay_ms(ms);
}

