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
#include "ble_rpc_pkt_receiver.h"
#include "ble_rpc_defines.h"
#include "ble_gap.h"
#include "hal_transport.h"

#include "app_scheduler.h"

static uint32_t m_app_error_code;
static uint32_t m_app_error_handler_called;


void ble_rpc_pkt_handle(void * p_event_data, uint16_t event_size);


void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    m_app_error_handler_called  = true;
    m_app_error_code            = error_code;
}

void setup_func(void)
{
    m_app_error_code            = NRF_SUCCESS;
    m_app_error_handler_called  = false;
    nrf_cunit_reset_mock();
}


void teardown_func(void)
{
    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();
}


// Test case to test the init of pkt receiver. It will test if the packet receiver successfully
// registers with Transport Layer.
CU_TEST_START(pkt_rcv_register_with_transport_layer)
{
    uint32_t err_code;

    // ----------------------------------------------------------------------------------------

    nrf_cunit_mock_call * p_mock_call;

    // Expect the open transport function to be called in the transport layer module.
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_open", &p_mock_call);

    nrf_cunit_mock_call * p_transport_register_mock;
    // Expect the register function to be called in the transport layer module.
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_evt_handler_reg",
                                  &p_transport_register_mock);


    // ----------------------------------------------------------------------------------------

    err_code = ble_rpc_pkt_receiver_init();
    CU_ASSERT_INT_EQUAL(err_code, NRF_SUCCESS);

    hci_transport_event_handler_t hci_transport_event_handler;
    hci_transport_event_handler = (hci_transport_event_handler_t)
                                (p_transport_register_mock->arg_actual[0]);

    CU_ASSERT(hci_transport_event_handler != NULL);
    if (hci_transport_event_handler == NULL)
    {
        CU_MESSAGE("No transport event handler was registered by the module under test.")
        return;
    }
}
CU_TEST_END


// Test case to check if the pkt receiver does NOT call the registration function if the
// hci_transport_open returns failure.
CU_TEST_START(pkt_rcv_register_with_transport_layer_open_fail)
{
    uint32_t err_code;

    // ----------------------------------------------------------------------------------------

    nrf_cunit_mock_call * p_mock_call;

    // Expect the open transport function to be called in the transport layer module.
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_open", &p_mock_call);
    p_mock_call->error = NRF_ERROR_INVALID_STATE;

    // ----------------------------------------------------------------------------------------

    err_code = ble_rpc_pkt_receiver_init();
    CU_ASSERT_INT_EQUAL(err_code, NRF_ERROR_INVALID_STATE);
}
CU_TEST_END


// Test case to check if the pkt receiver calls tranport_close when the registration fails and also
// propagates the original error back to the caller.
CU_TEST_START(pkt_rcv_register_with_transport_layer_register_fail)
{
    uint32_t err_code;

    // ----------------------------------------------------------------------------------------

    nrf_cunit_mock_call * p_mock_call;

    // Expect the open transport function to be called in the transport layer module.
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_open", &p_mock_call);

    // Expect the register function to be called in the transport layer module.
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_evt_handler_reg", &p_mock_call);
    p_mock_call->error = NRF_ERROR_NULL;

    // Expect the close function to be called in the transport layer module.
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_close", &p_mock_call);

    // ----------------------------------------------------------------------------------------

    err_code = ble_rpc_pkt_receiver_init();
    CU_ASSERT_INT_EQUAL(err_code, NRF_ERROR_NULL);

}
CU_TEST_END


// Test case to check if an event is received, the handler of the event_decoder module is called
// when an BLE event packet is sent to the pkt receiver module.
CU_TEST_START(pkt_rcv_calling_event_decoder_handler)
{
    uint32_t err_code;

    // ----------------------------------------------------------------------------------------
    // Prepare stimuli

    uint8_t sample_encoded_conn_evt[] =
                                {
                                 BLE_RPC_PKT_EVT,                      // Packet Type
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

    // ----------------------------------------------------------------------------------------
    // Prepare the expected output

    nrf_cunit_mock_call * p_transport_mock;

    // Expect the open transport function to be called in the transport layer module.
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_open", &p_transport_mock);

    nrf_cunit_mock_call * p_transport_register_mock;
    // Expect the register function to be called in the transport layer module.
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_evt_handler_reg", &p_transport_register_mock);

    nrf_cunit_mock_call * p_transport_extract_mock;
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_extract", &p_transport_extract_mock);
    p_transport_extract_mock->result[0] = (uint32_t)sample_encoded_conn_evt;
    p_transport_extract_mock->result[1] = sizeof(sample_encoded_conn_evt);

    // Setup event decoder mock
    nrf_cunit_mock_call * p_evt_dec_mock;

    /** When a packet is received, then we expect it to be passed to the ble_rpc_event_decoder.
     *  and thereafter, we expect the handling of the event to be scheduled in app_eventscheduler.
     */
    nrf_cunit_expect_call_return((uint8_t *) "ble_rpc_event_pkt_received", &p_evt_dec_mock);
    p_evt_dec_mock->arg[0] = (uint32_t)sample_encoded_conn_evt;
    p_evt_dec_mock->arg[1] = sizeof(sample_encoded_conn_evt);

    nrf_cunit_expect_call_return((uint8_t *) "app_sched_event_put", &p_evt_dec_mock);
    p_evt_dec_mock->compare_rule[2] = COMPARE_NOT_NULL;
    // ----------------------------------------------------------------------------------------

    err_code = ble_rpc_pkt_receiver_init();
    CU_ASSERT_INT_EQUAL(err_code, NRF_SUCCESS);

    hci_transport_event_handler_t transport_event_handler;
    transport_event_handler = (hci_transport_event_handler_t)
                                (p_transport_register_mock->arg_actual[0]);

    CU_ASSERT(transport_event_handler != NULL);
    if (transport_event_handler == NULL)
    {
        return;
    }

    hci_transport_evt_t hci_evt;
    hci_evt.evt_type = HCI_TRANSPORT_RX_RDY;

    transport_event_handler(hci_evt);

    // Verify that application error handler was not called.
    CU_ASSERT_INT_EQUAL(m_app_error_handler_called, false);
}
CU_TEST_END


// Test case to check if an event is received, the handler of the command_encoder module is called
// when an BLE command response packet is sent to the pkt receiver module.
CU_TEST_START(pkt_rcv_calling_command_encoder_handler)
{
    uint32_t err_code;

    // ----------------------------------------------------------------------------------------
    // Prepare stimuli

    const uint8_t sample_cmd_response_message[] =
    {
        BLE_RPC_PKT_RESP,
        SD_BLE_UUID_ENCODE,
        (uint8_t)(NRF_ERROR_INVALID_ADDR),
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 8u),
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 16u),
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 24u),
    };
    // ----------------------------------------------------------------------------------------
    // Prepare the expected output

    nrf_cunit_mock_call * p_transport_mock;

    // Expect the open transport function to be called in the transport layer module.
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_open", &p_transport_mock);

    nrf_cunit_mock_call * p_transport_register_mock;
    // Expect the register function to be called in the transport layer module.
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_evt_handler_reg",
                                 &p_transport_register_mock);

    nrf_cunit_mock_call * p_transport_extract_mock;
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_extract", &p_transport_extract_mock);
    p_transport_extract_mock->result[0] = (uint32_t)sample_cmd_response_message;
    p_transport_extract_mock->result[1] = sizeof(sample_cmd_response_message);

    // Setup command encoder mock
    nrf_cunit_mock_call * p_cmd_enc_mock;

    nrf_cunit_expect_call_return((uint8_t *) "ble_rpc_cmd_rsp_pkt_received", &p_cmd_enc_mock);
    p_cmd_enc_mock->arg[0] = (uint32_t)sample_cmd_response_message;
    p_cmd_enc_mock->arg[1] = sizeof(sample_cmd_response_message);

    // ----------------------------------------------------------------------------------------

    err_code = ble_rpc_pkt_receiver_init();
    CU_ASSERT_INT_EQUAL(err_code, NRF_SUCCESS);

    hci_transport_event_handler_t transport_event_handler;
    transport_event_handler = (hci_transport_event_handler_t)
                                (p_transport_register_mock->arg_actual[0]);

    CU_ASSERT(transport_event_handler != NULL);
    if (transport_event_handler == NULL)
    {
        return;
    }

    hci_transport_evt_t hci_evt;
    hci_evt.evt_type = HCI_TRANSPORT_RX_RDY;

    transport_event_handler(hci_evt);

    // Verify that application error handler was not called.
    CU_ASSERT_INT_EQUAL(m_app_error_handler_called, false);
}
CU_TEST_END


// Test case to check if the pkt_receiver module calls the application error handler when
// the transport API fails.
CU_TEST_START(pkt_rcv_app_error_handler)
{
    uint32_t err_code;

    // ----------------------------------------------------------------------------------------
    // Prepare the expected output

    nrf_cunit_mock_call * p_transport_mock;

    // Expect the open transport function to be called in the transport layer module.
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_open", &p_transport_mock);

    nrf_cunit_mock_call * p_transport_register_mock;
    // Expect the register function to be called in the transport layer module.
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_evt_handler_reg",
                                 &p_transport_register_mock);

    nrf_cunit_mock_call * p_transport_extract_mock;
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_extract", &p_transport_extract_mock);
    p_transport_extract_mock->error = NRF_ERROR_NO_MEM;

    // ----------------------------------------------------------------------------------------

    err_code = ble_rpc_pkt_receiver_init();
    CU_ASSERT_INT_EQUAL(err_code, NRF_SUCCESS);

    hci_transport_event_handler_t transport_event_handler;
    transport_event_handler = (hci_transport_event_handler_t)
                                (p_transport_register_mock->arg_actual[0]);

    CU_ASSERT(transport_event_handler != NULL);
    if (transport_event_handler == NULL)
    {
        return;
    }

    hci_transport_evt_t hci_evt;
    hci_evt.evt_type = HCI_TRANSPORT_RX_RDY;

    transport_event_handler(hci_evt);

    // Verify that application error handler was called.
    CU_ASSERT_INT_EQUAL(m_app_error_handler_called, true);
    CU_ASSERT_INT_EQUAL(m_app_error_code, NRF_ERROR_NO_MEM);
}
CU_TEST_END


CU_SUITE_START(test_suite_pkt_receiver)
{
    CU_SUITE_SETUP_FUNC(setup_func);
    CU_SUITE_TEARDOWN_FUNC(teardown_func);

    CU_SUITE_ADD_TEST(pkt_rcv_register_with_transport_layer);
    CU_SUITE_ADD_TEST(pkt_rcv_register_with_transport_layer_open_fail);
    CU_SUITE_ADD_TEST(pkt_rcv_register_with_transport_layer_register_fail);
    CU_SUITE_ADD_TEST(pkt_rcv_calling_event_decoder_handler);
    CU_SUITE_ADD_TEST(pkt_rcv_app_error_handler);
    CU_SUITE_ADD_TEST(pkt_rcv_calling_command_encoder_handler);
}
CU_SUITE_END

CU_REGISTRY_START_STANDALONE(TC_CT_FUN_BV_UNIT_PKT_RECEIVER)
{
    CU_REGISTRY_ADD_SUITE(test_suite_pkt_receiver);
}
CU_REGISTRY_END


int main(void)
{
    CU_ADD_REGISTRY(TC_CT_FUN_BV_UNIT_PKT_RECEIVER);
    return 0;
}
