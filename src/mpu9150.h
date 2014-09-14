#ifndef MPU9150_H
#define MPU9150_H

#include <stdint.h>
#include <stdbool.h>

void mpu9150_reset(void);
void mpu9150_init(void);
void mpu9150_read_data(int16_t * values);
void mpu9150_calibrate(void);
bool mpu9150_new_data();

#endif
