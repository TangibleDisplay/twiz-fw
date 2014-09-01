#ifndef HAL_H
#define HAL_H

#include "nrf_delay.h"      // nrf_delay_ms()
#include "hardware.h"       // get_ms()
#include "mpu.h"            // i2c_read() & i2c_write()
#include <chprintf.h>       // for chsnprintf()
#include "simple_uart.h"    // simple_uart_putstring()
#include "ble_uart.h"       // TODO later: use print(), printInt() & printHex()

#define USE_SIMPLE_UART true
#if USE_SIMPLE_UART
static void simple_uart_getstring(char *s) {
    if (!s) return;
    do { *s = simple_uart_get(); }
    while ( *s++ );
}
#define uart_init() simple_uart_config(3,0,2,1,0) /*rts, tx, cts, rx, flow-ctrl*/
#define LOG_STR(s)  simple_uart_putstring((const uint8_t *)(s))
#define GET_STR(s)  simple_uart_getstring(s)
#define GET_C       simple_uart_get
#else // BLE_UART
#define uart_init() /* TODO ?*/
#define LOG_STR(s)  print(s)
#define GET_STR(s)  get_str(s)
#define GET_C       /* TODO ?*/
#endif

#define LOG(...)    { char s[128]; chsnprintf(s, 128, __VA_ARGS__); LOG_STR(s); }
                    /* TODO: test if 128 is enough once the fusion is finished */

#define log_e(...)  LOG(__VA_ARGS__)
#define log_i(...)  LOG(__VA_ARGS__)

#define delay_ms    nrf_delay_ms
#define reg_int_cb  /* TODO: register interrupt call back function */
                    /* int reg_int_cb(struct int_param_s *int_param); */

#define min(a,b)    ((a<b)?a:b)
#define abs(x)      (((x)>0)?(x):-(x))
#define labs(x)     abs(x) /*long*/
#define fabs(x)     abs(x) /*double*/
#define fabsf(x)    abs(x) /*float*/

#define __no_operation __NOP

#endif // HAL_H

