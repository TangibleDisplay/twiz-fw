#ifndef MPU_H
#define MPU_H

#define MPU9150_WAI_DEFAULT         0x68

#define MPU9150_WR_ADDR             0b11010000
#define MPU9150_RD_ADDR             0b11010001

/** This typdef contains the Accelerometer and Gyro register addresses **/

#define MPU9150_SELF_TEST_X           13
#define MPU9150_SELF_TEST_Y           14
#define MPU9150_SELF_TEST_Z           15
#define MPU9150_SELF_TEST_A           16
#define MPU9150_SMPLRT_DIV            25
#define MPU9150_CONFIG                26
#define MPU9150_GYRO_CONFIG           27
#define MPU9150_ACCEL_CONFIG          28
#define MPU9150_FIFO_EN               35
#define MPU9150_I2C_MST_CTRL          36
#define MPU9150_I2C_SLV0_ADDR         37
#define MPU9150_I2C_SLV0_REG          38
#define MPU9150_I2C_SLV0_CTRL         39
#define MPU9150_I2C_SLV1_ADDR         40
#define MPU9150_I2C_SLV1_REG          41
#define MPU9150_I2C_SLV1_CTRL         42
#define MPU9150_I2C_SLV2_ADDR         43
#define MPU9150_I2C_SLV2_REG          44
#define MPU9150_I2C_SLV2_CTRL         45
#define MPU9150_I2C_SLV3_ADDR         46
#define MPU9150_I2C_SLV3_REG          47
#define MPU9150_I2C_SLV3_CTRL         48
#define MPU9150_I2C_SLV4_ADDR         49
#define MPU9150_I2C_SLV4_REG          50
#define MPU9150_I2C_SLV4_DO           51
#define MPU9150_I2C_SLV4_CTRL         52
#define MPU9150_I2C_SLV4_DI           53
#define MPU9150_I2C_MST_STATUS        54
#define MPU9150_INT_PIN_CFG           55
#define MPU9150_INT_ENABLE            56
#define MPU9150_INT_STATUS            58
#define MPU9150_ACCEL_XOUT_H          59
#define MPU9150_ACCEL_XOUT_L          60
#define MPU9150_ACCEL_YOUT_H          61
#define MPU9150_ACCEL_YOUT_L          62
#define MPU9150_ACCEL_ZOUT_H          63
#define MPU9150_ACCEL_ZOUT_L          64
#define MPU9150_TEMP_OUT_H            65
#define MPU9150_TEMP_OUT_L            66
#define MPU9150_GYRO_XOUT_H           67
#define MPU9150_GYRO_XOUT_L           68
#define MPU9150_GYRO_YOUT_H           69
#define MPU9150_GYRO_YOUT_L           70
#define MPU9150_GYRO_ZOUT_H           71
#define MPU9150_GYRO_ZOUT_L           72
#define MPU9150_EXT_SENS_DATA_00      73
#define MPU9150_EXT_SENS_DATA_01      74
#define MPU9150_EXT_SENS_DATA_02      75
#define MPU9150_EXT_SENS_DATA_03      76
#define MPU9150_EXT_SENS_DATA_04      77
#define MPU9150_EXT_SENS_DATA_05      78
#define MPU9150_EXT_SENS_DATA_06      79
#define MPU9150_EXT_SENS_DATA_07      80
#define MPU9150_EXT_SENS_DATA_08      81
#define MPU9150_EXT_SENS_DATA_09      82
#define MPU9150_EXT_SENS_DATA_10      83
#define MPU9150_EXT_SENS_DATA_11      84
#define MPU9150_EXT_SENS_DATA_12      85
#define MPU9150_EXT_SENS_DATA_13      86
#define MPU9150_EXT_SENS_DATA_14      87
#define MPU9150_EXT_SENS_DATA_15      88
#define MPU9150_EXT_SENS_DATA_16      89
#define MPU9150_EXT_SENS_DATA_17      90
#define MPU9150_EXT_SENS_DATA_18      91
#define MPU9150_EXT_SENS_DATA_19      92
#define MPU9150_EXT_SENS_DATA_20      93
#define MPU9150_EXT_SENS_DATA_21      94
#define MPU9150_EXT_SENS_DATA_22      95
#define MPU9150_EXT_SENS_DATA_23      96
#define MPU9150_I2C_SLV0_DO           99
#define MPU9150_I2C_SLV1_DO           100
#define MPU9150_I2C_SLV2_DO           101
#define MPU9150_I2C_SLV3_DO           102
#define MPU9150_I2C_MST_DELAY_CTRL    103
#define MPU9150_SIGNAL_PATH_RESET     104
#define MPU9150_USER_CTRL             106
#define MPU9150_PWR_MGMT_1            107
#define MPU9150_PWR_MGMT_2            108
#define MPU9150_FIFO_COUNTH           114
#define MPU9150_FIFO_COUNTL           115
#define MPU9150_FIFO_R_W              116
#define MPU9150_WHO_AM_I              117


/** This typdef contains the Magnetometer register addresses **/

#define MPU9150_MAG_WIA             0x00 //RO - Device ID
#define MPU9150_MAG_INFO                    0X01 //RO - Information
#define MPU9150_MAG_ST1                     0X02 //RO - Status 1
#define MPU9150_MAG_HXL                     0X03 //RO - Measurement Data
#define MPU9150_MAG_HXH                     0X04 //RO - Measurement Data
#define MPU9150_MAG_HYL                     0X05 //RO - Measurement Data
#define MPU9150_MAG_HYH                     0X06 //RO - Measurement Data
#define MPU9150_MAG_HZL                     0X07 //RO - Measurement Data
#define MPU9150_MAG_HZH                     0X08 //RO - Measurement Data
#define MPU9150_MAG_ST2                     0X09 //RO - Status 2
#define MPU9150_MAG_CNTL                    0X0A //RW - Control
#define MPU9150_MAG_RSV                     0X0B //RESERVED
#define MPU9150_MAG_ASTC                    0X0C //RW - Self Test (mag address)
#define MPU9150_MAG_TS1                     0X0D //RW - Test 1
#define MPU9150_MAG_TS2                     0X0E //RW - Test 2
#define MPU9150_MAG_I2CDIS                  0X0F //RW - I2C disable
#define MPU9150_MAG_ASAX                    0X10 //RO - X-axis sensitivity
#define MPU9150_MAG_ASAY                    0X11 //RO - Y-axis sensitivity
#define MPU9150_MAG_ASAZ                    0X12 //RO - Z-axis sensitivity

/** Bit Positions **/
#define MP9150_PWR_MGT_1_CLKSEL             0
#define MP9150_PWR_MGT_1_TEMP_DIS           3
#define MP9150_PWR_MGT_1_CYCLE              5
#define MP9150_PWR_MGT_1_SLEEP              6
#define MP9150_PWR_MGT_1_RESET              7

//Magnetometer Registers
#define MPU9150_MAG_ADDRESS		0x0C
#define MPU9150_MAG_XOUT_L		0x03
#define MPU9150_MAG_XOUT_H		0x04
#define MPU9150_MAG_YOUT_L		0x05
#define MPU9150_MAG_YOUT_H		0x06
#define MPU9150_MAG_ZOUT_L		0x07
#define MPU9150_MAG_ZOUT_H		0x08

/** mpu9150 functions **/

bool mpuInit(void);

static uint8_t mpuBuffer[16];

#endif // MPU_H
