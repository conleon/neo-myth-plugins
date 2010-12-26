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

//this fixes the linking order for the library...quick hack
#ifndef _opengenie_h_
#define _opengenie_h_

	#ifdef __cplusplus
		extern "C" 
		{
	#endif 

	typedef struct BitField BitField;
	typedef struct RomInfo RomInfo;
	typedef struct StackPatchDescription StackPatchDescription;
	
	//port.h
	/*version*/
	#define	OPENGENIE_MAJOR_VERSION 2
	#define OPENGENIE_MINOR_VERSION 7
	#define OPENGENIE_MAINTENANCE_VERSION 7
	#define OPENGENIE_BULD_VERSION 7
	#define OPENGENIE_BUILD_DATE 10/10/09
	#define OPENGENIE_VERSION_STR_CONV(__V__) #__V__
	#define OPENGENIE_VERSION_TO_STRING(__V__) OPENGENIE_VERSION_STR_CONV(__V__)

	/*platform config*/
	#define WINDOWS_BUILD

	#if defined UNIX_BUILD
		#define PATH_SEPARATOR_C '/'
		#define PATH_SEPARATOR_S "/"
		#undef ALLOW_DEBUG_BLOCKS
		#undef ALIGN_MEMORY
		#define DEFAULT_MEMORY_ALIGNMENT (16)
		#define ALLOW_ATTRIBUTES
		#define ALLOW_REGISTERS
		#define extractFilename(s) myStrrchr(s, '/')
		#include <dirent.h>
		#undef ALLOW_FILE_LOGGING
	#elif defined WINDOWS_BUILD
		#include <stdio.h>
		#include <string.h>
		#include <ctype.h>

		//vc6 hack
		static const int strncasecmp(const char* s , const char* fs,int len)
		{
			const char* p = s;
			int i;
			int found = 0;

			char low0 = tolower(fs[0]);

			for(;*p;++p)
			{
				if(tolower(*p) == low0)
				{
					p = (p + 1);//no need to check first 2 times
					found = 1;
					for(i = 1; i < len; i++,p++)
					{
						if(tolower(*p) != fs[i])
						{
							found = 0;
							p = (s+1);

							break;
						}
					}

					if(found)
						return 0;
				}
			}
			return 1;
		}
		
		static const int strcasecmp(const char* s , const char* fs)
		{
			return strncasecmp(s,fs,strlen(fs));
		}
		
		#undef ALLOW_FILE_LOGGING
		#define PATH_SEPARATOR_C '\\'
		#define PATH_SEPARATOR_S "\\"
		#undef ALLOW_DEBUG_BLOCKS
		#undef ALIGN_MEMORY
		#define DEFAULT_MEMORY_ALIGNMENT (16)
		#undef ALLOW_ATTRIBUTES
		#define ALLOW_REGISTERS
		#define extractFilename(s) myStrrchr(s, '\\')
		#include <windows.h>
	#endif
	
	/*my exit func*/
	#define MY_EXIT_FUNC atexit

	/*memory allocator*/
	#if defined (ALIGN_MEMORY)
		#define xalloc(__S__) memalign(DEFAULT_MEMORY_ALIGNMENT,__S__)
		#define xrealloc realloc
		#define ALLIGNEDBY(__S__) __attribute__ ((aligned (__S__)))
	#else
		#define xalloc malloc
		#define xrealloc realloc
		#define ALLIGNEDBY(__S__)
	#endif

	#define INLINE
	
	/*attributes*/
 	#if defined (ALLOW_ATTRIBUTES)
		#define ATTRIBUTE(__ATTR__)__attribute__ ((section (ATTR))) = { 0 }
		#define FORCE_INLINE __attribute__((always_INLINE))
		#define SET_FUNC_ATTR(__ATTR__) __attribute__ ((__ATTR__))
	#else
		#define ATTRIBUTE(__ATTR__)
		#define FORCE_INLINE
		#define SET_FUNC_ATTR(__ATTR__)
	#endif

	/* max path */
	#undef MAX_PATH
	#define MAX_PATH (512)

	/*misc*/
	#define getExtension(s) myStrrchr(s, '.')

	/*logging*/
	#if defined(ALLOW_CONSOLE_LOGGING)
		#define dumpToConsole printf
	#else
		#define dumpToConsole
	#endif

	/*registers?*/
	#if defined(ALLOW_REGISTERS)
		#define REG register
	#else
		#define REG
	#endif

	/*Dir iterator?*/
	#undef COMPILE_WITH_DIRECTORY_ITERATOR
	
	//framework.h
	/*std*/
	#include <malloc.h>
	#include <stdio.h>
	#include <string.h>
	#include <stdarg.h>
	#include <stdlib.h>

	/* rom format */
	#define FMT_SMD (3)
	#define FMT_GEN (4)
	#define FMT_BIN (5)
	#define FMT_MD  (6)
	#define FMT_32X (7)
	#define FMT_SCD (8)
	#define FMT_UNKNOWN (2)
	#define LOADERS_COUNT  (6)

	/* result type */
	#define RST_SUCCESS (1)
	#define RST_FAILED (0)

	/*Framework config bitfield size*/
	#define MIN_FW_CONFIG_SIZE (7)

	/*for global config bitfield*/
	#define AUTO_IPS_PATCH (0)
	#define AUTO_CHECKSUM_PATCH (1)
	
	/* framework internal bitfield configuration*/
	void initFrameworkConfig(unsigned short stackSize);
	void loadDefaultFrameworkConfig();
	void releaseFrameworkConfig();
	void frameworkExitFunc();
	BitField* getFrameworkConfigurationBitField();
	const char* getFrameworkVersionString();

	//bit_field.h
	#define BIT_UNKNOWN (0x2)
	#define BIT_ON  (0x1)
	#define BIT_OFF (0x0)
	
	struct BitField
	{
		unsigned char* bits;
		unsigned short bitsLen;
		unsigned short bitsPointer;
	};

	unsigned char getLastBitFieldFlag(BitField* field);
	unsigned char getBitFieldStatus(BitField* field,unsigned short bit);
	unsigned char getBitFieldStatusSafe(BitField* field,unsigned short bit);
	
	BitField* createBitField(unsigned short stackLen);
	BitField* setBitField(BitField* field,unsigned short bit , unsigned char status);
	BitField* setBitFieldRange(BitField* field,unsigned range,unsigned char status);
	BitField* setBitFieldSafe(BitField* field,unsigned short bit , unsigned char status);
	BitField* setBitField(BitField* field,unsigned short bit , unsigned char status);
	BitField* releaseBitFieldFields(BitField* field);
	BitField* releaseBitField(BitField* field);
	
	//checksum_patcher.h
	unsigned char applyChecksumPatchAutomatically(RomInfo* rom);
	unsigned char applyChecksumPatch(RomInfo* rom , unsigned short checksum);
	unsigned short getSavedChecksum(RomInfo* rom);
	unsigned short calculateChecksum(RomInfo* rom);
	
	//directory_iterator.h
	#ifdef COMPILE_WITH_DIRECTORY_ITERATOR
		#define DIRECTORY_STACK_SIZE ( 4092 )
		unsigned char dirIteratorInit(const char* dir);
		unsigned char dirIteratorGetStatus();

		const char* dirIteratorGetEntry();
		void dirIteratorRewind();
		void dirIteratorRelease();

		void dirIteratorSetFileScanCallback(unsigned char (*cb)(const char*));
	#endif
	
	//fmt_bin.h , fmt_smd.h , fmt_md.h
	unsigned char loadBINrom(RomInfo* rom,const char* filename);
	unsigned char loadSMDrom(RomInfo* rom,const char* filename);
	unsigned char loadMDHrom(RomInfo* rom,const char* filename);

	//gamegenie
	#define GAME_GENIE_MAX_CODE_LENGTH (16)
	#define GAME_GENIE_MAX_NAME_LENGTH (32)

	typedef struct GameGenieCode GameGenieCode;

	struct GameGenieCode
	{
		unsigned int address;
		unsigned short data;

		char gamePatchCode[GAME_GENIE_MAX_CODE_LENGTH];
		char gameName[GAME_GENIE_MAX_NAME_LENGTH];
	};

	GameGenieCode* createGameGenieCodeInterface(const char* gameName);
	GameGenieCode* releaseGameGenieCodeInterface(GameGenieCode* gg);

	unsigned char loadGameGenieCodeFromFile(GameGenieCode* gg,const char* cheatFile);
	unsigned char loadGameGenieCodeFromStream(GameGenieCode* gg,const char* cheatStream);
	unsigned char applyGameGenieCode(RomInfo* rom , GameGenieCode* gg);
	
	//htmlgen.h
	void htmlgenInit(const char* dest);
	void htmlgenWriteFrontPart(const char* title);
	void htmlgenWriteBottomPart();
	unsigned char htmlgenWriteRom(RomInfo* rom);
	void htmlgenRelease();
	void htmlgenWriteBorderHeadingList();
	void htmlgenWriteBorderFrontPart();
	void htmlgenWriteBorderBottomPart();
	void htmlgenWriteCopyrightInfo();
	
	//ips_patcher.h
	unsigned char applyIPSPatch(RomInfo* rom);
	//vc6 plugin
	unsigned char applyIPSPatch2(RomInfo* rom,const char* ipsFile);

	//logger.h
	#ifdef ALLOW_FILE_LOGGING
	#define writeToLog(__S__,...) writeToLogRaw(__LINE__,__FILE__,__S__,__VA_ARGS__)
	#define writeToLogSimple(__S__) writeToLogRaw(__LINE__,__FILE__,__S__,NULL)

	unsigned char initLogger(const char* file);
	unsigned char writeToLogRaw(unsigned line,const char* file,const char* s,...);

	void releaseLogger();

	#else//for vc6
		static void dummyWriteToLog(const char* s , ...){return;}
		#define writeToLog dummyWriteToLog
		#define writeToLogSimple(__S__) dummyWriteToLog(__S__,NULL);
	#endif
	
	//memory_patcher.h
	void patchRomAddress(RomInfo* rom,const char* data , unsigned address, unsigned len);
	void patchRomStringGarbage(RomInfo* rom,unsigned startAddress,unsigned endAddress);
	void patchRomSystemName(RomInfo* rom , const char* data);
	void patchRomCopyright(RomInfo* rom , const char* data);
	void patchRomDomesticName(RomInfo* rom , const char* data);
	void patchRomOverseasName(RomInfo* rom , const char* data);
	void patchRomProductNumber(RomInfo* rom , const char* data);
	void patchRomModemBrand(RomInfo* rom , const char* data);
	void patchRomModemVersion(RomInfo* rom , const char* data);
	void patchRomMemo(RomInfo* rom , const char* data);
	void patchRomIODeviceData(RomInfo* rom , BitField* field);
	void patchRomIODeviceDataS(RomInfo* rom , const char* data);
	void patchRomCountryList(RomInfo* rom , BitField* field);
	void patchRomCountryListS(RomInfo* rom , const char* data);
	
	//raw_patchfile.h
	unsigned char importRawPatchFile(RomInfo* rom , unsigned __int64 address,unsigned __int64 limitLen,unsigned __int64 realLimitLen,const char* patchFile );
	
	//rom.h
//needed for SCD support
	static const unsigned ROM_MAX_LENGTH = ((8 * 1024 * 1024) + 512);

	//for faster way to detect compatible files
	static const unsigned ROM_MIN_LENGTH = 0x1f0 + 0x28;
	static const unsigned ROM_SAVE_OVERFLOW = 0x10000 * 2;
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
		char ALLIGNEDBY(16) filename[0xFF + 2];

		/* our position in the rom data , might come in handly later ! */
		unsigned streamAddress;

		/*rom data*/
		unsigned char ALLIGNEDBY(16) romHeader[0xFF + 2];
		unsigned char* romData;
		unsigned __int64 romLength;

		/* SEGA MEGA DRIVE  or  SEGA GENESIS - 16bytes*/
		char ALLIGNEDBY(16) system[0x10 + 2];

		/*(C)SEGA 1988.JUL- 16bytes*/
		char ALLIGNEDBY(16) copyright[0x10 + 2];

		/*domestic name 48bytes*/
		char ALLIGNEDBY(16) domesticName[0x30 + 2];

		/*overseas name 48bytes*/
		char ALLIGNEDBY(16) overseasName[0x30 + 2];

		/*GM XXXX...X GM MK-XXXX 00*/
		char ALLIGNEDBY(16) productNo[0x0e + 2];

		/*checksum of rom */
		unsigned short checksum;

		/*IO data */
		char ALLIGNEDBY(16) controlerData[0x10 + 2];

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
		char ALLIGNEDBY(16) modemBrand[4 + 2];
		char ALLIGNEDBY(16) modemVersion[4 + 2];

		/*misc*/
		char ALLIGNEDBY(16) memo[0x28 + 2];

		/*countries*/
		char ALLIGNEDBY(16) supportedCountries[0x10 + 2];
	};


	RomInfo* createRom();
	RomInfo* releaseRom(RomInfo* rom);
	RomInfo* resetRom(RomInfo* rom,unsigned char resetMode);
	unsigned char deinterleaveBuffer(char* out , const char* in , int len , int startAddress);
	unsigned char mountRom(RomInfo* rom , unsigned char* ownMemory , const int ownMemoryLen,unsigned char mode);
	unsigned char loadRom(RomInfo* rom , const char* filename);
	unsigned char isSupportedRom(RomInfo* rom);
	void unMountRom(RomInfo* rom);//WARNING!!! ONLY FOR FOR MEMORY THAT YOU DONT OWN!
	//dump to console
	void debugDumpRomInfo(RomInfo* rom);

	void updateRomInfo(RomInfo* rom);//,const char* filename);
	
	//rom_writter.h
	unsigned char writeRomHeader(RomInfo* rom ,const char* outFilename);
	unsigned char writeRom(RomInfo* rom,const char* outFilename,unsigned char forceMode);
	
	//stack_patch.h
	#define STACK_PATCH_ALLOC_BLOCK (64)
	
	struct StackPatchDescription
	{
		unsigned int patchDataLength;
		unsigned __int64 patchAddress;
		char* patchData;
	};

	int getPatchDescriptionListLength();
	void stackPatchRewind();
	void releaseStackPatchList();
	void debugDumpStackPatchList();
	StackPatchDescription* addStackPatch(const char* data , unsigned int dataLen , unsigned __int64 address);
	//please use this only if you know exactly what you're doing!
	StackPatchDescription* addStackPatchManually();
	StackPatchDescription* getPatchDescription(int index);
	StackPatchDescription* getPatchDescriptionStackTop();
	
	//util.h
	unsigned getCharacterPositionInString(const char* str,const char ch,unsigned start);
	unsigned getLastCharacterPositionInString(const char* str,const char ch,unsigned start);
	void skipWhitespace(const char* p , unsigned short* pos);
	unsigned short extractShort(unsigned char* p);
	unsigned extractInt(unsigned char* p);
	void intToB(unsigned int in,char* dst4b);
	void shortToB(unsigned short in,char* dst2b);
	int compare(const void *first, const void *second);
	unsigned char getFileType(const char* path);
	void swap16(void *p, const int len);
	int hexToInt(const char* hexString);
	const char* myStrrchr(const char* s , const char c);
	
	#ifdef __cplusplus
		}
	#endif 
#endif


