/** PIN defines for TWIMU board.
    Is needed by several app_* SDK functions.
*/

#ifndef TWIMU_BOARDS_H__
#define TWIMU_BOARDS_H__

#include "nrf_gpio.h"

#define UART_TX_PIN                         0
#define UART_RX_PIN                         1
#define UART_CTS_PIN                        2 /* unused */
#define UART_RTS_PIN                        3 /* unused */
#define TWI_MASTER_CONFIG_DATA_PIN_NUMBER   (4u)
#define TWI_MASTER_CONFIG_CLOCK_PIN_NUMBER  (5u)
#define LED_0                               13


#endif
