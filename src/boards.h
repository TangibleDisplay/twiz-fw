/** PIN defines for TWIMU board.
    Is needed by several app_* SDK functions.
*/

#ifndef BOARDS_H
#define BOARDS_H

#include "nrf_gpio.h"

// IHM:
#define BUTTON          29       /* connect to VCC when pressed */
#define LED_0           12       /* ! INVERTED LOGIC ! */
#define LED_1           13       /* ! INVERTED LOGIC ! */
#define LED_2           14       /* ! INVERTED LOGIC ! */
// aliases:
#define LED_R           LED_0
#define LED_G           LED_1
#define LED_B           LED_2
#define LED             LED_G    /* default green */

// IMU:
#define I2C_SDA         (4u)
#define I2C_SCL         (5u)
#define I2C_INT         (6u)
// Aliases:
#define TWI_MASTER_CONFIG_DATA_PIN_NUMBER   I2C_SDA
#define TWI_MASTER_CONFIG_CLOCK_PIN_NUMBER  I2C_SCL
#define TWI_MASTER_CONFIG_INT_PIN_NUMBER    I2C_INT

// Analog Inputs:
#define AREF            0
#define AIN0            26
#define AIN1            27
#define AIN2            1

// UART:
#define TEST_UART_TX_PIN 15      /* 3V */
#define TEST_UART_RX_PIN 16      /* 3V */
// for retro compatibility
#define UART_TX_PIN     AREF     /* 3V */
#define UART_RX_PIN     AIN2     /* 3V */
#define UART_CTS_PIN    2        /* not connected, for retro-compatibility only */
#define UART_RTS_PIN    3        /* not connected, for retro-compatibility only */
#define HWFC            false    /* no hardware flow control */

// Extra GPIOs
#define GPIO8           8
#define GPIO9           9
#define GPIO10          10
#define GPIO11          11

#endif

