#ifndef MPU9150_H
#define MPU9150_H

#include <stdint.h>

typedef struct imu_data_s {
    int16_t accel[3]; // x, y, z
    int16_t euler[3]; // yaw, pitch, roll
} imu_data_t;

void mpu9150_update(void);

void imu_init(void);
imu_data_t * get_imu_data(imu_data_t * imu_data);

#endif
