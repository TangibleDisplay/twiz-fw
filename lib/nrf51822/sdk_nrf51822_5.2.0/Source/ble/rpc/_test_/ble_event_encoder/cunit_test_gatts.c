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

#define MAX_EVENT_LEN                           56   /**< Maximum possible length, in bytes, of an encoded BLE Event. (Must be a multiple of 4, to make sure memory is aligned)*/

CU_TEST_START(encode_event_gatts_evt_write_no_data)
{
    uint8_t expected[] = {BLE_RPC_PKT_EVT,
                          BLE_GATTS_EVT_WRITE, 0,
                          0xDA, 0xBC,                   // Connection Handle
                          0x12, 0x34,                   // Attribute Handle
                          BLE_GATTS_OP_WRITE_REQ,       // Write Operation
                           // Context - START
                           0x12, 0x34,                     // Service UUID
                           BLE_UUID_TYPE_BLE,              // Service UUID Type
                           0x56, 0x78,                     // Char UUID
                           BLE_UUID_TYPE_BLE,              // Char UUID Type
                           0x90, 0xAB,                     // Desc UUID
                           BLE_UUID_TYPE_BLE,              // Desc UUID Type
                           0xCD, 0xEF,                     // Service Handle
                           0x12, 0x34,                     // Value Handle
                           BLE_GATTS_ATTR_TYPE_CHAR_VAL,   // Attribute type
                           // Context - STOP
                          0x56, 0x78,                  // Offset
                          0x00, 0x00};                 // Data Length

    ble_evt_t event;

    event.header.evt_id  = BLE_GATTS_EVT_WRITE;
    event.header.evt_len = 100; // non-valid length

    event.evt.gatts_evt.conn_handle = 0xBCDA;

    event.evt.gatts_evt.params.write.handle                 = 0x3412;
    event.evt.gatts_evt.params.write.op                     = BLE_GATTS_OP_WRITE_REQ;
    event.evt.gatts_evt.params.write.context.srvc_uuid.uuid = 0x3412;
    event.evt.gatts_evt.params.write.context.srvc_uuid.type = BLE_UUID_TYPE_BLE;
    event.evt.gatts_evt.params.write.context.char_uuid.uuid = 0x7856;
    event.evt.gatts_evt.params.write.context.char_uuid.type = BLE_UUID_TYPE_BLE;
    event.evt.gatts_evt.params.write.context.desc_uuid.uuid = 0xAB90;
    event.evt.gatts_evt.params.write.context.desc_uuid.type = BLE_UUID_TYPE_BLE;
    event.evt.gatts_evt.params.write.context.srvc_handle    = 0xEFCD;
    event.evt.gatts_evt.params.write.context.value_handle   = 0x3412;
    event.evt.gatts_evt.params.write.context.type           = BLE_GATTS_ATTR_TYPE_CHAR_VAL;
    event.evt.gatts_evt.params.write.offset                 = 0x7856;
    event.evt.gatts_evt.params.write.len                    = 0;
    event.evt.gatts_evt.params.write.data[0]                = 1; //This isn't expected to be encoded

    nrf_cunit_mock_call * p_mock_call;

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);

    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);

    //lint -e628 -e516 -save // no argument information provided for function '__INTADDR__()'
                             // Symbol '__INTADDR__()' has arg. type conflict
    //lint -restore

    /** This will get into the encoder impl. */
    ble_rpc_event_handle(&event);

    /** --------------------------------------- */

    nrf_cunit_verify_call_return();
}
CU_TEST_END


CU_TEST_START(encode_event_gatts_evt_write_with_data)
{
    static uint8_t m_write_data[GATT_MTU_SIZE_DEFAULT - 3];
    uint32_t       i;


    for (i = 0; i < sizeof(m_write_data); i++)
    {
        m_write_data[i] = i + 1; // Sample pattern of data = {1,2,3...}
    }

    uint8_t partial_expected_pkt[] = {BLE_RPC_PKT_EVT,
                                      BLE_GATTS_EVT_WRITE, 0,
                                      0xDA, 0xBC,                       // Connection Handle
                                      0x12, 0x34,                       // Attribute Handle
                                      BLE_GATTS_OP_WRITE_REQ,           // Write Operation
                                       // Context - START
                                       0x12, 0x34,                         // Service UUID
                                       BLE_UUID_TYPE_BLE,                  // Service UUID Type
                                       0x56, 0x78,                         // Char UUID
                                       BLE_UUID_TYPE_BLE,                  // Char UUID Type
                                       0x90, 0xAB,                         // Desc UUID
                                       BLE_UUID_TYPE_BLE,                  // Desc UUID Type
                                       0xCD, 0xEF,                         // Service Handle
                                       0x12, 0x34,                         // Value Handle
                                       BLE_GATTS_ATTR_TYPE_CHAR_VAL,       // Attribute type
                                       // Context - STOP
                                      0x56, 0x78,                       // Offset
                                      sizeof(m_write_data), 0};         // Data Length

    uint8_t expected_pkt[sizeof(partial_expected_pkt) + sizeof(m_write_data)];

    memcpy(expected_pkt, partial_expected_pkt, sizeof(partial_expected_pkt));
    memcpy(&(expected_pkt[sizeof(partial_expected_pkt)]), m_write_data, sizeof(m_write_data));

    STATIC_ASSERT((MAX_EVENT_LEN % sizeof(uint32_t)) == 0);
    uint32_t    memory_for_var_length_event[MAX_EVENT_LEN / sizeof(uint32_t)];
    ble_evt_t * p_event = (ble_evt_t *)memory_for_var_length_event;

    p_event->header.evt_id  = BLE_GATTS_EVT_WRITE;
    p_event->header.evt_len = 100; // non-valid length

    p_event->evt.gatts_evt.conn_handle= 0xBCDA;

    p_event->evt.gatts_evt.params.write.handle                 = 0x3412;
    p_event->evt.gatts_evt.params.write.op                     = BLE_GATTS_OP_WRITE_REQ;
    p_event->evt.gatts_evt.params.write.context.srvc_uuid.uuid = 0x3412;
    p_event->evt.gatts_evt.params.write.context.srvc_uuid.type = BLE_UUID_TYPE_BLE;
    p_event->evt.gatts_evt.params.write.context.char_uuid.uuid = 0x7856;
    p_event->evt.gatts_evt.params.write.context.char_uuid.type = BLE_UUID_TYPE_BLE;
    p_event->evt.gatts_evt.params.write.context.desc_uuid.uuid = 0xAB90;
    p_event->evt.gatts_evt.params.write.context.desc_uuid.type = BLE_UUID_TYPE_BLE;
    p_event->evt.gatts_evt.params.write.context.srvc_handle    = 0xEFCD;
    p_event->evt.gatts_evt.params.write.context.value_handle   = 0x3412;
    p_event->evt.gatts_evt.params.write.context.type           = BLE_GATTS_ATTR_TYPE_CHAR_VAL;
    p_event->evt.gatts_evt.params.write.offset                 = 0x7856;
    p_event->evt.gatts_evt.params.write.len                    = sizeof(m_write_data);

    memcpy(p_event->evt.gatts_evt.params.write.data, m_write_data, sizeof(m_write_data));  /*lint !e419 Apparent data overrun.*/

    nrf_cunit_mock_call * p_mock_call;

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);

    p_mock_call->arg[1] = (uint32_t) expected_pkt;
    p_mock_call->arg[2] = sizeof(expected_pkt);

    //lint -e628 -e516 -save // no argument information provided for function '__INTADDR__()'
                             // Symbol '__INTADDR__()' has arg. type conflict
    //lint -restore

    /** This will get into the encoder impl. */
    ble_rpc_event_handle(p_event);

    /** --------------------------------------- */

    nrf_cunit_verify_call_return();
}
CU_TEST_END

CU_TEST_START(encode_event_gatts_sys_attr_missing)
{
    ble_evt_t event;
    uint8_t zero       = 0x0;
    uint8_t hint = 0x12;
    uint8_t expected[] = {BLE_RPC_PKT_EVT,
                          BLE_GATTS_EVT_SYS_ATTR_MISSING, zero,         // Event ID
                          0xDA, 0xBC,                                   // Connection handle
                          hint};                                        // hint 
                          
    event.header.evt_id                                 = BLE_GATTS_EVT_SYS_ATTR_MISSING; 
    event.header.evt_len                                = 100; // non-valid length
    event.evt.gatts_evt.conn_handle                     = 0xBCDA;
    event.evt.gatts_evt.params.sys_attr_missing.hint    = hint;
                
    nrf_cunit_mock_call * p_mock_call;

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);

    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);
                          
     /** This will get into the encoder impl. */
    ble_rpc_event_handle(&event);
                          
    /** Verify Results */
    nrf_cunit_verify_call_return();       
    
}
CU_TEST_END


static void gatts_setup_func(void)
{
    nrf_cunit_reset_mock();

    nrf_cunit_mock_call * p_transport_alloc_mock_call;
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_alloc", &p_transport_alloc_mock_call);

    p_transport_alloc_mock_call->result[0] = (uint32_t)m_tx_buffer;
}


static void gatts_teardown_func(void)
{
    nrf_cunit_mock_call * p_transport_free_mock_call;
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_free", &p_transport_free_mock_call);
    (void)hci_transport_tx_free();

    nrf_cunit_verify_call_return();
}


CU_SUITE_START_PUBLIC(rpc_event_encode_gatts)
{
    CU_SUITE_SETUP_FUNC(gatts_setup_func);
    CU_SUITE_TEARDOWN_FUNC(gatts_teardown_func);

    CU_SUITE_ADD_TEST(encode_event_gatts_evt_write_no_data);
    CU_SUITE_ADD_TEST(encode_event_gatts_evt_write_with_data);
    CU_SUITE_ADD_TEST(encode_event_gatts_sys_attr_missing);
}
CU_SUITE_END
