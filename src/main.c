#include <stdbool.h>
#include "app_error.h"
#include "hardware.h"
#include "ble_uart.h"
#include "mpu.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    // Initialize
    leds_init();
    timers_init();
    ble_stack_init();
    gap_params_init();
    services_init();
    advertising_init();
    conn_params_init();
    sec_params_init();

    advertising_start();

    bool flag0 = true;
    bool err;

    uint32_t timestamp_ms;

    while(true)
    {
        if (bleUartConnected()) {
            power_manage();
            if (flag0) {
                nrf_delay_ms(50); // TODO find why nothing here is printed without it
                print("Connected\n");

                err = mpuInit(); // TODO find why it doesn't always work
                APP_ERROR_CHECK_BOOL(err);
                print("IMU init OK\n");

                flag0 = false;
            }
        } else {
            flag0 = true;

            if (ms_passed_since(timestamp_ms) >= 100) {
                nrf_gpio_pin_toggle(LED_0);
                get_ms(&timestamp_ms); // save last blink date
            }
        }
    }
}


