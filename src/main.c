#include <stdbool.h>
#include "app_error.h"
#include "hardware.h"
#include "ble_uart.h"
#include "mpu.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "inv_mpu.h"

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
                nrf_delay_ms(30); // TODO find why useful
                print("Connected\n");

                nrf_delay_ms(30); // TODO find why useful
                struct int_param_s int_param;
                err = mpu_init(&int_param); // TODO: set interrupt pin !
                APP_ERROR_CHECK_BOOL(err);
                print("IMU init OK\n");

                nrf_gpio_pin_set(LED_0);
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


