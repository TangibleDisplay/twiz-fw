#ifndef MPU_H
#define MPU_H

typedef struct imu_data_s {
    int16_t accel[3]; // x, y, z
    int16_t euler[3]; // yaw, pitch, roll
} imu_data_t;

void imu_init(void);
void imu_update(void);
imu_data_t * get_imu_data(imu_data_t * imu_data);

#endif
