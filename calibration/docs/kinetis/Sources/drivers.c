// Copyright (c) 2014, Freescale Semiconductor, Inc.
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Freescale Semiconductor, Inc. nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL FREESCALE SEMICONDUCTOR, INC. BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "Events.h"
#include "I2CFreedom.h"
#include "UART.h"
#include "FTM.h"
#include "include_all.h"
#include "string.h"

// I2C communications globals
#define I2C_BUF_LEN           16
uint8_t I2C_Buf[I2C_BUF_LEN];

// UART (for bluetooth) globals
uint8_t sUARTOutputBuf[UART_OUTPUT_BUFFER_SIZE]; 
uint8_t sUARTInputBuf[UART_INPUT_BUFFER_SIZE];

// Freedom FRDM-KL46Z board sensor I2C addresses
#define MMA8451_FRDM_I2C_ADDR		0x1D
#define MAG3110_FRDM_I2C_ADDR		0x0E

// MAG3110 registers and constants
#define MAG3110_STATUS					0x00
#define MAG3110_OUT_X_MSB       	  	0x01
#define MAG3110_WHO_AM_I      			0x07
#define MAG3110_CTRL_REG1        	 	0x10
#define MAG3110_CTRL_REG2         		0x11
#define MAG3110_WHO_AM_I_VALUE     		0xC4

// MMA8451 registers and constants
#define MMA8451_STATUS					0x00 
#define MMA8451_OUT_X_MSB       	  	0x01 
#define MMA8451_WHO_AM_I      			0x0D 
#define MMA8451_XYZ_DATA_CFG     	  	0x0E 
#define MMA8451_CTRL_REG1        	 	0x2A 
#define MMA8451_CTRL_REG2         		0x2B 
#define MMA8451_WHO_AM_I_VALUE     		0x1A 

// initialize MAG3110 magnetometer sensor on Freedom board for 40Hz
int8 MAG3110_Freedom_Init_40Hz(LDD_TDeviceData *DeviceDataPtr, struct MagSensor *pthisMag)
{
	LDD_I2C_TBusState BusState;		// I2C bus state
	LDD_I2C_TErrorMask MAG3110_I2C_Error;

	// set up the MAG3110 I2C address
	I2CFreedom_SelectSlaveDevice(DeviceDataPtr, LDD_I2C_ADDRTYPE_7BITS, MAG3110_FRDM_I2C_ADDR);

	// write 0000 0000 = 0x00 to CTRL_REG1 to place MMAG3110 into standby
	// [7-1] = 0000 000
	// [0]: AC=0 for standby
	I2C_Buf[0] = MAG3110_CTRL_REG1;
	I2C_Buf[1] = 0x00;
	globals.I2C_Status &= ~I2C_SENT_FLAG;

	// transmit the bytes
	I2CFreedom_MasterSendBlock(DeviceDataPtr, I2C_Buf, 2, LDD_I2C_SEND_STOP);

	// loop while no error detected and the I2C sent callback has not yet set the sent flag
	do
	{
		// read the error flag
		// ERR_OK = 0x00: device is present
		// ERR_DISABLED = 0x07: device is disabled
		// ERR_SPEED = 0x01: device does not work in the active speed mode
		I2CFreedom_GetError(DeviceDataPtr, &MAG3110_I2C_Error);
	}
	while ((!MAG3110_I2C_Error) && !(globals.I2C_Status & I2C_SENT_FLAG));

	// return immediately with error condition if MPL3115 is not present
	if (MAG3110_I2C_Error)
		return false;

	// wait until the I2C bus is idle
	do
	{
		I2CFreedom_CheckBus(DeviceDataPtr, &BusState);
	} while (BusState != LDD_I2C_IDLE);

	// write 1001 0000 = 0x90 to CTRL_REG2
	// [7]: AUTO_MRST_EN = 1: enable degaussing
	// [6]: unused=0
	// [5]: RAW=0: normal mode
	// [4]: Mag_RST=1: enable a single degauss
	// [3-0]: unused=0
	I2C_Buf[0] = MAG3110_CTRL_REG2;
	I2C_Buf[1] = 0x90;
	globals.I2C_Status &= ~I2C_SENT_FLAG;
	I2CFreedom_MasterSendBlock(DeviceDataPtr, I2C_Buf, 2, LDD_I2C_SEND_STOP);
	// wait until the I2C sent callback function sets the sent flag
	while ((globals.I2C_Status & I2C_SENT_FLAG) == 0);
	// wait until the I2C bus is idle
	do
	{
		I2CFreedom_CheckBus(DeviceDataPtr, &BusState);
	} while (BusState != LDD_I2C_IDLE);

	// write 0000 1001 = 0x09 to CTRL_REG1 to configure and take out of standby
	// [7-5]: DR=000 for 40Hz ODR with OS=01
	// [4-3]: OS=01 for 32x oversampling and 40Hz ODR
	// [2]: FT=0 for normal reads
	// [1]: TM=0 to not trigger immediate measurement
	// [0]: AC=1 for active mode
	I2C_Buf[0] = MAG3110_CTRL_REG1;
	I2C_Buf[1] = 0x09;
	globals.I2C_Status &= ~I2C_SENT_FLAG;
	I2CFreedom_MasterSendBlock(DeviceDataPtr, I2C_Buf, 2, LDD_I2C_SEND_STOP);
	// wait until the I2C sent callback function sets the sent flag
	while ((globals.I2C_Status & I2C_SENT_FLAG) == 0);
	// wait until the I2C bus is idle
	do
	{
		I2CFreedom_CheckBus(DeviceDataPtr, &BusState);
	} while (BusState != LDD_I2C_IDLE);

	return true;
}

// initialize MMA8451 accelerometer sensor on Freedom board for 50Hz
int8 MMA8451_Freedom_Init_50Hz(LDD_TDeviceData *DeviceDataPtr, struct AccelSensor *pthisAccel)
{
	LDD_I2C_TBusState BusState;		// I2C bus state
	LDD_I2C_TErrorMask MMA8451_I2C_Error;

	// set up the MMA8451 I2C address
	I2CFreedom_SelectSlaveDevice(DeviceDataPtr, LDD_I2C_ADDRTYPE_7BITS, MMA8451_FRDM_I2C_ADDR);

	// write 0000 0000 = 0x00 to CTRL_REG1 to place MMA8451 into standby
	// [7-1] = 0000 000
	// [0]: active=0
	I2C_Buf[0] = MMA8451_CTRL_REG1;
	I2C_Buf[1] = 0x00;
	globals.I2C_Status &= ~I2C_SENT_FLAG;

	// transmit the bytes
	I2CFreedom_MasterSendBlock(DeviceDataPtr, I2C_Buf, 2, LDD_I2C_SEND_STOP);

	// loop while no error detected and the I2C sent callback has not yet set the sent flag
	do
	{
		// read the error flag
		// ERR_OK = 0x00: device is present
		// ERR_DISABLED = 0x07: device is disabled
		// ERR_SPEED = 0x01: device does not work in the active speed mode
		I2CFreedom_GetError(DeviceDataPtr, &MMA8451_I2C_Error);
	}
	while ((!MMA8451_I2C_Error) && !(globals.I2C_Status & I2C_SENT_FLAG));

	// return immediately with error condition if MPL3115 is not present
	if (MMA8451_I2C_Error)
		return false;

	// wait until the I2C bus is idle
	do
	{
		I2CFreedom_CheckBus(DeviceDataPtr, &BusState);
	} while (BusState != LDD_I2C_IDLE);

	// write 0000 0001 = 0x01 to XYZ_DATA_CFG register to set g range
	// [7-5]: reserved=000
	// [4]: HPF_OUT=0
	// [3-2]: reserved=00
	// [1-0]: FS=01 for +/-4g: 512 counts / g = 8192 counts / g after 4 bit left shift
	I2C_Buf[0] = MMA8451_XYZ_DATA_CFG;
	I2C_Buf[1] = 0x01;
	globals.I2C_Status &= ~I2C_SENT_FLAG;
	I2CFreedom_MasterSendBlock(DeviceDataPtr, I2C_Buf, 2, LDD_I2C_SEND_STOP);
	// wait until the I2C sent callback function sets the sent flag
	while ((globals.I2C_Status & I2C_SENT_FLAG) == 0);
	// wait until the I2C bus is idle
	do
	{
		I2CFreedom_CheckBus(DeviceDataPtr, &BusState);
	} while (BusState != LDD_I2C_IDLE);

	// write 0010 0001 = 0x21 to CTRL_REG1
	// [7-6]: aslp_rate=00
	// [5-3]: dr=100 for 50Hz data rate
	// [2]: unused=0
	// [1]: f_read=0 for normal 16 bit reads
	// [0]: active=1 to take the part out of standby and enable sampling
	I2C_Buf[0] = MMA8451_CTRL_REG1;
	I2C_Buf[1] = 0x21;
	globals.I2C_Status &= ~I2C_SENT_FLAG;
	I2CFreedom_MasterSendBlock(DeviceDataPtr, I2C_Buf, 2, LDD_I2C_SEND_STOP);
	// wait until the I2C sent callback function sets the sent flag
	while ((globals.I2C_Status & I2C_SENT_FLAG) == 0);
	// wait until the I2C bus is idle
	do
	{
		I2CFreedom_CheckBus(DeviceDataPtr, &BusState);
	} while (BusState != LDD_I2C_IDLE);

	return true;
}

// read MAG3110 magnetometer on shield board data over I2C
void MAG3110_Freedom_ReadData(LDD_TDeviceData *DeviceDataPtr, struct MagSensor *pthisMag)
{
	LDD_I2C_TBusState BusState;		// I2C bus state

	// set up the MAG3110 I2C address
	I2CFreedom_SelectSlaveDevice(DeviceDataPtr, LDD_I2C_ADDRTYPE_7BITS, MAG3110_FRDM_I2C_ADDR);
	// set up the address of the first output register
	I2C_Buf[0] = MAG3110_OUT_X_MSB;
	globals.I2C_Status &= ~I2C_SENT_FLAG;
	I2CFreedom_MasterSendBlock(DeviceDataPtr, I2C_Buf, 1, LDD_I2C_NO_SEND_STOP);
	// wait until the I2C sent callback function sets the sent flag
	while ((globals.I2C_Status & I2C_SENT_FLAG) == 0);

	// read the 6 bytes of sequential sensor data
	globals.I2C_Status &= ~I2C_RCVD_FLAG;
	I2CFreedom_MasterReceiveBlock(DeviceDataPtr, I2C_Buf, 6, LDD_I2C_SEND_STOP);
	// wait until the I2C received callback function sets the received flag
	while ((globals.I2C_Status & I2C_RCVD_FLAG) == 0);
	// wait until the I2C bus is idle
	do
	{
		I2CFreedom_CheckBus(DeviceDataPtr, &BusState);
	} while (BusState != LDD_I2C_IDLE);

	// place the 12 bytes read into the 16 bit magnetometer structure
	pthisMag->iBp[X] = (I2C_Buf[0] << 8) | I2C_Buf[1];
	pthisMag->iBp[Y] = (I2C_Buf[2] << 8) | I2C_Buf[3];
	pthisMag->iBp[Z] = (I2C_Buf[4] << 8) | I2C_Buf[5];

	// check for -32768 in the magnetometer data since
	// this value cannot be negated in a later HAL operation
	if (pthisMag->iBp[X] == -32768) pthisMag->iBp[X]++;
	if (pthisMag->iBp[Y] == -32768) pthisMag->iBp[Y]++;
	if (pthisMag->iBp[Z] == -32768) pthisMag->iBp[Z]++;
	
	// store the gain terms in the magnetometer sensor structure
#define MAG3110_UTPERCOUNT 0.1F      				// fixed range for MAG3110 magnetometer
#define MAG3110_COUNTSPERUT 10.0F					// must be reciprocal of FUTPERCOUNT
	pthisMag->fuTPerCount = MAG3110_UTPERCOUNT;
	pthisMag->fCountsPeruT = MAG3110_COUNTSPERUT;

	return;
}

// read MMA8451 accelerometer data on Freedom board data over I2C
void MMA8451_Freedom_ReadData(LDD_TDeviceData *DeviceDataPtr, struct AccelSensor *pthisAccel)
{
	LDD_I2C_TBusState BusState;		// I2C bus state

	// set up the MMA8451 I2C address
	I2CFreedom_SelectSlaveDevice(DeviceDataPtr, LDD_I2C_ADDRTYPE_7BITS, MMA8451_FRDM_I2C_ADDR);
	// set up the address of the first output register
	I2C_Buf[0] = MMA8451_OUT_X_MSB;
	globals.I2C_Status &= ~I2C_SENT_FLAG;
	I2CFreedom_MasterSendBlock(DeviceDataPtr, I2C_Buf, 1, LDD_I2C_NO_SEND_STOP);
	// wait until the I2C sent callback function sets the sent flag
	while ((globals.I2C_Status & I2C_SENT_FLAG) == 0);

	// read the 6 bytes of sequential sensor data
	globals.I2C_Status &= ~I2C_RCVD_FLAG;
	I2CFreedom_MasterReceiveBlock(DeviceDataPtr, I2C_Buf, 6, LDD_I2C_SEND_STOP);
	// wait until the I2C received callback function sets the received flag
	while ((globals.I2C_Status & I2C_RCVD_FLAG) == 0);
	// wait until the I2C bus is idle
	do
	{
		I2CFreedom_CheckBus(DeviceDataPtr, &BusState);
	} while (BusState != LDD_I2C_IDLE);

	// place the 12 bytes read into the 16 bit accelerometer structure
	pthisAccel->iGp[X] = (I2C_Buf[0] << 8) | I2C_Buf[1];
	pthisAccel->iGp[Y] = (I2C_Buf[2] << 8) | I2C_Buf[3];
	pthisAccel->iGp[Z] = (I2C_Buf[4] << 8) | I2C_Buf[5];

	// check for -32768 in the accelerometer since
	// this value cannot be negated in a later HAL operation
	if (pthisAccel->iGp[X] == -32768) pthisAccel->iGp[X]++;
	if (pthisAccel->iGp[Y] == -32768) pthisAccel->iGp[Y]++;
	if (pthisAccel->iGp[Z] == -32768) pthisAccel->iGp[Z]++;

	// store the gain terms in the accelerometer sensor structure
#define MMA8451_GPERCOUNT 0.0001220703125F			// equal to 1/8192
	pthisAccel->fgPerCount = MMA8451_GPERCOUNT;
	
	return;
}

// function appends a variable number of source bytes to a destimation buffer
// for transmission as the bluetooth packet
// bluetooth packets are delimited by inserting the special byte 0x7E at the start
// and end of packets. this function must therefore handle the case of 0x7E appearing
// as general data. this is done here with the substitutions:
// a) replace 0x7E by 0x7D and 0x5E (one byte becomes two) 
// b) replace 0x7D by 0x7D and 0x5D (one byte becomes two)
// the inverse mapping must be performed at the application receiving the bluetooth stream: ie:
// replace 0x7D and 0x5E with 0x7E
// replace 0x7D and 0x5D with 0x7D
// NOTE: do not use this function to append the start and end bytes 0x7E to the bluetooth
// buffer. instead add the start and end bytes 0x7E explicitly as in:
// sUARTOutputBuf[iByteCount++] = 0x7E;
void sBufAppendItem(uint8* pDest, uint32* pIndex, uint8* pSource, uint16 iBytesToCopy)
{
	uint16 i;			// loop counter

	// loop over number of bytes to add to the destination buffer
	for (i = 0; i < iBytesToCopy; i++)
	{
		// check for special case 1: replace 0x7E (start and end byte) with 0x7D and 0x5E
		if (pSource[i] == 0x7E)
		{
			pDest[(*pIndex)++] = 0x7D;
			pDest[(*pIndex)++] = 0x5E;
		}
		// check for special case 2: replace 0x7D with 0x7D and 0x5D
		else if (pSource[i] == 0x7D)
		{
			pDest[(*pIndex)++] = 0x7D;
			pDest[(*pIndex)++] = 0x5D;
		}
		else
			// general case, simply add this byte without change
		{
			pDest[(*pIndex)++] = pSource[i];
		}
	}

	return;
}

// set bluetooth packets out over UART
void CreateAndSendPacketsViaUART(LDD_TDeviceData *DeviceDataPtr)
{
	struct fquaternion fq;		// quaternion to be transmitted
	uint32 iIndex;				// output buffer counter
	int16 tmpint16;				// scratch int16
	int16 iPhi, iThe, iRho;		// integer angles to be transmitted
	int16 iDelta;				// magnetic inclination angle if available
	int16 iOmega[3];			// scaled angular velocity vector
	uint16 isystick;			// algorithm systick time
	uint8 tmpuint8;				// scratch uint8
	uint8 flags;				// byte of flags
	int16 i, j, k;				// general purpose

	// zero the counter for bytes accumulated into the transmit buffer
	iIndex = 0;

	// ***************************************************************
	// Main type 1: range 0 to 35 = 36 bytes
	// Debug type 2: range 0 to 7 = 8 bytes
	// Angular velocity type 3: range 0 to 13 = 14 bytes
	// Euler angles type 4: range 0 to 13 = 14 bytes
	// Magnetic type 6: range 0 to 15 = 16 bytes
	// Total is 102 bytes vs 128 bytes in UART_OUTPUT_BUFFER_SIZE
	// at 25Hz, data rate is 25*102 = 2550 bytes/sec = 25.5kbaud
	// the UART is set to 115kbaud so about 22% is being used
	// ***************************************************************

	// ************************************************************************
	// fixed length packet type 1:
	// this packet type is always transmitted
	// total size is 0 to 35 equals 36 bytes 
	// ************************************************************************

	// [0]: packet start byte (need a iIndex++ here since not using sBufAppendItem)
	sUARTOutputBuf[iIndex++] = 0x7E;

	// [1]: packet type 1 byte (iIndex is automatically updated in sBufAppendItem)
	tmpuint8 = 0x01;
	sBufAppendItem(sUARTOutputBuf, &iIndex, &tmpuint8, 1);

	// [2]: packet number byte
	sBufAppendItem(sUARTOutputBuf, &iIndex, &(globals.iPacketNumber), 1);
	globals.iPacketNumber++;

	// [6-3]: 1MHz time stamp (4 bytes)
	sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&(globals.FTMTimestamp), 4);

	// [12-7]: integer uncalibrated accelerometer data words
	sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&thisAccel.iGp[X], 2);
	sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&thisAccel.iGp[Y], 2);
	sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&thisAccel.iGp[Z], 2);

	// [18-13]: integer calibrated magnetometer data words
	sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&thisMag.iBc[X], 2);
	sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&thisMag.iBc[Y], 2);
	sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&thisMag.iBc[Z], 2);

	// [24-19]: transmit zero data for gyro
	tmpint16 = 0;
	sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);
	sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);
	sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);	

	// initialize default quaternion, flags byte, angular velocity and orientation
	fq.q0 = 1.0F;
	fq.q1 = fq.q2 = fq.q3 = 0.0F;
	flags = 0x00;
	iOmega[X] = iOmega[Y] = iOmega[Z] = 0;
	iPhi = iThe = iRho = iDelta = 0;
	isystick = 0;

	// flags byte 33: quaternion type in least significant nibble
	// Q3:   coordinate nibble, 1
	// Q3M:	 coordinate nibble, 6
	// Q3G:	 coordinate nibble, 3
	// Q6MA: coordinate nibble, 2
	// Q6KA: coordinate nibble, 5
	// Q6AG: coordinate nibble, 4
	// Q9:   coordinate nibble, 8

	// flags byte 33: coordinate in most significant nibble
	// NED:       0, quaternion nibble    
	// Android:	  1, quaternion nibble
	// Windows 8: 2, quaternion nibble

	// set the quaternion, flags, angular velocity and Euler angles
	switch (globals.QuaternionPacketType)
	{
	case Q3:
		fq = thisSV_3DOF_G_BASIC.fLPq;
		flags |= 0x01;
		iOmega[X] = (int16)(thisSV_3DOF_G_BASIC.fOmega[X] * 20.0F);
		iOmega[Y] = (int16)(thisSV_3DOF_G_BASIC.fOmega[Y] * 20.0F);
		iOmega[Z] = (int16)(thisSV_3DOF_G_BASIC.fOmega[Z] * 20.0F);
		iPhi = (int16) (10.0F * thisSV_3DOF_G_BASIC.fLPPhi);
		iThe = (int16) (10.0F * thisSV_3DOF_G_BASIC.fLPThe);
		iRho = (int16) (10.0F * thisSV_3DOF_G_BASIC.fLPRho);
		iDelta = 0;
		isystick = (uint16)(thisSV_3DOF_G_BASIC.systick / 20);
		break;
	case Q3M:
		fq = thisSV_3DOF_B_BASIC.fLPq;
		//	flags |= 0x02;
		flags |= 0x06;
		iOmega[X] = (int16)(thisSV_3DOF_B_BASIC.fOmega[X] * 20.0F);
		iOmega[Y] = (int16)(thisSV_3DOF_B_BASIC.fOmega[Y] * 20.0F);
		iOmega[Z] = (int16)(thisSV_3DOF_B_BASIC.fOmega[Z] * 20.0F);
		iPhi = (int16) (10.0F * thisSV_3DOF_B_BASIC.fLPPhi);
		iThe = (int16) (10.0F * thisSV_3DOF_B_BASIC.fLPThe);
		iRho = (int16) (10.0F * thisSV_3DOF_B_BASIC.fLPRho);
		iDelta = 0;
		isystick = (uint16)(thisSV_3DOF_B_BASIC.systick / 20);
		break;
	case Q6MA:
		fq = thisSV_6DOF_GB_BASIC.fLPq;
		flags |= 0x02;
		iOmega[X] = (int16)(thisSV_6DOF_GB_BASIC.fOmega[X] * 20.0F);
		iOmega[Y] = (int16)(thisSV_6DOF_GB_BASIC.fOmega[Y] * 20.0F);
		iOmega[Z] = (int16)(thisSV_6DOF_GB_BASIC.fOmega[Z] * 20.0F);
		iPhi = (int16) (10.0F * thisSV_6DOF_GB_BASIC.fLPPhi);
		iThe = (int16) (10.0F * thisSV_6DOF_GB_BASIC.fLPThe);
		iRho = (int16) (10.0F * thisSV_6DOF_GB_BASIC.fLPRho);
		iDelta = (int16) (10.0F * thisSV_6DOF_GB_BASIC.fLPDelta);
		isystick = (uint16)(thisSV_6DOF_GB_BASIC.systick / 20);
		break;
	default:
		// use the default data already initialized
		break;
	}

	// [32-25]: scale the quaternion (30K = 1.0F) and add to the buffer
	tmpint16 = (int16)(fq.q0 * 30000.0F);
	sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);
	tmpint16 = (int16)(fq.q1 * 30000.0F);
	sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);
	tmpint16 = (int16)(fq.q2 * 30000.0F);
	sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);
	tmpint16 = (int16)(fq.q3 * 30000.0F);
	sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);

	// set the coordinate system bits in flags from default NED (00)
#if (THISCOORDSYSTEM == ANDROID)
	// set the Android flag bits
	flags |= 0x10;
#elif  (THISCOORDSYSTEM == WIN8)
	// set the Win8 flag bits
	flags |= 0x20;
#endif // THISCOORDSYSTEM

	// [33]: add the flags byte to the buffer
	sBufAppendItem(sUARTOutputBuf, &iIndex, &flags, 1);

	// [34]: add the board ID byte
#define BOARD_FRDM_KL46Z_STANDALONE	 8
	tmpuint8 = BOARD_FRDM_KL46Z_STANDALONE;
	sBufAppendItem(sUARTOutputBuf, &iIndex, &tmpuint8, 1);

	// [35]: add the tail byte for the standard packet type 1
	sUARTOutputBuf[iIndex++] = 0x7E;

	// ************************************************************************
	// Variable length debug packet type 2
	// currently total size is 0 to 7 equals 8 bytes 
	// ************************************************************************

	if (globals.DebugPacketOn)
	{
		// [0]: packet start byte
		sUARTOutputBuf[iIndex++] = 0x7E;

		// [1]: packet type 2 byte
		tmpuint8 = 0x02;
		sBufAppendItem(sUARTOutputBuf, &iIndex, &tmpuint8, 1);

		// [2]: packet number byte
		sBufAppendItem(sUARTOutputBuf, &iIndex, &(globals.iPacketNumber), 1);
		globals.iPacketNumber++;

		// [4-3] software version number
		tmpint16 = THISBUILD;
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);

		// [6-5] systick count / 20
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&isystick, 2);

		// [7 in practice but can be variable]: add the tail byte for the debug packet type 2
		sUARTOutputBuf[iIndex++] = 0x7E;
	}

	// ************************************************************************
	// Angular Velocity Bluetooth transmit packet type 3
	// total bytes for packet type 2 is range 0 to 13 = 14 bytes
	// ************************************************************************

	if (globals.AngularVelocityPacketOn)
	{
		// [0]: packet start byte
		sUARTOutputBuf[iIndex++] = 0x7E;

		// [1]: packet type 3 byte (angular velocity)
		tmpuint8 = 0x03;
		sBufAppendItem(sUARTOutputBuf, &iIndex, &tmpuint8, 1);

		// [2]: packet number byte
		sBufAppendItem(sUARTOutputBuf, &iIndex, &(globals.iPacketNumber), 1);
		globals.iPacketNumber++;

		// [6-3]: time stamp (4 bytes)
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&(globals.FTMTimestamp), 4);

		// [12-7]: add the scaled angular velocity vector to the output buffer
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&iOmega[X], 2);
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&iOmega[Y], 2);
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&iOmega[Z], 2);

		// [13]: add the tail byte for the angular velocity packet type 3
		sUARTOutputBuf[iIndex++] = 0x7E;
	}

	// ************************************************************************
	// Roll, Pitch, Compass Euler angles packet type 4
	// total bytes for packet type 4 is range 0 to 13 = 14 bytes
	// ************************************************************************

	if 	(globals.RPCPacketOn)
	{
		// [0]: packet start byte
		sUARTOutputBuf[iIndex++] = 0x7E;

		// [1]: packet type 4 byte (Euler angles)
		tmpuint8 = 0x04;
		sBufAppendItem(sUARTOutputBuf, &iIndex, &tmpuint8, 1);

		// [2]: packet number byte
		sBufAppendItem(sUARTOutputBuf, &iIndex, &(globals.iPacketNumber), 1);
		globals.iPacketNumber++;

		// [6-3]: time stamp (4 bytes)
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&(globals.FTMTimestamp), 4);

		// [12-7]: add the angles (resolution 0.1 deg per count) to the transmit buffer
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&iPhi, 2);
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&iThe, 2);
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&iRho, 2);

		// [13]: add the tail byte for the roll, pitch, compass angle packet type 4
		sUARTOutputBuf[iIndex++] = 0x7E;
	}

	// ************************************************************************
	// magnetic buffer packet type 6
	// currently total size is 0 to 15 equals 16 bytes 
	// ************************************************************************

	// [0]: packet start byte
	sUARTOutputBuf[iIndex++] = 0x7E;

	// [1]: packet type 6 byte
	tmpuint8 = 0x06;
	sBufAppendItem(sUARTOutputBuf, &iIndex, &tmpuint8, 1);

	// [2]: packet number byte
	sBufAppendItem(sUARTOutputBuf, &iIndex, &(globals.iPacketNumber), 1);
	globals.iPacketNumber++;

	// [4-3]: number of active measurements in the magnetic buffer
	sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&(thisMagBuffer.iMagBufferCount), 2);	

	// [6-5]: fit error (%) with resolution 0.1% 
	if (thisMagCal.fFitErrorpc > 3276.7F)
		tmpint16 = 32767;
	else
		tmpint16 = (int16) (thisMagCal.fFitErrorpc * 10.0F);		
	sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);		

	// always calculate magnetic buffer row and column (low overhead and saves warnings)
	k = globals.MagneticPacketID - 10;
	j = k / MAGBUFFSIZEX;
	i = k - j * MAGBUFFSIZEX;

	// [8-7]: int16: ID of magnetic variable to be transmitted
	// ID 0 to 4 inclusive are magnetic calibration coefficients
	// ID 5 to 9 inclusive are for future expansion
	// ID 10 to (MAGBUFFSIZEX=12) * (MAGBUFFSIZEY=24)-1 or 10 to 10+288-1 are magnetic buffer elements
	// where the convention is used that a negative value indicates empty buffer element (index=-1)
	if ((globals.MagneticPacketID >= 10) && (thisMagBuffer.index[i][j] == -1))
	{
		// use negative ID to indicate inactive magnetic buffer element
		tmpint16 = -globals.MagneticPacketID;
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);
	}
	else
	{
		// use positive ID unchanged for variable or active magnetic buffer entry
		tmpint16 = globals.MagneticPacketID;
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);
	}

	// [10-9]: int16: variable 1 to be transmitted this iteration
	// [12-11]: int16: variable 2 to be transmitted this iteration
	// [14-13]: int16: variable 3 to be transmitted this iteration
	switch (globals.MagneticPacketID)
	{
	case 0:	
		// item 1: currently unused
		tmpint16 = 0;			
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);		
		// item 2: geomagnetic field strength with resolution 0.1uT
		tmpint16 = (int16)(thisMagCal.fB * 10.0F);
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);
		// item 3: magnetic inclination angle with resolution 0.1 deg	
		tmpint16 = iDelta;
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);
		break;
	case 1:
		// items 1 to 3: hard iron components range -3276uT to +3276uT encoded with 0.1uT resolution
		tmpint16 = (int16)(thisMagCal.fV[X] * 10.0F);
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);
		tmpint16 = (int16)(thisMagCal.fV[Y] * 10.0F);
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);		
		tmpint16 = (int16)(thisMagCal.fV[Z] * 10.0F);
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);
		break;			
	case 2:
		// items 1 to 3: diagonal soft iron range -32. to +32. encoded with 0.001 resolution
		tmpint16 = (int16)(thisMagCal.finvW[X][X] * 1000.0F);
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);
		tmpint16 = (int16)(thisMagCal.finvW[Y][Y] * 1000.0F);
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);
		tmpint16 = (int16)(thisMagCal.finvW[Z][Z] * 1000.0F);
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);
		break;
	case 3: 
		// items 1 to 3: off-diagonal soft iron range -32. to +32. encoded with 0.001 resolution
		tmpint16 = (int16)(thisMagCal.finvW[X][Y] * 1000.0F);	
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);
		tmpint16 = (int16)(thisMagCal.finvW[X][Z] * 1000.0F);
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);
		tmpint16 = (int16)(thisMagCal.finvW[Y][Z] * 1000.0F);
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);
		break;
	case 4:				
	case 5:		
	case 6:		
	case 7:		
	case 8:		
	case 9:		
		// cases 4 to 9 inclusive are for future expansion so transmit zeroes for now
		tmpint16 = 0;		
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);		
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);	
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&tmpint16, 2);	
		break;
	default:
		// 10 and upwards: this handles the magnetic buffer elements
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&(thisMagBuffer.iBp[X][i][j]), 2);
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&(thisMagBuffer.iBp[Y][i][j]), 2);
		sBufAppendItem(sUARTOutputBuf, &iIndex, (uint8*)&(thisMagBuffer.iBp[Z][i][j]), 2);
		break;
	}

	// wrap the variable ID back to zero if necessary
	globals.MagneticPacketID++;
	if (globals.MagneticPacketID >= (10 + MAGBUFFSIZEX * MAGBUFFSIZEY))
		globals.MagneticPacketID = 0;

	// [15]: add the tail byte for the magnetic packet type 6
	sUARTOutputBuf[iIndex++] = 0x7E;

	// ************************************************************************
	// all packets have now been constructed in the output buffer so
	// transmit over UART to the Bluetooth module
	// the final iIndex++ gives the number of bytes to transmit
	// which is one more than the last index in the buffer.
	// this function is non-blocking
	// ************************************************************************

	UART_SendBlock(DeviceDataPtr, sUARTOutputBuf, iIndex);

	return;
}

