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

#ifndef LCD_H
#define LCD_H

#include "Cpu.h"

#define LCDCHARS   	4  		// number of digits in the LCD

// LCD PIN1 to LCDWF0
#define   CHAR1a    37 		// FRDM-KL46Z pin LCD-05
#define   CHAR1b    17 		// FRDM-KL46Z pin LCD-06
#define   CHAR2a    7 		// FRDM-KL46Z pin LCD-07
#define   CHAR2b    8 		// FRDM-KL46Z pin LCD-08
#define   CHAR3a    53 		// FRDM-KL46Z pin LCD-09
#define   CHAR3b    38 		// FRDM-KL46Z pin LCD-10
#define   CHAR4a    10 		// FRDM-KL46Z pin LCD-11
#define   CHAR4b    11 		// FRDM-KL46Z pin LCD-12
#define   CHARCOM0    40 	// FRDM-KL46Z pin LCD-01
#define   CHARCOM1    52 	// FRDM-KL46Z pin LCD-02
#define   CHARCOM2    19 	// FRDM-KL46Z pin LCD-03
#define   CHARCOM3    18 	// FRDM-KL46Z pin LCD-04

// but masks used to define which segments are enabled for each character
#define SEGA  0x08
#define SEGB  0x04
#define SEGC  0x02
#define SEGD  0x01
#define SEGE  0x02
#define SEGG  0x04
#define SEGF  0x08

extern const uint8 WF_ORDERING_TABLE[];
extern const char ASCII_TO_WF_CODIFICATION_TABLE[];

// functions in LCD.c
void LCDWriteString (uint8 *ptext);
void LCDWriteChar(uint8 location, uint8 character);

#endif   // #ifndef LCD_H
