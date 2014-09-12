#include "twi_ble_stack.h"

#include "twi_ble_evt.h"
#include "twi_sys_evt.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "twi_error.h"

void ble_stack_init(void) {
  // Initialize the SoftDevice handler module.
  SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_SYNTH_250_PPM, false);
  // Register with the SoftDevice handler module for BLE events.
  ERR_CHECK(softdevice_ble_evt_handler_set(ble_evt_dispatch));
  // Register with the SoftDevice handler module for BLE events.
  ERR_CHECK(softdevice_sys_evt_handler_set(sys_evt_dispatch));
}
