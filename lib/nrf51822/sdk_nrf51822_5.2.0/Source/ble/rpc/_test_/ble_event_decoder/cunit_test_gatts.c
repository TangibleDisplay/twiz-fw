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

  @brief BLE Serialization: Event decoder cunit tests.
*/
#ifdef WIN32
 #include <stdint.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include "nrf_cunit.h"
#include "nrf_cunit_mock.h"
#include "nrf_error.h"
#include <nrf51.h>
#include <nrf_soc.h>
#include "ble.h"
#include "ble_gatts.h"
#include "ble_rpc_event_decoder.h"
#include "ble_rpc_defines.h"

//lint -e526 -e628 -e516 // Symbol '__INTADDR__()' not defined
                         // no argument information provided for function '__INTADDR__()'
                         // Symbol '__INTADDR__()' has arg. type conflict

extern bool g_interrupt_called;

static void reset_test(void)
{
}


/** @brief Function for pulling the ble event from the evt decoder and verify it against the input
 *    expected event.
 */ 
static bool compare_ble_events(const ble_evt_t * const p_expected_evt, const uint16_t expected_len,
                               const ble_evt_t * const p_actual_evt, const uint16_t actual_len)
{
    if (expected_len != actual_len)
    {
        CU_MESSAGE("Expected length (%u) of the ble event does not match the actual length (%u).",
                    expected_len, actual_len);
        return false;
    }

    if (memcmp(p_expected_evt, p_actual_evt, expected_len) != 0)
    {
        CU_MESSAGE("Expected event does NOT match the actual event pulled from the event decoder.");

        int i;
        for (i = 0; i < expected_len; i++)
        {
            CU_MESSAGE("Index: %u Actual: %u, Expected %u", i, ((uint8_t *)p_actual_evt)[i],
                            ((uint8_t *)p_expected_evt)[i]);
        }

        return false;
    }

    return true;
}


CU_TEST_START(gatts_evt_sys_attr_missing)
{
    // Place holder for the actual decoded event.
    ble_evt_t actual_ble_evt = {{0}, {{0}}};
    uint16_t  actual_len     = sizeof(actual_ble_evt);

    /**************************** Creation of the encoded input data ****************************/
    /** The connected event as it looks on the wire (UART) */
    uint8_t encoded_gatts_sys_attr_missing_evt[] = { BLE_RPC_PKT_EVT,
                                                     BLE_GATTS_EVT_SYS_ATTR_MISSING, 0,    // Evt ID
                                                     0x86, 0x99,                           // Conn Handle
                                                     0x77                                  // Hint
                                                   };

    uint16_t encoded_packet_length = 6;

    /**************************** Creation of the expected event ****************************/
    /** The BLE event as it should look when it has been decoded and fetch by the application. */
    ble_evt_t expected_ble_evt = {{0}, {{0}}};
    expected_ble_evt.header.evt_id                              = BLE_GATTS_EVT_SYS_ATTR_MISSING;
    expected_ble_evt.header.evt_len                             = 3;
    expected_ble_evt.evt.gatts_evt.conn_handle                  = 0x9986;
    expected_ble_evt.evt.gatts_evt.params.sys_attr_missing.hint = 0x77;

    uint16_t expected_len  = (uint16_t)(offsetof(ble_evt_t, evt.gatts_evt.params.sys_attr_missing));
    expected_len          += sizeof(ble_gatts_evt_sys_attr_missing_t);

    /**************************** Execution of the test / Verify decoder behavior ****************************/
    // Simulate an event from the transport layer. -- Should stimulate an event from hci_transmport (evt. dispatcher) with pointer to RX buffer.
    // Execute the call to the decoder with the encoded data.
    // This should trigger an interrupt, and the data can be retrieved with sd_ble_evt_get(...)
    (void) ble_rpc_event_pkt_received(encoded_gatts_sys_attr_missing_evt, encoded_packet_length);

    // Fetch the decoded event and compare it to the expected.
    uint32_t err_code = sd_ble_evt_get((uint8_t *)(&actual_ble_evt), &actual_len);
    CU_ASSERT_INT_EQUAL(err_code, NRF_SUCCESS);

    CU_ASSERT(compare_ble_events(&expected_ble_evt, expected_len, &actual_ble_evt, actual_len) == true);

    nrf_cunit_verify_call_return();
}
CU_TEST_END


CU_TEST_START(gatts_evt_write)
{
    // Place holder for the actual decoded event.
    ble_evt_t actual_ble_evt = {{0}, {{0}}};
    uint16_t  actual_len     = sizeof(actual_ble_evt);

    /**************************** Creation of the encoded input data ****************************/
    /** The connected event as it looks on the wire (UART) */
    uint8_t encoded_gatts_write_evt[] = { BLE_RPC_PKT_EVT,
                                          BLE_GATTS_EVT_WRITE, 0,               // Evt ID
                                          0x90, 0x78,                           // Conn Handle
                                          0x56, 0x34,                           // Attribute handle
                                          0x12,                                 // Operation
                                          0x01, 0x10,                           // service uuid
                                          0x20,                                 // service uuid type
                                          0x03, 0x30,                           // char uuid
                                          0x40,                                 // char uuid type
                                          0x05, 0x50,                           // desc uuid
                                          0x60,                                 // desc uuid type
                                          0x07, 0x70,                           // Service handle
                                          0x08, 0x80,                           // Value handle
                                          0x90,                                 // Attribute type
                                          0x45, 0x54,                           // Offset
                                          0x05, 0x00,                           // Data length
                                          0x99, 0x88, 0x77, 0x66, 0x55          // Data
                                        };

    uint16_t encoded_packet_length = 31;

    /**************************** Creation of the expected event ****************************/
    /** The BLE event as it should look when it has been decoded and fetch by the application. */
    uint32_t    event_buffer[80 / sizeof(uint32_t)];
    ble_evt_t * expected_ble_evt = (ble_evt_t *)event_buffer;

    memset(event_buffer, 0, 80);

    expected_ble_evt->header.evt_id                                     = BLE_GATTS_EVT_WRITE;
    expected_ble_evt->header.evt_len                                    = 33;
    expected_ble_evt->evt.gatts_evt.conn_handle                         = 0x7890;
    expected_ble_evt->evt.gatts_evt.params.write.handle                 = 0x3456;
    expected_ble_evt->evt.gatts_evt.params.write.op                     = 0x12;

    // context
    expected_ble_evt->evt.gatts_evt.params.write.context.srvc_uuid.uuid = 0x1001;
    expected_ble_evt->evt.gatts_evt.params.write.context.srvc_uuid.type = 0x20;
    expected_ble_evt->evt.gatts_evt.params.write.context.char_uuid.uuid = 0x3003;
    expected_ble_evt->evt.gatts_evt.params.write.context.char_uuid.type = 0x40;
    expected_ble_evt->evt.gatts_evt.params.write.context.desc_uuid.uuid = 0x5005;
    expected_ble_evt->evt.gatts_evt.params.write.context.desc_uuid.type = 0x60;
    expected_ble_evt->evt.gatts_evt.params.write.context.srvc_handle    = 0x7007;
    expected_ble_evt->evt.gatts_evt.params.write.context.value_handle   = 0x8008;
    expected_ble_evt->evt.gatts_evt.params.write.context.type           = 0x90;

    expected_ble_evt->evt.gatts_evt.params.write.offset                 = 0x5445;
    expected_ble_evt->evt.gatts_evt.params.write.len                    = 0x5;

    expected_ble_evt->evt.gatts_evt.params.write.data[0] = 0x99;
    expected_ble_evt->evt.gatts_evt.params.write.data[1] = 0x88; //lint !e415 !e416
    expected_ble_evt->evt.gatts_evt.params.write.data[2] = 0x77; //lint !e415 !e416
    expected_ble_evt->evt.gatts_evt.params.write.data[3] = 0x66; //lint !e415 !e416
    expected_ble_evt->evt.gatts_evt.params.write.data[4] = 0x55; //lint !e415 !e416

    uint16_t expected_len  = (uint16_t)(offsetof(ble_evt_t, evt.gatts_evt.params.write.data));
    expected_len          += expected_ble_evt->evt.gatts_evt.params.write.len; // One byte is already counted in the struct itself.

    /**************************** Execution of the test / Verify decoder behavior ****************************/
    // Simulate an event from the transport layer. -- Should stimulate an event from hci_transmport (evt. dispatcher) with pointer to RX buffer.
    // Execute the call to the decoder with the encoded data.
    // This should trigger an interrupt, and the data can be retrieved with sd_ble_evt_get(...)
    (void) ble_rpc_event_pkt_received(encoded_gatts_write_evt, encoded_packet_length);

    // Fetch the decoded event and compare it to the expected.
    uint32_t err_code = sd_ble_evt_get((uint8_t *)(&actual_ble_evt), &actual_len);
    CU_ASSERT_INT_EQUAL(err_code, NRF_SUCCESS);

    CU_ASSERT(compare_ble_events(expected_ble_evt, expected_len, &actual_ble_evt, actual_len) == true);

    nrf_cunit_verify_call_return();
//    CU_ASSERT(pull_and_verify_ble_evt(expected_ble_evt, expected_len) == true);
}
CU_TEST_END


// This test case is intended to test that if an invalid (non-existing/unsupport) gap event is
// received, then sd_ble_evt_get(...) shall return NRF_ERROR_NOT_FOUND.
CU_TEST_START(gatts_evt_unsupported)
{
    // Place holder for the actual decoded event length, size is 2 less (Too small).
    ble_evt_t actual_ble_evt     = {{0}, {{0}}};
    uint16_t  actual_ble_evt_len = sizeof(actual_ble_evt);

    /**************************** Creation of the encoded input data ****************************/
    /** The connected event as it looks on the wire (UART) */
    uint8_t encoded_gatts_unsupported_evt[] = { BLE_RPC_PKT_EVT,
                                                (BLE_GATTS_EVT_LAST - 1), 0,          // Evt ID - Doesn't exist.
                                                0xaa, 0xbb,                           // Some data.
                                                0xcc, 0xdd                            // Some data.
                                              };

    uint16_t encoded_packet_length = 7;

    /**************************** Creation of the expected event ****************************/
    /** We only validate that length can is returned correctly, without destroying the event. */
    // NONE Needed as we verify unaligned pointer behavior.

    /**************************** Execution of the test / Verify decoder behavior ****************************/
    // Simulate an event from the transport layer. -- Should stimulate an event from hci_transmport (evt. dispatcher) with pointer to RX buffer.
    // Execute the call to the decoder with the encoded data.
    // This should trigger an interrupt, and the data can be retrieved with sd_ble_evt_get(...)
    (void) ble_rpc_event_pkt_received(encoded_gatts_unsupported_evt, encoded_packet_length);

    uint32_t err_code = sd_ble_evt_get((uint8_t *)(&actual_ble_evt), &actual_ble_evt_len);
    CU_ASSERT_INT_EQUAL(err_code, NRF_ERROR_NOT_FOUND);
    CU_ASSERT_INT_EQUAL(actual_ble_evt_len, 0);

    nrf_cunit_verify_call_return();
}
CU_TEST_END


static void event_decoder_suite_setup(void)
{
    nrf_cunit_mock_call * p_mock_obj;

    reset_test();
    nrf_cunit_reset_mock();
    // Verify the SWI2_IRQ is not set.
    CU_ASSERT(NVIC_GetPendingIRQ(SWI2_IRQn) == 0);

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_consume", &p_mock_obj);
    p_mock_obj->compare_rule[0] = COMPARE_ANY;
}


static void event_decoder_suite_teardown(void)
{
    nrf_cunit_verify_call_return();
}



CU_SUITE_START_PUBLIC(event_decoder_gatts_suite)
{
    // CUnit test setup function
    CU_SUITE_SETUP_FUNC(event_decoder_suite_setup);
    CU_SUITE_TEARDOWN_FUNC(event_decoder_suite_teardown);

    CU_SUITE_ADD_TEST(gatts_evt_sys_attr_missing);
    CU_SUITE_ADD_TEST(gatts_evt_write);
    CU_SUITE_ADD_TEST(gatts_evt_unsupported);
}
CU_SUITE_END

