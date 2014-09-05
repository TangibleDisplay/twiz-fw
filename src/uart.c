#include <stdint.h>
#include <stdbool.h>

#include "uart.h"
#include "boards.h"

#ifdef UART_IRQ

#include "nordic_common.h"
#include "app_uart.h"

void uart_evt_handler(app_uart_evt_t * p_app_uart_event)
{
    UNUSED_PARAMETER(p_app_uart_event);
}

void uart_init()
{
    static uint32_t err_code;

    // Init UART : 115200, no flow control
    app_uart_comm_params_t uart_params = {
        .rx_pin_no = UART_RX_PIN,
        .tx_pin_no = UART_TX_PIN,
        .flow_control = APP_UART_FLOW_CONTROL_DISABLED,
        .use_parity = false,
        .baud_rate = UART_BAUDRATE_BAUDRATE_Baud115200,
    };
    APP_UART_FIFO_INIT(&uart_params, 128, 128, uart_evt_handler, APP_IRQ_PRIORITY_HIGH, err_code);
    APP_ERROR_CHECK(err_code);
}

#else

#include "simple_uart.h"
void uart_init()
{
    simple_uart_config(UART_RTS_PIN, UART_TX_PIN, UART_CTS_PIN, UART_RX_PIN, false);
}

#endif
