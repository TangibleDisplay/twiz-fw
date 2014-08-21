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

#include "nrf_cunit.h"
#include "nrf_cunit_mock.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include "ble_rpc_event_decoder.h"
#include "ble_rpc_defines.h"
#include "nrf.h"
#include "nrf51.h"
#include "nrf_error.h"
#include "ble.h"
#include "ble_gap.h"
#include "ble_gatts.h"

//lint -e526 -e628 -e516 // Symbol '__INTADDR__()' not defined
                         // no argument information provided for function '__INTADDR__()'
                         // Symbol '__INTADDR__()' has arg. type conflict

NVIC_Type _nvic;

//@todo: remove lint suppression after completing implementation
//lint -e551 -save
static uint8_t m_wfe_count;
static uint8_t m_sev_count; 
//lint -restore

uint32_t sd_nvic_EnableIRQ(IRQn_Type IRQn)
{
    // Enable the IRQ
    NVIC_EnableIRQ(IRQn);

    return NRF_SUCCESS;
}


static void reset_test(void)
{
    m_wfe_count = 0;
    m_sev_count = 0;
}

void __WFE(void)
{
    m_wfe_count++;
}


void __SEV(void)
{
    m_sev_count++;
}


NVIC_Type * get_nvic(void)
{
    return &_nvic;
}


// Test case that will verify that after an event packet is received and fetched by the application
// using sd_ble_evt_get(...) then the packet buffer is released by calling
// hci_transport_rx_pkt_consume(...) in the HCI transport layer.
CU_TEST_START(generic_evt_packet_consume)
{
    reset_test();
    nrf_cunit_reset_mock();
    /****************************  place holders for event get ****************************/
    // Place holder for the actual decoded event length, size is 2 less (Too small).
    ble_evt_t actual_ble_evt     = {{0}, {{0}}};
    uint16_t  actual_ble_evt_len = sizeof(actual_ble_evt);

    /**************************** Creation of the encoded input data ****************************/
    /** The connected event as it looks on the wire (UART) */
    uint8_t encoded_gap_timeout_evt[] = { BLE_RPC_PKT_EVT,
                                          BLE_GAP_EVT_TIMEOUT, 0,               // Evt ID
                                          0xaa, 0xbb,                           // Conn Handle
                                          0xcc                                  // Source
                                        };

    uint16_t encoded_packet_length = 6;

    /***************** Expected mock call when sd_ble_evt_get(...) is called *****************/
    nrf_cunit_mock_call * p_mock_obj;
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_consume", &p_mock_obj);
    p_mock_obj->arg[0] = (uint32_t)encoded_gap_timeout_evt;

    /**************************** pre-condition / No Verification here. ****************************/
    // Pre-condition - SWI2_IRQn should not be active.
    CU_ASSERT(NVIC_GetPendingIRQ(SWI2_IRQn) == 0);
    // Precondition
    CU_ASSERT(sd_nvic_EnableIRQ(SWI2_IRQn) == NRF_SUCCESS);

    /**************************** Execution of the test / Verify decoder behavior ****************************/
    // Simulate an event from the transport layer. -- Should stimulate an event from hci_transmport (evt. dispatcher) with pointer to RX buffer.
    // Execute the call to the decoder with the encoded data.
    // This should trigger an interrupt, and the data can be retrieved with sd_ble_evt_get(...)
    (void) ble_rpc_event_pkt_received(encoded_gap_timeout_evt, encoded_packet_length);

    uint32_t err_code = sd_ble_evt_get((uint8_t *)(&actual_ble_evt), &actual_ble_evt_len);
    CU_ASSERT_INT_EQUAL(err_code, NRF_SUCCESS);
    // Just verify the header id. Verification of correct decoding is done in other unit tests.
    CU_ASSERT_INT_EQUAL(actual_ble_evt.header.evt_id, BLE_GAP_EVT_TIMEOUT);


    nrf_cunit_verify_call_return();
}
CU_TEST_END


// Test case that will verify that if a event packet is received but it is not GAP or GATTS, but
// another unsupported/invalid event, then sd_ble_evt_get(...) return NRF_ERROR_NOT_FOUND and zero
// length data.
CU_TEST_START(generic_evt_unsupported)
{
    reset_test();
    nrf_cunit_reset_mock();

    /**************************** Creation of the encoded input data ****************************/
    /** The connected event as it looks on the wire (UART) */
    uint8_t encoded_l2cap_unsupported[] = { BLE_RPC_PKT_EVT,
                                            BLE_L2CAP_EVT_BASE, 0,                // Evt ID - L2CAP base - L2CAP events are currently uynsupported.
                                            0xaa, 0xbb,                           // Some data.
                                            0xcc, 0xdd                            // Some data.
                                          };

    uint16_t encoded_packet_length = 6;

    /***************** Expected mock call when sd_ble_evt_get(...) is called *****************/
    nrf_cunit_mock_call * p_mock_obj;
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_consume", &p_mock_obj);
    p_mock_obj->arg[0] = (uint32_t)encoded_l2cap_unsupported;

    // Place holder for the actual decoded event length, size is 2 less (Too small).
    ble_evt_t actual_ble_evt     = {{0}, {{0}}};
    uint16_t  actual_ble_evt_len = sizeof(actual_ble_evt);

    /**************************** Creation of the expected event ****************************/
    /** We only validate that length can is returned correctly, without destroying the event. */
    // NONE Needed as we verify unaligned pointer behavior.

    /**************************** Execution of the test / Verify decoder behavior ****************************/
    // Simulate an event from the transport layer. -- Should stimulate an event from hci_transmport (evt. dispatcher) with pointer to RX buffer.
    // Execute the call to the decoder with the encoded data.
    // This should trigger an interrupt, and the data can be retrieved with sd_ble_evt_get(...)
    (void) ble_rpc_event_pkt_received(encoded_l2cap_unsupported, encoded_packet_length);

    uint32_t err_code = sd_ble_evt_get((uint8_t *)(&actual_ble_evt), &actual_ble_evt_len);
    CU_ASSERT_INT_EQUAL(err_code, NRF_ERROR_NOT_FOUND);
    CU_ASSERT_INT_EQUAL(actual_ble_evt_len, 0);

    nrf_cunit_verify_call_return();
}
CU_TEST_END


// Test case that will verify that in case multiple events are received, then they will be queued
// even if sd_ble_evt_get(...) has not yet been executed.
// Thus we will call ble_rpc_event_pkt_received multiple times before calling sd_ble_evt_get(...)
// and no event packet must be lost.
// hci_transport_rx_pkt_consume(...) in the HCI transport layer is expected too be called in the
// order of which the packets are processed.
CU_TEST_START(generic_evt_packet_queue)
{
    reset_test();
    nrf_cunit_reset_mock();
    /****************************  place holders for event get ****************************/
    // Place holder for the actual decoded event length, size is 2 less (Too small).
    ble_evt_t actual_ble_evt     = {{0}, {{0}}};
    uint16_t  actual_ble_evt_len = sizeof(actual_ble_evt);

    /**************************** Creation of the encoded input data ****************************/
    /** The connected event as it looks on the wire (UART) */
    uint8_t encoded_gap_timeout_evt[] = { BLE_RPC_PKT_EVT,
                                          BLE_GAP_EVT_TIMEOUT, 0,               // Evt ID
                                          0xaa, 0xbb,                           // Conn Handle
                                          0xcc                                  // Source
                                        };

    uint16_t encoded_packet_length = 6;

    /** The connected event as it looks on the wire (UART) */
    uint8_t encoded_disconn_evt[] = { BLE_RPC_PKT_EVT,
                                      BLE_GAP_EVT_DISCONNECTED, 0,          // Evt ID
                                      0x24, 0x12,                           // Conn Handle
                                      0x10,                                 // Address type
                                      0x34, 0x56, 0x78, 0x90, 0xab, 0x12,   // Address
                                      0xff                                  // Reason
                                    };

    uint16_t encoded_disconn_packet_length = 13;

    /***************** Expected mock call when sd_ble_evt_get(...) is called *****************/
    nrf_cunit_mock_call * p_mock_obj;
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_consume", &p_mock_obj);
    p_mock_obj->arg[0] = (uint32_t)encoded_gap_timeout_evt;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_consume", &p_mock_obj);
    p_mock_obj->arg[0] = (uint32_t)encoded_disconn_evt;

    /**************************** Execution of the test / Verify decoder behavior ****************************/
    // Simulate an event from the transport layer. -- Should stimulate an event from hci_transmport (evt. dispatcher) with pointer to RX buffer.
    // Execute the call to the decoder with the encoded data.
    // This should trigger an interrupt, and the data can be retrieved with sd_ble_evt_get(...)
    (void) ble_rpc_event_pkt_received(encoded_gap_timeout_evt, encoded_packet_length);

    (void) ble_rpc_event_pkt_received(encoded_disconn_evt, encoded_disconn_packet_length);

    uint32_t err_code = sd_ble_evt_get((uint8_t *)(&actual_ble_evt), &actual_ble_evt_len);
    CU_ASSERT_INT_EQUAL(err_code, NRF_SUCCESS);

    // Just verify the header id. Verification of correct decoding is done in other unit tests.
    CU_ASSERT_INT_EQUAL(actual_ble_evt.header.evt_id, BLE_GAP_EVT_TIMEOUT);


    actual_ble_evt_len = sizeof(actual_ble_evt);
    err_code           = sd_ble_evt_get((uint8_t *)(&actual_ble_evt), &actual_ble_evt_len);
    CU_ASSERT_INT_EQUAL(err_code, NRF_SUCCESS);

    // Just verify the header id. Verification of correct decoding is done in other unit tests.
    CU_ASSERT_INT_EQUAL(actual_ble_evt.header.evt_id, BLE_GAP_EVT_DISCONNECTED);

    nrf_cunit_verify_call_return();
}
CU_TEST_END


CU_SUITE_START(test_suite_common)
{
    CU_SUITE_ADD_TEST(generic_evt_unsupported);
    CU_SUITE_ADD_TEST(generic_evt_packet_consume);
    CU_SUITE_ADD_TEST(generic_evt_packet_queue);
}
CU_SUITE_END

CU_REGISTRY_START_STANDALONE(TC_CT_FUN_BV_UNIT_EVENT_DECODE)
{
    CU_REGISTRY_ADD_SUITE_EXTERN(test_suite_common);

    CU_REGISTRY_ADD_SUITE_EXTERN(test_suite_sd_ble_event_get);
    CU_REGISTRY_ADD_SUITE_EXTERN(test_suite_sd_ble_event_get_no_event_returned);
    CU_REGISTRY_ADD_SUITE_EXTERN(test_suite_gap_regression);
    CU_REGISTRY_ADD_SUITE_EXTERN(event_decoder_gap_suite);
    CU_REGISTRY_ADD_SUITE_EXTERN(event_decoder_gatts_suite);
}
CU_REGISTRY_END


int main(void)
{
    CU_ADD_REGISTRY(TC_CT_FUN_BV_UNIT_EVENT_DECODE);
    return 0;
}
