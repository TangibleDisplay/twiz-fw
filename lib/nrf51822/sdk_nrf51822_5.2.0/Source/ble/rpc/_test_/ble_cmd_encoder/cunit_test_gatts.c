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
#include "ble_gatts.h"
#include "ble_types.h"

void setup_cmd_encoder(void);  // Defined in cunit_test.c. todo: Consider placing this in header file.

void reset_unit_test(void);    // Defined in cunit_test.c. todo: Consider placing this in header file.

void set_cmd_response(uint8_t * p_response, uint32_t response_length);

/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_service_add(...)
 * when all pointer input data is valid also output data is validated.
 *
 * The expected encoded packet is:
 *     uint32_t sd_ble_gatts_service_add(uint8_t                  type, 
 *                                       ble_uuid_t const * const p_uuid, 
 *                                       uint16_t         * const p_handle)
 */
CU_TEST_START(test_encoder_sd_ble_gatts_service_add)
{
    reset_unit_test();    

    nrf_cunit_mock_call * p_mock_call;
        
    const uint8_t    type                   = 0xAAu;        
    const ble_uuid_t ble_uuid               = {0x55CCu, 0xAAu};
    uint16_t         handle                 = 0x55CCu;    
    const uint8_t    expected[]             = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_SERVICE_ADD,
        type, 
        RPC_BLE_FIELD_PRESENT,
        LSB(ble_uuid.uuid), 
        MSB(ble_uuid.uuid), 
        ble_uuid.type, 
        RPC_BLE_FIELD_PRESENT
    }; 
    const uint16_t   expected_handle        = ~handle;
    const uint8_t    cmd_response_message[] = 
    {
        BLE_RPC_PKT_RESP,
        SD_BLE_GATTS_SERVICE_ADD,
        (uint8_t)(NRF_SUCCESS),
        (uint8_t)(NRF_SUCCESS << 8u),
        (uint8_t)(NRF_SUCCESS << 16u),        
        (uint8_t)(NRF_SUCCESS << 24u),
        LSB(expected_handle),
        MSB(expected_handle)        
    };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);
    
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));
    
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_service_add(type, &ble_uuid, &handle);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);        
    CU_ASSERT_INT_EQUAL(expected_handle, handle);        
            
    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();    
}
CU_TEST_END

/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_service_add(...)
 * when UUID pointer input data is not valid 
 *
 * The expected encoded packet is:
 *     uint32_t sd_ble_gatts_service_add(uint8_t                  type, 
 *                                       ble_uuid_t const * const p_uuid, 
 *                                       uint16_t         * const p_handle)
 */
CU_TEST_START(test_encoder_sd_ble_gatts_service_add_null_uuid)
{
    reset_unit_test();    

    nrf_cunit_mock_call * p_mock_call;
        
    const uint8_t    type                = 0xAAu;        
    uint16_t         handle              = 0x77CCu;    
    const uint8_t    expected[]          = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_SERVICE_ADD,
        type, 
        RPC_BLE_FIELD_NOT_PRESENT,
        RPC_BLE_FIELD_PRESENT
    }; 
    const uint8_t cmd_response_message[] = {
                                            BLE_RPC_PKT_RESP,
                                            SD_BLE_GATTS_SERVICE_ADD,
                                            0, 0, 0, 0
                                           };    
     nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);
    
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));
    
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_service_add(type, NULL, &handle);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);        
    
    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();    
}
CU_TEST_END

/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_service_add(...)
 * when p_handle pointer input data is not valid 
 *
 * The expected encoded packet is:
 *     uint32_t sd_ble_gatts_service_add(uint8_t                  type, 
 *                                       ble_uuid_t const * const p_uuid, 
 *                                       uint16_t         * const p_handle)
 */
CU_TEST_START(test_encoder_sd_ble_gatts_service_add_null_handle)
{
    reset_unit_test();    

    nrf_cunit_mock_call * p_mock_call;
        
    const uint8_t    type                = 0xAAu;    
    const ble_uuid_t ble_uuid            = {0x55CCu, 0xAAu};    
    const uint8_t    expected[]          = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_SERVICE_ADD,
        type, 
        RPC_BLE_FIELD_PRESENT,
        LSB(ble_uuid.uuid), 
        MSB(ble_uuid.uuid), 
        ble_uuid.type, 
        RPC_BLE_FIELD_NOT_PRESENT
    };     
    const uint8_t cmd_response_message[] = {
                                            BLE_RPC_PKT_RESP,
                                            SD_BLE_GATTS_SERVICE_ADD,
                                            0, 0, 0, 0
                                           };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);
    
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));
    
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_service_add(type, &ble_uuid, NULL);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);        
    
    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();    
}
CU_TEST_END

/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_service_add(...)
 * when no optional data is supplied 
 *
 * The expected encoded packet is:
 *     uint32_t sd_ble_gatts_service_add(uint8_t                  type, 
 *                                       ble_uuid_t const * const p_uuid, 
 *                                       uint16_t         * const p_handle)
 */
CU_TEST_START(test_encoder_sd_ble_gatts_service_add_no_optional)
{
    reset_unit_test();    

    nrf_cunit_mock_call * p_mock_call;
        
    const uint8_t    type                = 0xAAu;    
    const uint8_t    expected[]          = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_SERVICE_ADD,
        type, 
        RPC_BLE_FIELD_NOT_PRESENT,
        RPC_BLE_FIELD_NOT_PRESENT
    };     
    const uint8_t cmd_response_message[] = {
                                            BLE_RPC_PKT_RESP,
                                            SD_BLE_GATTS_SERVICE_ADD,
                                            0, 0, 0, 0
                                           };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);
    
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));
    
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_service_add(type, NULL, NULL);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);        
    
    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();    
}
CU_TEST_END

/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_sys_attr_set(...)
 * when all optional input data is present 
 *
 * The expected encoded packet is:
 *     uint32_t sd_ble_gatts_sys_attr_set(uint16_t conn_handle, 
 *                                        uint8_t const * const p_sys_attr_data, 
 *                                        uint16_t len)
 */
CU_TEST_START(test_encoder_sd_ble_gatts_sys_attr_set)
{
    reset_unit_test();    

    nrf_cunit_mock_call * p_mock_call;
    
    const uint16_t conn_handle           = 0xAA55u;
    const uint8_t  sys_attr_data[]       = {0x77u, 0xBBu};
    const uint16_t len                   = sizeof(sys_attr_data);    
    const uint8_t  expected[]            = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_SYS_ATTR_SET,
        LSB(conn_handle), 
        MSB(conn_handle),   
        RPC_BLE_FIELD_PRESENT,        
        LSB(len),
        //lint -e(572) "Excessive shift value"
        MSB(len),
        sys_attr_data[0],        
        sys_attr_data[1]                
    };  
    const uint8_t cmd_response_message[] = {
                                            BLE_RPC_PKT_RESP,
                                            SD_BLE_GATTS_SYS_ATTR_SET,
                                            0, 0, 0, 0
                                           };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);
        
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));
    
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_sys_attr_set(conn_handle, sys_attr_data, len);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();        
}
CU_TEST_END

/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_sys_attr_set(...)
 * when no optional input data is present 
 *
 * The expected encoded packet is:
 *     uint32_t sd_ble_gatts_sys_attr_set(uint16_t conn_handle, 
 *                                        uint8_t const * const p_sys_attr_data, 
 *                                        uint16_t len)
 */
CU_TEST_START(test_encoder_sd_ble_gatts_sys_attr_set_no_optional)
{
    reset_unit_test();    

    nrf_cunit_mock_call * p_mock_call;
    
    const uint16_t conn_handle           = 0xAA55u;
    const uint16_t len                   = 0;
    const uint8_t  expected[]            = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_SYS_ATTR_SET,
        LSB(conn_handle), 
        MSB(conn_handle),         
        RPC_BLE_FIELD_NOT_PRESENT
    };  
    const uint8_t cmd_response_message[] = {
                                            BLE_RPC_PKT_RESP,
                                            SD_BLE_GATTS_SYS_ATTR_SET,
                                            0, 0, 0, 0
                                           };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);
        
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));
    
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_sys_attr_set(conn_handle, NULL, len);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();        
}
CU_TEST_END

/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_sys_attr_set(...)
 * when out-of-bound size input buffer is supplied 
 *
 * The expected encoded packet is:
 *     uint32_t sd_ble_gatts_sys_attr_set(uint16_t conn_handle, 
 *                                        uint8_t const * const p_sys_attr_data, 
 *                                        uint16_t len)
 */
CU_TEST_START(test_encoder_sd_ble_gatts_sys_attr_set_out_of_bound)
{    
    reset_unit_test();    

    const uint16_t conn_handle     = 0xAA55u;
    const uint8_t  sys_attr_data[] = {0x77u, 0xBBu};    
    const uint16_t len             = 0xFFFFu;
        
    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_sys_attr_set(conn_handle, sys_attr_data, len);
    CU_ASSERT_INT_EQUAL(NRF_ERROR_NO_MEM, err_code);
}
CU_TEST_END

/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_hvx(...)
 * when all optional input data is present 
 *
 * The expected encoded packet is:
 *     uint32_t sd_ble_gatts_hvx(uint16_t conn_handle, 
 *                               ble_gatts_hvx_params_t const * const p_hvx_params)
 *
 * GATT HVx parameters. 
 * typedef struct
 * {
 *  uint16_t          handle;             
 *  uint8_t           type;               
 *  uint16_t          offset;             
 *  uint16_t*         p_len;              
 *  uint8_t*          p_data;             
 * } ble_gatts_hvx_params_t; 
 */
CU_TEST_START(test_encoder_sd_ble_gatts_hvx_all_optionals)
{
    reset_unit_test();    

    nrf_cunit_mock_call * p_mock_call;
    
    const uint16_t conn_handle              = 0xAA55u;
    uint8_t        data[]                   = {0x99u, 0x77u};    
    uint16_t       len                      = sizeof(data);    
    const ble_gatts_hvx_params_t hvx_params = {0xCC33u, 0xFFu, 0x11BBu, &len, data};
    const uint8_t  expected[]               = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_HVX,
        LSB(conn_handle), 
        MSB(conn_handle),
        RPC_BLE_FIELD_PRESENT,
        LSB(hvx_params.handle),
        MSB(hvx_params.handle),
        hvx_params.type,
        LSB(hvx_params.offset),
        MSB(hvx_params.offset),        
        RPC_BLE_FIELD_PRESENT, 
        LSB(len),        
        MSB(len),         
        RPC_BLE_FIELD_PRESENT,         
        data[0],        
        data[1],         
    };  
    const uint8_t cmd_response_message[]    = {
                                               BLE_RPC_PKT_RESP,
                                               SD_BLE_GATTS_HVX,
                                               0, 0, 0, 0
                                              };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);
            
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_hvx(conn_handle, &hvx_params);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();        
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_hvx(...)
 * when no optional input data is present 
 *
 * The expected encoded packet is:
 *     uint32_t sd_ble_gatts_hvx(uint16_t conn_handle, 
 *                               ble_gatts_hvx_params_t const * const p_hvx_params)
 *
 * GATT HVx parameters. 
 * typedef struct
 * {
 *  uint16_t          handle;             
 *  uint8_t           type;               
 *  uint16_t          offset;             
 *  uint16_t*         p_len;              
 *  uint8_t*          p_data;             
 * } ble_gatts_hvx_params_t; 
 */
CU_TEST_START(test_encoder_sd_ble_gatts_hvx_no_optionals)
{
    reset_unit_test();    

    nrf_cunit_mock_call * p_mock_call;
    
    const uint16_t conn_handle              = 0xAA55u;
    const uint8_t  expected[]               = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_HVX,
        LSB(conn_handle), 
        MSB(conn_handle),
        RPC_BLE_FIELD_NOT_PRESENT
    };  
    const uint8_t cmd_response_message[]    = {
                                               BLE_RPC_PKT_RESP,
                                               SD_BLE_GATTS_HVX,
                                               0, 0, 0, 0
                                              };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);
            
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_hvx(conn_handle, NULL);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();        
}
CU_TEST_END

/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_hvx(...)
 * when all optional input data, excluding hvx_params.p_len field, is present 
 *
 * The expected encoded packet is:
 *     uint32_t sd_ble_gatts_hvx(uint16_t conn_handle, 
 *                               ble_gatts_hvx_params_t const * const p_hvx_params)
 *
 * GATT HVx parameters. 
 * typedef struct
 * {
 *  uint16_t          handle;             
 *  uint8_t           type;               
 *  uint16_t          offset;             
 *  uint16_t*         p_len;              
 *  uint8_t*          p_data;             
 * } ble_gatts_hvx_params_t; 
 */
CU_TEST_START(test_encoder_sd_ble_gatts_hvx_no_len_field)
{
    reset_unit_test();    

    nrf_cunit_mock_call * p_mock_call;
    
    const uint16_t conn_handle              = 0xAA55u;
    uint8_t        data[]                   = {0x99u, 0x22u};    
    const ble_gatts_hvx_params_t hvx_params = {0xCC33u, 0xFFu, 0x11BBu, NULL, data};
    const uint8_t  expected[]               = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_HVX,
        LSB(conn_handle), 
        MSB(conn_handle),
        RPC_BLE_FIELD_PRESENT,
        LSB(hvx_params.handle),
        MSB(hvx_params.handle),
        hvx_params.type,
        LSB(hvx_params.offset),
        MSB(hvx_params.offset),        
        RPC_BLE_FIELD_NOT_PRESENT, 
        RPC_BLE_FIELD_NOT_PRESENT         
    };  
    const uint8_t cmd_response_message[]    = {
                                               BLE_RPC_PKT_RESP,
                                               SD_BLE_GATTS_HVX,
                                               0, 0, 0, 0
                                              };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);
            
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_hvx(conn_handle, &hvx_params);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();        
}
CU_TEST_END

/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_hvx(...)
 * when all optional input data, excluding hvx_params.p_data field, is present 
 *
 * The expected encoded packet is:
 *     uint32_t sd_ble_gatts_hvx(uint16_t conn_handle, 
 *                               ble_gatts_hvx_params_t const * const p_hvx_params)
 *
 * GATT HVx parameters. 
 * typedef struct
 * {
 *  uint16_t          handle;             
 *  uint8_t           type;               
 *  uint16_t          offset;             
 *  uint16_t*         p_len;              
 *  uint8_t*          p_data;             
 * } ble_gatts_hvx_params_t; 
 */
CU_TEST_START(test_encoder_sd_ble_gatts_hvx_no_data_field)
{
    reset_unit_test();    

    nrf_cunit_mock_call * p_mock_call;
    
    const uint16_t conn_handle              = 0xAA55u;
    uint16_t       len                      = 0;    
    const ble_gatts_hvx_params_t hvx_params = {0xCC33u, 0xFFu, 0x11BBu, &len, NULL};
    const uint8_t  expected[]               = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_HVX,
        LSB(conn_handle), 
        MSB(conn_handle),
        RPC_BLE_FIELD_PRESENT,
        LSB(hvx_params.handle),
        MSB(hvx_params.handle),
        hvx_params.type,
        LSB(hvx_params.offset),
        MSB(hvx_params.offset),        
        RPC_BLE_FIELD_PRESENT, 
        LSB(len),
        MSB(len),                
        RPC_BLE_FIELD_NOT_PRESENT
    };  
    const uint8_t cmd_response_message[]    = {
                                               BLE_RPC_PKT_RESP,
                                               SD_BLE_GATTS_HVX,
                                               0, 0, 0, 0
                                              };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);
            
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_hvx(conn_handle, &hvx_params);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();        
}
CU_TEST_END

/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_hvx(...)
 * when out-of-bound size input buffer is supplied.
 *
 * The expected encoded packet is:
 *     uint32_t sd_ble_gatts_hvx(uint16_t conn_handle, 
 *                               ble_gatts_hvx_params_t const * const p_hvx_params)
 *
 * GATT HVx parameters. 
 * typedef struct
 * {
 *  uint16_t          handle;             
 *  uint8_t           type;               
 *  uint16_t          offset;             
 *  uint16_t*         p_len;              
 *  uint8_t*          p_data;             
 * } ble_gatts_hvx_params_t; 
 */
CU_TEST_START(test_encoder_sd_ble_gatts_hvx_out_of_bound)
{
    reset_unit_test();    

    const uint16_t conn_handle              = 0xAA55u;
    uint16_t       len                      = 0xFFFFu;  
    uint8_t        data[]                   = {0x99u, 0x22u};    
    const ble_gatts_hvx_params_t hvx_params = {0xCC33u, 0xFFu, 0x11BBu, &len, data};
    
    uint32_t err_code = sd_ble_gatts_hvx(conn_handle, &hvx_params);
    CU_ASSERT_INT_EQUAL(NRF_ERROR_DATA_SIZE, err_code);
}
CU_TEST_END

/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_hvx(...)
 * when length output parameter is checked.
 *
 * The expected encoded packet is:
 *     uint32_t sd_ble_gatts_hvx(uint16_t conn_handle, 
 *                               ble_gatts_hvx_params_t const * const p_hvx_params)
 *
 * GATT HVx parameters. 
 * typedef struct
 * {
 *  uint16_t          handle;             
 *  uint8_t           type;               
 *  uint16_t          offset;             
 *  uint16_t*         p_len;              
 *  uint8_t*          p_data;             
 * } ble_gatts_hvx_params_t; 
 */
CU_TEST_START(test_encoder_sd_ble_gatts_hvx_write_length)
{
    reset_unit_test();    

    nrf_cunit_mock_call * p_mock_call;
    
    const uint16_t               conn_handle = 0xAA55u;
    uint16_t                     len         = 0;    
    const ble_gatts_hvx_params_t hvx_params  = {0xCC33u, 0xFFu, 0x11BBu, &len, NULL};
    const uint8_t                expected[]  = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_HVX,
        LSB(conn_handle), 
        MSB(conn_handle),
        RPC_BLE_FIELD_PRESENT,
        LSB(hvx_params.handle),
        MSB(hvx_params.handle),
        hvx_params.type,
        LSB(hvx_params.offset),
        MSB(hvx_params.offset),        
        RPC_BLE_FIELD_PRESENT, 
        LSB(len),
        MSB(len),                
        RPC_BLE_FIELD_NOT_PRESENT
    };  
    const uint8_t cmd_response_message[]     = 
    {
        BLE_RPC_PKT_RESP,
        SD_BLE_GATTS_HVX,
        0, 0, 0, 0,
        0x33u, 0x77u
    };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);
            
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));



    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_hvx(conn_handle, &hvx_params);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);
    const uint16_t expected_length = 
        uint16_decode(&(cmd_response_message[BLE_OP_CODE_SIZE  +
                                             BLE_PKT_TYPE_SIZE +
                                             RPC_ERR_CODE_SIZE]));
    CU_ASSERT_INT_EQUAL(expected_length, *(hvx_params.p_len));    

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();        
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_characteristic_add(...)
 * when all pointer input data is valid
 * sd_ble_gatts_characteristic_add(uint16_t                  service_handle,
 *                                 ble_gatts_char_md_t const *const p_char_md,
 *                                 ble_gatts_attr_t const    *const p_attr_char_value,
 *                                 ble_gatts_char_handles_t  *const p_handles)
 */
CU_TEST_START(test_encoder_sd_ble_gatts_characteristic_add_all_params_present)
{    
    reset_unit_test();    

    nrf_cunit_mock_call *       p_mock_call;
    uint16_t                    service_handle = 0xAAu;
    ble_gatts_char_md_t         char_md;
    ble_gatts_attr_t            attr_char_value;
    
    // Stimuli preparation.

    memset(&char_md, 0, sizeof(char_md));
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    
    char_md.char_props.read   = 1;
    char_md.char_props.notify = 1;
    char_md.char_ext_props.reliable_wr = 1;
    char_md.char_ext_props.wr_aux      = 0;
    
    static uint8_t s_user_descriptor[512]; // Maximum possible size of user descriptor.

    unsigned int i;

    for (i = 0; i < sizeof(s_user_descriptor); i++)
    {
        s_user_descriptor[i] = i % 256;  // The modulo operation ensure that the uint8_t boundary is
                                         // not exceeded.
    }

    static ble_gatts_char_pf_t char_pf;

    char_pf.format      = 0x01;
    char_pf.exponent    = 0x02;
    char_pf.unit        = 0x0304;
    char_pf.name_space  = 0x05;
    char_pf.desc        = 0x0607;
    
    ble_gatts_attr_md_t user_desc_md;
    
    user_desc_md.read_perm.lv  = 0xa;
    user_desc_md.read_perm.sm  = 0xb;
    user_desc_md.write_perm.lv = 0xc;
    user_desc_md.write_perm.sm = 0xd;
    user_desc_md.wr_auth       = 1;
    user_desc_md.rd_auth       = 0;
    user_desc_md.vloc          = 2;
    user_desc_md.vlen          = 1;

    ble_gatts_attr_md_t cccd_md;

    cccd_md.read_perm.lv  = 0x1;
    cccd_md.read_perm.sm  = 0x2;
    cccd_md.write_perm.lv = 0x3;
    cccd_md.write_perm.sm = 0x4;
    cccd_md.wr_auth       = 0;
    cccd_md.rd_auth       = 1;
    cccd_md.vloc          = 3;
    cccd_md.vlen          = 0;

    ble_gatts_attr_md_t sccd_md;

    sccd_md.read_perm.lv  = 0x2;
    sccd_md.read_perm.sm  = 0x3;
    sccd_md.write_perm.lv = 0x4;
    sccd_md.write_perm.sm = 0x5;
    sccd_md.wr_auth       = 1;
    sccd_md.rd_auth       = 0;
    sccd_md.vloc          = 1;
    sccd_md.vlen          = 1;

    char_md.p_char_user_desc        = s_user_descriptor;
    char_md.char_user_desc_max_size = 0xabcd;
    char_md.char_user_desc_size     = 512;
    char_md.p_char_pf               = &char_pf;
    char_md.p_user_desc_md          = &user_desc_md;
    char_md.p_cccd_md               = &cccd_md;
    char_md.p_sccd_md               = &sccd_md;

    ble_uuid_t uuid;

    uuid.uuid   = 0x0102;
    uuid.type   = 0xab;

    ble_gatts_attr_md_t attr_md;

    attr_md.read_perm.lv  = 0x5;
    attr_md.read_perm.sm  = 0x6;
    attr_md.write_perm.lv = 0x7;
    attr_md.write_perm.sm = 0x8;
    attr_md.wr_auth       = 0;
    attr_md.rd_auth       = 1;
    attr_md.vloc          = 0;
    attr_md.vlen          = 0;

    uint8_t attr_data[512];

    for (i = 0; i < sizeof(attr_data); i++)
    {
        attr_data[i] = i % 256;  // The modulo operation ensure that the uint8_t boundary is
                                  // not exceeded.
    }

    attr_char_value.p_uuid      = &uuid;
    attr_char_value.p_attr_md   = &attr_md;
    attr_char_value.init_len    = 512;
    attr_char_value.init_offs   = 0;
    attr_char_value.max_len     = 512;
    attr_char_value.p_value     = attr_data;

    // Expected output

    // Since we need to fit the huge user descriptor in the middle of expected output, the output is
    // prepared in parts and then combined.

    const uint8_t part_exp_output_until_user_des[] =
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_CHARACTERISTIC_ADD,
        LSB(service_handle),
        MSB(service_handle),
        /*p_char_md - Characteristic Metadata*/
        RPC_BLE_FIELD_PRESENT,      // Characteristic Metadata Present
        0x12,                       // Characteristic properties.
        0x01,                       // Characteristic Extended Properties.
        0xcd,                       // LSB of char_user_desc_max_size
        0xab,                       // MSB of char_user_desc_max_size
        0x00,                       // LSB of char_user_desc_size
        0x02,                       // MSB of char_user_desc_size
        RPC_BLE_FIELD_PRESENT       // User Descriptor field present
    };


    const uint8_t part_exp_output_after_user_des[] =
    {
        RPC_BLE_FIELD_PRESENT,      // Presentation Format present.
         0x01,                         // Presentation format - Format
         0x02,                         // Presentation format - Exponent
         0x04, 0x03,                   // Presentation format - Unit
         0x05,                         // Presentation format - Name Space
         0x07, 0x06,                   // Presentation format - Description
        RPC_BLE_FIELD_PRESENT,     // User desc metadata present
         0xab,                         // Read Permission lv , sm
         0xcd,                         // Write Permission lv , sm
         0x15,                         // wr_auth, rd_auth, vloc, vlen
        RPC_BLE_FIELD_PRESENT,     // CCCD metadata Present
         0x12,                         // Read Permission lv , sm
         0x34,                         // Write Permission lv , sm
         0x0e,                         // wr_auth, rd_auth, vloc, vlen
        RPC_BLE_FIELD_PRESENT,     // SCCD metadata Present
         0x23,                         // Read Permission lv , sm
         0x45,                         // Write Permission lv , sm
         0x13,                         // wr_auth, rd_auth, vloc, vlen
        /*p_attr_char_value*/
        RPC_BLE_FIELD_PRESENT,         // Characteristic attribute present.
         RPC_BLE_FIELD_PRESENT,          // UUID present.
          0x02,                            // LSB of UUID
          0x01,                            // MSB of UUID
          0xab,                            // UUID Type
         RPC_BLE_FIELD_PRESENT,          // Attribute metadata present.
          0x56,                           // Read Permission lv , sm
          0x78,                           // Write Permission lv , sm
          0x08,                           // wr_auth, rd_auth, vloc, vlen
         LSB(attr_char_value.init_len),
         MSB(attr_char_value.init_len),
         LSB(attr_char_value.init_offs),
         MSB(attr_char_value.init_offs),
         LSB(attr_char_value.max_len),
         MSB(attr_char_value.max_len),
         RPC_BLE_FIELD_PRESENT         // Attribute data present.
    };

    
    uint8_t complete_exp_output[
                                sizeof(part_exp_output_until_user_des) +
                                sizeof(s_user_descriptor)              +
                                sizeof(part_exp_output_after_user_des) +
                                sizeof(attr_data)                      +
                                sizeof(uint8_t)                          // to accommodate the RPC_BLE_FIELD_PRESENT field for handles.
                                ];

    // Populate expected output until just before user description
    memcpy(complete_exp_output,
           part_exp_output_until_user_des,
           sizeof(part_exp_output_until_user_des));

    // Append expected output with user descriptor
    memcpy(complete_exp_output + sizeof(part_exp_output_until_user_des),
           s_user_descriptor,
           sizeof(s_user_descriptor));

    // Append expected output with the rest of the partial expected output until just before
    // attribute data.
    memcpy(complete_exp_output + sizeof(part_exp_output_until_user_des) + sizeof(s_user_descriptor),
           part_exp_output_after_user_des,
           sizeof(part_exp_output_after_user_des));

    // Append expected output with attribute data.
    memcpy(
           (
            complete_exp_output +
            sizeof(part_exp_output_until_user_des) +
            sizeof(s_user_descriptor) +
            sizeof(part_exp_output_after_user_des)
           ),
           attr_data,
           sizeof(attr_data)
          );

    // And finally append the 'field present' field for handles.

    complete_exp_output[sizeof(complete_exp_output) - 1] = RPC_BLE_FIELD_PRESENT;

    /*Mock call*/
    ble_gatts_char_handles_t    handles_returned_by_mock;

    handles_returned_by_mock.value_handle        = 0xABCD;
    handles_returned_by_mock.user_desc_handle    = 0xED01;
    handles_returned_by_mock.cccd_handle         = 0x0203;
    handles_returned_by_mock.sccd_handle         = 0x0405;

    const uint8_t cmd_response_message[] = {
                                            BLE_RPC_PKT_RESP,
                                            SD_BLE_GATTS_CHARACTERISTIC_ADD,
                                            0, 0, 0, 0,                 // Return code.
                                            LSB(handles_returned_by_mock.value_handle),
                                            MSB(handles_returned_by_mock.value_handle),
                                            LSB(handles_returned_by_mock.user_desc_handle),
                                            MSB(handles_returned_by_mock.user_desc_handle),
                                            LSB(handles_returned_by_mock.cccd_handle),
                                            MSB(handles_returned_by_mock.cccd_handle),
                                            LSB(handles_returned_by_mock.sccd_handle),
                                            MSB(handles_returned_by_mock.sccd_handle)};
    
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)complete_exp_output;
    p_mock_call->arg[2] = sizeof(complete_exp_output);
    
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));



    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    ble_gatts_char_handles_t returned_handles;

    memset(&returned_handles, 0, sizeof(returned_handles));

    uint32_t err_code = sd_ble_gatts_characteristic_add(service_handle, &char_md, &attr_char_value, &returned_handles);

    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);        
    
    CU_ASSERT_INT_EQUAL(returned_handles.value_handle, handles_returned_by_mock.value_handle);
    CU_ASSERT_INT_EQUAL(returned_handles.user_desc_handle, handles_returned_by_mock.user_desc_handle);
    CU_ASSERT_INT_EQUAL(returned_handles.cccd_handle, handles_returned_by_mock.cccd_handle);
    CU_ASSERT_INT_EQUAL(returned_handles.sccd_handle, handles_returned_by_mock.sccd_handle);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();    

}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_characteristic_add(...)
 * when all pointer input data equal to NULL
 * sd_ble_gatts_characteristic_add(uint16_t                  service_handle,
 *                                 ble_gatts_char_md_t const *const p_char_md,
 *                                 ble_gatts_attr_t const    *const p_attr_char_value,
 *                                 ble_gatts_char_handles_t  *const p_handles)
 */
CU_TEST_START(test_encoder_sd_ble_gatts_characteristic_add_all_pointer_params_NULL)
{
    reset_unit_test();    

    nrf_cunit_mock_call *       p_mock_call;
    uint16_t                    service_handle = 0xAAu;
    ble_gatts_char_md_t         char_md;
    ble_gatts_attr_t            attr_char_value;

    // Stimuli preparation.

    memset(&char_md, 0, sizeof(char_md));
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    char_md.char_props.read   = 1;
    char_md.char_props.notify = 1;
    char_md.char_ext_props.reliable_wr = 1;
    char_md.char_ext_props.wr_aux      = 0;
    char_md.p_char_user_desc        = NULL;
    char_md.char_user_desc_max_size = 0xabcd;
    char_md.char_user_desc_size     = 0;
    char_md.p_char_pf               = NULL;
    char_md.p_user_desc_md          = NULL;
    char_md.p_cccd_md               = NULL;
    char_md.p_sccd_md               = NULL;

    attr_char_value.p_uuid      = NULL;
    attr_char_value.p_attr_md   = NULL;
    attr_char_value.init_len    = 512;
    attr_char_value.init_offs   = 0;
    attr_char_value.max_len     = 512;
    attr_char_value.p_value     = NULL;

    // Expected output

    const uint8_t exp_output[] =
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_CHARACTERISTIC_ADD,
        LSB(service_handle),
        MSB(service_handle),
        /*p_char_md - Characteristic Metadata*/
        RPC_BLE_FIELD_PRESENT,      // Characteristic Metadata Present
        0x12,                       // Characteristic properties.
        0x01,                       // Characteristic Extended Properties.
        0xcd,                       // LSB of char_user_desc_max_size
        0xab,                       // MSB of char_user_desc_max_size
        0x00,                       // LSB of char_user_desc_size
        0x00,                       // MSB of char_user_desc_size
        RPC_BLE_FIELD_NOT_PRESENT,  // User Descriptor field NOT present
        RPC_BLE_FIELD_NOT_PRESENT,  // Presentation Format NOT present.
        RPC_BLE_FIELD_NOT_PRESENT,  // User desc metadata NOT present
        RPC_BLE_FIELD_NOT_PRESENT,  // CCCD metadata NOT present
        RPC_BLE_FIELD_NOT_PRESENT,  // SCCD metadata Present
        /*p_attr_char_value*/
        RPC_BLE_FIELD_PRESENT,          // Characteristic attribute present.
        RPC_BLE_FIELD_NOT_PRESENT,      // UUID NOT present.
        RPC_BLE_FIELD_NOT_PRESENT,      // Attribute metadata NOT present.
        LSB(attr_char_value.init_len),
        MSB(attr_char_value.init_len),
        LSB(attr_char_value.init_offs),
        MSB(attr_char_value.init_offs),
        LSB(attr_char_value.max_len),
        MSB(attr_char_value.max_len),
        RPC_BLE_FIELD_NOT_PRESENT,      // Attribute data present.
        RPC_BLE_FIELD_PRESENT           // Handles present.
    };


    /*Mock call*/
    ble_gatts_char_handles_t    handles_returned_by_mock;

    handles_returned_by_mock.value_handle        = 0xABCD;
    handles_returned_by_mock.user_desc_handle    = 0xED01;
    handles_returned_by_mock.cccd_handle         = 0x0203;
    handles_returned_by_mock.sccd_handle         = 0x0405;

    const uint8_t cmd_response_message[] = {
                                            BLE_RPC_PKT_RESP,
                                            SD_BLE_GATTS_CHARACTERISTIC_ADD,
                                            0, 0, 0, 0,                 // Return code.
                                            LSB(handles_returned_by_mock.value_handle),
                                            MSB(handles_returned_by_mock.value_handle),
                                            LSB(handles_returned_by_mock.user_desc_handle),
                                            MSB(handles_returned_by_mock.user_desc_handle),
                                            LSB(handles_returned_by_mock.cccd_handle),
                                            MSB(handles_returned_by_mock.cccd_handle),
                                            LSB(handles_returned_by_mock.sccd_handle),
                                            MSB(handles_returned_by_mock.sccd_handle)};

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)exp_output;
    p_mock_call->arg[2] = sizeof(exp_output);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));



    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();

    ble_gatts_char_handles_t returned_handles;

    memset(&returned_handles, 0, sizeof(returned_handles));

    uint32_t err_code = sd_ble_gatts_characteristic_add(service_handle, &char_md, &attr_char_value, &returned_handles);

    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    CU_ASSERT_INT_EQUAL(returned_handles.value_handle, handles_returned_by_mock.value_handle);
    CU_ASSERT_INT_EQUAL(returned_handles.user_desc_handle, handles_returned_by_mock.user_desc_handle);
    CU_ASSERT_INT_EQUAL(returned_handles.cccd_handle, handles_returned_by_mock.cccd_handle);
    CU_ASSERT_INT_EQUAL(returned_handles.sccd_handle, handles_returned_by_mock.sccd_handle);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();

}
CU_TEST_END



/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_characteristic_add(...)
 * when all pointer input given as function parameters to the API are NULL
 * sd_ble_gatts_characteristic_add(uint16_t                  service_handle,
 *                                 ble_gatts_char_md_t const *const p_char_md,
 *                                 ble_gatts_attr_t const    *const p_attr_char_value,
 *                                 ble_gatts_char_handles_t  *const p_handles)
 */
CU_TEST_START(test_encoder_sd_ble_gatts_characteristic_add_all_input_pointers_NULL)
{
    reset_unit_test();    

    nrf_cunit_mock_call *       p_mock_call;
    uint16_t                    service_handle = 0xAAu;

    // Expected output

    const uint8_t exp_output[] =
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_CHARACTERISTIC_ADD,
        LSB(service_handle),
        MSB(service_handle),
        RPC_BLE_FIELD_NOT_PRESENT,      // Characteristic Metadata not present.
        RPC_BLE_FIELD_NOT_PRESENT,      // Characteristic attribute not present.
        RPC_BLE_FIELD_NOT_PRESENT       // Handles present.
    };


    /*Mock call*/
    ble_gatts_char_handles_t    handles_returned_by_mock;

    handles_returned_by_mock.value_handle        = 0xABCD;
    handles_returned_by_mock.user_desc_handle    = 0xED01;
    handles_returned_by_mock.cccd_handle         = 0x0203;
    handles_returned_by_mock.sccd_handle         = 0x0405;

    const uint8_t cmd_response_message[] = {
                                            BLE_RPC_PKT_RESP,
                                            SD_BLE_GATTS_CHARACTERISTIC_ADD,
                                            0, 0, 0, 0,                 // Return code.
                                            LSB(handles_returned_by_mock.value_handle),
                                            MSB(handles_returned_by_mock.value_handle),
                                            LSB(handles_returned_by_mock.user_desc_handle),
                                            MSB(handles_returned_by_mock.user_desc_handle),
                                            LSB(handles_returned_by_mock.cccd_handle),
                                            MSB(handles_returned_by_mock.cccd_handle),
                                            LSB(handles_returned_by_mock.sccd_handle),
                                            MSB(handles_returned_by_mock.sccd_handle)};

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)exp_output;
    p_mock_call->arg[2] = sizeof(exp_output);

    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));



    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();

    ble_gatts_char_handles_t returned_handles;

    memset(&returned_handles, 0, sizeof(returned_handles));

    uint32_t err_code = sd_ble_gatts_characteristic_add(service_handle, NULL, NULL, NULL);

    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();
}
CU_TEST_END


// Support for sd_ble_gatts_descriptor_add is not present at the moment. Re-introduce this test case
// once this has been added. Also add a test case with all options present.
CU_TEST_START(test_encoder_sd_ble_gatts_descriptor_add_not_supported)
{
    reset_unit_test();

    CU_ASSERT(sd_ble_gatts_descriptor_add(0, NULL, NULL) == NRF_ERROR_NOT_SUPPORTED);
}
CU_TEST_END


#if 0
// Support for sd_ble_gatts_descriptor_add is not present at the moment. Re-introduce this test case
// once this has been added. Also add a test case with all options present. Also remove
// test_encoder_sd_ble_gatts_descriptor_add_not_supported.
CU_TEST_START(test_encoder_sd_ble_gatts_descriptor_add_optionals_absent)
{    
    reset_unit_test();    

    nrf_cunit_mock_call * p_mock_call;     
    
    uint16_t char_handle = 0xAAu;
    ble_gatts_attr_t attr;
    uint16_t handle = 0xAABB;
   
    attr.p_uuid     = NULL;
    attr.p_attr_md  = NULL;
    attr.init_len   = 0xAABB;
    attr.init_offs  = 0xCCDD;
    attr.max_len    = 0xEEFF;
    attr.p_value    = NULL;
    
    handle          = 0xAAu;
 
    const uint8_t  expected[]               = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_DESCRIPTOR_ADD,
        LSB(char_handle), 
        MSB(char_handle),
        /*p_attr*/
        RPC_BLE_FIELD_PRESENT,               /*p_attr*/
          RPC_BLE_FIELD_NOT_PRESENT,         /*p_uuid*/
          RPC_BLE_FIELD_NOT_PRESENT,         /*p_attr_md*/
          LSB(attr.init_len),
          MSB(attr.init_len),
          LSB(attr.init_offs),
          MSB(attr.init_offs),
          LSB(attr.max_len),
          MSB(attr.max_len),
          RPC_BLE_FIELD_NOT_PRESENT,        /*p_value*/
        /*p_handle*/
        RPC_BLE_FIELD_PRESENT,
        LSB(handle),
        MSB(handle),  
    };  

    /*Mock call*/
    const uint8_t cmd_response_message[] = {
                                            BLE_RPC_PKT_RESP,
                                            SD_BLE_GATTS_DESCRIPTOR_ADD,
                                            0, 0, 0, 0
                                           };    
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected);
    
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));



    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_descriptor_add(char_handle, &attr, &handle);

    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);        
    
    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();    

}
CU_TEST_END
#endif

/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_value_set(...)
 * when all pointer input data is valid
 * sd_ble_gatts_value_set(uint16_t handle, 
 *                        uint16_t offset, 
 *                        uint16_t* const p_len, 
 *                        uint8_t const * const p_value)
 */
CU_TEST_START(test_encoder_sd_ble_gatts_value_set)
{
    reset_unit_test();    

    nrf_cunit_mock_call * p_mock_call;
    
    const uint16_t handle                 = 0xAA55u;
    const uint16_t offset                 = 0x33BBu;    
    const uint8_t  buf[]                  = {0xCCu, 0x22u};
    uint16_t       len                    = sizeof(buf);    
    const uint8_t  expected[]             = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_VALUE_SET,
        LSB(handle),
        MSB(handle),
        LSB(offset),
        MSB(offset),  
        RPC_BLE_FIELD_PRESENT,
        LSB(len),
        MSB(len),             
        RPC_BLE_FIELD_PRESENT,        
        buf[0],
        buf[1]
    };  
    const uint8_t  cmd_response_message[] = {
                                             BLE_RPC_PKT_RESP,
                                             SD_BLE_GATTS_VALUE_SET,
                                             0, 0, 0, 0
                                            };                

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected); 
    
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));



    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_value_set(handle, offset, &len, buf);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code); 

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();                   
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_value_set(...)
 * when len field input data is NULL
 * sd_ble_gatts_value_set(uint16_t handle, 
 *                        uint16_t offset, 
 *                        uint16_t* const p_len, 
 *                        uint8_t const * const p_value)
 */
CU_TEST_START(test_encoder_sd_ble_gatts_value_set_null_len)
{
    reset_unit_test();    

    nrf_cunit_mock_call * p_mock_call;
    
    const uint16_t handle     = 0xAA55u;
    const uint16_t offset     = 0x33BBu;    
    const uint8_t  buf[]      = {0xCCu, 0x22u};
    const uint8_t  expected[] = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_VALUE_SET,
        LSB(handle),
        MSB(handle),
        LSB(offset),
        MSB(offset),  
        RPC_BLE_FIELD_NOT_PRESENT,
        // @note: If len field is omitted value field must also be omitted.
        RPC_BLE_FIELD_NOT_PRESENT,
    };  
    const uint8_t  cmd_response_message[] = {
                                             BLE_RPC_PKT_RESP,
                                             SD_BLE_GATTS_VALUE_SET,
                                             0, 0, 0, 0
                                            };                

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected); 
    
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));



    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_value_set(handle, offset, NULL, buf);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code); 

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();                   
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_value_set(...)
 * when value field input data is NULL
 * sd_ble_gatts_value_set(uint16_t handle, 
 *                        uint16_t offset, 
 *                        uint16_t* const p_len, 
 *                        uint8_t const * const p_value)
 */
CU_TEST_START(test_encoder_sd_ble_gatts_value_set_null_value)
{
    reset_unit_test();    

    nrf_cunit_mock_call * p_mock_call;
    
    const uint16_t handle     = 0xAA55u;
    const uint16_t offset     = 0x33BBu;    
    uint16_t       len        = 0;    
    const uint8_t  expected[] = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_VALUE_SET,
        LSB(handle),
        MSB(handle),
        LSB(offset),
        MSB(offset),  
        RPC_BLE_FIELD_PRESENT,
        LSB(len),
        MSB(len),          
        RPC_BLE_FIELD_NOT_PRESENT,
    };  
    const uint8_t  cmd_response_message[] = {
                                             BLE_RPC_PKT_RESP,
                                             SD_BLE_GATTS_VALUE_SET,
                                             0, 0, 0, 0
                                            };                    

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected); 
    
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));



    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_value_set(handle, offset, &len, NULL);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code); 

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();                   
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_value_set(...)
 * when all optional input data is NULL
 * sd_ble_gatts_value_set(uint16_t handle, 
 *                        uint16_t offset, 
 *                        uint16_t* const p_len, 
 *                        uint8_t const * const p_value)
 */
CU_TEST_START(test_encoder_sd_ble_gatts_value_set_null)
{
    reset_unit_test();    

    nrf_cunit_mock_call * p_mock_call;
    
    const uint16_t handle     = 0xAA55u;
    const uint16_t offset     = 0x33BBu;    
    const uint8_t  expected[] = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_VALUE_SET,
        LSB(handle),
        MSB(handle),
        LSB(offset),
        MSB(offset),  
        RPC_BLE_FIELD_NOT_PRESENT,
        RPC_BLE_FIELD_NOT_PRESENT
    };  
    const uint8_t  cmd_response_message[] = {
                                             BLE_RPC_PKT_RESP,
                                             SD_BLE_GATTS_VALUE_SET,
                                             0, 0, 0, 0
                                            };                        

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected); 
    
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));



    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_value_set(handle, offset, NULL, NULL);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code); 

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();                   
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_value_set(...)
 * when input data len value is out-of-bound (size).
 * sd_ble_gatts_value_set(uint16_t handle, 
 *                        uint16_t offset, 
 *                        uint16_t* const p_len, 
 *                        uint8_t const * const p_value)
 */
CU_TEST_START(test_encoder_sd_ble_gatts_value_set_out_of_bound)
{
    reset_unit_test();    

    const uint16_t handle = 0xAA55u;
    const uint16_t offset = 0x33BBu;    
    const uint8_t  buf[]  = {0xCCu, 0x22u};
    uint16_t       len    = 0xFFFFu;    
    
    uint32_t err_code = sd_ble_gatts_value_set(handle, offset, &len, buf);
    CU_ASSERT_INT_EQUAL(NRF_ERROR_DATA_SIZE, err_code); 
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_value_set(...)
 * when all input data is valid and also validates the output data.
 * sd_ble_gatts_value_set(uint16_t handle, 
 *                        uint16_t offset, 
 *                        uint16_t* const p_len, 
 *                        uint8_t const * const p_value)
 */
CU_TEST_START(test_encoder_sd_ble_gatts_value_set_response)
{
    reset_unit_test();    

    nrf_cunit_mock_call * p_mock_call;
    
    const uint16_t handle                = 0xAA55u;
    const uint16_t offset                = 0x33BBu;    
    const uint8_t  buf[]                 = {0xCCu, 0x22u};
    uint16_t       len                   = sizeof(buf);    
    const uint8_t  expected[]            = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_VALUE_SET,
        LSB(handle),
        MSB(handle),
        LSB(offset),
        MSB(offset),  
        RPC_BLE_FIELD_PRESENT,
        LSB(len),
        MSB(len),             
        RPC_BLE_FIELD_PRESENT,        
        buf[0],
        buf[1]
    };  
    const uint16_t bytes_written          = 1u;
    const uint8_t  cmd_response_message[] = 
    {
        BLE_RPC_PKT_RESP,
        SD_BLE_GATTS_VALUE_SET,
        0, 0, 0, 0, 
        LSB(bytes_written),
        //lint -e(572) "Excessive shift value"        
        MSB(bytes_written)
    };        
    
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected); 
    
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));



    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_value_set(handle, offset, &len, buf);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code); 
    CU_ASSERT_INT_EQUAL(len, 
        uint16_decode(&(cmd_response_message[BLE_OP_CODE_SIZE  +
                                             BLE_PKT_TYPE_SIZE +
                                             RPC_ERR_CODE_SIZE])));

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();                   
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_sys_attr_get(...)
 * when all optional input data is present.
 * sd_ble_gatts_sys_attr_get(uint16_t         conn_handle, 
 *                           uint8_t  * const p_sys_attr_data, 
 *                           uint16_t * const p_len)
 */
CU_TEST_START(test_encoder_sd_ble_gatts_sys_attr_get)
{
    reset_unit_test();    

    nrf_cunit_mock_call * p_mock_call;
    
    const uint16_t handle                     = 0x55AAu;
    uint8_t        sys_attr_data[10u]         = {0};
    uint16_t       len                        = sizeof(sys_attr_data);
    const uint8_t  expected[]                 = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_SYS_ATTR_GET,
        LSB(handle),
        MSB(handle),
        RPC_BLE_FIELD_PRESENT,        
        LSB(len),
        MSB(len),        
        RPC_BLE_FIELD_PRESENT        
    };        
    const uint8_t  cmd_response_message[]     = {
                                                 BLE_RPC_PKT_RESP,
                                                 SD_BLE_GATTS_SYS_ATTR_GET,
                                                 0, 0, 0, 0
                                                };        

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected); 
    
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));


    
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_sys_attr_get(handle, sys_attr_data, &len);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);
    
    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();                           
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_sys_attr_get(...)
 * when len input data is NULL.
 * sd_ble_gatts_sys_attr_get(uint16_t         conn_handle, 
 *                           uint8_t  * const p_sys_attr_data, 
 *                           uint16_t * const p_len)
 */
CU_TEST_START(test_encoder_sd_ble_gatts_sys_attr_get_len_omitted)
{
    reset_unit_test();    

    nrf_cunit_mock_call * p_mock_call;
    
    const uint16_t handle                     = 0x55AAu;
    uint8_t        sys_attr_data[10u]         = {0};
    const uint8_t  expected[]                 = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_SYS_ATTR_GET,
        LSB(handle),
        MSB(handle),
        RPC_BLE_FIELD_NOT_PRESENT,        
        RPC_BLE_FIELD_PRESENT        
    };    
    const uint8_t  cmd_response_message[]     = {
                                                 BLE_RPC_PKT_RESP,
                                                 SD_BLE_GATTS_SYS_ATTR_GET,
                                                 0, 0, 0, 0
                                                };            
        
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected); 
    
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));


    
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_sys_attr_get(handle, sys_attr_data, NULL);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);
    
    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();                               
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_sys_attr_get(...)
 * when attributes input data is NULL.
 * sd_ble_gatts_sys_attr_get(uint16_t         conn_handle, 
 *                           uint8_t  * const p_sys_attr_data, 
 *                           uint16_t * const p_len)
 */
CU_TEST_START(test_encoder_sd_ble_gatts_sys_attr_get_attributes_omitted)
{
    reset_unit_test();    

    nrf_cunit_mock_call * p_mock_call;
    
    const uint16_t handle                     = 0x55AAu;
    uint16_t       len                        = 0x33CCu;
    const uint8_t  expected[]                 = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_SYS_ATTR_GET,
        LSB(handle),
        MSB(handle),
        RPC_BLE_FIELD_PRESENT,        
        LSB(len),
        MSB(len),        
        RPC_BLE_FIELD_NOT_PRESENT        
    };    
    const uint8_t  cmd_response_message[]     = {
                                                 BLE_RPC_PKT_RESP,
                                                 SD_BLE_GATTS_SYS_ATTR_GET,
                                                 0, 0, 0, 0
                                                };            
        
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected); 
    
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));


    
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_sys_attr_get(handle, NULL, &len);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);
    
    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();                               
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_sys_attr_get(...)
 * when all optional input data is NULL.
 * sd_ble_gatts_sys_attr_get(uint16_t         conn_handle, 
 *                           uint8_t  * const p_sys_attr_data, 
 *                           uint16_t * const p_len)
 */
CU_TEST_START(test_encoder_sd_ble_gatts_sys_attr_get_optional_input_omitted)
{
    reset_unit_test();    

    nrf_cunit_mock_call * p_mock_call;
    
    const uint16_t handle                     = 0x55AAu;
    const uint8_t  expected[]                 = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_SYS_ATTR_GET,
        LSB(handle),
        MSB(handle),
        RPC_BLE_FIELD_NOT_PRESENT,        
        RPC_BLE_FIELD_NOT_PRESENT        
    };    
    const uint8_t  cmd_response_message[]     = {
                                                 BLE_RPC_PKT_RESP,
                                                 SD_BLE_GATTS_SYS_ATTR_GET,
                                                 0, 0, 0, 0
                                                };            
        
    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected); 
    
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));


    
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_sys_attr_get(handle, NULL, NULL);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);
    
    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();                               
}
CU_TEST_END


/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_sys_attr_get(...),
 * when all optional input data is valid, and the response, when all optional output params are 
 * present.
 * sd_ble_gatts_sys_attr_get(uint16_t         conn_handle, 
 *                           uint8_t  * const p_sys_attr_data, 
 *                           uint16_t * const p_len)
 */
CU_TEST_START(test_encoder_sd_ble_gatts_sys_attr_get_default_output)
{
    reset_unit_test();    

    nrf_cunit_mock_call * p_mock_call;
    
    const uint16_t handle                     = 0x55AAu;
    uint8_t        sys_attr_data[10u]         = {0};
    uint16_t       len                        = sizeof(sys_attr_data);
    const uint8_t  expected[]                 = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_SYS_ATTR_GET,
        LSB(handle),
        MSB(handle),
        RPC_BLE_FIELD_PRESENT,        
        LSB(len),
        MSB(len),        
        RPC_BLE_FIELD_PRESENT        
    };      
    const uint16_t response_length            = 4u;
    const uint8_t  cmd_response_message[]     = 
    {
        BLE_RPC_PKT_RESP,
        SD_BLE_GATTS_SYS_ATTR_GET,
        0, 0, 0, 0,
        LSB(response_length),
        //lint -e(572) "Excessive shift value"        
        MSB(response_length),        
        RPC_BLE_FIELD_PRESENT,                
        0xAAu, 0x55u, 0x33u, 0xCCu
    };        

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected); 
    
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));


    
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_sys_attr_get(handle, sys_attr_data, &len);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);
    CU_ASSERT_INT_EQUAL(len, response_length);    
    CU_ASSERT(memcmp(
        sys_attr_data, 
        &(cmd_response_message[
                               BLE_OP_CODE_SIZE        +
                               BLE_PKT_TYPE_SIZE       +
                               RPC_ERR_CODE_SIZE       +
                               sizeof(response_length) +
                               RPC_BLE_FIELD_LEN]),
        response_length) == 0); 
            
    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();                           
}
CU_TEST_END


/** systems attribute data
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_sys_attr_get(...),
 * when 'systems attribute data' field is set as not present in input data, and the response, when 
 * no optional output params are present.
 * sd_ble_gatts_sys_attr_get(uint16_t         conn_handle, 
 *                           uint8_t  * const p_sys_attr_data, 
 *                           uint16_t * const p_len)
 */
CU_TEST_START(test_encoder_sd_ble_gatts_sys_attr_get_no_conditional_output)
{
    reset_unit_test();    

    nrf_cunit_mock_call * p_mock_call;
    
    const uint16_t handle                     = 0x55AAu;
    uint16_t       len                        = 0;
    const uint8_t  expected[]                 = 
    {
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_SYS_ATTR_GET,
        LSB(handle),
        MSB(handle),
        RPC_BLE_FIELD_PRESENT,        
        LSB(len),
        MSB(len),        
        RPC_BLE_FIELD_NOT_PRESENT        
    };      
    const uint16_t response_length            = 0xFFFFu;
    const uint8_t  cmd_response_message[]     = 
    {
        BLE_RPC_PKT_RESP,
        SD_BLE_GATTS_SYS_ATTR_GET,
        0, 0, 0, 0,
        LSB(response_length),
        MSB(response_length),        
        RPC_BLE_FIELD_NOT_PRESENT
    };        

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t)expected;
    p_mock_call->arg[2] = sizeof(expected); 
    
    set_cmd_response((uint8_t *)cmd_response_message, sizeof(cmd_response_message));


    
    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t)cmd_response_message;

    setup_cmd_encoder();
    
    uint32_t err_code = sd_ble_gatts_sys_attr_get(handle, NULL, &len);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);
    CU_ASSERT_INT_EQUAL(len, response_length);    
            
    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();                           
}

CU_TEST_END

/**
 * This test will verify the encoding of the SoftDevice call: sd_ble_gatts_value_get(...)
 * when all pointer input data is valid
 * sd_ble_gatts_value_get(uint16_t handle, 
 *                        uint16_t offset, 
 *                        uint16_t* const p_len, 
 *                        uint8_t const * const p_data)
 */
CU_TEST_START(test_encoder_sd_ble_gatts_value_get) {
    reset_unit_test();

    nrf_cunit_mock_call * p_mock_call;

    const uint16_t handle = 0xAA55u;
    const uint16_t offset = 0x33BBu;
    uint8_t buf[] = {0x00, 0x00, 0x00};
    uint16_t len = 3;
    
    const uint8_t expected[] ={
        BLE_RPC_PKT_CMD,
        SD_BLE_GATTS_VALUE_GET,
        LSB(handle),
        MSB(handle),
        LSB(offset),
        MSB(offset),
        RPC_BLE_FIELD_PRESENT,
        LSB(len),
        MSB(len),
        RPC_BLE_FIELD_PRESENT
    };
    const uint8_t cmd_response_message[] = {
        BLE_RPC_PKT_RESP,
        SD_BLE_GATTS_VALUE_GET,
        0, 0, 0, 0, 0x03, 0x00, 0x03, 0x00, 0xa1, 0xb2, 0xc3, 
    };

    nrf_cunit_expect_call_return((uint8_t*) "hci_transport_pkt_write", &p_mock_call);
    p_mock_call->arg[1] = (uint32_t) expected;
    p_mock_call->arg[2] = sizeof (expected);

    set_cmd_response((uint8_t *) cmd_response_message, sizeof (cmd_response_message));

    nrf_cunit_expect_call_return((uint8_t *) "hci_transport_rx_pkt_consume", &p_mock_call);
    p_mock_call->arg[0] = (uint32_t) cmd_response_message;

    setup_cmd_encoder();

    uint32_t err_code = sd_ble_gatts_value_get(handle, offset, &len, buf);
    CU_ASSERT_INT_EQUAL(NRF_SUCCESS, err_code);

    // ----- Verifying the mock calls.
    nrf_cunit_verify_call_return();
}
CU_TEST_END


CU_SUITE_START_PUBLIC(ble_cmd_encoder_gatts_encode_commands)
{
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_service_add);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_service_add_null_uuid);    
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_characteristic_add_all_params_present);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_characteristic_add_all_pointer_params_NULL);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_characteristic_add_all_input_pointers_NULL);
//    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_descriptor_add_optionals_absent);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_descriptor_add_not_supported);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_service_add_null_handle);                
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_service_add_no_optional);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_sys_attr_set);    
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_sys_attr_set_no_optional);        
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_sys_attr_set_out_of_bound);            
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_hvx_all_optionals);        
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_hvx_no_optionals);            
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_hvx_no_len_field);                    
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_hvx_no_data_field);        
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_hvx_out_of_bound);            
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_hvx_write_length);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_value_set);    
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_value_set_null_len);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_value_set_null_value);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_value_set_null);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_value_set_out_of_bound);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_value_set_response);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_sys_attr_get);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_sys_attr_get_len_omitted);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_sys_attr_get_attributes_omitted);    
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_sys_attr_get_optional_input_omitted);    
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_sys_attr_get_default_output);    
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_sys_attr_get_no_conditional_output);
    CU_SUITE_ADD_TEST(test_encoder_sd_ble_gatts_value_get); 
}
CU_SUITE_END

