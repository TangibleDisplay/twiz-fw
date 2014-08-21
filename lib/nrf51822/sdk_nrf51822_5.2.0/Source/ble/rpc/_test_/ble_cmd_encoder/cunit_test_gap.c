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
#endif

#include <stdlib.h>
#include <string.h>

/* for cunit test framework*/
#include "boards.h"
#include "nrf_cunit.h"
#include "nrf_error.h"
#include "nrf_cunit_mock.h"
#include "hal_transport.h"
#include "ble_rpc_defines.h"
#include "nordic_common.h"
#include "ble_rpc_cmd_encoder.h"
#include "app_util.h"

// For the SoftDevice API to work.
#include "nrf.h"
#include "nrf51.h"
#include "nrf_soc.h"
#include "nrf_sdm.h"
#include "ble.h"
#include "ble_gap.h"
#include "ble_types.h"


void setup_cmd_encoder(void);  // Defined in cunit_test.c. todo: Consider placing this in header file.

void reset_unit_test(void);    // Defined in cunit_test.c. todo: Consider placing this in header file.

void set_cmd_response(uint8_t * p_response, uint32_t response_length);

/*
 * This test will ensure that sd_ble_gap_device_name_set(...) is correctly encoded.
 * It also ensure that we wait for the cmd_response until returning the error code.
 * Format is opcode, data
 *  sd_ble_gap_device_name_set(ble_gap_conn_sec_mode_t const * const p_write_perm,
 *                             uint8_t const * const p_dev_name,
 *                             uint16_t len)
 * The expected encoded packet is:
 *                             CmdOpCode (DeviceNameSet)
 *                             RPC_BLE_FIELD_PRESENT
 *                             SecurityMode (uint8_t)
 *                             Length (uint16_t)
 *                             RPC_BLE_FIELD_PRESENT
 *                             DeviceName (uint8_t[])
 *
 * This test also verifies that only the corresponding response is returned.
 */
CU_TEST_START(test_encoder_sd_ble_gap_device_name_set)
{
    reset_unit_test();
    //                       ----- Mock setup ------
    // Will verify calls need to ensure encoding sd_ble_gap_device_name_set function.
    uint8_t expected[]          = {
                                   BLE_RPC_PKT_CMD,
                                   SD_BLE_GAP_DEVICE_NAME_SET,
                                   RPC_BLE_FIELD_PRESENT, // Write Permissions Present.
                                    0x21,                 // Write Perm
                                   12, 0,                 // Device Name Length
                                   RPC_BLE_FIELD_PRESENT, // Device Name Present.
                                    'H', 'e' , 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\0'};
    uint8_t cmd_response_message[]       = {
                                   BLE_RPC_PKT_RESP,
                                   SD_BLE_GAP_DEVICE_NAME_SET,
                                   0, 0, 0, 0
                                  };

    nrf_cunit_mock_call * p_mock_call;

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    // ----------------------------------------------------------------------------------------

    // ----- Execute the command to be tested. -----
    uint8_t device_name[] = "Hello World";
    ble_gap_conn_sec_mode_t sec_mode;
    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&sec_mode);

    uint32_t err_code = sd_ble_gap_device_name_set(&sec_mode, device_name, sizeof(device_name));
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);
    // ---------------------------------------------

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();
}
CU_TEST_END


/*
 * This test will ensure that sd_ble_gap_device_name_set(...) is correctly encoded.
 * It also ensure that we wait for the cmd_response until returning the error code.
 * Format is opcode, data
 *  sd_ble_gap_device_name_set(ble_gap_conn_sec_mode_t const * const p_write_perm,
 *                             uint8_t const * const p_dev_name,
 *                             uint16_t len)
 * The expected encoded packet is:
 *                             CmdOpCode (DeviceNameSet)
 *                             RPC_BLE_FIELD_PRESENT
 *                             SecurityMode (uint8_t)
 *                             Length (uint16_t)
 *                             RPC_BLE_FIELD_NOT_PRESENT
 *
 * This test also verifies that only the corresponding response is returned.
 */
CU_TEST_START(nrffosdk_1302_test_encoder_sd_ble_gap_device_name_set_dev_name_absent)
{
    reset_unit_test();
    //                       ----- Mock setup ------
    // Will verify calls need to ensure encoding sd_ble_gap_device_name_set function.
    uint8_t expected[]          = {
                                   BLE_RPC_PKT_CMD,
                                   SD_BLE_GAP_DEVICE_NAME_SET,
                                   RPC_BLE_FIELD_PRESENT,    // Write Permissions Present.
                                    0x21,                    // Write Perm
                                   0xcd, 0xab,               // Device Name Length
                                   RPC_BLE_FIELD_NOT_PRESENT // Device Name NOT Present.
                                   };
    uint8_t cmd_response_message[]       = {
                                   BLE_RPC_PKT_RESP,
                                   SD_BLE_GAP_DEVICE_NAME_SET,
                                   0, 0, 0, 0
                                  };

    nrf_cunit_mock_call * p_mock_call;

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);

    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    // ----------------------------------------------------------------------------------------

    // ----- Execute the command to be tested. -----
    ble_gap_conn_sec_mode_t sec_mode;
    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&sec_mode);

    uint32_t err_code = sd_ble_gap_device_name_set(&sec_mode, NULL, 0xabcd);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);
    // ---------------------------------------------

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();
}
CU_TEST_END



/*
 * This test will ensure that sd_ble_gap_device_name_set(...) is correctly encoded.
 * It also ensure that we wait for the cmd_response until returning the error code.
 * Format is opcode, data
 *  sd_ble_gap_device_name_set(ble_gap_conn_sec_mode_t const * const p_write_perm,
 *                             uint8_t const * const p_dev_name,
 *                             uint16_t len)
 * The expected encoded packet is:
 *                             CmdOpCode (DeviceNameSet)
 *                             RPC_BLE_FIELD_NOT_PRESENT
 *                             Length (uint16_t)
 *                             RPC_BLE_FIELD_NOT_PRESENT
 *
 * This test also verifies that only the corresponding response is returned.
 */
CU_TEST_START(nrffosdk_1302_test_encoder_sd_ble_gap_device_name_set_dev_name_and_write_perm_absent)
{
    reset_unit_test();
    //                       ----- Mock setup ------
    // Will verify calls need to ensure encoding sd_ble_gap_device_name_set function.
    uint8_t expected[]          = {
                                   BLE_RPC_PKT_CMD,
                                   SD_BLE_GAP_DEVICE_NAME_SET,
                                   RPC_BLE_FIELD_NOT_PRESENT,    // Write Permissions NOT Present.
                                   0xef, 0xcd,                   // Device Name Length
                                   RPC_BLE_FIELD_NOT_PRESENT     // Device Name NOT Present.
                                   };
    uint8_t cmd_response_message[]       = {
                                   BLE_RPC_PKT_RESP,
                                   SD_BLE_GAP_DEVICE_NAME_SET,
                                   0, 0, 0, 0
                                  };

    nrf_cunit_mock_call * p_mock_call;

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    // ----------------------------------------------------------------------------------------

    // ----- Execute the command to be tested. -----

    uint32_t err_code = sd_ble_gap_device_name_set(NULL, NULL, 0xcdef);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);
    // ---------------------------------------------

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();
}
CU_TEST_END


/**
 * This test will ensure that a SoftDevice call is correctly encoded and passed to the transport
 * layer.
 * An emulated received response is sent to the transport event handler in the encoder to verify
 * correct decoding of responses.
 *
 * This test will ensure that sd_ble_gap_device_name_set(...) is correctly encoded.
 * It also ensure that we wait for the cmd_response until returning the error code.
 * Format is opcode, data
 *  sd_ble_gap_device_name_set(ble_gap_conn_sec_mode_t const * const p_write_perm,
 *                             uint8_t const * const p_dev_name,
 *                             uint16_t len)
 * The expected encoded packet is:
 *                             CmdOpCode (DeviceNameSet)
 *                             RPC_BLE_FIELD_PRESENT
 *                             SecurityMode (uint8_t)
 *                             Length (uint16_t)
 *                             RPC_BLE_FIELD_PRESENT
 *                             DeviceName (uint8_t[])
 *
 * This test ensures that the NRF_ERROR_INVALID_PARAM is correctly decoded.
 */
CU_TEST_START(test_encoder_sd_ble_gap_device_name_set_param_error)
{
    reset_unit_test();
    /**                       ----- Mock setup ------
     * Will verify calls need to ensure encoding sd_ble_gap_device_name_set function.
     */
    nrf_cunit_mock_call * p_mock_call;
    uint8_t expected[]          = {
                                   BLE_RPC_PKT_CMD,
                                   SD_BLE_GAP_DEVICE_NAME_SET,
                                   RPC_BLE_FIELD_PRESENT, // Write Permissions Present.
                                    0x11,                 // Write Perm
                                   12, 0,                 // Device Name Length
                                   RPC_BLE_FIELD_PRESENT, // Device Name Present.
                                    'H', 'e' , 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\0'};
    uint8_t cmd_response_message[] = {
                                      BLE_RPC_PKT_RESP,
                                      SD_BLE_GAP_DEVICE_NAME_SET,
                                      7, 0, 0, 0
                                     };

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    /* ---------------------------------------------------------------------------------------- */
    uint8_t device_name[] = "Hello World";
    ble_gap_conn_sec_mode_t sec_mode;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    uint32_t err_code = sd_ble_gap_device_name_set(&sec_mode, device_name, sizeof(device_name));
    CU_ASSERT_INT_EQUAL(NRF_ERROR_INVALID_PARAM, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();
}
CU_TEST_END



/**
 * This test will verify that the command encoder is capable of handling command responses which
 * only has the OpCode available. As the reason for this is unknown, we return NRF_ERROR_INTERNAL.
 *
 */
CU_TEST_START(test_encoder_sd_ble_gap_device_name_set_internal_error)
{
    reset_unit_test();
    //                       ----- Mock setup ------
    // Will verify calls need to ensure encoding sd_ble_gap_device_name_set function.
    uint8_t expected[]          = {
                                   BLE_RPC_PKT_CMD,
                                   SD_BLE_GAP_DEVICE_NAME_SET,
                                   RPC_BLE_FIELD_PRESENT, // Write Permissions Present.
                                    0x11,                 // Write Perm
                                   12, 0,                 // Device Name Length
                                   RPC_BLE_FIELD_PRESENT, // Device Name Present.
                                    'H', 'e' , 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\0'};
    uint8_t cmd_response_message[]       = {
                                   BLE_RPC_PKT_RESP,
                                   SD_BLE_GAP_DEVICE_NAME_SET
                                  };

    nrf_cunit_mock_call * p_mock_call;

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    // ----------------------------------------------------------------------------------------

    // ----- Execute the commands to be tested. -----
    uint8_t device_name[] = "Hello World";
    ble_gap_conn_sec_mode_t sec_mode;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    uint32_t err_code = sd_ble_gap_device_name_set(&sec_mode, device_name, sizeof(device_name));
    CU_ASSERT_INT_EQUAL(NRF_ERROR_INTERNAL, err_code);
    // ---------------------------------------------

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();
}
CU_TEST_END


/**
 * This test will ensure that a SoftDevice call is correctly encoded and passed to the transport
 * layer.
 * An emulated received response is sent to the transport event handler in the encoder to verify
 * correct decoding of responses.
 *
 * This test will ensure that sd_ble_gap_appearance_set(...) is correctly encoded.
 * It also ensure that we wait for the cmd_response until returning the error code.
 * Format is opcode, data
 *  sd_ble_gap_device_name_set(ble_gap_conn_sec_mode_t const * const p_write_perm,
 *                             uint8_t const * const p_dev_name,
 *                             uint16_t len)
 * The expected encoded packet is:
 *                             CmdOpCode (ApperanceSet)
 *                             Apperance (uint16_t)
 */
CU_TEST_START(test_encoder_sd_ble_gap_appearance_set_heart_rate)
{
    reset_unit_test();
    // BLE_APPEARANCE_GENERIC_HEART_RATE_SENSOR = 832 = 0x0340 (Will be encoded LSB first)
    //                       ----- Mock setup ------
    // Will verify calls need to ensure encoding sd_ble_gap_appearance_set function.
    nrf_cunit_mock_call * p_mock_call;
    uint8_t               expected[]             = {
                                                    BLE_RPC_PKT_CMD,
                                                    SD_BLE_GAP_APPEARANCE_SET,
                                                    0x40, 0x03
                                                    };
    uint8_t               cmd_response_message[] = {
                                                    BLE_RPC_PKT_RESP,
                                                    SD_BLE_GAP_APPEARANCE_SET,
                                                    0, 0, 0, 0
                                                   };

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    // ----------------------------------------------------------------------------------------

    uint32_t err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_HEART_RATE_SENSOR);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();
}
CU_TEST_END


/**
 * This test will verify that the command encoder is capable of handling command responses which
 * only has the OpCode available. As the reason for this is unknown, we return NRF_ERROR_INTERNAL.
 *
 */
CU_TEST_START(test_encoder_sd_ble_gap_appearance_set_internal_error)
{
    reset_unit_test();
    //                       ----- Mock setup ------
    // Will verify calls need to ensure encoding sd_ble_gap_device_name_set function.
    nrf_cunit_mock_call * p_mock_call;
    uint8_t               expected[]             = {
                                                    BLE_RPC_PKT_CMD,
                                                    SD_BLE_GAP_APPEARANCE_SET,
                                                    0xC2, 0x03
                                                   };
    uint8_t               cmd_response_message[] = {
                                                    BLE_RPC_PKT_RESP,
                                                    SD_BLE_GAP_APPEARANCE_SET,
                                                    0, 0
                                                   };

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    // ----------------------------------------------------------------------------------------

    uint32_t err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_HID_MOUSE);
    CU_ASSERT_INT_EQUAL(NRF_ERROR_INTERNAL, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();
}
CU_TEST_END


/**
 * This test will ensure that sd_ble_gap_ppcp_set(...) is correctly encoded.
 * It also ensure that we wait for the cmd_response until returning the error code.
 *
 */
CU_TEST_START(test_encoder_sd_ble_gap_ppcp_set)
{
    reset_unit_test();
    //                       ----- Mock setup ------
    // Will verify calls need to ensure encoding sd_ble_gap_appearance_set function.
    nrf_cunit_mock_call * p_mock_call;
    uint8_t               expected[]             = {
                                                    BLE_RPC_PKT_CMD,
                                                    SD_BLE_GAP_PPCP_SET,
                                                    RPC_BLE_FIELD_PRESENT, // Connection parameters present.
                                                     0x34, 0x12,            // min conn interval
                                                     0xcd, 0xab,            // max conn interval
                                                     0x78, 0x56,            // Slave latency
                                                     0xef, 0x90,            // Supervision timeout
                                                    };
    uint8_t               cmd_response_message[] = {
                                                    BLE_RPC_PKT_RESP,
                                                    SD_BLE_GAP_PPCP_SET,
                                                    0, 0, 0, 0
                                                   };

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    // ----------------------------------------------------------------------------------------

    ble_gap_conn_params_t connection_params;
    connection_params.min_conn_interval     = 0x1234;
    connection_params.max_conn_interval     = 0xabcd;
    connection_params.slave_latency         = 0x5678;
    connection_params.conn_sup_timeout      = 0x90ef;

    uint32_t err_code = sd_ble_gap_ppcp_set(&connection_params);

    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();
}
CU_TEST_END


/**
 * This test will ensure that sd_ble_gap_ppcp_set(...) is correctly encoded.
 * It also ensure that we wait for the cmd_response until returning the error code.
 *
 * It also verifies that NRF_ERROR_INVALID_PARAM is returned.
 */
CU_TEST_START(test_encoder_sd_ble_gap_ppcp_set_none_and_invalid_param)
{
    reset_unit_test();
    //                       ----- Mock setup ------
    // Will verify calls need to ensure encoding sd_ble_gap_appearance_set function.
    nrf_cunit_mock_call * p_mock_call;
    uint8_t               expected[]             = {
                                                    BLE_RPC_PKT_CMD,
                                                    SD_BLE_GAP_PPCP_SET,
                                                    RPC_BLE_FIELD_PRESENT, // Connection parameters present.
                                                     0x34, 0x12,            // min conn interval
                                                     0xcd, 0xab,            // max conn interval
                                                     0x78, 0x56,            // Slave latency
                                                     0xef, 0x90,            // Supervision timeout
                                                    };
    uint8_t               cmd_response_message[] = {
                                                    BLE_RPC_PKT_RESP,
                                                    SD_BLE_GAP_PPCP_SET,
                                                    7, 0, 0, 0
                                                   };

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    // ----------------------------------------------------------------------------------------

    ble_gap_conn_params_t connection_params;
    connection_params.min_conn_interval     = 0x1234;
    connection_params.max_conn_interval     = 0xabcd;
    connection_params.slave_latency         = 0x5678;
    connection_params.conn_sup_timeout      = 0x90ef;

    uint32_t err_code = sd_ble_gap_ppcp_set(&connection_params);

    CU_ASSERT_INT_EQUAL(NRF_ERROR_INVALID_PARAM, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();
}
CU_TEST_END


/**
 * This test will ensure that sd_ble_gap_ppcp_set(...) is correctly encoded.
 * It also ensure that we wait for the cmd_response until returning the error code.
 *
 * It also verifies that if connection parameters pointer is set to NULL, then the output is
 * as per expectations.
 */
CU_TEST_START(test_encoder_sd_ble_gap_ppcp_set_conn_params_absent)
{
    reset_unit_test();
    //                       ----- Mock setup ------
    // Will verify calls need to ensure encoding sd_ble_gap_appearance_set function.
    nrf_cunit_mock_call * p_mock_call;
    uint8_t               expected[]             = {
                                                    BLE_RPC_PKT_CMD,
                                                    SD_BLE_GAP_PPCP_SET,
                                                    RPC_BLE_FIELD_NOT_PRESENT // Connection parameters present.
                                                    };
    uint8_t               cmd_response_message[] = {
                                                    BLE_RPC_PKT_RESP,
                                                    SD_BLE_GAP_PPCP_SET,
                                                    0, 0, 0, 0
                                                   };

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    // ----------------------------------------------------------------------------------------

    uint32_t err_code = sd_ble_gap_ppcp_set(NULL);

    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gap_adv_start(...)
 *  when directed advertising AND whitelist are not used.
 *
 * The expected encoded packet is:
 *                  The expected encoded packet is: CmdOpCode (PpcpSet) - adv params
 *                  BLE_GAP_ADV_TYPE_ADV_IND         = 0x00
 *                  peer addr                        = NULL
 *                  BLE_GAP_ADV_FP_ANY               = 0x00
 *                  whitelist                        = NULL
 *                  BLE_GAP_ADV_INTERVAL_MIN         = 0x0020
 *                  BLE_GAP_ADV_TIMEOUT_LIMITED_MAX  = 0x00B4 (180)
 */
CU_TEST_START(test_encoder_sd_ble_gap_adv_start_simple)
{
    reset_unit_test();
    nrf_cunit_mock_call * p_mock_call;
    uint8_t               expected[]             = {
                                                    BLE_RPC_PKT_CMD,
                                                    SD_BLE_GAP_ADV_START,
                                                    0x00, 0x00,
                                                    0x00, 0x00,
                                                    0x20, 0x00,
                                                    0xB4, 0x00
                                                   };
    uint8_t               cmd_response_message[] = {
                                                    BLE_RPC_PKT_RESP,
                                                    SD_BLE_GAP_ADV_START,
                                                    0, 0, 0, 0
                                                   };

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    // ----------------------------------------------------------------------------------------

    ble_gap_adv_params_t adv_params;
    adv_params.type         = BLE_GAP_ADV_TYPE_ADV_IND;
    adv_params.p_peer_addr  = NULL;
    adv_params.fp           = BLE_GAP_ADV_FP_ANY;
    adv_params.p_whitelist  = NULL;
    adv_params.interval     = BLE_GAP_ADV_INTERVAL_MIN;
    adv_params.timeout      = BLE_GAP_ADV_TIMEOUT_LIMITED_MAX;

    uint32_t err_code = sd_ble_gap_adv_start(&adv_params);

    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gap_adv_start(...)
 *  when directed advertising is used but without whitelist.
 *
 * The expected encoded packet is:
 *                  The expected encoded packet is: CmdOpCode (PpcpSet) - adv params
 *                  BLE_GAP_ADV_TYPE_ADV_IND         = 0x00
 *                  peer addr->addr_type             = BLE_GAP_ADDR_TYPE_RANDOM_STATIC
 *                  peer addr->addr                  = {0x00, 0x01, 0x00, 0x3D, 0x7D, 0x01}
 *                  BLE_GAP_ADV_FP_ANY               = 0x00
 *                  whitelist                        = NULL
 *                  BLE_GAP_ADV_INTERVAL_MIN         = 0x0020
 *                  BLE_GAP_ADV_TIMEOUT_LIMITED_MAX  = 0x00B4 (180)
 */
CU_TEST_START(test_encoder_sd_ble_gap_adv_start_directed_peer)
{
    reset_unit_test();
    nrf_cunit_mock_call * p_mock_call;
    uint8_t               expected[]             = {
                                                    BLE_RPC_PKT_CMD,
                                                    SD_BLE_GAP_ADV_START,
                                                    0x01,                               // BLE_GAP_ADV_TYPE_ADV_DIRECT_IND
                                                    0x01,                               // Peer address present.
                                                    0x01,                               // BLE_GAP_ADDR_TYPE_RANDOM_STATIC
                                                    0x00, 0x01, 0x00, 0x3D, 0x7D, 0x01, // Peer address.
                                                    0x00,                               // BLE_GAP_ADV_FP_ANY
                                                    0x00,                               // Whitelist not present
                                                    0x20, 0x00,                         // BLE_GAP_ADV_INTERVAL_MIN
                                                    0xB4, 0x00                          // BLE_GAP_ADV_TIMEOUT_LIMITED_MAX
                                                   };
    uint8_t               cmd_response_message[] = {
                                                    BLE_RPC_PKT_RESP,
                                                    SD_BLE_GAP_ADV_START,
                                                    0, 0, 0, 0
                                                   };

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    // ----------------------------------------------------------------------------------------

    ble_gap_addr_t peer_addr = {BLE_GAP_ADDR_TYPE_RANDOM_STATIC,
                               {0x00, 0x01, 0x00, 0x3D, 0x7D, 0x01}};

    ble_gap_adv_params_t adv_params;
    adv_params.type         = BLE_GAP_ADV_TYPE_ADV_DIRECT_IND;
    adv_params.p_peer_addr  = &peer_addr;
    adv_params.fp           = BLE_GAP_ADV_FP_ANY;
    adv_params.p_whitelist  = NULL;
    adv_params.interval     = BLE_GAP_ADV_INTERVAL_MIN;
    adv_params.timeout      = BLE_GAP_ADV_TIMEOUT_LIMITED_MAX;

    uint32_t err_code = sd_ble_gap_adv_start(&adv_params);

    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gap_adv_start(...)
 *  when directed advertising is used but without whitelist.
 *
 * The expected encoded packet is:
 *                  The expected encoded packet is: CmdOpCode (PpcpSet) - adv params
 *                  BLE_GAP_ADV_TYPE_ADV_IND         = 0x00
 *                  peer addr->addr_type             = BLE_GAP_ADDR_TYPE_RANDOM_STATIC
 *                  peer addr->addr                  = {0x00, 0x01, 0x00, 0x3D, 0x7D, 0x01}
 *                  BLE_GAP_ADV_FP_ANY               = 0x00
 *                  whitelist                        = NULL
 *                  BLE_GAP_ADV_INTERVAL_MIN         = 0x0020
 *                  BLE_GAP_ADV_TIMEOUT_LIMITED_MAX  = 0x00B4 (180)
 */
CU_TEST_START(test_encoder_sd_ble_gap_adv_start_directed_peer_with_whitelist)
{
    reset_unit_test();
    nrf_cunit_mock_call * p_mock_call;
    uint32_t              i = 0;
    uint8_t               expected[]             = {
                                                    BLE_RPC_PKT_CMD,
                                                    SD_BLE_GAP_ADV_START,
                                                    0x01,                               // BLE_GAP_ADV_TYPE_ADV_DIRECT_IND
                                                    0x01,                               // Peer address present.
                                                    0x01,                               // BLE_GAP_ADDR_TYPE_RANDOM_STATIC
                                                    0x00, 0x01, 0x00, 0x3D, 0x7D, 0x01, // Peer address.
                                                    0x00,                               // BLE_GAP_ADV_FP_ANY
                                                    0x01,                               // Whitelist present
                                                    0x08,                               // BLE_GAP_WHITELIST_ADDR_MAX_COUNT
                                                    0x01, 0x43, 0x8F, 0xAE, 0x8D, 0xBD, 0x41, // Address 1.
                                                    0x01, 0x43, 0x8F, 0xAE, 0x8D, 0xBD, 0x42, // Address 2.
                                                    0x01, 0x43, 0x8F, 0xAE, 0x8D, 0xBD, 0x43, // Address 3.
                                                    0x01, 0x43, 0x8F, 0xAE, 0x8D, 0xBD, 0x44, // Address 4.
                                                    0x01, 0x43, 0x8F, 0xAE, 0x8D, 0xBD, 0x45, // Address 5.
                                                    0x01, 0x43, 0x8F, 0xAE, 0x8D, 0xBD, 0x46, // Address 6.
                                                    0x01, 0x43, 0x8F, 0xAE, 0x8D, 0xBD, 0x47, // Address 7.
                                                    0x01, 0x43, 0x8F, 0xAE, 0x8D, 0xBD, 0x48, // Address 8.
                                                    0x08,                               // BLE_GAP_WHITELIST_IRK_MAX_COUNT
                                                    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, // IRK 1.
                                                    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
                                                    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, // IRK 2.
                                                    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
                                                    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, // IRK 3.
                                                    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
                                                    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, // IRK 4.
                                                    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
                                                    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, // IRK 5.
                                                    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
                                                    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, // IRK 6.
                                                    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
                                                    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, // IRK 7.
                                                    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
                                                    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, // IRK 8.
                                                    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
                                                    0x20, 0x00,                         // BLE_GAP_ADV_INTERVAL_MIN
                                                    0xB4, 0x00                          // BLE_GAP_ADV_TIMEOUT_LIMITED_MAX
                                                   };
    uint8_t               cmd_response_message[] = {
                                                    BLE_RPC_PKT_RESP,
                                                    SD_BLE_GAP_ADV_START,
                                                    0, 0, 0, 0
                                                   };

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    // ----------------------------------------------------------------------------------------

    ble_gap_addr_t      peer_addr = {BLE_GAP_ADDR_TYPE_RANDOM_STATIC,
                                    {0x00, 0x01, 0x00, 0x3D, 0x7D, 0x01} };

    ble_gap_whitelist_t whitelist;
    ble_gap_irk_t    irk_list[BLE_GAP_WHITELIST_IRK_MAX_COUNT];
    ble_gap_addr_t   whitelist_addr[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
    ble_gap_addr_t * p_addr[BLE_GAP_WHITELIST_IRK_MAX_COUNT];
    ble_gap_irk_t  * p_irk[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];

    for (i = 0; i < (BLE_GAP_WHITELIST_ADDR_MAX_COUNT); i++)
    {
        whitelist_addr[i].addr_type = BLE_GAP_ADDR_TYPE_RANDOM_STATIC;
        whitelist_addr[i].addr[0] = 0x43;
        whitelist_addr[i].addr[1] = 0x8F;
        whitelist_addr[i].addr[2] = 0xAE;
        whitelist_addr[i].addr[3] = 0x8D;
        whitelist_addr[i].addr[4] = 0xBD;
        whitelist_addr[i].addr[5] = (0x41 + i);
        p_addr[i] = &(whitelist_addr[i]);
    }

    for (i = 0; i < BLE_GAP_WHITELIST_IRK_MAX_COUNT; i++)
    {
        uint32_t j;
        for (j = 0; j < BLE_GAP_SEC_KEY_LEN; j++)
        {
            irk_list[i].irk[j] = ((i+1) & 0xFF);
        }
        p_irk[i] = &(irk_list[i]);
    }

    whitelist.addr_count = BLE_GAP_WHITELIST_ADDR_MAX_COUNT;
    whitelist.irk_count  = BLE_GAP_WHITELIST_IRK_MAX_COUNT;
    whitelist.pp_addrs   = p_addr;
    whitelist.pp_irks    = p_irk;

    ble_gap_adv_params_t adv_params;
    adv_params.type         = BLE_GAP_ADV_TYPE_ADV_DIRECT_IND;
    adv_params.p_peer_addr  = &peer_addr;
    adv_params.fp           = BLE_GAP_ADV_FP_ANY;
    adv_params.p_whitelist  = &whitelist;
    adv_params.interval     = BLE_GAP_ADV_INTERVAL_MIN;
    adv_params.timeout      = BLE_GAP_ADV_TIMEOUT_LIMITED_MAX;

    uint32_t err_code = sd_ble_gap_adv_start(&adv_params);

    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gap_adv_set(...)
 *
 * The expected encoded packet is:
 *     sd_ble_gap_adv_data_set(uint8_t const * const p_data,
 *                             uint8_t dlen,
 *                             uint8_t const * const p_sr_data,
 *                             uint8_t srdlen)
 */
CU_TEST_START(test_encoder_sd_ble_gap_adv_data_set)
{
    reset_unit_test();
    nrf_cunit_mock_call * p_mock_call;
    uint8_t               expected[]             = {
                                                    BLE_RPC_PKT_CMD,
                                                    SD_BLE_GAP_ADV_DATA_SET,
                                                    0x10, 0x0A, 0x0B, 0x01, 0x02, 0x22, 0x33, 0x34,
                                                          0x10, 0x0A, 0x0B, 0x01, 0x02, 0x22, 0x33,
                                                          0x34, 0xFF,
                                                    0x05, 0x20, 0x00, 0xB4, 0x00, 0x34};
    uint8_t               cmd_response_message[] = {
                                                    BLE_RPC_PKT_RESP,
                                                    SD_BLE_GAP_ADV_DATA_SET,
                                                    0, 0, 0, 0
                                                   };

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    // ----------------------------------------------------------------------------------------
    uint8_t data[]           = {0x0A, 0x0B, 0x01, 0x02, 0x22, 0x33, 0x34, 0x10, 0x0A, 0x0B, 0x01,
                                0x02, 0x22, 0x33, 0x34, 0xFF,};
    uint8_t data_length      = sizeof(data);
    uint8_t scan_resp_data[] = {0x20, 0x00, 0xB4, 0x00, 0x34};
    uint8_t sr_data_length   = sizeof(scan_resp_data);

    uint32_t err_code = sd_ble_gap_adv_data_set(data, data_length, scan_resp_data, sr_data_length);

    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gap_disconnect(...)
 *
 * The expected encoded packet is:
 *     uint32_t sd_ble_gap_disconnect(uint16_t conn_handle, uint8_t hci_status_code)
 */
CU_TEST_START(test_encoder_sd_ble_gap_disconnect)
{
    reset_unit_test();
    nrf_cunit_mock_call * p_mock_call;
    
    const uint16_t conn_handle              = 0xAA55u;
    const uint8_t  hci_status_code          = 0xAAu;
    const uint8_t  expected[]               = {
                                               BLE_RPC_PKT_CMD,
                                               SD_BLE_GAP_DISCONNECT,
                                               LSB(conn_handle),
                                               MSB(conn_handle),
                                               hci_status_code
                                              };
    const uint8_t cmd_response_message[]    = {
                                               BLE_RPC_PKT_RESP,
                                               SD_BLE_GAP_DISCONNECT,
                                               0, 0, 0, 0
                                              };
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gap_disconnect(conn_handle, hci_status_code);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();        
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gap_sec_params_reply(...)
 * when no optional input data is present 
 *
 * The expected encoded packet is:
 *     sd_ble_gap_sec_params_reply(uint16_t conn_handle, 
 *                                 uint8_t sec_status, 
 *                                 ble_gap_sec_params_t const * const p_sec_params)
 */
CU_TEST_START(test_encoder_sd_ble_gap_sec_params_reply_no_optionals)
{
    reset_unit_test();
    nrf_cunit_mock_call * p_mock_call;
    
    const uint16_t conn_handle           = 0xAA55u;
    const uint8_t  sec_status            = 0xAAu;
    const uint8_t  expected[]            = {
                                            BLE_RPC_PKT_CMD,
                                            SD_BLE_GAP_SEC_PARAMS_REPLY,
                                            LSB(conn_handle),
                                            MSB(conn_handle),
                                            sec_status,
                                            RPC_BLE_FIELD_NOT_PRESENT
                                           };
    const uint8_t cmd_response_message[] = {
                                            BLE_RPC_PKT_RESP,
                                            SD_BLE_GAP_SEC_PARAMS_REPLY,
                                            0, 0, 0, 0
                                           };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gap_sec_params_reply(conn_handle, sec_status, NULL);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();        
}
CU_TEST_END

/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gap_sec_params_reply(...)
 * when all optional input data is present 
 *
 * The expected encoded packet is:
 *     sd_ble_gap_sec_params_reply(uint16_t conn_handle, 
 *                                 uint8_t sec_status, 
 *                                 ble_gap_sec_params_t const * const p_sec_params)
 */
CU_TEST_START(test_encoder_sd_ble_gap_sec_params_reply_all_optionals)
{
    reset_unit_test();
    nrf_cunit_mock_call * p_mock_call;
    ble_gap_sec_params_t  sec_params;
    
    memset(&sec_params, 0, sizeof(ble_gap_sec_params_t));

    sec_params.timeout      = 0x11EEu;
    sec_params.bond         = 0u;
    sec_params.mitm         = 1u;
    sec_params.io_caps      = 6u;
    sec_params.oob          = 1u;
    sec_params.min_key_size = 0xBBu;
    sec_params.max_key_size = 0x77u;  
    
    const uint16_t conn_handle            = 0xAA55u;
    const uint8_t  sec_status             = 0xAAu;
    const uint8_t  expected[]             = {
                                             BLE_RPC_PKT_CMD,
                                             SD_BLE_GAP_SEC_PARAMS_REPLY,
                                             LSB(conn_handle),
                                             MSB(conn_handle),
                                             sec_status,
                                             RPC_BLE_FIELD_PRESENT,
                                             LSB(sec_params.timeout),
                                             MSB(sec_params.timeout),
                                             0x3A, // 0bxx111010 // (sec_params.oob<< 5), (sec_params.io_caps << 2), (sec_params.mitm << 1), (sec_params.bond << 0)
                                             sec_params.min_key_size,
                                             sec_params.max_key_size,
                                            };
    const uint8_t  cmd_response_message[] = {
                                             BLE_RPC_PKT_RESP,
                                             SD_BLE_GAP_SEC_PARAMS_REPLY,
                                             0, 0, 0, 0
                                            };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gap_sec_params_reply(conn_handle, sec_status, &sec_params);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();        
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gap_appearance_get(...)
 *
 * The expected encoded packet is:
 *     sd_ble_gap_appearance_get(uint16_t * const p_appearance)
 */
CU_TEST_START(test_encoder_sd_ble_gap_appearance_get)
{
    reset_unit_test();
    nrf_cunit_mock_call * p_mock_call;
    
    const uint8_t  expected[]            = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GAP_APPEARANCE_GET,
        RPC_BLE_FIELD_PRESENT
    };  
    const uint8_t cmd_response_message[] = 
    {
        BLE_RPC_PKT_RESP,
        SD_BLE_GAP_APPEARANCE_GET,
        0, 0, 0, 0, 
        0xAAu, 0x55u
    };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    
    uint16_t appearance = 0xFFFFu;
    uint32_t err_code = sd_ble_gap_appearance_get(&appearance);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);
    const uint16_t expected_appearance = 
        uint16_decode(&(cmd_response_message[BLE_OP_CODE_SIZE  +
                                             BLE_PKT_TYPE_SIZE +
                                             RPC_ERR_CODE_SIZE]));
    CU_ASSERT_INT_EQUAL(appearance, expected_appearance);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();        
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gap_appearance_get(...)
 *
 * The expected encoded packet is:
 *     sd_ble_gap_appearance_get(uint16_t * const p_appearance)
 */
CU_TEST_START(test_encoder_sd_ble_gap_appearance_get_null)
{
    reset_unit_test();
    nrf_cunit_mock_call * p_mock_call;

    const uint8_t  expected[]            =
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GAP_APPEARANCE_GET,
        RPC_BLE_FIELD_NOT_PRESENT
    };
    const uint8_t cmd_response_message[] =
    {
        BLE_RPC_PKT_RESP,
        SD_BLE_GAP_APPEARANCE_GET,
        7, 0, 0, 0
    };

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();

    uint32_t err_code = sd_ble_gap_appearance_get(NULL);
    CU_ASSERT_INT_EQUAL(NRF_ERROR_INVALID_PARAM, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gap_device_name_get(...)
 * when all optional input data is present
 *
 * The expected encoded packet is:
 *     sd_ble_gap_device_name_get(uint8_t * const p_dev_name, uint16_t * const p_len)
 */
CU_TEST_START(test_encoder_sd_ble_gap_device_name_get)
{
    reset_unit_test();
    nrf_cunit_mock_call * p_mock_call;
    
    uint8_t device_name[12u]             = {0};
    const uint16_t buffer_size           = sizeof(device_name);
    const uint8_t  expected[]            = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GAP_DEVICE_NAME_GET,
        RPC_BLE_FIELD_PRESENT,
        LSB(buffer_size),
        //lint -e(572) "Excessive shift value"        
        MSB(buffer_size),
        RPC_BLE_FIELD_PRESENT
    };  
    const uint8_t cmd_response_message[] = 
    {
        BLE_RPC_PKT_RESP,
        SD_BLE_GAP_DEVICE_NAME_GET,
        0, 0, 0, 0, 
        12u, 0,
        'H', 'e' , 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\0'        
    };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    
    uint16_t length          = buffer_size;    
    uint32_t err_code        = sd_ble_gap_device_name_get(device_name, &length);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    const uint16_t decoded_length = uint16_decode(&
        (cmd_response_message[BLE_OP_CODE_SIZE + BLE_PKT_TYPE_SIZE + RPC_ERR_CODE_SIZE]));
    CU_ASSERT_INT_EQUAL(length, decoded_length);            
        
    CU_ASSERT(memcmp(
        device_name,
        &(cmd_response_message[BLE_OP_CODE_SIZE + BLE_PKT_TYPE_SIZE + RPC_ERR_CODE_SIZE + sizeof(length)]),
        length) == 0);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();           
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gap_device_name_get(...)
 * when optional length field is omitted.
 *
 * The expected encoded packet is:
 *     sd_ble_gap_device_name_get(uint8_t * const p_dev_name, uint16_t * const p_len)
 */
CU_TEST_START(test_encoder_sd_ble_gap_device_name_get_length_omitted)
{
    reset_unit_test();
    nrf_cunit_mock_call * p_mock_call;
    
    uint8_t        device_name[12u]      = {0};
    const uint8_t  expected[]            = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GAP_DEVICE_NAME_GET,
        RPC_BLE_FIELD_NOT_PRESENT,        
        RPC_BLE_FIELD_PRESENT
    };  
    const uint8_t cmd_response_message[] = 
    {
        BLE_RPC_PKT_RESP,
        SD_BLE_GAP_DEVICE_NAME_GET,
        0, 0, 0, 0
    };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code        = sd_ble_gap_device_name_get(device_name, NULL);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);
    
    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();           
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gap_device_name_get(...)
 * when optional buffer field is omitted.
 *
 * The expected encoded packet is:
 *     sd_ble_gap_device_name_get(uint8_t * const p_dev_name, uint16_t * const p_len)
 */
CU_TEST_START(test_encoder_sd_ble_gap_device_name_get_buffer_omitted)
{
    reset_unit_test();
    nrf_cunit_mock_call * p_mock_call;
    
    uint16_t      buffer_size            = 0;
    const uint8_t expected[]             = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GAP_DEVICE_NAME_GET,
        RPC_BLE_FIELD_PRESENT,
        LSB(buffer_size),
        MSB(buffer_size),
        RPC_BLE_FIELD_NOT_PRESENT
    };  
    const uint8_t cmd_response_message[] = 
    {
        BLE_RPC_PKT_RESP,
        SD_BLE_GAP_DEVICE_NAME_GET,
        (uint8_t)(NRF_ERROR_INVALID_ADDR),
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 8u),
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 16u),        
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 24u)                
    };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gap_device_name_get(NULL, &buffer_size);
    CU_ASSERT_INT_EQUAL(NRF_ERROR_INVALID_ADDR, err_code);
    
    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();           
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gap_device_name_get(...)
 * when all optional fields are omitted.
 *
 * The expected encoded packet is:
 *     sd_ble_gap_device_name_get(uint8_t * const p_dev_name, uint16_t * const p_len)
 */
CU_TEST_START(test_encoder_sd_ble_gap_device_name_get_optionals_omitted)
{
    reset_unit_test();
    nrf_cunit_mock_call * p_mock_call;
    
    const uint8_t  expected[]            = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GAP_DEVICE_NAME_GET,
        RPC_BLE_FIELD_NOT_PRESENT,
        RPC_BLE_FIELD_NOT_PRESENT
    };  
    const uint8_t cmd_response_message[] = 
    {
        BLE_RPC_PKT_RESP,
        SD_BLE_GAP_DEVICE_NAME_GET,
        (uint8_t)(NRF_ERROR_INVALID_ADDR),
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 8u),
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 16u),        
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 24u)                
    };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gap_device_name_get(NULL, NULL);
    CU_ASSERT_INT_EQUAL(NRF_ERROR_INVALID_ADDR, err_code);
    
    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();           
}
CU_TEST_END





/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gap_address_set(...)
 * when all input data is NULL  
 *
 * The expected encoded packet is:
 *     sd_ble_gap_address_set(ble_gap_addr_t const * const p_addr)
 */
CU_TEST_START(test_encoder_sd_ble_gap_address_set_null_input)
{
    reset_unit_test();
    nrf_cunit_mock_call * p_mock_call;

    const uint8_t expected[] ={
        BLE_RPC_PKT_CMD,
        SD_BLE_GAP_ADDRESS_SET,
        RPC_BLE_FIELD_NOT_PRESENT
    };
    const uint8_t cmd_response_message[] ={
        BLE_RPC_PKT_RESP,
        SD_BLE_GAP_ADDRESS_SET,
        (uint8_t) (NRF_ERROR_INVALID_ADDR),
        (uint8_t) (NRF_ERROR_INVALID_ADDR << 8u),
        (uint8_t) (NRF_ERROR_INVALID_ADDR << 16u),
        (uint8_t) (NRF_ERROR_INVALID_ADDR << 24u)
    };

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof (expected);

    set_cmd_response((uint8_t *) cmd_response_message, sizeof (cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;
    
    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gap_address_set(NULL);
    CU_ASSERT_INT_EQUAL(NRF_ERROR_INVALID_ADDR, err_code); 
    
    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return(); 
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gap_address_set(...)
 *
 * The expected encoded packet is:
 *     sd_ble_gap_address_set(ble_gap_addr_t const * const p_addr)
 */
CU_TEST_START(test_encoder_sd_ble_gap_address_set)
{
    reset_unit_test();
    nrf_cunit_mock_call * p_mock_call;
    
    const ble_gap_addr_t ble_gap_addr           = {0xAAu, {0, 0x1u, 0x2u, 0x3u, 0x4u, 0x5u}};
    const uint8_t        expected[]             = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GAP_ADDRESS_SET,
        RPC_BLE_FIELD_PRESENT,
        ble_gap_addr.addr_type,
        ble_gap_addr.addr[0], ble_gap_addr.addr[1], ble_gap_addr.addr[2],
        ble_gap_addr.addr[3], ble_gap_addr.addr[4], ble_gap_addr.addr[5]
    };  
    const uint8_t        cmd_response_message[] = {
        BLE_RPC_PKT_RESP,
        SD_BLE_GAP_ADDRESS_SET, 0, 0, 0, 0};

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof (expected);

    set_cmd_response((uint8_t *) cmd_response_message, sizeof (cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;
    
    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gap_address_set(&ble_gap_addr);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code); 
    
    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();                   
}
CU_TEST_END

#if 0
/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gap_address_get(...)
 * when all input data is NULL   
 *
 * The expected encoded packet is:
 *     sd_ble_gap_address_get(ble_gap_addr_t * const p_addr)
 */
CU_TEST_START(test_encoder_sd_ble_gap_address_get_null_input)
{
    reset_unit_test();
    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gap_address_get(NULL);
    CU_ASSERT_INT_EQUAL(NRF_ERROR_INVALID_ADDR, err_code); 
}
CU_TEST_END

/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gap_address_get(...)
 *
 * The expected encoded packet is:
 *     sd_ble_gap_address_get(ble_gap_addr_t * const p_addr)
 */
CU_TEST_START(test_encoder_sd_ble_gap_address_get)
{
    reset_unit_test();
    nrf_cunit_mock_call * p_mock_call;
    ble_gap_addr_t        ble_gap_addr;    
    
    const uint8_t  expected[]            = 
    {
        SD_BLE_GAP_ADDRESS_GET
    }; 
    const uint8_t cmd_response_message[] = 
    {
        SD_BLE_GAP_ADDRESS_GET,
        0, 0, 0, 0, 
        0xAAu,
        0, 0x1u, 0x2u, 0x3u, 0x4u, 0x5u
    };        

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected); 

    nrf_cunit_expect_call_return((uint8_t *) "transport_rx_pkt_extract", &p_mock_call);
    p_mock_call->result[0] = (uint32_t)cmd_response_message;
    p_mock_call->result[1] = sizeof(cmd_response_message);                
    
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gap_address_get(&ble_gap_addr);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);  
    CU_ASSERT(memcmp(
        &ble_gap_addr, 
        &(cmd_response_message[BLE_OP_CODE_SIZE + BLE_PKT_TYPE_SIZE + BLE_ERR_CODE_SIZE]),
        sizeof(ble_gap_addr)) == 0);    
    
    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();               
}
CU_TEST_END
#endif


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gap_conn_param_update(...)
 * when all input data is valid.
 * sd_ble_gap_conn_param_update(uint16_t conn_handle, 
 *                              ble_gap_conn_params_t const * const p_conn_params)
 */
CU_TEST_START(test_encoder_sd_ble_gap_conn_param_update)
{
    reset_unit_test();
    nrf_cunit_mock_call * p_mock_call;
    
    const uint16_t handle                     = 0x55AAu;
    const ble_gap_conn_params_t p_conn_params = {0xAA55u, 0x55AAu, 0xAA55u, 0x55AAu};
    const uint8_t  expected[]                 = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GAP_CONN_PARAM_UPDATE,
        LSB(handle),
        MSB(handle),
        RPC_BLE_FIELD_PRESENT,        
        LSB(p_conn_params.min_conn_interval),
        MSB(p_conn_params.min_conn_interval),
        LSB(p_conn_params.max_conn_interval),
        MSB(p_conn_params.max_conn_interval),
        LSB(p_conn_params.slave_latency    ),
        MSB(p_conn_params.slave_latency    ),
        LSB(p_conn_params.conn_sup_timeout ),
        MSB(p_conn_params.conn_sup_timeout )
    };      
    const uint8_t  cmd_response_message[]     = {
                                                 BLE_RPC_PKT_RESP,
                                                 SD_BLE_GAP_CONN_PARAM_UPDATE,
                                                 0, 0, 0, 0
                                                };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected); 
    
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));
    
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gap_conn_param_update(handle, &p_conn_params);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();                       
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gap_conn_param_update(...)
 * when all optional input data is NULL.
 * sd_ble_gap_conn_param_update(uint16_t conn_handle, 
 *                              ble_gap_conn_params_t const * const p_conn_params)
 */
CU_TEST_START(test_encoder_sd_ble_gap_conn_param_update_set_null)
{
    reset_unit_test();
    nrf_cunit_mock_call * p_mock_call;
    
    const uint16_t handle                     = 0x55AAu;
    const uint8_t  expected[]                 = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GAP_CONN_PARAM_UPDATE,
        LSB(handle),
        MSB(handle),
        RPC_BLE_FIELD_NOT_PRESENT        
    };      
    const uint8_t  cmd_response_message[]     = {
                                                 BLE_RPC_PKT_RESP,
                                                 SD_BLE_GAP_CONN_PARAM_UPDATE,
                                                 0, 0, 0, 0
                                                };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected); 
    
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));
    
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gap_conn_param_update(handle, NULL);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();                       
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gap_ppcp_get(...),
 * when all optional input data is omitted and the response, when all optional output params are 
 * omitted.
 *
 * sd_ble_gap_ppcp_get(ble_gap_conn_params_t * const p_conn_params)
 */
CU_TEST_START(test_encoder_sd_ble_gap_ppcp_get_null_ptr)
{
    reset_unit_test();
    nrf_cunit_mock_call * p_mock_call;
    
    const uint8_t  expected[]            = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GAP_PPCP_GET,
        RPC_BLE_FIELD_NOT_PRESENT
    };    
    
    const uint8_t cmd_response_message[] = 
    {
        BLE_RPC_PKT_RESP,
        SD_BLE_GAP_PPCP_GET,
        (uint8_t)(NRF_ERROR_INVALID_ADDR),
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 8u),
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 16u),        
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 24u)                
    };
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected); 
    
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));
    
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    uint32_t err_code = sd_ble_gap_ppcp_get(NULL);
    CU_ASSERT_INT_EQUAL(NRF_ERROR_INVALID_ADDR, err_code);
    
    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return(); 
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gap_ppcp_get(...),
 * when all optional input data is valid, and the response, when all optional output params are 
 * present.
 *
 * sd_ble_gap_ppcp_get(ble_gap_conn_params_t * const p_conn_params)
 */
CU_TEST_START(test_encoder_sd_ble_gap_ppcp_get)
{
    reset_unit_test();
    nrf_cunit_mock_call * p_mock_call;
    ble_gap_conn_params_t p_conn_params;

    const uint8_t  expected[]            = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GAP_PPCP_GET,
        RPC_BLE_FIELD_PRESENT
    }; 
    
    const uint8_t cmd_response_message[] = 
    {
        BLE_RPC_PKT_RESP,
        SD_BLE_GAP_PPCP_GET,
        0, 0, 0, 0,                    // API RETURN CODE
        0x19, 0x00, 
        0x19, 0x00, 
        0x00, 0x00, 
        0x00, 0x20
    };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected); 
    
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));
    
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();
    uint32_t err_code = sd_ble_gap_ppcp_get(&p_conn_params);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);
    
    CU_ASSERT(memcmp(
        &p_conn_params, 
        &(cmd_response_message[BLE_OP_CODE_SIZE + BLE_PKT_TYPE_SIZE + RPC_ERR_CODE_SIZE]),
        sizeof(p_conn_params)) == 0);  
    
    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return(); 
}
CU_TEST_END


CU_TEST_START(test_encoder_sd_ble_gap_sec_info_reply)
{
    reset_unit_test();
    nrf_cunit_mock_call * p_mock_call;

    uint16_t conn_handle = 10002;
    ble_gap_enc_info_t gap_enc_info;
    ble_gap_sign_info_t gap_sign_info;

    gap_enc_info.auth = 1;
    gap_enc_info.div  = 10001;
    uint8_t     ltk[] = {0x34, 0x34, 0x34, 0x34,
    		             0x56, 0x56, 0x56, 0x56,
    		             0x67, 0x67, 0x67, 0x67,
    		             0x78, 0x78, 0x78, 0x78 };

    memcpy(gap_enc_info.ltk, ltk, sizeof(ltk));
    gap_enc_info.ltk_len = 33;

    memcpy(gap_sign_info.csrk, ltk, sizeof(ltk));

    const uint8_t        expected[]             =
    {
        BLE_RPC_PKT_CMD,
		SD_BLE_GAP_SEC_INFO_REPLY,
		0x12, 0x27, // conn_handle
		0x01,      // FIELD_PRESENT
		0x11, 0x27, // gap_enc_info.div
		0x34, 0x34, 0x34, 0x34,
		0x56, 0x56, 0x56, 0x56,
		0x67, 0x67, 0x67, 0x67,
		0x78, 0x78, 0x78, 0x78, // gap_enc_info.ltk
		0x43, // gap_enc_info->auth | gap_enc_info->ltk_len << 1
		0x01, // FIELD_PRESENT
		0x34, 0x34, 0x34, 0x34,
		0x56, 0x56, 0x56, 0x56,
		0x67, 0x67, 0x67, 0x67,
		0x78, 0x78, 0x78, 0x78, // gap_sign_info.csrk
    };
    const uint8_t        cmd_response_message[] = {
                                                    BLE_RPC_PKT_RESP,
                                                    SD_BLE_GAP_SEC_INFO_REPLY,
                                                    0, 0, 0, 0
                                                  };

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();

    uint32_t err_code = sd_ble_gap_sec_info_reply(conn_handle, &gap_enc_info, &gap_sign_info);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();
}

CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gap_adv_stop(void),
 * when all optional input data is valid, and the response, when all optional output params are 
 * present.
 *
 * sd_ble_gap_ppcp_get(void)
 */
CU_TEST_START(test_encoder_sd_ble_gap_adv_stop)
{
    reset_unit_test();
    nrf_cunit_mock_call * p_mock_call;

    const uint8_t expected[] = {
        BLE_RPC_PKT_CMD,
        SD_BLE_GAP_ADV_STOP        
    };
    const uint8_t cmd_response_message[] = {
        BLE_RPC_PKT_RESP,
        SD_BLE_GAP_ADV_STOP,
        0, 0, 0, 0
    };

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof (expected);

    set_cmd_response((uint8_t *) cmd_response_message, sizeof (cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();

    uint32_t err_code = sd_ble_gap_adv_stop();
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();
}
CU_TEST_END



CU_SUITE_START_PUBLIC(ble_cmd_encoder_gap_encode_commands)
{
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_device_name_set);
    CU_SUITE_ADD_TEST(nrffosdk_1302_test_encoder_sd_ble_gap_device_name_set_dev_name_absent);
    CU_SUITE_ADD_TEST(nrffosdk_1302_test_encoder_sd_ble_gap_device_name_set_dev_name_and_write_perm_absent);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_device_name_set_internal_error);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_device_name_set_param_error);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_appearance_set_heart_rate);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_appearance_set_internal_error);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_ppcp_set);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_ppcp_set_none_and_invalid_param);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_ppcp_set_conn_params_absent);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_adv_start_simple);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_adv_start_directed_peer);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_adv_start_directed_peer_with_whitelist);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_adv_data_set);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_disconnect); 
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_sec_params_reply_no_optionals);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_sec_params_reply_all_optionals);  
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_appearance_get);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_appearance_get_null);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_device_name_get_buffer_omitted);    
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_device_name_get_length_omitted);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_device_name_get_optionals_omitted);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_device_name_get);
    
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_address_set_null_input);         
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_address_set);
    
#if 0
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_address_get_null_input);             
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_address_get);       
#endif
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_conn_param_update);       
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_conn_param_update_set_null);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_ppcp_get);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_ppcp_get_null_ptr);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_sec_info_reply);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gap_adv_stop);
}
CU_SUITE_END

