/*
	Copyright (c) 2009 - 2010 conleon1988 (conleon1988@gmail.com)

	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.
*/


	#ifndef _rom_h_
	#define _rom_h_
	#ifdef __cplusplus
		extern "C" 
		{
	#endif 
	#include "bit_field.h"
	#include "port.h"
	#include <stdio.h>

	//needed for SCD support
	static const unsigned ROM_MAX_LENGTH = ((8 * 1024 * 1024) + 512);

	//for faster way to detect compatible files
	static const unsigned ROM_MIN_LENGTH = 0x1f0 + 0x28;
/*
	region data
       E = Europe
       J = Japan
       U = USA       
       A = Asia
       B = Brazil
       4 = Brazil
       F = France
       8 = Hong Kong

	io data
      J = Joypad                4 = Team Play
      6 = 6-button Joypad       0 = Joystick for MS
      K = Keyboard              R = Serial RS232C
      P = Printer               T = Tablet
      B = Control Ball          V = Paddle Controller
      F = Floppy Disk Drive     C = CD-ROM
      L = Activator             M = Mega Mouse

*/
	//too many flags to use a normal bit flag 32bit integer because of the shifting it will overflow
	//so i made a bitfield...

	//IO
	#define JOYPAD (0)
	#define JOYPAD_6BTN (1)
	#define KEYBOARD (2)
	#define PRINTER (3)
	#define CONTROL_BALL (4)
	#define FLOPPY_DISK (5)
	#define ACTIVATOR (6)
	#define TEAMPLAY (7)
	#define JOYSTIC_MS (8)
	#define RS232C (9)
	#define TABLET (10)
	#define PADDLE_CONTROLLER (11)
	#define CDROM (12)
	#define MEGA_MOUSE (13)
	#define IODEVICE_COUNT (14)

	static const char  ALLIGNEDBY(16) IODEVICE_DATA[]  =
	{
		'J','6','K','P','B','F','L','4','0','R','T','V','C','M'
	};

	//regions
	#define EUROPE (0)
	#define JAPAN  (1)
	#define USA (2)
	#define ASIA (3)
	#define BRAZIL (4)
	#define FRANCE (5)
	#define HONGKONG (6)
	#define REGION_COUNT (7)

	static const char ALLIGNEDBY(16) REGION_DATA[] =
	{
		'E','J','U','A','B','F','8'
	};

	//reset modes for rom
	#define RESET_FAST (0)
	#define RESET_FULL (1)

	typedef struct RomInfo RomInfo;

	struct  RomInfo
	{
		/*New - Sega CD file handle. Remeber : The handle will remain open until you're done with it.Reset/release rom to close it! 
		This handle is important for fast streaming support in SCD patching*/
		FILE* scdHandle;

		/*jump level for scd*/
		unsigned scdJumpLevel;

		/*If failed loading , try again with another loader until max loader count*/
		unsigned short loadRetries;

		/*file mode md,smd,gen,bin,unknown*/
		unsigned char mode;

		/*total chunks*/	
		unsigned chunkCount;

		/* stores supported countries for fast bit testing*/
		BitField* countryBits;

		/*stores io device bit flags */
		BitField* IODeviceBits;

		/*keep track of the filename*/
		char ALLIGNEDBY(16) filename[0xFF];

		/* our position in the rom data , might come in handly later ! */
		unsigned streamAddress;

		/*rom data*/
		unsigned char ALLIGNEDBY(16) romHeader[512 + 1];
		unsigned char* romData;
		unsigned __int64 romLength;

		/* SEGA MEGA DRIVE  or  SEGA GENESIS - 16bytes*/
		char ALLIGNEDBY(16) system[0x10 + 1];

		/*(C)SEGA 1988.JUL- 16bytes*/
		char ALLIGNEDBY(16) copyright[0x10 + 1];

		/*domestic name 48bytes*/
		char ALLIGNEDBY(16) domesticName[0x30 + 1];

		/*overseas name 48bytes*/
		char ALLIGNEDBY(16) overseasName[0x30 + 1];

		/*GM XXXX...X GM MK-XXXX 00*/
		char ALLIGNEDBY(16) productNo[0x0e + 1];

		/*checksum of rom */
		unsigned short checksum;

		/*IO data */
		char ALLIGNEDBY(16) controlerData[0x10 + 1];

		/* rom start,end */
		unsigned romStartAddress;
		unsigned romEndAddress;

		/* ram start,end */
		unsigned ramStartAddress;
		unsigned ramEndAddress;

		/*save misc info */
		unsigned short saveMagic;
		unsigned short saveFlags;
		unsigned saveStartAddress;
		unsigned saveEndAddress;

		/*modem */
		unsigned short modemMagic;
		char ALLIGNEDBY(16) modemBrand[4 + 1];
		char ALLIGNEDBY(16) modemVersion[4 + 1];

		/*misc*/
		char ALLIGNEDBY(16) memo[0x28 + 1];

		/*countries*/
		char ALLIGNEDBY(16) supportedCountries[0x10 + 1];
	};


	RomInfo* createRom();
	RomInfo* releaseRom(RomInfo* rom);
	RomInfo* resetRom(RomInfo* rom,unsigned char resetMode);
	unsigned char loadRom(RomInfo* rom , const char* filename);
	unsigned char isSupportedRom(RomInfo* rom);
	//dump to console
	void debugDumpRomInfo(RomInfo* rom);

	void updateRomInfo(RomInfo* rom);//,const char* filename);
	#ifdef __cplusplus
		}
	#endif 
#endif



