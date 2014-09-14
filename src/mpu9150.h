#ifndef MPU9150_H
#define MPU9150_H

#include <stdint.h>
#include <stdbool.h>

void mpu9150_reset(void);
void mpu9150_init(void);
void mpu9150_read_data(float * values);
void mpu9150_measure_biases(void);
bool mpu9150_new_data();

#endif
