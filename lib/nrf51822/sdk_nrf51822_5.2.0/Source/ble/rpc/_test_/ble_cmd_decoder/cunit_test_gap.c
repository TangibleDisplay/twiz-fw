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
#include "ble_rpc_defines.h"
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
} expected_transport_data_t;

static const uint8_t sm_sample_adv_data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                             0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
                                             0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                             0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
static uint8_t       sm_expected_cmd_resp[32];

static uint8_t m_tx_buffer[1200];

static void expected_transport_api_setup(nrf_cunit_mock_call * const p_trans_api_mock_obj,
                                         uint8_t                     op_code,
                                         const uint32_t              err_code,
                                         uint8_t                     packet_type)
{
    int     index = 0;

    memset(sm_expected_cmd_resp, 0, sizeof(sm_expected_cmd_resp));

    sm_expected_cmd_resp[index++] = packet_type;
    
    sm_expected_cmd_resp[index++] = op_code;
    sm_expected_cmd_resp[index++] = err_code & 0x000000ff;
    sm_expected_cmd_resp[index++] = (err_code & 0x0000ff00) >> 8;
    sm_expected_cmd_resp[index++] = (err_code & 0x00ff0000) >> 16;
    sm_expected_cmd_resp[index++] = (err_code & 0xff000000) >> 24;

    p_trans_api_mock_obj->arg[1] = (uint32_t)sm_expected_cmd_resp;
    p_trans_api_mock_obj->arg[2] = index;
}


static void expected_transport_api_data_setup(nrf_cunit_mock_call             * const p_trans_api_mock_obj,
                                              expected_transport_data_t const * const expected_data,
                                              uint8_t                                 packet_type)
{
    uint32_t index = 0;
    uint32_t i     = 0;

    memset(sm_expected_cmd_resp, 0, sizeof(sm_expected_cmd_resp));

    sm_expected_cmd_resp[index++] = packet_type;

    sm_expected_cmd_resp[index++] = expected_data->op_code;
    sm_expected_cmd_resp[index++] = expected_data->err_code & 0x000000ff;
    sm_expected_cmd_resp[index++] = (expected_data->err_code & 0x0000ff00) >> 8;
    sm_expected_cmd_resp[index++] = (expected_data->err_code & 0x00ff0000) >> 16;
    sm_expected_cmd_resp[index++] = (expected_data->err_code & 0xff000000) >> 24;

    for (; i < expected_data->ext_data_len; i++)
    {
        sm_expected_cmd_resp[index++] = expected_data->extended_data[i];
    }

    p_trans_api_mock_obj->arg[0] = BLE_RPC_PKT_RESP;
    p_trans_api_mock_obj->arg[1] = (uint32_t)sm_expected_cmd_resp;
    p_trans_api_mock_obj->arg[2] = index;
}


static void expected_adv_data_setup(nrf_cunit_mock_call * const p_sd_call_mock_obj,
                                    const uint8_t               raw_adv_data_len,
                                    const uint8_t               raw_sr_data_len,
                                    uint8_t * const             p_encoded_cmd)
{
    p_sd_call_mock_obj->arg[0] = (raw_adv_data_len == 0) ? 0 : (uint32_t)(sm_sample_adv_data);
    p_sd_call_mock_obj->arg[1] = (uint32_t)(raw_adv_data_len);
    p_sd_call_mock_obj->arg[2] = (raw_sr_data_len == 0) ? 0 : (uint32_t)(sm_sample_adv_data);
    p_sd_call_mock_obj->arg[3] = (uint32_t)(raw_sr_data_len);

    int index = 0;

    // Encode packet type
    p_encoded_cmd[index++] = BLE_RPC_PKT_CMD;

    // Encode OPCode
    p_encoded_cmd[index++] = SD_BLE_GAP_ADV_DATA_SET;

    // Encode adv data len
    p_encoded_cmd[index++] = raw_adv_data_len;

    // Encode adv data
    memcpy(&(p_encoded_cmd[index]), sm_sample_adv_data, raw_adv_data_len);
    index += raw_adv_data_len;

    // Encode SR data len
    p_encoded_cmd[index++] = raw_sr_data_len;

    // Encode SR data
    memcpy(&(p_encoded_cmd[index]), sm_sample_adv_data, raw_sr_data_len);
}


/** @brief Function for setting up a sample whitelist structure for testing purpose.
*/
static void expected_white_list_setup(ble_gap_whitelist_t * const p_white_list,
                                          const uint8_t               addr_cnt_needed,
                                          const uint8_t               irk_cnt_needed,
                                          uint8_t * const             p_exp_encoded_wl)
{
    static ble_gap_addr_t * sp_addresses[BLE_GAP_WHITELIST_ADDR_MAX_COUNT];
    static ble_gap_irk_t  * sp_irks[BLE_GAP_WHITELIST_IRK_MAX_COUNT];
    static bool             s_is_first_time_call = true;
    static ble_gap_addr_t   s_sample_gap_addr_obj;
    static ble_gap_irk_t    s_sample_irk_obj;
    uint8_t                 sample_ble_add[] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xab};
    uint8_t                 sample_irk[]     = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                                0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10};

    if (s_is_first_time_call)
    {
        // Initialize the static variables used by this function.
        s_is_first_time_call = false;

        s_sample_gap_addr_obj.addr_type = BLE_GAP_ADDR_TYPE_RANDOM_STATIC;
        memcpy(s_sample_gap_addr_obj.addr, sample_ble_add, sizeof(s_sample_gap_addr_obj.addr));

        uint8_t i;
        for (i = 0; i < (sizeof(sp_addresses) / sizeof(ble_gap_addr_t *)); i++)
        {
            sp_addresses[i] = &s_sample_gap_addr_obj;
        }

        memcpy(s_sample_irk_obj.irk, sample_irk, sizeof(s_sample_irk_obj.irk));
        for (i = 0; i < (sizeof(sp_irks) / sizeof(ble_gap_irk_t *)); i++)
        {
            sp_irks[i] = &s_sample_irk_obj;
        }
    }

    p_white_list->addr_count    = addr_cnt_needed;
    p_white_list->irk_count     = irk_cnt_needed;
    p_white_list->pp_addrs      = (addr_cnt_needed != 0) ? sp_addresses : NULL;
    p_white_list->pp_irks       = (irk_cnt_needed != 0) ? sp_irks : NULL;

    // Fill the expected encoded whitelist
    int index = 0;
    int i;
    p_exp_encoded_wl[index++] = addr_cnt_needed;

    for (i = 0; i < addr_cnt_needed; i++)
    {
        p_exp_encoded_wl[index++] = BLE_GAP_ADDR_TYPE_RANDOM_STATIC;
        memcpy(&(p_exp_encoded_wl[index]), sample_ble_add, sizeof(sample_ble_add));
        index+= sizeof(sample_ble_add);
    }

    p_exp_encoded_wl[index++] = irk_cnt_needed;

    for (i = 0; i < irk_cnt_needed; i++)
    {
        memcpy(&(p_exp_encoded_wl[index]), sample_irk, sizeof(sample_irk));
        index+= sizeof(sample_irk);
    }
}


/* This test case will test if the  RPC_SD_BLE_GAP_DEVICE_NAME_SET command is decoded properly
 * by the decoder and the command response was written to transport layer.
 */
CU_TEST_START(gap_device_name_set)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_device_name_set", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected SoftDevice call
    ble_gap_conn_sec_mode_t exp_write_perm;

    exp_write_perm.sm = 2;
    exp_write_perm.lv = 3;

    uint8_t     exp_dev_name[]  = {5, 6, 7, 8};
    uint16_t    exp_len         = 4;

    p_sd_call_mock_obj->arg[0]    = (uint32_t)&exp_write_perm;
    p_sd_call_mock_obj->arg[1]    = (uint32_t)exp_dev_name;
    p_sd_call_mock_obj->arg[2]    = (uint32_t)exp_len;

    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call, 
                                 SD_BLE_GAP_DEVICE_NAME_SET,
                                 NRF_SUCCESS,
                                 BLE_RPC_PKT_RESP);

    // Stimuli

    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                     // Packet type
                         SD_BLE_GAP_DEVICE_NAME_SET,
                         RPC_BLE_FIELD_PRESENT,               // Write Permissions Present
                         0x32,                                // Write Permissions
                         0x04, 0x00,                          // Device name length
                         RPC_BLE_FIELD_PRESENT,               // Device Name Present
                         0x05, 0x06, 0x07, 0x08};             // Device Name

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_device_name_set_sanchk_cmdlen_lt_dev_name_len)
{
    // Preparation of expected output

    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    //nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_device_name_set", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected SoftDevice call

//    exp_write_perm.sm = 2;
//    exp_write_perm.lv = 3;
//
//    uint8_t     exp_dev_name[]  = {5, 6, 7, 8};
//    uint16_t    exp_len         = 2;

//    p_sd_call_mock_obj->arg[0]    = (uint32_t)&exp_write_perm;
//    p_sd_call_mock_obj->arg[1]    = (uint32_t)exp_dev_name;
//    p_sd_call_mock_obj->arg[2]    = (uint32_t)exp_len;

    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_DEVICE_NAME_SET,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli

    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                     // Packet type
                         SD_BLE_GAP_DEVICE_NAME_SET,
                         0x01,                                // write permission present
                         0x32,                                // Write Permissions
                         0x02, 0x00,                          // Device name length
                         0x05, 0x06, 0x07, 0x08};             // Device Name

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 4; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END

CU_TEST_START(gap_device_name_set_sanchk_cmdlen_lt_actual_pkt)
{
    // Preparation of expected output
    //nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    //nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_device_name_set", &p_sd_call_mock_obj); - This will not be called at all
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_DEVICE_NAME_SET,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli

    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                     // Packet type
                         SD_BLE_GAP_DEVICE_NAME_SET,
                         0x01,                                // write permission present
                         0x32,                                // Write Permissions
                         0x00, 0x40,                          // Device name length
                         0x01,                                // device name present
                         0x05, 0x06, 0x07, 0x08};             // Device Name

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 8; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will test if the  RPC_SD_BLE_GAP_DEVICE_NAME_SET command is decoded properly
 * by the decoder when the optional params are absent and the command response was written to transport layer.
 */
CU_TEST_START(nrffosdk_1302_gap_device_name_set_optional_params_absent)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_device_name_set", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected SoftDevice call
    uint16_t    exp_len = 4;

    p_sd_call_mock_obj->arg[0]    = 0;
    p_sd_call_mock_obj->arg[1]    = 0;
    p_sd_call_mock_obj->arg[2]    = (uint32_t)exp_len;

    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_DEVICE_NAME_SET,
                                 NRF_SUCCESS,
                                 BLE_RPC_PKT_RESP);

    // Stimuli

    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                         // Packet type
                         SD_BLE_GAP_DEVICE_NAME_SET,
                         RPC_BLE_FIELD_NOT_PRESENT,               // Write Permissions NOT Present
                         0x04, 0x00,                              // Device name length
                         RPC_BLE_FIELD_NOT_PRESENT,               // Device Name NOT Present
                         };

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 9; // sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will test if the  RPC_SD_BLE_GAP_DEVICE_NAME_GET command is decoded properly
 * by the decoder and the command response was written to transport layer.
 */
CU_TEST_START(gap_device_name_get_successful)
{
   nrf_cunit_mock_call * p_sd_call_mock_obj;
   nrf_cunit_mock_call * p_transport_read_mock_call;
   nrf_cunit_mock_call * p_transport_write_mock_call;

   nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_device_name_get", &p_sd_call_mock_obj);
   nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
   nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

   uint8_t expected_transport_op_code   = SD_BLE_GAP_DEVICE_NAME_GET;
   uint32_t expected_transport_err_code = NRF_SUCCESS;
   uint8_t zero = 0;
   uint8_t expected_transport_data[] = {BLE_RPC_PKT_RESP,                                       // packet type
                                        expected_transport_op_code,                             // op code
                                        (expected_transport_err_code & 0x000000ffu) >> 0,       // error code
                                        (expected_transport_err_code & 0x0000ff00u) >> 8,       // error code
                                        (expected_transport_err_code & 0x00ff0000u) >> 16,      // error code
                                        (expected_transport_err_code & 0xff000000u) >> 24,      // error code
                                        11, zero,                                               // length of device name
                                        'H', 'e', 'l', 'l', 'o', '_', 'w', 'o', 'r', 'l', 'd'}; // device name

   p_transport_write_mock_call->arg[1] = (uint32_t)expected_transport_data;
   p_transport_write_mock_call->arg[2] = sizeof(expected_transport_data);

   uint8_t device_name[] = {'H', 'e', 'l', 'l', 'o', '_', 'w', 'o', 'r', 'l', 'd'};
   p_sd_call_mock_obj->arg[0] = (uint32_t)device_name;
   p_sd_call_mock_obj->arg[1] = sizeof(device_name);


   uint16_t buffer_length = 20;
   uint8_t in_data[] = {BLE_RPC_PKT_CMD,                // Packet type
                        SD_BLE_GAP_DEVICE_NAME_GET,
                        1,                              // Length field present
                        0, 0,                           // Placeholder for length field
                        1};                             // Device name buffer present
   (void)uint16_encode(buffer_length, &in_data[3]);

   p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
   p_transport_read_mock_call->result[0] = (uint32_t)in_data;
   p_transport_read_mock_call->result[1] = sizeof(in_data);

   ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will test if the  RPC_SD_BLE_GAP_DEVICE_NAME_GET command is decoded properly
 * by the decoder and an error code is returned if length field is not present.
 */
CU_TEST_START(gap_device_name_get_length_field_not_present)
{
   nrf_cunit_mock_call * p_sd_call_mock_obj;
   nrf_cunit_mock_call * p_transport_read_mock_call;
   nrf_cunit_mock_call * p_transport_write_mock_call;

   nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_device_name_get", &p_sd_call_mock_obj);
   nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
   nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

   uint8_t expected_transport_op_code   = SD_BLE_GAP_DEVICE_NAME_GET;
   uint32_t expected_transport_err_code = NRF_ERROR_INVALID_ADDR;

   uint8_t expected_transport_data[] = {BLE_RPC_PKT_RESP,                                       // packet type
                                        expected_transport_op_code,                             // op code
                                        (expected_transport_err_code & 0x000000ffu) >> 0,       // error code
                                        (expected_transport_err_code & 0x0000ff00u) >> 8,       // error code
                                        (expected_transport_err_code & 0x00ff0000u) >> 16,      // error code
                                        (expected_transport_err_code & 0xff000000u) >> 24};     // error code

   p_transport_write_mock_call->arg[1] = (uint32_t)expected_transport_data;
   p_transport_write_mock_call->arg[2] = sizeof(expected_transport_data);

   p_sd_call_mock_obj->error = NRF_ERROR_INVALID_ADDR;

   uint8_t in_data[] = {BLE_RPC_PKT_CMD,                // Packet type
                        SD_BLE_GAP_DEVICE_NAME_GET,
                        0,                              // Length _NOT_ present
                        1};                             // Device name buffer present

   p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
   p_transport_read_mock_call->result[0] = (uint32_t)in_data;
   p_transport_read_mock_call->result[1] = sizeof(in_data);

   ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will test if the  RPC_SD_BLE_GAP_DEVICE_NAME_GET command is decoded properly
 * by the decoder and an error code is returned if result buffer field is not present.
 */
CU_TEST_START(gap_device_name_get_remote_result_buffer_not_present)
{
   nrf_cunit_mock_call * p_sd_call_mock_obj;
   nrf_cunit_mock_call * p_transport_read_mock_call;
   nrf_cunit_mock_call * p_transport_write_mock_call;

   nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_device_name_get", &p_sd_call_mock_obj);
   nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
   nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

   uint8_t expected_transport_op_code   = SD_BLE_GAP_DEVICE_NAME_GET;
   uint32_t expected_transport_err_code = NRF_ERROR_INVALID_ADDR;

   uint8_t expected_transport_data[] = {BLE_RPC_PKT_RESP,                                       // packet type
                                        expected_transport_op_code,                             // op code
                                        (expected_transport_err_code & 0x000000ffu) >> 0,       // error code
                                        (expected_transport_err_code & 0x0000ff00u) >> 8,       // error code
                                        (expected_transport_err_code & 0x00ff0000u) >> 16,      // error code
                                        (expected_transport_err_code & 0xff000000u) >> 24};     // error code

   p_transport_write_mock_call->arg[1] = (uint32_t)expected_transport_data;
   p_transport_write_mock_call->arg[2] = sizeof(expected_transport_data);

   p_sd_call_mock_obj->error = NRF_ERROR_INVALID_ADDR;

   uint16_t buffer_length = 20;
   uint8_t in_data[] = {BLE_RPC_PKT_CMD,                // Packet type
                        SD_BLE_GAP_DEVICE_NAME_GET,
                        1,                              // Length present
                        0, 0,                           // Placeholder for buffer size
                        0};                             // Device name buffer present
   (void)uint16_encode(buffer_length, &in_data[3]);

   p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
   p_transport_read_mock_call->result[0] = (uint32_t)in_data;
   p_transport_read_mock_call->result[1] = sizeof(in_data);

   ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will test if the  RPC_SD_BLE_GAP_DEVICE_NAME_GET command is propagating an error
 * if the buffer size suggested by the caller is to small for the SoftDevice.
 */
CU_TEST_START(gap_device_name_get_to_small_buffer_given_to_softdevice)
{
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_device_name_get", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    uint8_t expected_transport_op_code   = SD_BLE_GAP_DEVICE_NAME_GET;
    uint32_t expected_transport_err_code = NRF_ERROR_DATA_SIZE;

    uint8_t expected_transport_data[] = {BLE_RPC_PKT_RESP,                                       // packet type
                                         expected_transport_op_code,                             // op code
                                         (expected_transport_err_code & 0x000000ffu) >> 0,       // error code
                                         (expected_transport_err_code & 0x0000ff00u) >> 8,       // error code
                                         (expected_transport_err_code & 0x00ff0000u) >> 16,      // error code
                                         (expected_transport_err_code & 0xff000000u) >> 24       // error code
                                         }; // device name

    p_transport_write_mock_call->arg[0] = BLE_RPC_PKT_RESP;
    p_transport_write_mock_call->arg[1] = (uint32_t)expected_transport_data;
    p_transport_write_mock_call->arg[2] = sizeof(expected_transport_data);

    p_sd_call_mock_obj->error = NRF_ERROR_DATA_SIZE;

    uint16_t buffer_length = 20;
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                // Packet type
                         SD_BLE_GAP_DEVICE_NAME_GET,
                         1,                              // Length present
                         0, 0,                           // Placeholder for buffer size
                         1};                             // Device name buffer present
    (void)uint16_encode(buffer_length, &in_data[3]);

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will test if the  RPC_SD_BLE_GAP_DEVICE_NAME_GET command is propagating an error
 * if SoftDevice is reporting invalid address.
 */
CU_TEST_START(gap_device_name_get_invalid_addr_is_propagated)
{
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_device_name_get", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    uint8_t expected_transport_op_code   = SD_BLE_GAP_DEVICE_NAME_GET;
    uint32_t expected_transport_err_code = NRF_ERROR_INVALID_ADDR;

    uint8_t expected_transport_data[] = {BLE_RPC_PKT_RESP,                                       // packet type
                                         expected_transport_op_code,                             // op code
                                         (expected_transport_err_code & 0x000000ffu) >> 0,       // error code
                                         (expected_transport_err_code & 0x0000ff00u) >> 8,       // error code
                                         (expected_transport_err_code & 0x00ff0000u) >> 16,      // error code
                                         (expected_transport_err_code & 0xff000000u) >> 24       // error code
                                         }; // device name

    p_transport_write_mock_call->arg[0] = BLE_RPC_PKT_RESP;
    p_transport_write_mock_call->arg[1] = (uint32_t)expected_transport_data;
    p_transport_write_mock_call->arg[2] = sizeof(expected_transport_data);

    p_sd_call_mock_obj->error = NRF_ERROR_INVALID_ADDR;

    uint16_t buffer_length = 20;
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                   // Packet type
                         SD_BLE_GAP_DEVICE_NAME_GET,
                            1,                              // Length present
                            0, 0,                           // Placeholder for buffer size
                            1};                             // Device name buffer present
    (void)uint16_encode(buffer_length, &in_data[3]);

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will test if the  RPC_SD_BLE_GAP_DEVICE_NAME_GET command is sending an error
 * if the suggested size is larger than the one provided by the command handler. The buffer in the
 * command handler should in general be large enough to handle its return values by the SoftDevice
 * call. If not we will have to handle this error, so that the SoftDevice will not write out
 * of bounds. SoftDevice function should not even be called.
 */
CU_TEST_START(gap_device_name_get_to_big_buffer_indicated_by_caller)
{
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    uint8_t expected_transport_op_code   = SD_BLE_GAP_DEVICE_NAME_GET;
    uint32_t expected_transport_err_code = NRF_ERROR_INVALID_LENGTH;

    uint8_t expected_transport_data[] = {BLE_RPC_PKT_RESP,                                       // packet type
                                         expected_transport_op_code,                             // op code
                                         (expected_transport_err_code & 0x000000ffu) >> 0,       // error code
                                         (expected_transport_err_code & 0x0000ff00u) >> 8,       // error code
                                         (expected_transport_err_code & 0x00ff0000u) >> 16,      // error code
                                         (expected_transport_err_code & 0xff000000u) >> 24       // error code
                                         }; // device name

    p_transport_write_mock_call->arg[0] = BLE_RPC_PKT_RESP;
    p_transport_write_mock_call->arg[1] = (uint32_t)expected_transport_data;
    p_transport_write_mock_call->arg[2] = sizeof(expected_transport_data);

    uint16_t buffer_length = 90;
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                // Packet type
                         SD_BLE_GAP_DEVICE_NAME_GET,
                         1,                              // Length present
                         0, 0,                           // Placeholder for buffer size
                         1};                             // Device name buffer present
    (void)uint16_encode(buffer_length, &in_data[3]);

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will test if the  RPC_SD_BLE_GAP_APPEARANCE_SET command is decoded properly
 * by the decoder and the command response was written to transport layer.
 */
CU_TEST_START(gap_appearance_set)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_appearance_set", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected SoftDevice call
    uint16_t exp_appearance = 2;

    p_sd_call_mock_obj->arg[0]    = (uint32_t)exp_appearance;

    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_APPEARANCE_SET,
                                 NRF_SUCCESS,
                                 BLE_RPC_PKT_RESP);

    // Stimuli

    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                       // Packet type
                         SD_BLE_GAP_APPEARANCE_SET,
                         0x02, 0x00};                           // Appearance

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END
// This test case send a pkt where the actual pkt length is less than no of bytes sent for gap_appearance_set command
CU_TEST_START(gap_appearance_set_sanchk_cmdlen_lt_actual_pkt)
{
    // Preparation of expected output
//    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

//    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_device_name_set", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_APPEARANCE_SET,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

//    p_sd_call_mock_obj->executed = false;   // This SoftDevice mock is never executed,  hence set to false.

    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                      // Packet type
                         SD_BLE_GAP_APPEARANCE_SET,
                         0x04, 0x00};                          // appearance


        p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
        p_transport_read_mock_call->result[0] = (uint32_t)in_data;
        p_transport_read_mock_call->result[1] = 1; //sizeof(in_data)

        ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END
/* This test case will test if the  RPC_SD_BLE_GAP_PPCP_SET command is decoded properly
 * by the decoder and the command response was written to transport layer.
 */
CU_TEST_START(gap_ppcp_set)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_ppcp_set", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected SoftDevice call
    ble_gap_conn_params_t expected_conn_params;

    expected_conn_params.min_conn_interval  = 0x1234;
    expected_conn_params.max_conn_interval  = 0x5678;
    expected_conn_params.slave_latency      = 0x90ab;
    expected_conn_params.conn_sup_timeout   = 0xcdef;

    p_sd_call_mock_obj->arg[0] = (uint32_t)(&expected_conn_params);

    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_PPCP_SET,
                                 NRF_SUCCESS,
                                 BLE_RPC_PKT_RESP);

    // Stimuli

    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                        // Packet type
                         SD_BLE_GAP_PPCP_SET,
                         RPC_BLE_FIELD_PRESENT,                  // Connection Parameters present.
                          0x34, 0x12,                            // Min conn interval
                          0x78, 0x56,                            // Max conn interval
                          0xab, 0x90,                            // Slave Latency
                          0xef, 0xcd};                           // Supervision timeout.

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will test if the  RPC_SD_BLE_GAP_PPCP_SET command is decoded properly
 * by the decoder when the connection parameters are absent in the input command and the command
 * response was written to transport layer.
 */
CU_TEST_START(gap_ppcp_set_conn_params_absent)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_ppcp_set", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected SoftDevice call
    p_sd_call_mock_obj->arg[0] = (uint32_t)NULL;

    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_PPCP_SET,
                                 NRF_SUCCESS,
                                 BLE_RPC_PKT_RESP);

    // Stimuli

    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                // Packet type
                         SD_BLE_GAP_PPCP_SET,
                         RPC_BLE_FIELD_NOT_PRESENT,      // Connection Parameters present.
                         };

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will test if the RPC_SD_BLE_GAP_PPCP_GET command is decoded properly
 * by the decoder and the command response was written to transport layer.
 */
CU_TEST_START(gap_ppcp_get_successful)
{
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_ppcp_get", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    uint8_t expected_transport_op_code   = SD_BLE_GAP_PPCP_GET;

    uint8_t expected_transport_data[] = {BLE_RPC_PKT_RESP,                                  // packet type
                                         expected_transport_op_code,                        // op code
                                         0x00, 0x00, 0x00, 0x00,                            // error code = NRF_SUCCESS
                                         0x34, 0x12,                                        // Min conn interval
                                         0x78, 0x56,                                        // Max conn interval
                                         0xab, 0x90,                                        // Slave Latency
                                         0xef, 0xcd};                                       // Appearance

    p_transport_write_mock_call->arg[0] = BLE_RPC_PKT_RESP;
    p_transport_write_mock_call->arg[1] = (uint32_t)expected_transport_data;
    p_transport_write_mock_call->arg[2] = sizeof(expected_transport_data);

    ble_gap_conn_params_t conn_params;
    conn_params.min_conn_interval   = 0x1234;
    conn_params.max_conn_interval   = 0x5678;
    conn_params.slave_latency       = 0x90ab;
    conn_params.conn_sup_timeout    = 0xcdef;

    p_sd_call_mock_obj->result[0] = (uint32_t)&conn_params;
    (void)conn_params;

    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                // Packet type
                         SD_BLE_GAP_PPCP_GET,
                         1};                             // Connection parameter result buffer present

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will test if the RPC_SD_BLE_GAP_PPCP_GET command is decoded properly
 * by the decoder and the command response was written to transport layer.
 */
CU_TEST_START(gap_ppcp_get_result_buffer_not_present)
{
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_ppcp_get", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    uint8_t expected_transport_data[] = {BLE_RPC_PKT_RESP,              // packet type
                                         SD_BLE_GAP_PPCP_GET,       // op code
                                         0x10, 00, 00, 00};             // Encoded NRF_ERROR_INVALID_ADDR

    p_transport_write_mock_call->arg[1] = (uint32_t)expected_transport_data;
    p_transport_write_mock_call->arg[2] = sizeof(expected_transport_data);

    p_transport_write_mock_call->arg[0] = (uint32_t)NULL;

    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                // Packet type
                         SD_BLE_GAP_PPCP_GET,
                         0};                             // Connection parameter result buffer not present

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will test if the  RPC_SD_BLE_GAP_ADV_START command is decoded properly
 * by the decoder and the command response was written to transport layer.
 * Adv type: Undirected (i.e p_peer_addr element in ble_gap_adv_params_t struct is NULL)
 * No Whitelist advertisement.
 */
CU_TEST_START(gap_adv_start_undirected_no_white_list)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_adv_start", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected SoftDevice call
    ble_gap_adv_params_t expected_adv_params;

    expected_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
    expected_adv_params.p_peer_addr = NULL;
    expected_adv_params.fp          = BLE_GAP_ADV_FP_FILTER_BOTH;
    expected_adv_params.p_whitelist = NULL;
    expected_adv_params.interval    = 0x1234;
    expected_adv_params.timeout     = 0x5678;

    p_sd_call_mock_obj->arg[0] = (uint32_t)(&expected_adv_params);

    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_ADV_START,
                                 NRF_SUCCESS,
                                 BLE_RPC_PKT_RESP);

    // Stimuli

    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                        // Packet type
                         SD_BLE_GAP_ADV_START,
                         BLE_GAP_ADV_TYPE_ADV_IND,               // Adv type
                         0x00,                                   // Peer Address not present
                         BLE_GAP_ADV_FP_FILTER_BOTH,             // Filter Policy
                         0x00,                                   // Gap Whitelist not present
                         0x34, 0x12,                             // Interval
                         0x78, 0x56};                            // Timeout

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will test if the RPC_SD_BLE_GAP_ADV_START command is decoded properly
 * by the decoder and the command response was written to transport layer.
 * Adv type: directed
 * No Whitelist advertisement.
 */
CU_TEST_START(gap_adv_start_directed_no_white_list)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_adv_start", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected SoftDevice call
    ble_gap_adv_params_t expected_adv_params;
    ble_gap_addr_t       directed_peer_addr;
    uint8_t              sample_ble_add[] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xab};

    expected_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_DIRECT_IND;

    directed_peer_addr.addr_type    = BLE_GAP_ADDR_TYPE_RANDOM_STATIC;
    memcpy(directed_peer_addr.addr, sample_ble_add, sizeof(directed_peer_addr.addr));

    expected_adv_params.p_peer_addr = &directed_peer_addr;
    expected_adv_params.fp          = BLE_GAP_ADV_FP_FILTER_BOTH;
    expected_adv_params.p_whitelist = NULL;
    expected_adv_params.interval    = 0x1234;
    expected_adv_params.timeout     = 0x5678;

    p_sd_call_mock_obj->arg[0] = (uint32_t)(&expected_adv_params);

    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_ADV_START,
                                 NRF_SUCCESS,
                                 BLE_RPC_PKT_RESP);

    // Stimuli

    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                   // Packet type
                         SD_BLE_GAP_ADV_START,
                         BLE_GAP_ADV_TYPE_ADV_DIRECT_IND,   // Adv type
                         0x01,                              // Peer Address Present
                         BLE_GAP_ADDR_TYPE_RANDOM_STATIC,   // Peer addr type (for directed adv)
                         0x12, 0x34, 0x56, 0x78, 0x90, 0xab,// Peer addr (for directed adv)
                         BLE_GAP_ADV_FP_FILTER_BOTH,        // Filter Policy
                         0x00,                              // Whitelist not present
                         0x34, 0x12,                        // Interval
                         0x78, 0x56};                       // Timeout

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will test if the RPC_SD_BLE_GAP_ADV_START command is decoded properly
 * by the decoder and the command response was written to transport layer.
 * Adv type: directed
 * Whitelist advertisement enabled based on Address.
 */
CU_TEST_START(gap_adv_start_directed_white_list_addr)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_adv_start", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected SoftDevice call
    ble_gap_adv_params_t expected_adv_params;
    ble_gap_addr_t       directed_peer_addr;
    uint8_t              sample_ble_add[] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xab};

    expected_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_DIRECT_IND;

    directed_peer_addr.addr_type    = BLE_GAP_ADDR_TYPE_RANDOM_STATIC;
    memcpy(directed_peer_addr.addr, sample_ble_add, sizeof(directed_peer_addr.addr));

    expected_adv_params.p_peer_addr = &directed_peer_addr;
    expected_adv_params.fp          = BLE_GAP_ADV_FP_FILTER_BOTH;

    ble_gap_whitelist_t  white_list;
    const uint8_t        num_address_in_wl = 2;
    uint8_t              exp_encoded_white_list[sizeof(white_list.addr_count) +
                                                sizeof(white_list.irk_count)  +
                                                (num_address_in_wl * (1 + BLE_GAP_ADDR_LEN))];

    expected_white_list_setup(&white_list, num_address_in_wl, 0, exp_encoded_white_list);

    expected_adv_params.p_whitelist = &white_list;

    expected_adv_params.interval    = 0x1234;
    expected_adv_params.timeout     = 0x5678;

    p_sd_call_mock_obj->arg[0] = (uint32_t)(&expected_adv_params);

    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_ADV_START,
                                 NRF_SUCCESS,
                                 BLE_RPC_PKT_RESP);

    // Stimuli

    uint8_t part_inp_data_until_fp[] =
                            {BLE_RPC_PKT_CMD,                  // Packet type
                            SD_BLE_GAP_ADV_START,
                            BLE_GAP_ADV_TYPE_ADV_DIRECT_IND,   // Adv type
                            0x01,                              // Peer addr present
                            BLE_GAP_ADDR_TYPE_RANDOM_STATIC,   // Peer addr type (for directed adv)
                            0x12, 0x34, 0x56, 0x78, 0x90, 0xab,// Peer addr (for directed adv)
                            BLE_GAP_ADV_FP_FILTER_BOTH,        // Filter Policy
                            0x01                               // Whilte list present
                            };
    uint8_t part_inp_data_int_timeout[] =
                            {0x34, 0x12,                       // Interval
                            0x78, 0x56};                       // Timeout

    // Create a buffer big enough.
    uint8_t in_data[sizeof(part_inp_data_until_fp) +
                    sizeof(exp_encoded_white_list) +
                    sizeof(part_inp_data_int_timeout)];

    // Since we already have the expected encoded whitelist, we can encode it separately into the
    // input data as follows.

    // Start with the first part of the input data (until filter policy)
    memcpy(in_data, part_inp_data_until_fp, sizeof(part_inp_data_until_fp));

    // Append the encoded whitelist
    memcpy(in_data + sizeof(part_inp_data_until_fp),
           exp_encoded_white_list,
           sizeof(exp_encoded_white_list));

    // Append the interval and timeout
    memcpy(in_data + sizeof(part_inp_data_until_fp) + sizeof(exp_encoded_white_list),
           part_inp_data_int_timeout,
           sizeof(part_inp_data_int_timeout));

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will test if the RPC_SD_BLE_GAP_ADV_START command is decoded properly
 * by the decoder and the command response was written to transport layer.
 * Adv type: directed
 * Whitelist advertisement enabled based on IRKs.
 */
CU_TEST_START(gap_adv_start_directed_white_list_irk)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_adv_start", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected SoftDevice call
    ble_gap_adv_params_t expected_adv_params;
    ble_gap_addr_t       directed_peer_addr;
    uint8_t              sample_ble_add[] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xab};

    expected_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_DIRECT_IND;

    directed_peer_addr.addr_type    = BLE_GAP_ADDR_TYPE_RANDOM_STATIC;
    memcpy(directed_peer_addr.addr, sample_ble_add, sizeof(directed_peer_addr.addr));

    expected_adv_params.p_peer_addr = &directed_peer_addr;
    expected_adv_params.fp          = BLE_GAP_ADV_FP_FILTER_BOTH;

    ble_gap_whitelist_t  white_list;
    const uint8_t        num_irks_in_wl = 2;
    uint8_t              exp_encoded_white_list[sizeof(white_list.addr_count) +
                                                sizeof(white_list.irk_count)  +
                                                (num_irks_in_wl * BLE_GAP_SEC_KEY_LEN)];

    expected_white_list_setup(&white_list, 0, num_irks_in_wl, exp_encoded_white_list);

    expected_adv_params.p_whitelist = &white_list;

    expected_adv_params.interval    = 0x1234;
    expected_adv_params.timeout     = 0x5678;

    p_sd_call_mock_obj->arg[0] = (uint32_t)(&expected_adv_params);

    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_ADV_START,
                                 NRF_SUCCESS,
                                 BLE_RPC_PKT_RESP);

    // Stimuli

    uint8_t part_inp_data_until_fp[] =
                            {
                             BLE_RPC_PKT_CMD,                   // Packet type
                             SD_BLE_GAP_ADV_START,
                             0x01,                              // Peer Address present.
                             BLE_GAP_ADV_TYPE_ADV_DIRECT_IND,   // Adv type
                             BLE_GAP_ADDR_TYPE_RANDOM_STATIC,   // Peer addr type (for directed adv)
                             0x12, 0x34, 0x56, 0x78, 0x90, 0xab,// Peer addr (for directed adv)
                             BLE_GAP_ADV_FP_FILTER_BOTH,        // Filter Policy
                             0x01                               // White list present.
                            };
    uint8_t part_inp_data_int_timeout[] =
                            {
                             0x34, 0x12,                        // Interval
                             0x78, 0x56                         // Timeout
                            };

    // Create a buffer big enough.
    uint8_t in_data[sizeof(part_inp_data_until_fp) +
                    sizeof(exp_encoded_white_list) +
                    sizeof(part_inp_data_int_timeout)];

    // Since we already have the expected encoded whitelist, we can encode it separately into the
    // input data as follows.

    // Start with the first part of the input data (until filter policy)
    memcpy(in_data, part_inp_data_until_fp, sizeof(part_inp_data_until_fp));

    // Append the encoded whitelist
    memcpy(in_data + sizeof(part_inp_data_until_fp),
           exp_encoded_white_list,
           sizeof(exp_encoded_white_list));

    // Append the interval and timeout
    memcpy(in_data + sizeof(part_inp_data_until_fp) + sizeof(exp_encoded_white_list),
           part_inp_data_int_timeout,
           sizeof(part_inp_data_int_timeout));

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will test if the RPC_SD_BLE_GAP_ADV_START command is decoded properly
 * by the decoder and the command response was written to transport layer.
 * Adv type: directed
 * Whitelist advertisement enabled based on 2 IRKs and 1 Address.
 */
CU_TEST_START(gap_adv_start_directed_white_list_addr_and_irk)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_adv_start", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected SoftDevice call
    ble_gap_adv_params_t expected_adv_params;
    ble_gap_addr_t       directed_peer_addr;
    uint8_t              sample_ble_add[] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xab};

    expected_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_DIRECT_IND;

    directed_peer_addr.addr_type    = BLE_GAP_ADDR_TYPE_RANDOM_STATIC;
    memcpy(directed_peer_addr.addr, sample_ble_add, sizeof(directed_peer_addr.addr));

    expected_adv_params.p_peer_addr = &directed_peer_addr;
    expected_adv_params.fp          = BLE_GAP_ADV_FP_FILTER_BOTH;

    ble_gap_whitelist_t  white_list;
    const uint8_t        num_address_in_wl  = 1;
    const uint8_t        num_irks_in_wl     = 2;
    uint8_t              exp_encoded_white_list[sizeof(white_list.addr_count)           +
                                                sizeof(white_list.irk_count)            +
                                                (num_irks_in_wl * BLE_GAP_SEC_KEY_LEN)  +
                                                (num_address_in_wl * (1 + BLE_GAP_ADDR_LEN))];

    expected_white_list_setup(&white_list, num_address_in_wl, num_irks_in_wl, exp_encoded_white_list);

    expected_adv_params.p_whitelist = &white_list;

    expected_adv_params.interval    = 0x1234;
    expected_adv_params.timeout     = 0x5678;

    p_sd_call_mock_obj->arg[0] = (uint32_t)(&expected_adv_params);

    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_ADV_START,
                                 NRF_SUCCESS,
                                 BLE_RPC_PKT_RESP);

    // Stimuli

    uint8_t part_inp_data_until_fp[] =
                            {
                             BLE_RPC_PKT_CMD,                   // Packet type
                             SD_BLE_GAP_ADV_START,
                             0x01,                              // Peer Address present.
                             BLE_GAP_ADV_TYPE_ADV_DIRECT_IND,   // Adv type
                             BLE_GAP_ADDR_TYPE_RANDOM_STATIC,   // Peer addr type (for directed adv)
                             0x12, 0x34, 0x56, 0x78, 0x90, 0xab,// Peer addr (for directed adv)
                             BLE_GAP_ADV_FP_FILTER_BOTH,        // Filter Policy
                             0x01                               // White list present
                            };
    uint8_t part_inp_data_int_timeout[] =
                            {
                             0x34, 0x12,                       // Interval
                             0x78, 0x56                        // Timeout
                            };

    // Create a buffer big enough.
    uint8_t in_data[sizeof(part_inp_data_until_fp) +
                    sizeof(exp_encoded_white_list) +
                    sizeof(part_inp_data_int_timeout)];

    // Since we already have the expected encoded whitelist, we can encode it separately into the
    // input data as follows.

    // Start with the first part of the input data (until filter policy)
    memcpy(in_data, part_inp_data_until_fp, sizeof(part_inp_data_until_fp));

    // Append the encoded whitelist
    memcpy(in_data + sizeof(part_inp_data_until_fp),
           exp_encoded_white_list,
           sizeof(exp_encoded_white_list));

    // Append the interval and timeout
    memcpy(in_data + sizeof(part_inp_data_until_fp) + sizeof(exp_encoded_white_list),
           part_inp_data_int_timeout,
           sizeof(part_inp_data_int_timeout));

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END



/* This test case will test if the RPC_SD_BLE_GAP_ADV_START command is decoded properly
 * by the decoder and the command response was written to transport layer.
 * Adv type: undirected
 * Whitelist advertisement enabled based on 1 IRK and 2 addresses.
 */
CU_TEST_START(gap_adv_start_undirected_white_list_addr_and_irk)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_adv_start", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected SoftDevice call
    ble_gap_adv_params_t expected_adv_params;
    ble_gap_addr_t       directed_peer_addr;
    uint8_t              sample_ble_add[] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xab};

    expected_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;

    directed_peer_addr.addr_type    = BLE_GAP_ADDR_TYPE_RANDOM_STATIC;
    memcpy(directed_peer_addr.addr, sample_ble_add, sizeof(directed_peer_addr.addr));

    expected_adv_params.p_peer_addr = NULL;
    expected_adv_params.fp          = BLE_GAP_ADV_FP_FILTER_BOTH;

    ble_gap_whitelist_t  white_list;
    const uint8_t        num_address_in_wl  = 2;
    const uint8_t        num_irks_in_wl     = 1;
    uint8_t              exp_encoded_white_list[sizeof(white_list.addr_count)           +
                                                sizeof(white_list.irk_count)            +
                                                (num_irks_in_wl * BLE_GAP_SEC_KEY_LEN)  +
                                                (num_address_in_wl * (1 + BLE_GAP_ADDR_LEN))];

    expected_white_list_setup(&white_list, num_address_in_wl, num_irks_in_wl, exp_encoded_white_list);

    expected_adv_params.p_whitelist = &white_list;

    expected_adv_params.interval    = 0x1234;
    expected_adv_params.timeout     = 0x5678;

    p_sd_call_mock_obj->arg[0] = (uint32_t)(&expected_adv_params);

    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_ADV_START,
                                 NRF_SUCCESS,
                                 BLE_RPC_PKT_RESP);

    // Stimuli

    uint8_t part_inp_data_until_fp[] =
                            {
                             BLE_RPC_PKT_CMD,                   // Packet type
                             SD_BLE_GAP_ADV_START,
                             BLE_GAP_ADV_TYPE_ADV_IND,          // Adv type
                             0x00,                              // Peer address absent
                             BLE_GAP_ADV_FP_FILTER_BOTH,        // Filter Policy
                             0x01                               // White list present
                            };
    uint8_t part_inp_data_int_timeout[] =
                            {
                             0x34, 0x12,                       // Interval
                             0x78, 0x56                        // Timeout
                            };

    // Create a buffer big enough.
    uint8_t in_data[sizeof(part_inp_data_until_fp) +
                    sizeof(exp_encoded_white_list) +
                    sizeof(part_inp_data_int_timeout)];

    // Since we already have the expected encoded whitelist, we can encode it separately into the
    // input data as follows.

    // Start with the first part of the input data (until filter policy)
    memcpy(in_data, part_inp_data_until_fp, sizeof(part_inp_data_until_fp));

    // Append the encoded whitelist
    memcpy(in_data + sizeof(part_inp_data_until_fp),
           exp_encoded_white_list,
           sizeof(exp_encoded_white_list));

    // Append the interval and timeout
    memcpy(in_data + sizeof(part_inp_data_until_fp) + sizeof(exp_encoded_white_list),
           part_inp_data_int_timeout,
           sizeof(part_inp_data_int_timeout));

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END



/* This test case will test if the RPC_SD_BLE_GAP_ADV_DATA_SET command is decoded properly
 * by the decoder and the command response was written to transport layer.
 * Adv data         : Present
 * Scan Resp data   : Present
 */
CU_TEST_START(gap_adv_data_set_adv_data_and_scan_rsp)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_adv_data_set", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected SoftDevice call
    const uint8_t raw_adv_data_len = 31;
    const uint8_t raw_sr_data_len  = 31;

    uint8_t in_data[sizeof(uint8_t)    +  // Packet type
                    sizeof(uint8_t)    +  // For Opcode
                    sizeof(uint8_t)    +  // For Adv data len
                    raw_adv_data_len   +  // For Adv data
                    sizeof(uint8_t)    +  // For SR data len
                    raw_sr_data_len];     // For SR data


    expected_adv_data_setup(p_sd_call_mock_obj, raw_adv_data_len, raw_sr_data_len, in_data);

    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_ADV_DATA_SET,
                                 NRF_SUCCESS,
                                 BLE_RPC_PKT_RESP);

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END



/* This test case will test if the RPC_SD_BLE_GAP_ADV_DATA_SET command is decoded properly
 * by the decoder and the command response was written to transport layer.
 * Adv data         : Not present
 * Scan Resp data   : Present
 */
CU_TEST_START(gap_adv_data_set_scan_rsp_data_only)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_adv_data_set", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    // Expected SoftDevice call
    const uint8_t raw_adv_data_len = 0;
    const uint8_t raw_sr_data_len  = 15;

    uint8_t in_data[sizeof(uint8_t)    +  // Packet type
                    sizeof(uint8_t)    +  // For Opcode
                    sizeof(uint8_t)    +  // For Adv data len
                    raw_adv_data_len   +  // For Adv data
                    sizeof(uint8_t)    +  // For SR data len
                    raw_sr_data_len];     // For SR data


    expected_adv_data_setup(p_sd_call_mock_obj, raw_adv_data_len, raw_sr_data_len, in_data);

    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_ADV_DATA_SET,
                                 NRF_SUCCESS,
                                 BLE_RPC_PKT_RESP);

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will test if the RPC_SD_BLE_GAP_ADV_DATA_SET command is decoded properly
 * by the decoder and the command response was written to transport layer.
 * Adv data         : Present
 * Scan Resp data   : Not Present
 */
CU_TEST_START(gap_adv_data_set_adv_data_only)
{
    // Preparation of expected output
     nrf_cunit_mock_call * p_sd_call_mock_obj;
     nrf_cunit_mock_call * p_transport_read_mock_call;
     nrf_cunit_mock_call * p_transport_write_mock_call;

     nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_adv_data_set", &p_sd_call_mock_obj);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     // Expected SoftDevice call
     const uint8_t raw_adv_data_len = 31;
     const uint8_t raw_sr_data_len  = 0;

     uint8_t in_data[sizeof(uint8_t)    +  // Packet type
                     sizeof(uint8_t)    +  // For Opcode
                     sizeof(uint8_t)    +  // For Adv data len
                     raw_adv_data_len   +  // For Adv data
                     sizeof(uint8_t)    +  // For SR data len
                     raw_sr_data_len];     // For SR data


     expected_adv_data_setup(p_sd_call_mock_obj, raw_adv_data_len, raw_sr_data_len, in_data);

     // Expected Transport Layer API call
     expected_transport_api_setup(p_transport_write_mock_call,
                                  SD_BLE_GAP_ADV_DATA_SET,
                                  NRF_SUCCESS,
                                  BLE_RPC_PKT_RESP);

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = sizeof(in_data);

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* @todo Describe.
 * conn_handle   = 0x04 (Just a number for test)
 * sec_status    = 0
 * p_sec_params  = NULL
 */
CU_TEST_START(gap_sec_params_reply_null)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_sd_mock_call;
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns.
     expected_data.op_code       = SD_BLE_GAP_SEC_PARAMS_REPLY;
     expected_data.err_code      = NRF_SUCCESS;
     expected_data.ext_data_len  = 0;

     // Expected data to the SoftDevice, those data should be decoded from the received in_data and passed to SoftDevice as parameters.
     uint16_t                   expected_handle = 0x04;
     uint8_t                    expected_sec_status = 0;
     ble_gap_sec_params_t *     expected_p_sec_params = NULL;

     nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_sec_params_reply", &p_sd_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     p_sd_mock_call->arg[0]    = expected_handle; // Expected Connection handle.
     p_sd_mock_call->arg[1]    = expected_sec_status;
     p_sd_mock_call->arg[2]    = (uint32_t) expected_p_sec_params;

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call,
                                       &expected_data,
                                       BLE_RPC_PKT_RESP);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,                        // Packet type
                          SD_BLE_GAP_SEC_PARAMS_REPLY,
                          0x04, 0x00, 0x00, 0x00};


     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = sizeof(in_data);

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test will verify that a BLE_ERROR_INVALID_CONN_HANDLE can be correctly encoded and
 * transmitted back to the caller through the transport layer.
 *
 * conn_handle   = 0x44 0x44(Just a number for test)
 * sec_status    = 0
 * p_sec_params  = NULL
 */
CU_TEST_START(gap_sec_params_reply_invalid_conn_handle)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_sd_mock_call;
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns.
     expected_data.op_code       = SD_BLE_GAP_SEC_PARAMS_REPLY;
     expected_data.err_code      = BLE_ERROR_INVALID_CONN_HANDLE;
     expected_data.ext_data_len  = 0;

     // Expected data to the SoftDevice, those data should be decoded from the received in_data and passed to SoftDevice as parameters.
     uint16_t                   expected_handle = 0x4444;
     uint8_t                    expected_sec_status = 0;
     ble_gap_sec_params_t *     expected_p_sec_params = NULL;

     nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_sec_params_reply", &p_sd_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     p_sd_mock_call->arg[0]    = expected_handle; // Expected Connection handle.
     p_sd_mock_call->arg[1]    = expected_sec_status;
     p_sd_mock_call->arg[2]    = (uint32_t) expected_p_sec_params;
     p_sd_mock_call->error     = BLE_ERROR_INVALID_CONN_HANDLE;

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call,
                                       &expected_data,
                                       BLE_RPC_PKT_RESP);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,                        // Packet type
                          SD_BLE_GAP_SEC_PARAMS_REPLY,
                          0x44, 0x44, 0x00, 0x00};


     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = sizeof(in_data);

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* @todo Describe.
 * conn_handle   = 0x04 (Just a number for test)
 * sec_status    = 0
 * p_sec_params  = NULL
 */
CU_TEST_START(gap_sec_params_reply)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_sd_mock_call;
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns.
     expected_data.op_code       = SD_BLE_GAP_SEC_PARAMS_REPLY;
     expected_data.err_code      = NRF_SUCCESS;
     expected_data.ext_data_len  = 0;

     // Expected data to the SoftDevice, those data should be decoded from the received in_data and passed to SoftDevice as parameters.
     uint16_t                   expected_handle = 0x04;
     uint8_t                    expected_sec_status = BLE_GAP_SEC_STATUS_AUTH_REQ;
     ble_gap_sec_params_t       expected_sec_params;

     expected_sec_params.timeout      = 180;
     expected_sec_params.bond         = 1;
     expected_sec_params.mitm         = 1;
     expected_sec_params.io_caps      = BLE_GAP_IO_CAPS_KEYBOARD_ONLY;
     expected_sec_params.oob          = 1;
     expected_sec_params.min_key_size = 15;
     expected_sec_params.max_key_size = 16;

     nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_sec_params_reply", &p_sd_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     p_sd_mock_call->arg[0]    = expected_handle; // Expected Connection handle.
     p_sd_mock_call->arg[1]    = expected_sec_status;
     p_sd_mock_call->arg[2]    = (uint32_t) &expected_sec_params;

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call,
                                       &expected_data,
                                       BLE_RPC_PKT_RESP);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,                        // Packet type
                          SD_BLE_GAP_SEC_PARAMS_REPLY,
                          0x04, 0x00, 0x83, 0x01, 0xB4, 0x00, 0x2B, 0x0F, 0x10};


     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = sizeof(in_data);

     ble_rpc_cmd_handle(NULL, 0);

     (void)expected_sec_params;
}
CU_TEST_END


/* This test case will test if the RPC_SD_BLE_GAP_CONN_PARAM_UPDATE command is decoded properly
 * by the decoder and the command response was written to transport layer.
 * conn_handle      = 0x1010 (Just a number for test)
 * p_conn_params    = NULL
 */
CU_TEST_START(gap_conn_param_update_null)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_sd_mock_call;
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns.
     expected_data.op_code       = SD_BLE_GAP_CONN_PARAM_UPDATE;
     expected_data.err_code      = NRF_ERROR_INVALID_ADDR;
     expected_data.ext_data_len  = 0;

     nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_conn_param_update", &p_sd_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     p_sd_mock_call->arg[0]    = 0x1010; // Expected Connection handle.
     p_sd_mock_call->arg[1]    = (uint32_t)NULL;
     p_sd_mock_call->error       = NRF_ERROR_INVALID_ADDR;

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call,
                                       &expected_data,
                                       BLE_RPC_PKT_RESP);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,                        // Packet type
                          SD_BLE_GAP_CONN_PARAM_UPDATE,
                          0x10, 0x10, 0x00};


     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = sizeof(in_data);

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/* This test case will test if the RPC_SD_BLE_GAP_CONN_PARAM_UPDATE command is decoded properly
 * by the decoder and the command response was written to transport layer.
 * conn_handle      = 0x10 (Just a number for test)
 * p_conn_params    = Connection Parameters
 */
CU_TEST_START(gap_conn_param_update_params)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_sd_mock_call;
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns.
     expected_data.op_code       = SD_BLE_GAP_CONN_PARAM_UPDATE;
     expected_data.err_code      = NRF_SUCCESS;
     expected_data.ext_data_len  = 0;

     nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_conn_param_update", &p_sd_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     ble_gap_conn_params_t conn_params;
     conn_params.min_conn_interval  = 0x0006;
     conn_params.max_conn_interval  = 0x0C80;
     conn_params.slave_latency      = 0x0234;
     conn_params.conn_sup_timeout   = 0xFFFF;

     p_sd_mock_call->arg[0]    = 0x10; // Expected Connection handle.
     p_sd_mock_call->arg[1]    = (uint32_t)&conn_params;

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call,
                                       &expected_data,
                                       BLE_RPC_PKT_RESP);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,                        // Packet type
                          SD_BLE_GAP_CONN_PARAM_UPDATE,
                          0x10, 0x00, 0x01, 0x06, 0x00, 0x80,
                          0x0C, 0x34, 0x02, 0xFF, 0xFF};

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = sizeof(in_data);

     ble_rpc_cmd_handle(NULL, 0);

     (void) conn_params;
}
CU_TEST_END


/* @todo Describe.
 */
CU_TEST_START(gap_disconnect)
{
    //                       ----- Mock setup ------
    // Will verify calls need to ensure decoding sd_ble_gatts_service_add function.
    // Preparation of expected output
     nrf_cunit_mock_call *      p_sd_mock_call;
     nrf_cunit_mock_call *      p_transport_read_mock_call;
     nrf_cunit_mock_call *      p_transport_write_mock_call;
     expected_transport_data_t  expected_data;

     // Expected data to the Transport Layer, when the SoftDevice call returns.
     expected_data.op_code       = SD_BLE_GAP_DISCONNECT;
     expected_data.err_code      = NRF_SUCCESS;
     expected_data.ext_data_len  = 0;

     nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_disconnect", &p_sd_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
     nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

     p_sd_mock_call->arg[0]    = 0x08; // Expected Connection handle.
     p_sd_mock_call->arg[1]    = BLE_HCI_CONN_INTERVAL_UNACCEPTABLE;

     // Expected Transport Layer API call
     expected_transport_api_data_setup(p_transport_write_mock_call,
                                       &expected_data,
                                       BLE_RPC_PKT_RESP);


     // ----- Execute the command to be tested. -----
     uint8_t in_data[] = {BLE_RPC_PKT_CMD,                        // Packet type
                          SD_BLE_GAP_DISCONNECT,
                          0x08, 0x00, 0x3B};

     p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
     p_transport_read_mock_call->result[0] = (uint32_t)in_data;
     p_transport_read_mock_call->result[1] = sizeof(in_data);

     ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_appearance_get_successful)
{
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    
    uint8_t expected_transport_data[] = {BLE_RPC_PKT_RESP,                       // packet type
                                         SD_BLE_GAP_APPEARANCE_GET,          // op code
                                         //lint --e(572) "Excessive shift value"
                                         (NRF_SUCCESS & 0x000000ffu) >> 0,       // error code
                                         //lint --e(572) "Excessive shift value"
                                         (NRF_SUCCESS & 0x0000ff00u) >> 8,       // error code
                                         //lint --e(572) "Excessive shift value"
                                         (NRF_SUCCESS & 0x00ff0000u) >> 16,      // error code
                                         //lint --e(572) "Excessive shift value"
                                         (NRF_SUCCESS & 0xff000000u) >> 24,      // error code
                                         0xAA, 0xBB};                            // Appearance

    // Preparation of expected mock.
    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_appearance_get", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);
    
    p_transport_write_mock_call->arg[0] = BLE_RPC_PKT_RESP;
    p_transport_write_mock_call->arg[1] = (uint32_t)expected_transport_data;
    p_transport_write_mock_call->arg[2] = sizeof(expected_transport_data);                                         


    p_sd_call_mock_obj->compare_rule[0] = COMPARE_NOT_NULL;
    p_sd_call_mock_obj->result[0] = 0xBBAA;
    
 
    // Test Data 
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                        // Packet type
                         SD_BLE_GAP_APPEARANCE_GET,
                         1};        // Should be same as mock data sent in SD Call
    
    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    // ----- Execute the command to be tested. -----
    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END

/**
 * This test will verify the decoding of the SoftDevice call: sd_ble_gap_sec_info_reply(...) when 
 * all optional fields are omitted.
 *
 * The expected decoded packet is:
 *     sd_ble_gap_sec_info_reply(...)
 */
CU_TEST_START(gap_sec_info_reply_optionals_not_present)
{
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;
    
    expected_transport_data_t expected_transport_data;
    
    expected_transport_data.op_code              = SD_BLE_GAP_SEC_INFO_REPLY;
    expected_transport_data.err_code             = NRF_SUCCESS;    
    expected_transport_data.ext_data_len         = 0;     
    
    // Preparation of expected mock.
    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_sec_info_reply", &p_sd_call_mock_obj);        
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    uint16_t conn_handle = 0x55AAu;
    p_sd_call_mock_obj->arg[0] = (uint32_t)&conn_handle;    
    p_sd_call_mock_obj->arg[1] = 0;    
    p_sd_call_mock_obj->arg[2] = 0;       

    // Expected Transport Layer API call
    expected_transport_api_data_setup(p_transport_write_mock_call,
                                      &expected_transport_data,
                                      BLE_RPC_PKT_RESP);
        
    // Test Data - should be same as mock data sent in SD Call. 
    uint8_t in_data[] = 
    {
        BLE_RPC_PKT_CMD,                        // Packet type
        SD_BLE_GAP_SEC_INFO_REPLY,
        LSB(conn_handle),
        MSB(conn_handle),
        0,
        0
    };    
    
    // ----- Execute the command to be tested. -----
    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);
    
}
CU_TEST_END


/**
 * This test will verify the decoding of the SoftDevice call: sd_ble_gap_sec_info_reply(...) when 
 * all optional fields are present.
 *
 * The expected decoded packet is:
 *     sd_ble_gap_sec_info_reply(...)
 */
CU_TEST_START(gap_sec_info_reply_optionals_present)
{
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;
    
    expected_transport_data_t expected_transport_data;
    
    expected_transport_data.op_code      = SD_BLE_GAP_SEC_INFO_REPLY;
    expected_transport_data.err_code     = NRF_SUCCESS;    
    expected_transport_data.ext_data_len = 0;     
    
    // Preparation of expected mock.
    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_sec_info_reply", &p_sd_call_mock_obj);        
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    uint16_t            conn_handle = 0x55AAu;    
    ble_gap_enc_info_t  enc_info;
    enc_info.div                    = 0xAA55;
    enc_info.auth                   = 1;
    enc_info.ltk_len                = 0x10;

    unsigned int i;

    for (i = 0; i < sizeof(enc_info.ltk); i++)
    {
        enc_info.ltk[i] = i;
    }
    ble_gap_sign_info_t sign_info;

    for (i = 0; i < sizeof(sign_info.csrk); i++)
    {
        sign_info.csrk[i] = i;
    }
    p_sd_call_mock_obj->arg[0] = (uint32_t)&conn_handle;        
    p_sd_call_mock_obj->arg[1] = (uint32_t)&enc_info;    
    p_sd_call_mock_obj->arg[2] = (uint32_t)&sign_info;           

    // Expected Transport Layer API call
    expected_transport_api_data_setup(p_transport_write_mock_call,
                                      &expected_transport_data,
                                      BLE_RPC_PKT_RESP);
        
    // Test Data - should be same as mock data sent in SD Call. 
    uint8_t in_data[] = 
    {
        BLE_RPC_PKT_CMD,                        // Packet type
        SD_BLE_GAP_SEC_INFO_REPLY,
        LSB(conn_handle),
        MSB(conn_handle),
        1,
        LSB(enc_info.div),
        MSB(enc_info.div),
        enc_info.ltk[0], enc_info.ltk[1], enc_info.ltk[2], enc_info.ltk[3], enc_info.ltk[4], 
        enc_info.ltk[5], enc_info.ltk[6], enc_info.ltk[7], enc_info.ltk[8], enc_info.ltk[9], 
        enc_info.ltk[10], enc_info.ltk[11], enc_info.ltk[12], enc_info.ltk[13], enc_info.ltk[14],         
        enc_info.ltk[15],
        0x21,                                            // Auth Key and LTK Length
        1,
        sign_info.csrk[0], sign_info.csrk[1], sign_info.csrk[2], sign_info.csrk[3], 
        sign_info.csrk[4], sign_info.csrk[5], sign_info.csrk[6], sign_info.csrk[7], 
        sign_info.csrk[8], sign_info.csrk[9], sign_info.csrk[10], sign_info.csrk[11], 
        sign_info.csrk[12], sign_info.csrk[13], sign_info.csrk[14], sign_info.csrk[15],        
    };    
    
    // ----- Execute the command to be tested. -----
    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/**
 * This test will verify the decoding of the SoftDevice call: sd_ble_gap_sec_info_reply(...) when 
 * optional encryption information field is omitted.
 *
 * The expected decoded packet is:
 *     sd_ble_gap_sec_info_reply(...)
 */
CU_TEST_START(gap_sec_info_reply_encrypt_not_present)
{
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;
    
    expected_transport_data_t expected_transport_data;
    
    expected_transport_data.op_code      = SD_BLE_GAP_SEC_INFO_REPLY;
    expected_transport_data.err_code     = NRF_SUCCESS;    
    expected_transport_data.ext_data_len = 0;     
    
    // Preparation of expected mock.
    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_sec_info_reply", &p_sd_call_mock_obj);        
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    uint16_t            conn_handle = 0x55AAu;    
    ble_gap_sign_info_t sign_info;
    unsigned int i;

    for (i = 0; i < sizeof(sign_info.csrk); i++)
    {
        sign_info.csrk[i] = i;
    }

    p_sd_call_mock_obj->arg[0] = (uint32_t)&conn_handle;        
    p_sd_call_mock_obj->arg[1] = 0;    
    p_sd_call_mock_obj->arg[2] = (uint32_t)&sign_info;           

    // Expected Transport Layer API call
    expected_transport_api_data_setup(p_transport_write_mock_call,
                                      &expected_transport_data,
                                      BLE_RPC_PKT_RESP);
        
    // Test Data - should be same as mock data sent in SD Call. 
    uint8_t in_data[] = 
    {
        BLE_RPC_PKT_CMD,                        // Packet type
        SD_BLE_GAP_SEC_INFO_REPLY,
        LSB(conn_handle),
        MSB(conn_handle),
        0,
        1,
        sign_info.csrk[0], sign_info.csrk[1], sign_info.csrk[2], sign_info.csrk[3], 
        sign_info.csrk[4], sign_info.csrk[5], sign_info.csrk[6], sign_info.csrk[7], 
        sign_info.csrk[8], sign_info.csrk[9], sign_info.csrk[10], sign_info.csrk[11], 
        sign_info.csrk[12], sign_info.csrk[13], sign_info.csrk[14], sign_info.csrk[15],        
    };    
    
    // ----- Execute the command to be tested. -----
    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


/**
 * This test will verify the decoding of the SoftDevice call: sd_ble_gap_sec_info_reply(...) when 
 * optional signing information field is omitted.
 *
 * The expected decoded packet is:
 *     sd_ble_gap_sec_info_reply(...)
 */
CU_TEST_START(gap_sec_info_reply_signing_not_present)
{
    nrf_cunit_mock_call * p_sd_call_mock_obj;
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;
    
    expected_transport_data_t expected_transport_data;
    
    expected_transport_data.op_code      = SD_BLE_GAP_SEC_INFO_REPLY;
    expected_transport_data.err_code     = NRF_SUCCESS;    
    expected_transport_data.ext_data_len = 0;     
    
    // Preparation of expected mock.
    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_sec_info_reply", &p_sd_call_mock_obj);        
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    uint16_t            conn_handle = 0x55AAu;    
    ble_gap_enc_info_t  enc_info;
    enc_info.div                    = 0xAA55;
    enc_info.auth                   = 1;
    enc_info.ltk_len                = 0x10;
    p_sd_call_mock_obj->arg[0] = (uint32_t)&conn_handle;        
    p_sd_call_mock_obj->arg[1] = (uint32_t)&enc_info;    
    p_sd_call_mock_obj->arg[2] = 0;           

    // Expected Transport Layer API call
    expected_transport_api_data_setup(p_transport_write_mock_call,
                                      &expected_transport_data,
                                      BLE_RPC_PKT_RESP);
        
    // Test Data - should be same as mock data sent in SD Call. 
    uint8_t in_data[] = 
    {
        BLE_RPC_PKT_CMD,                        // Packet type
        SD_BLE_GAP_SEC_INFO_REPLY,
        LSB(conn_handle),
        MSB(conn_handle),
        1,
        LSB(enc_info.div),
        MSB(enc_info.div),
        enc_info.ltk[0], enc_info.ltk[1], enc_info.ltk[2], enc_info.ltk[3], enc_info.ltk[4], 
        enc_info.ltk[5], enc_info.ltk[6], enc_info.ltk[7], enc_info.ltk[8], enc_info.ltk[9], 
        enc_info.ltk[10], enc_info.ltk[11], enc_info.ltk[12], enc_info.ltk[13], enc_info.ltk[14],         
        enc_info.ltk[15],
        0x21,
        0
    };    
    
    // ----- Execute the command to be tested. -----
    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_adv_data_set_sanchk_cmdlen_lt_adv_data_len)
{
// preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_ADV_DATA_SET,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                        // Packet type
                         SD_BLE_GAP_ADV_DATA_SET,
                         0x4,               // Adv Data Len
                         0x1, 0x2, 0x3, 04, // Adv Data
                         0};                // Scan Data Len


    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 2; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_adv_data_set_sanchk_cmdlen_lt_adv_data_len_zero)
{
// preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_ADV_DATA_SET,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                        // Packet type
                         SD_BLE_GAP_ADV_DATA_SET,
                         0x0,               // Adv Data Len
                         0x12,              // Scan Data Len
                         0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04, 0x01, 0x02, 0x03, 0x04};


    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 12; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_adv_data_set_sanchk_cmdlen_both_len_zero)
{
    // preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_ADV_DATA_SET,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                        // Packet type
                         SD_BLE_GAP_ADV_DATA_SET,
                         0x0,               // Adv Data Len
                         0x0                // Scan Data Len
    };


    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 2; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_adv_data_set_sanchk_cmdlen_lt_scan_data_len)
{

    // preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_ADV_DATA_SET,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    //stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                        // Packet type
                         SD_BLE_GAP_ADV_DATA_SET,
                         0x2,               // Adv Data Len
                         0x1, 0x2,          // Adv Data
                         0x3,               // Scan Data Len
                         0xF, 0xD, 0xE};


    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 5;      // 5 < sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_adv_start_only_type)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_ADV_START,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                        // Packet type
                         SD_BLE_GAP_ADV_START,
                         0x1, 0x2, 0x03, 0x04, 0x05, 0x06, 0x07, 0x8};


    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 1; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_adv_data_start_sanchk_cmdlen_lt_peer_addr)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_ADV_START,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

        // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                           // Packet type
                         SD_BLE_GAP_ADV_START,
                         0x1,                                       // Type
                         0x1,                                       // Peer Address _present
                         0x2, 0x03, 0x04, 0x05, 0x06, 0x07, 0x8,    //Peer Address
                         0x90};                                     // Filter Policy

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 9; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_adv_data_start_sanchk_cmd_len_lt_fp)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_ADV_START,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

        // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                           // Packet type
                         SD_BLE_GAP_ADV_START,
                         0x1,                                       // Type
                         0x1,                                       // Peer Address _present
                         0x2, 0x03, 0x04, 0x05, 0x06, 0x07, 0x8,    //Peer Address
                         0x90};                                     // Filter Policy

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 10; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_adv_data_start_sanchk_cmd_len_lt_gap_whitelist_present)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_ADV_START,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                           // Packet type
                         SD_BLE_GAP_ADV_START,
                         0x1,                                       // Type
                         0x1,                                       // Peer Address _present
                         0x2, 0x03, 0x04, 0x05, 0x06, 0x07, 0x80,   //Peer Address
                         0x3,                                       // Filter Policy
                         0x01};                                     // Gap whitelist present

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 12; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_adv_data_start_sanchk_cmd_len_lt_gap_whitelist_count_mismatch)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_ADV_START,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                           // Packet type
                         SD_BLE_GAP_ADV_START,
                         0x1,                                       // Type
                         0x1,                                       // Peer Address _present
                         0x2, 0x03, 0x04, 0x05, 0x06, 0x07, 0x80,   //Peer Address
                         0x3,                                       // Filter Policy
                         0x01,                                      // Gap whitelist present
                         0x03,                                      // whitelist count
                         0xD, 0x0E, 0x09,                           // whitelist address
                         0x00,                                      // irk count
                         0x1, 06, 0x5};

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 15; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_adv_data_start_sanchk_cmd_len_lt_gap_irk_count_mismatch)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_ADV_START,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                           // Packet type
                         SD_BLE_GAP_ADV_START,
                         0x1,                                       // Type
                         0x1,                                       // Peer Address _present
                         0x2, 0x03, 0x04, 0x05, 0x06, 0x07, 0x80,   //Peer Address
                         0x3,                                       // Filter Policy
                         0x01,                                      // Gap whitelist present
                         0x01,                                      // whitelist count
                         0xD, 0x0E, 0x09, 0x1, 0x2, 0x3, 0x4,       //whitelist address
                         0x01,                                      // irk count
                         0x1, 06, 0x5, 07, 0x1, 0x2, 0x3, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x12, 0x18};

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 19; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_adv_data_start_sanchk_cmd_len_lt_interval)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_ADV_START,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                           // Packet type
                         SD_BLE_GAP_ADV_START,
                         0x1,                                       // Type
                         0x1,                                       // Peer Address _present
                         0x2, 0x03, 0x04, 0x05, 0x06, 0x07, 0x80,   //Peer Address
                         0x3,                                       // Filter Policy
                         0x01,                                      // Gap whitelist present
                         0x01,                                      // whitelist count
                         0x21, 0x22, 0x23, 0x21, 0x22, 0x23, 0x21,  //whitelist address
                         0x01,                                      // irk count
                         0x21, 0x22, 0x23, 0x21, 0x22, 0x23, 0x21, 0x22, 0x21, 0x22, 0x23, 0x21, 0x22, 0x23, 0x21, 0x22,    // irk address
                         0x01, 0x02};                               // interval

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 38; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_adv_data_start_sanchk_cmd_len_lt_timeout)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_ADV_START,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                           // Packet type
                         SD_BLE_GAP_ADV_START,
                         0x1,                                       // Type
                         0x1,                                       // Peer Address _present
                         0x2, 0x03, 0x04, 0x05, 0x06, 0x07, 0x80,   //Peer Address
                         0x3,                                       // Filter Policy
                         0x01,                                      // Gap whitelist present
                         0x01,                                      // whitelist count
                         0x21, 0x22, 0x23, 0x21, 0x22, 0x23, 0x21,  //whitelist address
                         0x01,                                      // irk count
                         0x21, 0x22, 0x23, 0x21, 0x22, 0x23, 0x21, 0x22, 0x21, 0x22, 0x23, 0x21, 0x22, 0x23, 0x21, 0x22,    // irk address
                         0x01, 0x02,                                // interval
                         0x67, 0x45};                               // timeout

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 38; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_conn_param_update_sanchk_cmdlen_lt_chandle)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_CONN_PARAM_UPDATE,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                        // Packet type
                         SD_BLE_GAP_CONN_PARAM_UPDATE,
                         01,02};    // handle

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 2; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_conn_param_update_sanchk_cmdlen_lt_field_present)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_CONN_PARAM_UPDATE,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                   // Packet type
                         SD_BLE_GAP_CONN_PARAM_UPDATE,
                         01,02,                             // handle
                         0x01};                             // field present

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 3; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_conn_param_update_sanchk_cmdlen_lt_conn_param)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_CONN_PARAM_UPDATE,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                   // Packet type
                         SD_BLE_GAP_CONN_PARAM_UPDATE,
                         01,02,                             // handle
                         0x1,                               // field present
                         0x01, 0x02, 0x03, 0x04, 0x5, 0x6, 0x7, 0x8};

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 5; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_disconnect_sanchk_cmdlen_lt_handle)
{

    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_DISCONNECT,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                        // Packet type
                         SD_BLE_GAP_DISCONNECT,
                         0x1, 0x03};        // handle

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 2; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_disconnect_sanchk_cmdlen_lt_hcicode)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_DISCONNECT,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                        // Packet type
                         SD_BLE_GAP_DISCONNECT,
                         0x1, 0x03,     // handle
                         0x16};         // HCI Code

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 3; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_ppcp_set_sanchk_cmdlen_lt_connparam)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_PPCP_SET,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                                   // Packet type
                         SD_BLE_GAP_PPCP_SET,
                         0x1, 0x03, 0x02, 0x04, 0x05, 0x07, 0x6, 0x8};      // handle

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 6; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_sec_params_reply_sanchk_cmdlen_lt_chandle)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_SEC_PARAMS_REPLY,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,   // Packet type
                         SD_BLE_GAP_SEC_PARAMS_REPLY,
                         0x1, 0x2,          // handle
                         0x01,              // sec status
                         0x01,              // sec params present
                         0x90, 0x90,        // timeout
                         0x11,              // bit field
                         16,                // Min Key Size
                         16};               // Max Key Size


    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 1; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_sec_params_reply_sanchk_cmdlen_lt_secstatus)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_SEC_PARAMS_REPLY,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,   // Packet type
                         SD_BLE_GAP_SEC_PARAMS_REPLY,
                         0x1, 0x2,          // handle
                         0x01,              // sec status
                         0x01,              // sec params present
                         0x90, 0x90,        // timeout
                         0x11,              // bit field
                         16,                // Min Key Size
                         16};               // Max Key Size


    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 2; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_sec_params_reply_sanchk_cmdlen_lt_secparams)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_SEC_PARAMS_REPLY,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,   // Packet type
                         SD_BLE_GAP_SEC_PARAMS_REPLY,
                         0x1, 0x2,          // handle
                         0x01,              // sec status
                         0x01,              // sec params present
                         0x90, 0x90,        // timeout
                         0x11,              // bit field
                         16,                // Min Key Size
                         16};               // Max Key Size


    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 3; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_sec_params_reply_sanchk_cmdlen_lt_secparams_absent)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_SEC_PARAMS_REPLY,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,   // Packet type
                         SD_BLE_GAP_SEC_PARAMS_REPLY,
                         0x1, 0x2,          // handle
                         0x01,              // sec status
                         0x00,              // sec params present
                         0x90, 0x90,        // timeout
                         0x11,              // bit field
                         16,                // Min Key Size
                         16};               // Max Key Size


    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 3; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_sec_params_reply_sanchk_cmdlen_lt_tmeout)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_SEC_PARAMS_REPLY,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,   // Packet type
                         SD_BLE_GAP_SEC_PARAMS_REPLY,
                         0x1, 0x2,          // handle
                         0x01,              // sec status
                         0x01,              // sec params present
                         0x90, 0x90,        // timeout
                         0x11,              // bit field
                         16,                // Min Key Size
                         16};               // Max Key Size


    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 5; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_sec_params_reply_sanchk_cmdlen_lt_bitfield)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_SEC_PARAMS_REPLY,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,   // Packet type
                         SD_BLE_GAP_SEC_PARAMS_REPLY,
                         0x1, 0x2,          // handle
                         0x01,              // sec status
                         0x01,              // sec params present
                         0x90, 0x90,        // timeout
                         0x11,              // bit field
                         16,                // Min Key Size
                         16};               // Max Key Size


    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 7; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_sec_params_reply_sanchk_cmdlen_lt_minkeysize)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_SEC_PARAMS_REPLY,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,   // Packet type
                         SD_BLE_GAP_SEC_PARAMS_REPLY,
                         0x1, 0x2,          // handle
                         0x01,              // sec status
                         0x01,              // sec params present
                         0x90, 0x90,        // timeout
                         0x11,              // bit field
                         16,                // Min Key Size
                         16};               // Max Key Size


    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 8; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_sec_params_reply_sanchk_cmdlen_lt_maxkeysize)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_SEC_PARAMS_REPLY,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,   // Packet type
                         SD_BLE_GAP_SEC_PARAMS_REPLY,
                         0x1, 0x2,          // handle
                         0x01,              // sec status
                         0x01,              // sec params present
                         0x90, 0x90,        // timeout
                         0x11,              // bit field
                         16,                // Min Key Size
                         16};               // Max Key Size


    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 9; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_sec_info_reply_sanchk_cmdlen_lt_chandle)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_SEC_INFO_REPLY,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,               // Packet type
                         SD_BLE_GAP_SEC_INFO_REPLY,
                        0x01, 0xFF,                     // Connection handle
                        0x1,                            // enc info present
                        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, // enc info
                        0x01,                           // sign info present
                        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};


    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 2; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_sec_info_reply_sanchk_cmdlen_lt_enc_info)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_SEC_INFO_REPLY,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,               // Packet type
                         SD_BLE_GAP_SEC_INFO_REPLY,
                        0x01, 0xFF,                     // Connection handle
                        0x1,                            // enc info present
                        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, // enc info
                        0x01,                           // sign info present
                        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};


    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 10; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_sec_info_reply_sanchk_cmdlen_lt_sign_info)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_SEC_INFO_REPLY,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,               // Packet type
                         SD_BLE_GAP_SEC_INFO_REPLY,
                        0x01, 0xFF,                     // Connection handle
                        0x0,                            // enc info present
                        0x01,                           // sign info present
                        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};


    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 15; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_sec_info_reply_sanchk_cmdlen_lt_enc_sign_info)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_SEC_INFO_REPLY,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,               // Packet type
                         SD_BLE_GAP_SEC_INFO_REPLY,
                        0x01, 0xFF,                     // Connection handle
                        0x1,                            // enc info present
                        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, // enc info
                        0x01,                           // sign info present
                        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};


    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 25; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_sec_info_reply_sanchk_cmdlen_lt_enc_sign_info_absent)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_SEC_INFO_REPLY,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,               // Packet type
                         SD_BLE_GAP_SEC_INFO_REPLY,
                        0x01, 0xFF,                     // Connection handle
                        0x0,                            // enc info present
                        0x0};                           // sign info present



    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 3; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_appearance_get_sanchk_cmdlen_lt_app_get)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_APPEARANCE_GET,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,               // Packet type
                         SD_BLE_GAP_APPEARANCE_GET,
                        0x01};                          // Appearance present



    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 1; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_ppcp_get_sanchk_cmdlen_lt_ppcp)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_PPCP_GET,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                // Packet type
                         SD_BLE_GAP_PPCP_GET,
                         0x01};                          // Appearance present

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 1;

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_device_name_get_sanchk_cmdlen_lt_length)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_DEVICE_NAME_GET,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                // Packet type
                         SD_BLE_GAP_DEVICE_NAME_GET,
                         0x1};                           // Device name present

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 1;

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_device_name_get_sanchk_cmdlen_lt_dname)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_DEVICE_NAME_GET,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                // Packet type
                         SD_BLE_GAP_DEVICE_NAME_GET,
                         0x1,                            // Device name present
                         0x00, 0x05                      // length
    };
    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 1; // lenth present


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_device_name_get_sanchk_cmdlen_lt_dname_2)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_DEVICE_NAME_GET,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                // Packet type
                         SD_BLE_GAP_DEVICE_NAME_GET,
                         0x1,                            // Device name present
                         0x00, 0x05                      // length
    };
    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 2; // lenth present


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_device_name_get_sanchk_cmdlen_lt_dname_buff_present)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_DEVICE_NAME_GET,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,                // Packet type
                         SD_BLE_GAP_DEVICE_NAME_GET,
                         0x1,                            // Device name present
                         0x05, 0x00,                     // length
                         0x01                            // Device buffer present
    };
    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 4;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_device_name_set_sanchk_cmdlen_lt_dev_name_present)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_DEVICE_NAME_SET,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,               // Packet type
                         SD_BLE_GAP_DEVICE_NAME_SET,
                         0x01,                          // Write permission present
                         0x01,
                         0x1, 0x00,                     // Device name length
                         0x01                           // device name present

    };
    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 5;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_device_name_set_sanchk_cmdlen_lt_write_per_present)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_DEVICE_NAME_SET,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,               // Packet type
                         SD_BLE_GAP_DEVICE_NAME_SET,
                         0x01,                          // Write permission present
                        };
    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 1;


    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_ppcp_set_sanchk_cmdlen_lt_connparam_absent)
{
    // Preparation of expected output
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);


    // Expected Transport Layer API call
    expected_transport_api_setup(p_transport_write_mock_call,
                                 SD_BLE_GAP_PPCP_SET,
                                 NRF_ERROR_INVALID_LENGTH,
                                 BLE_RPC_PKT_RESP);

    // Stimuli
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,   // Packet type
                         SD_BLE_GAP_PPCP_SET,
                         0x0};              // conn param absent

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = 1; //sizeof(in_data)

    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


CU_TEST_START(gap_appearance_get_nullptr)
{
    nrf_cunit_mock_call * p_transport_read_mock_call;
    nrf_cunit_mock_call * p_transport_write_mock_call;
    nrf_cunit_mock_call * p_sd_call_mock_obj;

    uint8_t expected_transport_data[] = {BLE_RPC_PKT_RESP,                       // packet type
                                         SD_BLE_GAP_APPEARANCE_GET,          // op code
                                         //lint --e(572) "Excessive shift value"
                                         (NRF_ERROR_INVALID_ADDR & 0x000000ffu) >> 0,       // error code
                                         //lint --e(572) "Excessive shift value"
                                         (NRF_ERROR_INVALID_ADDR & 0x0000ff00u) >> 8,       // error code
                                         //lint --e(572) "Excessive shift value"
                                         (NRF_ERROR_INVALID_ADDR & 0x00ff0000u) >> 16,      // error code
                                         //lint --e(572) "Excessive shift value"
                                         (NRF_ERROR_INVALID_ADDR & 0xff000000u) >> 24,      // error code
                                         };

    // Preparation of expected mock.
    nrf_cunit_expect_call_return((uint8_t *)"sd_ble_gap_appearance_get", &p_sd_call_mock_obj);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_extract", &p_transport_read_mock_call);
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_pkt_write", &p_transport_write_mock_call);

    p_transport_write_mock_call->arg[0] = BLE_RPC_PKT_RESP;
    p_transport_write_mock_call->arg[1] = (uint32_t)expected_transport_data;
    p_transport_write_mock_call->arg[2] = sizeof(expected_transport_data);


    p_sd_call_mock_obj->arg[0]  = (uint32_t)NULL;
    p_sd_call_mock_obj->error   = NRF_ERROR_INVALID_ADDR;


    // Test Data
    uint8_t in_data[] = {BLE_RPC_PKT_CMD,   // Packet type
                         SD_BLE_GAP_APPEARANCE_GET, 0x00};        // Should be same as mock data sent in SD Call

    p_transport_read_mock_call->arg[0]    = BLE_RPC_PKT_CMD;
    p_transport_read_mock_call->result[0] = (uint32_t)in_data;
    p_transport_read_mock_call->result[1] = sizeof(in_data);

    // ----- Execute the command to be tested. -----
    ble_rpc_cmd_handle(NULL, 0);

}
CU_TEST_END


static void gap_setup_func(void)
{
    nrf_cunit_reset_mock();
    nrf_cunit_mock_call * p_transport_mock_call;
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_alloc", &p_transport_mock_call);

    p_transport_mock_call->result[0] = (uint32_t)m_tx_buffer;

    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_rx_pkt_consume", &p_transport_mock_call);
    p_transport_mock_call->compare_rule[0] = COMPARE_ANY;
}

static void gap_teardown_func(void)
{
    nrf_cunit_mock_call * p_transport_alloc_mock_call;
    nrf_cunit_expect_call_return((uint8_t *)"hci_transport_tx_free", &p_transport_alloc_mock_call);
    (void)hci_transport_tx_free();

    nrf_cunit_verify_call_return();
}

//lint -e14
CU_SUITE_START_PUBLIC(cmd_decoder_gap_suite)
{
    CU_SUITE_SETUP_FUNC(gap_setup_func);
    CU_SUITE_TEARDOWN_FUNC(gap_teardown_func);

    CU_SUITE_ADD_TEST(gap_device_name_set);
    CU_SUITE_ADD_TEST(nrffosdk_1302_gap_device_name_set_optional_params_absent);
    CU_SUITE_ADD_TEST(gap_device_name_get_successful);
    CU_SUITE_ADD_TEST(gap_device_name_get_length_field_not_present);
    CU_SUITE_ADD_TEST(gap_device_name_get_remote_result_buffer_not_present);
    CU_SUITE_ADD_TEST(gap_device_name_get_to_small_buffer_given_to_softdevice);
    CU_SUITE_ADD_TEST(gap_device_name_get_invalid_addr_is_propagated);
    CU_SUITE_ADD_TEST(gap_device_name_get_to_big_buffer_indicated_by_caller);
    CU_SUITE_ADD_TEST(gap_appearance_set);
    CU_SUITE_ADD_TEST(gap_ppcp_set);
    CU_SUITE_ADD_TEST(gap_ppcp_set_conn_params_absent);
    CU_SUITE_ADD_TEST(gap_ppcp_get_successful);
    CU_SUITE_ADD_TEST(gap_ppcp_get_result_buffer_not_present);
    CU_SUITE_ADD_TEST(gap_adv_start_undirected_no_white_list);
    CU_SUITE_ADD_TEST(gap_adv_start_directed_no_white_list);
    CU_SUITE_ADD_TEST(gap_adv_start_directed_white_list_addr);
    CU_SUITE_ADD_TEST(gap_adv_start_directed_white_list_irk);
    CU_SUITE_ADD_TEST(gap_adv_start_directed_white_list_addr_and_irk);
    CU_SUITE_ADD_TEST(gap_adv_start_undirected_white_list_addr_and_irk);
    CU_SUITE_ADD_TEST(gap_adv_data_set_adv_data_and_scan_rsp);
    CU_SUITE_ADD_TEST(gap_adv_data_set_scan_rsp_data_only);
    CU_SUITE_ADD_TEST(gap_adv_data_set_adv_data_only);
    CU_SUITE_ADD_TEST(gap_disconnect);
    CU_SUITE_ADD_TEST(gap_sec_params_reply_null);
    CU_SUITE_ADD_TEST(gap_sec_params_reply);
    CU_SUITE_ADD_TEST(gap_sec_params_reply_invalid_conn_handle); //fails starts
    CU_SUITE_ADD_TEST(gap_conn_param_update_null);
    CU_SUITE_ADD_TEST(gap_conn_param_update_params);
    CU_SUITE_ADD_TEST(gap_appearance_get_successful);
    CU_SUITE_ADD_TEST(gap_sec_info_reply_optionals_not_present);
    CU_SUITE_ADD_TEST(gap_sec_info_reply_optionals_present);
    CU_SUITE_ADD_TEST(gap_sec_info_reply_encrypt_not_present);
    CU_SUITE_ADD_TEST(gap_sec_info_reply_signing_not_present);
    CU_SUITE_ADD_TEST(gap_device_name_set_sanchk_cmdlen_lt_actual_pkt);
    CU_SUITE_ADD_TEST(gap_device_name_set_sanchk_cmdlen_lt_dev_name_len);
    CU_SUITE_ADD_TEST(gap_appearance_set_sanchk_cmdlen_lt_actual_pkt);
    CU_SUITE_ADD_TEST(gap_adv_data_set_sanchk_cmdlen_lt_adv_data_len);
    CU_SUITE_ADD_TEST(gap_adv_data_set_sanchk_cmdlen_lt_scan_data_len);
    CU_SUITE_ADD_TEST(gap_adv_data_set_sanchk_cmdlen_lt_adv_data_len_zero);
    CU_SUITE_ADD_TEST(gap_adv_data_set_sanchk_cmdlen_both_len_zero);
    CU_SUITE_ADD_TEST(gap_adv_start_only_type);
    CU_SUITE_ADD_TEST(gap_adv_data_start_sanchk_cmdlen_lt_peer_addr);
    CU_SUITE_ADD_TEST(gap_adv_data_start_sanchk_cmd_len_lt_fp);
    CU_SUITE_ADD_TEST(gap_adv_data_start_sanchk_cmd_len_lt_gap_whitelist_present);
    CU_SUITE_ADD_TEST(gap_adv_data_start_sanchk_cmd_len_lt_gap_whitelist_count_mismatch);
    CU_SUITE_ADD_TEST(gap_adv_data_start_sanchk_cmd_len_lt_gap_irk_count_mismatch); // fails here
    CU_SUITE_ADD_TEST(gap_adv_data_start_sanchk_cmd_len_lt_interval);
    CU_SUITE_ADD_TEST(gap_adv_data_start_sanchk_cmd_len_lt_timeout);
    CU_SUITE_ADD_TEST(gap_conn_param_update_sanchk_cmdlen_lt_chandle);
    CU_SUITE_ADD_TEST(gap_conn_param_update_sanchk_cmdlen_lt_field_present);
    CU_SUITE_ADD_TEST(gap_conn_param_update_sanchk_cmdlen_lt_conn_param);
    CU_SUITE_ADD_TEST(gap_disconnect_sanchk_cmdlen_lt_handle);
    CU_SUITE_ADD_TEST(gap_disconnect_sanchk_cmdlen_lt_hcicode);
    CU_SUITE_ADD_TEST(gap_ppcp_set_sanchk_cmdlen_lt_connparam);
    CU_SUITE_ADD_TEST(gap_sec_params_reply_sanchk_cmdlen_lt_chandle);
    CU_SUITE_ADD_TEST(gap_sec_params_reply_sanchk_cmdlen_lt_secstatus);
    CU_SUITE_ADD_TEST(gap_sec_params_reply_sanchk_cmdlen_lt_secparams);
    CU_SUITE_ADD_TEST(gap_sec_params_reply_sanchk_cmdlen_lt_tmeout);
    CU_SUITE_ADD_TEST(gap_sec_params_reply_sanchk_cmdlen_lt_bitfield);
    CU_SUITE_ADD_TEST(gap_sec_params_reply_sanchk_cmdlen_lt_minkeysize);
    CU_SUITE_ADD_TEST(gap_sec_params_reply_sanchk_cmdlen_lt_maxkeysize);
    CU_SUITE_ADD_TEST(gap_sec_params_reply_sanchk_cmdlen_lt_secparams_absent);
    CU_SUITE_ADD_TEST(gap_sec_info_reply_sanchk_cmdlen_lt_chandle);
    CU_SUITE_ADD_TEST(gap_sec_info_reply_sanchk_cmdlen_lt_enc_info);
    CU_SUITE_ADD_TEST(gap_sec_info_reply_sanchk_cmdlen_lt_sign_info);
    CU_SUITE_ADD_TEST(gap_sec_info_reply_sanchk_cmdlen_lt_enc_sign_info);
    CU_SUITE_ADD_TEST(gap_sec_info_reply_sanchk_cmdlen_lt_enc_sign_info_absent);
    CU_SUITE_ADD_TEST(gap_appearance_get_sanchk_cmdlen_lt_app_get);
    CU_SUITE_ADD_TEST(gap_ppcp_get_sanchk_cmdlen_lt_ppcp);
    CU_SUITE_ADD_TEST(gap_device_name_get_sanchk_cmdlen_lt_length);
    CU_SUITE_ADD_TEST(gap_device_name_get_sanchk_cmdlen_lt_dname);
    CU_SUITE_ADD_TEST(gap_device_name_get_sanchk_cmdlen_lt_dname_2);
    CU_SUITE_ADD_TEST(gap_device_name_get_sanchk_cmdlen_lt_dname_buff_present);

    CU_SUITE_ADD_TEST(gap_device_name_set_sanchk_cmdlen_lt_dev_name_present);
    CU_SUITE_ADD_TEST(gap_device_name_set_sanchk_cmdlen_lt_write_per_present);
    CU_SUITE_ADD_TEST(gap_ppcp_set_sanchk_cmdlen_lt_connparam_absent);
    CU_SUITE_ADD_TEST(gap_appearance_get_nullptr);

}
CU_SUITE_END
