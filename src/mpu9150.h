#ifndef MPU9150_H
#define MPU9150_H

typedef struct imu_data_s {
    int16_t imu[4]; // w/h, x, y, z (quaternions / heading+accel3d)
} imu_data_t;

void mpu9150_update(void);

void imu_init(void);
imu_data_t * get_imu_data(imu_data_t * imu_data);

#endif
