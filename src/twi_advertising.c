#include "twi_advertising.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "ble_advdata.h"
#include "nrf_gpio.h"
#include "imu.h"
#include "twi_error.h"

void advertising_init(void) {
  ble_advdata_t advdata;
  uint8_t flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

  // Set advertising data
  memset(&advdata, 0, sizeof(advdata));
  advdata.name_type               = BLE_ADVDATA_FULL_NAME;
  advdata.include_appearance      = false;
  advdata.flags.size              = sizeof(flags);
  advdata.flags.p_data            = &flags;

  // Use manufacturer specific data to broadcast imu data
  ble_advdata_manuf_data_t adv_manuf_data;
  uint8_array_t            adv_manuf_data_array;
  uint8_t                  adv_manuf_data_data[sizeof(imu_data_t)];

  adv_manuf_data_array.p_data = (uint8_t*) get_imu_data( (imu_data_t*) adv_manuf_data_data );
  adv_manuf_data_array.size = sizeof(adv_manuf_data_data);

  // Company identifier of Nordic
  adv_manuf_data.company_identifier = 0x0059;
  adv_manuf_data.data = adv_manuf_data_array;
  advdata.p_manuf_specific_data = &adv_manuf_data;


  // Advertise, but no scan response
  APP_ERROR_CHECK(ble_advdata_set(&advdata, NULL));

}


void advertising_start(void) {
  ble_gap_adv_params_t adv_params;

  memset(&adv_params, 0, sizeof(adv_params));
  adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
  adv_params.p_peer_addr = NULL;
  adv_params.fp          = BLE_GAP_ADV_FP_ANY;
  adv_params.interval    = APP_ADV_INTERVAL;
  adv_params.timeout     = APP_ADV_TIMEOUT_IN_SECONDS;

  ERR_CHECK(sd_ble_gap_adv_start(&adv_params));
}
