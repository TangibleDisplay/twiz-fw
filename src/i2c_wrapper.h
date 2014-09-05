#ifndef I2C_WRAPPER_H
#define I2C_WRAPPER_H

#include <stdint.h>

int i2c_init(void);
int i2c_write(uint8_t devAddr, uint8_t regAddr, uint8_t dataLength, uint8_t const *data);
int i2c_read(uint8_t  devAddr, uint8_t regAddr, uint8_t dataLength, uint8_t *data);

#endif // I2C_WRAPPER_H
