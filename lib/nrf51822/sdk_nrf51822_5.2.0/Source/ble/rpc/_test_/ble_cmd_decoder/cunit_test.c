/* Copyright (c) 2013 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file

  @brief BLE Serialization: Command decoder cunit tests.
*/
#ifdef WIN32
 #include <stdint.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "nrf_cunit.h"
#include "nrf_cunit_mock.h"
#include "ble_rpc_cmd_decoder.h"
#include "ble_gap.h"
#include "ble_gatts.h"
#include "ble_srv_common.h"
#include "nrf_error.h"
#include "app_util.h"
#include "ble_hci.h"
#include "nordic_common.h"
#include "ble_rpc_defines.h"

//lint -esym(550, exp_write_perm)       // Symbol not accessed
//lint -esym(550, expected_conn_params) // Symbol not accessed
//lint -esym(550, expected_adv_params)  // Symbol not accessed
//lint -esym(550, exp_appearance)       // Symbol not accessed
//lint -esym(550, exp_attr_char_values) // Symbol not accessed
//lint -esym(550, exp_char_md)          // Symbol not accessed
//lint -esym(550, exp_handles)          // Symbol not accessed
//lint -e453

/*lint -save -e14 */
#ifndef WIN32
__weak void TC_CT_FUN_BV_UNIT_DECODE_GATTS(void) { /** Do nothing; */ }

__weak void TC_CT_FUN_BV_UNIT_decode_gatts_characteristic_add(void) { /** Do nothing; */ }

__weak void cmd_decoder_gap_suite(void) { /** Do nothing; */ }

__weak void cmd_decoder_ble(void) { /** Do nothing; */ }

__weak void cmd_decoder_misc_suite(void) { /** Do nothing; */ }

__weak void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name) { CU_ASSERT(!"Call to: __weak void app_error_handler(...) is NOT allowed.");}
#endif
/*lint -restore */


CU_REGISTRY_START_STANDALONE(TC_CT_FUN_BV_UNIT_CMD_DECODE)
{
    CU_REGISTRY_ADD_SUITE_EXTERN(cmd_decoder_misc_suite);
    CU_REGISTRY_ADD_SUITE_EXTERN(cmd_decoder_ble);
    CU_REGISTRY_ADD_SUITE_EXTERN(cmd_decoder_gap_suite);
    CU_REGISTRY_ADD_SUITE_EXTERN(TC_CT_FUN_BV_UNIT_DECODE_GATTS);
    CU_REGISTRY_ADD_SUITE_EXTERN(TC_CT_FUN_BV_UNIT_decode_gatts_characteristic_add);
}
CU_REGISTRY_END


int main(void)
{ 
    CU_ENABLE_SUMMARY();
    CU_ADD_REGISTRY(TC_CT_FUN_BV_UNIT_CMD_DECODE);
    return 0;
}
