#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "timers.h"
#include "printf.h"

// This function *MUST* be called *AFTER* soft device init, else external
// is not yet configured !
void timer_init()
{
    // Stop timer.
    NRF_TIMER2->TASKS_STOP         = 1;
    // clear the task first to be usable for later.
    NRF_TIMER2->TASKS_CLEAR    = 1;
    // Set the timer in Timer Mode.
    NRF_TIMER2->MODE           = TIMER_MODE_MODE_Timer;
    // Prescaler 9 produces 31250Hz timer frequency
    NRF_TIMER2->PRESCALER      = 9;
    // 32 bit mode.
    NRF_TIMER2->BITMODE        = 0;

}


/** @brief Function for using the peripheral hardware timers to generate an event after requested number of milliseconds.
 *
 * @param[in] timer Timer to be used for delay, values from @ref  NRF_TIMER2
 * @param[in] number_of_ms Number of milliseconds the timer will count.
 * @note This function will power ON the requested timer, wait until the delay, and then power OFF that timer.
 */
void nrf_timer_delay_ms(uint_fast16_t volatile number_of_ms)
{
    timer_init();

    // With 15625Hz timer, we need to multiply ms by 31.25 to get the number of ticks to wait
    NRF_TIMER2->CC[0] = number_of_ms * 31;
    NRF_TIMER2->CC[0] += number_of_ms/4;

    // Start timer.
    NRF_TIMER2->TASKS_START = 1;

    // Wait for CC event
    while (NRF_TIMER2->EVENTS_COMPARE[0] == 0) {
        //NRF_TIMER2->TASKS_CAPTURE[1] = 1;
        //uint32_t v = NRF_TIMER2->CC[1];
        //printf("%x\r\n", v);
    }

    // Clear event
    NRF_TIMER2->EVENTS_COMPARE[0] = 0;

    // Stop timer.
    NRF_TIMER2->TASKS_STOP = 1;
}
