/*********************************************************************
File    : i2c.h
Purpose : 
**********************************************************************/
#ifndef __I2C_H__
#define __I2C_H__
/****************************** Includes *****************************/
/****************************** Defines *******************************/
#define SENSORS_I2C               I2C2

#define I2C_SPEED                 400000
#define I2C_OWN_ADDRESS           0x00

#define I2C_Config() I2cMaster_Init();
                  
/***************************** Prototypes *****************************/
void I2cMaster_Init(void);
unsigned long  Sensors_I2C_WriteRegister(unsigned char Address, unsigned char RegisterAddr, unsigned char RegisterLen, unsigned char *RegisterValue);
unsigned long  Sensors_I2C_ReadRegister(unsigned char Address, unsigned char RegisterAddr, unsigned char RegisterLen, unsigned char *RegisterValue);
int Sensors_I2C_WriteRegister_swap(unsigned char slave_addr,unsigned char reg_addr,unsigned char len, unsigned char *data_ptr);
int Sensors_I2C_ReadRegister_swap(unsigned char slave_addr,unsigned char reg_addr,unsigned char len, unsigned char *data_ptr);


#endif // __I2C_H__


