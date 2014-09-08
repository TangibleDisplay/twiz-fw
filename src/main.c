#include <stdint.h>
#include <stdbool.h>

#include "nordic_common.h"
#include "softdevice_handler.h"
#include "timers.h"
#include "leds.h"
#include "uart.h"
#include "printf.h"
#include "i2c_wrapper.h"
#include "nrf_delay.h"


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    // Init SD so that timers have a clock reference, and to be ablr to call SD functions.
    // Must be done first !
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_SYNTH_250_PPM, false);
    nrf_delay_ms(100);

    // Init LEDs
    leds_init();

    // Init UART
    uart_init();
    printf("Hello World!\r\n");

    // Init timers
    timer_init();

    // Init TWI
    i2c_init();

    // Run MPU9150 mainloop
    mpu9150_init();
    mpu9150_calibrate();
    ak8975a_init();
    ak8975a_calibrate();
    mpu9150_mainloop();


    // Main loop
    while(1) {
        leds_blink(500);
        printf("Hello float=%.20f, time=%d\r\n", 1.123456789987654321, get_time());
    }
    return 0;
}
