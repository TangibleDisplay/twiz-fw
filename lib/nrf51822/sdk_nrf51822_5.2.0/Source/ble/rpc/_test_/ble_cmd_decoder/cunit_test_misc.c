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
#include "ble.h"
#include "nrf51.h" /* For IRQn_Type enum used by nrf_soc.h */
#include "nrf_soc.h"
#include "ble_gap.h"
#include "ble_gatts.h"
#include "ble_srv_common.h"
#include "nrf_error.h"
#include "app_util.h"
#include "ble_hci.h"
#include "nordic_common.h"
#include "hal_transport.h"
#include "ble_rpc_defines.h"

static uint8_t       sm_expected_cmd_resp[32];

//lint -e14
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
}


static void expected_transport_api_setup(nrf_cunit_mock_call * const p_trans_api_mock_obj,
                                         const uint8_t               op_code,
                                         const uint32_t              err_code,
                                         const uint8_t               packet_type)
{
    int     index = 0;

    memset(sm_expected_cmd_resp, 0, sizeof(sm_expected_cmd_resp));

    sm_expected_cmd_resp[index++] = packet_type;

    sm_expected_cmd_resp[index++] = op_code;
    sm_expected_cmd_resp[index++] = err_code & 0x000000ff;
    sm_expected_cmd_resp[index++] = (err_code & 0x0000ff00) >> 8;
    sm_expected_cmd_resp[index++] = (err_code & 0x00ff0000) >> 16;
    sm_expected_cmd_resp[index++] = (err_code & 0xff000000) >> 24;

    p_trans_api_mock_obj->arg[0] = BLE_RPC_PKT_RESP;
    p_trans_api_mock_obj->arg[1] = (uint32_t)sm_expected_cmd_resp;
    p_trans_api_mock_obj->arg[2] = index;
}


static void cmd_decoder_misc_setup_func(void)
{
    nrf_cunit_reset_mock();
}


CU_TEST_START(ble_uuid_encode_sig_uuid_successfull)
{
    uint16_t uuid_gatt   = 0x1801;
    uint8_t  uuid_type   = BLE_UUID_TYPE_BLE;

    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;
    nrf_cunit_mock_call * p_transport_alloc_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_uuid_encode", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_alloc", &p_transport_alloc_mock_call);

    uint8_t buffer[20];
    p_transport_alloc_mock_call->result[0] = (uint32_t)buffer;

    uint8_t expected_transport_op_code   = SD_BLE_UUID_ENCODE;
    uint32_t expected_transport_err_code = NRF_SUCCESS;

    uint8_t expected_transport_data[] = {BLE_RPC_PKT_RESP,                                       // Packet type
                                         expected_transport_op_code,                             // op code
                                         (expected_transport_err_code & 0x000000ffu) >> 0,       // error code
                                         (expected_transport_err_code & 0x0000ff00u) >> 8,       // error code
                                         (expected_transport_err_code & 0x00ff0000u) >> 16,      // error code
                                         (expected_transport_err_code & 0xff000000u) >> 24,      // error code
                                         2,                                                      // length of encoded UUID
                                         0x01, 0x18};                                            // encoded UUID

    p_transport_write_mock_call->arg[1] = (uint32_t)expected_transport_data;
    p_transport_write_mock_call->arg[2] = sizeof(expected_transport_data);


    // Expected SoftDevice call
    ble_uuid_t uuid_data;

    uuid_data.uuid = 0x1801;
    uuid_data.type = uuid_type;

    p_sd_call_mock_obj->arg[0]    = (uint32_t)&uuid_data;
    p_sd_call_mock_obj->result[0] = 2;

    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                  // Packet type
                         SD_BLE_UUID_ENCODE,
                         1,                                // ble_uuid present (not null)
                         uuid_gatt & 0xFF, uuid_gatt >> 8, // ble_uuid_t->uuid
                         uuid_type,                        // ble_uuid_t->type
                         1,                                // length pointer present (not null)
                         1};                               // result buffer present (not null)

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

    (void) uuid_data;
}
CU_TEST_END


CU_TEST_START(ble_uuid_encode_uuid_not_present)
{
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;
    nrf_cunit_mock_call * p_transport_alloc_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_uuid_encode", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_alloc", &p_transport_alloc_mock_call);

    uint8_t buffer[20];
    p_transport_alloc_mock_call->result[0] = (uint32_t)buffer;

    uint8_t expected_transport_op_code   = SD_BLE_UUID_ENCODE;
    uint32_t expected_transport_err_code = NRF_ERROR_INVALID_ADDR;

    uint8_t expected_transport_data[] = {BLE_RPC_PKT_RESP,                                       // Packet type
                                         expected_transport_op_code,                             // op code
                                         (expected_transport_err_code & 0x000000ffu) >> 0,       // error code
                                         (expected_transport_err_code & 0x0000ff00u) >> 8,       // error code
                                         (expected_transport_err_code & 0x00ff0000u) >> 16,      // error code
                                         (expected_transport_err_code & 0xff000000u) >> 24};     // error code


    p_transport_write_mock_call->arg[0] = BLE_RPC_PKT_RESP;
    p_transport_write_mock_call->arg[1] = (uint32_t)expected_transport_data;
    p_transport_write_mock_call->arg[2] = sizeof(expected_transport_data);

    p_sd_call_mock_obj->arg[0]    = (uint32_t)NULL;

    p_sd_call_mock_obj->error = NRF_ERROR_INVALID_ADDR;

    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                  // Packet type
                         SD_BLE_UUID_ENCODE,
                         0,                                // ble_uuid present (not null)
                         1,                                // length pointer present (null)
                         1};                               // result buffer present (not null)

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(ble_uuid_encode_uuid_length_pointer_not_present)
{
    uint16_t uuid_gatt   = 0x1801;
    uint8_t  uuid_type   = BLE_UUID_TYPE_BLE;

    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;
    nrf_cunit_mock_call * p_transport_alloc_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_uuid_encode", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_alloc", &p_transport_alloc_mock_call);

    uint8_t buffer[20];
    p_transport_alloc_mock_call->result[0] = (uint32_t)buffer;

    uint8_t expected_transport_op_code   = SD_BLE_UUID_ENCODE;
    uint32_t expected_transport_err_code = NRF_ERROR_INVALID_ADDR;

    uint8_t expected_transport_data[] = {BLE_RPC_PKT_RESP,                                       // Packet type
                                         expected_transport_op_code,                             // op code
                                         (expected_transport_err_code & 0x000000ffu) >> 0,       // error code
                                         (expected_transport_err_code & 0x0000ff00u) >> 8,       // error code
                                         (expected_transport_err_code & 0x00ff0000u) >> 16,      // error code
                                         (expected_transport_err_code & 0xff000000u) >> 24};     // error code


    p_transport_write_mock_call->arg[0] = BLE_RPC_PKT_RESP;
    p_transport_write_mock_call->arg[1] = (uint32_t)expected_transport_data;
    p_transport_write_mock_call->arg[2] = sizeof(expected_transport_data);


    // Expected SoftDevice call
    ble_uuid_t uuid_data;

    uuid_data.uuid = 0x1801;
    uuid_data.type = uuid_type;

    p_sd_call_mock_obj->arg[0]    = (uint32_t)&uuid_data;
    p_sd_call_mock_obj->result[0] = 2;

    p_sd_call_mock_obj->error = NRF_ERROR_INVALID_ADDR;

    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                  // Packet type
                         SD_BLE_UUID_ENCODE,
                         1,                                // ble_uuid present (not null)
                         uuid_gatt & 0xFF, uuid_gatt >> 8, // ble_uuid_t->uuid
                         uuid_type,                        // ble_uuid_t->type
                         0,                                // length pointer present (null)
                         1};                               // result buffer present (not null)

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

    (void)uuid_data;
}
CU_TEST_END

CU_TEST_START(ble_uuid_encode_vs_uuid_successfull)
{
    uint16_t uuid_gatt   = 0x1801;
    uint8_t  uuid_type   = BLE_UUID_TYPE_VENDOR_BEGIN;

    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;
    nrf_cunit_mock_call * p_transport_alloc_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_uuid_encode", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_alloc", &p_transport_alloc_mock_call);

    uint8_t buffer[30];
    p_transport_alloc_mock_call->result[0] = (uint32_t)buffer;

    uint8_t expected_transport_op_code   = SD_BLE_UUID_ENCODE;
    uint32_t expected_transport_err_code = NRF_SUCCESS;

    uint8_t expected_transport_data[] = {BLE_RPC_PKT_RESP,                                              // Packet type
                                         expected_transport_op_code,                                    // op code
                                         (expected_transport_err_code & 0x000000ffu) >> 0,              // error code
                                         (expected_transport_err_code & 0x0000ff00u) >> 8,              // error code
                                         (expected_transport_err_code & 0x00ff0000u) >> 16,             // error code
                                         (expected_transport_err_code & 0xff000000u) >> 24,             // error code
                                         16,                                                            // length of encoded UUID
                                         0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x11, 0x22, 0x33, // VS UUID base
                                         0x01, 0x18,                                                    // encoded UUID
                                         0x66, 0x77};                                                   // VS UUID base

    p_transport_write_mock_call->arg[0] = BLE_RPC_PKT_RESP;
    p_transport_write_mock_call->arg[1] = (uint32_t)expected_transport_data;
    p_transport_write_mock_call->arg[2] = sizeof(expected_transport_data);


    // Expected SoftDevice call
    ble_uuid_t uuid_data;

    uuid_data.uuid = 0x1801;
    uuid_data.type = uuid_type;

    uint8_t vendor_specific_base[] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x11, 0x22,
                                      0x33, 0x44,
                                      0x55, 0x66, 0x77};

    p_sd_call_mock_obj->arg[0]    = (uint32_t)&uuid_data;
    p_sd_call_mock_obj->arg[1]    = (uint32_t)vendor_specific_base;
    p_sd_call_mock_obj->result[0] = 16;

    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                  // Packet type
                         SD_BLE_UUID_ENCODE,
                         1,                                // ble_uuid present (not null)
                         uuid_gatt & 0xFF, uuid_gatt >> 8, // ble_uuid_t->uuid
                         uuid_type,                        // ble_uuid_t->type
                         1,                                // length pointer present (not null)
                         1};                               // result buffer present (not null)

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

    (void)uuid_data;
}
CU_TEST_END


/* This test case will make the SoftDevice return error to the command decoder module and will
 * test if this error is encoded properly in the command response.
 */
CU_TEST_START(misc_soft_device_retuns_error)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;
    nrf_cunit_mock_call * p_transport_alloc_mock_call;
    nrf_cunit_mock_call * p_transport_consume_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_adv_data_set", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_alloc", &p_transport_alloc_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_consume", &p_transport_consume_mock_call);

    p_transport_consume_mock_call->compare_rule[0] = COMPARE_ANY;

    uint8_t buffer[40];
    p_transport_alloc_mock_call->result[0] = (uint32_t)buffer;

    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                                 // Packet type
                         SD_BLE_GAP_ADV_DATA_SET,                     // Opcode
                         31,                                              // Adv data len
                         0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,  // Adv data
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
                         0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                         0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                         0                                               // SR data len
                         };

    p_sd_call_mock_obj->error = NRF_ERROR_BUSY;

    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                              SD_BLE_GAP_ADV_DATA_SET,
                              NRF_ERROR_BUSY,
                              BLE_RPC_PKT_RESP);

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

    nrf_cunit_verify_call_return();
}
CU_TEST_END


/* This test case will test if the unrecognised command is properly responded to with a command
 * response with error code set to NRF_ERROR_NOT_SUPPORTED.
 */
CU_TEST_START(misc_unsupported_command)
{

    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;
    nrf_cunit_mock_call * p_transport_alloc_mock_call;
    nrf_cunit_mock_call * p_transport_consume_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_alloc", &p_transport_alloc_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_consume", &p_transport_consume_mock_call);

    p_transport_consume_mock_call->compare_rule[0] = COMPARE_ANY;

    uint8_t buffer[20];
    p_transport_alloc_mock_call->result[0] = (uint32_t)buffer;

    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 0xFF, // INVALID_OPCODE
                                 NRF_ERROR_NOT_SUPPORTED,
                                 BLE_RPC_PKT_RESP);

    // Stimuli

    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                        // Packet type
                         0xFF,                                   // INVALID Opcode
                         0x23,                                   // Some data.
                         0x04, 0x00,
                         0x05, 0x06, 0x07, 0x08};

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

    nrf_cunit_verify_call_return();
}
CU_TEST_END


CU_TEST_START(misc_sd_power_system_off)
{
    nrf_cunit_mock_call *      p_sd_mock_call;
    nrf_cunit_mock_call *      p_transport_read_mock_call;
    nrf_cunit_mock_call *      p_transport_consume_mock_call;

    nrf_cunit_expect_call_return((uint8_t *) "sd_power_system_off", &p_sd_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_consume", &p_transport_consume_mock_call);

    p_transport_consume_mock_call->compare_rule[0] = COMPARE_ANY;

    p_sd_mock_call->arg[0] = 0x56;

    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                        // Packet type
                         SD_POWER_SYSTEM_OFF};               // Opcode


    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

    nrf_cunit_verify_call_return();
}
CU_TEST_END


CU_TEST_START(ble_uuid_encode_sanchk_len_lt_UUID)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;
    nrf_cunit_mock_call * p_transport_alloc_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_alloc", &p_transport_alloc_mock_call);

    uint8_t buffer[20];
    p_transport_alloc_mock_call->result[0] = (uint32_t)buffer;

    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_UUID_ENCODE,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,               // Packet type
                         SD_BLE_UUID_ENCODE,
                         0x1,                            // uuid present
                         0x19, 0x28,                     // uuid
                         0x01,                           // type
                         0x01,                           // length present
                         0x01                            // uuid result buffer present
                         };

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 1;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(ble_uuid_encode_sanchk_len_lt_UUID_nt_present)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;
    nrf_cunit_mock_call * p_transport_alloc_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_alloc", &p_transport_alloc_mock_call);

    uint8_t buffer[20];
    p_transport_alloc_mock_call->result[0] = (uint32_t)buffer;

    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_UUID_ENCODE,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,               // Packet type
                         SD_BLE_UUID_ENCODE,        // Opcode
                         0x0,                            // uuid present
                         0x19, 0x28,                     // uuid
                         0x01,                           // type
                         0x01,                           // length present
                         0x01                            // uuid result buffer present
                         };
    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 1;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(ble_uuid_encode_sanchk_len_lt_cond_2)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;
    nrf_cunit_mock_call * p_transport_alloc_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_alloc", &p_transport_alloc_mock_call);

    uint8_t buffer[20];
    p_transport_alloc_mock_call->result[0] = (uint32_t)buffer;

    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_UUID_ENCODE,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                // Packet type
                         SD_BLE_UUID_ENCODE,         // Opcode
                         0x1,                            // uuid present
                         0x19, 0x28,                     // uuid
                         0x01,                           // type
                         0x01,                           // length present
                         0x01                            // uuid result buffer present
                         };
    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 3;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(ble_uuid_encode_sanchk_len_lt_cond)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;
    nrf_cunit_mock_call * p_transport_alloc_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_alloc", &p_transport_alloc_mock_call);

    uint8_t buffer[20];
    p_transport_alloc_mock_call->result[0] = (uint32_t)buffer;

    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_UUID_ENCODE,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                // Packet type
                         SD_BLE_UUID_ENCODE,
                         0x1,                            // uuid present
                         0x19, 0x28,                     // uuid
                         0x01,                           // type
                         0x01,                           // length present
                         0x01                            // uuid result buffer present
                         };
    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 3;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(ble_uuid_encode_sanchk_len_lt_length)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;
    nrf_cunit_mock_call * p_transport_alloc_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_alloc", &p_transport_alloc_mock_call);

    uint8_t buffer[20];
    p_transport_alloc_mock_call->result[0] = (uint32_t)buffer;

    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_UUID_ENCODE,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,               // Packet type
                         SD_BLE_UUID_ENCODE,
                         0x1,                            // uuid present
                         0x19, 0x28,                     // uuid
                         0x01,                           // type
                         0x01,                           // length present
                         0x01                            // uuid result buffer present
                         };
    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 4;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(ble_uuid_encode_sanchk_len_lt_uuid_buff_present)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;
    nrf_cunit_mock_call * p_transport_alloc_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_alloc", &p_transport_alloc_mock_call);

    uint8_t buffer[20];
    p_transport_alloc_mock_call->result[0] = (uint32_t)buffer;

    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_UUID_ENCODE,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,               // Packet type
                         SD_BLE_UUID_ENCODE,        // Opcode
                         0x1,                            // uuid present
                         0x19, 0x28,                     // uuid
                         0x01,                           // type
                         0x01,                           // length present
                         0x01                            // uuid result buffer present
                         };
    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 5;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(cmd_decoder_alloc_error_in_command_resp_send_should_wait_for_free_buffer)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_mock_call;

    /*** Data that is received on from the transport layer. ***********/
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                        // Packet type
                         0xFF,                                   // INVALID opcode
                         0x23,                                   // Some data.
                         0x04, 0x00,
                         0x05, 0x06, 0x07, 0x08};


    /*** Data has been received, and a call to pkt_extract is expected, return received data. ***/
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_mock_call);
    p_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_mock_call->result[0] = (uint32_t)in_data;
    p_mock_call->result[1] = sizeof(in_data);

    /************* A response should be returned, allocate a tx buffer. *************/
    /*************** No buffer is available, so a NO_MEM is returned. ***************/
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_alloc", &p_mock_call);
    p_mock_call->error = NRF_ERROR_NO_MEM;

    /************* A response should be returned, allocate a tx buffer. *************/
    /***********Second call to alloc returns a buffer which is now free. ************/
    uint8_t buffer[20];
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_alloc", &p_mock_call);
    p_mock_call->result[0] = (uint32_t)buffer;

    /********************* Setting up the expected response. *********************/
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_mock_call);
    expected_transport_api_setup(p_mock_call,
                                 0xFF,
                                 NRF_ERROR_NOT_SUPPORTED,
                                 BLE_RPC_PKT_RESP);

    /************* When cmd response has been send, the rx buffer should be freed. *************/
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->compare_rule[0] = COMPARE_ANY;

    ble_rpc_cmd_handle(NULL, 0);

    nrf_cunit_verify_call_return();
}
CU_TEST_END


CU_TEST_START(cmd_decoder_alloc_error_in_command_resp_send_with_data_should_wait_for_free_buffer)
{
    uint16_t uuid_gatt   = 0x1801;
    uint8_t  uuid_type   = BLE_UUID_TYPE_BLE;

    nrf_cunit_mock_call * p_mock_call;
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_alloc_mock_call;
    nrf_cunit_mock_call * p_transport_consume_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_uuid_encode", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_alloc", &p_transport_alloc_mock_call);
    p_transport_alloc_mock_call->error = NRF_ERROR_NO_MEM;

    uint8_t buffer[20];
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_alloc", &p_mock_call);
    p_mock_call->result[0] = (uint32_t)buffer;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_consume", &p_transport_consume_mock_call);
    p_transport_consume_mock_call->compare_rule[0] = COMPARE_ANY;
    p_transport_alloc_mock_call->error = NRF_ERROR_NO_MEM;

    /********************* Setting up the expected response. *********************/
    uint8_t expected_transport_op_code   = SD_BLE_UUID_ENCODE;
    uint32_t expected_transport_err_code = NRF_SUCCESS;

    uint8_t expected_transport_data[] = {BLE_RPC_PKT_RESP,                                       // Packet type
                                         expected_transport_op_code,                             // op code
                                         (expected_transport_err_code & 0x000000ffu) >> 0,       // error code
                                         (expected_transport_err_code & 0x0000ff00u) >> 8,       // error code
                                         (expected_transport_err_code & 0x00ff0000u) >> 16,      // error code
                                         (expected_transport_err_code & 0xff000000u) >> 24,      // error code
                                         2,                                                      // length of encoded UUID
                                         0x01, 0x18};                                            // encoded UUID

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected_transport_data;
    p_mock_call->arg[2] = sizeof(expected_transport_data);

    // Expected SoftDevice call
    ble_uuid_t uuid_data;

    uuid_data.uuid = 0x1801;
    uuid_data.type = uuid_type;

    p_sd_call_mock_obj->arg[0]    = (uint32_t)&uuid_data;
    p_sd_call_mock_obj->result[0] = 2;

    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                  // Packet type
                         SD_BLE_UUID_ENCODE,           // Opcode
                         1,                                // ble_uuid present (not null)
                         uuid_gatt & 0xFF, uuid_gatt >> 8, // ble_uuid_t->uuid
                         uuid_type,                        // ble_uuid_t->type
                         1,                                // length pointer present (not null)
                         1};                               // result buffer present (not null)

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

    nrf_cunit_verify_call_return();
    (void) uuid_data;
}
CU_TEST_END


//lint -e14
CU_SUITE_START_PUBLIC(cmd_decoder_misc_suite)
{
    CU_SUITE_SETUP_FUNC(cmd_decoder_misc_setup_func);

    CU_SUITE_ADD_TEST(misc_soft_device_retuns_error);
    CU_SUITE_ADD_TEST(misc_unsupported_command);
    CU_SUITE_ADD_TEST(misc_sd_power_system_off);
    CU_SUITE_ADD_TEST(cmd_decoder_alloc_error_in_command_resp_send_should_wait_for_free_buffer);
    CU_SUITE_ADD_TEST(cmd_decoder_alloc_error_in_command_resp_send_with_data_should_wait_for_free_buffer);
}
CU_SUITE_END

static void ble_setup_func(void)
{
    nrf_cunit_reset_mock();

    nrf_cunit_mock_call * p_transport_mock_call;
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_consume", &p_transport_mock_call);
    p_transport_mock_call->compare_rule[0] = COMPARE_ANY;
}

static void ble_teardown_func(void)
{
    nrf_cunit_verify_call_return();
}

//lint -e14
CU_SUITE_START_PUBLIC(cmd_decoder_ble) 
{
    CU_SUITE_SETUP_FUNC(ble_setup_func);
    CU_SUITE_TEARDOWN_FUNC(ble_teardown_func);

    CU_SUITE_ADD_TEST(ble_uuid_encode_sig_uuid_successfull);
    CU_SUITE_ADD_TEST(ble_uuid_encode_vs_uuid_successfull);
    CU_SUITE_ADD_TEST(ble_uuid_encode_uuid_not_present);
    CU_SUITE_ADD_TEST(ble_uuid_encode_uuid_length_pointer_not_present);
    CU_SUITE_ADD_TEST(ble_uuid_encode_sanchk_len_lt_UUID);
    CU_SUITE_ADD_TEST(ble_uuid_encode_sanchk_len_lt_UUID_nt_present);
    CU_SUITE_ADD_TEST(ble_uuid_encode_sanchk_len_lt_cond);
    CU_SUITE_ADD_TEST(ble_uuid_encode_sanchk_len_lt_cond_2);
    CU_SUITE_ADD_TEST(ble_uuid_encode_sanchk_len_lt_length);
    CU_SUITE_ADD_TEST(ble_uuid_encode_sanchk_len_lt_uuid_buff_present);

}
CU_SUITE_END

