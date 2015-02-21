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
#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "time.h"
#include "string.h"
#include "include_all.h"

// sensor data structures and state vectors
struct ProjectGlobals globals;					// globals structure
struct AccelSensor thisAccel;					// this accelerometer
struct MagSensor thisMag;						// this magnetometer
struct MagCalibration thisMagCal;				// hard and soft iron magnetic calibration
struct MagneticBuffer thisMagBuffer;			// magnetometer measurement buffer
struct SV_3DOF_G_BASIC thisSV_3DOF_G_BASIC;		// 3DOF accelerometer structure
struct SV_3DOF_B_BASIC thisSV_3DOF_B_BASIC;		// 3DOF magnetometer structure
struct SV_6DOF_GB_BASIC thisSV_6DOF_GB_BASIC;	// 6DOF accelerometer and magnetometer structure

// function runs the sensor fusion algorithms
void Fusion_Run(void)
{
	int8 initiatemagcal;			// flag to initiate a new magnetic calibration

	// read the sensor data and apply the HAL
	MMA8451_Freedom_ReadData(I2CFreedom_DeviceData, &thisAccel);
	ApplyAccelHAL(&thisAccel);
	MAG3110_Freedom_ReadData(I2CFreedom_DeviceData, &thisMag);
	ApplyMagHAL(&thisMag);

	// scale the HAL-aligned accelerometer measurements
	thisAccel.fGp[X] = (float) thisAccel.iGp[X] * thisAccel.fgPerCount;
	thisAccel.fGp[Y] = (float) thisAccel.iGp[Y] * thisAccel.fgPerCount;
	thisAccel.fGp[Z] = (float) thisAccel.iGp[Z] * thisAccel.fgPerCount;
	thisMag.fBp[X] = (float) thisMag.iBp[X] * thisMag.fuTPerCount;
	thisMag.fBp[Y] = (float) thisMag.iBp[Y] * thisMag.fuTPerCount;
	thisMag.fBp[Z] = (float) thisMag.iBp[Z] * thisMag.fuTPerCount;		

	// magnetic DOF: remove hard and soft iron terms from Bp (uT) to get calibrated data Bc (uT)
	fInvertMagCal(&thisMag, &thisMagCal);

	// update the magnetometer measurement buffer integer magnetometer data (typically at 25Hz)
	if (!((globals.loopcounter < 100) && (thisMag.iBp[X] == 0) && (thisMag.iBp[Y] == 0) && (thisMag.iBp[Z] == 0)))
		  iUpdateMagnetometerBuffer(&thisMagBuffer, &thisAccel, &thisMag, globals.loopcounter);

	// 3DOF Accel Basic: call the tilt algorithm, low pass filters and Euler angle calculation
	thisSV_3DOF_G_BASIC.systick = SYST_CVR & 0x00FFFFFF;
	fRun_3DOF_G_BASIC(&thisSV_3DOF_G_BASIC, &thisAccel, globals.loopcounter, THISCOORDSYSTEM);
	thisSV_3DOF_G_BASIC.systick -= SYST_CVR & 0x00FFFFFF;
	if (thisSV_3DOF_G_BASIC.systick < 0) thisSV_3DOF_G_BASIC.systick += SYST_RVR;

	// 3DOF Magnetometer Basic: call the 2D vehicle compass algorithm
	thisSV_3DOF_B_BASIC.systick = SYST_CVR & 0x00FFFFFF;
	fRun_3DOF_B_BASIC(&thisSV_3DOF_B_BASIC, &thisMag, globals.loopcounter, THISCOORDSYSTEM);
	thisSV_3DOF_B_BASIC.systick -= SYST_CVR & 0x00FFFFFF;
	if (thisSV_3DOF_B_BASIC.systick < 0) thisSV_3DOF_B_BASIC.systick += SYST_RVR;

	// call the eCompass orientation algorithm, low pass filters and Euler angle calculation
	thisSV_6DOF_GB_BASIC.systick = SYST_CVR & 0x00FFFFFF;
	fRun_6DOF_GB_BASIC(&thisSV_6DOF_GB_BASIC, &thisMag, &thisAccel, globals.loopcounter, THISCOORDSYSTEM);
	thisSV_6DOF_GB_BASIC.systick -= SYST_CVR & 0x00FFFFFF;
	if (thisSV_6DOF_GB_BASIC.systick < 0) thisSV_6DOF_GB_BASIC.systick += SYST_RVR;

	// decide whether to perform a magnetic calibration
	// do the first 4 element calibration immediately there are a minimum of MINMEASUREMENTS4CAL
	initiatemagcal = (!thisMagCal.iMagCalHasRun && (thisMagBuffer.iMagBufferCount >= MINMEASUREMENTS4CAL));

	// otherwise initiate a calibration at intervals depending on the number of measurements available
	initiatemagcal |= ((thisMagBuffer.iMagBufferCount >= MINMEASUREMENTS4CAL) && 
			(thisMagBuffer.iMagBufferCount < MINMEASUREMENTS7CAL) &&
			!(globals.loopcounter % INTERVAL4CAL));
	initiatemagcal |= ((thisMagBuffer.iMagBufferCount >= MINMEASUREMENTS7CAL) &&
			!(globals.loopcounter % INTERVAL7CAL));
	initiatemagcal |= ((thisMagBuffer.iMagBufferCount >= MINMEASUREMENTS10CAL) &&
			!(globals.loopcounter % INTERVAL10CAL));

	// initiate the magnetic calibration if any of the conditions are met
	if (initiatemagcal)
	{
		// set the flags denoting that a calibration is in progress
		LED_RED_ClrVal(NULL);   
		MagCal_Run(&thisMagCal, &thisMagBuffer);
		LED_RED_SetVal(NULL); 
		thisMagCal.iMagCalHasRun = 1;
	} 

	// increment the loopcounter (used for time stamping magnetic data)
	globals.loopcounter++;

	return;
}

// function runs the magnetic calibration
void MagCal_Run(struct MagCalibration *pthisMagCal, struct MagneticBuffer *pthisMagBuffer)
{
	int8 i, j;			// loop counters
	int8 isolver;		// magnetic solver used

	// 4 element calibration case
	if (pthisMagBuffer->iMagBufferCount < MINMEASUREMENTS7CAL)
	{
		// age the existing fit error to avoid one good calibration locking out future updates
		if (pthisMagCal->iValidMagCal)
		{
			pthisMagCal->fFitErrorpc *= (1.0F + (float) INTERVAL4CAL / ((float) SENSORFS * FITERRORAGINGSECS));	
		}
		// call the 4 element matrix inversion calibration
		isolver = 4;
		fUpdateCalibration4INV(pthisMagCal, pthisMagBuffer, &thisMag);
	}
	// 7 element calibration case
	else if (pthisMagBuffer->iMagBufferCount < MINMEASUREMENTS10CAL)
	{
		// age the existing fit error to avoid one good calibration locking out future updates
		if (pthisMagCal->iValidMagCal)
		{
			pthisMagCal->fFitErrorpc *= (1.0F + (float) INTERVAL7CAL / ((float) SENSORFS * FITERRORAGINGSECS));	
		}
		// call the 7 element eigenpair calibration
		isolver = 7;
		fUpdateCalibration7EIG(pthisMagCal, pthisMagBuffer, &thisMag);
	}
	// 10 element calibration case
	else
	{
		// age the existing fit error to avoid one good calibration locking out future updates
		if (pthisMagCal->iValidMagCal)
		{
			pthisMagCal->fFitErrorpc *= (1.0F + (float) INTERVAL10CAL / ((float) SENSORFS * FITERRORAGINGSECS));	
		}
		// call the 10 element eigenpair calibration
		isolver = 10;
		fUpdateCalibration10EIG(pthisMagCal, pthisMagBuffer, &thisMag);
	}

	// the trial geomagnetic field must be in range (earth is 22uT to 67uT)
	if ((pthisMagCal->ftrB >= MINBFITUT) && (pthisMagCal->ftrB <= MAXBFITUT))		
	{
		// always accept the calibration if i) no previous calibration exists ii) the calibration fit is reduced or
		// an improved solver was used giving a good trial calibration (4% or under)
		if ((pthisMagCal->iValidMagCal == 0) ||
				(pthisMagCal->ftrFitErrorpc <= pthisMagCal->fFitErrorpc) ||
				((isolver > pthisMagCal->iValidMagCal) && (pthisMagCal->ftrFitErrorpc <= 4.0F)))
		{
			// accept the new calibration solution
			pthisMagCal->iValidMagCal = isolver;
			pthisMagCal->fFitErrorpc = pthisMagCal->ftrFitErrorpc;
			pthisMagCal->fB = pthisMagCal->ftrB;
			pthisMagCal->fFourBsq = 4.0F * pthisMagCal->ftrB * pthisMagCal->ftrB;
			for (i = X; i <= Z; i++)
			{
				pthisMagCal->fV[i] = pthisMagCal->ftrV[i];
				for (j = X; j <= Z; j++)
				{
					pthisMagCal->finvW[i][j] = pthisMagCal->ftrinvW[i][j];
				}
			}
		} // end of test to accept the new calibration 
	} // end of test for geomagenetic field strength in range

	return;
}

// function applies the hardware abstraction layer to the Fast (typically 200Hz) accelerometer readings
void ApplyAccelHAL(struct AccelSensor *pthisAccel)
{
	// sensor shield boards
#if THISCOORDSYSTEM == NED
	int16 itmp16;
	itmp16 = thisAccel.iGp[X];
	thisAccel.iGp[X] = thisAccel.iGp[Y];
	thisAccel.iGp[Y] = itmp16;
#endif // NED
#if THISCOORDSYSTEM == ANDROID
	thisAccel.iGp[X] = -thisAccel.iGp[X];
	thisAccel.iGp[Y] = -thisAccel.iGp[Y];
#endif // Android
#if (THISCOORDSYSTEM == WIN8)
	thisAccel.iGp[Z] = -thisAccel.iGp[Z];
#endif // Win8

	return;
}

// function applies the hardware abstraction layer to the Fast (typically 200Hz) magnetometer readings
void ApplyMagHAL(struct MagSensor *pthisMag)
{
	// sensor shield boards
#if THISCOORDSYSTEM == NED
	thisMag.iBp[X] = -thisMag.iBp[X];
#endif // NED
#if THISCOORDSYSTEM == ANDROID
	int16 itmp16;
	itmp16 = thisMag.iBp[X];
	thisMag.iBp[X] = thisMag.iBp[Y];
	thisMag.iBp[Y] = -itmp16;
	thisMag.iBp[Z] = -thisMag.iBp[Z];	
#endif // Android
#if THISCOORDSYSTEM == WIN8
	int16 itmp16;
	itmp16 = thisMag.iBp[X];
	thisMag.iBp[X] = thisMag.iBp[Y];
	thisMag.iBp[Y] = -itmp16;
	thisMag.iBp[Z] = -thisMag.iBp[Z];
#endif // Win8

	return;
}

