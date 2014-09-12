#ifndef TWI_GAP_H
#define TWI_GAP_H

#include "ble_gap.h"

/*
 * Timeout for Pairing Request or Security Request (in seconds).
 */
#define SEC_PARAM_TIMEOUT               30
/*
 * Perform bonding.
 */
#define SEC_PARAM_BOND                  1
/*
 * Man In The Middle protection not required.
 */
#define SEC_PARAM_MITM                  0
/*
 * No I/O capabilities.
 */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE
/*
 * Out Of Band data not available.
 */
#define SEC_PARAM_OOB                   0
/*
 * Minimum encryption key size.
 */
#define SEC_PARAM_MIN_KEY_SIZE          7
/*
 * Maximum encryption key size.
 */
#define SEC_PARAM_MAX_KEY_SIZE          16


/*
 * Security requirements for this application.
 */
extern ble_gap_sec_params_t m_sec_params;


/*
 * Function for initializing security parameters.
 */
void sec_params_init(void);


/*
 * Function for the GAP initialization.
 * This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 * device including the device name, appearance, and the preferred connection parameters.
 */
void gap_params_init(void);


#endif
