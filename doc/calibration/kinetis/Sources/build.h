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

#ifndef BUILD_H
#define BUILD_H

// coordinate system for the build
#define NED 0                       // identifier for NED coordinate system
#define ANDROID 1                   // identifier for Android coordinate system
#define WIN8 2						// identifier for Windows 8 coordinate system
#define THISCOORDSYSTEM ANDROID		// the coordinate system to be used

// int16 build number sent in debug packet
#define THISBUILD  420

// sampling rate and kalman filter timing
#define FTM_INCLK_HZ		125000		// int32: 125kHz FTM timer frequency set in PE: do not change
#define SENSORFS 			25         // int32: 25Hz: frequency (Hz) of sensor sampling process

// vector components
#define X 0
#define Y 1
#define Z 2

// booleans
#define true 1
#define false 0

// geomagnetic model parameters
#define DEFAULTB 50.0F						// default geomagnetic field (uT)

// useful multiplicative conversion constants
#define PI 3.141592654F					// Pi
#define FDEGTORAD 0.01745329251994F		// degrees to radians conversion = pi / 180
#define FRADTODEG 57.2957795130823F		// radians to degrees conversion = 180 / pi
#define FRECIP180 0.0055555555555F		// multiplicative factor 1/180
#define ONETHIRD 0.33333333F			// one third
#define ONESIXTH 0.166666667F			// one sixth
#define ONETWELFTH 0.0833333333F		// one twelfth
#define ONEOVER48 0.02083333333F		// 1 / 48
#define ONEOVER120 0.0083333333F		// 1 / 120
#define ONEOVER3840 0.0002604166667F	// 1 / 3840

// these type definitions re-define (with no changes) those in  PE-Types.h for Kinetis
typedef signed char				int8;
typedef unsigned char			uint8;
typedef signed short int		int16;
typedef unsigned short int		uint16;
typedef signed long int			int32;
typedef unsigned long int		uint32;
// the quaternion type to be transmitted
typedef enum quaternion {Q3, Q3M, Q3G, Q6MA, Q6AG, Q9} quaternion_type;

// quaternion structure definition
struct fquaternion
{
	float q0;	// scalar component
	float q1;	// x vector component
	float q2;	// y vector component
	float q3;	// z vector component
};
	
// accelerometer sensor structure definition
struct AccelSensor
{
	int16 iGp[3];			// 25Hz integer readings (counts)
	float fGp[3];			// 25Hz readings (g)
	float fgPerCount;		// initialized to FGPERCOUNT
};

// magnetometer sensor structure definition
struct MagSensor
{
	int16 iBp[3];			// slow (typically 25Hz) averaged raw readings (counts)
	int16 iBc[3];			// slow (typically 25Hz) averaged calibrated readings (counts)
	float fBp[3];			// slow (typically 25Hz) averaged raw readings (uT)
	float fBc[3];			// slow (typically 25Hz) averaged calibrated readings (uT)
	float fuTPerCount;		// initialized to FUTPERCOUNT
	float fCountsPeruT;		// initialized to FCOUNTSPERUT
};

// project globals structure
struct ProjectGlobals
{
	// flags
	uint8 AngularVelocityPacketOn;
	uint8 DebugPacketOn;
	uint8 RPCPacketOn;
	volatile uint8 I2C_Status;
	volatile uint8 FusionGo;

	// packet quaternion type
	quaternion_type QuaternionPacketType;

	// magnetic packet variable identifier
	int16 MagneticPacketID;

	// packet number
	uint8 iPacketNumber;  	
	
	// global counter incrementing each iteration of sensor fusion (typically 25Hz)
	int32 loopcounter;
	
	// FTM hardware timer: reload value and time stamp
	uint16 FTMReload;
	uint32 FTMTimestamp;
};

#endif // BUILD_H
