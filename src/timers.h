#ifndef TIMERS_H
#define TIMERS_H

#include <stdint.h>

// Warning : this function needs the clock to be configured either throug SD init or manually
void timer_init(void);
void nrf_timer_delay_ms(uint32_t ms);
uint32_t get_time();
//uint32_t ms_passed_since(uint32_t);

#endif // TIMERS_H
