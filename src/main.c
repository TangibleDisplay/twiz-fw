#include <stdint.h>

#include "nordic_common.h"
#include "softdevice_handler.h"
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
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_SYNTH_250_PPM, false);

    while(1) {
        leds_blink(500);
        static uint32_t t;
        get_ms(&t);
        printf("Hello float=%.20f, time=%d\r\n", 1.123456789987654321, t);

    }
    return 0;
}
