#ifndef AK8975A_H
#define AK8975A_H

void ak8975a_init(void);
void ak8975a_read_raw_data(int16_t *val);
void ak8975a_read_data(float *mx, float *my, float *mz);
void ak8975a_calibrate(void);

#endif
