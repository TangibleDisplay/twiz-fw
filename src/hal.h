#ifndef HAL_H
#define HAL_H

#include "nrf_delay.h"      // nrf_delay_ms()
#include "hardware.h"       // get_ms()
#include "mpu.h"            // i2c_read() & i2c_write()
#include <stdlib.h>         // for sprintf()
#include "simple_uart.h"    // simple_uart_putstring()
//#include "ble_uart.h"       // TODO later: use print() & printInt()

#define STR(s)      (const uint8_t *)(s)
#define LOG(s)      simple_uart_putstring(STR(s))
#define log_e(s)    LOG(s)
#define log_i(...)  { char s[99]; sprintf(s, __VA_ARGS__); LOG(STR(s)); }
                    /* s[81] should suffice but this litle margin can't hurt
                     * */

#define delay_ms    nrf_delay_ms
#define reg_int_cb  /* TODO: register interrupt call back function */
                    /* int reg_int_cb(struct int_param_s *int_param); */

#define min(a,b)    ((a<b)?a:b)
#define abs(x)      (((x)>0)?(x):-(x))
#define labs(x)     abs(x) /*long*/
#define fabs(x)     abs(x) /*double*/
#define fabsf(x)    abs(x) /*float*/

#endif // HAL_H

