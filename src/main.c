#include <stdint.h>
#include <stdbool.h>

#include "nordic_common.h"
#include "softdevice_handler.h"
#include "timers.h"
#include "leds.h"
#include "uart.h"
#include "printf.h"
#include "i2c_wrapper.h"


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    // Init SD so that timers have a clock reference, and to be ablr to call SD functions.
    // Must be done first !
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_SYNTH_250_PPM, false);

    // Init LEDs
    leds_init();

    // Init UART
    uart_init();

    // Init timers
    timers_init();

    // Init TWI
    i2c_init();

    // Read WHOAMI register of MPU9150
    int err;
    uint8_t data = 0;

    err = i2c_write(0x68, 107, 1, &data);
    printf("i2c_write err = %d\r\n", err);

    err = i2c_read(0x68, 0x75, 1, &data);
    printf("err = %d, WHOAMI = %x\r\n", err, data);

    // Write some bytes on reg 25 and 26 and read them back
    uint8_t buf[2] = {0xDE, 0xAD};
    err = i2c_write(0x68, 0x19, 2, buf);
    printf("i2c_write err = %d\r\n", err);

    err = i2c_read(0x68, 0x75, 1, &data);
    printf("err = %d, WHOAMI = %x\r\n", err, data);

    err = i2c_read(0x68, 0x19, 2, buf);
    printf("err = %d, 0x0d = %x, 0x0e = %x\r\n", err, buf[0], buf[1]);



    // Main loop
    while(1) {
        leds_blink(500);
        static uint32_t t;
        get_ms(&t);
        printf("Hello float=%.20f, time=%d\r\n", 1.123456789987654321, t);
    }
    return 0;
}
