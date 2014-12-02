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

  @brief Application GPIOTE cunit tests.
*/
#ifdef WIN32
 #include <stdio.h>
 #include <stdint.h>
 #include <string.h>
#endif
#include <stdbool.h>
#include <stdlib.h>

/* for cunit test framework*/
#include "boards.h"
#include "nrf_cunit.h"
#include "nrf_error.h"
#include "nrf_cunit_mock.h"

#include "ble_rpc_event_encoder.h"

#include "ble_gap.h"
#include "ble.h"
#include "ble_hci.h"

#include "stddef.h"
#include <string.h>
#include "ble_rpc_defines.h"
#include "app_util.h"


CU_REGISTRY_START_STANDALONE(ble_encoder)
{
    CU_REGISTRY_ADD_SUITE_EXTERN(rpc_evt_encode_misc);
    CU_REGISTRY_ADD_SUITE_EXTERN(rpc_event_encode_gap);
    CU_REGISTRY_ADD_SUITE_EXTERN(rpc_event_encode_gatts);
}
CU_REGISTRY_END


int main(void)
{ 
    CU_ENABLE_SUMMARY();
    CU_ADD_REGISTRY(ble_encoder);
    return 0;
}
