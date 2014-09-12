#ifndef TWI_BLE_EVT_H
#define TWI_BLE_EVT_H

#include "ble.h"

/*
 * Maximum duration of a connection (in seconds).
 */
#define MAX_CONN_TIME 60


/*
 * Initialize the connection timer.
 */
void twi_ble_evt_init(void);


/*
 * Function for dispatching a BLE stack event to all modules with a BLE stack event handler.
 * If new services need to handle BLE event, add them here.
 * p_ble_evt   Bluetooth stack event.
 */
void ble_evt_dispatch(ble_evt_t * p_ble_evt);


#endif
