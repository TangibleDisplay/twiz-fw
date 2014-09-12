#ifndef TWI_CONN_H
#define TWI_CONN_H

#include "ble_conn_params.h"

#include <stdint.h>

#include "low_res_timer.h"

/*
 * Minimum acceptable connection interval (7.5 milliseconds).
 */
#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)
/*
 * Maximum acceptable connection interval (75 milliseconds).
 */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(200, UNIT_1_25_MS)
/*
 * Slave latency.
 */
#define SLAVE_LATENCY                   0
/*
 * Connection supervisory timeout (4 seconds).
 */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)
/*
 * Time from initiating event (connect or start of notification) to first time
 * sd_ble_gap_conn_param_update is called (15 seconds).
 */
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(20000, APP_TIMER_PRESCALER)
/*
 * Time between each call to sd_ble_gap_conn_param_update after the first call (5 seconds).
 */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)
/*
 * Number of attempts before giving up the connection parameter negotiation.
 */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3


/*
 * Handle of the current connection.
 */
extern uint16_t m_conn_handle;


/*
 * Function for handling a Connection Parameters error
 * nrf_error   Error code containing information about what went wrong.
 */
void conn_params_error_handler(uint32_t nrf_error);


/*
 * Function for initializing the Connection Parameters module.
 */
void conn_params_init(void);


#endif
