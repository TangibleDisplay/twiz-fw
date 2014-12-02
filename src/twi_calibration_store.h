#ifndef TWI_CALIBRATION_STORE_H
#define TWI_CALIBRATION_STORE_H

#include <stdint.h>
#include "pstorage.h"
#include "imu.h"

void calibration_store_init(void);
bool calibration_store_load(calibration_data_t *data);
void calibration_store_write(const calibration_data_t *cal_data);

#endif
