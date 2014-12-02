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

#include "Cpu.h"
#include "LCD.h"  

// 12 element waveform ordering table
const uint8 WF_ORDERING_TABLE[ ] =
{
		CHAR1a,   	// FRDM-KL46Z pin LCD-05
		CHAR1b,  	// FRDM-KL46Z pin LCD-06
		CHAR2a,   	// FRDM-KL46Z pin LCD-07 
		CHAR2b,   	// FRDM-KL46Z pin LCD-08 
		CHAR3a,   	// FRDM-KL46Z pin LCD-09  
		CHAR3b,  	// FRDM-KL46Z pin LCD-10  
		CHAR4a,   	// FRDM-KL46Z pin LCD-11  
		CHAR4b,   	// FRDM-KL46Z pin LCD-12 
		CHARCOM0,   // FRDM-KL46Z pin LCD-01  
		CHARCOM1,   // FRDM-KL46Z pin LCD-02  
		CHARCOM2,   // FRDM-KL46Z pin LCD-03 
		CHARCOM3,   // FRDM-KL46Z pin LCD-04 
};

// ASCII to segment LCD encoding table defining which segments are on and off for all 49 supported characters
// two bytes per character with byte 1 defining segments D, E, F and G and byte 2 segments A, B, C
const char ASCII_TO_WF_CODIFICATION_TABLE[] =
{
		(SEGD + SEGE + SEGF + !SEGG) , (SEGC + SEGB + SEGA),		// char 0, offset=0
		(!SEGD + !SEGE + !SEGF + !SEGG) , (SEGC + SEGB + !SEGA),	// char 1, offset=4
		(SEGD + SEGE + !SEGF + SEGG) , (!SEGC + SEGB + SEGA),		// char 2, offset=8
		(SEGD + !SEGE + !SEGF + SEGG) , (SEGC + SEGB + SEGA),		// char 3, offset=12
		(!SEGD + !SEGE + SEGF + SEGG) , (SEGC + SEGB + !SEGA),		// char 4, offset=16
		(SEGD + !SEGE + SEGF + SEGG) , (SEGC + !SEGB + SEGA),		// char 5, offset=20
		(SEGD + SEGE + SEGF + SEGG) , (SEGC + !SEGB + SEGA),		// char 6, offset=24
		(!SEGD + !SEGE + !SEGF + !SEGG) , (SEGC + SEGB + SEGA),		// char 7, offset=28
		(SEGD + SEGE + SEGF + SEGG) , (SEGC + SEGB + SEGA),			// char 8, offset=32
		(SEGD + !SEGE + SEGF + SEGG) , (SEGC + SEGB + SEGA),		// char 9, offset=36
		(!SEGD + !SEGE + !SEGF + !SEGG) , (!SEGC + !SEGB + !SEGA),	// char :, offset=40
		(!SEGD + !SEGE + !SEGF + !SEGG) , (!SEGC + !SEGB + !SEGA),	// char ;, offset=44
		(!SEGD + !SEGE + !SEGF + !SEGG) , (!SEGC + !SEGB + !SEGA),	// char <, offset=48
		(SEGD + !SEGE + !SEGF + SEGG) , (!SEGC + !SEGB + !SEGA),	// char =, offset=52
		(!SEGD + !SEGE + !SEGF + !SEGG) , (!SEGC + !SEGB + !SEGA),	// char >, offset=56
		(!SEGD + !SEGE + !SEGF + !SEGG) , (!SEGC + !SEGB + SEGA),	// char ?, offset=60
		(SEGD + SEGE + SEGF + SEGG) , (SEGC + SEGB + SEGA),			// char @, offset=64
		(!SEGD + SEGE + SEGF + SEGG) , (SEGC + SEGB + SEGA),		// char A, offset=68
		(SEGD + SEGE + SEGF + SEGG) , (SEGC + !SEGB + !SEGA),		// char B, offset=72
		(SEGD + SEGE + SEGF + !SEGG) , (!SEGC + !SEGB + SEGA),		// char C, offset=76
		(SEGD + SEGE + !SEGF + SEGG) , (SEGC + SEGB + SEGA),		// char D, offset=80
		(SEGD + SEGE + SEGF + SEGG) , (!SEGC + !SEGB + SEGA),		// char E, offset=84
		(!SEGD + SEGE + SEGF + SEGG) , (!SEGC + !SEGB + SEGA),		// char F, offset=88
		(SEGD + SEGE + SEGF + SEGG) , (SEGC + !SEGB + SEGA),		// char G, offset=92
		(!SEGD + SEGE + SEGF + SEGG) , (SEGC + SEGB + !SEGA),		// char H, offset=96
		(!SEGD + !SEGE + !SEGF + !SEGG) , (SEGC + !SEGB + !SEGA),	// char I, offset=100
		(SEGD + SEGE + !SEGF + !SEGG) , (SEGC + SEGB + !SEGA),		// char J, offset=104
		(!SEGD + !SEGE + !SEGF + !SEGG) , (!SEGC + !SEGB + !SEGA),	// char K, offset=108
		(SEGD + SEGE + SEGF + !SEGG) , (!SEGC + !SEGB + !SEGA),		// char L, offset=112
		(!SEGD + SEGE + SEGF + !SEGG) , (SEGC + SEGB + !SEGA),		// char M, offset=116
		(!SEGD + SEGE + !SEGF + SEGG) , (SEGC + !SEGB + !SEGA),		// char N, offset=120
		(SEGD + SEGE + !SEGF + SEGG) , (SEGC + !SEGB + !SEGA),		// char O, offset=124
		(!SEGD + SEGE + SEGF + SEGG) , (SEGC + SEGB + SEGA),		// char P, offset=128
		(SEGD + !SEGE + SEGF + SEGG) , (SEGC + SEGB + SEGA),		// char Q, offset=132
		(!SEGD + SEGE + SEGF + SEGG) , (!SEGC + SEGB + SEGA),		// char R, offset=136
		(SEGD + !SEGE + SEGF + SEGG) , (SEGC + !SEGB + SEGA),		// char S, offset=140
		(SEGD + SEGE + SEGF + SEGG) , (!SEGC + !SEGB + !SEGA),		// char T, offset=144
		(SEGD + SEGE + SEGF + !SEGG) , (SEGC + SEGB + !SEGA),		// char U, offset=148
		(!SEGD + !SEGE + !SEGF + !SEGG) , (!SEGC + !SEGB + !SEGA),	// char V, offset=152
		(!SEGD + SEGE + SEGF + !SEGG) , (SEGC + SEGB + !SEGA),		// char W, offset=156
		(!SEGD + !SEGE + !SEGF + !SEGG) , (!SEGC + !SEGB + !SEGA),	// char X, offset=160
		(!SEGD + !SEGE + !SEGF + !SEGG) , (!SEGC + !SEGB + !SEGA),	// char Y, offset=164
		(SEGD + !SEGE + !SEGF + !SEGG) , (!SEGC + !SEGB + SEGA),	// char Z, offset=168
		(SEGD + !SEGE + !SEGF + !SEGG) , (!SEGC + !SEGB + SEGA),	// char [, offset=172
		(SEGD + !SEGE + !SEGF + !SEGG) , (!SEGC + !SEGB + SEGA),	// char \, offset=176
		(SEGD + !SEGE + !SEGF + !SEGG) , (!SEGC + !SEGB + SEGA),	// char ], offset=180
		(SEGD + !SEGE + !SEGF + !SEGG) , (!SEGC + !SEGB + SEGA),	// char ^, offset=184
		(SEGD + !SEGE + !SEGF + !SEGG) , (!SEGC + !SEGB + SEGA),	// char _, offset=188
		(SEGD + !SEGE + !SEGF + !SEGG) , (!SEGC + !SEGB + SEGA),	// char `, offset=192
};

// display a string starting at the first character (location 0) of the LCD display
void LCDWriteString(uint8 *ptext)
{
	uint8 location = 0;		// LCD character location 0 upwards

	// write characters until maximum for display or '\0' detected at end of string and then
	// complete with blank characters '>' (since space is below '0' in ASCII table and not supported)
	while ((location < LCDCHARS) && *ptext) 
		LCDWriteChar(location++, *(ptext++));
	while (location < LCDCHARS)
		LCDWriteChar(location++, '>'); 

	return;
}

// write the requested character at the specified LCD digit location
void LCDWriteChar(uint8 location, uint8 character)
{
	uint8 *pLCD;			// pointer to physical LCD

	// return with no action of location is beyond the number of digits in the LCD
	if (location >= LCDCHARS)
		return;

	// set pLCD to point to the physical LCD address
	pLCD = (uint8 *)&LCD_WF3TO0;

	// map lower case characters to upper case and characters out of range to blank '>'
	if (character >= 'a' && character <= 'z')
		character -= ('a' - 'A');
	if (character < '0' || character > 'Z')
		character = '>'; 

	// two waveform passes (for pass 2 bit 0 has the special symbol information)
	pLCD[WF_ORDERING_TABLE[2 * location]] = ASCII_TO_WF_CODIFICATION_TABLE[(character - '0') * 2];
	pLCD[WF_ORDERING_TABLE[2 * location + 1]] = ASCII_TO_WF_CODIFICATION_TABLE[(character - '0') * 2 + 1] | (pLCD[WF_ORDERING_TABLE[2 * location + 1]] & 0x01);

	return;
}
