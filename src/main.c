
#include "timers.h"

/**
 * @brief Function for application main entry.
 */
int main(void)
{

    // Initialize
    leds_init();
    timers_init();
    //uart_init();
    while(1)
        leds_blink(500);
}
