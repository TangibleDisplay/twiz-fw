/**
   From : https://github.com/kriswiner/STM32F401
   TODO : see licensing restrictions and directives !
*/

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "mpu9150.h"
#include "i2c_wrapper.h"
#include "nrf_delay.h"
#include "printf.h"

// Define registers per MPU6050, Register Map and Descriptions, Rev 4.2, 08/19/2013 6 DOF Motion sensor fusion device
// Invensense Inc., www.invensense.com
// See also MPU-9150 Register Map and Descriptions, Revision 4.0, RM-MPU-9150A-00, 9/12/2012 for registers not listed in
// above document; the MPU6050 and MPU 9150 are virtually identical but the latter has an on-board magnetic sensor
//
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

#define XGOFFS_TC        0x00 // Bit 7 PWR_MODE, bits 6:1 XG_OFFS_TC, bit 0 OTP_BNK_VLD
#define YGOFFS_TC        0x01
#define ZGOFFS_TC        0x02
#define X_FINE_GAIN      0x03 // [7:0] fine gain
#define Y_FINE_GAIN      0x04
#define Z_FINE_GAIN      0x05
#define XA_OFFSET_H      0x06 // User-defined trim values for accelerometer
#define XA_OFFSET_L_TC   0x07
#define YA_OFFSET_H      0x08
#define YA_OFFSET_L_TC   0x09
#define ZA_OFFSET_H      0x0A
#define ZA_OFFSET_L_TC   0x0B
#define SELF_TEST_X      0x0D
#define SELF_TEST_Y      0x0E
#define SELF_TEST_Z      0x0F
#define SELF_TEST_A      0x10
#define XG_OFFS_USRH     0x13  // User-defined trim values for gyroscope, populate with calibration routine
#define XG_OFFS_USRL     0x14
#define YG_OFFS_USRH     0x15
#define YG_OFFS_USRL     0x16
#define ZG_OFFS_USRH     0x17
#define ZG_OFFS_USRL     0x18
#define SMPLRT_DIV       0x19
#define CONFIG           0x1A
#define GYRO_CONFIG      0x1B
#define ACCEL_CONFIG     0x1C
#define FF_THR           0x1D  // Free-fall
#define FF_DUR           0x1E  // Free-fall
#define MOT_THR          0x1F  // Motion detection threshold bits [7:0]
#define MOT_DUR          0x20  // Duration counter threshold for motion interrupt generation, 1 kHz rate, LSB = 1 ms
#define ZMOT_THR         0x21  // Zero-motion detection threshold bits [7:0]
#define ZRMOT_DUR        0x22  // Duration counter threshold for zero motion interrupt generation, 16 Hz rate, LSB = 64 ms
#define FIFO_EN          0x23
#define I2C_MST_CTRL     0x24
#define I2C_SLV0_ADDR    0x25
#define I2C_SLV0_REG     0x26
#define I2C_SLV0_CTRL    0x27
#define I2C_SLV1_ADDR    0x28
#define I2C_SLV1_REG     0x29
#define I2C_SLV1_CTRL    0x2A
#define I2C_SLV2_ADDR    0x2B
#define I2C_SLV2_REG     0x2C
#define I2C_SLV2_CTRL    0x2D
#define I2C_SLV3_ADDR    0x2E
#define I2C_SLV3_REG     0x2F
#define I2C_SLV3_CTRL    0x30
#define I2C_SLV4_ADDR    0x31
#define I2C_SLV4_REG     0x32
#define I2C_SLV4_DO      0x33
#define I2C_SLV4_CTRL    0x34
#define I2C_SLV4_DI      0x35
#define I2C_MST_STATUS   0x36
#define INT_PIN_CFG      0x37
#define INT_ENABLE       0x38
#define DMP_INT_STATUS   0x39  // Check DMP interrupt
#define INT_STATUS       0x3A
#define ACCEL_XOUT_H     0x3B
#define ACCEL_XOUT_L     0x3C
#define ACCEL_YOUT_H     0x3D
#define ACCEL_YOUT_L     0x3E
#define ACCEL_ZOUT_H     0x3F
#define ACCEL_ZOUT_L     0x40
#define TEMP_OUT_H       0x41
#define TEMP_OUT_L       0x42
#define GYRO_XOUT_H      0x43
#define GYRO_XOUT_L      0x44
#define GYRO_YOUT_H      0x45
#define GYRO_YOUT_L      0x46
#define GYRO_ZOUT_H      0x47
#define GYRO_ZOUT_L      0x48
#define EXT_SENS_DATA_00 0x49
#define EXT_SENS_DATA_01 0x4A
#define EXT_SENS_DATA_02 0x4B
#define EXT_SENS_DATA_03 0x4C
#define EXT_SENS_DATA_04 0x4D
#define EXT_SENS_DATA_05 0x4E
#define EXT_SENS_DATA_06 0x4F
#define EXT_SENS_DATA_07 0x50
#define EXT_SENS_DATA_08 0x51
#define EXT_SENS_DATA_09 0x52
#define EXT_SENS_DATA_10 0x53
#define EXT_SENS_DATA_11 0x54
#define EXT_SENS_DATA_12 0x55
#define EXT_SENS_DATA_13 0x56
#define EXT_SENS_DATA_14 0x57
#define EXT_SENS_DATA_15 0x58
#define EXT_SENS_DATA_16 0x59
#define EXT_SENS_DATA_17 0x5A
#define EXT_SENS_DATA_18 0x5B
#define EXT_SENS_DATA_19 0x5C
#define EXT_SENS_DATA_20 0x5D
#define EXT_SENS_DATA_21 0x5E
#define EXT_SENS_DATA_22 0x5F
#define EXT_SENS_DATA_23 0x60
#define MOT_DETECT_STATUS 0x61
#define I2C_SLV0_DO      0x63
#define I2C_SLV1_DO      0x64
#define I2C_SLV2_DO      0x65
#define I2C_SLV3_DO      0x66
#define I2C_MST_DELAY_CTRL 0x67
#define SIGNAL_PATH_RESET  0x68
#define MOT_DETECT_CTRL   0x69
#define USER_CTRL        0x6A  // Bit 7 enable DMP, bit 3 reset DMP
#define PWR_MGMT_1       0x6B // Device defaults to the SLEEP mode
#define PWR_MGMT_2       0x6C
#define DMP_BANK         0x6D  // Activates a specific bank in the DMP
#define DMP_RW_PNT       0x6E  // Set read/write pointer to a specific start address in specified DMP bank
#define DMP_REG          0x6F  // Register in DMP from which to read or to which to write
#define DMP_REG_1        0x70
#define DMP_REG_2        0x71
#define FIFO_COUNTH      0x72
#define FIFO_COUNTL      0x73
#define FIFO_R_W         0x74
#define WHO_AM_I_MPU9150 0x75 // Should return 0x68


// On the TWI, ADO is set to 0
#define MPU9150_ADDRESS 0x68

// Set initial input parameters
#define AFS_2G  0
#define AFS_4G  1
#define AFS_8G  2
#define AFS_16G 3

#define  GFS_250DPS  0
#define  GFS_500DPS  1
#define  GFS_1000DPS 2
#define  GFS_2000DPS 3

static uint8_t Ascale = AFS_2G;     // AFS_2G, AFS_4G, AFS_8G, AFS_16G
static uint8_t Gscale = GFS_250DPS; // GFS_250DPS, GFS_500DPS, GFS_1000DPS, GFS_2000DPS
static float aRes, gRes, mRes;      // scale resolutions per LSB for the sensors
static calibrated = false;

// Pin definitions
// XXX FIXME int intPin = 12;  // These can be changed, 2 and 3 are the Arduinos ext int pins

static int16_t accelCount[3];  // Stores the 16-bit signed accelerometer sensor output
static int16_t gyroCount[3];   // Stores the 16-bit signed gyro sensor output
static int16_t magCount[3];    // Stores the 16-bit signed magnetometer sensor output
static float magCalibration[3] = {0, 0, 0}, magbias[3] = {0, 0, 0};  // Factory mag calibration and mag bias
static float gyroBias[3] = {0, 0, 0}, accelBias[3] = {0, 0, 0}; // Bias corrections for gyro and accelerometer
static float ax, ay, az, gx, gy, gz, mx, my, mz; // variables to hold latest sensor data values
static int16_t tempCount;   // Stores the real internal chip temperature in degrees Celsius
static float temperature;

static int delt_t = 0; // used to control display output rate
static int count = 0;  // used to control display output rate

// parameters for 6 DoF sensor fusion calculations
static float PI = 3.14159265358979323846f;
// gyroscope measurement error in rads/s (start at 60 deg/s), then reduce after ~10 s to 3
// PI * (60.0f / 180.0f);
static float GyroMeasError = 1.0471975511965976f;
// compute beta = sqrt(3.0f / 4.0f) * GyroMeasError;
static float beta = 0.9068996821171088f;
// gyroscope measurement drift in rad/s/s (start at 0.0 deg/s/s)
// PI * (1.0f / 180.0f)
static float GyroMeasDrift = 0.017453292519943295f;
// compute zeta, the other free parameter in the Madgwick scheme usually set to a small or zero value
// zeta = sqrt(3.0f / 4.0f) * GyroMeasDrift;
static float zeta = 0.015114994701951814f;
// these are the free parameters in the Mahony filter and fusion scheme, Kp for proportional feedback,
// Ki for integral
#define Kp (2.0f * 5.0f)
#define Ki (0.0f)

static float pitch, yaw, roll;
// integration interval for both filter schemes
static float deltat = 0.0f;
// used to calculate integration interval
static int lastUpdate = 0, firstUpdate = 0, Now = 0;

// vector to hold quaternion
static float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};
// vector to hold integral error for Mahony method
static float eInt[3] = {0.0f, 0.0f, 0.0f};

static void getGres() {
    switch (Gscale)
        {
            // Possible gyro scales (and their register bit settings) are:
            // 250 DPS (00), 500 DPS (01), 1000 DPS (10), and 2000 DPS  (11).
            // Here's a bit of an algorithm to calculate DPS/(ADC tick) based on that 2-bit value:
        case GFS_250DPS:
            gRes = 250.0/32768.0;
            break;
        case GFS_500DPS:
            gRes = 500.0/32768.0;
            break;
        case GFS_1000DPS:
            gRes = 1000.0/32768.0;
            break;
        case GFS_2000DPS:
            gRes = 2000.0/32768.0;
            break;
        }
}


static void getAres() {
    switch (Ascale)
        {
            // Possible accelerometer scales (and their register bit settings) are:
            // 2 Gs (00), 4 Gs (01), 8 Gs (10), and 16 Gs  (11).
            // Here's a bit of an algorithm to calculate DPS/(ADC tick) based on that 2-bit value:
        case AFS_2G:
            aRes = 2.0/32768.0;
            break;
        case AFS_4G:
            aRes = 4.0/32768.0;
            break;
        case AFS_8G:
            aRes = 8.0/32768.0;
            break;
        case AFS_16G:
            aRes = 16.0/32768.0;
            break;
        }
}


static void read_accel_data(int16_t * destination)
{
    uint8_t rawData[6];  // x/y/z accel register data stored here
    i2c_read_bytes(MPU9150_ADDRESS, ACCEL_XOUT_H, 6, &rawData[0]);  // Read the six raw data registers into data array
    destination[0] = (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]) ;  // Turn the MSB and LSB into a signed 16-bit value
    destination[1] = (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]) ;
    destination[2] = (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]) ;
}

static void read_gyro_data(int16_t * destination)
{
    uint8_t rawData[6];  // x/y/z gyro register data stored here
    i2c_read_bytes(MPU9150_ADDRESS, GYRO_XOUT_H, 6, &rawData[0]);  // Read the six raw data registers sequentially into data array
    destination[0] = (int16_t)(((int16_t)rawData[0] << 8) | rawData[1]) ;  // Turn the MSB and LSB into a signed 16-bit value
    destination[1] = (int16_t)(((int16_t)rawData[2] << 8) | rawData[3]) ;
    destination[2] = (int16_t)(((int16_t)rawData[4] << 8) | rawData[5]) ;
}

static void read_mag_data(int16_t * destination)
{
    uint8_t rawData[6];  // x/y/z gyro register data stored here
    i2c_write_byte(AK8975A_ADDRESS, AK8975A_CNTL, 0x01); // toggle enable data read from magnetometer, no continuous read mode!
    nrf_delay_ms(10);
    // Only accept a new magnetometer data read if the data ready bit is set and
    // if there are no sensor overflow or data read errors
    if(i2c_read_byte(AK8975A_ADDRESS, AK8975A_ST1) & 0x01) { // wait for magnetometer data ready bit to be set
        i2c_read_bytes(AK8975A_ADDRESS, AK8975A_XOUT_L, 6, &rawData[0]);  // Read the six raw data registers sequentially into data array
        destination[0] = ((int16_t)rawData[1] << 8) | rawData[0] ;  // Turn the MSB and LSB into a signed 16-bit value
        destination[1] = ((int16_t)rawData[3] << 8) | rawData[2] ;
        destination[2] = ((int16_t)rawData[5] << 8) | rawData[4] ;
    }
}

static void ak8975a_init(float * destination)
{
    uint8_t rawData[3];  // x/y/z gyro register data stored here
    i2c_write_byte(AK8975A_ADDRESS, AK8975A_CNTL, 0x00); // Power down
    nrf_delay_ms(10);
    i2c_write_byte(AK8975A_ADDRESS, AK8975A_CNTL, 0x0F); // Enter Fuse ROM access mode
    nrf_delay_ms(10);
    i2c_read_bytes(AK8975A_ADDRESS, AK8975A_ASAX, 3, &rawData[0]);  // Read the x-, y-, and z-axis calibration values
    destination[0] =  (float)(rawData[0] - 128)/256.0f + 1.0f; // Return x-axis sensitivity adjustment values
    destination[1] =  (float)(rawData[1] - 128)/256.0f + 1.0f;
    destination[2] =  (float)(rawData[2] - 128)/256.0f + 1.0f;
}

static int16_t read_temp_data()
{
    uint8_t rawData[2];  // x/y/z gyro register data stored here
    i2c_read_bytes(MPU9150_ADDRESS, TEMP_OUT_H, 2, &rawData[0]);  // Read the two raw data registers sequentially into data array
    return ((int16_t)rawData[0] << 8) | rawData[1] ;  // Turn the MSB and LSB into a 16-bit value
}

static void mpu9150_reset() {
    // Write a one to bit 7 reset bit; toggle reset device
    i2c_write_byte(MPU9150_ADDRESS, PWR_MGMT_1, 0x80);
    while(i2c_read_byte(MPU9150_ADDRESS, PWR_MGMT_1) & 0x80) ;
    nrf_delay_ms(200);
}

void mpu9150_init()
{
    // Take MPU9150 out of sleep
    i2c_write_byte(MPU9150_ADDRESS, PWR_MGMT_1, 0x00);
    // Delay 100ms for gyro startup
    nrf_delay_ms(100);

    // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001
    i2c_write_byte(MPU9150_ADDRESS, PWR_MGMT_1, 0x01);
    i2c_write_byte(MPU9150_ADDRESS, PWR_MGMT_2, 0x00);
    nrf_delay_ms(100);

    // Disable I2C master mode, disable FIFO
    i2c_write_byte(MPU9150_ADDRESS, I2C_MST_CTRL, 0x00);
    i2c_write_byte(MPU9150_ADDRESS, FIFO_EN, 0x00);
    // Reset sensors PATH and registers and FIFO
    i2c_write_byte(MPU9150_ADDRESS, USER_CTRL, 0x5);
    while(i2c_read_byte(MPU9150_ADDRESS, USER_CTRL) & 0x05) ;

    // Configure Gyro and Accelerometer
    // Disable FSYNC and set accelerometer and gyro bandwidth to 44 and 42 Hz, respectively;
    // DLPF_CFG = bits 2:0 = 010; this sets the sample rate at 1 kHz for both
    // Maximum delay is 4.9 ms which is just over a 200 Hz maximum rate
    i2c_write_byte(MPU9150_ADDRESS, CONFIG, 0x01); // XXXX FIXME : 0x03

    // Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
    i2c_write_byte(MPU9150_ADDRESS, SMPLRT_DIV, 0x04);  // Use a 200 Hz rate; the same rate set in CONFIG above

    // Set gyroscope full scale range
    // Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are left-shifted into positions 4:3
    uint8_t c =  i2c_read_byte(MPU9150_ADDRESS, GYRO_CONFIG);
    i2c_write_byte(MPU9150_ADDRESS, GYRO_CONFIG, c & ~0xE0); // Clear self-test bits [7:5]
    i2c_write_byte(MPU9150_ADDRESS, GYRO_CONFIG, c & ~0x18); // Clear AFS bits [4:3]
    i2c_write_byte(MPU9150_ADDRESS, GYRO_CONFIG, c | Gscale << 3); // Set full scale range for the gyro

    // Set accelerometer configuration
    c =  i2c_read_byte(MPU9150_ADDRESS, ACCEL_CONFIG);
    i2c_write_byte(MPU9150_ADDRESS, ACCEL_CONFIG, c & ~0xE0); // Clear self-test bits [7:5]
    i2c_write_byte(MPU9150_ADDRESS, ACCEL_CONFIG, c & ~0x18); // Clear AFS bits [4:3]
    i2c_write_byte(MPU9150_ADDRESS, ACCEL_CONFIG, c | Ascale << 3); // Set full scale range for the accelerometer

    // The accelerometer, gyro, and thermometer are set to 1 kHz sample rates,
    // but all these rates are further reduced by a factor of 5 to 200 Hz because of the SMPLRT_DIV setting

    // Configure Interrupts and Bypass Enable
    // Set interrupt pin active high, push-pull, latched and clear on read of INT_STATUS,
    // enable I2C_BYPASS_EN so additional chips can join the I2C bus and all can be controlled
    // by the TWI as master
    i2c_write_byte(MPU9150_ADDRESS, INT_PIN_CFG, 0x22);
    i2c_write_byte(MPU9150_ADDRESS, INT_ENABLE, 0x01);  // Enable data ready (bit 0) interrupt
}

// Function which accumulates gyro and accelerometer data after device initialization. It calculates the average
// of the at-rest readings and then loads the resulting offsets into accelerometer and gyro bias registers.
void mpu9150_calibrate(float * dest1, float * dest2)
{
    static uint8_t data[14]; // data array to hold accelerometer and gyro x, y, z, data
    int ii, meas_count;
    int32_t gyro_bias[3] = {0, 0, 0}, accel_bias[3] = {0, 0, 0};
    static int32_t gyro_temp[3];
    static int32_t accel_temp[3];

    // If already calibrated, then reset chip to reset HW cal register to factory trim
    if(calibrated) {
        mpu9150_reset();
        mpu9150_init();
    }
    calibrated = true;

    // Configure MPU9150 gyro and accelerometer for bias calculation
    i2c_write_byte(MPU9150_ADDRESS, GYRO_CONFIG, 0x00);  // Set gyro full-scale to 250 degrees per second, maximum sensitivity
    i2c_write_byte(MPU9150_ADDRESS, ACCEL_CONFIG, 0x00); // Set accelerometer full-scale to 2 g, maximum sensitivity
    nrf_delay_ms(200);

    uint16_t  gyrosensitivity  = 131;   // = 131 LSB/degrees/sec
    uint16_t  accelsensitivity = 16384;  // = 16384 LSB/g

    // Accumulate 200 measures each 5 ms (200Hz sample rate)
    meas_count = 200;
    for (int i = 0; i<meas_count; i++) {
        nrf_delay_ms(5);

        // Burst read to ensure that accel, temp and gyro measurements are taken at the same time
        i2c_read_bytes(MPU9150_ADDRESS, ACCEL_XOUT_H, 14, data);

        // Debug
#if 1
        for (int j=0; j<14; j=j+2)
            printf("%02x%02x ", data[j], data[j+1]);
        printf("\r\n");
#endif
        // Form signed 16-bit integer for each data
        accel_temp[0] = (int16_t) (((int16_t)data[0] << 8) | data[1]  ) ;
        accel_temp[1] = (int16_t) (((int16_t)data[2] << 8) | data[3]  ) ;
        accel_temp[2] = (int16_t) (((int16_t)data[4] << 8) | data[5]  ) ;
        gyro_temp[0]  = (int16_t) (((int16_t)data[8] << 8) | data[9]  ) ;
        gyro_temp[1]  = (int16_t) (((int16_t)data[10] << 8) | data[11]  ) ;
        gyro_temp[2]  = (int16_t) (((int16_t)data[12] << 8) | data[13]) ;

        // Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
        accel_bias[0] += (int32_t) accel_temp[0];
        accel_bias[1] += (int32_t) accel_temp[1];
        accel_bias[2] += (int32_t) accel_temp[2];
        gyro_bias[0]  += (int32_t) gyro_temp[0];
        gyro_bias[1]  += (int32_t) gyro_temp[1];
        gyro_bias[2]  += (int32_t) gyro_temp[2];

    }
    // Normalize sums to get average count biases
    accel_bias[0] /= (int32_t) meas_count;
    accel_bias[1] /= (int32_t) meas_count;
    accel_bias[2] /= (int32_t) meas_count;
    gyro_bias[0]  /= (int32_t) meas_count;
    gyro_bias[1]  /= (int32_t) meas_count;
    gyro_bias[2]  /= (int32_t) meas_count;

    // Remove gravity from the z-axis accelerometer bias calculation
    if(accel_bias[2] > 0L) {accel_bias[2] -= (int32_t) accelsensitivity;}
    else {accel_bias[2] += (int32_t) accelsensitivity;}

    // Construct the gyro biases for push to the hardware gyro bias registers, which are reset to zero upon device startup
    data[0] = (-gyro_bias[0]/4  >> 8) & 0xFF; // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
    data[1] = (-gyro_bias[0]/4)       & 0xFF; // Biases are additive, so change sign on calculated average gyro biases
    data[2] = (-gyro_bias[1]/4  >> 8) & 0xFF;
    data[3] = (-gyro_bias[1]/4)       & 0xFF;
    data[4] = (-gyro_bias[2]/4  >> 8) & 0xFF;
    data[5] = (-gyro_bias[2]/4)       & 0xFF;

    /// Push gyro biases to hardware registers
    i2c_write_byte(MPU9150_ADDRESS, XG_OFFS_USRH, data[0]);
    i2c_write_byte(MPU9150_ADDRESS, XG_OFFS_USRL, data[1]);
    i2c_write_byte(MPU9150_ADDRESS, YG_OFFS_USRH, data[2]);
    i2c_write_byte(MPU9150_ADDRESS, YG_OFFS_USRL, data[3]);
    i2c_write_byte(MPU9150_ADDRESS, ZG_OFFS_USRH, data[4]);
    i2c_write_byte(MPU9150_ADDRESS, ZG_OFFS_USRL, data[5]);

    // Construct gyro bias in deg/s for later manual subtraction
    dest1[0] = (float) gyro_bias[0]/(float) gyrosensitivity;
    dest1[1] = (float) gyro_bias[1]/(float) gyrosensitivity;
    dest1[2] = (float) gyro_bias[2]/(float) gyrosensitivity;

    // Construct the accelerometer biases for push to the hardware accelerometer bias registers. These registers contain
    // factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold
    // non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature
    // compensation calculations. Accelerometer bias registers expect bias input as 2048 LSB per g, so that
    // the accelerometer biases calculated above must be divided by 8.

    int32_t accel_bias_reg[3] = {0, 0, 0}; // A place to hold the factory accelerometer trim biases
    i2c_read_bytes(MPU9150_ADDRESS, XA_OFFSET_H, 2, data); // Read factory accelerometer trim values
    accel_bias_reg[0] = (int16_t) ((int16_t)data[0] << 8) | data[1];
    i2c_read_bytes(MPU9150_ADDRESS, YA_OFFSET_H, 2, data);
    accel_bias_reg[1] = (int16_t) ((int16_t)data[0] << 8) | data[1];
    i2c_read_bytes(MPU9150_ADDRESS, ZA_OFFSET_H, 2, data);
    accel_bias_reg[2] = (int16_t) ((int16_t)data[0] << 8) | data[1];

    uint32_t mask = 1uL; // Define mask for temperature compensation bit 0 of lower byte of accelerometer bias registers
    uint8_t mask_bit[3] = {0, 0, 0}; // Define array to hold mask bit for each accelerometer bias axis

    for(ii = 0; ii < 3; ii++)
        // If temperature compensation bit is set, record that fact in mask_bit
        mask_bit[ii] = accel_bias_reg[ii] & mask;

    // Construct total accelerometer bias, including calculated average accelerometer bias from above
    accel_bias_reg[0] -= (accel_bias[0]/8); // Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g (16 g full scale)
    accel_bias_reg[1] -= (accel_bias[1]/8);
    accel_bias_reg[2] -= (accel_bias[2]/8);

    data[0] = (accel_bias_reg[0] >> 8) & 0xFF;
    data[1] = (accel_bias_reg[0])      & 0xFF;
    data[1] = data[1] | mask_bit[0]; // preserve temperature compensation bit when writing back to accelerometer bias registers
    data[2] = (accel_bias_reg[1] >> 8) & 0xFF;
    data[3] = (accel_bias_reg[1])      & 0xFF;
    data[3] = data[3] | mask_bit[1]; // preserve temperature compensation bit when writing back to accelerometer bias registers
    data[4] = (accel_bias_reg[2] >> 8) & 0xFF;
    data[5] = (accel_bias_reg[2])      & 0xFF;
    data[5] = data[5] | mask_bit[2]; // preserve temperature compensation bit when writing back to accelerometer bias registers

    // Apparently this is not working for the acceleration biases in the MPU-9250
    // Are we handling the temperature correction bit properly?
    // Push accelerometer biases to hardware registers
      i2c_write_byte(MPU9150_ADDRESS, XA_OFFSET_H, data[0]);
      i2c_write_byte(MPU9150_ADDRESS, XA_OFFSET_L_TC, data[1]);
      i2c_write_byte(MPU9150_ADDRESS, YA_OFFSET_H, data[2]);
      i2c_write_byte(MPU9150_ADDRESS, YA_OFFSET_L_TC, data[3]);
      i2c_write_byte(MPU9150_ADDRESS, ZA_OFFSET_H, data[4]);
      i2c_write_byte(MPU9150_ADDRESS, ZA_OFFSET_L_TC, data[5]);

    // Output scaled accelerometer biases for manual subtraction in the main program
    dest2[0] = (float)accel_bias[0]/(float)accelsensitivity;
    dest2[1] = (float)accel_bias[1]/(float)accelsensitivity;
    dest2[2] = (float)accel_bias[2]/(float)accelsensitivity;
}


// Accelerometer and gyroscope self test; check calibration wrt factory settings
// Should return percent deviation from factory trim values, +/- 14 or less deviation is a pass
void mpu9150_selftest(float * destination)
{
    uint8_t rawData[4] = {0, 0, 0, 0};
    uint8_t selfTest[6];
    float factoryTrim[6];

    // Configure the accelerometer for self-test
    i2c_write_byte(MPU9150_ADDRESS, ACCEL_CONFIG, 0xF0); // Enable self test on all three axes and set accelerometer range to +/- 8 g
    i2c_write_byte(MPU9150_ADDRESS, GYRO_CONFIG,  0xE0); // Enable self test on all three axes and set gyro range to +/- 250 degrees/s
    nrf_delay_ms(500);  // Delay a while to let the device execute the self-test
    rawData[0] = i2c_read_byte(MPU9150_ADDRESS, SELF_TEST_X); // X-axis self-test results
    rawData[1] = i2c_read_byte(MPU9150_ADDRESS, SELF_TEST_Y); // Y-axis self-test results
    rawData[2] = i2c_read_byte(MPU9150_ADDRESS, SELF_TEST_Z); // Z-axis self-test results
    rawData[3] = i2c_read_byte(MPU9150_ADDRESS, SELF_TEST_A); // Mixed-axis self-test results
    // Extract the acceleration test results first
    selfTest[0] = (rawData[0] >> 3) | (rawData[3] & 0x30) >> 4 ; // XA_TEST result is a five-bit unsigned integer
    selfTest[1] = (rawData[1] >> 3) | (rawData[3] & 0x0C) >> 4 ; // YA_TEST result is a five-bit unsigned integer
    selfTest[2] = (rawData[2] >> 3) | (rawData[3] & 0x03) >> 4 ; // ZA_TEST result is a five-bit unsigned integer
    // Extract the gyration test results first
    selfTest[3] = rawData[0]  & 0x1F ; // XG_TEST result is a five-bit unsigned integer
    selfTest[4] = rawData[1]  & 0x1F ; // YG_TEST result is a five-bit unsigned integer
    selfTest[5] = rawData[2]  & 0x1F ; // ZG_TEST result is a five-bit unsigned integer
    // Process results to allow final comparison with factory set values
    factoryTrim[0] = (4096.0f*0.34f)*(pow( (0.92f/0.34f) , ((selfTest[0] - 1.0f)/30.0f))); // FT[Xa] factory trim calculation
    factoryTrim[1] = (4096.0f*0.34f)*(pow( (0.92f/0.34f) , ((selfTest[1] - 1.0f)/30.0f))); // FT[Ya] factory trim calculation
    factoryTrim[2] = (4096.0f*0.34f)*(pow( (0.92f/0.34f) , ((selfTest[2] - 1.0f)/30.0f))); // FT[Za] factory trim calculation
    factoryTrim[3] =  ( 25.0f*131.0f)*(pow( 1.046f , (selfTest[3] - 1.0f) ));             // FT[Xg] factory trim calculation
    factoryTrim[4] =  (-25.0f*131.0f)*(pow( 1.046f , (selfTest[4] - 1.0f) ));             // FT[Yg] factory trim calculation
    factoryTrim[5] =  ( 25.0f*131.0f)*(pow( 1.046f , (selfTest[5] - 1.0f) ));             // FT[Zg] factory trim calculation

    //  Output self-test results and factory trim calculation if desired
    //  Serial.println(selfTest[0]); Serial.println(selfTest[1]); Serial.println(selfTest[2]);
    //  Serial.println(selfTest[3]); Serial.println(selfTest[4]); Serial.println(selfTest[5]);
    //  Serial.println(factoryTrim[0]); Serial.println(factoryTrim[1]); Serial.println(factoryTrim[2]);
    //  Serial.println(factoryTrim[3]); Serial.println(factoryTrim[4]); Serial.println(factoryTrim[5]);

    // Report results as a ratio of (STR - FT)/FT; the change from Factory Trim of the Self-Test Response
    // To get to percent, must multiply by 100 and subtract result from 100
    for (int i = 0; i < 6; i++) {
        destination[i] = 100.0f + 100.0f*(selfTest[i] - factoryTrim[i])/factoryTrim[i]; // Report percent differences
    }
}



// Implementation of Sebastian Madgwick's "...efficient orientation filter for... inertial/magnetic sensor arrays"
// (see http://www.x-io.co.uk/category/open-source/ for examples and more details)
// which fuses acceleration, rotation rate, and magnetic moments to produce a quaternion-based estimate of absolute
// device orientation -- which can be converted to yaw, pitch, and roll. Useful for stabilizing quadcopters, etc.
// The performance of the orientation filter is at least as good as conventional Kalman-based filtering algorithms
// but is much less computationally intensive---it can be performed on a 3.3 V Pro Mini operating at 8 MHz!
void madgwick_quaternion_update(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
{
    float q1 = q[0], q2 = q[1], q3 = q[2], q4 = q[3];   // short name local variable for readability
    float norm;
    float hx, hy, _2bx, _2bz;
    float s1, s2, s3, s4;
    float qDot1, qDot2, qDot3, qDot4;

    // Auxiliary variables to avoid repeated arithmetic
    float _2q1mx;
    float _2q1my;
    float _2q1mz;
    float _2q2mx;
    float _4bx;
    float _4bz;
    float _2q1 = 2.0f * q1;
    float _2q2 = 2.0f * q2;
    float _2q3 = 2.0f * q3;
    float _2q4 = 2.0f * q4;
    float _2q1q3 = 2.0f * q1 * q3;
    float _2q3q4 = 2.0f * q3 * q4;
    float q1q1 = q1 * q1;
    float q1q2 = q1 * q2;
    float q1q3 = q1 * q3;
    float q1q4 = q1 * q4;
    float q2q2 = q2 * q2;
    float q2q3 = q2 * q3;
    float q2q4 = q2 * q4;
    float q3q3 = q3 * q3;
    float q3q4 = q3 * q4;
    float q4q4 = q4 * q4;

    // Normalise accelerometer measurement
    norm = sqrt(ax * ax + ay * ay + az * az);
    if (norm == 0.0f) return; // handle NaN
    norm = 1.0f/norm;
    ax *= norm;
    ay *= norm;
    az *= norm;

    // Normalise magnetometer measurement
    norm = sqrt(mx * mx + my * my + mz * mz);
    if (norm == 0.0f) return; // handle NaN
    norm = 1.0f/norm;
    mx *= norm;
    my *= norm;
    mz *= norm;

    // Reference direction of Earth's magnetic field
    _2q1mx = 2.0f * q1 * mx;
    _2q1my = 2.0f * q1 * my;
    _2q1mz = 2.0f * q1 * mz;
    _2q2mx = 2.0f * q2 * mx;
    hx = mx * q1q1 - _2q1my * q4 + _2q1mz * q3 + mx * q2q2 + _2q2 * my * q3 + _2q2 * mz * q4 - mx * q3q3 - mx * q4q4;
    hy = _2q1mx * q4 + my * q1q1 - _2q1mz * q2 + _2q2mx * q3 - my * q2q2 + my * q3q3 + _2q3 * mz * q4 - my * q4q4;
    _2bx = sqrt(hx * hx + hy * hy);
    _2bz = -_2q1mx * q3 + _2q1my * q2 + mz * q1q1 + _2q2mx * q4 - mz * q2q2 + _2q3 * my * q4 - mz * q3q3 + mz * q4q4;
    _4bx = 2.0f * _2bx;
    _4bz = 2.0f * _2bz;

    // Gradient decent algorithm corrective step
    s1 = -_2q3 * (2.0f * q2q4 - _2q1q3 - ax) + _2q2 * (2.0f * q1q2 + _2q3q4 - ay) - _2bz * q3 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q4 + _2bz * q2) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q3 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s2 = _2q4 * (2.0f * q2q4 - _2q1q3 - ax) + _2q1 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q2 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + _2bz * q4 * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q3 + _2bz * q1) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q4 - _4bz * q2) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s3 = -_2q1 * (2.0f * q2q4 - _2q1q3 - ax) + _2q4 * (2.0f * q1q2 + _2q3q4 - ay) - 4.0f * q3 * (1.0f - 2.0f * q2q2 - 2.0f * q3q3 - az) + (-_4bx * q3 - _2bz * q1) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q2 + _2bz * q4) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q1 - _4bz * q3) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    s4 = _2q2 * (2.0f * q2q4 - _2q1q3 - ax) + _2q3 * (2.0f * q1q2 + _2q3q4 - ay) + (-_4bx * q4 + _2bz * q2) * (_2bx * (0.5f - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q1 + _2bz * q3) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q2 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5f - q2q2 - q3q3) - mz);
    norm = sqrt(s1 * s1 + s2 * s2 + s3 * s3 + s4 * s4);    // normalise step magnitude
    norm = 1.0f/norm;
    s1 *= norm;
    s2 *= norm;
    s3 *= norm;
    s4 *= norm;

    // Compute rate of change of quaternion
    qDot1 = 0.5f * (-q2 * gx - q3 * gy - q4 * gz) - beta * s1;
    qDot2 = 0.5f * (q1 * gx + q3 * gz - q4 * gy) - beta * s2;
    qDot3 = 0.5f * (q1 * gy - q2 * gz + q4 * gx) - beta * s3;
    qDot4 = 0.5f * (q1 * gz + q2 * gy - q3 * gx) - beta * s4;

    // Integrate to yield quaternion
    q1 += qDot1 * deltat;
    q2 += qDot2 * deltat;
    q3 += qDot3 * deltat;
    q4 += qDot4 * deltat;
    norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);    // normalise quaternion
    norm = 1.0f/norm;
    q[0] = q1 * norm;
    q[1] = q2 * norm;
    q[2] = q3 * norm;
    q[3] = q4 * norm;

}



// Similar to Madgwick scheme but uses proportional and integral filtering on the error between estimated reference vectors and
// measured ones.
void mahony_quaternion_update(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
{
    float q1 = q[0], q2 = q[1], q3 = q[2], q4 = q[3];   // short name local variable for readability
    float norm;
    float hx, hy, bx, bz;
    float vx, vy, vz, wx, wy, wz;
    float ex, ey, ez;
    float pa, pb, pc;

    // Auxiliary variables to avoid repeated arithmetic
    float q1q1 = q1 * q1;
    float q1q2 = q1 * q2;
    float q1q3 = q1 * q3;
    float q1q4 = q1 * q4;
    float q2q2 = q2 * q2;
    float q2q3 = q2 * q3;
    float q2q4 = q2 * q4;
    float q3q3 = q3 * q3;
    float q3q4 = q3 * q4;
    float q4q4 = q4 * q4;

    // Normalise accelerometer measurement
    norm = sqrt(ax * ax + ay * ay + az * az);
    if (norm == 0.0f) return; // handle NaN
    norm = 1.0f / norm;        // use reciprocal for division
    ax *= norm;
    ay *= norm;
    az *= norm;

    // Normalise magnetometer measurement
    norm = sqrt(mx * mx + my * my + mz * mz);
    if (norm == 0.0f) return; // handle NaN
    norm = 1.0f / norm;        // use reciprocal for division
    mx *= norm;
    my *= norm;
    mz *= norm;

    // Reference direction of Earth's magnetic field
    hx = 2.0f * mx * (0.5f - q3q3 - q4q4) + 2.0f * my * (q2q3 - q1q4) + 2.0f * mz * (q2q4 + q1q3);
    hy = 2.0f * mx * (q2q3 + q1q4) + 2.0f * my * (0.5f - q2q2 - q4q4) + 2.0f * mz * (q3q4 - q1q2);
    bx = sqrt((hx * hx) + (hy * hy));
    bz = 2.0f * mx * (q2q4 - q1q3) + 2.0f * my * (q3q4 + q1q2) + 2.0f * mz * (0.5f - q2q2 - q3q3);

    // Estimated direction of gravity and magnetic field
    vx = 2.0f * (q2q4 - q1q3);
    vy = 2.0f * (q1q2 + q3q4);
    vz = q1q1 - q2q2 - q3q3 + q4q4;
    wx = 2.0f * bx * (0.5f - q3q3 - q4q4) + 2.0f * bz * (q2q4 - q1q3);
    wy = 2.0f * bx * (q2q3 - q1q4) + 2.0f * bz * (q1q2 + q3q4);
    wz = 2.0f * bx * (q1q3 + q2q4) + 2.0f * bz * (0.5f - q2q2 - q3q3);

    // Error is cross product between estimated direction and measured direction of gravity
    ex = (ay * vz - az * vy) + (my * wz - mz * wy);
    ey = (az * vx - ax * vz) + (mz * wx - mx * wz);
    ez = (ax * vy - ay * vx) + (mx * wy - my * wx);
    if (Ki > 0.0f)
        {
            eInt[0] += ex;      // accumulate integral error
            eInt[1] += ey;
            eInt[2] += ez;
        }
    else
        {
            eInt[0] = 0.0f;     // prevent integral wind up
            eInt[1] = 0.0f;
            eInt[2] = 0.0f;
        }

    // Apply feedback terms
    gx = gx + Kp * ex + Ki * eInt[0];
    gy = gy + Kp * ey + Ki * eInt[1];
    gz = gz + Kp * ez + Ki * eInt[2];

    // Integrate rate of change of quaternion
    pa = q2;
    pb = q3;
    pc = q4;
    q1 = q1 + (-q2 * gx - q3 * gy - q4 * gz) * (0.5f * deltat);
    q2 = pa + (q1 * gx + pb * gz - pc * gy) * (0.5f * deltat);
    q3 = pb + (q1 * gy - pa * gz + pc * gx) * (0.5f * deltat);
    q4 = pc + (q1 * gz + pa * gy - pb * gx) * (0.5f * deltat);

    // Normalise quaternion
    norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);
    norm = 1.0f / norm;
    q[0] = q1 * norm;
    q[1] = q2 * norm;
    q[2] = q3 * norm;
    q[3] = q4 * norm;

}

// XXX FIXME : this need to be split up inn several functions
// and calibration data need to be stored in flash
void mpu9150_mainloop()
{
    // Check that MPU9150 is reponding
    mpu9150_reset();
    uint8_t whoami = i2c_read_byte(MPU9150_ADDRESS, WHO_AM_I_MPU9150);
    printf("MPU9150 : I am 0x%x\n\r", whoami);

    if (whoami != 0x68) {
            // WHO_AM_I should be 0x68
            printf("ERROR : I SHOULD BE 0x68\n\r");
            return;
        }
    printf("MPU9150 is online...\n\r");

    // Init MPU
    mpu9150_init();

#ifdef MPU9150_SELFTEST
    // Run self tests
    printf("Running MPU9150 self tests...\r\n");
    static float SelfTest[6];
    mpu9150_selftest(SelfTest);
    printf("x-axis self test: acceleration trim within %2.2f%% of factory value\n\r", SelfTest[0]);
    printf("y-axis self test: acceleration trim within %2.2f%% of factory value\n\r", SelfTest[1]);
    printf("z-axis self test: acceleration trim within %2.2f%% of factory value\n\r", SelfTest[2]);
    printf("x-axis self test: gyration trim within %2.2f%% of factory value\n\r", SelfTest[3]);
    printf("y-axis self test: gyration trim within %2.2f%% of factory value\n\r", SelfTest[4]);
    printf("z-axis self test: gyration trim within %2.2f%% of factory value\n\r", SelfTest[5]);

    // Re-init MPU
    mpu9150_init();
#endif

    // Calibrate accel and gyro MPU9150
    // XXX FIXME : calibrate magnetometer
    mpu9150_calibrate(gyroBias, accelBias);
    printf("x gyro bias = %f\n\r", gyroBias[0]);
    printf("y gyro bias = %f\n\r", gyroBias[1]);
    printf("z gyro bias = %f\n\r", gyroBias[2]);
    printf("x accel bias = %f\n\r", accelBias[0]);
    printf("y accel bias = %f\n\r", accelBias[1]);
    printf("z accel bias = %f\n\r", accelBias[2]);
    printf("\r\n");

    // Go on !
    printf("MPU9150 initialized for active data mode....\n\r");

    // Initialize device for active mode read of magnetometer
    ak8975a_init(magCalibration);
    printf("AK8975 initialized for active data mode....\n\r");

}
