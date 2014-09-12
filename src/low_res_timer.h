#ifndef LOW_RES_TIMER_H
#define LOW_RES_TIMER_H

#include <stdint.h>
#include "app_timer.h"

#define APP_TIMER_PRESCALER  0   /**< Value of the RTC1 PRESCALER register. */

void low_res_timer_init(void);
void low_res_timer_start(void);

#endif
