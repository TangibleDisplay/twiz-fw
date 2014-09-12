#include <stdint.h>
#include "low_res_timer.h"
#include "nordic_common.h"
#include "twi_advertising.h"
#include "nrf_gpio.h"
#include "boards.h"

#define APP_TIMER_MAX_TIMERS            3                                           /**< Maximum number of simultaneously created timers. */
#define APP_TIMER_OP_QUEUE_SIZE         4                                           /**< Size of timer operation queues. */

app_timer_id_t                          m_imu_timer_id;                             /* The application timer ID that will update periodically the imu data */

#define IMU_UPDATE_INTERVAL_MS          40                                          /* The interval between two imu data updates (to set up the application timer) */

// IMU timer handler
static void imu_timer_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);

    // update imu data to advertize it
	advertising_init();
    // Visual debug : toggle LED 0
    nrf_gpio_pin_toggle(LED_0);
}

// Init low res timer
void low_res_timer_init(void)
{
    // Initialize timer module, making it use the scheduler
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, false);

	// Register the timer that will update the imu data advertizing
	uint32_t err_code = app_timer_create(&m_imu_timer_id, APP_TIMER_MODE_REPEATED, imu_timer_handler);
    APP_ERROR_CHECK(err_code);
}

// Start timers
void low_res_timer_start(void)
{
    //Start the imu data update timer // TODO: use either IMU_UPDATE_INTERVAL_MS or APP_ADV_INTERVAL !!
    uint32_t err_code = app_timer_start(m_imu_timer_id,
                                        APP_TIMER_TICKS(IMU_UPDATE_INTERVAL_MS, APP_TIMER_PRESCALER),
                                        NULL);
    APP_ERROR_CHECK(err_code);
}
