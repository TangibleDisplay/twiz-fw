#ifndef TIMERS_H
#define TIMERS_H

#include <stdint.h>

void timers_init(void);
int get_ms(uint32_t*);
uint32_t ms_passed_since(uint32_t);

#endif // TIMERS_H
