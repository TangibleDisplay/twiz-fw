/**
   From : https://github.com/kriswiner/STM32F401
   TODO : see licensing restrictions and directives !
*/

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>

#include "mpu9150.h"
#include "i2c_wrapper.h"
#include "nrf_delay.h"
#include "printf.h"
#include "nordic_common.h"
#include "high_res_timer.h"
#include "app_error.h"
#include "twi_advertising.h"
#include "fusion.h"

// Define registers per MPU6050, Register Map and Descriptions, Rev 4.2, 08/19/2013 6 DOF Motion sensor fusion device
// Invensense Inc., www.invensense.com
// See also MPU-9150 Register Map and Descriptions, Revision 4.0, RM-MPU-9150A-00, 9/12/2012 for registers not listed in
// above document; the MPU6050 and MPU 9150 are virtually identical but the latter has an on-board magnetic sensor
//
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
static bool calibrated = false;

// Pin definitions
// XXX FIXME int intPin = 12;  // These can be changed, 2 and 3 are the Arduinos ext int pins

static float magCalibration[3] = {0, 0, 0}, magBias[3] = {0, 0, 0};  // Factory mag calibration and mag bias
static float gyroBias[3] = {0, 0, 0}, accelBias[3] = {0, 0, 0}; // Bias corrections for gyro and accelerometer
static float ax, ay, az, gx, gy, gz, mx, my, mz; // variables to hold latest sensor data values
static float temperature;

// parameters for 6 DoF sensor fusion calculations
static float PI = 3.14159265358979323846f;
// gyroscope measurement error in rads/s (start at 60 deg/s), then reduce after ~10 s to 3
// PI * (60.0f / 180.0f);

static float pitch, yaw, roll;
// integration interval for both filter schemes
static float deltat = 0.0f;

// vector to hold quaternion
static float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};

// set global variables : yaw, pitch and roll
static inline void update_euler_from_quaternions(void);



static void mpu9150_reset() {
    // Write a one to bit 7 reset bit; toggle reset device
    i2c_write_byte(MPU9150_ADDRESS, PWR_MGMT_1, 0x80);
    while(i2c_read_byte(MPU9150_ADDRESS, PWR_MGMT_1) & 0x80) ;
    nrf_delay_ms(200);
}

void mpu9150_init()
{
    uint8_t whoami = i2c_read_byte(MPU9150_ADDRESS, WHO_AM_I_MPU9150);
    if (whoami != 0x68) {
        // WHO_AM_I should be 0x68
        printf("ERROR : I SHOULD BE 0x68\n\r");
        APP_ERROR_CHECK_BOOL(false);
    }

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
    // Enable I2C_BYPASS_EN so magnetometer can join the I2C bus and can be controlled
    // by the TWI as master
    i2c_write_byte(MPU9150_ADDRESS, INT_PIN_CFG, 0x22);
    i2c_write_byte(MPU9150_ADDRESS, INT_ENABLE, 0x01);  // Enable data ready (bit 0) interrupt
}

// Function which accumulates gyro and accelerometer data after device initialization. It calculates the average
// of the at-rest readings and then loads the resulting offsets into accelerometer and gyro bias registers.
void mpu9150_calibrate()
{
    static uint8_t data[14]; // data array to hold accelerometer and gyro x, y, z, data
    int ii, meas_count;
    int32_t gyro_bias[3] = {0, 0, 0}, accel_bias[3] = {0, 0, 0};
    static int32_t gyro_temp[3];
    static int32_t accel_temp[3];

    printf("Calibrating MPU9150. Please don't move !\r\n");

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
#if 0
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
    gyroBias[0] = (float) gyro_bias[0]/(float) gyrosensitivity;
    gyroBias[1] = (float) gyro_bias[1]/(float) gyrosensitivity;
    gyroBias[2] = (float) gyro_bias[2]/(float) gyrosensitivity;

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

    // Push accelerometer biases to hardware registers
    i2c_write_byte(MPU9150_ADDRESS, XA_OFFSET_H, data[0]);
    i2c_write_byte(MPU9150_ADDRESS, XA_OFFSET_L_TC, data[1]);
    i2c_write_byte(MPU9150_ADDRESS, YA_OFFSET_H, data[2]);
    i2c_write_byte(MPU9150_ADDRESS, YA_OFFSET_L_TC, data[3]);
    i2c_write_byte(MPU9150_ADDRESS, ZA_OFFSET_H, data[4]);
    i2c_write_byte(MPU9150_ADDRESS, ZA_OFFSET_L_TC, data[5]);

    // Output scaled accelerometer biases for manual subtraction in the main program
    accelBias[0] = (float)accel_bias[0]/(float)accelsensitivity;
    accelBias[1] = (float)accel_bias[1]/(float)accelsensitivity;
    accelBias[2] = (float)accel_bias[2]/(float)accelsensitivity;

    printf("x gyro bias = %f\n\r", gyroBias[0]);
    printf("y gyro bias = %f\n\r", gyroBias[1]);
    printf("z gyro bias = %f\n\r", gyroBias[2]);
    printf("x accel bias = %f\n\r", accelBias[0]);
    printf("y accel bias = %f\n\r", accelBias[1]);
    printf("z accel bias = %f\n\r", accelBias[2]);
    printf("\r\n");

    printf("Calibrating MPU9150 done.\r\n");
}


// Accelerometer and gyroscope self test; check calibration wrt factory settings
// Should return percent deviation from factory trim values, +/- 14 or less deviation is a pass
void mpu9150_selftest()
{
    static uint8_t rawData[4] = {0, 0, 0, 0};
    static uint8_t selfTest[6];
    static float factoryTrim[6];

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

    // Report results as a ratio of (STR - FT)/FT; the change from Factory Trim of the Self-Test Response
    // To get to percent, must multiply by 100 and subtract result from 100
    for (int i = 0; i < 6; i++) {
        selfTest[i] = 100.0f + 100.0f*(selfTest[i] - factoryTrim[i])/factoryTrim[i]; // Report percent differences
    }

    printf("x-axis self test: acceleration trim within %2.2f%% of factory value\n\r", selfTest[0]);
    printf("y-axis self test: acceleration trim within %2.2f%% of factory value\n\r", selfTest[1]);
    printf("z-axis self test: acceleration trim within %2.2f%% of factory value\n\r", selfTest[2]);
    printf("x-axis self test: gyration trim within %2.2f%% of factory value\n\r", selfTest[3]);
    printf("y-axis self test: gyration trim within %2.2f%% of factory value\n\r", selfTest[4]);
    printf("z-axis self test: gyration trim within %2.2f%% of factory value\n\r", selfTest[5]);

    // Re-init MPU
    mpu9150_init();
}

// Read accel, temps and gyro values.
static void mpu9150_read_data(int16_t * values)
{
    static uint8_t data[14];

    // Burst read all sensors to ensure the same timestamp for everybody
    i2c_read_bytes(MPU9150_ADDRESS, ACCEL_XOUT_H, 14, data);

    // Convert each 2 byte into signed 16bit values
    // XXX FIXME : WARNING, accel axis seems to be inconsistent with the datasheet (all signs are reversed)
    // Hence, the "-...." on the accel values
    for(int i=0; i<3; i++)
        values[i] = -(int16_t)(((int16_t)data[2*i] << 8) | data[2*i+1]) ;
    for(int i=3; i<7; i++)
        values[i] = (int16_t)(((int16_t)data[2*i] << 8) | data[2*i+1]) ;

#if 0
    for (int j=0; j<14; j++)
        printf("%02x ", (uint8_t)data[j]);
    printf("\r\n");
#endif

#if 0
    for (int j=0; j<7; j++)
        printf("%08x ", values[j]);
    printf("\r\n");
#endif
}


// XXX FIXME : this need to be split up in several functions
// and calibration data need to be stored in flash
void mpu9150_update()
{
    // Used to calculate integration interval
    static int lastUpdate = 0, Now = 0;
    // Used to display not so often
    static int lastDisplay = 0;

    // If intPin goes high or NEW_DATA register is set, then all data registers have new data
    // XXX FIXME : should do this in interrupt service
    if (i2c_read_byte(MPU9150_ADDRESS, INT_STATUS) & 0x01) {
        static int16_t data[7];

        // Read accel, temp and gyro data
        mpu9150_read_data(data);

        // Biases are removed by hardware if calibrate routine has been called
        ax = (float)data[0];
        ay = (float)data[1];
        az = (float)data[2];

        // Biases are removed by hardware if calibrate routine has been called.
        // Gyro values need to be scaled. Here we are at 250dps for full scale
        gx = (float)data[4]*250.0/32768.0*PI/180.0;
        gy = (float)data[5]*250.0/32768.0*PI/180.0;
        gz = (float)data[6]*250.0/32768.0*PI/180.0;

        // Temperature (not used)
        temperature = data[4];

        // Get mag data
        ak8975a_read_data(&mx, &my, &mz);
    }

    // Get integration time by time elapsed since last filter update
    Now = get_time();
    deltat = (float)((Now - lastUpdate)/1000000.0f) ;
    lastUpdate = Now;

    madgwick_quaternion_update(ax, ay, az, gx, gy, gz, mx, my, mz, deltat, q);

#if 0
    printf("ax=%04.2f, ay=%04.2f, az=%04.2f, gx=%04.2f, gy=%04.2f, gz=%04.2f, mx=%04.2f, my=%04.2f, mz=%04.2f\r\n",
           ax, ay, az, gx, gy, gz, mx, my, mz);
#endif

    // Display 10 times/s
    if((Now-lastDisplay) > 20000) {
        lastDisplay = Now;

        //update_euler_from_quaternions();
    }
}

// set global variables : yaw, pitch and roll
static inline void update_euler_from_quaternions(void)
{
    // Define output variables from updated quaternion---these are Tait-Bryan angles, commonly used in aircraft orientation.
    // In this coordinate system, the positive z-axis is down toward Earth.
    // Yaw is the angle between Sensor x-axis and Earth magnetic North (or true North if corrected for local declination, looking
    // down on the sensor positive yaw is counterclockwise.
    // Pitch is angle between sensor x-axis and Earth ground plane, toward the Earth is positive, up toward the sky is negative.
    // Roll is angle between sensor y-axis and Earth ground plane, y-axis up is positive roll.
    // These arise from the definition of the homogeneous rotation matrix constructed from quaternions.
    // Tait-Bryan angles as well as Euler angles are non-commutative; that is, the get the correct orientation the rotations must be
    // applied in the correct order which for this configuration is yaw, pitch, and then roll.
    // For more see http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles which has additional links.
    yaw   = atan2(2.0f * (q[1] * q[2] + q[0] * q[3]), q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3]);
    pitch = -asin(2.0f * (q[1] * q[3] - q[0] * q[2]));
    roll  = atan2(2.0f * (q[0] * q[1] + q[2] * q[3]), q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3]);
    pitch *= 180.0f / PI;
    yaw   *= 180.0f / PI;
    yaw   -= 4.11f; // Declination at Paris, 2014
    roll  *= 180.0f / PI;
    //printf("Yaw, Pitch, Roll: %3.2f %3.2f %3.2f\n\r", yaw, pitch, roll);
}

void imu_init(void)
{
    // Init I2C
    i2c_init();
    nrf_delay_ms(300); // TODO: check if it can be faster

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
