#include <stdint.h>
#include <stdbool.h>

#include "nordic_common.h"
#include "softdevice_handler.h"
#include "app_util.h"
#include "timers.h"
#include "leds.h"
#include "app_uart.h"
#include "app_gpiote.h"
#include "printf.h"


void uart_evt_handler(app_uart_evt_t * p_app_uart_event)
{
    UNUSED_VARIABLE(p_app_uart_event);
}


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    static uint32_t err_code;

    // Init LEDs
    leds_init();

    // Init SD so that timers have a clock reference
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_SYNTH_250_PPM, false);

    // Init timers
    timers_init();

    // Init UART
    APP_GPIOTE_INIT(1);
    app_uart_comm_params_t uart_params = {
        .rx_pin_no = 1,
        .tx_pin_no = 0,
        .flow_control = APP_UART_FLOW_CONTROL_DISABLED,
        .use_parity = false,
        .baud_rate = UART_BAUDRATE_BAUDRATE_Baud115200,
    };
    APP_UART_FIFO_INIT(&uart_params, 128, 128, uart_evt_handler, APP_IRQ_PRIORITY_LOW, err_code);
    APP_ERROR_CHECK(err_code);

    while(1) {
        leds_blink(500);
        static uint32_t t;
        get_ms(&t);
        printf("Hello float=%.20f, time=%d\r\n", 1.123456789987654321, t);
    }
    return 0;
}
