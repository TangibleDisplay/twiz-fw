#include <stdint.h>
#include <stdbool.h>

#include "uart.h"
#include "boards.h"
#include "nrf_delay.h"

#ifdef UART_IRQ

#include "nordic_common.h"
#include "app_uart.h"

static void uart_evt_handler(app_uart_evt_t * p_app_uart_event)
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

int putchar(int c)
{
    while (app_uart_put(c) != NRF_SUCCESS);
    return 0;
}

int getchar()
{
    uint8_t c;
    while(app_uart_get(&c) != NRF_SUCCESS);
    return c;
}

// Return true is byte has been received, false on timeout
bool getchar_timeout(uint32_t timeout_ms, char *c)
{
    while (timeout_ms--) {
        if (app_uart_get((uint8_t *)c) == NRF_SUCCESS)
            return true;
        nrf_delay_ms(1);
    }
    return false;
}

#else

#include "simple_uart.h"
void uart_init()
{
    simple_uart_config(UART_RTS_PIN, UART_TX_PIN, UART_CTS_PIN, UART_RX_PIN, false);
    nrf_delay_ms(300);
}

int putchar(int c)
{
    simple_uart_put(c);
    return 0;
}

int getchar() {
    return simple_uart_get();
}

// Return NRF_SUCCESS on success, -1 on timeout
bool getchar_timeout(uint32_t timeout_ms, char *c)
{
    return simple_uart_get_with_timeout(timeout_ms, (uint8_t *)c);
}

#endif
