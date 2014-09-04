#include "app_timer.h"
#include "timers.h"

/**< Value of the RTC1 PRESCALER register. */
#define APP_TIMER_PRESCALER     0
/**< Maximum number of simultaneously created timers. */
#define APP_TIMER_MAX_TIMERS    2
/**< Size of timer operation queues. */
#define APP_TIMER_OP_QUEUE_SIZE 4
/**< RTC interval, argument in ms (ticks). */
#define RTC_INTERVAL            APP_TIMER_TICKS(1, APP_TIMER_PRESCALER)

static app_timer_id_t   m_RTC_timer_id;
static int32_t          millisecond_counter = 0;

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

    // Start application timers
    err_code = app_timer_start(m_RTC_timer_id, RTC_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);

}
