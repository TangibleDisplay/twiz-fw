#include <stdint.h>
#include <string.h>
#include "uart.h"
#include "boards.h"
#include "nordic_common.h"
#include "mpu9150.h"
#include "leds.h"
#include "low_res_timer.h"
#include "high_res_timer.h"
#include "twi_gap.h"
#include "twi_conn.h"
#include "twi_advertising.h"
#include "twi_ble_stack.h"
#include "twi_sys_evt.h"
#include "twi_scheduler.h"


// IMU timer handler
void imu_timer_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);

    // update imu data to advertize it
	advertising_init();
    // Visual debug : toggle LED 0
    nrf_gpio_pin_toggle(LED_0);
}


/**@brief Function for application main entry.
 */
int main(void)
{
    // Initialize
    leds_init();
    ble_stack_init();
    scheduler_init();
    low_res_timer_init();
    high_res_timer_init();
    uart_init();
    imu_init();

    // Setup BLE stack
    advertising_init();
    conn_params_init();
    sec_params_init();
    gap_params_init();

    // Start execution
    low_res_timer_start();
    advertising_start();

    // Enter main loop
    for (;;)
    {
        app_sched_execute();
        mpu9150_update();
    }
}

/**
 * @}
 */
