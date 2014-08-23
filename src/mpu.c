#ifndef MPU_C
#define MPU_C

#include <string.h>
#include "ble_uart.h"
#include "twi_master.h"
#include "mpu.h"
#include "nrf_delay.h"

#define TWI_RETRIES 3

bool mpuWriteFromBuffer(uint8_t* buffer, uint16_t num, bool cond)
{
    bool write_stat = false;
    for (int i=0; i<TWI_RETRIES; i++) {
        write_stat = twi_master_transfer(MPU9150_WR_ADDR, buffer, num, cond);
        if(write_stat) {
            break;
        }
    }
    return write_stat;
}

bool mpuReadToBuffer(uint8_t* buffer, uint8_t num, bool cond)
{
    bool read_stat = false;
    for (int i=0; i<TWI_RETRIES; i++) {
        read_stat = twi_master_transfer(MPU9150_RD_ADDR, buffer, num, cond);
        if(read_stat) {
            break;
        }
    }
    return read_stat;
}

bool mpuInit()
{
    if(!twi_master_init()) {
        print("Error: mpu init fail\n");
        return false;
    }
    nrf_delay_ms(6);

    memset(mpuBuffer, 0, sizeof(mpuBuffer));

    mpuBuffer[0] = MPU9150_WHO_AM_I;
    if(!mpuWriteFromBuffer(mpuBuffer, 1, TWI_DONT_ISSUE_STOP)) {
        print("Error: mpu write fail\n");
        return false;
    }

    if(!mpuReadToBuffer(mpuBuffer+1, 1, TWI_ISSUE_STOP)) {
        print("Error: mpu read fail\n");
        return false;
    }

    print("mpuInit: 0x"); printHex(mpuBuffer[1]); print("\n");
    return true;
}

// HAL for invensense:

int i2c_write(uint8_t devAddr,
              uint8_t regAddr,
              uint8_t dataLength,
              uint8_t const *data)
{
    devAddr <<= 1;                              // TODO class attribute + CHECK IF USEFUL !!
    bool transfer_succeeded;

#define TESTED_BUT_SLOW
#ifdef TESTED_BUT_SLOW                          // TODO: RE-TEST IT !!

    const int bytes_num = 1 + dataLength;
	uint8_t buffer[bytes_num];
	buffer[0] = regAddr;
    for (int i = 0; i < dataLength; i++)        // TODO: hack twi_master_write to avoid this !
        buffer[i+1] = data[i];
    transfer_succeeded = twi_master_transfer(devAddr, buffer, bytes_num, TWI_ISSUE_STOP);

#else                                           // TODO: TEST IT !!

    transfer_succeeded  = twi_master_transfer(devAddr, &regAddr, 1, TWI_DONT_ISSUE_STOP);
    transfer_succeeded &= twi_master_transfer(devAddr, data, dataLength, TWI_ISSUE_STOP);

#endif

    return !transfer_succeeded; // negate to comply with invensense expectation: return 0 => OK
}

int i2c_read(uint8_t devAddr,
             uint8_t regAddr,
             uint8_t dataLength,
             uint8_t *data)
{
    devAddr <<= 1;                              // TODO class attribute + CHECK IF USEFUL !!
    bool transfer_succeeded;
    transfer_succeeded  = twi_master_transfer(devAddr, &regAddr, 1, TWI_DONT_ISSUE_STOP);
    transfer_succeeded &= twi_master_transfer(devAddr, data, dataLength, TWI_ISSUE_STOP);
    return !transfer_succeeded; // negate to comply with invensense expectation: return 0 => OK
}

#endif // MPU_C
