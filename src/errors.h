#ifndef ERRORS_H
#define ERRORS_H

#include <stdint.h>
#include "ble_debug_assert_handler.h"

void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name);
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name);

#endif
