#include "twi_error.h"

#include "ble_debug_assert_handler.h"
#include "nrf_soc.h"


void app_error_handler(err_t error_code, uint32_t line_num, const uint8_t * p_file_name) {
#if RESET_ON_ERROR
    // On assert, the system can only recover with a reset.
    sd_nvic_SystemReset();
#else
    // This call can be used for debug purposes during application development.
    // @note CAUTION: Activating this code will write the stack to flash on an error.
    //                This function should NOT be used in a final product.
    //                It is intended STRICTLY for development/debugging purposes.
    //                The flash write will happen EVEN if the radio is active, thus
    //                interrupting any communication.
    //                Use with care. Un-comment the line below to use.
    ble_debug_assert_handler(error_code, line_num, p_file_name);
#endif
}

void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name) {
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}
