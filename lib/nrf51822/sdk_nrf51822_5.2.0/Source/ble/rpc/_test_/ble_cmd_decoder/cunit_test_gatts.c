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
#include "hal_transport.h"
#include "app_util.h"
#include "ble_hci.h"
#include "nordic_common.h"
#include "ble_rpc_defines.h"

//lint -esym(550, exp_write_perm)       // Symbol not accessed
//lint -esym(550, expected_conn_params) // Symbol not accessed
//lint -esym(550, expected_adv_params)  // Symbol not accessed
//lint -esym(550, exp_appearance)       // Symbol not accessed
//lint -esym(550, exp_attr_char_values) // Symbol not accessed
//lint -esym(550, exp_char_md)           // Symbol not accessed
//lint -esym(550, exp_handles)           // Symbol not accessed
//lint -e453
typedef struct {
    uint8_t             op_code;
    uint32_t            err_code;
    uint32_t            ext_data_len;
    uint8_t *           extended_data;
    uint8_t             packet_type;
} expected_transport_data_t;


static uint8_t       sm_expected_cmd_resp[35];
static uint8_t       m_tx_buffer[1200];


static void expected_transport_api_data_setup(nrf_cunit_mock_call             * const p_trans_api_mock_obj,
                                              expected_transport_data_t const * const expected_data)
{
    uint32_t index = 0;
    uint32_t i     = 0;

    memset(sm_expected_cmd_resp, 0, sizeof(sm_expected_cmd_resp));

    sm_expected_cmd_resp[index++] = expected_data->packet_type;

    sm_expected_cmd_resp[index++] = expected_data->op_code;
    sm_expected_cmd_resp[index++] = expected_data->err_code & 0x000000ff;
    sm_expected_cmd_resp[index++] = (expected_data->err_code & 0x0000ff00) >> 8;
    sm_expected_cmd_resp[index++] = (expected_data->err_code & 0x00ff0000) >> 16;
    sm_expected_cmd_resp[index++] = (expected_data->err_code & 0xff000000) >> 24;

    for (; i < expected_data->ext_data_len; i++)
    {
        sm_expected_cmd_resp[index++] = expected_data->extended_data[i];
    }

    p_trans_api_mock_obj->arg[1] = (uint32_t)sm_expected_cmd_resp;
    p_trans_api_mock_obj->arg[2] = index;
}


/* This test case will test a simple decoding of gatts_service_add(...) with the parameters:
 * service type = 0x01 (BLE_GATTS_SRVC_TYPE_PRIMARY)
 * uuid         = 2902
 * type         = 0x01 (BLE).
 * response with error code set to NRF_SUCCESS and Handle Value 0x05.
 */
CU_TEST_START(gatts_service_add_simple)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_sd_mock_call;
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;
     ble_uuid_t                 expected_uuid;
     uint8_t                    expected_handle[] = {0x05, 0x00};

     // Expected data to the SoftDevice, those data should be decoded from the received in_data and passed to SoftDevice as parameters.
     expected_uuid.type = BLE_UUID_TYPE_BLE;
     expected_uuid.uuid = BLE_UUID_HEART_RATE_SERVICE;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect a success, and the returned handle value to be encoded, and passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_SERVICE_ADD;
     expected_data.err_code      = NRF_SUCCESS;
     expected_data.ext_data_len  = 2;
     expected_data.extended_data = expected_handle;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gatts_service_add", &p_sd_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     p_sd_mock_call->arg[0]    = BLE_GATTS_SRVC_TYPE_PRIMARY;
     p_sd_mock_call->arg[1]    = (uint32_t)&expected_uuid;
     p_sd_mock_call->result[0] = 0x05;                          // Handle value to be returned from SoftDevice.

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_SERVICE_ADD, 0x01, 0x01, 0x0D, 0x18, 0x01, 0x01};

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = sizeof(in_data);

     ble_rpc_cmd_handle(NULL, 0);

     (void)expected_uuid;
}
CU_TEST_END


/* This test case will test NULL is correctly decoded, to allow SoftDevice to return the error.
 * service type = 0x01 (BLE_GATTS_SRVC_TYPE_PRIMARY)
 * uuid         = NULL
 * type         = NULL
 * response with error code set to NRF_ERROR_INVALID_PARAM and no handle value.
 */
CU_TEST_START(gatts_service_error)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_sd_mock_call;
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_SERVICE_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_PARAM;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gatts_service_add", &p_sd_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     p_sd_mock_call->error     = NRF_ERROR_INVALID_PARAM;
     p_sd_mock_call->arg[0]    = BLE_GATTS_SRVC_TYPE_PRIMARY;
     p_sd_mock_call->arg[1]    = 0;

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_SERVICE_ADD, 0x01, 0x00, 0x00};

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = sizeof(in_data);

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will test NULL is correctly decoded to initialize the system attributes.
 * conn_handle   = 0x05 (Just a number for test)
 * sys_attr_data = NULL
 * length        = 0
 */
CU_TEST_START(gatts_sys_attr_set_null)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
    nrf_cunit_mock_call *      p_sd_mock_call;
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_data;

    // Expected data to the SoftDevice, those data should be decoded from the received in_data and passed to SoftDevice as parameters.
    uint16_t                   expected_handle = 0x05;
    uint8_t *                  expected_attr   = NULL;
    uint16_t                   expected_len    = 0;


    // Expected data to the Transport Layer, when the SoftDevice call returns, we expect a success, and the returned handle value to be encoded, and passed to the tansport layer for transmission.
    expected_data.op_code       = SD_BLE_GATTS_SYS_ATTR_SET;
    expected_data.err_code      = NRF_SUCCESS;
    expected_data.ext_data_len  = 0;
    expected_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gatts_sys_attr_set", &p_sd_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    p_sd_mock_call->arg[0] = expected_handle;
    p_sd_mock_call->arg[1] = (uint32_t)expected_attr;
    p_sd_mock_call->arg[2] = expected_len;                          // Handle value to be returned from SoftDevice.


     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_SYS_ATTR_SET, 0x05, 0x00, 0x00};

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = sizeof(in_data);

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will test a set of CCCDs is correctly decoded and sent to SoftDevice. (Example data is from HID Keyboard)
 * conn_handle   = 0x06 (Just a number for test)
 * sys_attr_data = {0x0B, 0x00, 0x02, 0x00, 0x00, 0x00, 0x14, 0x00, 0x02, 0x00, 0x00, 0x00,
 *                  0x1A, 0x00, 0x02, 0x00, 0x00, 0x00, 0x23, 0x00, 0x02, 0x00, 0x00, 0x00, 0x2F, 0x65}
 * length        = 26
 */
CU_TEST_START(gatts_sys_attr_set_cccds)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
    nrf_cunit_mock_call *      p_sd_mock_call;
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_data;

    // Expected data to the SoftDevice, those data should be decoded from the received in_data and passed to SoftDevice as parameters.
    uint16_t                   expected_handle = 0x06;
    uint8_t                    expected_attr[] = {0x0B, 0x00, 0x02, 0x00, 0x00, 0x00, 0x14, 0x00,
                                                  0x02, 0x00, 0x00, 0x00, 0x1A, 0x00, 0x02, 0x00,
                                                  0x00, 0x00, 0x23, 0x00, 0x02, 0x00, 0x00, 0x00,
                                                  0x2F, 0x65};
    uint16_t                   expected_len    = 26;


    // Expected data to the Transport Layer, when the SoftDevice call returns, we expect a success, and the returned handle value to be encoded, and passed to the tansport layer for transmission.
    expected_data.op_code       = SD_BLE_GATTS_SYS_ATTR_SET;
    expected_data.err_code      = NRF_SUCCESS;
    expected_data.ext_data_len  = 0;
    expected_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gatts_sys_attr_set", &p_sd_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    p_sd_mock_call->arg[0] = expected_handle;
    p_sd_mock_call->arg[1] = (uint32_t)expected_attr;
    p_sd_mock_call->arg[2] = expected_len;


     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_SYS_ATTR_SET,
                          0x06, 0x00, 0x01, 0x1A, 0x00,
                          0x0B, 0x00, 0x02, 0x00, 0x00, 0x00, 0x14, 0x00, 0x02, 0x00, 0x00, 0x00,
                          0x1A, 0x00, 0x02, 0x00, 0x00, 0x00, 0x23, 0x00, 0x02, 0x00, 0x00, 0x00,
                          0x2F, 0x65};

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = sizeof(in_data);

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will ensure an empty hvx packet can be correctly decoded.
 * conn_handle   = 0x07 (Just a number for test)
 * p_hvx_param  = NULL
 */
CU_TEST_START(gatts_hvx_null)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
    nrf_cunit_mock_call *      p_sd_mock_call;
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_data;

    // Expected data to the SoftDevice, those data should be decoded from the received in_data and passed to SoftDevice as parameters.
    uint16_t                   expected_handle = 0x07;
    ble_gatts_hvx_params_t *   expected_params = NULL;

    // Expected data to the Transport Layer, when the SoftDevice call returns, we expect a success, and the returned handle value to be encoded, and passed to the tansport layer for transmission.
    expected_data.op_code       = SD_BLE_GATTS_HVX;
    expected_data.err_code      = NRF_SUCCESS;
    expected_data.ext_data_len  = 0;
    expected_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gatts_hvx", &p_sd_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    p_sd_mock_call->arg[0] = expected_handle;
    p_sd_mock_call->arg[1] = (uint32_t)expected_params;

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);

     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_HVX, 0x07, 0x00, 0x00};

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = sizeof(in_data);

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will ensure an notification packet can be correctly decoded.
 * HRM notification is used as example.
 * conn_handle   = 0x07 (Just a number for test)
 * p_hvx_param  = Notification w. HRM flags and measurement data included. (Value handle = 0x0A)
 */
CU_TEST_START(gatts_hvx_notification)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
    nrf_cunit_mock_call *      p_sd_mock_call;
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_data;
    uint16_t                   expected_written_length = 2;

    // Expected data to the SoftDevice, those data should be decoded from the received in_data and passed to SoftDevice as parameters.
    uint16_t               expected_handle = 0x07;
    ble_gatts_hvx_params_t expected_params;
    uint16_t               expected_hvx_data_len = 3;
    uint8_t                expected_hvx_data[] = {0x07, 0xB4, 0x01};

    expected_params.handle = 0x0A; // Value handle used for test.
    expected_params.type   = BLE_GATT_HVX_NOTIFICATION;
    expected_params.offset = 0;
    expected_params.p_len  = &expected_hvx_data_len;
    expected_params.p_data = expected_hvx_data;

    // Expected data to the Transport Layer, when the SoftDevice call returns, we expect a success, and the returned handle value to be encoded, and passed to the tansport layer for transmission.
    expected_data.op_code       = SD_BLE_GATTS_HVX;
    expected_data.err_code      = NRF_SUCCESS;
    expected_data.ext_data_len  = 2;
    expected_data.extended_data = (uint8_t*)&expected_written_length;
    expected_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gatts_hvx", &p_sd_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    p_sd_mock_call->arg[0]    = expected_handle;
    p_sd_mock_call->arg[1]    = (uint32_t)&expected_params;
    p_sd_mock_call->result[0] = expected_written_length;

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);

     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_HVX,
                          0x07, 0x00, 0x01, 0x0A, 0x00, 0x01, 0x00, 0x00, 0x01, 0x03, 0x00, 0x01, 0x07, 0xB4, 0x01};

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = sizeof(in_data);

     ble_rpc_cmd_handle(NULL, 0);

     (void)expected_params;
}
CU_TEST_END


/* This test case will ensure an notification packet can be correctly decoded even if length and data fields in structure are NULL.
 * conn_handle   = 0x07 (Just a number for test)
 * p_hvx_param  = Notification where length and data are NULL. (Value handle = 0x0A)
 */
CU_TEST_START(gatts_hvx_notification_null_length_and_data)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
    nrf_cunit_mock_call *      p_sd_mock_call;
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_data;

    // Expected data to the SoftDevice, those data should be decoded from the received in_data and passed to SoftDevice as parameters.
    uint16_t               expected_handle = 0x07;
    ble_gatts_hvx_params_t expected_params;

    expected_params.handle = 0x0A; // Value handle used for test.
    expected_params.type   = BLE_GATT_HVX_NOTIFICATION;
    expected_params.offset = 0;
    expected_params.p_len  = NULL;
    expected_params.p_data = NULL;

    // Expected data to the Transport Layer, when the SoftDevice call returns, we expect a success, and the returned handle value to be encoded, and passed to the tansport layer for transmission.
    expected_data.op_code       = SD_BLE_GATTS_HVX;
    expected_data.err_code      = NRF_ERROR_INVALID_ADDR;
    expected_data.ext_data_len  = 0;
    expected_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gatts_hvx", &p_sd_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    p_sd_mock_call->arg[0] = expected_handle;
    p_sd_mock_call->arg[1] = (uint32_t)&expected_params;
    p_sd_mock_call->error  = NRF_ERROR_INVALID_ADDR;

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);

     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_HVX, 0x07, 0x00, 0x01, 0x0A, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = sizeof(in_data);

     ble_rpc_cmd_handle(NULL, 0);

     (void)expected_params;
}
CU_TEST_END


/* This test case will ensure an notification packet can be correctly decoded even if length is specified but no data is provided.
 * conn_handle   = 0x07 (Just a number for test)
 * p_hvx_param  = Notification where length and data are NULL. (Value handle = 0x0A)
 */
CU_TEST_START(gatts_hvx_notification_null_data_w_length)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
    nrf_cunit_mock_call *      p_sd_mock_call;
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_data;
    uint16_t                   expected_written_length = 0;

    // Expected data to the SoftDevice, those data should be decoded from the received in_data and passed to SoftDevice as parameters.
    uint16_t               expected_handle = 0x07;
    ble_gatts_hvx_params_t expected_params;
    uint16_t               expected_hvx_data_len = 1;

    expected_params.handle = 0x0A; // Value handle used for test.
    expected_params.type   = BLE_GATT_HVX_NOTIFICATION;
    expected_params.offset = 0;
    expected_params.p_len  = &expected_hvx_data_len;
    expected_params.p_data = NULL;

    // Expected data to the Transport Layer, when the SoftDevice call returns, we expect a success, and the returned handle value to be encoded, and passed to the tansport layer for transmission.
    expected_data.op_code       = SD_BLE_GATTS_HVX;
    expected_data.err_code      = NRF_SUCCESS;
    expected_data.ext_data_len  = 2;
    expected_data.extended_data = (uint8_t*)&expected_written_length;
    expected_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gatts_hvx", &p_sd_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    p_sd_mock_call->arg[0]    = expected_handle;
    p_sd_mock_call->arg[1]    = (uint32_t)&expected_params;
    p_sd_mock_call->result[0] = expected_written_length;

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);

     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_HVX,
                          0x07, 0x00,                   // handle
                          0x01,                         // hvx present
                          0x0A, 0x00,                   // handle
                          0x01,                         // type
                          0x00, 0x00,                   // offset
                          0x01,                         // len present
                          0x01, 0x00,                   // len
                          0x00};                        // data present

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = sizeof(in_data) + 1;

     ble_rpc_cmd_handle(NULL, 0);

     (void)expected_params;
}
CU_TEST_END


/* This test case will ensure that system attributes can be requested and correctly decoded for
 * even with NULL pointers.
 * conn_handle     = 0x00 (Just a number for test)
 * p_sys_attr_data = NULL
 * p_len           = NULL
 */
CU_TEST_START(gatts_sys_attr_get_null)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
    nrf_cunit_mock_call *      p_sd_mock_call;
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;

    expected_transport_data_t  expected_data;

    // Expected data to the SoftDevice, those data should be decoded from the received in_data and passed to SoftDevice as parameters.
    uint16_t               expected_handle = 0x00;

    // Expected data to the Transport Layer, when the SoftDevice call returns, we expect a success, and the returned handle value to be encoded, and passed to the tansport layer for transmission.
    expected_data.op_code       = SD_BLE_GATTS_SYS_ATTR_GET;
    expected_data.err_code      = NRF_ERROR_INVALID_ADDR;
    expected_data.ext_data_len  = 0;
    expected_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gatts_sys_attr_get", &p_sd_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    p_sd_mock_call->arg[0]    = expected_handle;
    p_sd_mock_call->arg[1]    = (uint32_t)NULL;
    p_sd_mock_call->arg[2]    = (uint32_t)NULL;
    p_sd_mock_call->error     = NRF_ERROR_INVALID_ADDR;

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);

     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_SYS_ATTR_GET, 0x00, 0x00, 0x00, 0x00};

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = sizeof(in_data);

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will ensure that system attributes can be requested and correctly decoded for
 * for data length pointer valid, but value 0, and p_data is NULL pointer.
 * conn_handle     = 0x08 (Just a number for test)
 * p_sys_attr_data = NULL
 * *p_len          = 0
 * Return length
 * *p_len         = 32
 */
CU_TEST_START(gatts_sys_attr_get_data_length)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
    nrf_cunit_mock_call *      p_sd_mock_call;
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_data;

    uint16_t sys_attr_len_response_size = 32;
    // SoftDevice should return 32 bytes of data available and this should be encoded by the decoder in the response.
    // Following the length, is the data present field, which is expected to be 0x00 as NULL input data return just the length without data.
    uint8_t                   expected_response_data[] = {32, 0x00, 0x00};

    // Expected data to the SoftDevice, those data should be decoded from the received in_data and passed to SoftDevice as parameters.
    uint16_t               expected_handle = 0x08;
    uint16_t               expected_length = 0;

    // Expected data to the Transport Layer, when the SoftDevice call returns, we expect a success, and the returned handle value to be encoded, and passed to the tansport layer for transmission.
    expected_data.op_code       = SD_BLE_GATTS_SYS_ATTR_GET;
    expected_data.err_code      = NRF_SUCCESS;
    expected_data.ext_data_len  = 3;
    expected_data.extended_data = expected_response_data;
    expected_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gatts_sys_attr_get", &p_sd_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    p_sd_mock_call->arg[0]    = expected_handle;
    p_sd_mock_call->arg[1]    = (uint32_t)NULL;
    p_sd_mock_call->arg[2]    = (uint32_t)&expected_length;
    p_sd_mock_call->result[2] = sys_attr_len_response_size;

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);

     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_SYS_ATTR_GET, 0x08, 0x00, 0x01, 0x00, 0x00, 0x00};

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = sizeof(in_data);

     ble_rpc_cmd_handle(NULL, 0);

     (void)expected_length;
}
CU_TEST_END


/* This test case will ensure that system attributes can be requested and correctly decoded for
 * for data length pointer valid and valid data pointer. The exact data length will also be returned.
 * conn_handle      = 0x08 (Just a number for test)
 * *p_sys_attr_data = 64 bytes memory.
 * *p_len           = 64
 * Return length
 * *p_sys_attr_data = System Attributes.
 * *p_len           = 32
 */
CU_TEST_START(gatts_sys_attr_get_data)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
    nrf_cunit_mock_call *      p_sd_mock_call;
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_data;

    uint16_t sys_attr_len_response = 26;
    uint8_t  sys_attr_response[]   = {0x0B, 0x00, 0x02, 0x00, 0x00, 0x00, 0x14, 0x00, 0x02, 0x00,
                                      0x00, 0x00, 0x1A, 0x00, 0x02, 0x00, 0x00, 0x00, 0x23, 0x00,
                                      0x02, 0x00, 0x00, 0x00, 0x2F, 0x65};

    // Expected data to the SoftDevice, those data should be decoded from the received in_data and passed to SoftDevice as parameters.
    uint16_t               expected_handle = 0x08;
    uint16_t               expected_length = sizeof(sys_attr_response);

    // SoftDevice should return 32 bytes of data available and this should be encoded by the decoder in the response.
    // Following the length, is the data present field, which is expected to be 0x00 as NULL input data return just the length without data.
    uint8_t expected_response_data[3 + sizeof(sys_attr_response)];

    expected_response_data[0] = 0x1A;
    expected_response_data[1] = 0x00;
    expected_response_data[2] = 0x01;
    memcpy(&expected_response_data[3], sys_attr_response, sizeof(sys_attr_response));

    // Expected data to the Transport Layer, when the SoftDevice call returns, we expect a success, and the returned handle value to be encoded, and passed to the tansport layer for transmission.
    expected_data.op_code       = SD_BLE_GATTS_SYS_ATTR_GET;
    expected_data.err_code      = NRF_SUCCESS;
    expected_data.ext_data_len  = sizeof(expected_response_data);
    expected_data.extended_data = expected_response_data;
    expected_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gatts_sys_attr_get", &p_sd_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    p_sd_mock_call->arg[0]    = expected_handle;
    p_sd_mock_call->arg[1]    = (uint32_t)0xDEADBEFF;       // We just expect something different from NULL.
    p_sd_mock_call->arg[2]    = (uint32_t)&expected_length;
    p_sd_mock_call->result[1] = (uint32_t)sys_attr_response;
    p_sd_mock_call->result[2] = sys_attr_len_response;

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);

     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_SYS_ATTR_GET, 0x08, 0x00, 0x01, 0x1A, 0x00, 0x01};

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = sizeof(in_data);

     ble_rpc_cmd_handle(NULL, 0);

     (void)expected_length;
}
CU_TEST_END


/* This test case will ensure that a value can be set on a given attribute with the provided handle.
 * This test ensure correct behavior when NULL pointers are provided.
 * value_handl      = 0x09 (Just a number for test)
 * offset           = 5
 * p_len            = NULL
 * p_value          = NULL
 * Response
 * NRF_ERROR_INVALID_ADDRESS
 */
CU_TEST_START(gatts_value_set_null)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
    nrf_cunit_mock_call *      p_sd_mock_call;
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_data;

    // Expected data to the SoftDevice, those data should be decoded from the received in_data and passed to SoftDevice as parameters.
    uint16_t               expected_handle = 0x09;
    uint16_t               expected_offset = 0x05;

    // SoftDevice should return 32 bytes of data available and this should be encoded by the decoder in the response.
    // Following the length, is the data present field, which is expected to be 0x00 as NULL input data return just the length without data.

    // Expected data to the Transport Layer, when the SoftDevice call returns, we expect a success, and the returned handle value to be encoded, and passed to the tansport layer for transmission.
    expected_data.op_code       = SD_BLE_GATTS_VALUE_SET;
    expected_data.err_code      = NRF_ERROR_INVALID_ADDR;
    expected_data.ext_data_len  = 0;
    expected_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gatts_value_set", &p_sd_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    p_sd_mock_call->arg[0]    = expected_handle;
    p_sd_mock_call->arg[1]    = expected_offset;
    p_sd_mock_call->arg[2]    = (uint32_t)NULL;
    p_sd_mock_call->arg[3]    = (uint32_t)NULL;
    p_sd_mock_call->error     = NRF_ERROR_INVALID_ADDR;

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);

     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_VALUE_SET, 0x09, 0x00, 0x05, 0x00, 0x00, 0x00};

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = sizeof(in_data);

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will ensure that a value can be set on a given attribute with the provided handle.
 * This test ensure correct behavior when data length is provided but NULL pointer for the value.
 * value_handl      = 0x09 (Just a number for test)
 * offset           = 0
 * *p_len           = 16
 * p_value          = NULL
 * Response
 * NRF_ERROR_INVALID_ADDRESS
 */
CU_TEST_START(gatts_value_set_null_data)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output

    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_data;


    // SoftDevice should return 32 bytes of data available and this should be encoded by the decoder in the response.
    // Following the length, is the data present field, which is expected to be 0x00 as NULL input data return just the length without data.

    // Expected data to the Transport Layer, when the SoftDevice call returns, we expect a success, and the returned handle value to be encoded, and passed to the tansport layer for transmission.
    expected_data.op_code       = SD_BLE_GATTS_VALUE_SET;
    expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
    expected_data.ext_data_len  = 0;
    expected_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected Transport Layer API call
    expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);

    // ----- Execute the command to be tested. -----
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                         SD_BLE_GATTS_VALUE_SET,
                         0x09, 0x00,
                         0x00, 0x00,
                         0x01,
                         0x10, 0x00,
                         0x00};

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will ensure that a value can be set on a given attribute with the provided handle.
 * This test ensure correct behavior when data length is provided with data value.
 * The number of bytes written are returned and it's validated that the value is correctly encoded.
 * value_handl      = 0x09 (Just a number for test)
 * offset           = 0
 * *p_len           = 8
 * *p_value         = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x11, 0x55, 0xFF}
 * Response
 * NRF_SUCCESS
 * *p_len           = 4
 *
 */
CU_TEST_START(gatts_value_set)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
    nrf_cunit_mock_call *      p_sd_mock_call;
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_transport_data;

    // Expected data to the SoftDevice, those data should be decoded from the received in_data and passed to SoftDevice as parameters.
    uint16_t               expected_handle = 0x09;
    uint16_t               expected_offset = 0x00;
    uint16_t               expected_length = 8;
    uint8_t                expected_data[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x11, 0x55, 0xFF};
    uint16_t               returned_length = 4;

    // SoftDevice should return 32 bytes of data available and this should be encoded by the decoder in the response.
    // Following the length, is the data present field, which is expected to be 0x00 as NULL input data return just the length without data.

    // Expected data to the Transport Layer, when the SoftDevice call returns, we expect a success, and the returned handle value to be encoded, and passed to the tansport layer for transmission.
    uint8_t  extended_data[]              = {0x04, 0x00};
    expected_transport_data.op_code       = SD_BLE_GATTS_VALUE_SET;
    expected_transport_data.err_code      = NRF_SUCCESS;
    expected_transport_data.ext_data_len  = 2;
    expected_transport_data.extended_data = extended_data;
    expected_transport_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gatts_value_set", &p_sd_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    p_sd_mock_call->arg[0]    = expected_handle;
    p_sd_mock_call->arg[1]    = expected_offset;
    p_sd_mock_call->arg[2]    = (uint32_t)&expected_length;
    p_sd_mock_call->arg[3]    = (uint32_t)expected_data;
    p_sd_mock_call->result[2] = returned_length;

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_transport_data);

     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_VALUE_SET,
                          0x09, 0x00, 0x00, 0x00, 0x01, 0x08, 0x00, 0x01, 0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x11, 0x55, 0xFF};

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = sizeof(in_data);

     ble_rpc_cmd_handle(NULL, 0);

     (void)expected_length;
}
CU_TEST_END


CU_TEST_START(gatts_char_add_all_fields_present)
{
    nrf_cunit_mock_call *      p_sd_mock_call;
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_transport_data;

    uint8_t  extended_data[]              = {0x23, 0x00, // value_handle
                                             0x24, 0x00, // user_desc_handle
                                             0x25, 0x00, // cccd_handle
                                             0x26, 0x00};// sccd_handle

    expected_transport_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
    expected_transport_data.err_code      = NRF_SUCCESS;
    expected_transport_data.ext_data_len  = 8;
    expected_transport_data.extended_data = extended_data;
    expected_transport_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gatts_characteristic_add", &p_sd_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected Transport Layer API call
    expected_transport_api_data_setup(p_transport_write_mock_call, &expected_transport_data);

    // ----- Execute the command to be tested. -----
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                         SD_BLE_GATTS_CHARACTERISTIC_ADD,
                         0x01, 0x00,                   // Service handle
                         0x01,                         // Char metadata present
                         0xFE,                         // char_md.char_props
                         0xFE,                         // char_md.char_ext_props
                         0x12, 0x00,                   // User desc max size
                         0x05, 0x00,                   // User desc size
                         0x01,                         // User desc present
                         0x01, 0x02, 0x03, 0x04, 0x05, // User desc
                         0x01,                         // Presentation format present
                         0xAA,                         // PF: Format
                         0xFD,                         // PF: Exponent (-3 in 2's complement)
                         0xCC, 0xDD,                   // PF: Unit
                         0xEE,                         // PF: Namespace
                         0x88, 0x99,                   // PF: Description
                         0x01,                         // User desc metadata present
                         0x21, 0x21, 0x13,             // read perm, write perm, auth
                         0x01,                         // CCCD metadata present
                         0x21, 0x21, 0x13,             // read perm, write perm, auth
                         0x01,                         // SCCD metadata present
                         0x21, 0x21, 0x13,             // read perm, write perm, auth
                         0x01,                         // Char attribute present
                         0x01,                         // UUID present
                         0xAB, 0xCD, 0x1,              // UUID, UUID type
                         0x01,                         // Char attribute, attribute metadata present
                         0x21, 0x21, 0x13,             // read perm, write perm, auth
                         0x05, 0x00,                   // value length
                         0x78, 0x00,                   // value offset
                         0x79, 0x00,                   // max value length
                         0x01,                         // attribute data present
                         0xEE, 0xDD, 0xCC, 0xBB, 0xAA, // attribute data
                         0x01                          // Handles present
                         };

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    uint16_t                 exp_handle;
    ble_gatts_char_md_t      exp_char_md;
    ble_gatts_attr_t         exp_attr_char_values;

    exp_handle = 0x0001;

    // Metadata
    exp_char_md.char_props.broadcast       = 0x0;
    exp_char_md.char_props.read            = 0x1;
    exp_char_md.char_props.write_wo_resp   = 0x1;
    exp_char_md.char_props.write           = 0x1;
    exp_char_md.char_props.notify          = 0x1;
    exp_char_md.char_props.indicate        = 0x1;
    exp_char_md.char_props.auth_signed_wr  = 0x1;
    exp_char_md.char_ext_props.reliable_wr = 0x0;
    exp_char_md.char_ext_props.wr_aux      = 0x1;
    uint8_t char_user_desc[5]              = {0x01, 0x02, 0x03, 0x04, 0x05};
    exp_char_md.p_char_user_desc           = char_user_desc;
    exp_char_md.char_user_desc_max_size    = 0x0012;
    exp_char_md.char_user_desc_size        = 0x0005;

    ble_gatts_char_pf_t char_pf;
    char_pf.format     = 0xAA;
    char_pf.exponent   = -3;
    char_pf.unit       = 0xDDCC;
    char_pf.name_space = 0xEE;
    char_pf.desc       = 0x9988;

    ble_gatts_attr_md_t user_desc_md;
    user_desc_md.read_perm.sm  = 0x1;
    user_desc_md.read_perm.lv  = 0x2;
    user_desc_md.write_perm.sm = 0x1;
    user_desc_md.write_perm.lv = 0x2;
    user_desc_md.vlen          = 0x1;
    user_desc_md.vloc          = 0x1;
    user_desc_md.rd_auth       = 0x0;
    user_desc_md.wr_auth       = 0x1;

    ble_gatts_attr_md_t cccd_md;
    cccd_md.read_perm.sm       = 0x1;
    cccd_md.read_perm.lv       = 0x2;
    cccd_md.write_perm.sm      = 0x1;
    cccd_md.write_perm.lv      = 0x2;
    cccd_md.vlen               = 0x1;
    cccd_md.vloc               = 0x1;
    cccd_md.rd_auth            = 0x0;
    cccd_md.wr_auth            = 0x1;

    ble_gatts_attr_md_t sccd_md;
    sccd_md.read_perm.sm       = 0x1;
    sccd_md.read_perm.lv       = 0x2;
    sccd_md.write_perm.sm      = 0x1;
    sccd_md.write_perm.lv      = 0x2;
    sccd_md.vlen               = 0x1;
    sccd_md.vloc               = 0x1;
    sccd_md.rd_auth            = 0x0;
    sccd_md.wr_auth            = 0x1;

    exp_char_md.p_char_pf        = &char_pf;
    exp_char_md.p_user_desc_md   = &user_desc_md;
    exp_char_md.p_cccd_md        = &cccd_md;
    exp_char_md.p_sccd_md        = &sccd_md;

    // Attributes
    ble_uuid_t uuid;
    uuid.uuid = 0xCDAB;
    uuid.type = 0x1;
    exp_attr_char_values.p_uuid = &uuid;

    ble_gatts_attr_md_t attr_md;
    attr_md.read_perm.sm       = 0x1;
    attr_md.read_perm.lv       = 0x2;
    attr_md.write_perm.sm      = 0x1;
    attr_md.write_perm.lv      = 0x2;
    attr_md.vlen               = 0x1;
    attr_md.vloc               = 0x1;
    attr_md.rd_auth            = 0x0;
    attr_md.wr_auth            = 0x1;

    exp_attr_char_values.p_attr_md = &attr_md;
    exp_attr_char_values.init_len  = 0x0005;
    exp_attr_char_values.init_offs = 0x0078;
    exp_attr_char_values.max_len   = 0x0079;

    uint8_t attribute_data[5] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};
    exp_attr_char_values.p_value = attribute_data;

    ble_gatts_char_handles_t exp_result_handles;
    exp_result_handles.value_handle     = 0x0023;
    exp_result_handles.user_desc_handle = 0x0024;
    exp_result_handles.cccd_handle      = 0x0025;
    exp_result_handles.sccd_handle      = 0x0026;

    p_sd_mock_call->arg[0]    = (uint32_t)&exp_handle;
    p_sd_mock_call->arg[1]    = (uint32_t)&exp_char_md;
    p_sd_mock_call->arg[2]    = (uint32_t)&exp_attr_char_values;
    p_sd_mock_call->result[0] = (uint32_t)&exp_result_handles;

    ble_rpc_cmd_handle(NULL, 0);

    (void)exp_handle;
    (void)exp_result_handles;

}
CU_TEST_END


CU_TEST_START(gatts_char_add_without_char_metadata)
{
    nrf_cunit_mock_call *      p_sd_mock_call;
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_transport_data;

    uint8_t  extended_data[]              = {0x23, 0x00, // value_handle
                                             0x24, 0x00, // user_desc_handle
                                             0x25, 0x00, // cccd_handle
                                             0x26, 0x00};// sccd_handle

    expected_transport_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
    expected_transport_data.err_code      = NRF_SUCCESS;
    expected_transport_data.ext_data_len  = 8;
    expected_transport_data.extended_data = extended_data;
    expected_transport_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gatts_characteristic_add", &p_sd_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected Transport Layer API call
    expected_transport_api_data_setup(p_transport_write_mock_call, &expected_transport_data);

    // ----- Execute the command to be tested. -----
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                         SD_BLE_GATTS_CHARACTERISTIC_ADD,
                         0x01, 0x00,                   // Service handle
                         0x00,                         // Char metadata present
                         0x01,                         // Char attribute present
                         0x01,                         // UUID present
                         0xAB, 0xCD, 0x1,              // UUID, UUID type
                         0x01,                         // Char attribute, attribute metadata present
                         0x21, 0x21, 0x13,             // read perm, write perm, auth
                         0x05, 0x00,                   // value length
                         0x78, 0x00,                   // value offset
                         0x79, 0x00,                   // max value length
                         0x01,                         // attribute data present
                         0xEE, 0xDD, 0xCC, 0xBB, 0xAA, // attribute data
                         0x01                          // Handles present
                         };

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    uint16_t                 exp_handle;
    ble_gatts_attr_t         exp_attr_char_values;

    exp_handle = 0x0001;

    // Attributes
    ble_uuid_t uuid;
    uuid.uuid = 0xCDAB;
    uuid.type = 0x1;
    exp_attr_char_values.p_uuid = &uuid;

    ble_gatts_attr_md_t attr_md;
    attr_md.read_perm.sm       = 0x1;
    attr_md.read_perm.lv       = 0x2;
    attr_md.write_perm.sm      = 0x1;
    attr_md.write_perm.lv      = 0x2;
    attr_md.vlen               = 0x1;
    attr_md.vloc               = 0x1;
    attr_md.rd_auth            = 0x0;
    attr_md.wr_auth            = 0x1;

    exp_attr_char_values.p_attr_md = &attr_md;
    exp_attr_char_values.init_len  = 0x0005;
    exp_attr_char_values.init_offs = 0x0078;
    exp_attr_char_values.max_len   = 0x0079;

    uint8_t attribute_data[5] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};
    exp_attr_char_values.p_value = attribute_data;

    ble_gatts_char_handles_t exp_result_handles;
    exp_result_handles.value_handle     = 0x0023;
    exp_result_handles.user_desc_handle = 0x0024;
    exp_result_handles.cccd_handle      = 0x0025;
    exp_result_handles.sccd_handle      = 0x0026;

    p_sd_mock_call->arg[0]    = (uint32_t)&exp_handle;
    p_sd_mock_call->arg[1]    = (uint32_t)NULL;
    p_sd_mock_call->arg[2]    = (uint32_t)&exp_attr_char_values;
    p_sd_mock_call->result[0] = (uint32_t)&exp_result_handles;

    ble_rpc_cmd_handle(NULL, 0);

    (void)exp_handle;
    (void)exp_result_handles;

}
CU_TEST_END


CU_TEST_START(gatts_char_add_without_char_attributes)
{
    nrf_cunit_mock_call *      p_sd_mock_call;
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_transport_data;

    uint8_t  extended_data[]              = {0x23, 0x00, // value_handle
                                             0x24, 0x00, // user_desc_handle
                                             0x25, 0x00, // cccd_handle
                                             0x26, 0x00};// sccd_handle

    expected_transport_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
    expected_transport_data.err_code      = NRF_SUCCESS;
    expected_transport_data.ext_data_len  = 8;
    expected_transport_data.extended_data = extended_data;
    expected_transport_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gatts_characteristic_add", &p_sd_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected Transport Layer API call
    expected_transport_api_data_setup(p_transport_write_mock_call, &expected_transport_data);

    // ----- Execute the command to be tested. -----
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                         SD_BLE_GATTS_CHARACTERISTIC_ADD,
                         0x01, 0x00,                   // Service handle
                         0x01,                         // Char metadata present
                         0xFE,                         // char_md.char_props
                         0xFE,                         // char_md.char_ext_props
                         0x12, 0x00,                   // User desc max size
                         0x05, 0x00,                   // User desc size
                         0x01,                         // User desc present
                         0x01, 0x02, 0x03, 0x04, 0x05, // User desc
                         0x01,                         // Presentation format present
                         0xAA,                         // PF: Format
                         0xFD,                         // PF: Exponent (-3 in 2's complement)
                         0xCC, 0xDD,                   // PF: Unit
                         0xEE,                         // PF: Namespace
                         0x88, 0x99,                   // PF: Description
                         0x01,                         // User desc metadata present
                         0x21, 0x21, 0x13,             // read perm, write perm, auth
                         0x01,                         // CCCD metadata present
                         0x21, 0x21, 0x13,             // read perm, write perm, auth
                         0x01,                         // SCCD metadata present
                         0x21, 0x21, 0x13,             // read perm, write perm, auth
                         0x00,                         // Char attribute present
                         0x01                          // Handles present
                         };

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    uint16_t                 exp_handle;
    ble_gatts_char_md_t      exp_char_md;

    exp_handle = 0x0001;

    // Metadata
    exp_char_md.char_props.broadcast       = 0x0;
    exp_char_md.char_props.read            = 0x1;
    exp_char_md.char_props.write_wo_resp   = 0x1;
    exp_char_md.char_props.write           = 0x1;
    exp_char_md.char_props.notify          = 0x1;
    exp_char_md.char_props.indicate        = 0x1;
    exp_char_md.char_props.auth_signed_wr  = 0x1;
    exp_char_md.char_ext_props.reliable_wr = 0x0;
    exp_char_md.char_ext_props.wr_aux      = 0x1;
    uint8_t char_user_desc[5]              = {0x01, 0x02, 0x03, 0x04, 0x05};
    exp_char_md.p_char_user_desc           = char_user_desc;
    exp_char_md.char_user_desc_max_size    = 0x0012;
    exp_char_md.char_user_desc_size        = 0x0005;

    ble_gatts_char_pf_t char_pf;
    char_pf.format     = 0xAA;
    char_pf.exponent   = -3;
    char_pf.unit       = 0xDDCC;
    char_pf.name_space = 0xEE;
    char_pf.desc       = 0x9988;

    ble_gatts_attr_md_t user_desc_md;
    user_desc_md.read_perm.sm  = 0x1;
    user_desc_md.read_perm.lv  = 0x2;
    user_desc_md.write_perm.sm = 0x1;
    user_desc_md.write_perm.lv = 0x2;
    user_desc_md.vlen          = 0x1;
    user_desc_md.vloc          = 0x1;
    user_desc_md.rd_auth       = 0x0;
    user_desc_md.wr_auth       = 0x1;

    ble_gatts_attr_md_t cccd_md;
    cccd_md.read_perm.sm       = 0x1;
    cccd_md.read_perm.lv       = 0x2;
    cccd_md.write_perm.sm      = 0x1;
    cccd_md.write_perm.lv      = 0x2;
    cccd_md.vlen               = 0x1;
    cccd_md.vloc               = 0x1;
    cccd_md.rd_auth            = 0x0;
    cccd_md.wr_auth            = 0x1;

    ble_gatts_attr_md_t sccd_md;
    sccd_md.read_perm.sm       = 0x1;
    sccd_md.read_perm.lv       = 0x2;
    sccd_md.write_perm.sm      = 0x1;
    sccd_md.write_perm.lv      = 0x2;
    sccd_md.vlen               = 0x1;
    sccd_md.vloc               = 0x1;
    sccd_md.rd_auth            = 0x0;
    sccd_md.wr_auth            = 0x1;

    exp_char_md.p_char_pf        = &char_pf;
    exp_char_md.p_user_desc_md   = &user_desc_md;
    exp_char_md.p_cccd_md        = &cccd_md;
    exp_char_md.p_sccd_md        = &sccd_md;

    ble_gatts_char_handles_t exp_result_handles;
    exp_result_handles.value_handle     = 0x0023;
    exp_result_handles.user_desc_handle = 0x0024;
    exp_result_handles.cccd_handle      = 0x0025;
    exp_result_handles.sccd_handle      = 0x0026;

    p_sd_mock_call->arg[0]    = (uint32_t)&exp_handle;
    p_sd_mock_call->arg[1]    = (uint32_t)&exp_char_md;
    p_sd_mock_call->arg[2]    = (uint32_t)NULL;
    p_sd_mock_call->result[0] = (uint32_t)&exp_result_handles;

    ble_rpc_cmd_handle(NULL, 0);

    (void)exp_handle;
    (void)exp_result_handles;

}
CU_TEST_END


CU_TEST_START(gatts_char_add_without_handles)
{
    nrf_cunit_mock_call *      p_sd_mock_call;
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_transport_data;

    expected_transport_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
    expected_transport_data.err_code      = NRF_SUCCESS;
    expected_transport_data.ext_data_len  = 0;
    expected_transport_data.extended_data = NULL;
    expected_transport_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gatts_characteristic_add", &p_sd_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected Transport Layer API call
    expected_transport_api_data_setup(p_transport_write_mock_call, &expected_transport_data);

    // ----- Execute the command to be tested. -----
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                         SD_BLE_GATTS_CHARACTERISTIC_ADD,
                         0x01, 0x00,                   // Service handle
                         0x01,                         // Char metadata present
                         0xFE,                         // char_md.char_props
                         0xFE,                         // char_md.char_ext_props
                         0x12, 0x00,                   // User desc max size
                         0x05, 0x00,                   // User desc size
                         0x01,                         // User desc present
                         0x01, 0x02, 0x03, 0x04, 0x05, // User desc
                         0x01,                         // Presentation format present
                         0xAA,                         // PF: Format
                         0xFD,                         // PF: Exponent (-3 in 2's complement)
                         0xCC, 0xDD,                   // PF: Unit
                         0xEE,                         // PF: Namespace
                         0x88, 0x99,                   // PF: Description
                         0x01,                         // User desc metadata present
                         0x21, 0x21, 0x13,             // read perm, write perm, auth
                         0x01,                         // CCCD metadata present
                         0x21, 0x21, 0x13,             // read perm, write perm, auth
                         0x01,                         // SCCD metadata present
                         0x21, 0x21, 0x13,             // read perm, write perm, auth
                         0x01,                         // Char attribute present
                         0x01,                         // UUID present
                         0xAB, 0xCD, 0x1,              // UUID, UUID type
                         0x01,                         // Char attribute, attribute metadata present
                         0x21, 0x21, 0x13,             // read perm, write perm, auth
                         0x05, 0x00,                   // value length
                         0x78, 0x00,                   // value offset
                         0x79, 0x00,                   // max value length
                         0x01,                         // attribute data present
                         0xEE, 0xDD, 0xCC, 0xBB, 0xAA, // attribute data
                         0x00                          // Handles present
                         };

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    uint16_t                 exp_handle;
    ble_gatts_char_md_t      exp_char_md;
    ble_gatts_attr_t         exp_attr_char_values;

    exp_handle = 0x0001;

    // Metadata
    exp_char_md.char_props.broadcast       = 0x0;
    exp_char_md.char_props.read            = 0x1;
    exp_char_md.char_props.write_wo_resp   = 0x1;
    exp_char_md.char_props.write           = 0x1;
    exp_char_md.char_props.notify          = 0x1;
    exp_char_md.char_props.indicate        = 0x1;
    exp_char_md.char_props.auth_signed_wr  = 0x1;
    exp_char_md.char_ext_props.reliable_wr = 0x0;
    exp_char_md.char_ext_props.wr_aux      = 0x1;
    uint8_t char_user_desc[5]              = {0x01, 0x02, 0x03, 0x04, 0x05};
    exp_char_md.p_char_user_desc           = char_user_desc;
    exp_char_md.char_user_desc_max_size    = 0x0012;
    exp_char_md.char_user_desc_size        = 0x0005;

    ble_gatts_char_pf_t char_pf;
    char_pf.format     = 0xAA;
    char_pf.exponent   = -3;
    char_pf.unit       = 0xDDCC;
    char_pf.name_space = 0xEE;
    char_pf.desc       = 0x9988;

    ble_gatts_attr_md_t user_desc_md;
    user_desc_md.read_perm.sm  = 0x1;
    user_desc_md.read_perm.lv  = 0x2;
    user_desc_md.write_perm.sm = 0x1;
    user_desc_md.write_perm.lv = 0x2;
    user_desc_md.vlen          = 0x1;
    user_desc_md.vloc          = 0x1;
    user_desc_md.rd_auth       = 0x0;
    user_desc_md.wr_auth       = 0x1;

    ble_gatts_attr_md_t cccd_md;
    cccd_md.read_perm.sm       = 0x1;
    cccd_md.read_perm.lv       = 0x2;
    cccd_md.write_perm.sm      = 0x1;
    cccd_md.write_perm.lv      = 0x2;
    cccd_md.vlen               = 0x1;
    cccd_md.vloc               = 0x1;
    cccd_md.rd_auth            = 0x0;
    cccd_md.wr_auth            = 0x1;

    ble_gatts_attr_md_t sccd_md;
    sccd_md.read_perm.sm       = 0x1;
    sccd_md.read_perm.lv       = 0x2;
    sccd_md.write_perm.sm      = 0x1;
    sccd_md.write_perm.lv      = 0x2;
    sccd_md.vlen               = 0x1;
    sccd_md.vloc               = 0x1;
    sccd_md.rd_auth            = 0x0;
    sccd_md.wr_auth            = 0x1;

    exp_char_md.p_char_pf        = &char_pf;
    exp_char_md.p_user_desc_md   = &user_desc_md;
    exp_char_md.p_cccd_md        = &cccd_md;
    exp_char_md.p_sccd_md        = &sccd_md;

    // Attributes
    ble_uuid_t uuid;
    uuid.uuid = 0xCDAB;
    uuid.type = 0x1;
    exp_attr_char_values.p_uuid = &uuid;

    ble_gatts_attr_md_t attr_md;
    attr_md.read_perm.sm       = 0x1;
    attr_md.read_perm.lv       = 0x2;
    attr_md.write_perm.sm      = 0x1;
    attr_md.write_perm.lv      = 0x2;
    attr_md.vlen               = 0x1;
    attr_md.vloc               = 0x1;
    attr_md.rd_auth            = 0x0;
    attr_md.wr_auth            = 0x1;

    exp_attr_char_values.p_attr_md = &attr_md;
    exp_attr_char_values.init_len  = 0x0005;
    exp_attr_char_values.init_offs = 0x0078;
    exp_attr_char_values.max_len   = 0x0079;

    uint8_t attribute_data[5] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};
    exp_attr_char_values.p_value = attribute_data;

    p_sd_mock_call->arg[0]    = (uint32_t)&exp_handle;
    p_sd_mock_call->arg[1]    = (uint32_t)&exp_char_md;
    p_sd_mock_call->arg[2]    = (uint32_t)&exp_attr_char_values;
    p_sd_mock_call->result[0] = (uint32_t)NULL;

    ble_rpc_cmd_handle(NULL, 0);

    (void)exp_handle;

}
CU_TEST_END


CU_TEST_START(gatts_char_add_error)
{
    nrf_cunit_mock_call *      p_sd_mock_call;
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_transport_data;

    expected_transport_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
    expected_transport_data.err_code      = NRF_ERROR_INVALID_PARAM;
    expected_transport_data.ext_data_len  = 0;
    expected_transport_data.extended_data = NULL;
    expected_transport_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gatts_characteristic_add", &p_sd_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected Transport Layer API call
    expected_transport_api_data_setup(p_transport_write_mock_call, &expected_transport_data);

    // ----- Execute the command to be tested. -----
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                         SD_BLE_GATTS_CHARACTERISTIC_ADD,
                         0x01, 0x03,                   // Service handle
                         0x00,                         // Char metadata present
                         0x00,                         // Char attribute present
                         0x00                          // Handles present
                         };

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    uint16_t                 exp_handle;
    exp_handle = 0x0301;

    p_sd_mock_call->arg[0]    = (uint32_t)&exp_handle;
    p_sd_mock_call->arg[1]    = (uint32_t)NULL;
    p_sd_mock_call->arg[2]    = (uint32_t)NULL;
    p_sd_mock_call->result[0] = (uint32_t)NULL;
    p_sd_mock_call->error     = NRF_ERROR_INVALID_PARAM;

    ble_rpc_cmd_handle(NULL, 0);

    (void)exp_handle;

}
CU_TEST_END


CU_TEST_START(gatts_char_add_char_vloc_INVALID_PARAM_attr_metadata)
{
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_transport_data;

    expected_transport_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
    expected_transport_data.err_code      = NRF_ERROR_INVALID_PARAM;
    expected_transport_data.ext_data_len  = 0;
    expected_transport_data.extended_data = NULL;
    expected_transport_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected Transport Layer API call
    expected_transport_api_data_setup(p_transport_write_mock_call, &expected_transport_data);

    // ----- Execute the command to be tested. -----
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                         SD_BLE_GATTS_CHARACTERISTIC_ADD,
                         0x01, 0x00,                   // Service handle
                         0x00,                         // Char metadata present
                         0x01,                         // Char attribute present
                         0x01,                         // UUID present
                         0xAB, 0xCD, 0x1,              // UUID, UUID type
                         0x01,                         // Char attribute, attribute metadata present
                         0x21, 0x21, 0x11,             // read perm, write perm, auth
                         0x05, 0x00,                   // value length
                         0x78, 0x00,                   // value offset
                         0x79, 0x00,                   // max value length
                         0x01,                         // attribute data present
                         0xEE, 0xDD, 0xCC, 0xBB, 0xAA, // attribute data
                         0x01                          // Handles present
                         };

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_char_vloc_INVALID_PARAM_char_metadata_user_desc_metadata)
{
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_transport_data;

    expected_transport_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
    expected_transport_data.err_code      = NRF_ERROR_INVALID_PARAM;
    expected_transport_data.ext_data_len  = 0;
    expected_transport_data.extended_data = NULL;
    expected_transport_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected Transport Layer API call
    expected_transport_api_data_setup(p_transport_write_mock_call, &expected_transport_data);

    // ----- Execute the command to be tested. -----
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                         SD_BLE_GATTS_CHARACTERISTIC_ADD,
                         0x01, 0x00,                   // Service handle
                         0x01,                         // Char metadata present
                         0xFE,                         // char_md.char_props
                         0xFE,                         // char_md.char_ext_props
                         0x12, 0x00,                   // User desc max size
                         0x05, 0x00,                   // User desc size
                         0x00,                         // User desc present
                         0x00,                         // Presentation format present
                         0x01,                         // User desc metadata present
                         0x21, 0x21, 0x11,             // read perm, write perm, auth
                         0x00,                         // CCCD metadata present
                         0x00,                         // SCCD metadata present
                         0x00,                         // Char attribute present
                         0x00                          // Handles present
                         };

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_char_vloc_INVALID_PARAM_char_metadata_cccd_desc_metadata)
{
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_transport_data;

    expected_transport_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
    expected_transport_data.err_code      = NRF_ERROR_INVALID_PARAM;
    expected_transport_data.ext_data_len  = 0;
    expected_transport_data.extended_data = NULL;
    expected_transport_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected Transport Layer API call
    expected_transport_api_data_setup(p_transport_write_mock_call, &expected_transport_data);

    // ----- Execute the command to be tested. -----
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                         SD_BLE_GATTS_CHARACTERISTIC_ADD,
                         0x01, 0x00,                   // Service handle
                         0x01,                         // Char metadata present
                         0xFE,                         // char_md.char_props
                         0xFE,                         // char_md.char_ext_props
                         0x12, 0x00,                   // User desc max size
                         0x05, 0x00,                   // User desc size
                         0x00,                         // User desc present
                         0x00,                         // Presentation format present
                         0x00,                         // User desc metadata present
                         0x01,                         // CCCD metadata present
                         0x21, 0x21, 0x11,             // read perm, write perm, auth
                         0x00,                         // SCCD metadata present
                         0x00,                         // Char attribute present
                         0x00                          // Handles present
                         };

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_char_vloc_INVALID_PARAM_char_metadata_sccd_desc_metadata)
{
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_transport_data;

    expected_transport_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
    expected_transport_data.err_code      = NRF_ERROR_INVALID_PARAM;
    expected_transport_data.ext_data_len  = 0;
    expected_transport_data.extended_data = NULL;
    expected_transport_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected Transport Layer API call
    expected_transport_api_data_setup(p_transport_write_mock_call, &expected_transport_data);

    // ----- Execute the command to be tested. -----
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                         SD_BLE_GATTS_CHARACTERISTIC_ADD,
                         0x01, 0x00,                   // Service handle
                         0x01,                         // Char metadata present
                         0xFE,                         // char_md.char_props
                         0xFE,                         // char_md.char_ext_props
                         0x12, 0x00,                   // User desc max size
                         0x05, 0x00,                   // User desc size
                         0x00,                         // User desc present
                         0x00,                         // Presentation format present
                         0x00,                         // User desc metadata present
                         0x00,                         // CCCD metadata present
                         0x01,                         // SCCD metadata present
                         0x21, 0x21, 0x11,             // read perm, write perm, auth
                         0x00,                         // Char attribute present
                         0x00                          // Handles present
                         };

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_without_char_attr_attr_metadata_or_uuid_or_data)
{
    nrf_cunit_mock_call *      p_sd_mock_call;
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_transport_data;

    uint8_t  extended_data[]              = {0x23, 0x00, // value_handle
                                             0x24, 0x00, // user_desc_handle
                                             0x25, 0x00, // cccd_handle
                                             0x26, 0x00};// sccd_handle

    expected_transport_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
    expected_transport_data.err_code      = NRF_SUCCESS;
    expected_transport_data.ext_data_len  = 8;
    expected_transport_data.extended_data = extended_data;
    expected_transport_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gatts_characteristic_add", &p_sd_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected Transport Layer API call
    expected_transport_api_data_setup(p_transport_write_mock_call, &expected_transport_data);

    // ----- Execute the command to be tested. -----
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                         SD_BLE_GATTS_CHARACTERISTIC_ADD,
                         0x01, 0x00,                   // Service handle
                         0x00,                         // Char metadata present
                         0x01,                         // Char attribute present
                         0x00,                         // UUID present
                         0x00,                         // Char attribute, attribute metadata present
                         0x05, 0x00,                   // value length
                         0x78, 0x00,                   // value offset
                         0x79, 0x00,                   // max value length
                         0x00,                         // attribute data present
                         0x01                          // Handles present
                         };

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    uint16_t                 exp_handle;
    ble_gatts_attr_t         exp_attr_char_values;

    exp_handle = 0x0001;

    // Attributes
    exp_attr_char_values.p_uuid = NULL;

    exp_attr_char_values.p_attr_md = NULL;
    exp_attr_char_values.init_len  = 0x0005;
    exp_attr_char_values.init_offs = 0x0078;
    exp_attr_char_values.max_len   = 0x0079;

    exp_attr_char_values.p_value = NULL;

    ble_gatts_char_handles_t exp_result_handles;
    exp_result_handles.value_handle     = 0x0023;
    exp_result_handles.user_desc_handle = 0x0024;
    exp_result_handles.cccd_handle      = 0x0025;
    exp_result_handles.sccd_handle      = 0x0026;

    p_sd_mock_call->arg[0]    = (uint32_t)&exp_handle;
    p_sd_mock_call->arg[1]    = (uint32_t)NULL;
    p_sd_mock_call->arg[2]    = (uint32_t)&exp_attr_char_values;
    p_sd_mock_call->result[0] = (uint32_t)&exp_result_handles;

    ble_rpc_cmd_handle(NULL, 0);

    (void)exp_handle;
    (void)exp_result_handles;

}
CU_TEST_END


CU_TEST_START(gatts_service_add_sanchk_lt_type)
{

    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_SERVICE_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;


     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

          // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_SERVICE_ADD,
                          0x01,                         // Type
                          0x01,                         // Service UUID Present
                          0x12, 0x34,                   // UUId
                          0x01,                         // UUID type
                          0x01                          // Handle present
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 1;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_service_add_sanchk_lt_serUuid_absent)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_SERVICE_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;


     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

          // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_SERVICE_ADD,
                          0x01,                         //Type
                          0x01,                         // Service UUID Present
                          0x12, 0x34,                   // UUId
                          0x01,                         // UUID type
                          0x01                          // Handle present
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 2;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_service_add_sanchk_lt_serUuid_1)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_SERVICE_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;


     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

          // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_SERVICE_ADD,
                          0x01,                         // Type
                          0x01,                         // Service UUID Present
                          0x12, 0x34,                   // UUId
                          0x01,                         // UUID type
                          0x01                          // Handle present
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 4;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_service_add_sanchk_lt_serUuid_2)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_SERVICE_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;


     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_SERVICE_ADD,
                          0x01,                         //Type
                          0x01,                         // Service UUID Present
                          0x12, 0x34,                   // UUId
                          0x01,                         // UUID type
                          0x01                          // Handle present
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 5;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_service_add_sanchk_lt_handle)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_SERVICE_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;


     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

          // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_SERVICE_ADD,
                          0x01,                         //Type
                          0x01,                         // Service UUID Present
                          0x12, 0x34,                   // UUId
                          0x01,                         // UUID type
                          0x01                          // Handle present
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 6;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_value_set_sanchk_lt_offset)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_VALUE_SET;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;


     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

          // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_VALUE_SET,
                          0xFF, 0xEF,                   // handle
                          0x12,                         // offset
                          0x01,                         // length present
                          0x10, 0x00,                   // length
                          0x01,                         // Attr present
                          0x1, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 3;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_value_set_sanchk_lt_length)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_VALUE_SET;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

          // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_VALUE_SET,
                          0xFF, 0xEF,                   // handle
                          0x12, 0x21,                   // offset
                          0x01,                         // length present
                          0x10, 0x00,                   // length
                          0x01,                         // Attr present
                          0x1, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 7;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_value_set_sanchk_lt_length_cond)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_VALUE_SET;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

          // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_VALUE_SET,
                          0xFF, 0xEF,                   // handle
                          0x12,0x90,                    // offset
                          0x01,                         // length present
                          0x10, 0x00,                   // length
                          0x01,                         // Attr present
                          0x1, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 5;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_value_set_sanchk_lt_attr_val_cond)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_VALUE_SET;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

          // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_VALUE_SET,
                          0xFF, 0xEF,                   // handle
                          0x12,0x90,                    // offset
                          0x01,                         // length present
                          0x10, 0x00,                   // length
                          0x01,                         // Attr present
                          0x1, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 8;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_value_set_sanchk_lt_handle)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_VALUE_SET;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_VALUE_SET,
                          0xFF, 0xEF,                   // handle
                          0x12,0x90,                    // offset
                          0x01,                         // length present
                          0x10, 0x00,                   // length
                          0x01,                         // Attr present
                          0x1, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 2;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_value_set_sanchk_lt_attr_val_2)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_VALUE_SET;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

          // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_VALUE_SET,
                          0xFF, 0xEF,                   // handle
                          0x12,0x90,                    // offset
                          0x01,                         // length present
                          0xFF, 0xFF,                   // length
                          0x01,                         // Attr present
                          0x1, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 12;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_value_set_sanchk_lt_attr_val)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_VALUE_SET;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

          // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_VALUE_SET,
                          0xFF, 0xEF,                   // handle
                          0x12,0x90,                    // offset
                          0x01,                         // length present
                          0x10, 0x00,                   // length
                          0x01,                         // Attr present
                          0x1, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 12;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_hvx_sanchk_lt_handle)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_HVX;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

          // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_HVX,
                          0xFF, 0xE,                    // conn. handle
                          0x01,                         // hvx params present
                          0x01, 0x02,                   // char. val. handle
                          0x01,                         // type
                          0x90,                         // offset
                          0x01,                         // data len present
                          0x10, 0x00,                   // length
                          0x01,                         // data present
                          1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 2;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END

#if 0
// TODO: MANUAL TEST, needs increase buffer in transport layer. Comment this one in when having mem-pool impl.
CU_TEST_START(gatts_char_add_user_desc_512)
{
    reset_unit_test();
    nrf_cunit_mock_call *      p_sd_mock_call;
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_transport_data;

    uint8_t  extended_data[]              = {0x23, 0x00, // value_handle
                                             0x24, 0x00, // user_desc_handle
                                             0x25, 0x00, // cccd_handle
                                             0x26, 0x00};// sccd_handle

    expected_transport_data.op_code       = RPC_SD_BLE_GATTS_CHARACTERISTIC_ADD;
    expected_transport_data.err_code      = NRF_SUCCESS;
    expected_transport_data.ext_data_len  = 8;
    expected_transport_data.extended_data = extended_data;
    expected_transport_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gatts_characteristic_add", &p_sd_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected Transport Layer API call
    expected_transport_api_data_setup(p_transport_write_mock_call, &expected_transport_data);

    // ----- Execute the command to be tested. -----
    uint8_t in_data[18+512] = {BLE_RPC_PKT_CMD,              // Packet type
                               RPC_SD_BLE_GATTS_CHARACTERISTIC_ADD,
                         0x01, 0x00,                   // Service handle
                         0x01,                         // Char metadata present
                         0xFE,                         // char_md.char_props
                         0xFE,                         // char_md.char_ext_props
                         0x00, 0x02,                   // User desc max size
                         0x00, 0x02,                   // User desc size
                         0x01};                        // User desc present
    uint16_t index = 11;
    uint16_t i;
    for (i = 0; i < 512; i++)
    {
        in_data[index++] = (uint8_t)i;
    }

    in_data[index++] = 0x00;                           // Presentation format present
    in_data[index++] = 0x00;                           // User desc metadata present
    in_data[index++] = 0x00;                           // CCCD metadata present
    in_data[index++] = 0x00;                           // SCCD metadata present
    in_data[index++] = 0x00;                           // Char attribute present
    in_data[index++] = 0x01;                           // Handles present


    p_transport_read_mock_call->arg[0]    = RPC_TRANSPORT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    uint16_t                 exp_handle;
    ble_gatts_char_md_t      exp_char_md;

    exp_handle = 0x0001;

    // Metadata
    exp_char_md.char_props.broadcast       = 0x0;
    exp_char_md.char_props.read            = 0x1;
    exp_char_md.char_props.write_wo_resp   = 0x1;
    exp_char_md.char_props.write           = 0x1;
    exp_char_md.char_props.notify          = 0x1;
    exp_char_md.char_props.indicate        = 0x1;
    exp_char_md.char_props.auth_signed_wr  = 0x1;
    exp_char_md.char_ext_props.reliable_wr = 0x0;
    exp_char_md.char_ext_props.wr_aux      = 0x1;

    uint8_t char_user_desc[512];
    for (i = 0; i < 512; i++)
    {
        char_user_desc[i] = (uint8_t)i;
    }

    exp_char_md.p_char_user_desc           = char_user_desc;
    exp_char_md.char_user_desc_max_size    = 0x0200;
    exp_char_md.char_user_desc_size        = 0x0200;

    exp_char_md.p_char_pf        = NULL;
    exp_char_md.p_user_desc_md   = NULL;
    exp_char_md.p_cccd_md        = NULL;
    exp_char_md.p_sccd_md        = NULL;

    ble_gatts_char_handles_t exp_result_handles;
    exp_result_handles.value_handle     = 0x0023;
    exp_result_handles.user_desc_handle = 0x0024;
    exp_result_handles.cccd_handle      = 0x0025;
    exp_result_handles.sccd_handle      = 0x0026;

    p_sd_mock_call->arg[0]    = (uint32_t)&exp_handle;
    p_sd_mock_call->arg[1]    = (uint32_t)&exp_char_md;
    p_sd_mock_call->arg[2]    = (uint32_t)NULL;
    p_sd_mock_call->result[0] = (uint32_t)&exp_result_handles;

    rpc_cmd_handle(NULL, 0);

    (void)exp_handle;
    (void)exp_result_handles;

}
CU_TEST_END


CU_TEST_START(gatts_char_add_char_attr_attr_data_512)
{
    reset_unit_test();
    nrf_cunit_mock_call *      p_sd_mock_call;
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_transport_data;

    uint8_t  extended_data[]              = {0x23, 0x00, // value_handle
                                             0x24, 0x00, // user_desc_handle
                                             0x25, 0x00, // cccd_handle
                                             0x26, 0x00};// sccd_handle
    expected_transport_data.op_code       = RPC_SD_BLE_GATTS_CHARACTERISTIC_ADD;
    expected_transport_data.err_code      = NRF_SUCCESS;
    expected_transport_data.ext_data_len  = 8;
    expected_transport_data.extended_data = extended_data;
    expected_transport_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gatts_characteristic_add", &p_sd_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected Transport Layer API call
    expected_transport_api_data_setup(p_transport_write_mock_call, &expected_transport_data);

    // ----- Execute the command to be tested. -----
    uint8_t in_data[22+512] = {BLE_RPC_PKT_CMD,        // Packet type
                               RPC_SD_BLE_GATTS_CHARACTERISTIC_ADD,
                         0x01, 0x00,                   // Service handle
                         0x00,                         // Char metadata present
                         0x01,                         // Char attribute present
                         0x01,                         // UUID present
                         0xAB, 0xCD, 0x1,              // UUID, UUID type
                         0x01,                         // Char attribute, attribute metadata present
                         0x21, 0x21, 0x13,             // read perm, write perm, auth
                         0x00, 0x02,                   // value length
                         0x00, 0x00,                   // value offset
                         0x00, 0x02,                   // max value length
                         0x01};                        // attribute data present

    uint16_t index = 20;
    uint16_t i;
    for (i = 0; i < 512; i++)
    {
        in_data[index++] = (uint8_t)i;
    }

    in_data[index++] = 0x01;                           // Handles present


    p_transport_read_mock_call->arg[0]    = RPC_TRANSPORT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    uint16_t                 exp_handle;
    ble_gatts_attr_t         exp_attr_char_values;

    exp_handle = 0x0001;

    // Attributes
    ble_uuid_t uuid;
    uuid.uuid = 0xCDAB;
    uuid.type = 0x1;
    exp_attr_char_values.p_uuid = &uuid;

    ble_gatts_attr_md_t attr_md;
    attr_md.read_perm.sm       = 0x1;
    attr_md.read_perm.lv       = 0x2;
    attr_md.write_perm.sm      = 0x1;
    attr_md.write_perm.lv      = 0x2;
    attr_md.vlen               = 0x1;
    attr_md.vloc               = 0x1;
    attr_md.rd_auth            = 0x0;
    attr_md.wr_auth            = 0x1;

    exp_attr_char_values.p_attr_md = &attr_md;
    exp_attr_char_values.init_len  = 0x0200;
    exp_attr_char_values.init_offs = 0x0000;
    exp_attr_char_values.max_len   = 0x0200;

    uint8_t attribute_data[512];
    for (i = 0; i < 512; i++)
    {
        attribute_data[i] = (uint8_t)i;
    }

    exp_attr_char_values.p_value = attribute_data;

    ble_gatts_char_handles_t exp_result_handles;
    exp_result_handles.value_handle     = 0x0023;
    exp_result_handles.user_desc_handle = 0x0024;
    exp_result_handles.cccd_handle      = 0x0025;
    exp_result_handles.sccd_handle      = 0x0026;

    p_sd_mock_call->arg[0]    = (uint32_t)&exp_handle;
    p_sd_mock_call->arg[1]    = (uint32_t)NULL;
    p_sd_mock_call->arg[2]    = (uint32_t)&exp_attr_char_values;
    p_sd_mock_call->result[0] = (uint32_t)&exp_result_handles;

    rpc_cmd_handle(NULL, 0);

    (void)exp_handle;
    (void)exp_result_handles;

}
CU_TEST_END
#endif

CU_TEST_START(gatts_hvx_sanchk_lt_param_absent)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_HVX;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

          // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_HVX,
                          0xFF, 0xE,                    // conn. handle
                          0x01,                         // hvx params present
                          0x01, 0x02,                   // char. val. handle
                          0x01,                         // type
                          0x90,                         // offset
                          0x01,                         // data len present
                          0x10, 0x00,                   // length
                          0x01,                         // data present
                          1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 3;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_hvx_sanchk_lt_char_val_handle)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_HVX;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

          // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_HVX,
                          0xFF, 0xE,                    // conn. handle
                          0x01,                         // hvx params present
                          0x01, 0x02,                   // char. val. handle
                          0x01,                         // type
                          0x90,                         // offset
                          0x01,                         // data len present
                          0x10, 0x00,                   // length
                          0x01,                         // data present
                          1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 5;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_hvx_sanchk_lt_type)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_HVX;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

          // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_HVX,
                          0xFF, 0xE,                    // conn. handle
                          0x01,                         // hvx params present
                          0x01, 0x02,                   // char. val. handle
                          0x01,                         // type
                          0x90,                         // offset
                          0x01,                         // data len present
                          0x10, 0x00,                   // length
                          0x01,                         // data present
                          1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 6;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_hvx_sanchk_lt_offset)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_HVX;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_HVX,
                          0xFF, 0xE,                    // conn. handle
                          0x01,                         // hvx params present
                          0x01, 0x02,                   // char. val. handle
                          0x01,                         // type
                          0x90,0x90,                    // offset
                          0x01,                         // data len present
                          0x10, 0x00,                   // length
                          0x01,                         // data present
                          1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 7;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_hvx_sanchk_lt_data_len_present)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_HVX;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_HVX,
                          0xFF, 0xE,                    // conn. handle
                          0x01,                         // hvx params present
                          0x01, 0x02,                   // char. val. handle
                          0x01,                         // type
                          0x90,0x90,                    // offset
                          0x01,                         // data len present
                          0x10, 0x00,                   // length
                          0x01,                         // data present
                          1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 9;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END

CU_TEST_START(gatts_hvx_sanchk_lt_data_len)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_HVX;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

          // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_HVX,
                          0xFF, 0xE,                    // conn. handle
                          0x01,                         // hvx params present
                          0x01, 0x02,                   // char. val. handle
                          0x01,                         // type
                          0x90,0x90,                    // offset
                          0x01,                         // data len present
                          0xFF, 0xFF,                   // length
                          0x01,                         // data present
                          1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 12;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END

CU_TEST_START(gatts_hvx_sanchk_lt_data_present)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_HVX;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

          // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_HVX,
                          0xFF, 0xE,                    // conn. handle
                          0x01,                         // hvx params present
                          0x01, 0x02,                   // char. val. handle
                          0x01,                         // type
                          0x90,0x90,                    // offset
                          0x01,                         // data len present
                          0x00, 0x00,                   // length
                          0x01,                         // data present
                          1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 12;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END

CU_TEST_START(gatts_hvx_sanchk_lt_data)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_HVX;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

          // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_HVX,
                          0xFF, 0xE,                    // conn. handle
                          0x01,                         // hvx params present
                          0x01, 0x02,                   // char. val. handle
                          0x01,                         // type
                          0x90,0x90,                    // offset
                          0x01,                         // data len present
                          0x90, 0x00,                   // length
                          0x01,                         // data present
                          1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = sizeof(in_data);

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_sys_attr_set_sanchk_lt_handle)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_SYS_ATTR_SET;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_SYS_ATTR_SET,
                          0x67, 67,                     // conn handle
                          0x01,                         // sys attr present
                          0x10, 0x00,                   // sys attr len
                          0x09                          // sys attr
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 1;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_sys_attr_set_sanchk_lt_sys_attr_present)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_SYS_ATTR_SET;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

          // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_SYS_ATTR_SET,
                          0x67, 67,                     // conn handle
                          0x01,                         // sys attr present
                          0x10, 0x00,                   // sys attr len
                          0x09                          // sys attr
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 3;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_sys_attr_set_sanchk_lt_sys_attr_len)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_SYS_ATTR_SET;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_SYS_ATTR_SET,
                          0x67, 67,                     // conn handle
                          0x01,                         // sys attr present
                          0x10, 0x00,                   // sys attr len
                          0x09                          // sys attr
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 5;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_sys_attr_set_sanchk_lt_sys_attr)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_SYS_ATTR_SET;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,                                   // Packet type
                          SD_BLE_GATTS_SYS_ATTR_SET,
                          0x67, 67,                                          // conn handle
                          0x01,                                              // sys attr present
                          0xA, 0x00,                                         // sys attr len
                          0x1, 2, 3, 4, 5, 6, 6, 7, 8, 9, 10,                // sys attr
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 15;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_sys_attr_get_sanchk_lt_conn_handle)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_SYS_ATTR_GET;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_SYS_ATTR_GET,
                          0x67, 67,                     // conn handle
                          0x01,                         // sys attr len present
                          0xA, 0x00,                    // sys attr len
                          0x1                           // sys attr data present
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 2;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END

CU_TEST_START(gatts_sys_attr_get_sanchk_lt_sys_attr_len_present)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_SYS_ATTR_GET;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_SYS_ATTR_GET,
                          0x67, 67,                     // conn handle
                          0x01,                         // sys attr len present
                          0xA, 0x00,                    // sys attr len
                          0x1                           // sys attr data present
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 3;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_sys_attr_get_sanchk_lt_sys_attr_len)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_SYS_ATTR_GET;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_SYS_ATTR_GET,
                          0x67, 67,                     // conn handle
                          0x01,                         // sys attr len present
                          0xA, 0x00,                    // sys attr len
                          0x1                           // sys attr data present
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 5;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_sys_attr_get_sanchk_lt_sys_data_present)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_SYS_ATTR_GET;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_SYS_ATTR_GET,
                          0x67, 67,                     // conn handle
                          0x01,                         // sys attr len present
                          0xA, 0x00,                    // sys attr len
                          0x1                           // sys attr data present
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 6;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_sanchk_lt_serHdl)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_CHARACTERISTIC_ADD,
                          0x67, 0x67,                   // SER handle
                         };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 2;

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_sanchk_lt_char_metadata_absent)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_CHARACTERISTIC_ADD,
                          0x67, 0x67,                   // SER handle
                          0x00};

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 3;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_sanchk_lt_char_metadata_char_attr_absent)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_CHARACTERISTIC_ADD,
                          0x67, 0x67,                   // SER handle
                          0x00,                         // char meta data absent
                          0x00,                         // char attr absent
                          0x01};                        // handles present

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 5;

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_sanchk_lt_mdata_charprop)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_CHARACTERISTIC_ADD,
                          0x67, 0x67,                   // SER handle
                          0x01,                         // char meta data present
                          0x01,                         // char property
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 4;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_sanchk_lt_mdata_charextprop)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_write_mock_call;
    expected_transport_data_t  expected_data;

    // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
    expected_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
    expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
    expected_data.ext_data_len  = 0;
    expected_data.packet_type   = BLE_RPC_PKT_RESP;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected Transport Layer API call
    expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


    // ----- Execute the command to be tested. -----
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                         SD_BLE_GATTS_CHARACTERISTIC_ADD,
                         0x67, 0x67,                   // SER handle
                         0x01,                         // char meta data present
                         0x01,                         // char property
                         0x01                          // ext property
                         };

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 5;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_sanchk_lt_mdata_usrdesc_max_size)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,              // Packet type
                          SD_BLE_GATTS_CHARACTERISTIC_ADD,
                          0x67, 0x67,                   // SER handle
                          0x01,                         // char meta data present
                          0x01,                         // char property
                          0x01,                         // ext property
                          0x0a, 0x00                    // user desc max size
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 7;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_sanchk_lt_mdata_usr_desc_size)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,        // Packet type
                          SD_BLE_GATTS_CHARACTERISTIC_ADD,
                          0x67, 0x67,             // SER handle
                          0x01,                   // char meta data present
                          0x01,                   // char property
                          0x01,                   // ext property
                          0x0a, 0x00,             // user desc max size
                          0x05, 0x00              // user desc size
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 9;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_sanchk_lt_mdata_usr_desc_absent)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,        // Packet type
                          SD_BLE_GATTS_CHARACTERISTIC_ADD,
                          0x67, 0x67,             // SER handle
                          0x01,                   // char meta data present
                          0x01,                   // char property
                          0x01,                   // ext property
                          0x0a, 0x00,             // user desc max size
                          0x05, 0x00,             // user desc size
                          0x00
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 10;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_sanchk_lt_mdata_usr_desc)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,        // Packet type
                          SD_BLE_GATTS_CHARACTERISTIC_ADD,
                          0x67, 0x67,             // SER handle
                          0x01,                   // char meta data present
                          0x01,                   // char property
                          0x01,                   // ext property
                          0xFF, 0xFF,             // user desc max size
                          0xFF, 0xFF,             // user desc size
                          0x01,                   // user desc present
                          0x11, 0x12, 0x13};

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 15;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_sanchk_lt_mdata_usr_pf_absent)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,        // Packet type
                          SD_BLE_GATTS_CHARACTERISTIC_ADD,
                          0x67, 0x67,             // SER handle
                          0x01,                   // char meta data present
                          0x01,                   // char property
                          0x01,                   // ext property
                          0x05, 0x00,             // user desc max size
                          0x03, 0x00,             // user desc size
                          0x01,                   // user desc present
                          0x11, 0x12, 0x13,
                          0x00                    // presentation format absent
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 14;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_sanchk_lt_mdata_usr_pf_present)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,        // Packet type
                          SD_BLE_GATTS_CHARACTERISTIC_ADD,
                          0x67, 0x67,             // SER handle
                          0x01,                   // char meta data present
                          0x01,                   // char property
                          0x01,                   // ext property
                          0x05, 0x00,             // user desc max size
                          0x03, 0x00,             // user desc size
                          0x01,                   // user desc present
                          0x11, 0x12, 0x15,
                          0x01,                   // presentation format present
                          1, 2, 3, 4, 5, 6, 7     // presentation format
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 19;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_sanchk_lt_mdata_usr_desc_metadata)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,        // Packet type
                          SD_BLE_GATTS_CHARACTERISTIC_ADD,
                          0x67, 0x67,             // SER handle
                          0x01,                   // char meta data present
                          0x01,                   // char property
                          0x01,                   // ext property
                          0x05, 0x00,             // user desc max size
                          0x03, 0x00,             // user desc size
                          0x01,                   // user desc present
                          0x11, 0x12, 0x13,       // user desc data
                          0x01,                   // presentation format present
                          1, 2, 3, 4, 5, 6, 7,    // presentation format
                          0x01,                   // user desc meta data present
                          1, 2, 3                 // user desc meta data
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 25;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_sanchk_lt_mdata_usr_desc_cccd_data_present)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,        // Packet type
                          SD_BLE_GATTS_CHARACTERISTIC_ADD,
                          0x67, 0x67,             // SER handle
                          0x01,                   // char meta data present
                          0x01,                   // char property
                          0x01,                   // ext property
                          0x05, 0x00,             // user desc max size
                          0x03, 0x00,             // user desc size
                          0x01,                   // user desc present
                          0x11, 0x12, 0x13,
                          0x01,                   // presentation format present
                          1, 2, 3, 4, 5, 6, 7,     // presentation format
                          0x01,                   // user desc meta present
                          1, 2, 3,                // user desc meta data
                          0x01,                   // cccd present
                          4, 5, 6                 // cccd
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 28;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_sanchk_lt_mdata_usr_desc_sccd_data_present)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,        // Packet type
                          SD_BLE_GATTS_CHARACTERISTIC_ADD,
                          0x67, 0x67,             // SER handle
                          0x01,                   // char meta data present
                          0x01,                   // char property
                          0x01,                   // ext property
                          0x05, 0x00,             // user desc max size
                          0x03, 0x00,             // user desc size
                          0x01,                   // user desc present
                          0x11, 0x12, 0x13,
                          0x01,                   // presentation format present
                          1, 2, 3, 4, 5, 6, 7,    // presentation format
                          0x01,                   // user desc meta present
                          1, 2, 3,                // user desc meta data
                          0x01,                   // cccd present
                          4, 5, 3,                // cccd
                          0x01,                   // sccd present
                          7, 8, 3
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 32;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_sanchk_lt_char_attr_uuid)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,        // Packet type
                          SD_BLE_GATTS_CHARACTERISTIC_ADD,
                          0x67, 0x67,             // SER handle
                          0x00,                   // char meta data absent
                          0x01,                   // char attr present
                          0x01,                   // uuid present
                          1, 2, 3                 // uuid
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 8;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_sanchk_lt_char_attr_metadata)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,        // Packet type
                          SD_BLE_GATTS_CHARACTERISTIC_ADD,
                          0x67, 0x67,             // SER handle
                          0x00,                   // char meta data absent
                          0x01,                   // char attr present
                          0x01,                   // uuid present
                          1, 2, 3,                // uuid
                          0x01,                   // attr meta data present
                          1, 2, 3
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 12;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


#if 0
CU_TEST_START(gatts_char_add_sanchk_lt_char_attr_metadata_absent)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = RPC_SD_BLE_GATTS_CHARACTERISTIC_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,        // Packet type
                          RPC_SD_BLE_GATTS_CHARACTERISTIC_ADD,
                          0x67, 0x67,             // SER handle
                          0x00,                   // char meta data absent
                          0x01,                   // char attr present
                          0x01,                   // uuid present
                          1, 2, 3,                // uuid
                          0x0,                   // attr meta data present
                          };

     p_transport_read_mock_call->arg[0]    = RPC_TRANSPORT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 9;


    rpc_cmd_handle(NULL, 0);

}
CU_TEST_END
#endif


CU_TEST_START(gatts_char_add_sanchk_lt_char_attr_val_len)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,        // Packet type
                          SD_BLE_GATTS_CHARACTERISTIC_ADD,
                          0x67, 0x67,             // SER handle
                          0x00,                   // char meta data absent
                          0x01,                   // char attr present
                          0x01,                   // uuid present
                          1, 2, 3,                // uuid
                          0x01,                   // attr meta data present
                          1, 2, 3,                 // attr meta data
                          0x1, 00
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 14;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_sanchk_lt_char_attr_val_offset)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,        // Packet type
                          SD_BLE_GATTS_CHARACTERISTIC_ADD,
                          0x67, 0x67,             // SER handle
                          0x00,                   // char meta data absent
                          0x01,                   // char attr present
                          0x01,                   // uuid present
                          1, 2, 3,                // uuid
                          0x01,                   // attr meta data present
                          1, 2, 3,                // attr meta data
                          0x1, 00,                // len
                          0x00, 0x00              //offset
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 16;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_sanchk_lt_char_attr_max_val_len)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,        // Packet type
                          SD_BLE_GATTS_CHARACTERISTIC_ADD,
                          0x67, 0x67,             // SER handle
                          0x00,                   // char meta data absent
                          0x01,                   // char attr present
                          0x01,                   // uuid present
                          1, 2, 3,                // uuid
                          0x01,                   // attr meta data present
                          1, 2, 3,                // attr meta data
                          0x1, 00,                // len
                          0x00, 0x00,             //offset
                          0x10, 0x00              // max value len
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 18;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_sanchk_lt_char_attr_data_absent)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,        // Packet type
                          SD_BLE_GATTS_CHARACTERISTIC_ADD,
                          0x67, 0x67,             // SER handle
                          0x00,                   // char meta data absent
                          0x01,                   // char attr present
                          0x01,                   // uuid present
                          1, 2, 3,                // uuid
                          0x01,                   // attr meta data present
                          1, 2, 3,                // attr meta data
                          0x1, 00,                // len
                          0x00, 0x00,             //offset
                          0x10, 0x00,              // max value len
                          0x01
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 19;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_sanchk_lt_char_attr_data)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,        // Packet type
                          SD_BLE_GATTS_CHARACTERISTIC_ADD,
                          0x67, 0x67,             // SER handle
                          0x00,                   // char meta data absent
                          0x01,                   // char attr present
                          0x01,                   // uuid present
                          1, 2, 3,                // uuid
                          0x01,                   // attr meta data present
                          1, 2, 3,                // attr meta data
                          0xFF, 0xFF,             // len
                          0x00, 0x00,             //offset
                          0x10, 0x00,             // max value len
                          0x01,
                          0x1, 2, 4,5
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 22;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gatts_char_add_sanchk_lt_char_attr_handles)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns, we expect an invalid param error to be passed to the tansport layer for transmission.
     expected_data.op_code       = SD_BLE_GATTS_CHARACTERISTIC_ADD;
     expected_data.err_code      = NRF_ERROR_INVALID_LENGTH;
     expected_data.ext_data_len  = 0;
     expected_data.packet_type   = BLE_RPC_PKT_RESP;

     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call, &expected_data);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,        // Packet type
                          SD_BLE_GATTS_CHARACTERISTIC_ADD,
                          0x67, 0x67,             // SER handle
                          0x00,                   // char meta data absent
                          0x00,                   // char attr present
                          0x01                    // handle present
                          };

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = 5;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


static void gatts_setup_func(void)
{
    nrf_cunit_reset_mock();
    nrf_cunit_mock_call * p_transport_alloc_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_alloc", &p_transport_alloc_mock_call);

    p_transport_alloc_mock_call->result[0] = (uint32_t)m_tx_buffer;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_consume", &p_transport_alloc_mock_call);
    p_transport_alloc_mock_call->compare_rule[0] = COMPARE_ANY;
}


static void gatts_teardown_func(void)
{
    nrf_cunit_mock_call * p_transport_alloc_mock_call;
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_free", &p_transport_alloc_mock_call);
    (void)hci_transport_tx_free();

    nrf_cunit_verify_call_return();
}


//lint -e14
CU_SUITE_START_PUBLIC(TC_CT_FUN_BV_UNIT_decode_gatts_characteristic_add)
{
    CU_SUITE_SETUP_FUNC(gatts_setup_func);
    CU_SUITE_TEARDOWN_FUNC(gatts_teardown_func);

    CU_SUITE_ADD_TEST(gatts_char_add_all_fields_present);
    CU_SUITE_ADD_TEST(gatts_char_add_without_char_metadata);
    CU_SUITE_ADD_TEST(gatts_char_add_without_char_attributes);
    CU_SUITE_ADD_TEST(gatts_char_add_without_handles);
    CU_SUITE_ADD_TEST(gatts_char_add_error);

    CU_SUITE_ADD_TEST(gatts_char_add_char_vloc_INVALID_PARAM_attr_metadata);
    CU_SUITE_ADD_TEST(gatts_char_add_char_vloc_INVALID_PARAM_char_metadata_user_desc_metadata);
    CU_SUITE_ADD_TEST(gatts_char_add_char_vloc_INVALID_PARAM_char_metadata_cccd_desc_metadata);
    CU_SUITE_ADD_TEST(gatts_char_add_char_vloc_INVALID_PARAM_char_metadata_sccd_desc_metadata);

    CU_SUITE_ADD_TEST(gatts_char_add_without_char_attr_attr_metadata_or_uuid_or_data);

    CU_SUITE_ADD_TEST(gatts_char_add_sanchk_lt_serHdl);
    CU_SUITE_ADD_TEST(gatts_char_add_sanchk_lt_char_metadata_absent);
    CU_SUITE_ADD_TEST(gatts_char_add_sanchk_lt_char_metadata_char_attr_absent);
    CU_SUITE_ADD_TEST(gatts_char_add_sanchk_lt_mdata_charprop);
    CU_SUITE_ADD_TEST(gatts_char_add_sanchk_lt_mdata_charextprop);
    CU_SUITE_ADD_TEST(gatts_char_add_sanchk_lt_mdata_usrdesc_max_size);
    CU_SUITE_ADD_TEST(gatts_char_add_sanchk_lt_mdata_usr_desc_size);
    CU_SUITE_ADD_TEST(gatts_char_add_sanchk_lt_mdata_usr_desc_absent);
    CU_SUITE_ADD_TEST(gatts_char_add_sanchk_lt_mdata_usr_desc);
    CU_SUITE_ADD_TEST(gatts_char_add_sanchk_lt_mdata_usr_pf_absent);
    CU_SUITE_ADD_TEST(gatts_char_add_sanchk_lt_mdata_usr_pf_present);
    CU_SUITE_ADD_TEST(gatts_char_add_sanchk_lt_mdata_usr_desc_metadata);
    CU_SUITE_ADD_TEST(gatts_char_add_sanchk_lt_mdata_usr_desc_cccd_data_present);
    CU_SUITE_ADD_TEST(gatts_char_add_sanchk_lt_mdata_usr_desc_sccd_data_present);

    CU_SUITE_ADD_TEST(gatts_char_add_sanchk_lt_char_attr_uuid);
    CU_SUITE_ADD_TEST(gatts_char_add_sanchk_lt_char_attr_metadata);

    CU_SUITE_ADD_TEST(gatts_char_add_sanchk_lt_char_attr_val_len);
    CU_SUITE_ADD_TEST(gatts_char_add_sanchk_lt_char_attr_val_offset);
    CU_SUITE_ADD_TEST(gatts_char_add_sanchk_lt_char_attr_max_val_len);
    CU_SUITE_ADD_TEST(gatts_char_add_sanchk_lt_char_attr_data_absent);
    CU_SUITE_ADD_TEST(gatts_char_add_sanchk_lt_char_attr_data);
    CU_SUITE_ADD_TEST(gatts_char_add_sanchk_lt_char_attr_handles);


#if 0
// TODO: MANUAL TEST, needs increase buffer in transport layer. Comment this one in when having mem-pool impl.
// To make the two next test cases run, increase the buffer size in RPC_TRANSPORT_PACKET_READ_BUF_SIZE to 2048.
    CU_SUITE_ADD_TEST(gatts_char_add_user_desc_512);
    CU_SUITE_ADD_TEST(gatts_char_add_char_attr_attr_data_512);
#endif
}
CU_SUITE_END


//lint -e14
CU_SUITE_START_PUBLIC(TC_CT_FUN_BV_UNIT_DECODE_GATTS)
{
    CU_SUITE_SETUP_FUNC(gatts_setup_func);
    CU_SUITE_TEARDOWN_FUNC(gatts_teardown_func);

    CU_SUITE_ADD_TEST(gatts_service_add_simple);
    CU_SUITE_ADD_TEST(gatts_service_error);
    CU_SUITE_ADD_TEST(gatts_sys_attr_set_null);
    CU_SUITE_ADD_TEST(gatts_sys_attr_set_cccds);
    CU_SUITE_ADD_TEST(gatts_hvx_null);
    CU_SUITE_ADD_TEST(gatts_hvx_notification);
    CU_SUITE_ADD_TEST(gatts_hvx_notification_null_length_and_data);
    CU_SUITE_ADD_TEST(gatts_hvx_notification_null_data_w_length);
    CU_SUITE_ADD_TEST(gatts_sys_attr_get_null);
    CU_SUITE_ADD_TEST(gatts_sys_attr_get_data_length);
    CU_SUITE_ADD_TEST(gatts_sys_attr_get_data);
    CU_SUITE_ADD_TEST(gatts_value_set_null);
    CU_SUITE_ADD_TEST(gatts_value_set_null_data);
    CU_SUITE_ADD_TEST(gatts_value_set);
    CU_SUITE_ADD_TEST(gatts_service_add_sanchk_lt_type);
    CU_SUITE_ADD_TEST(gatts_service_add_sanchk_lt_serUuid_absent);
    CU_SUITE_ADD_TEST(gatts_service_add_sanchk_lt_serUuid_1);
    CU_SUITE_ADD_TEST(gatts_service_add_sanchk_lt_serUuid_2);
    CU_SUITE_ADD_TEST(gatts_service_add_sanchk_lt_handle);
    CU_SUITE_ADD_TEST(gatts_value_set_sanchk_lt_handle);
    CU_SUITE_ADD_TEST(gatts_value_set_sanchk_lt_offset);
    CU_SUITE_ADD_TEST(gatts_value_set_sanchk_lt_length_cond);
    CU_SUITE_ADD_TEST(gatts_value_set_sanchk_lt_length);
    CU_SUITE_ADD_TEST(gatts_value_set_sanchk_lt_attr_val_cond);
    CU_SUITE_ADD_TEST(gatts_value_set_sanchk_lt_attr_val);
    CU_SUITE_ADD_TEST(gatts_value_set_sanchk_lt_attr_val_2);
    CU_SUITE_ADD_TEST(gatts_hvx_sanchk_lt_handle);
    CU_SUITE_ADD_TEST(gatts_hvx_sanchk_lt_param_absent);
    CU_SUITE_ADD_TEST(gatts_hvx_sanchk_lt_char_val_handle);
    CU_SUITE_ADD_TEST(gatts_hvx_sanchk_lt_type);
    CU_SUITE_ADD_TEST(gatts_hvx_sanchk_lt_offset);
    CU_SUITE_ADD_TEST(gatts_hvx_sanchk_lt_data_len_present);
    CU_SUITE_ADD_TEST(gatts_hvx_sanchk_lt_data_len);
    CU_SUITE_ADD_TEST(gatts_hvx_sanchk_lt_data_present);
    CU_SUITE_ADD_TEST(gatts_hvx_sanchk_lt_data);
    CU_SUITE_ADD_TEST(gatts_sys_attr_set_sanchk_lt_handle);
    CU_SUITE_ADD_TEST(gatts_sys_attr_set_sanchk_lt_sys_attr_present);
    CU_SUITE_ADD_TEST(gatts_sys_attr_set_sanchk_lt_sys_attr_len);
    CU_SUITE_ADD_TEST(gatts_sys_attr_set_sanchk_lt_sys_attr);
    CU_SUITE_ADD_TEST(gatts_sys_attr_get_sanchk_lt_conn_handle);
    CU_SUITE_ADD_TEST(gatts_sys_attr_get_sanchk_lt_sys_attr_len_present);
    CU_SUITE_ADD_TEST(gatts_sys_attr_get_sanchk_lt_sys_attr_len);
    CU_SUITE_ADD_TEST(gatts_sys_attr_get_sanchk_lt_sys_data_present);
}
CU_SUITE_END
