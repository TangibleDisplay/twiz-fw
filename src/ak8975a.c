#include "ak8975a.h"
#include "printf.h"


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

static int ak8975a_read_raw_data(int16_t *data)
{
    static bool running = false;
    static uint8_t rawData[6];  // x/y/z gyro register data stored here
    static int ret = 0;

    if (running) {
        // If there is a data available
        if (i2c_read_byte(AK8975A_ADDRESS, AK8975A_ST1) & 0x01) {
            // If there is no overflow
            if((i2c_read_byte(AK8975A_ADDRESS, AK8975A_ST2) & 0x0C)==0) {
                // Read the six raw data registers sequentially into data array
                i2c_read_bytes(AK8975A_ADDRESS, AK8975A_XOUT_L, 6, rawData);
                // Turn the MSB and LSB into a signed 16-bit value
                // XXX FIXME : WARNING, magnetometer axis are not the same as the accel / gyro ones
                // Thus : x <--> y, and z <--> -z
                data[1] = ((int16_t)rawData[1])*256 | rawData[0];
                data[0] = ((int16_t)rawData[3])*256 | rawData[2];
                data[2] = -((int16_t)rawData[5])*256 | rawData[4];
                ret = 0;
            }
            else
                ret = -1;
            // Launch a new acquisition
            i2c_write_byte(AK8975A_ADDRESS, AK8975A_CNTL, 0x01);
            return ret;
        }
    }
    else {
        // Else launch the first acquisition
        i2c_write_byte(AK8975A_ADDRESS, AK8975A_CNTL, 0x01);
        running = true;
    }
    return -1;
}

static void ak8975a_read_data(float *mx, float *my, float *mz)
{
    static int16_t data[3];
    while(ak8975a_read_raw_data(data) != 0) ;
    *mx = (data[0] - magBias[0])*magCalibration[0];
    *my = (data[1] - magBias[1])*magCalibration[1];
    *mz = (data[2] - magBias[2])*magCalibration[2];
}

static void ak8975a_load_factory_calibration_data()
{
    // Get and store factory trim values
    uint8_t rawData[3];
    // Power down
    i2c_write_byte(AK8975A_ADDRESS, AK8975A_CNTL, 0x00);
    nrf_delay_ms(1);
    // Enter Fuse ROM access mode
    i2c_write_byte(AK8975A_ADDRESS, AK8975A_CNTL, 0x0F);
    nrf_delay_ms(10);
    // Read the x-, y-, and z-axis calibration values
    i2c_read_bytes(AK8975A_ADDRESS, AK8975A_ASAX, 3, rawData);
    // Back to power down mode
    i2c_write_byte(AK8975A_ADDRESS, AK8975A_CNTL, 0x00);

    magCalibration[0] =  (float)(rawData[0] - 128)/256.0f + 1.0f; // Return x-axis sensitivity adjustment values
    magCalibration[1] =  (float)(rawData[1] - 128)/256.0f + 1.0f;
    magCalibration[2] =  (float)(rawData[2] - 128)/256.0f + 1.0f;
}




void ak8975a_calibrate()
{
#if 1

    // Calibrate for hard iron : for some times, user is asked to move the device in
    // all directions. We record the min / max values, then compute the halfsum which
    // will be our offset.

    int meas_count = 2000;
    static int16_t data[3];
    static int xmin = 32767, ymin =32767, zmin = 32767;
    static int xmax = -327678, ymax = -32768, zmax = -32768;

    // Read a lot a values, hoping that the users moves the TWI in all possible directions
    // Data include already the factory trim correction.
    for(int i=0; i<meas_count; i++) {
        if (ak8975a_read_raw_data(data) != 0) {
            i--;
            continue;
        }

#if 1
        for(int j=0; j<3; j++)
            printf("%d ", (int)data[j]);
        printf("\r\n");
#endif

        // Keep track of min and max along each axis
        xmin = MIN(xmin, data[0]);
        xmax = MAX(xmax, data[0]);

        ymin = MIN(ymin, data[1]);
        ymax = MAX(ymax, data[1]);

        zmin = MIN(zmin, data[2]);
        zmax = MAX(zmax, data[2]);
    }

    printf("xmin=%d, xmax=%d, ymin=%d, ymax=%d, zmin=%d, zmax=%d\r\n", xmin, xmax, ymin, ymax, zmin, zmax);

    // Now calculate biases
    magBias[0] = (xmin + xmax)/2.;
    magBias[1] = (ymin + ymax)/2.;
    magBias[2] = (zmin + zmax)/2.;

    // And update calibration data so that max = 180
    magCalibration[0] = 360./(xmax - xmin);
    magCalibration[1] = 360./(ymax - ymin);
    magCalibration[2] = 360./(zmax - zmin);

    printf("Mag calibration values :\r\n");
    printf("\txbias = %f, ybias = %f, zbias = %f\r\n", magBias[0], magBias[1], magBias[2]);
    printf("\txcal = %f, ycal = %f, zcal = %f\r\n", magCalibration[0], magCalibration[1], magCalibration[2]);

#else
    // Valeur de calibration sur la table de la A06 :)
    // xbias = 7.000000, ybias = 29.500000, zbias = -142.000000
    // xcal = 1.113281, ycal = 1.121094, zcal = 1.175781
    magBias[0] = 7.000000;
    magBias[1] = 29.500000;
    magBias[2] = -142.000000;
    magCalibration[0] = 1.113281;
    magCalibration[1] = 1.121094;
    magCalibration[2] = 1.175781;
#endif

}
