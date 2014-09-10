#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_soc.h"
#include "timers.h"
#include "printf.h"

// This function *MUST* be called *AFTER* soft device init, else external clock
// is not yet configured !
void timer_init()
{
    // We'll use TIMER1 and TIMER2 (both 16 bits) to create a virtual 32 bit TIMER
    // Stop timers
    NRF_TIMER1->TASKS_STOP  = 1;
    NRF_TIMER2->TASKS_STOP  = 1;
    // Clear timers
    NRF_TIMER1->TASKS_CLEAR = 1;
    NRF_TIMER2->TASKS_CLEAR = 1;
    // Set the timer1 in Timer Mode, and timer 2 in counter mode
    NRF_TIMER1->MODE        = TIMER_MODE_MODE_Timer;
    NRF_TIMER2->MODE        = TIMER_MODE_MODE_Counter;
    // Prescaler 4 produces 1MHz for timer1 frequency
    NRF_TIMER1->PRESCALER      = 4;
    // Ensure 16 bit mode on both timers
    NRF_TIMER1->BITMODE        = 0;
    NRF_TIMER2->BITMODE        = 0;
    // On timer1 overflow, increment timer2 using PPI
    NRF_TIMER1->CC[0] = 0;
    sd_ppi_channel_assign(1, &NRF_TIMER1->EVENTS_COMPARE[0], &NRF_TIMER2->TASKS_COUNT);
    sd_ppi_channel_enable_set(PPI_CHEN_CH1_Msk);
    // Start timers
    NRF_TIMER2->TASKS_START = 1;
    NRF_TIMER1->TASKS_START = 1;
}


// Get current time
uint32_t get_time()
{
    // Capture both timers
    NRF_TIMER2->TASKS_CAPTURE[1] = 1;
    uint32_t th = NRF_TIMER2->CC[1];
    NRF_TIMER1->TASKS_CAPTURE[1] = 1;
    uint32_t tl = NRF_TIMER1->CC[1];
    NRF_TIMER2->TASKS_CAPTURE[1] = 1;
    uint32_t th2 = NRF_TIMER2->CC[1];
    if (th != th2) {
        // Overflow --> reread TIMER1
        NRF_TIMER1->TASKS_CAPTURE[1] = 1;
        tl = NRF_TIMER1->CC[1];
    }

    return (th << 16) | tl;
}


void nrf_timer_delay_ms(uint32_t ms)
{
    uint32_t now = get_time();
    while(get_time() < (now + ms*1000));
}
