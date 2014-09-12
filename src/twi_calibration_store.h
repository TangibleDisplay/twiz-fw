#ifndef TWI_CALIBRATION_STORE_H
#define TWI_CALIBRATION_STORE_H

#include <stdint.h>
#include "pstorage.h"

typedef struct {
    uint32_t magic;
    float gyro_bias[3];
    float accel_bias[3];
    float mag_bias[3];
    float mag_calibration[3];
} calibration_data_t;

void calibration_store_init(void);
uint32_t calibration_store_load(calibration_data_t *data);
uint32_t calibration_store_write(const calibration_data_t *cal_data);

#endif
