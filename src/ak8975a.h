#ifndef AK8975A_H
#define AK8975A_H

void ak8975a_init(void);
void ak8975a_read_data(float *mx, float *my, float *mz);
void ak8975a_calibrate(void);

// Mag calibration data : if D is the raw measurement, then corrected value is A*(D-O), where :
//   D is a vertical 1x3 vector
//   A is the 3x3 scale matrix defined below
//   O is the vertical offset 1x3 vector defined below

typedef struct {
    float scale[9]; // 3x3 matrix, coefs stored row by row
    float offset[3];
} mag_calibration_t;

extern mag_calibration_t mag_calibration;

#endif
