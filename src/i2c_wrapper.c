#include "twi_master.h"
#include "i2c_wrapper.h"
#include "boards.h"
#include <string.h>

// HAL for invensense:

int i2c_init(void)
{
    return !twi_master_init(); // invensense expects an error code: 0 = OK, error otherwise
}

int i2c_write_bytes(uint8_t devAddr, uint8_t regAddr, uint8_t dataLength, uint8_t const *data)
{
    devAddr <<= 1;

    const int bytes_num = 1 + dataLength;
	uint8_t buffer[bytes_num];
	buffer[0] = regAddr;
    memcpy(buffer+1, data, dataLength);

    return !twi_master_transfer(devAddr, buffer, bytes_num, TWI_ISSUE_STOP);
}

int i2c_write_byte(uint8_t devAddr, uint8_t regAddr, uint8_t const data)
{
    devAddr <<= 1;

	uint8_t buffer[2];
	buffer[0] = regAddr;
	buffer[1] = data;

    return !twi_master_transfer(devAddr, buffer, 2, TWI_ISSUE_STOP);
}


int i2c_read_bytes(uint8_t devAddr, uint8_t regAddr, uint8_t dataLength, uint8_t *data)
{
    bool transfer_succeeded;
    devAddr <<= 1;

    transfer_succeeded  = twi_master_transfer(devAddr, &regAddr, 1, TWI_DONT_ISSUE_STOP);
    transfer_succeeded &= twi_master_transfer(devAddr|TWI_READ_BIT, data, dataLength, TWI_ISSUE_STOP);

    return !transfer_succeeded;
}

uint8_t i2c_read_byte(uint8_t devAddr, uint8_t regAddr)
{
    uint8_t data;
    devAddr <<= 1;

    twi_master_transfer(devAddr, &regAddr, 1, TWI_DONT_ISSUE_STOP);
    twi_master_transfer(devAddr|TWI_READ_BIT, &data, 1, TWI_ISSUE_STOP);

    return data;
}
