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

#ifndef TASKS_H
#define TASKS_H

#include "build.h"

// 3DOF basic accelerometer state vector structure
struct SV_3DOF_G_BASIC
{
	// start: elements common to all motion state vectors
	// Euler angles
	float fLPPhi;					// low pass roll (deg)
	float fLPThe;					// low pass pitch (deg)
	float fLPPsi;					// low pass yaw (deg)
	float fLPRho;					// low pass compass (deg)
	float fLPChi;					// low pass tilt from vertical (deg)
	// orientation matrix, quaternion and rotation vector
	float fLPR[3][3];				// low pass filtered orientation matrix
	struct fquaternion fLPq;		// low pass filtered orientation quaternion
	float fLPRVec[3];				// rotation vector
	// angular velocity
	float fOmega[3];				// angular velocity (deg/s)
	// systick timer for benchmarking
	int32 systick;					// systick timer
	// end: elements common to all motion state vectors
	float fR[3][3];					// unfiltered orientation matrix
	struct fquaternion fq;			// unfiltered orientation quaternion
	float fdeltat;					// time interval (s)
	float flpf;						// low pass filter coefficient
	int8 resetflag;					// flag to request re-initialization on next pass
};

// 3DOF basic magnetometer state vector structure
struct SV_3DOF_B_BASIC
{
	// start: elements common to all motion state vectors
	// Euler angles
	float fLPPhi;						// low pass roll (deg)
	float fLPThe;						// low pass pitch (deg)
	float fLPPsi;						// low pass yaw (deg)
	float fLPRho;						// low pass compass (deg)
	float fLPChi;						// low pass tilt from vertical (deg)
	// orientation matrix, quaternion and rotation vector
	float fLPR[3][3];					// low pass filtered orientation matrix
	struct fquaternion fLPq;			// low pass filtered orientation quaternion
	float fLPRVec[3];					// rotation vector
	// angular velocity
	float fOmega[3];					// angular velocity (deg/s)
	// systick timer for benchmarking
	int32 systick;						// systick timer
	// end: elements common to all motion state vectors
	float fR[3][3];						// unfiltered orientation matrix
	struct fquaternion fq;				// unfiltered orientation quaternion
	float fdeltat;						// time interval (s)
	float flpf;							// low pass filter coefficient
	int8 resetflag;					// flag to request re-initialization on next pass
};

// 6DOF basic accelerometer and magnetometer state vector structure
struct SV_6DOF_GB_BASIC
{
	// start: elements common to all motion state vectors
	// Euler angles
	float fLPPhi;					// low pass roll (deg)
	float fLPThe;					// low pass pitch (deg)
	float fLPPsi;					// low pass yaw (deg)
	float fLPRho;					// low pass compass (deg)
	float fLPChi;					// low pass tilt from vertical (deg)
	// orientation matrix, quaternion and rotation vector
	float fLPR[3][3];				// low pass filtered orientation matrix
	struct fquaternion fLPq;		// low pass filtered orientation quaternion
	float fLPRVec[3];				// rotation vector
	// angular velocity
	float fOmega[3];				// virtual gyro angular velocity (deg/s)
	// systick timer for benchmarking
	int32 systick;					// systick timer
	// end: elements common to all motion state vectors
	float fR[3][3];					// unfiltered orientation matrix
	struct fquaternion fq;			// unfiltered orientation quaternion
	float fDelta;					// unfiltered inclination angle (deg)
	float fLPDelta;					// low pass filtered inclination angle (deg)
	float fdeltat;					// time interval (s)
	float flpf;						// low pass filter coefficient
	int8 resetflag;					// flag to request re-initialization on next pass
};

// globals defined in tasks_func.c declared here for use elsewhere
extern struct ProjectGlobals globals;		
extern struct AccelSensor thisAccel;   
extern struct MagSensor thisMag;       
extern struct MagCalibration thisMagCal;
extern struct MagneticBuffer thisMagBuffer;
extern struct SV_3DOF_G_BASIC thisSV_3DOF_G_BASIC;
extern struct SV_3DOF_B_BASIC thisSV_3DOF_B_BASIC;
extern struct SV_6DOF_GB_BASIC thisSV_6DOF_GB_BASIC;

// function prototypes for functions in tasks_func.c
void ApplyAccelHAL(struct AccelSensor *pthisAccel);
void ApplyMagHAL(struct MagSensor *pthisMag);
void Fusion_Run(void);
void MagCal_Run(struct MagCalibration *pthisMagCal, struct MagneticBuffer *pthisMagBuffer);

#endif   // #ifndef TASKS_H
