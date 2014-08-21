#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "hardware.h"

static app_timer_id_t   m_RTC_timer_id;
static int32_t          millisecond_counter = 0;


/**@brief     Error handler function, which is called when an error has occurred.
 *
 * @warning   This handler is an example only and does not fit a final product. You need to analyze
 *            how your product is supposed to react in case of error.
 *
 * @param[in] error_code  Error code supplied to the handler.
 * @param[in] line_num    Line number where the handler is called.
 * @param[in] p_file_name Pointer to the file name.
 */
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    // This call can be used for debug purposes during application development.
    // @note CAUTION: Activating this code will write the stack to flash on an error.
    //                This function should NOT be used in a final product.
    //                It is intended STRICTLY for development/debugging purposes.
    //                The flash write will happen EVEN if the radio is active, thus interrupting
    //                any communication.
    //                Use with care. Un-comment the line below to use.
    // ble_debug_assert_handler(error_code, line_num, p_file_name);

    // On assert, the system can only recover with a reset.
    NVIC_SystemReset();
}

/**@brief       Assert macro callback function.
 *
 * @details     This function will be called in case of an assert in the SoftDevice.
 *
 * @warning     This handler is an example only and does not fit a final product. You need to
 *              analyze how your product is supposed to react in case of Assert.
 * @warning     On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

void leds_init(void)
{
    nrf_gpio_pin_clear(LED_0);
    nrf_gpio_cfg_output(LED_0);
}

void leds_blink(uint16_t ms)
{
    nrf_gpio_pin_set(LED_0);
    nrf_delay_ms(ms);
    nrf_gpio_pin_clear(LED_0);
    nrf_delay_ms(ms);
}


/**@brief Function for handling the RTC timer timeout.
 *
 * @details This function will be called each time the RTC timer expires (every ms).
 *
 * @param[in]   p_context   Pointer used for passing some arbitrary information (context) from the
 *                          app_start_timer() call to the timeout handler.
 */
static void RTC_timeout_handler(void * p_context)
{
    ++millisecond_counter;
}

/**@brief Function for to get milliseconds count since the RTC timer started
 */
int get_ms(uint32_t *count) // respects eMD6 convention
{
    if (!count)
        return 1;
    *count = millisecond_counter;
    return 0;
}

/**@brief Function for to get how many milliseconds passed since given timestamp
 */
uint32_t ms_passed_since(uint32_t last_ms)
{
    uint32_t now_ms;
    get_ms(&now_ms);

    return now_ms - last_ms;
}

/**@brief Function for starting the application timers.
 */
static void application_timers_start(void)
{
    // Start application timers
    uint32_t err_code = app_timer_start(m_RTC_timer_id, RTC_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function to initialize, create and start the times
 */
void timers_init(void)
{
    // Initialize timer module
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, false);

    // Create timers
    uint32_t err_code = app_timer_create(&m_RTC_timer_id,
                                         APP_TIMER_MODE_REPEATED,
                                         RTC_timeout_handler);
    APP_ERROR_CHECK(err_code);

    application_timers_start(); // TODO move ?
}

