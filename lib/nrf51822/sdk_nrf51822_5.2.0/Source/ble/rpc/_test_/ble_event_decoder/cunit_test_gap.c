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
#include "ble_gap.h"
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


/** @brief Function for pulling the ble event from the evt decoder and verify it against the input
 * expected event.
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


// This test case is intended to test if the connected event is decoded properly.
/*
 * Test update:
 * 1) Event will no longer be fetched by the event decoder, but will be pushed by hci_transport -> ble_rpc_dispatcher -> ble_rpc_event_decoder_x
 *    - mock calls to remove: rpc_transport_register, rpc_transport_packet_read
 * 2) Call the decoder with rx packet -
 * 3) sd_ble_evt_get(...) -- no changes in step one
 * 4) Verify packet.      -- no changes in step one
 */
CU_TEST_START(decode_connected_event)
{

    // Place holder for the actual decoded event.
    ble_evt_t actual_ble_evt = {{0}, {{0}}};
    uint16_t  actual_len     = sizeof(actual_ble_evt);

    /**************************** Creation of the encoded input data ****************************/
    /** The connected event as it looks on the wire (UART) */
    uint8_t encoded_conn_evt[] = { BLE_RPC_PKT_EVT,
                                   BLE_GAP_EVT_CONNECTED, 0,             // Connected event.
                                   0x12, 0x34,                           // Conn Handle:  0x3412
                                   0x01,                                 // Address type:
                                   0x12, 0x34, 0x56, 0x78, 0x90, 0xab,   // Address
                                   0xff,                                 // IRK Match and Index
                                   0x12, 0x34,                           // Min Connection Interval
                                   0x56, 0x78,                           // Max Connection Interval
                                   0x90, 0xab,                           // Slave latency
                                   0xcd, 0xef                            // Conn Supv timeout
                                 };

    /**************************** Creation of the expected event ****************************/
    /** The BLE event as it should look when it has been decoded and fetch by the application. */
    ble_evt_t expected_ble_evt;
    expected_ble_evt.header.evt_id                                    = BLE_GAP_EVT_CONNECTED;
    expected_ble_evt.header.evt_len                                   = 18; // Length of the following data, please note the irk_match and irk_match_idx are bit fields within the same byte.
    expected_ble_evt.evt.gap_evt.conn_handle                          = 0x3412;
    expected_ble_evt.evt.gap_evt.params.connected.peer_addr.addr_type = 0x01;
    expected_ble_evt.evt.gap_evt.params.connected.peer_addr.addr[0]   = 0x12;
    expected_ble_evt.evt.gap_evt.params.connected.peer_addr.addr[1]   = 0x34;
    expected_ble_evt.evt.gap_evt.params.connected.peer_addr.addr[2]   = 0x56;
    expected_ble_evt.evt.gap_evt.params.connected.peer_addr.addr[3]   = 0x78;
    expected_ble_evt.evt.gap_evt.params.connected.peer_addr.addr[4]   = 0x90;
    expected_ble_evt.evt.gap_evt.params.connected.peer_addr.addr[5]   = 0xab;
    expected_ble_evt.evt.gap_evt.params.connected.irk_match           = 0x01;
    expected_ble_evt.evt.gap_evt.params.connected.irk_match_idx       = 0x7f;
    expected_ble_evt.evt.gap_evt.params.connected.conn_params.min_conn_interval     = 0x3412;
    expected_ble_evt.evt.gap_evt.params.connected.conn_params.max_conn_interval     = 0x7856;
    expected_ble_evt.evt.gap_evt.params.connected.conn_params.slave_latency         = 0xab90;
    expected_ble_evt.evt.gap_evt.params.connected.conn_params.conn_sup_timeout      = 0xefcd;

    uint16_t expected_len = expected_ble_evt.header.evt_len + sizeof(ble_evt_hdr_t);

    // Verify the SWI2_IRQ is not set.
    CU_ASSERT(NVIC_GetPendingIRQ(SWI2_IRQn) == 0);

    /**************************** Execution of the test / Verify decoder behavior ****************************/
    // Simulate an event from the transport layer. -- Should stimulate an event from hci_transmport (evt. dispatcher) with pointer to RX buffer.
    // Execute the call to the decoder with the encoded data.
    // This should trigger an interrupt, and the data can be retrieved with sd_ble_evt_get(...)
    (void) ble_rpc_event_pkt_received(encoded_conn_evt, 21);

    // Fetch the decoded event and compare it to the expected.
    uint32_t err_code = sd_ble_evt_get((uint8_t *)(&actual_ble_evt), &actual_len);
    CU_ASSERT_INT_EQUAL(err_code, NRF_SUCCESS);

    CU_ASSERT(compare_ble_events(&expected_ble_evt, expected_len, &actual_ble_evt, actual_len) == true);

    nrf_cunit_verify_call_return();
}
CU_TEST_END


// This test case is intended to test if the disconnected event is decoded properly.
CU_TEST_START(decode_disconnected_event)
{
    // Place holder for the actual decoded event.
    ble_evt_t actual_ble_evt = {{0}, {{0}}};
    uint16_t  actual_len     = sizeof(actual_ble_evt);

    /**************************** Creation of the encoded input data ****************************/
    /** The connected event as it looks on the wire (UART) */
    uint8_t encoded_disconn_evt[] = { BLE_RPC_PKT_EVT,
                                      BLE_GAP_EVT_DISCONNECTED, 0,          // Evt ID
                                      0x24, 0x12,                           // Conn Handle
                                      0xff                                  // Reason
                                    };

    /**************************** Creation of the expected event ****************************/
    /** The BLE event as it should look when it has been decoded and fetch by the application. */
    ble_evt_t expected_ble_evt;
    expected_ble_evt.header.evt_id                                       = BLE_GAP_EVT_DISCONNECTED;
    expected_ble_evt.header.evt_len                                      = 3;
    expected_ble_evt.evt.gap_evt.conn_handle                             = 0x1224;
    expected_ble_evt.evt.gap_evt.params.disconnected.reason              = 0xff;

    uint16_t expected_len = expected_ble_evt.header.evt_len + sizeof(ble_evt_hdr_t);

    /**************************** Execution of the test / Verify decoder behaviour ****************************/
    // Simulate an event from the transport layer. -- Should stimulate an event from hci_transmport (evt. dispatcher) with pointer to RX buffer.
    // Execute the call to the decoder with the encoded data.
    // This should trigger an interrupt, and the data can be retrieved with sd_ble_evt_get(...)
    (void) ble_rpc_event_pkt_received(encoded_disconn_evt, 6);

    // Fetch the decoded event and compare it to the expected.
    uint32_t err_code = sd_ble_evt_get((uint8_t *)(&actual_ble_evt), &actual_len);
    CU_ASSERT_INT_EQUAL(err_code, NRF_SUCCESS);

    CU_ASSERT(compare_ble_events(&expected_ble_evt, expected_len, &actual_ble_evt, actual_len) == true);

    nrf_cunit_verify_call_return();
}
CU_TEST_END



// This test case is intended to test if the GAP timeout event is decoded properly.
CU_TEST_START(decode_gap_timeout_event)
{
    // Place holder for the actual decoded event.
    ble_evt_t actual_ble_evt = {{0}, {{0}}};
    uint16_t  actual_len     = sizeof(actual_ble_evt);

    /**************************** Creation of the encoded input data ****************************/
    /** The connected event as it looks on the wire (UART) */
    uint8_t encoded_gap_timeout_evt[] = { BLE_RPC_PKT_EVT,
                                          BLE_GAP_EVT_TIMEOUT, 0,               // Evt ID
                                          0xaa, 0xbb,                           // Conn Handle
                                          0xcc                                  // Source
                                        };

    /**************************** Creation of the expected event ****************************/
    /** The BLE event as it should look when it has been decoded and fetch by the application. */
    ble_evt_t expected_ble_evt;
    expected_ble_evt.header.evt_id                                    = BLE_GAP_EVT_TIMEOUT;
    expected_ble_evt.header.evt_len                                   = 3;
    expected_ble_evt.evt.gap_evt.conn_handle                          = 0xbbaa;
    expected_ble_evt.evt.gap_evt.params.timeout.src                   = 0xcc;

    uint16_t expected_len = expected_ble_evt.header.evt_len + sizeof(ble_evt_hdr_t);


    /**************************** Execution of the test / Verify decoder behavior ****************************/
    // Simulate an event from the transport layer. -- Should stimulate an event from hci_transmport (evt. dispatcher) with pointer to RX buffer.
    // Execute the call to the decoder with the encoded data.
    // This should trigger an interrupt, and the data can be retrieved with sd_ble_evt_get(...)
    (void) ble_rpc_event_pkt_received(encoded_gap_timeout_evt, 5);

    // Fetch the decoded event and compare it to the expected.
    uint32_t err_code = sd_ble_evt_get((uint8_t *)(&actual_ble_evt), &actual_len);
    CU_ASSERT_INT_EQUAL(err_code, NRF_SUCCESS);

    CU_ASSERT(compare_ble_events(&expected_ble_evt, expected_len, &actual_ble_evt, actual_len) == true);

    nrf_cunit_verify_call_return();
}
CU_TEST_END

// This test case is intended to test if the connection parameter update event is decoded properly.
CU_TEST_START(decode_conn_param_update_event)
{
    // Place holder for the actual decoded event.
    ble_evt_t actual_ble_evt = {{0}, {{0}}};
    uint16_t  actual_len     = sizeof(actual_ble_evt);

    /**************************** Creation of the encoded input data ****************************/
    /** The connected event as it looks on the wire (UART) */
    uint8_t encoded_conn_param_update_evt[] = { BLE_RPC_PKT_EVT,
                                                BLE_GAP_EVT_CONN_PARAM_UPDATE, 0,     // Evt ID
                                                0x12, 0x34,                           // Conn Handle
                                                0x12, 0x34,                           // Min Connection Interval
                                                0x56, 0x78,                           // Max Connection Interval
                                                0x90, 0xab,                           // Slave latency
                                                0xcd, 0xef                            // Conn Supv timeout
                                              };
    uint16_t encoded_packet_length = 17;

    /**************************** Creation of the expected event ****************************/
    /** The BLE event as it should look when it has been decoded and fetch by the application. */
    ble_evt_t expected_ble_evt;
    expected_ble_evt.header.evt_id                                                          = BLE_GAP_EVT_CONN_PARAM_UPDATE;
    expected_ble_evt.header.evt_len                                                         = 10;
    expected_ble_evt.evt.gap_evt.conn_handle                                                = 0x3412;
    expected_ble_evt.evt.gap_evt.params.conn_param_update.conn_params.min_conn_interval     = 0x3412;
    expected_ble_evt.evt.gap_evt.params.conn_param_update.conn_params.max_conn_interval     = 0x7856;
    expected_ble_evt.evt.gap_evt.params.conn_param_update.conn_params.slave_latency         = 0xab90;
    expected_ble_evt.evt.gap_evt.params.conn_param_update.conn_params.conn_sup_timeout      = 0xefcd;

    uint16_t expected_len = expected_ble_evt.header.evt_len + sizeof(ble_evt_hdr_t);

    /**************************** Execution of the test / Verify decoder behavior ****************************/
    // Simulate an event from the transport layer. -- Should stimulate an event from hci_transmport (evt. dispatcher) with pointer to RX buffer.
    // Execute the call to the decoder with the encoded data.
    // This should trigger an interrupt, and the data can be retrieved with sd_ble_evt_get(...)
    (void) ble_rpc_event_pkt_received(encoded_conn_param_update_evt, encoded_packet_length);

    // Fetch the decoded event and compare it to the expected.
    uint32_t err_code = sd_ble_evt_get((uint8_t *)(&actual_ble_evt), &actual_len);
    CU_ASSERT_INT_EQUAL(err_code, NRF_SUCCESS);

    CU_ASSERT(compare_ble_events(&expected_ble_evt, expected_len, &actual_ble_evt, actual_len));

    nrf_cunit_verify_call_return();
}
CU_TEST_END

// This test case is intended to test that we get NRF_ERROR_NOT_FOUND when there is no more
// elements to pop.
CU_TEST_START(second_pop_returns_no_more_elements_err_code)
{
    // Place holder for the actual decoded event.
    ble_evt_t actual_ble_evt = {{0}, {{0}}};
    uint16_t  actual_ble_len = sizeof(actual_ble_evt);

    /**************************** Creation of the encoded input data ****************************/
    /** The connected event as it looks on the wire (UART) */
    uint8_t encoded_conn_evt[] = { BLE_RPC_PKT_EVT,
                                   BLE_GAP_EVT_CONNECTED, 0,             // Evt ID
                                   0x12, 0x34,                           // Conn Handle
                                   0x01,                                 // Address type
                                   0x12, 0x34, 0x56, 0x78, 0x90, 0xab,   // Address
                                   0xff,                                 // IRK Match and Index
                                   0x12, 0x34,                           // Min Connection Interval
                                   0x56, 0x78,                           // Max Connection Interval
                                   0x90, 0xab,                           // Slave latency
                                   0xcd, 0xef                            // Conn Supv timeout
                                 };

    uint16_t encoded_packet_length = 21;

    /**************************** Creation of the expected event ****************************/
    /** The BLE event as it should look when it has been decoded and fetch by the application. */
    ble_evt_t expected_ble_evt;
    expected_ble_evt.header.evt_id                                    = BLE_GAP_EVT_CONNECTED;
    expected_ble_evt.header.evt_len                                   = 18;
    expected_ble_evt.evt.gap_evt.conn_handle                          = 0x3412;
    expected_ble_evt.evt.gap_evt.params.connected.peer_addr.addr_type = 0x01;
    expected_ble_evt.evt.gap_evt.params.connected.peer_addr.addr[0]   = 0x12;
    expected_ble_evt.evt.gap_evt.params.connected.peer_addr.addr[1]   = 0x34;
    expected_ble_evt.evt.gap_evt.params.connected.peer_addr.addr[2]   = 0x56;
    expected_ble_evt.evt.gap_evt.params.connected.peer_addr.addr[3]   = 0x78;
    expected_ble_evt.evt.gap_evt.params.connected.peer_addr.addr[4]   = 0x90;
    expected_ble_evt.evt.gap_evt.params.connected.peer_addr.addr[5]   = 0xab;
    expected_ble_evt.evt.gap_evt.params.connected.irk_match           = 0x01;
    expected_ble_evt.evt.gap_evt.params.connected.irk_match_idx       = 0x7f;
    expected_ble_evt.evt.gap_evt.params.connected.conn_params.min_conn_interval     = 0x3412;
    expected_ble_evt.evt.gap_evt.params.connected.conn_params.max_conn_interval     = 0x7856;
    expected_ble_evt.evt.gap_evt.params.connected.conn_params.slave_latency         = 0xab90;
    expected_ble_evt.evt.gap_evt.params.connected.conn_params.conn_sup_timeout      = 0xefcd;

    uint16_t expected_len = expected_ble_evt.header.evt_len + sizeof(ble_evt_hdr_t);

    /**************************** Execution of the test / Verify decoder behavior ****************************/
    // Simulate an event from the transport layer. -- Should stimulate an event from hci_transmport (evt. dispatcher) with pointer to RX buffer.
    // Execute the call to the decoder with the encoded data.
    // This should trigger an interrupt, and the data can be retrieved with sd_ble_evt_get(...)
    (void) ble_rpc_event_pkt_received(encoded_conn_evt, encoded_packet_length);

    // Fetch the decoded event and compare it to the expected.
    uint32_t err_code = sd_ble_evt_get((uint8_t *)(&actual_ble_evt), &actual_ble_len);
    CU_ASSERT_INT_EQUAL(err_code, NRF_SUCCESS);

    CU_ASSERT(compare_ble_events(&expected_ble_evt, expected_len, &actual_ble_evt, actual_ble_len));

    // Verify that second pop returns no more elements error code.
    ble_evt_t second_pop_event;
    uint16_t  second_pop_len = sizeof(second_pop_event);

    err_code    = sd_ble_evt_get((uint8_t *)(&second_pop_event), &second_pop_len);

    CU_ASSERT(err_code == NRF_ERROR_NOT_FOUND);
    CU_ASSERT_INT_EQUAL(second_pop_len, 0);

    nrf_cunit_verify_call_return();
}
CU_TEST_END


// This test case is intended to test that we get the event size if we provide a NULL pointer buffer
CU_TEST_START(NULL_pointer_buffer_returns_the_event_size)
{
    // Place holder for the actual decoded event length.
    uint16_t  actual_len         = 128;

    /**************************** Creation of the encoded input data ****************************/
    /** The connected event as it looks on the wire (UART) */
    uint8_t encoded_disconn_evt[] = { BLE_RPC_PKT_EVT,
                                      BLE_GAP_EVT_DISCONNECTED, 0,          // Evt ID
                                      0x24, 0x12,                           // Conn Handle
                                      0xff                                  // Reason
                                    };

    uint16_t encoded_packet_length = 6;

    /**************************** Creation of the expected event ****************************/
    /** We only validate that length can is returned correctly, without destroying the event. */
    // 3 = encoded_disconn_evt array size minus header (2 bytes).
    uint16_t expected_len = 3 + sizeof(ble_evt_hdr_t);

    /**************************** Execution of the test / Verify decoder behavior ****************************/
    // Simulate an event from the transport layer. -- Should stimulate an event from hci_transmport (evt. dispatcher) with pointer to RX buffer.
    // Execute the call to the decoder with the encoded data.
    // This should trigger an interrupt, and the data can be retrieved with sd_ble_evt_get(...)
    (void) ble_rpc_event_pkt_received(encoded_disconn_evt, encoded_packet_length);

    // Fetch the decoded event and compare it to the expected.
    uint32_t err_code     = sd_ble_evt_get(NULL, &actual_len);
    CU_ASSERT(err_code == NRF_SUCCESS);

    // Verify that actual length param has got a new value.
    CU_ASSERT(actual_len !=  128);
    // Verify that the value is as expected.
    CU_ASSERT_INT_EQUAL(actual_len, expected_len);

    nrf_cunit_verify_call_return();
}
CU_TEST_END


// This test case is intended to test that we get an NRF_ERROR_DATA_SIZE if we provide
// to small buffer
CU_TEST_START(too_small_buffer_gives_a_data_size_error)
{
    // Place holder for the actual decoded event length, size is 2 less (Too small).
    ble_evt_t actual_ble_evt = {{0}, {{0}}};
    uint16_t  actual_ble_len;

    /**************************** Creation of the encoded input data ****************************/
    /** The connected event as it looks on the wire (UART) */
    uint8_t encoded_gap_timeout_evt[] = { BLE_RPC_PKT_EVT,
                                          BLE_GAP_EVT_TIMEOUT, 0,               // Evt ID
                                          0xaa, 0xbb,                           // Conn Handle
                                          0xcc                                  // Source
                                        };

    uint16_t encoded_packet_length = 6;

    /**************************** Creation of the expected event ****************************/
    /** We only validate that length can is returned correctly, without destroying the event. */
    // 3 = encoded_gap_timeout_evt array size minus header (2 bytes).
    uint16_t expected_len = 3 + sizeof(ble_evt_hdr_t);


    /**************************** Execution of the test / Verify decoder behavior ****************************/
    // Simulate an event from the transport layer. -- Should stimulate an event from hci_transmport (evt. dispatcher) with pointer to RX buffer.
    // Execute the call to the decoder with the encoded data.
    // This should trigger an interrupt, and the data can be retrieved with sd_ble_evt_get(...)
    (void) ble_rpc_event_pkt_received(encoded_gap_timeout_evt, encoded_packet_length);

    // Fetch the decoded event and compare it to the expected.
    actual_ble_len    = expected_len - 2;
    uint32_t err_code = sd_ble_evt_get((uint8_t *)(&actual_ble_evt), &actual_ble_len);

    CU_ASSERT(err_code == NRF_ERROR_DATA_SIZE);
    CU_ASSERT_INT_EQUAL(err_code, NRF_ERROR_DATA_SIZE);

    nrf_cunit_verify_call_return();
}
CU_TEST_END


// This test case is intended to test that we get an NRF_ERROR_INVALID_ADDR if we provide
// a NULL pointer as length parameter.
CU_TEST_START(NULL_pointer_length_adress_gives_a_invalid_address_error)
{
    // Place holder for the actual decoded event length, size is 2 less (Too small).
    ble_evt_t actual_ble_evt = {{0}, {{0}}};

    /**************************** Creation of the encoded input data ****************************/
    /** The connected event as it looks on the wire (UART) */
    uint8_t encoded_gap_timeout_evt[] = { BLE_RPC_PKT_EVT,
                                          BLE_GAP_EVT_TIMEOUT, 0,               // Evt ID
                                          0xaa, 0xbb,                           // Conn Handle
                                          0xcc                                  // Source
                                        };

    uint16_t encoded_packet_length = 6;

    /**************************** Creation of the expected event ****************************/
    // NONE Needed as we verify NULL pointer behavior.

    /**************************** Execution of the test / Verify decoder behavior ****************************/
    // Simulate an event from the transport layer. -- Should stimulate an event from hci_transmport (evt. dispatcher) with pointer to RX buffer.
    // Execute the call to the decoder with the encoded data.
    // This should trigger an interrupt, and the data can be retrieved with sd_ble_evt_get(...)
    (void) ble_rpc_event_pkt_received(encoded_gap_timeout_evt, encoded_packet_length);

    uint32_t err_code = sd_ble_evt_get((uint8_t *)(&actual_ble_evt), NULL);

    CU_ASSERT(err_code == NRF_ERROR_INVALID_ADDR);
    CU_ASSERT_INT_EQUAL(err_code, NRF_ERROR_INVALID_ADDR);

    nrf_cunit_verify_call_return();
}
CU_TEST_END


// This test case is intended to test that we get an NRF_ERROR_INVALID_ADDR if we provide
// a unaligned memory as destination buffer.
CU_TEST_START(unaligned_dest_memory_gives_a_invalid_address_error)
{
    // Place holder for the actual decoded event length, size is 2 less (Too small).
    ble_evt_t actual_ble_evt    = {{0}, {{0}}};
    uint16_t  actual_ble_len    = sizeof(actual_ble_evt);

    uint8_t * un_aligned_buffer = ((uint8_t *)(&actual_ble_evt)) + 1;

    /**************************** Creation of the encoded input data ****************************/
    /** The connected event as it looks on the wire (UART) */
    uint8_t encoded_gap_timeout_evt[] = { BLE_RPC_PKT_EVT,
                                          BLE_GAP_EVT_TIMEOUT, 0,               // Evt ID
                                          0xaa, 0xbb,                           // Conn Handle
                                          0xcc                                  // Source
                                        };

    uint16_t encoded_packet_length = 6;

    /**************************** Creation of the expected event ****************************/
    /** We only validate that length can is returned correctly, without destroying the event. */
    // NONE Needed as we verify unaligned pointer behavior.

    /**************************** Execution of the test / Verify decoder behavior ****************************/
    // Simulate an event from the transport layer. -- Should stimulate an event from hci_transmport (evt. dispatcher) with pointer to RX buffer.
    // Execute the call to the decoder with the encoded data.
    // This should trigger an interrupt, and the data can be retrieved with sd_ble_evt_get(...)
    (void) ble_rpc_event_pkt_received(encoded_gap_timeout_evt, encoded_packet_length);

    uint32_t err_code = sd_ble_evt_get(un_aligned_buffer, &actual_ble_len);

    CU_ASSERT(err_code == NRF_ERROR_INVALID_ADDR);
    CU_ASSERT_INT_EQUAL(err_code, NRF_ERROR_INVALID_ADDR);

    nrf_cunit_verify_call_return();
}
CU_TEST_END


// This test case is intended to test that if an invalid (non-existing/unsupport) gap event is
// received, then sd_ble_evt_get(...) shall return NRF_ERROR_NOT_FOUND.
CU_TEST_START(gap_evt_unsupported)
{
    // Place holder for the actual decoded event length, size is 2 less (Too small).
    ble_evt_t actual_ble_evt     = {{0}, {{0}}};
    uint16_t  actual_ble_evt_len = sizeof(actual_ble_evt);

    /**************************** Creation of the encoded input data ****************************/
    /** The connected event as it looks on the wire (UART) */
    uint8_t encoded_gap_timeout_evt[] = { BLE_RPC_PKT_EVT,
                                          (BLE_GAP_EVT_LAST - 1), 0,            // Evt ID - Doesn't exist.
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
    (void) ble_rpc_event_pkt_received(encoded_gap_timeout_evt, encoded_packet_length);

    uint32_t err_code = sd_ble_evt_get((uint8_t *)(&actual_ble_evt), &actual_ble_evt_len);
    CU_ASSERT_INT_EQUAL(err_code, NRF_ERROR_NOT_FOUND);
    CU_ASSERT_INT_EQUAL(actual_ble_evt_len, 0);

    nrf_cunit_verify_call_return();
}
CU_TEST_END


CU_TEST_START(verify_that_irk_bitfields_are_decoded_correctly)
{
    nrf_cunit_mock_call * p_mock_obj;
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_consume", &p_mock_obj);
    p_mock_obj->compare_rule[0] = COMPARE_ANY;

    // Place holder for the actual decoded event length, size is 2 less (Too small).
    ble_evt_t actual_ble_evt = {{0}, {{0}}};
    uint16_t  actual_ble_len = sizeof(actual_ble_evt);

    /**************************** Creation of the encoded input data ****************************/
    /** The connected event as it looks on the wire (UART) */
    uint8_t ble_evt_pos_of_irk = 12;
    uint8_t regression_encoded_conn_evt[] = { BLE_RPC_PKT_EVT,
                                              BLE_GAP_EVT_CONNECTED, 0,             // Evt ID
                                              0x12, 0x34,                           // Conn Handle
                                              0x01,                                 // Address type
                                              0x12, 0x34, 0x56, 0x78, 0x90, 0xab,   // Address
                                              (0x18 << 1) | (0x1),                  // IRK Match and Index
                                              0x12, 0x34,                           // Min Connection Interval
                                              0x56, 0x78,                           // Max Connection Interval
                                              0x90, 0xab,                           // Slave latency
                                              0xcd, 0xef                            // Conn Supv timeout
                                            };

    CU_ASSERT_INT_EQUAL(regression_encoded_conn_evt[ble_evt_pos_of_irk], (0x18 << 1) | (0x1));
    CU_ASSERT_INT_EQUAL((0x18 << 1) | (0x1), 49); // Verify the decimal value

    uint16_t encoded_packet_length = 21;

    /**************************** Creation of the expected event ****************************/
    uint8_t ble_evt_pos_of_irk_in_padded_struct = 13;

    ble_evt_t expected_regression_encoded_conn_evt = {{0}, {{0}}};
    expected_regression_encoded_conn_evt.header.evt_id                                                  = BLE_GAP_EVT_CONNECTED;
    expected_regression_encoded_conn_evt.header.evt_len                                                 = 18;
    expected_regression_encoded_conn_evt.evt.gap_evt.conn_handle                                        = 0x3412;
    expected_regression_encoded_conn_evt.evt.gap_evt.params.connected.peer_addr.addr_type               = 0x01;
    expected_regression_encoded_conn_evt.evt.gap_evt.params.connected.peer_addr.addr[0]                 = 0x12;
    expected_regression_encoded_conn_evt.evt.gap_evt.params.connected.peer_addr.addr[1]                 = 0x34;
    expected_regression_encoded_conn_evt.evt.gap_evt.params.connected.peer_addr.addr[2]                 = 0x56;
    expected_regression_encoded_conn_evt.evt.gap_evt.params.connected.peer_addr.addr[3]                 = 0x78;
    expected_regression_encoded_conn_evt.evt.gap_evt.params.connected.peer_addr.addr[4]                 = 0x90;
    expected_regression_encoded_conn_evt.evt.gap_evt.params.connected.peer_addr.addr[5]                 = 0xab;
    expected_regression_encoded_conn_evt.evt.gap_evt.params.connected.irk_match                         = 0x01;
    expected_regression_encoded_conn_evt.evt.gap_evt.params.connected.irk_match_idx                     = 0x18;  // decimal 24
    expected_regression_encoded_conn_evt.evt.gap_evt.params.connected.conn_params.min_conn_interval     = 0x3412;
    expected_regression_encoded_conn_evt.evt.gap_evt.params.connected.conn_params.max_conn_interval     = 0x7856;
    expected_regression_encoded_conn_evt.evt.gap_evt.params.connected.conn_params.slave_latency         = 0xab90;
    expected_regression_encoded_conn_evt.evt.gap_evt.params.connected.conn_params.conn_sup_timeout      = 0xefcd;

    uint16_t expected_len  = (uint16_t)(offsetof(ble_evt_t, evt.gap_evt.params.connected));
    expected_len          += sizeof(ble_gap_evt_connected_t);

    // Verify that IRK has been set correctly
    uint8_t expected_irk_value = ((uint8_t *)&expected_regression_encoded_conn_evt)[ble_evt_pos_of_irk_in_padded_struct];
    CU_ASSERT_INT_EQUAL(expected_irk_value, 49); // should be 11000 bit-shifted one to the left, plus 0x1 (irk_match), decimal 49

    /**************************** Execution of the test / Verify decoder behavior ****************************/
    // Simulate an event from the transport layer. -- Should stimulate an event from hci_transmport (evt. dispatcher) with pointer to RX buffer.
    // Execute the call to the decoder with the encoded data.
    // This should trigger an interrupt, and the data can be retrieved with sd_ble_evt_get(...)
    (void) ble_rpc_event_pkt_received(regression_encoded_conn_evt, encoded_packet_length);

    uint32_t err_code = sd_ble_evt_get((uint8_t *)(&actual_ble_evt), &actual_ble_len);
    CU_ASSERT(err_code == NRF_SUCCESS);
    CU_ASSERT(compare_ble_events(&expected_regression_encoded_conn_evt, expected_len, &actual_ble_evt, actual_ble_len));

    nrf_cunit_verify_call_return();
}
CU_TEST_END


CU_TEST_START(gap_evt_sec_params_request)
{
    // Place holder for the actual decoded event length, size is 2 less (Too small).
    ble_evt_t actual_ble_evt = {{0}, {{0}}};
    uint16_t  actual_ble_len = sizeof(actual_ble_evt);

    /**************************** Creation of the encoded input data ****************************/
    /** The connected event as it looks on the wire (UART) */
    uint8_t encoded_sec_params_request_evt[] = { BLE_RPC_PKT_EVT,
                                                 BLE_GAP_EVT_SEC_PARAMS_REQUEST, 0,    // Evt ID
                                                 0x34, 0x56,                           // Conn Handle
                                                 0xef, 0xce,                           // timeout
                                                 0x2B,                                 // (oob) << 5 | (io_caps) << 2 | (mitm) << 1 | (bond)
                                                 9,                                    // min key size
                                                 12                                    // max key size
                                               };

    uint16_t encoded_packet_length = 8;

    /**************************** Creation of the expected event ****************************/
    ble_evt_t expected_ble_evt = {{0}, {{0}}};
    expected_ble_evt.header.evt_id           = BLE_GAP_EVT_SEC_PARAMS_REQUEST;
    expected_ble_evt.header.evt_len          = 8;
    expected_ble_evt.evt.gap_evt.conn_handle = 0x5634;
    expected_ble_evt.evt.gap_evt.params.sec_params_request.peer_params.timeout      = 0xCEEF,
    expected_ble_evt.evt.gap_evt.params.sec_params_request.peer_params.bond         = 0x1;
    expected_ble_evt.evt.gap_evt.params.sec_params_request.peer_params.mitm         = 0x1;
    expected_ble_evt.evt.gap_evt.params.sec_params_request.peer_params.io_caps      = 0x2;
    expected_ble_evt.evt.gap_evt.params.sec_params_request.peer_params.oob          = 0x1;
    expected_ble_evt.evt.gap_evt.params.sec_params_request.peer_params.min_key_size = 9;
    expected_ble_evt.evt.gap_evt.params.sec_params_request.peer_params.max_key_size = 12;

    uint16_t expected_len  = (uint16_t)(offsetof(ble_evt_t, evt.gap_evt.params.sec_params_request));
    expected_len          += sizeof(ble_gap_evt_sec_params_request_t);

    /**************************** Execution of the test / Verify decoder behavior ****************************/
    // Simulate an event from the transport layer. -- Should stimulate an event from hci_transmport (evt. dispatcher) with pointer to RX buffer.
    // Execute the call to the decoder with the encoded data.
    // This should trigger an interrupt, and the data can be retrieved with sd_ble_evt_get(...)
    (void) ble_rpc_event_pkt_received(encoded_sec_params_request_evt, encoded_packet_length);

    uint32_t err_code = sd_ble_evt_get((uint8_t *)(&actual_ble_evt), &actual_ble_len);
    CU_ASSERT(err_code == NRF_SUCCESS);
    CU_ASSERT(compare_ble_events(&expected_ble_evt, expected_len, &actual_ble_evt, actual_ble_len));

    nrf_cunit_verify_call_return();
}
CU_TEST_END


CU_TEST_START(gap_evt_sec_info_request)
{
    // Place holder for the actual decoded event length, size is 2 less (Too small).
    ble_evt_t actual_ble_evt = {{0}, {{0}}};
    uint16_t  actual_ble_len = sizeof(actual_ble_evt);

    /**************************** Creation of the encoded input data ****************************/
    /** The connected event as it looks on the wire (UART) */
    uint8_t encoded_sec_info_request_evt[] = { BLE_RPC_PKT_EVT,
                                               BLE_GAP_EVT_SEC_INFO_REQUEST, 0,      // Evt ID
                                               0x37, 0x56,                           // Conn Handle
                                               0x02,                                 // addr_type
                                               0x9, 0x8, 0x7, 0x6, 0x5, 0x4,         // addr
                                               0x22, 0x11,                           // LTK deversifier
                                               0x05                                  // (sign_info) << 2 | (id_info) << 1 | (enc_info) << 0
                                             };

    uint16_t encoded_packet_length = 15;

    /**************************** Creation of the expected event ****************************/
    ble_evt_t expected_ble_evt = {{0}, {{0}}};
    expected_ble_evt.header.evt_id           = BLE_GAP_EVT_SEC_INFO_REQUEST;
    expected_ble_evt.header.evt_len          = 14;
    expected_ble_evt.evt.gap_evt.conn_handle = 0x5637;
    expected_ble_evt.evt.gap_evt.params.sec_info_request.peer_addr.addr_type = BLE_GAP_ADDR_TYPE_RANDOM_PRIVATE_RESOLVABLE;
    expected_ble_evt.evt.gap_evt.params.sec_info_request.peer_addr.addr[0] = 0x9;
    expected_ble_evt.evt.gap_evt.params.sec_info_request.peer_addr.addr[1] = 0x8;
    expected_ble_evt.evt.gap_evt.params.sec_info_request.peer_addr.addr[2] = 0x7;
    expected_ble_evt.evt.gap_evt.params.sec_info_request.peer_addr.addr[3] = 0x6;
    expected_ble_evt.evt.gap_evt.params.sec_info_request.peer_addr.addr[4] = 0x5;
    expected_ble_evt.evt.gap_evt.params.sec_info_request.peer_addr.addr[5] = 0x4;
    expected_ble_evt.evt.gap_evt.params.sec_info_request.div = 0x1122;
    expected_ble_evt.evt.gap_evt.params.sec_info_request.enc_info = 1;
    expected_ble_evt.evt.gap_evt.params.sec_info_request.id_info = 0;
    expected_ble_evt.evt.gap_evt.params.sec_info_request.sign_info = 1;

    uint16_t expected_len  = (uint16_t)(offsetof(ble_evt_t, evt.gap_evt.params.sec_info_request));
    expected_len          += sizeof(ble_gap_evt_sec_info_request_t);

    /**************************** Execution of the test / Verify decoder behavior ****************************/
    // Simulate an event from the transport layer. -- Should stimulate an event from hci_transmport (evt. dispatcher) with pointer to RX buffer.
    // Execute the call to the decoder with the encoded data.
    // This should trigger an interrupt, and the data can be retrieved with sd_ble_evt_get(...)
    (void) ble_rpc_event_pkt_received(encoded_sec_info_request_evt, encoded_packet_length);

    uint32_t err_code = sd_ble_evt_get((uint8_t *)(&actual_ble_evt), &actual_ble_len);
    CU_ASSERT(err_code == NRF_SUCCESS);
    CU_ASSERT(compare_ble_events(&expected_ble_evt, expected_len, &actual_ble_evt, actual_ble_len));

    nrf_cunit_verify_call_return();
}
CU_TEST_END


CU_TEST_START(gap_evt_conn_sec_update)
{
    // Place holder for the actual decoded event length, size is 2 less (Too small).
    ble_evt_t actual_ble_evt = {{0}, {{0}}};
    uint16_t  actual_ble_len = sizeof(actual_ble_evt);

    /**************************** Creation of the encoded input data ****************************/
    /** The connected event as it looks on the wire (UART) */
    uint8_t encoded_conn_sec_update_evt[] = { BLE_RPC_PKT_EVT,
                                              BLE_GAP_EVT_CONN_SEC_UPDATE, 0,       // Evt ID
                                              0x88, 0x99,                           // Conn Handle
                                              0x12,                                 // (lv << 4) | (sm << 0)
                                              9                                     // active encryption key size
                                            };

    uint16_t encoded_packet_length = 7;

    /**************************** Creation of the expected event ****************************/
    ble_evt_t expected_ble_evt = {{0}, {{0}}};
    expected_ble_evt.header.evt_id           = BLE_GAP_EVT_CONN_SEC_UPDATE;
    expected_ble_evt.header.evt_len          = 4;
    expected_ble_evt.evt.gap_evt.conn_handle = 0x9988;
    expected_ble_evt.evt.gap_evt.params.conn_sec_update.conn_sec.sec_mode.sm = 2;
    expected_ble_evt.evt.gap_evt.params.conn_sec_update.conn_sec.sec_mode.lv = 1;
    expected_ble_evt.evt.gap_evt.params.conn_sec_update.conn_sec.encr_key_size = 9;

    uint16_t expected_len  = (uint16_t)(offsetof(ble_evt_t, evt.gap_evt.params.conn_sec_update));
    expected_len          += sizeof(ble_gap_evt_conn_sec_update_t);

    /**************************** Execution of the test / Verify decoder behavior ****************************/
    // Simulate an event from the transport layer. -- Should stimulate an event from hci_transmport (evt. dispatcher) with pointer to RX buffer.
    // Execute the call to the decoder with the encoded data.
    // This should trigger an interrupt, and the data can be retrieved with sd_ble_evt_get(...)
    (void) ble_rpc_event_pkt_received(encoded_conn_sec_update_evt, encoded_packet_length);

    uint32_t err_code = sd_ble_evt_get((uint8_t *)(&actual_ble_evt), &actual_ble_len);
    CU_ASSERT(err_code == NRF_SUCCESS);
    CU_ASSERT(compare_ble_events(&expected_ble_evt, expected_len, &actual_ble_evt, actual_ble_len));

    nrf_cunit_verify_call_return();
}
CU_TEST_END


CU_TEST_START(gap_evt_auth_status)
{
    // Place holder for the actual decoded event length, size is 2 less (Too small).
    ble_evt_t actual_ble_evt = {{0}, {{0}}};
    uint16_t  actual_ble_len = sizeof(actual_ble_evt);

    /**************************** Creation of the encoded input data ****************************/
    /** The connected event as it looks on the wire (UART) */
    uint8_t encoded_auth_status_evt[] = { BLE_RPC_PKT_EVT,
                                          BLE_GAP_EVT_AUTH_STATUS, 0,                           // Evt ID
                                          0xDA, 0xBC,                                           // Conn Handle
                                          0xC1,                                                 // Auth Status
                                          0xC2,                                                 // Error Source
                                          0x2A,                                                 // sm1 and sm2 levels 0x00101010
                                          0x15,                                                 // periph_kex
                                          0x15,                                                 // central_kex
                                          0x21, 0x43,                                           // Periph keys encode info
                                          0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, // ltk key
                                          0xAB,                                                 // enc info
                                          0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, // irk
                                          9,                                                    // address type
                                          0,1,2,3,4,5                                           // address
                                         };

    uint16_t encoded_packet_length = 50;

    /**************************** Creation of the expected event ****************************/
    ble_evt_t expected_ble_evt = {{0}, {{0}}};
    expected_ble_evt.header.evt_id = BLE_GAP_EVT_AUTH_STATUS;
    expected_ble_evt.header.evt_len= 52;

    expected_ble_evt.evt.gap_evt.conn_handle                       = 0xBCDA;
    expected_ble_evt.evt.gap_evt.params.auth_status.auth_status    = 0xC1;
    expected_ble_evt.evt.gap_evt.params.auth_status.error_src      = 0xC2;
    expected_ble_evt.evt.gap_evt.params.auth_status.sm1_levels.lv1 = 1;
    expected_ble_evt.evt.gap_evt.params.auth_status.sm1_levels.lv2 = 0;
    expected_ble_evt.evt.gap_evt.params.auth_status.sm1_levels.lv3 = 1;
    expected_ble_evt.evt.gap_evt.params.auth_status.sm2_levels.lv1 = 0;
    expected_ble_evt.evt.gap_evt.params.auth_status.sm2_levels.lv2 = 1;
    expected_ble_evt.evt.gap_evt.params.auth_status.sm2_levels.lv3 = 0;

    expected_ble_evt.evt.gap_evt.params.auth_status.periph_kex.ltk         = 1;
    expected_ble_evt.evt.gap_evt.params.auth_status.periph_kex.ediv_rand   = 0;
    expected_ble_evt.evt.gap_evt.params.auth_status.periph_kex.irk         = 1;
    expected_ble_evt.evt.gap_evt.params.auth_status.periph_kex.address     = 0;
    expected_ble_evt.evt.gap_evt.params.auth_status.periph_kex.csrk        = 1;

    expected_ble_evt.evt.gap_evt.params.auth_status.central_kex.ltk        = 1;
    expected_ble_evt.evt.gap_evt.params.auth_status.central_kex.ediv_rand  = 0;
    expected_ble_evt.evt.gap_evt.params.auth_status.central_kex.irk        = 1;
    expected_ble_evt.evt.gap_evt.params.auth_status.central_kex.address    = 0;
    expected_ble_evt.evt.gap_evt.params.auth_status.central_kex.csrk       = 1;

    expected_ble_evt.evt.gap_evt.params.auth_status.periph_keys.enc_info.div = 0x4321;

    uint32_t i = 0;
    for (i = 0; i < BLE_GAP_SEC_KEY_LEN; i++)
    {
        expected_ble_evt.evt.gap_evt.params.auth_status.periph_keys.enc_info.ltk[i] = i;
    }

    expected_ble_evt.evt.gap_evt.params.auth_status.periph_keys.enc_info.auth    = 1;
    expected_ble_evt.evt.gap_evt.params.auth_status.periph_keys.enc_info.ltk_len = 0x55;

    for (i = 0; i < BLE_GAP_SEC_KEY_LEN; i++)
    {
        expected_ble_evt.evt.gap_evt.params.auth_status.central_keys.irk.irk[i] = i;
    }

    expected_ble_evt.evt.gap_evt.params.auth_status.central_keys.id_info.addr_type = 9;

    for (i = 0; i < BLE_GAP_ADDR_LEN; i++)
    {
        expected_ble_evt.evt.gap_evt.params.auth_status.central_keys.id_info.addr[i] = i;
    }

    uint16_t expected_len  = (uint16_t)(offsetof(ble_evt_t, evt.gap_evt.params.auth_status));
    expected_len          += sizeof(ble_gap_evt_auth_status_t);

    /**************************** Execution of the test / Verify decoder behavior ****************************/
    // Simulate an event from the transport layer. -- Should stimulate an event from hci_transmport (evt. dispatcher) with pointer to RX buffer.
    // Execute the call to the decoder with the encoded data.
    // This should trigger an interrupt, and the data can be retrieved with sd_ble_evt_get(...)
    (void) ble_rpc_event_pkt_received(encoded_auth_status_evt, encoded_packet_length);

    uint32_t err_code = sd_ble_evt_get((uint8_t *)(&actual_ble_evt), &actual_ble_len);
    CU_ASSERT(err_code == NRF_SUCCESS);
    CU_ASSERT(compare_ble_events(&expected_ble_evt, expected_len, &actual_ble_evt, actual_ble_len));

    nrf_cunit_verify_call_return();
}
CU_TEST_END


static void setup_func_pkt_read(void)
{
    nrf_cunit_mock_call * p_mock_obj;

    nrf_cunit_reset_mock();
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_consume", &p_mock_obj);
    p_mock_obj->compare_rule[0] = COMPARE_ANY;
}


CU_SUITE_START_PUBLIC(test_suite_sd_ble_event_get)
{
    CU_SUITE_SETUP_FUNC(setup_func_pkt_read);

    CU_SUITE_ADD_TEST(decode_connected_event);
    CU_SUITE_ADD_TEST(decode_disconnected_event);
    CU_SUITE_ADD_TEST(decode_gap_timeout_event);
    CU_SUITE_ADD_TEST(decode_conn_param_update_event);
    CU_SUITE_ADD_TEST(second_pop_returns_no_more_elements_err_code);
    CU_SUITE_ADD_TEST(gap_evt_unsupported);
}
CU_SUITE_END


static void setup_func(void)
{
    nrf_cunit_reset_mock();
}

static void teardown_func(void)
{
    // Fetch the decoded event and compare it to the expected.
    ble_evt_t actual_ble_evt     = {{0}, {{0}}};
    uint16_t  actual_ble_evt_len = sizeof(actual_ble_evt);
    uint16_t  length             = actual_ble_evt_len;

    uint32_t err_code            = sd_ble_evt_get(NULL, &length);

    if (err_code != NRF_ERROR_NOT_FOUND)
    {
        nrf_cunit_mock_call * p_mock_obj;
        nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_consume", &p_mock_obj);
        p_mock_obj->compare_rule[0] = COMPARE_ANY;

        (void) sd_ble_evt_get((uint8_t *)(&actual_ble_evt), &actual_ble_evt_len);
    }

    nrf_cunit_verify_call_return();
}



/**
 * This test suite will validate behaviour when events are not fetched, such as invalid pointer,
 * memory alignment, fetching event size (event pointer NULL), etc.
 */
CU_SUITE_START_PUBLIC(test_suite_sd_ble_event_get_no_event_returned)
{
    CU_SUITE_SETUP_FUNC(setup_func);
    CU_SUITE_TEARDOWN_FUNC(teardown_func);

    CU_SUITE_ADD_TEST(NULL_pointer_buffer_returns_the_event_size);
    CU_SUITE_ADD_TEST(too_small_buffer_gives_a_data_size_error);
    CU_SUITE_ADD_TEST(NULL_pointer_length_adress_gives_a_invalid_address_error);
    CU_SUITE_ADD_TEST(unaligned_dest_memory_gives_a_invalid_address_error);
}
CU_SUITE_END


CU_SUITE_START_PUBLIC(test_suite_gap_regression)
{
    CU_SUITE_ADD_TEST(verify_that_irk_bitfields_are_decoded_correctly);
}
CU_SUITE_END


static void event_decoder_suite_setup(void)
{
    nrf_cunit_mock_call * p_mock_obj;

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


CU_SUITE_START_PUBLIC(event_decoder_gap_suite)
{
    // CUnit test setup function
    CU_SUITE_SETUP_FUNC(event_decoder_suite_setup);
    CU_SUITE_TEARDOWN_FUNC(event_decoder_suite_teardown);

    CU_SUITE_ADD_TEST(gap_evt_sec_params_request);
    CU_SUITE_ADD_TEST(gap_evt_auth_status);
    CU_SUITE_ADD_TEST(gap_evt_sec_info_request);
    CU_SUITE_ADD_TEST(gap_evt_conn_sec_update);
}
CU_SUITE_END
