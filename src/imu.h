#ifndef MPU_H
#define MPU_H

// warning, we inverse the endianness before storing data here:
typedef struct imu_data_s {
    uint16_t accel[3]; // x, y, z
    uint16_t euler[3]; // yaw, pitch, roll
} imu_data_t;

void imu_init(void);
void imu_update(void);
imu_data_t * get_imu_data(imu_data_t * imu_data);
void imu_calibrate(bool button_was_pressed);
bool imu_load_calibration_data(void);


// Calibration data :
//   Magnetometer : if D is the raw measurement, then corrected value is A*(D-O), where :
//      D is a vertical 1x3 vector
//      A (mag_scale) is the 3x3 scale matrix defined below
//      O (mag_offset) is the vertical offset 1x3 vector defined below
//   Accel and gyro :
//      biases...

typedef struct {
    uint32_t magic1;
    float mag_scale[9]; // 3x3 matrix, coefs stored row by row
    float mag_offset[3];
    float gyro_bias[3];
    float accel_bias[3];
    uint32_t magic2;
} calibration_data_t;

extern calibration_data_t cal;

#endif
