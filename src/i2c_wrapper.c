#include "twi_master.h"
#include "i2c_wrapper.h"
#include "boards.h"

// HAL for invensense:

int i2c_init(void)
{
    return !twi_master_init(); // invensense expects an error code: 0 = OK, error otherwise
}

int i2c_write(uint8_t devAddr, uint8_t regAddr, uint8_t dataLength, uint8_t const *data)
{
    bool transfer_succeeded;
    devAddr <<= 1;

#if 0 // TODO: FIND WHY IT FAILS! (Compass not found.)
    transfer_succeeded  = twi_master_transfer(devAddr, &regAddr, 1, TWI_ISSUE_STOP);
    transfer_succeeded &= twi_master_transfer(devAddr, data, dataLength, TWI_ISSUE_STOP);
#else
    const int bytes_num = 1 + dataLength;
	uint8_t buffer[bytes_num];
	buffer[0] = regAddr;

    for (int i = 0; i < dataLength; i++) // TODO: hack twi_master_write to avoid this !?
        buffer[i+1] = data[i];

    transfer_succeeded = twi_master_transfer(devAddr, buffer, bytes_num, TWI_ISSUE_STOP);
#endif

    return !transfer_succeeded; // invensense expects an error code: 0 = OK, error otherwise
}

int i2c_read(uint8_t devAddr, uint8_t regAddr, uint8_t dataLength, uint8_t *data)
{
    bool transfer_succeeded;
    devAddr <<= 1;

    transfer_succeeded  = twi_master_transfer(devAddr, &regAddr, 1, TWI_DONT_ISSUE_STOP);
    transfer_succeeded &= twi_master_transfer(devAddr|TWI_READ_BIT, data, dataLength, TWI_ISSUE_STOP);

    return !transfer_succeeded; // invensense expects an error code: 0 = OK, error otherwise
}
