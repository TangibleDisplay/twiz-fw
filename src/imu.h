#ifndef MPU_H
#define MPU_H

typedef struct imu_data_s {
    int16_t accel[3]; // x, y, z
    int16_t euler[3]; // yaw, pitch, roll
} imu_data_t;

void imu_init(void);
void imu_update(void);
imu_data_t * get_imu_data(imu_data_t * imu_data);
void imu_calibrate(void);


// Calibration data :
//   Magnetometer : if D is the raw measurement, then corrected value is A*(D-O), where :
//      D is a vertical 1x3 vector
//      A (mag_scale) is the 3x3 scale matrix defined below
//      O (mag_offset) is the vertical offset 1x3 vector defined below
//   Accel and gyro :
//      biases...

typedef struct {
    float mag_scale[9]; // 3x3 matrix, coefs stored row by row
    float mag_offset[3];
    float gyro_bias[3];
    float accel_bias[3];
} mag_calibration_t;

extern mag_calibration_t cal;

#endif
