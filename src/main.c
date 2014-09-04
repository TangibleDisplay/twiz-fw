
#include "timers.h"
#include "leds.h"
#include "simple_uart.h"
#include "printf.h"

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    // Initialize
    leds_init();
    timers_init();
    simple_uart_config(3, 0, 2, 1, 0); /*rts, tx, cts, rx, flow-ctrl*/
    while(1) {
        leds_blink(500);
        printf("Hello float=%.20f\r\n", 1.123456789987654321);
    }
    return 0;
}
