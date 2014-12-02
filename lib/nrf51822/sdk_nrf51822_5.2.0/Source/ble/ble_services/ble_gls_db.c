/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 */

#include "ble_gls_db.h"


typedef struct
{
    bool          in_use_flag;
    ble_gls_rec_t record;
} database_entry_t;
  
static database_entry_t m_database[BLE_GLS_DB_MAX_RECORDS];
static uint8_t          m_database_crossref[BLE_GLS_DB_MAX_RECORDS];
static uint16_t         m_num_records;


uint32_t ble_gls_db_init(void)
{
    int i;
  
    for (i = 0; i < BLE_GLS_DB_MAX_RECORDS; i++)
    {
        m_database[i].in_use_flag = false;
        m_database_crossref[i]    = 0xFF;
    }

    m_num_records = 0;
    
    return NRF_SUCCESS;
}


uint16_t ble_gls_db_num_records_get(void)
{
    return m_num_records;
}


uint32_t ble_gls_db_record_get(uint8_t rec_ndx, ble_gls_rec_t * p_rec)
{
    if (rec_ndx >= m_num_records)
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    // copy record to the specified memory
    *p_rec = m_database[m_database_crossref[rec_ndx]].record;

    return NRF_SUCCESS;
}


uint32_t ble_gls_db_record_add(ble_gls_rec_t * p_rec)
{
    int i;
  
    if (m_num_records == BLE_GLS_DB_MAX_RECORDS)
    {
        return NRF_ERROR_NO_MEM;
    }
    
    // find next available database entry
    for (i = 0; i < BLE_GLS_DB_MAX_RECORDS; i++)
    {
        if (!m_database[i].in_use_flag)
        {
            m_database[i].in_use_flag = true;
            m_database[i].record      = *p_rec;
              
            m_database_crossref[m_num_records] = i;
            m_num_records++;
    
            return NRF_SUCCESS;
        }
    }

    return NRF_ERROR_NO_MEM;
}


uint32_t ble_gls_db_record_delete(uint8_t rec_ndx)
{
    int i;
  
    if (rec_ndx >= m_num_records)
    {
        return NRF_ERROR_NOT_FOUND;
    }
    
    // free entry
    m_database[m_database_crossref[rec_ndx]].in_use_flag = false;
  
    // decrease number of records
    m_num_records--;

    // remove cross reference index
    for (i = rec_ndx; i < m_num_records; i++)
    {
        m_database_crossref[i] = m_database_crossref[i + 1];
    }
    
    return NRF_SUCCESS;
}
