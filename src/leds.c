#include "leds.h"
#include "nrf_gpio.h"
#include "high_res_timer.h"
#include "boards.h"

void led_on()
{
    nrf_gpio_pin_set(LED_0);
}

void led_off()
{
    nrf_gpio_pin_clear(LED_0);
}

void leds_init(void)
{
    nrf_gpio_pin_clear(LED_0);
    nrf_gpio_cfg_output(LED_0);
}

void leds_blink(uint16_t ms)
{
    nrf_gpio_pin_set(LED_0);
    nrf_timer_delay_ms(ms);
    nrf_gpio_pin_clear(LED_0);
    nrf_timer_delay_ms(ms);
}
