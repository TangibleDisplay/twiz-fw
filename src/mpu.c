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

#endif // MPU_C
