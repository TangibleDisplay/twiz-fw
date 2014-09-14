#ifndef FUSION_H
#define FUSION_H

#include <stdint.h>

void madgwick_quaternion_update(float ax, float ay, float az,
                                float gx, float gy, float gz,
                                float mx, float my, float mz,
                                float dt,
                                float *q);

void mahony_quaternion_update(float ax, float ay, float az,
                              float gx, float gy, float gz,
                              float mx, float my, float mz,
                              float dt,
                              float *q);

#endif
