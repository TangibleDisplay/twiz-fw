#include "twi_conn.h"

#include <stdbool.h>
#include <string.h>

#include "ble_hci.h"
#include "twi_error.h"

/*
 * At start, there is no connection.
 */
uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;


void conn_params_error_handler(uint32_t nrf_error) {
  APP_ERROR_HANDLER(nrf_error);
}


void conn_params_init(void) {
  ble_conn_params_init_t cp_init;

  memset(&cp_init, 0, sizeof(cp_init));

  cp_init.p_conn_params                  = NULL;
  cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
  cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
  cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
  cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
  cp_init.disconnect_on_fail             = true;
  cp_init.evt_handler                    = NULL;
  cp_init.error_handler                  = conn_params_error_handler;

  ERR_CHECK(ble_conn_params_init(&cp_init));
}
