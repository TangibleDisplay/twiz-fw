#ifndef HARDWARE_H
#define HARDWARE_H

#include "app_timer.h"

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_MAX_TIMERS            2                                           /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                           /**< Size of timer operation queues. */

#define RTC_INTERVAL                    APP_TIMER_TICKS(1, APP_TIMER_PRESCALER)     /**< RTC interval, argument in ms (ticks). */

#define LED_0 13

#define TWI_MASTER_CONFIG_CLOCK_PIN_NUMBER (5U)
#define TWI_MASTER_CONFIG_DATA_PIN_NUMBER (4U)

void leds_init(void);
void leds_blink(uint16_t);
void timers_init(void);
int get_ms(uint32_t*);
uint32_t ms_passed_since(uint32_t);

#endif // HARDWARE_H
