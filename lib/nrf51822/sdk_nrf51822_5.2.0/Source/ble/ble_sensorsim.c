/* Copyright (c) 2012 Nordic Semiconductor. All Rights Reserved.
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

#include "ble_sensorsim.h"


void ble_sensorsim_init(ble_sensorsim_state_t *     p_state,
                        const ble_sensorsim_cfg_t * p_cfg)
{
    if (p_cfg->start_at_max)
    {
        p_state->current_val   = p_cfg->max;
        p_state->is_increasing = false;
    }
    else
    {
        p_state->current_val   = p_cfg->min;
        p_state->is_increasing = true;
    }
}


uint32_t ble_sensorsim_measure(ble_sensorsim_state_t *     p_state,
                               const ble_sensorsim_cfg_t * p_cfg)
{
    if (p_state->is_increasing)
    {
        if (p_cfg->max - p_state->current_val > p_cfg->incr)
        {
            p_state->current_val += p_cfg->incr;
        }
        else
        {
            p_state->current_val   = p_cfg->max;
            p_state->is_increasing = false;
        }
    }
    else
    {
        if (p_state->current_val - p_cfg->min > p_cfg->incr)
        {
            p_state->current_val -= p_cfg->incr;
        }
        else
        {
            p_state->current_val   = p_cfg->min;
            p_state->is_increasing = true;
        }
    }
    return p_state->current_val;
}
