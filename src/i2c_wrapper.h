#ifndef I2C_WRAPPER_H
#define I2C_WRAPPER_H

#include <stdint.h>

int i2c_init(void);
int i2c_write_byte(uint8_t devAddr, uint8_t regAddr, uint8_t const data);
int i2c_write_bytes(uint8_t devAddr, uint8_t regAddr, uint8_t dataLength, uint8_t const *data);
uint8_t i2c_read_byte(uint8_t  devAddr, uint8_t regAddr);
int i2c_read_bytes(uint8_t  devAddr, uint8_t regAddr, uint8_t dataLength, uint8_t *data);

#endif // I2C_WRAPPER_H
