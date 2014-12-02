#ifndef LEDS_H
#define LEDS_H

#include <stdint.h>

void leds_init(void);
void leds_blink(uint16_t ms);
void led_on(void);
void led_off(void);

#endif
