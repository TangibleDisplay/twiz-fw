#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "imu.h"
#include "mpu9150.h"
#include "ak8975a.h"
#include "fusion.h"
#include "high_res_timer.h"
#include "twi_advertising.h"
#include "fusion.h"
#include "i2c_wrapper.h"

// vector to hold quaternion and AHRS results
// XXX FIXME : need a mutex !
static float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};
static float pitch, yaw, roll;

// variables to hold latest sensor data values
static float ax, ay, az, gx, gy, gz, mx, my, mz;


// XXX FIXME : this need to be split up in several functions
// and calibration data need to be stored in flash
void imu_update()
{
    // Used to calculate integration interval
    static int lastUpdate = 0, Now = 0;

    // If intPin goes high or NEW_DATA register is set, then all data registers have new data
    // XXX FIXME : should do this in interrupt service
    if (mpu9150_new_data()) {
        static int16_t data[7];

        // Read accel, temp and gyro data
        mpu9150_read_data(data);

        // Biases are removed by hardware if calibrate routine has been called
        ax = (float)data[0];
        ay = (float)data[1];
        az = (float)data[2];

        // Biases are removed by hardware if calibrate routine has been called.
        // Gyro values need to be scaled. Here we are at 250dps for full scale
        gx = (float)data[4]*250.0/32768.0*M_PI/180.0;
        gy = (float)data[5]*250.0/32768.0*M_PI/180.0;
        gz = (float)data[6]*250.0/32768.0*M_PI/180.0;

        // Get mag data
        ak8975a_read_data(&mx, &my, &mz);
    }

    // Get integration time by time elapsed since last filter update
    Now = get_time();
    float dt = (float)((Now - lastUpdate)/1000000.0f) ;
    lastUpdate = Now;

    madgwick_quaternion_update(ax, ay, az, gx, gy, gz, mx, my, mz, dt, q);

#if 0
    printf("ax=%04.2f, ay=%04.2f, az=%04.2f, gx=%04.2f, gy=%04.2f, gz=%04.2f, mx=%04.2f, my=%04.2f, mz=%04.2f\r\n",
           ax, ay, az, gx, gy, gz, mx, my, mz);
#endif
}

// set global variables : yaw, pitch and roll
static inline void update_euler_from_quaternions(void)
{
    // Define output variables from updated quaternion---these are Tait-Bryan angles,
    // commonly used in aircraft orientation.
    // In this coordinate system, the positive z-axis is down toward Earth.
    // Yaw is the angle between Sensor x-axis and Earth magnetic North (or true North
    // if corrected for local declination, looking  down on the sensor positive yaw is counterclockwise.
    // Pitch is angle between sensor x-axis and Earth ground plane, toward the Earth is positive,
    // up toward the sky is negative.
    // Roll is angle between sensor y-axis and Earth ground plane, y-axis up is positive roll.
    // These arise from the definition of the homogeneous rotation matrix constructed from quaternions.
    // Tait-Bryan angles as well as Euler angles are non-commutative; that is, the get the correct
    // orientation the rotations must be applied in the correct order which for this configuration
    // is yaw, pitch, and then roll.
    // For more see http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
    // which has additional links.
    yaw   = atan2(2.0f * (q[1] * q[2] + q[0] * q[3]), q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3]);
    pitch = -asin(2.0f * (q[1] * q[3] - q[0] * q[2]));
    roll  = atan2(2.0f * (q[0] * q[1] + q[2] * q[3]), q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3]);
    pitch *= 180.0f / M_PI;
    yaw   *= 180.0f / M_PI;
    yaw   -= 4.11f; // Declination at Paris, 2014
    roll  *= 180.0f / M_PI;
    //printf("Yaw, Pitch, Roll: %3.2f %3.2f %3.2f\n\r", yaw, pitch, roll);
}

void imu_init(void)
{
    // Init I2C
    i2c_init();

    // Init MPU
    mpu9150_reset();
    mpu9150_init();

    // Init Mag
    ak8975a_init();
    ak8975a_load_factory_calibration_data();

}

imu_data_t * get_imu_data(imu_data_t * imu_data)
{
    imu_data->accel[0] = (int16_t) ax;    // accel x
    imu_data->accel[1] = (int16_t) ay;    // accel y
    imu_data->accel[2] = (int16_t) az;    // accel z

    update_euler_from_quaternions();

    imu_data->euler[0] = (int16_t) yaw;   // heading
    imu_data->euler[1] = (int16_t) pitch;
    imu_data->euler[2] = (int16_t) roll;

    return imu_data;
}
