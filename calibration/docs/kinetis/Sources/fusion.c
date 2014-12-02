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
#include "include_all.h"
#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "time.h"
#include "string.h"

void fInit_3DOF_G_BASIC(struct SV_3DOF_G_BASIC *pthisSV, float flpftimesecs, int16 iSensorFS)
{
	// set algorithm sampling interval (typically 25Hz) 
	pthisSV->fdeltat = 1.0F / (float) iSensorFS;

    // set low pass filter constant with maximum value 1.0 (all pass) decreasing to 0.0 (increasing low pass)
    if (flpftimesecs > pthisSV->fdeltat)
          pthisSV->flpf = pthisSV->fdeltat / flpftimesecs;
    else
          pthisSV->flpf = 1.0F;
    
	// clear the reset flag
	pthisSV->resetflag = false;
	
	return;
} // end fInit_3DOF_G_BASIC

void fInit_3DOF_B_BASIC(struct SV_3DOF_B_BASIC *pthisSV, float flpftimesecs, int16 iSensorFS)
{
	// set algorithm sampling interval (typically 25Hz) 
	pthisSV->fdeltat = 1.0F / (float) iSensorFS;

    // set low pass filter constant with maximum value 1.0 (all pass) decreasing to 0.0 (increasing low pass)
    if (flpftimesecs > pthisSV->fdeltat)
          pthisSV->flpf = pthisSV->fdeltat / flpftimesecs;
    else
          pthisSV->flpf = 1.0F;
    
	// clear the reset flag
	pthisSV->resetflag = false;
	
	return;
} // end fInit_3DOF_B_BASIC

void fInit_6DOF_GB_BASIC(struct SV_6DOF_GB_BASIC *pthisSV, float flpftimesecs, int16 iSensorFS)
{
	// set algorithm sampling interval (typically 25Hz) 
	pthisSV->fdeltat = 1.0F / (float) iSensorFS;

    // set low pass filter constant with maximum value 1.0 (all pass) decreasing to 0.0 (increasing low pass)
    if (flpftimesecs > pthisSV->fdeltat)
          pthisSV->flpf = pthisSV->fdeltat / flpftimesecs;
    else
          pthisSV->flpf = 1.0F;
    
	// clear the reset flag
	pthisSV->resetflag = false;
	
	return;
} // end fInit_6DOF_GB_BASIC

// 3DOF orientation function which calls tilt functions and implements low pass filters
void fRun_3DOF_G_BASIC(struct SV_3DOF_G_BASIC *pthisSV, struct AccelSensor *pthisAccel, int32 loopcounter, int16 ithisCoordSystem)
{
	// do a reset and return if requested
	if (pthisSV->resetflag)
	{
		fInit_3DOF_G_BASIC(pthisSV, 0.2F, SENSORFS);
		return;
	}
	
	// apply the tilt estimation algorithm to get the instantaneous orientation matrix
	if (ithisCoordSystem == NED)
	{
		// call NED tilt function
		f3DOFTiltNED(pthisSV->fR, pthisAccel->fGp);
	}
	else if (ithisCoordSystem == ANDROID)
	{
		// call Android tilt function
		f3DOFTiltAndroid(pthisSV->fR, pthisAccel->fGp);
	}
	else
	{
		// call Windows 8 tilt function
		f3DOFTiltWin8(pthisSV->fR, pthisAccel->fGp);
	}

	// compute the instanteneous quaternion from the instantaneous rotation matrix
	fQuaternionFromRotationMatrix(pthisSV->fR, &(pthisSV->fq));

	// low pass filter the orientation quaternion and compute the low pass rotation matrix
	fLPFOrientationQuaternion(&(pthisSV->fq), &(pthisSV->fLPq), pthisSV->flpf, pthisSV->fdeltat, pthisSV->fOmega, loopcounter);
	fRotationMatrixFromQuaternion(pthisSV->fLPR, &(pthisSV->fLPq));
	fRotationVectorDegFromQuaternion(&(pthisSV->fLPq), pthisSV->fLPRVec);

	// calculate the Euler angles from the low pass orientation matrix
	if (ithisCoordSystem == NED)
	{
		// calculate NED Euler angles
		fNEDAnglesDegFromRotationMatrix(pthisSV->fLPR, &(pthisSV->fLPPhi), &(pthisSV->fLPThe), &(pthisSV->fLPPsi),
				&(pthisSV->fLPRho), &(pthisSV->fLPChi));
	}
	else if (ithisCoordSystem == ANDROID)
	{
		// calculate Android Euler angles
		fAndroidAnglesDegFromRotationMatrix(pthisSV->fLPR, &(pthisSV->fLPPhi), &(pthisSV->fLPThe), &(pthisSV->fLPPsi),
				&(pthisSV->fLPRho), &(pthisSV->fLPChi));
	}
	else
	{
		// calculate Windows 8 Euler angles
		fWin8AnglesDegFromRotationMatrix(pthisSV->fLPR, &(pthisSV->fLPPhi), &(pthisSV->fLPThe), &(pthisSV->fLPPsi),
				&(pthisSV->fLPRho), &(pthisSV->fLPChi));
	}

	return;
} // end fRun_3DOF_G_BASIC

// 2D automobile eCompass
void fRun_3DOF_B_BASIC(struct SV_3DOF_B_BASIC *pthisSV, struct MagSensor *pthisMag, int32 loopcounter, int16 ithisCoordSystem)
{
	// do a reset and return if requested
	if (pthisSV->resetflag)
	{
		fInit_3DOF_B_BASIC(pthisSV, 0.2F, SENSORFS);
		return;
	}
	
	// calculate the 3DOF magnetometer orientation matrix from fBc
	if (ithisCoordSystem == NED)
	{
		// call NED magnetic rotation matrix function
		f3DOFMagnetometerMatrixNED(pthisSV->fR, pthisMag->fBc);
	}
	else if (ithisCoordSystem == ANDROID)
	{
		// call Android magnetic rotation matrix function
		f3DOFMagnetometerMatrixAndroid(pthisSV->fR, pthisMag->fBc);
	}
	else
	{
		// call Windows 8 magnetic rotation matrix function
		f3DOFMagnetometerMatrixWin8(pthisSV->fR, pthisMag->fBc);
	}

	// compute the instanteneous quaternion from the instantaneous rotation matrix
	fQuaternionFromRotationMatrix(pthisSV->fR, &(pthisSV->fq));

	// low pass filter the orientation quaternion and compute the low pass rotation matrix
	fLPFOrientationQuaternion(&(pthisSV->fq), &(pthisSV->fLPq), pthisSV->flpf, pthisSV->fdeltat, pthisSV->fOmega, loopcounter);
	fRotationMatrixFromQuaternion(pthisSV->fLPR, &(pthisSV->fLPq));
	fRotationVectorDegFromQuaternion(&(pthisSV->fLPq), pthisSV->fLPRVec);

	// calculate the Euler angles from the low pass orientation matrix
	if (ithisCoordSystem == NED)
	{
		// calculate NED Euler angles
		fNEDAnglesDegFromRotationMatrix(pthisSV->fLPR, &(pthisSV->fLPPhi), &(pthisSV->fLPThe), &(pthisSV->fLPPsi),
				&(pthisSV->fLPRho), &(pthisSV->fLPChi));
	}
	else if (ithisCoordSystem == ANDROID)
	{
		// calculate Android Euler angles
		fAndroidAnglesDegFromRotationMatrix(pthisSV->fLPR, &(pthisSV->fLPPhi), &(pthisSV->fLPThe), &(pthisSV->fLPPsi),
				&(pthisSV->fLPRho), &(pthisSV->fLPChi));
	}
	else
	{
		// calculate Windows 8 Euler angles
		fWin8AnglesDegFromRotationMatrix(pthisSV->fLPR, &(pthisSV->fLPPhi), &(pthisSV->fLPThe), &(pthisSV->fLPPsi),
				&(pthisSV->fLPRho), &(pthisSV->fLPChi));
	}

	return;
}

// 6DOF orientation function which calls ecompass and implements low pass filters
void fRun_6DOF_GB_BASIC(struct SV_6DOF_GB_BASIC *pthisSV, struct MagSensor *pthisMag, struct AccelSensor *pthisAccel, int32 loopcounter, int16 ithisCoordSystem)
{
	// do a reset and return if requested
		if (pthisSV->resetflag)
		{
			fInit_6DOF_GB_BASIC(pthisSV, 0.6F, SENSORFS);
			return;
		}
		
	// call the eCompass algorithm to get the instantaneous orientation matrix and inclination angle
	if (ithisCoordSystem == NED)
	{
		// call the NED eCompass
		feCompassNED(pthisSV->fR, &(pthisSV->fDelta), pthisMag->fBc, pthisAccel->fGp);
	}
	else if  (ithisCoordSystem == ANDROID)
	{
		// call the Android eCompass
		feCompassAndroid(pthisSV->fR, &(pthisSV->fDelta), pthisMag->fBc, pthisAccel->fGp);
	}
	else
	{
		// call the Win8 eCompass
		feCompassWin8(pthisSV->fR, &(pthisSV->fDelta), pthisMag->fBc, pthisAccel->fGp);
	}

	// compute the instanteneous quaternion from the instantaneous rotation matrix
	fQuaternionFromRotationMatrix(pthisSV->fR, &(pthisSV->fq));

	// low pass filter the orientation quaternion and compute the low pass rotation matrix
	fLPFOrientationQuaternion(&(pthisSV->fq), &(pthisSV->fLPq), pthisSV->flpf, pthisSV->fdeltat, pthisSV->fOmega, loopcounter);
	fRotationMatrixFromQuaternion(pthisSV->fLPR, &(pthisSV->fLPq));
	fRotationVectorDegFromQuaternion(&(pthisSV->fLPq), pthisSV->fLPRVec);

	// compute the low pass filtered Euler angles
	if (ithisCoordSystem == NED)
	{
		// calculate the NED Euler angles
		fNEDAnglesDegFromRotationMatrix(pthisSV->fLPR, &(pthisSV->fLPPhi), &(pthisSV->fLPThe), &(pthisSV->fLPPsi),
				&(pthisSV->fLPRho), &(pthisSV->fLPChi));
	}
	else if (ithisCoordSystem == ANDROID)
	{
		// calculate the Android Euler angles
		fAndroidAnglesDegFromRotationMatrix(pthisSV->fLPR, &(pthisSV->fLPPhi), &(pthisSV->fLPThe), &(pthisSV->fLPPsi),
				&(pthisSV->fLPRho), &(pthisSV->fLPChi));
	}
	else
	{
		// calculate the Windows 8 Euler angles
		fWin8AnglesDegFromRotationMatrix(pthisSV->fLPR, &(pthisSV->fLPPhi), &(pthisSV->fLPThe), &(pthisSV->fLPPsi),
				&(pthisSV->fLPRho), &(pthisSV->fLPChi));
	}

	// low pass filter the geomagnetic inclination angle with a simple exponential filter
	fLPFScalar(&(pthisSV->fDelta), &(pthisSV->fLPDelta), pthisSV->flpf, loopcounter);

	return;
}  // end fRun_6DOF_GB_BASIC



