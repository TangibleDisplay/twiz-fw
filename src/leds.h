#ifndef LEDS_H
#define LEDS_H

#include <stdint.h>
#include "boards.h"

typedef enum {
    GREEN = LED_0,
    RED   = LED_1
} color_t;

void leds_init(void);
void led_blink(color_t c, uint16_t ms);
void led_on(color_t c);
void led_off(color_t c);

#endif
