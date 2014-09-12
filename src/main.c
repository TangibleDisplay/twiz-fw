#include "uart.h"
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
#include "twi_calibration_store.h"


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

    // Enter main loop
    for (;;)
    {
        mpu9150_update();
    }
}

/**
 * @}
 */
