#ifndef HIGH_RES_TIMERS_H
#define HIGH_RES_TIMERS_H

#include <stdint.h>

// Warning : this function needs the clock to be configured either through SD init or manually
void high_res_timer_init(void);
void nrf_timer_delay_ms(uint32_t ms);
uint32_t get_time();

#endif // TIMERS_H
