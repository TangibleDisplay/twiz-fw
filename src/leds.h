#ifndef LEDS_H
#define LEDS_H

#include <stdint.h>
#include "boards.h"

void leds_init(void);
void led_blink(int c, uint16_t ms);
void led_on(int c);
void led_off(int c);

#endif
