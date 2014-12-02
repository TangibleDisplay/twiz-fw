#include "uart.h"
#include "imu.h"
#include "leds.h"
#include "low_res_timer.h"
#include "high_res_timer.h"
#include "twi_gap.h"
#include "twi_conn.h"
#include "twi_advertising.h"
#include "twi_ble_stack.h"
#include "twi_sys_evt.h"
#include "twi_scheduler.h"
#include "twi_calibration_store.h"
#include "ak8975a.h"

/**@brief Function for application main entry.
 */
int main(void)
{
    // Initialize
    leds_init();
    ble_stack_init();
    low_res_timer_init();
    high_res_timer_init();
    uart_init();
    imu_init();
    APP_ERROR_CHECK(pstorage_init());
    calibration_store_init();

    // Setup BLE stack
    advertising_init();
    conn_params_init();
    sec_params_init();
    gap_params_init();

    // Start execution
    low_res_timer_start();
    advertising_start();

    // Try load calibration data from flash
    imu_load_calibration_data();

    led_on(LED_G);
    // Wait for 2 seconds for a 'c' on the serial port.
    // If we get a 'c', then start calibration procedure
    printf("Press c within 2 seconds to start calibration procedure\r\n");
    static char c;
    for (int i=0; i<2000; i++) {
        bool button_was_pressed = nrf_gpio_pin_read(BUTTON);
        if (getchar_timeout(1, &c) || button_was_pressed)
            if(c == 'c' || button_was_pressed) {
                led_off(LED_G);
                led_on(LED_R);
                printf("Starting calibration procedure\r\n");
                printf("Please close minicom and start python calibration GUI\r\n");
                imu_calibrate(button_was_pressed);
                led_off(LED_R);
                break;
            }
    }
    led_off(LED_G);

    // Enter main loop
    for (;;)
    {
        imu_update();
    }
}

/**
 * @}
 */
