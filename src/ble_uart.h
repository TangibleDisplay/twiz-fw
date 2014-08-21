#ifndef BLE_UART_H
#define BLE_UART_H

#include <stdint.h>
#include <stdbool.h>
#include "ble_conn_params.h"
#include "ble_nus.h"

void printChar(char c);
void print(char* s);
void printInt(uint32_t n);
void printHex(uint8_t n);

bool bleUartConnected();

void gap_params_init(void);
void advertising_init(void);
void nus_data_handler(ble_nus_t* p_nus, uint8_t* p_data, uint16_t length);
void services_init(void);
void sec_params_init(void);
void on_conn_params_evt(ble_conn_params_evt_t* p_evt);
void conn_params_error_handler(uint32_t nrf_error);
void conn_params_init(void);
void advertising_start(void);
void on_ble_evt(ble_evt_t* p_ble_evt);
void ble_evt_dispatch(ble_evt_t* p_ble_evt);
void ble_stack_init(void);
void power_manage(void);

#endif // BLE_UART_H
