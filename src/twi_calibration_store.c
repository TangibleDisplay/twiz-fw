
#include <stdint.h>
#include "twi_error.h"
#include "pstorage.h"
#include "twi_calibration_store.h"
#include "twi_error.h"
#include "nordic_common.h"
#include "string.h"
#include "printf.h"

#define MAGIC1 0xB28AD7CE
#define MAGIC2 0x3827BEDA
#define ROUND_UP(N, S) ((((N) + (S) - 1) / (S)) * (S))
#define DATA_SIZE ROUND_UP((MAX(sizeof cal, PSTORAGE_MIN_BLOCK_SIZE)), 4)

// Last operation status
static uint32_t status;
// Flash handle
static pstorage_handle_t flash_handle;

// Flash ops callback : store last operation status in global variable status
static void flash_pstorage_cb(pstorage_handle_t * handle,
                              uint8_t             op_code,
                              uint32_t            result,
                              uint8_t           * p_data,
                              uint32_t            param_len)
{
    status = result;
}


// Init calibration store
void calibration_store_init()
{
    // Register blocks and params to access flash
    pstorage_module_param_t param;

    param.block_size  = DATA_SIZE;
    param.block_count = 1;
    param.cb          = flash_pstorage_cb;

    APP_ERROR_CHECK(pstorage_register(&param, &flash_handle));
}


// Load calibration data. Verify that data is valid (magic is correct).
// Returns 0 on success, -1 on invalid magic.
uint32_t calibration_store_load(calibration_data_t *data)
{
    APP_ERROR_CHECK(pstorage_load((uint8_t *)data,
                                  &flash_handle,
                                  DATA_SIZE,
                                  0));
    if (data->magic1 != MAGIC1) {
        printf("Flash calibration data read : invalid magic1. Falling back to factory values.\r\n");
        printf("YOU SHOULD SERIOUSLY CONSIDER CALIBRATING THE IMU !!!\r\n");
        return -1;
    }

    if (data->magic2 != MAGIC2) {
        printf("Flash calibration data read : invalid magic2. Falling back to factory values.\r\n");
        printf("YOU SHOULD SERIOUSLY CONSIDER CALIBRATING THE IMU !!!\r\n");
        return -1;
    }

    return 0;
}

// Store calibration data in flash
uint32_t calibration_store_write(const calibration_data_t *cal_data) {
    uint32_t count;

    // First, make a static copy of calibration data (which shall be persistent during flash write)
    static calibration_data_t data;
    memcpy((void*)&data, (void*)cal_data, sizeof cal_data);

    // Ensure magic is correct
    data.magic1 = MAGIC1;
    data.magic2 = MAGIC2;

    // Erase flash block
    pstorage_clear(&flash_handle, DATA_SIZE);

    // Wait end of erase
    do {
        pstorage_access_status_get(&count);
    } while (count != 0);
    APP_ERROR_CHECK(status);

    // Write block
    pstorage_store(&flash_handle, (uint8_t *)&data, DATA_SIZE, 0);

    // Wait for end of write
    do {
        pstorage_access_status_get(&count);
    } while (count != 0);
    APP_ERROR_CHECK(status);

    return 0;
}
