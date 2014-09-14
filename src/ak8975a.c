#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "ak8975a.h"
#include "printf.h"
#include "i2c_wrapper.h"
#include "nrf_delay.h"
#include "nordic_common.h"
#include "app_error.h"
#include "uart.h"


//Magnetometer Registers
#define AK8975A_ADDRESS  0x0C

#define WHO_AM_I_AK8975A 0x00 // should return 0x48
#define INFO             0x01
#define AK8975A_ST1      0x02  // data ready status bit 0
#define AK8975A_XOUT_L   0x03  // data
#define AK8975A_XOUT_H   0x04
#define AK8975A_YOUT_L   0x05
#define AK8975A_YOUT_H   0x06
#define AK8975A_ZOUT_L   0x07
#define AK8975A_ZOUT_H   0x08
#define AK8975A_ST2      0x09  // Data overflow bit 3 and data read error status bit 2
#define AK8975A_CNTL     0x0A  // Power down (0000), single-measurement (0001), self-test (1000) and Fuse ROM (1111) modes on bits 3:0
#define AK8975A_ASTC     0x0C  // Self test control
#define AK8975A_ASAX     0x10  // Fuse ROM x-axis sensitivity adjustment value
#define AK8975A_ASAY     0x11  // Fuse ROM y-axis sensitivity adjustment value
#define AK8975A_ASAZ     0x12  // Fuse ROM z-axis sensitivity adjustment value

// Calibration values
mag_calibration_t mag_cal = {.scale = {1., 0, 0, 0, 1., 0, 0, 0, 1.},
                             .offset = {0, 0, 0},
};

// Init magnetometer.
// MUST be called AFTER mpu9150 init !
void ak8975a_init()
{
    // Check if magnometer is online
    uint8_t whoami = i2c_read_byte(AK8975A_ADDRESS, WHO_AM_I_AK8975A);
    printf("AK8975A : I am 0x%x\n\r", whoami);

    if (whoami != 0x48) {
        // WHO_AM_I should be 0x48
        printf("ERROR : I SHOULD BE 0x48\n\r");
        APP_ERROR_CHECK_BOOL(false);
    }
    printf("AK8975A is online...\n\r");

    // Power down mode
    i2c_write_byte(AK8975A_ADDRESS, AK8975A_CNTL, 0x00);
    nrf_delay_ms(10);
}

static void ak8975a_read_raw_data(int16_t *val)
{
 start:
    // Launch the first acquisition
    i2c_write_byte(AK8975A_ADDRESS, AK8975A_CNTL, 0x01);
    //nrf_delay_ms(1);

    // Wait for a data to become available
    while ((i2c_read_byte(AK8975A_ADDRESS, AK8975A_ST1) & 0x01) == 0);

    // If there is no overflow
    if((i2c_read_byte(AK8975A_ADDRESS, AK8975A_ST2) & 0x0C)==0) {
        int16_t v[3];
        // Read the six raw data registers sequentially into data array
        // WARNING : code valid for little endian only !
        i2c_read_bytes(AK8975A_ADDRESS, AK8975A_XOUT_L, 6, (uint8_t *)v);

        // WARNING, magnetometer axis are not the same as the accel / gyro ones
        // Thus : x <--> y, and z <--> -z
        val[0] = v[1];
        val[1] = v[0];
        val[2] = -v[2];
        return;
    }

    goto start;
}


void ak8975a_read_data(float *mx, float *my, float *mz)
{
    static int16_t data[3];
    ak8975a_read_raw_data(data);
    float x = data[0] - mag_cal.offset[0];
    float y = data[1] - mag_cal.offset[1];
    float z = data[2] - mag_cal.offset[2];
    *mx = x*mag_cal.scale[0] + y*mag_cal.scale[1] + z*mag_cal.scale[2];
    *my = x*mag_cal.scale[3] + y*mag_cal.scale[4] + z*mag_cal.scale[5];
    *mz = x*mag_cal.scale[6] + y*mag_cal.scale[7] + z*mag_cal.scale[8];
}


// Calibration protocol
#define NEW_VAL     ('r')
#define CAL_VAL     ('v')
#define WRITE_FLASH ('f')
#define QUIT        ('q')

void ak8975a_calibrate()
{
    /* Offline calibration for soft and hard iron : the user is asked (through the python
       calibration GUI) to move the TWIMU in all directions. The python GUI interacts with use
       through these simple commands :
         "r" : aks for a new raw mag value.
         "v" : sends 12 lines with each of the calibration coefficients in signed decimal ASCII form
         "f" : asks to store the calibration data in flash
         "q" : stops calibration routine
    */

    static int16_t data[3];
#define BUF_SIZE 48
    static char buf[BUF_SIZE] = {0};
    float *val = NULL;

    while(1) {
        getline(BUF_SIZE, buf);
        printf("%s\r\n", buf);

        switch (buf[0]) {
        case NEW_VAL :
            // If new raw mag values are asked for, then send them (ending with \r\n)
            ak8975a_read_raw_data(data);
            printf("%d %d %d\r\n", data[0], data[1], data[2]);
            break;

        case CAL_VAL:
            // Get 9 complete ASCII lines with the scale matrix values
            val = &mag_cal.scale[0];
            for(int i=0; i<9; i++) {
                getline(BUF_SIZE, buf);
                *val++ = atof(buf);
            }
            // Get 3 complete ASCII lines with the vector offset values
            val = &mag_cal.offset[0];
            for(int i=0; i<3; i++) {
                getline(BUF_SIZE, buf);
                *val++ = atof(buf);
            }

            printf("%f %f %f\r\n%f %f %f\r\n%f %f %f\r\n%f %f %f\r\n",
                   mag_cal.scale[0], mag_cal.scale[1], mag_cal.scale[2],
                   mag_cal.scale[3], mag_cal.scale[4], mag_cal.scale[5],
                   mag_cal.scale[6], mag_cal.scale[7], mag_cal.scale[8],
                   mag_cal.offset[0], mag_cal.offset[1], mag_cal.offset[2]);
            break;

        case WRITE_FLASH :
            // Store values in flash
            // XXX FIXME : TODO !
            break;

        case QUIT:
            return;
        }
    }
}
