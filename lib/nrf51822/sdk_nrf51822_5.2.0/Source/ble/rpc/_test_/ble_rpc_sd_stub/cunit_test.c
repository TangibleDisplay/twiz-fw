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
 #include <string.h>
#endif
#include <stdbool.h>
#include <stdlib.h>
#include "nrf_cunit.h"
#include "nrf_error.h"
#include "nrf_soc.h"


NVIC_Type _nvic;

static uint8_t m_wfe_count;
static uint8_t m_sev_count;
static bool    m_interrupt_called;


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


void SWI2_IRQHandler(void)
{
    m_interrupt_called = true;
}


static void reset_test(void)
{
    m_wfe_count = 0;
    m_sev_count = 0;
    m_interrupt_called = false;
}


void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_nam)
{
    CU_ASSERT(false);
}


// This test case is intended to test that we get call the three functions needed for this
// function to work properly. Only the number of times the different functions is called will be
// tested, not the functionality of the called functions.
CU_TEST_START(verify_that_sd_app_event_wait_calls_WFE_and_SEV)
{
    reset_test();
    uint32_t err_code = sd_app_evt_wait();

    CU_ASSERT(err_code == NRF_SUCCESS);

    CU_ASSERT(m_wfe_count == 2);
    CU_ASSERT_INT_EQUAL(m_wfe_count, 2);
    CU_ASSERT(m_sev_count == 1);
    CU_ASSERT_INT_EQUAL(m_sev_count, 1);
}
CU_TEST_END


CU_SUITE_START(sd_app_event_wait_suite)
{
    CU_SUITE_ADD_TEST(verify_that_sd_app_event_wait_calls_WFE_and_SEV);
}
CU_SUITE_END


CU_REGISTRY_START_STANDALONE(TC_CT_FUN_BV_UNIT_SD_STUB)
{
    CU_REGISTRY_ADD_SUITE(sd_app_event_wait_suite);
}
CU_REGISTRY_END


int main(void)
{
    CU_ADD_REGISTRY(TC_CT_FUN_BV_UNIT_SD_STUB);
    return 0;
}
