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
#include "hci_mem_pool.h" //todo: check if this needed.

//nrf_cunit_mock_call             * mp_user_reg_mock_call;
//hci_transport_event_handler_t  g_cmd_encoder_event_handler;
uint8_t                    g_tx_memory_buffer[1027];

nrf_cunit_mock_call *      pg_transport_tx_alloc_mock_obj;  /** < Mock Object for transport_tx_alloc function. This is kept global to allow unit tests to have control over this function.>*/

static uint8_t *                  pg_cmd_response_buf;

static uint32_t                   g_cmd_response_len;


void set_cmd_response(uint8_t * p_response, uint32_t response_length)
{
    pg_cmd_response_buf = p_response;
    g_cmd_response_len  = response_length;
}


void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_nam)
{
    CU_ASSERT(false);
}


void __WFE(void)
{
    static uint32_t rounds;

    CU_ASSERT(pg_cmd_response_buf != NULL);

    if ((++rounds & 0x0F) == 0)
    {
        ble_rpc_cmd_rsp_pkt_received(pg_cmd_response_buf, g_cmd_response_len);
    }
}


void reset_unit_test(void)
{
    pg_cmd_response_buf = NULL;

    nrf_cunit_reset_mock();
}


/**
 * @brief Function for setting up the encoder and verifying that the encoder registers itself as an user in the
 * transport layer module (mock).
 */
void setup_cmd_encoder(void)
{
    nrf_cunit_mock_call * p_mock_call;
    nrf_cunit_mock_call * p_tx_register_mock_call;
    hci_transport_tx_done_result_t tx_done = HCI_TRANSPORT_TX_DONE_SUCCESS;

    // Expect the transport_tx_alloc function to be called in the transport layer module.
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_tx_alloc", &p_mock_call);
    p_mock_call->result[0] = (uint32_t) g_tx_memory_buffer;

    // Expect the hci_transport_tx_done_register function to be called in the transport layer module.
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_tx_done_register", &p_tx_register_mock_call);

    // We enable the SoftDevice (ble_cmd_encoder).
    uint32_t err_code = ble_rpc_cmd_encoder_init();
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    hci_transport_tx_done_handler_t tx_done_handler = (hci_transport_tx_done_handler_t)p_tx_register_mock_call->arg_actual[0];
    if (tx_done_handler == NULL)
    {
        CU_ASSERT(!"No tx_done_handler registered in hci_transport.");
    }

    // Put the tx to be transmitted, so we only need to wait for response.
    tx_done_handler(tx_done);

    CU_ASSERT_INT_EQUAL(true, p_mock_call->executed);
}


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_uuid_encode(...)
 * when all optional input data is omitted  
 *
 * The expected encoded packet is:
 *     sd_ble_uuid_encode(ble_uuid_t const * const p_uuid,
 *                        uint8_t          * const p_uuid_le_len,
 *                        uint8_t          * const p_uuid_le)
 */
CU_TEST_START(test_encoder_sd_ble_uuid_encode_null_input)
{
    reset_unit_test();

    nrf_cunit_mock_call * p_mock_call;

    uint32_t err_code;    
    
    const uint8_t expected[]             = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_UUID_ENCODE,
        RPC_BLE_FIELD_NOT_PRESENT,
        RPC_BLE_FIELD_NOT_PRESENT,
        RPC_BLE_FIELD_NOT_PRESENT
    };
    const uint8_t cmd_response_message[] = 
    {
        BLE_RPC_PKT_RESP,
        SD_BLE_UUID_ENCODE,
        (uint8_t)(NRF_ERROR_INVALID_ADDR),
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 8u),
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 16u),        
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 24u),               
    };    
    
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);

    pg_cmd_response_buf = (uint8_t *)cmd_response_message;
    g_cmd_response_len   = sizeof(cmd_response_message);
    
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    err_code = sd_ble_uuid_encode(NULL, NULL, NULL);
    CU_ASSERT_INT_EQUAL(NRF_ERROR_INVALID_ADDR, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();               
}
CU_TEST_END

/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_uuid_encode(...) when all 
 * optional parameters are present and also validates the output parameters.
 *
 * The expected encoded packet is:
 *     sd_ble_uuid_encode(ble_uuid_t const * const p_uuid,
 *                        uint8_t          * const p_uuid_le_len,
 *                        uint8_t          * const p_uuid_le)
 */
CU_TEST_START(test_encoder_sd_ble_uuid_encode)
{
    reset_unit_test();

    nrf_cunit_mock_call * p_mock_call;
    
    uint8_t  uuid_le_len;
    uint8_t  uuid_le[16u];
    uint32_t err_code;    
    
    const ble_uuid_t ble_uuid            = {0xAA55u, 0x11u};            
    const uint8_t  expected[]            = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_UUID_ENCODE,
        RPC_BLE_FIELD_PRESENT,
        LSB(ble_uuid.uuid),
        MSB(ble_uuid.uuid),        
        ble_uuid.type,
        RPC_BLE_FIELD_PRESENT,
        RPC_BLE_FIELD_PRESENT
    };
    const uint8_t expected_uuid[]        = {0xAAu, 0x55u};
    const uint8_t expected_length        = sizeof(expected_uuid);    
    const uint8_t cmd_response_message[] = 
    {
        BLE_RPC_PKT_RESP,
        SD_BLE_UUID_ENCODE,
        (uint8_t)(NRF_SUCCESS),
        (uint8_t)(NRF_SUCCESS << 8u),
        (uint8_t)(NRF_SUCCESS << 16u),        
        (uint8_t)(NRF_SUCCESS << 24u),               
        expected_length,
        expected_uuid[0],
        expected_uuid[1]
    };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);

    pg_cmd_response_buf = (uint8_t *)cmd_response_message;
    g_cmd_response_len   = sizeof(cmd_response_message);

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    err_code = sd_ble_uuid_encode(&ble_uuid, &uuid_le_len, uuid_le);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);
    CU_ASSERT_INT_EQUAL(uuid_le_len, expected_length);         
    CU_ASSERT(memcmp(
                     uuid_le,
                     &(cmd_response_message[BLE_OP_CODE_SIZE  +
                                            BLE_PKT_TYPE_SIZE +
                                            RPC_ERR_CODE_SIZE +
                                            sizeof(expected_length)]),
                     uuid_le_len
                    )
                    ==
                    0);
    
    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();               
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_uuid_encode(...)
 * when uuid input data is omitted  
 *
 * The expected encoded packet is:
 *     sd_ble_uuid_encode(ble_uuid_t const * const p_uuid,
 *                        uint8_t          * const p_uuid_le_len,
 *                        uint8_t          * const p_uuid_le)
 */
CU_TEST_START(test_encoder_sd_ble_uuid_encode_uuid_input_omitted)
{
    reset_unit_test();

    nrf_cunit_mock_call * p_mock_call;

    uint8_t  uuid_le_len;
    uint8_t  uuid_le[16u];    
    uint32_t err_code;    
    
    const uint8_t expected[]             = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_UUID_ENCODE,
        RPC_BLE_FIELD_NOT_PRESENT,
        RPC_BLE_FIELD_PRESENT,
        RPC_BLE_FIELD_PRESENT
    };
    const uint8_t cmd_response_message[] = 
    {
        BLE_RPC_PKT_RESP,
        SD_BLE_UUID_ENCODE,
        (uint8_t)(NRF_ERROR_INVALID_ADDR),
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 8u),
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 16u),        
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 24u),               
    };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);

    pg_cmd_response_buf = (uint8_t *)cmd_response_message;
    g_cmd_response_len   = sizeof(cmd_response_message);

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    err_code = sd_ble_uuid_encode(NULL, &uuid_le_len, uuid_le);
    CU_ASSERT_INT_EQUAL(NRF_ERROR_INVALID_ADDR, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();               
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_uuid_encode(...)
 * when length input data is omitted  
 *
 * The expected encoded packet is:
 *     sd_ble_uuid_encode(ble_uuid_t const * const p_uuid,
 *                        uint8_t          * const p_uuid_le_len,
 *                        uint8_t          * const p_uuid_le)
 */
CU_TEST_START(test_encoder_sd_ble_uuid_encode_length_omitted)
{
    reset_unit_test();

    nrf_cunit_mock_call * p_mock_call;

    uint8_t  uuid_le[16u];    
    uint32_t err_code;    
    
    const ble_uuid_t ble_uuid            = {0xAA55u, 0x11u};                
    const uint8_t expected[]             = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_UUID_ENCODE,
        RPC_BLE_FIELD_PRESENT,
        LSB(ble_uuid.uuid),
        MSB(ble_uuid.uuid),        
        ble_uuid.type,        
        RPC_BLE_FIELD_NOT_PRESENT,
        RPC_BLE_FIELD_PRESENT
    };
    const uint8_t cmd_response_message[] = 
    {
        BLE_RPC_PKT_RESP,
        SD_BLE_UUID_ENCODE,
        (uint8_t)(NRF_ERROR_INVALID_ADDR),
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 8u),
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 16u),        
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 24u),               
    };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);

    pg_cmd_response_buf  = (uint8_t *)cmd_response_message;
    g_cmd_response_len    = sizeof(cmd_response_message);

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) pg_cmd_response_buf;

    setup_cmd_encoder();
    
    err_code = sd_ble_uuid_encode(&ble_uuid, NULL, uuid_le);
    CU_ASSERT_INT_EQUAL(NRF_ERROR_INVALID_ADDR, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();               
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_uuid_encode(...)
 * when uuid result buffer input data is omitted  
 *
 * The expected encoded packet is:
 *     sd_ble_uuid_encode(ble_uuid_t const * const p_uuid,
 *                        uint8_t          * const p_uuid_le_len,
 *                        uint8_t          * const p_uuid_le)
 */
CU_TEST_START(test_encoder_sd_ble_uuid_encode_uuid_output_omitted)
{
    reset_unit_test();

    nrf_cunit_mock_call * p_mock_call;

    uint8_t  uuid_le_len;
    uint32_t err_code;    
    
    const ble_uuid_t ble_uuid            = {0xAA55u, 0x11u};                
    const uint8_t expected[]             = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_UUID_ENCODE,
        RPC_BLE_FIELD_PRESENT,
        LSB(ble_uuid.uuid),
        MSB(ble_uuid.uuid),        
        ble_uuid.type,        
        RPC_BLE_FIELD_PRESENT,
        RPC_BLE_FIELD_NOT_PRESENT
    };
    const uint8_t cmd_response_message[] = 
    {
        BLE_RPC_PKT_RESP,
        SD_BLE_UUID_ENCODE,
        (uint8_t)(NRF_ERROR_INVALID_ADDR),
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 8u),
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 16u),        
        (uint8_t)(NRF_ERROR_INVALID_ADDR << 24u),               
    };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);

    pg_cmd_response_buf  = (uint8_t *)cmd_response_message;
    g_cmd_response_len    = sizeof(cmd_response_message);

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) pg_cmd_response_buf;

    setup_cmd_encoder();
    
    err_code = sd_ble_uuid_encode(&ble_uuid, &uuid_le_len, NULL);
    CU_ASSERT_INT_EQUAL(NRF_ERROR_INVALID_ADDR, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();               
}
CU_TEST_END

#if 0
/**
 * This test will verify the encoding of the SoftDevice call: sd_power_system_off()
 * uint32_t sd_power_system_off(void)
 */
CU_TEST_START(test_encoder_sd_power_system_off)
{
    reset_unit_test();

    nrf_cunit_mock_call * p_mock_call_w;

    const uint8_t  expected[]                 =
    {
        BLE_RPC_PKT_CMD,
    	RPC_SD_POWER_SYSTEM_OFF
    };

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call_w);
    p_mock_call_w->arg[1] = (uint32_t)expected;
    p_mock_call_w->arg[2] = sizeof(expected);

    setup_cmd_encoder();

    uint32_t err_code = sd_power_system_off();
    CU_ASSERT_INT_EQUAL(err_code, NRF_SUCCESS);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();
}
CU_TEST_END


/**
 * This test will verify that ble_rpc_cmd_encoder_init functions returns NRF_ERROR_NO_MEM when
 * hci_transport_tx_alloc function fails with error code NRF_ERROR_NO_MEM.
 */
CU_TEST_START(test_encoder_cmd_encoder_init_tx_alloc_fail)
{
    reset_unit_test();

    nrf_cunit_mock_call * p_mock_call;

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_tx_alloc", &p_mock_call);
    // Make the tranport_tx_alloc function return NRF_ERROR_NO_MEM.
    p_mock_call->error = NRF_ERROR_NO_MEM;

    CU_ASSERT(ble_rpc_cmd_encoder_init() == NRF_ERROR_NO_MEM);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();
}
CU_TEST_END


/**
 * This test will verify that ble_rpc_cmd_encoder_init functions returns NRF_ERROR_INTERNAL when
 * hci_transport_open function fails with error code NRF_ERROR_INTERNAL.
 */
CU_TEST_START(test_encoder_cmd_encoder_init_transport_open_fail)
{
    reset_unit_test();

    nrf_cunit_mock_call * p_mock_call;

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_tx_alloc", &p_mock_call);
    p_mock_call->error = NRF_ERROR_INTERNAL;

    CU_ASSERT(ble_rpc_cmd_encoder_init() == NRF_ERROR_INTERNAL);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();
}
CU_TEST_END
#endif

CU_SUITE_START(ble_cmd_encoder_encode_commands)
{
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_uuid_encode_null_input);
//    CU_SUITE_ADD_TEST(test_encoder_cmd_encoder_init_tx_alloc_fail);
//    CU_SUITE_ADD_TEST(test_encoder_cmd_encoder_init_transport_open_fail);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_uuid_encode);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_uuid_encode_uuid_input_omitted);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_uuid_encode_length_omitted);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_uuid_encode_uuid_output_omitted);
//    CU_SUITE_ADD_TEST(test_encoder_sd_power_system_off);
}
CU_SUITE_END

CU_REGISTRY_START_STANDALONE(ble_encoder)
{
    CU_REGISTRY_ADD_SUITE(ble_cmd_encoder_encode_commands);
    CU_REGISTRY_ADD_SUITE_EXTERN(ble_cmd_encoder_gap_encode_commands);
    CU_REGISTRY_ADD_SUITE_EXTERN(ble_cmd_encoder_gatts_encode_commands);
}
CU_REGISTRY_END

int main(void)
{ 
    CU_ENABLE_SUMMARY();

    CU_ADD_REGISTRY(ble_encoder);
    return 0;
}
