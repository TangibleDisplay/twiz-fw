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
#include "app_error.h"
#include "imu.h"

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

void mpu9150_reset() {
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


// Read accel and gyro raw values.
static void mpu9150_read_raw_data(int16_t * values)
{
    static uint8_t data[14];

    // Burst read all sensors to ensure the same timestamp for everybody
    i2c_read_bytes(MPU9150_ADDRESS, ACCEL_XOUT_H, 14, data);

#if 0
    for (int j=0; j<14; j++)
        printf("0x%02x ", (uint8_t)data[j]);
    printf("\r\n");
#endif

    // Convert each 2 byte into signed 16bit values
    // WARNING : code valid in little endian only !
    uint8_t *p;
    for(int i=0; i<3; i++) {
        p = ((uint8_t*)&values[i])+1;
        *p = data[2*i];
        p = (uint8_t*)&values[i];
        *p = data[2*i+1];
    }
    for(int i=3; i<6; i++) {
        p = ((uint8_t*)&values[i])+1;
        *p = data[2*i+2];
        p = (uint8_t*)&values[i];
        *p = data[2*i+3];
    }

#if 0
    for (int j=0; j<6; j++)
        printf("0x%04x ", (int16_t) values[j]);
    printf("\r\n");
#endif
}

// Read accel, temps and gyro raw values.
void mpu9150_read_data(float * values)
{
    int16_t data[6];

    // Read raw data
    mpu9150_read_raw_data(data);
#if 0
    printf("raw = %d %d %d %d %d %d\r\n",
           (int)data[0], (int)data[1], (int)data[2],
           data[3], data[4], data[5]);
#endif

    // XXX FIXME : WARNING, accel axis seems to be inconsistent with the datasheet (all signs are reversed)
    // Hence, the "-...." on the accel values
    for(int i=0; i<3; i++)
        data[i] = -data[i];

    // Apply correction (bias and gain for gyroscope)
    for(int i=0; i<3; i++) {
        values[i] = 1.*data[i] - cal.accel_bias[i];
        // Convert gyro in rad/s
        values[i+3] = (data[i+3] - cal.gyro_bias[i]) * 250.0 * M_PI / 180. / 32768.0;
    }
}



// Return true if a new measure is available
bool mpu9150_new_data()
{
    return i2c_read_byte(MPU9150_ADDRESS, INT_STATUS) & 0x01;
}


// Function which accumulates gyro and accelerometer data. It calculates the average
// of the at-rest readings and then store them in gyro_bias and accel_bias variables.
void mpu9150_measure_biases()
{
    static float data[6]; // data array to hold accelerometer and gyro x, y, z data
    static float gyro_bias[3] = {0, 0, 0};
    static float accel_bias[3] = {0, 0, 0};

    // Reset chip to reset HW cal register to factory trim
    mpu9150_reset();
    mpu9150_init();

    // Reset calibration data
    accel_bias[0] = 0;
    accel_bias[1] = 0;
    accel_bias[2] = 0;
    gyro_bias[0] = 0;
    gyro_bias[1] = 0;
    gyro_bias[2] = 0;

    cal.accel_bias[0] = 0;
    cal.accel_bias[1] = 0;
    cal.accel_bias[2] = 0;
    cal.gyro_bias[0] = 0;
    cal.gyro_bias[1] = 0;
    cal.gyro_bias[2] = 0;

    // Configure MPU9150 gyro and accelerometer for bias calculation
    // Set gyro full-scale to 250 degrees per second, maximum sensitivity
    i2c_write_byte(MPU9150_ADDRESS, GYRO_CONFIG, 0x00);
    // Set accelerometer full-scale to 2 g, maximum sensitivity
    i2c_write_byte(MPU9150_ADDRESS, ACCEL_CONFIG, 0x00);
    nrf_delay_ms(200);

    // Accumulate 200 measures each 5 ms (200Hz sample rate)
    const int meas_count = 200;
    for (int i = 0; i<meas_count; i++) {
        // Pause for 5ms
        nrf_delay_ms(5);

        // Get new measurement
        mpu9150_read_data(data);

#if 0
        // Debug
        printf("measure biases : ");
        for (int j=0; j<6; j=j+1)
            printf("%f ", data[j]);
        printf("\r\n");
#endif

        // Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
        accel_bias[0] += data[0];
        accel_bias[1] += data[1];
        accel_bias[2] += data[2];
        gyro_bias[0]  += data[3];
        gyro_bias[1]  += data[4];
        gyro_bias[2]  += data[5];

    }
    // Normalize sums to get average count biases
    cal.accel_bias[0] = accel_bias[0] / meas_count;
    cal.accel_bias[1] = accel_bias[1] / meas_count;
    cal.accel_bias[2] = accel_bias[2] / meas_count;
    cal.gyro_bias[0]  = gyro_bias[0] / meas_count;
    cal.gyro_bias[1]  = gyro_bias[1] / meas_count;
    cal.gyro_bias[2]  = gyro_bias[2] / meas_count;

    // Remove gravity from the z-axis accelerometer bias calculation
    const float  accelsensitivity = 16384.;  // = 16384 LSB/g

    if(cal.accel_bias[2] > 0L)
        cal.accel_bias[2] -= accelsensitivity;
    else
        cal.accel_bias[2] += accelsensitivity;
}
