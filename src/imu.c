#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "imu.h"
#include "mpu9150.h"
#include "ak8975a.h"
#include "fusion.h"
#include "high_res_timer.h"
#include "twi_advertising.h"
#include "twi_calibration_store.h"
#include "fusion.h"
#include "i2c_wrapper.h"
#include "app_util.h"
#include "softdevice_handler.h"
#include "uart.h"
#include "printf.h"
#include "leds.h"

// Vector to hold quaternion and AHRS results
// XXX FIXME : need a mutex !
static float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};
static float pitch, yaw, roll;

// Variables to hold latest sensor data values
static float ax, ay, az, gx, gy, gz, mx, my, mz;

// Calibration data
calibration_data_t cal = {.mag_scale = {1., 0, 0, 0, 1., 0, 0, 0, 1.},
                          .mag_offset = {0, 0, 0},
                          .accel_bias = {0, 0, 0},
                          .gyro_bias = {0, 0, 0},
};


void imu_update()
{
    // Used to calculate integration interval
    static int lastUpdate = 0, Now = 0;

    // If intPin goes high or NEW_DATA register is set, then all data registers have new data
    // XXX FIXME : should do this in interrupt service
    if (mpu9150_new_data()) {
        static float data[6];

        // Read accel, temp and gyro data
        mpu9150_read_data(data);

        ax = data[0];
        ay = data[1];
        az = data[2];

        gx = data[3];
        gy = data[4];
        gz = data[5];

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

// Set global variables : yaw, pitch and roll
// Called from CRITIAL_REGION_ENTER : NO PRINTF with IRQ ALLOWED HERE ! Only simple "simple_uart" calls !
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

}


imu_data_t * get_imu_data(imu_data_t * imu_data)
{
    CRITICAL_REGION_ENTER();

    imu_data->accel[0] = (int16_t) ax;    // accel x
    imu_data->accel[1] = (int16_t) ay;    // accel y
    imu_data->accel[2] = (int16_t) az;    // accel z

    update_euler_from_quaternions();

    imu_data->euler[0] = (int16_t) yaw;   // heading
    imu_data->euler[1] = (int16_t) pitch;
    imu_data->euler[2] = (int16_t) roll;

    CRITICAL_REGION_EXIT();
    return imu_data;
}


bool imu_load_calibration_data()
{
    calibration_data_t temp;
    if (calibration_store_load(&temp)) {
        memcpy(&cal, &temp, sizeof cal);
        return true;
    }
    else {
        printf("No valid calibration found in flash.\r\n");
        printf("YOU SHOULD REALLY CONSIDER RUNNING THE CALIBRATION PROCEDURE !!!\r\n");
        for(int i=0; i<200; i++)
            leds_blink(10);
        return false;
    }
}


void imu_store_calibration_data()
{
    calibration_store_write(&cal);
}


// Calibration protocol
#define NEW_MAG            ('m')
#define SEND_CAL           ('s')
#define WRITE_FLASH        ('w')
#define START_CAL_ACC_GYRO ('a')
#define END_CAL_ACC_GYRO   ('s')
#define READ_CAL_DATA      ('r')
#define READ_DATA          ('d')
#define QUIT               ('q')

void imu_calibrate()
{
    /* Offline calibration for MPU9150 : the user is asked (through the python
       calibration GUI) to move the TWIMU in all directions or to let is standing still horizontaly.
       The python GUI interacts with user through these simple commands :
         "m" : aks for a new raw mag value.
         "s" : sends 12 lines with each of the mag calibration coefficients in signed decimal ASCII form
         "w" : asks to store the calibration data in flash
         "a" : start accel and gyroscope biases calulation (IMU must be standing still and horizontaly)
         "s" : sent by nRF to signal the end of accel and gyroscope biases calculation
         "q" : stops calibration routine
         "r" : display calibration data
    */

#define BUF_SIZE 48
    static char buf[BUF_SIZE] = {0};
    static int16_t data[3];
    float *val = NULL;

    while(1) {
        getline(BUF_SIZE, buf);
        printf("%s\r\n", buf);

        switch (buf[0]) {
        case NEW_MAG :
            // If new raw mag values are asked for, then send them (ending with \r\n)
            ak8975a_read_raw_data(data);
            printf("%d %d %d\r\n", data[0], data[1], data[2]);
            printf("%c: done.\r\n", buf[0]);
            break;

        case SEND_CAL:
            // Get 9 complete ASCII lines with the scale matrix values
            val = &cal.mag_scale[0];
            for(int i=0; i<9; i++) {
                getline(BUF_SIZE, buf);
                *val++ = atof(buf);
            }
            // Get 3 complete ASCII lines with the vector offset values
            val = &cal.mag_offset[0];
            for(int i=0; i<3; i++) {
                getline(BUF_SIZE, buf);
                *val++ = atof(buf);
            }

            printf("Mag scale = %f %f %f\r\n%f %f %f\r\n%f %f %f\r\n",
                   cal.mag_scale[0], cal.mag_scale[1], cal.mag_scale[2],
                   cal.mag_scale[3], cal.mag_scale[4], cal.mag_scale[5],
                   cal.mag_scale[6], cal.mag_scale[7], cal.mag_scale[8]);
            printf("Mag offset = %f %f %f\r\n",
                   cal.mag_offset[0], cal.mag_offset[1], cal.mag_offset[2]);
            printf("%c: done.\r\n", buf[0]);
            break;

        case START_CAL_ACC_GYRO:
            // Turn on LED
            led_on();
            // Start bias measures
            mpu9150_measure_biases();
            // Turn off LED
            led_off();
            // Send stop command
            printf("%c\r\n", END_CAL_ACC_GYRO);
            printf("Accel bias = %f %f %f\r\n",
                   cal.accel_bias[0], cal.accel_bias[1], cal.accel_bias[2]);
            printf("Gyro bias = %f %f %f\r\n",
                   cal.gyro_bias[0], cal.gyro_bias[1], cal.gyro_bias[2]);
            printf("%c: done.\r\n", buf[0]);
            break;

        case WRITE_FLASH :
            // Store calibration values in flash
            imu_store_calibration_data();
            printf("%c: done.\r\n", buf[0]);
            break;

        case READ_CAL_DATA :
            printf("Mag scale = \t%f %f %f\r\n\t\t%f %f %f\r\n\t\t%f %f %f\r\n",
                   cal.mag_scale[0], cal.mag_scale[1], cal.mag_scale[2],
                   cal.mag_scale[3], cal.mag_scale[4], cal.mag_scale[5],
                   cal.mag_scale[6], cal.mag_scale[7], cal.mag_scale[8]);
            printf("Mag offset = %f %f %f\r\n",
                   cal.mag_offset[0], cal.mag_offset[1], cal.mag_offset[2]);
            printf("Accel bias = %f %f %f\r\n",
                   cal.accel_bias[0], cal.accel_bias[1], cal.accel_bias[2]);
            printf("Gyro bias = %f %f %f\r\n",
                   cal.gyro_bias[0], cal.gyro_bias[1], cal.gyro_bias[2]);
            printf("%c: done.\r\n", buf[0]);
            break;

        case QUIT:
            printf("End of calibration procedure\r\n");
            return;

        default :
            // Display all sensors data (with correction)
            // If new raw mag values are asked for, then send them (ending with \r\n)
            {
                float mx, my, mz;
                float acc_gyro_data[6];
                ak8975a_read_data(&mx, &my, &mz);
                mpu9150_read_data(acc_gyro_data);
                printf("%f %f %f %f %f %f %f %f %f\r\n",
                       acc_gyro_data[0], acc_gyro_data[1], acc_gyro_data[2],
                       acc_gyro_data[3], acc_gyro_data[4], acc_gyro_data[5],
                       mx, my, mz);
                printf("%c: done.\r\n", buf[0]);
            }
            break;


        }
    }
}
