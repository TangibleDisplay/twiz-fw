#ifndef TWI_ERROR_H
#define TWI_ERROR_H

#include <stdint.h>

#include "app_error.h"

/*
 * Set to 1 if the Twi should reset on failure. Else it will stop in a debug loop.
 */
#define RESET_ON_ERROR 0

typedef uint32_t err_t;

/*
 * Test a line of code and return the error if not a success.
 */
#define ERR_RETURN(LINE)				\
  do {						\
    err_t err_code;				\
    err_code = LINE;				\
    if (err_code != NRF_SUCCESS) return err_code;  \
  } while(0)


/*
 * Test a line of code and call the error handler if necessary.
 */
#define ERR_CHECK(LINE)				\
  do {						\
    err_t err_code;				\
    err_code = LINE;				\
    APP_ERROR_CHECK(err_code);			\
  } while(0)


/*
 * Value used as error code on stack dump, can be used to identify stack location on
 * stack unwind.
 */
#define DEAD_BEEF                       0xDEADBEEF


/*
 * Function for error handling, which is called when an error has occurred.
 * Currently, the stack is written to flash on an error. This is useful for debug only!!
 * error_code  Error code supplied to the handler.
 * line_num    Line number where the handler is called.
 * p_file_name Pointer to the file name.
 */
void app_error_handler(err_t error_code, uint32_t line_num, const uint8_t * p_file_name);


/*
 * Callback function for asserts in the SoftDevice.
 * This function will be called in case of an assert in the SoftDevice.
 * Currently, the stack is written to flash on an error. This is useful for debug only!!
 * line_num   Line number of the failing ASSERT call.
 * file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name);

#endif
