#include "twi_gap.h"

#include <stdint.h>
#include <string.h>

#include "twi_advertising.h"
#include "twi_conn.h"
#include "twi_error.h"

ble_gap_sec_params_t m_sec_params;

void gap_params_init(void) {
  ble_gap_conn_params_t   gap_conn_params;
  ble_gap_conn_sec_mode_t sec_mode;

  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

  ERR_CHECK(sd_ble_gap_device_name_set(&sec_mode,
				       (const uint8_t *)DEVICE_NAME,
				       strlen(DEVICE_NAME)));

  memset(&gap_conn_params, 0, sizeof(gap_conn_params));
  gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
  gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
  gap_conn_params.slave_latency     = SLAVE_LATENCY;
  gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

  ERR_CHECK(sd_ble_gap_ppcp_set(&gap_conn_params));
}


void sec_params_init(void) {
  m_sec_params.timeout      = SEC_PARAM_TIMEOUT;
  m_sec_params.bond         = SEC_PARAM_BOND;
  m_sec_params.mitm         = SEC_PARAM_MITM;
  m_sec_params.io_caps      = SEC_PARAM_IO_CAPABILITIES;
  m_sec_params.oob          = SEC_PARAM_OOB;
  m_sec_params.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
  m_sec_params.max_key_size = SEC_PARAM_MAX_KEY_SIZE;
}
