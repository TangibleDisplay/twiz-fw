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

#include "hal_transport.h"
#include "ble_rpc_event_encoder.h"

#include "ble_gap.h"
#include "ble.h"
#include "ble_hci.h"

#include "stddef.h"
#include <string.h>
#include "ble_rpc_defines.h"
#include "app_util.h"


static uint8_t m_tx_buffer[56];


static void error_test_assert_handler(void)
{
}

static void error_assert_handler(void)
{
    CU_ASSERT(false);
}

void (*error_handler) (void) = error_assert_handler;


void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_nam)
{
    error_handler();
//    CU_ASSERT(false);
}

CU_TEST_START(encode_event_gap_connected)
{
    uint8_t zero       = 0x0;
    uint8_t expected[] = {BLE_RPC_PKT_EVT,
                          BLE_GAP_EVT_CONNECTED, zero,
                          56,  zero,
                          BLE_GAP_ADDR_TYPE_RANDOM_STATIC,
                          1, 2, 3, 4, 5, 6,
                          (0x5E << 1) | 0x01,
                          88,  zero,
                          100, zero,
                          99,  zero,
                          30,  zero};

    ble_evt_t event;
    event.header.evt_id                                               = BLE_GAP_EVT_CONNECTED; /* 0x10 */
    event.header.evt_len                                              = 100; // non-valid length
                                                                      
    event.evt.gap_evt.conn_handle                                     = 56;
    event.evt.gap_evt.params.connected.peer_addr.addr_type            = BLE_GAP_ADDR_TYPE_RANDOM_STATIC; /* 0x01 */
    event.evt.gap_evt.params.connected.peer_addr.addr[0]              = 1;
    event.evt.gap_evt.params.connected.peer_addr.addr[1]              = 2;
    event.evt.gap_evt.params.connected.peer_addr.addr[2]              = 3;
    event.evt.gap_evt.params.connected.peer_addr.addr[3]              = 4;
    event.evt.gap_evt.params.connected.peer_addr.addr[4]              = 5;
    event.evt.gap_evt.params.connected.peer_addr.addr[5]              = 6;
    event.evt.gap_evt.params.connected.irk_match                      = 1;
    event.evt.gap_evt.params.connected.irk_match_idx                  = 0x5E;
    event.evt.gap_evt.params.connected.conn_params.min_conn_interval  = 88;
    event.evt.gap_evt.params.connected.conn_params.max_conn_interval  = 100;
    event.evt.gap_evt.params.connected.conn_params.slave_latency     = 99;
    event.evt.gap_evt.params.connected.conn_params.conn_sup_timeout   = 30;

    nrf_cunit_mock_call * p_mock_call;

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);

    p_mock_call->arg[1] = (uint32_t) &expected[0];
    p_mock_call->arg[2] = sizeof(expected);

    /** This will get into the encoder impl. */
    ble_rpc_event_handle(&event);

    (void)expected;
}
CU_TEST_END


CU_TEST_START(encode_event_gap_disconnected)
{
    uint8_t zero       = 0x0;
    uint8_t expected[] = {BLE_RPC_PKT_EVT,
                          BLE_GAP_EVT_DISCONNECTED, zero,
                          44,  zero,
                          BLE_HCI_CONN_INTERVAL_UNACCEPTABLE};

    ble_evt_t event;
    event.header.evt_id                                       = BLE_GAP_EVT_DISCONNECTED; /* 0x11 */
    event.header.evt_len                                      = 100; // non-valid length

    event.evt.gap_evt.conn_handle                             = 44;
    event.evt.gap_evt.params.disconnected.reason              = BLE_HCI_CONN_INTERVAL_UNACCEPTABLE;

    nrf_cunit_mock_call * p_mock_call;

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);
    
    /** This will get into the encoder impl. */
    ble_rpc_event_handle(&event);
}
CU_TEST_END



CU_TEST_START(encode_event_gap_timeout)
{
    uint8_t zero       = 0x0;
    uint8_t expected[] = {BLE_RPC_PKT_EVT,
                          BLE_GAP_EVT_TIMEOUT, zero,
                          33,  zero,
                          BLE_GAP_TIMEOUT_SRC_SECURITY_REQUEST};

    ble_evt_t event;
    event.header.evt_id                                       = BLE_GAP_EVT_TIMEOUT; /* 0x11 */
    event.header.evt_len                                      = 100; // non-valid length

    event.evt.gap_evt.conn_handle                             = 33;
    event.evt.gap_evt.params.timeout.src                      = BLE_GAP_TIMEOUT_SRC_SECURITY_REQUEST;

    nrf_cunit_mock_call * p_mock_call;

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);

    /** This will get into the encoder impl. */
    ble_rpc_event_handle(&event);
}
CU_TEST_END

CU_TEST_START(encode_event_conn_param_update)
{
    uint8_t zero       = 0x0;
    uint8_t expected[] = {BLE_RPC_PKT_EVT,
                          BLE_GAP_EVT_CONN_PARAM_UPDATE, zero,
                          56,  zero,
                          88,  zero,
                          100, zero,
                          99,  zero,
                          30,  zero};

    ble_evt_t event;
    event.header.evt_id                                              = BLE_GAP_EVT_CONN_PARAM_UPDATE; /* 0x12 */
    event.header.evt_len                                             = 100; // non-valid length

    event.evt.gap_evt.conn_handle                                    = 56;
    event.evt.gap_evt.params.conn_param_update.conn_params.min_conn_interval    = 88;
    event.evt.gap_evt.params.conn_param_update.conn_params.max_conn_interval    = 100;
    event.evt.gap_evt.params.conn_param_update.conn_params.slave_latency        = 99;
    event.evt.gap_evt.params.conn_param_update.conn_params.conn_sup_timeout     = 30;

    nrf_cunit_mock_call * p_mock_call;

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);

    /** This will get into the encoder impl. */
    ble_rpc_event_handle(&event);
}
CU_TEST_END

CU_TEST_START(encode_event_sec_param_request)
{
    uint8_t expected[] = {BLE_RPC_PKT_EVT,
                          BLE_GAP_EVT_SEC_PARAMS_REQUEST, 0,
                          0xDA, 0xBC,
                          0xCD, 0xAB,
                          0x35,        // 0b00110101
                          0xAA,
                          0x55};
    ble_evt_t event;
    //memset(&event, 0x00, sizeof(event));
    event.header.evt_id                                                  = BLE_GAP_EVT_SEC_PARAMS_REQUEST;
    event.header.evt_len                                                 = 100; // non-valid length

    event.evt.gap_evt.conn_handle                                        = 0xBCDA;
    event.evt.gap_evt.params.sec_params_request.peer_params.timeout      = 0xABCD;
    event.evt.gap_evt.params.sec_params_request.peer_params.bond         = 1;
    event.evt.gap_evt.params.sec_params_request.peer_params.mitm         = 0;
    event.evt.gap_evt.params.sec_params_request.peer_params.io_caps      = 5;
    event.evt.gap_evt.params.sec_params_request.peer_params.oob          = 1;
    event.evt.gap_evt.params.sec_params_request.peer_params.min_key_size = 0xAA;
    event.evt.gap_evt.params.sec_params_request.peer_params.max_key_size = 0x55;
    
    nrf_cunit_mock_call * p_mock_call;

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);
    
    /** This will get into the encoder impl. */
    ble_rpc_event_handle(&event);
}
CU_TEST_END

CU_TEST_START(encode_event_sec_info_request)
{
    uint8_t expected[] = {BLE_RPC_PKT_EVT,
                          BLE_GAP_EVT_SEC_INFO_REQUEST, 0,
                          0xDA, 0xBC,
                          BLE_GAP_ADDR_TYPE_RANDOM_STATIC,
                          1, 2, 3, 4, 5, 6,
                          0x34, 0x12,
                          5};
    ble_evt_t event;
    event.header.evt_id                                                  = BLE_GAP_EVT_SEC_INFO_REQUEST; /* 0x12 */
    event.header.evt_len                                                 = 100; // non-valid length

    event.evt.gap_evt.conn_handle                                        = 0xBCDA;
    
    event.evt.gap_evt.params.sec_info_request.peer_addr.addr_type = BLE_GAP_ADDR_TYPE_RANDOM_STATIC; /* 0x01 */
    event.evt.gap_evt.params.sec_info_request.peer_addr.addr[0]   = 1;
    event.evt.gap_evt.params.sec_info_request.peer_addr.addr[1]   = 2;
    event.evt.gap_evt.params.sec_info_request.peer_addr.addr[2]   = 3;
    event.evt.gap_evt.params.sec_info_request.peer_addr.addr[3]   = 4;
    event.evt.gap_evt.params.sec_info_request.peer_addr.addr[4]   = 5;
    event.evt.gap_evt.params.sec_info_request.peer_addr.addr[5]   = 6;
    
    event.evt.gap_evt.params.sec_info_request.div = 0x1234;
    
    event.evt.gap_evt.params.sec_info_request.sign_info = 1;
    event.evt.gap_evt.params.sec_info_request.id_info = 0;
    event.evt.gap_evt.params.sec_info_request.enc_info = 1;

    nrf_cunit_mock_call * p_mock_call;

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);

    /** This will get into the encoder impl. */
    ble_rpc_event_handle(&event);
}
CU_TEST_END


CU_TEST_START(encode_event_auth_status)
{
    uint8_t expected[] = {BLE_RPC_PKT_EVT,
                          BLE_GAP_EVT_AUTH_STATUS, 0,
                          0xDA, 0xBC,
                          0xC1,
                          0xC2,
                          0x2A, // 0x00101010
                          0x15,
                          0x15,
                          0x21, 0x43,
                          0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                          0xAB,
                          0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                          9,
                          0,1,2,3,4,5 };

    ble_evt_t event;
    event.header.evt_id = BLE_GAP_EVT_AUTH_STATUS;
    event.header.evt_len = 100; // non-valid length

    event.evt.gap_evt.conn_handle= 0xBCDA;
    event.evt.gap_evt.params.auth_status.auth_status     = 0xC1;
    event.evt.gap_evt.params.auth_status.error_src         = 0xC2;
    event.evt.gap_evt.params.auth_status.sm1_levels.lv1 = 1;
    event.evt.gap_evt.params.auth_status.sm1_levels.lv2 = 0;
    event.evt.gap_evt.params.auth_status.sm1_levels.lv3 = 1;
    event.evt.gap_evt.params.auth_status.sm2_levels.lv1 = 0;
    event.evt.gap_evt.params.auth_status.sm2_levels.lv2 = 1;
    event.evt.gap_evt.params.auth_status.sm2_levels.lv3 = 0;

    event.evt.gap_evt.params.auth_status.periph_kex.ltk         = 1;
    event.evt.gap_evt.params.auth_status.periph_kex.ediv_rand   = 0;
    event.evt.gap_evt.params.auth_status.periph_kex.irk         = 1;
    event.evt.gap_evt.params.auth_status.periph_kex.address     = 0;
    event.evt.gap_evt.params.auth_status.periph_kex.csrk        = 1;

    event.evt.gap_evt.params.auth_status.central_kex.ltk        = 1;
    event.evt.gap_evt.params.auth_status.central_kex.ediv_rand  = 0;
    event.evt.gap_evt.params.auth_status.central_kex.irk        = 1;
    event.evt.gap_evt.params.auth_status.central_kex.address    = 0;
    event.evt.gap_evt.params.auth_status.central_kex.csrk       = 1;

    event.evt.gap_evt.params.auth_status.periph_keys.enc_info.div = 0x4321;

    uint32_t i = 0;
    for (i = 0; i < BLE_GAP_SEC_KEY_LEN; i++)
    {
        event.evt.gap_evt.params.auth_status.periph_keys.enc_info.ltk[i] = i;
    }

    event.evt.gap_evt.params.auth_status.periph_keys.enc_info.auth    = 1;
    event.evt.gap_evt.params.auth_status.periph_keys.enc_info.ltk_len = 0x55;

    for (i = 0; i < BLE_GAP_SEC_KEY_LEN; i++)
    {
        event.evt.gap_evt.params.auth_status.central_keys.irk.irk[i] = i;
    }

    event.evt.gap_evt.params.auth_status.central_keys.id_info.addr_type = 9;

    for (i = 0; i < BLE_GAP_ADDR_LEN; i++)
    {
        event.evt.gap_evt.params.auth_status.central_keys.id_info.addr[i] = i;
    }

    nrf_cunit_mock_call * p_mock_call;

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);

    /** This will get into the encoder impl. */
    ble_rpc_event_handle(&event);
}
CU_TEST_END


CU_TEST_START(encode_event_conn_sec_update)
{
    uint8_t expected[] = {BLE_RPC_PKT_EVT,
                          BLE_GAP_EVT_CONN_SEC_UPDATE, 0,
                          0xDA, 0xBC,
                          0x85,
                          0x28};

    ble_evt_t event;
    event.header.evt_id = BLE_GAP_EVT_CONN_SEC_UPDATE;
    event.header.evt_len = 100; // non-valid length

    event.evt.gap_evt.conn_handle= 0xBCDA;

    event.evt.gap_evt.params.conn_sec_update.conn_sec.sec_mode.sm = 5;
    event.evt.gap_evt.params.conn_sec_update.conn_sec.sec_mode.lv = 8;
    event.evt.gap_evt.params.conn_sec_update.conn_sec.encr_key_size = 0x28;

    nrf_cunit_mock_call * p_mock_call;

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);

    /** This will get into the encoder impl. */
    ble_rpc_event_handle(&event);
}
CU_TEST_END


CU_TEST_START(encode_event_alloc_error_should_wait_for_free_buffer)
{
    nrf_cunit_reset_mock();

    uint8_t expected[] = {BLE_RPC_PKT_EVT,
                          BLE_GAP_EVT_TIMEOUT, 0x00,
                          0x33,  0x00,
                          0x01};

    // set custom error handler
    error_handler = error_test_assert_handler;

    // We expect first call to alloc to fail, due to no buffer available.
    nrf_cunit_mock_call * p_mock_call;
    nrf_cunit_mock_call * p_transport_alloc_mock_call;
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_alloc", &p_transport_alloc_mock_call);
    p_transport_alloc_mock_call->error = NRF_ERROR_NO_MEM;

    // The app is expected to loop until a buffer is free, so let second call to alloc return a new buffer.
    uint8_t buffer[20];
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_alloc", &p_transport_alloc_mock_call);
    p_transport_alloc_mock_call->result[0] = (uint32_t) buffer;

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);

    ble_evt_t event;
    event.header.evt_id                  = BLE_GAP_EVT_TIMEOUT; // Use any event
    event.header.evt_len                 = 6; // Size does not matter
    event.evt.gap_evt.conn_handle        = 0x0033;
    event.evt.gap_evt.params.timeout.src = BLE_GAP_TIMEOUT_SRC_SECURITY_REQUEST;

    ble_rpc_event_handle(&event);

    // reset error handler
    error_handler = error_assert_handler;

    nrf_cunit_verify_call_return();
}
CU_TEST_END


CU_TEST_START(encode_event_invalid_event_should_discard_the_event)
{
    nrf_cunit_reset_mock();

    // set custom error handler
    error_handler = error_test_assert_handler;

    // We expect first call to alloc to fail, due to no buffer available.
    nrf_cunit_mock_call * p_transport_alloc_mock_call;
    nrf_cunit_mock_call * p_transport_free_mock_call;

    uint8_t buffer[20];
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_alloc", &p_transport_alloc_mock_call);
    p_transport_alloc_mock_call->result[0] = (uint32_t) buffer;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_free", &p_transport_free_mock_call);

    ble_evt_t event;
    event.header.evt_id                  = BLE_EVT_INVALID; // Use any event
    event.header.evt_len                 = 6; // Size does not matter
    event.evt.gap_evt.conn_handle        = 0xAA33;
    event.evt.gap_evt.params.timeout.src = 0x04;

    ble_rpc_event_handle(&event);

    nrf_cunit_verify_call_return();
}
CU_TEST_END


static void gap_setup_func(void)
{
    nrf_cunit_reset_mock();

    nrf_cunit_mock_call * p_transport_alloc_mock_call;
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_alloc", &p_transport_alloc_mock_call);

    p_transport_alloc_mock_call->result[0] = (uint32_t)m_tx_buffer;
}


static void gap_teardown_func(void)
{
    nrf_cunit_mock_call * p_transport_free_mock_call;
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_free", &p_transport_free_mock_call);
    (void)hci_transport_tx_free();

    nrf_cunit_verify_call_return();
}


CU_SUITE_START_PUBLIC(rpc_evt_encode_misc)
{
    CU_SUITE_ADD_TEST(encode_event_alloc_error_should_wait_for_free_buffer);
    CU_SUITE_ADD_TEST(encode_event_invalid_event_should_discard_the_event);
}
CU_SUITE_END

CU_SUITE_START_PUBLIC(rpc_event_encode_gap)
{
    CU_SUITE_SETUP_FUNC(gap_setup_func);
    CU_SUITE_TEARDOWN_FUNC(gap_teardown_func);

    CU_SUITE_ADD_TEST(encode_event_gap_connected);
    CU_SUITE_ADD_TEST(encode_event_gap_disconnected);
    CU_SUITE_ADD_TEST(encode_event_gap_timeout);
    CU_SUITE_ADD_TEST(encode_event_conn_param_update);
    CU_SUITE_ADD_TEST(encode_event_sec_param_request);
    CU_SUITE_ADD_TEST(encode_event_sec_info_request);
    CU_SUITE_ADD_TEST(encode_event_auth_status);
    CU_SUITE_ADD_TEST(encode_event_conn_sec_update);
}
CU_SUITE_END
