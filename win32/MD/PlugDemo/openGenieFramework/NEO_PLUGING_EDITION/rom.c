
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


#include "opengenie.h"


RomInfo* SET_FUNC_ATTR(hot)  createRom() 
{
	RomInfo* rom = xalloc(sizeof(RomInfo));

	rom->romData = NULL;
	rom->IODeviceBits = createBitField(IODEVICE_COUNT);
	rom->countryBits = createBitField(REGION_COUNT);
	rom->scdHandle = NULL;
	rom->scdJumpLevel = 0;

	#define nulli(__p__,__s__) rom->__p__[(__s__)] = '\0'
		nulli(romHeader,0);
		nulli(system,0);
		nulli(copyright,0);
		nulli(domesticName,0);
		nulli(overseasName,0);
		nulli(productNo,0);
		nulli(controlerData,0);
		nulli(memo,0);
		nulli(supportedCountries,0);
		nulli(modemBrand,0);
		nulli(modemVersion,0);
		nulli(filename,0);
	#undef nulli

	return rom;
}

RomInfo* SET_FUNC_ATTR(hot) SET_FUNC_ATTR(returns_twice) resetRom(RomInfo* rom,unsigned char resetMode)
{
	if(!rom)
		return NULL;

	if(rom->scdHandle)
		fclose(rom->scdHandle);

	rom->scdHandle = NULL;

	rom->scdJumpLevel = 0;

	if(rom->romData)
	{
		free(rom->romData);
		rom->romData = NULL;
	}

	switch(resetMode)
	{
		/* full reset , suggested for the first time */
		case RESET_FULL:
		{
			memset(rom->romHeader,'\0',0x200);
			memset(rom->filename,'\0',0xFF);
			memset(rom->system,'\0',0x10);
			memset(rom->copyright,'\0',0x10);
			memset(rom->domesticName,'\0',0x30);
			memset(rom->overseasName,'\0',0x30);
			memset(rom->productNo,'\0',0x0e);
			memset(rom->modemBrand,'\0',4);
			memset(rom->modemVersion,'\0',4);
			memset(rom->memo,'\0',0x28);
			memset(rom->supportedCountries,'\0',0x10);
			memset(rom->controlerData,'\0',0x10);
		}
		break;

		/* faster reset , without loops , but might result in garbage text' */
		case RESET_FAST:
		{
			#define nulli(__p__,__s__) rom->__p__[(__s__)] = '\0'
				nulli(romHeader,0x10);
				nulli(system,0x10);
				nulli(copyright,0x10);
				nulli(domesticName,0x30);
				nulli(overseasName,0x30);
				nulli(productNo,0x0e);
				nulli(controlerData,0x10);
				nulli(memo,0x28);
				nulli(supportedCountries,0x10);
				nulli(modemBrand,4);
				nulli(modemVersion,4);
				nulli(filename,0xFF);

				nulli(romHeader,0);
				nulli(system,0);
				nulli(copyright,0);
				nulli(domesticName,0);
				nulli(overseasName,0);
				nulli(productNo,0);
				nulli(controlerData,0);
				nulli(memo,0);
				nulli(supportedCountries,0);
				nulli(modemBrand,0);
				nulli(modemVersion,0);
				nulli(filename,0);
			#undef nulli
		}
		break;
	}

	rom->mode = FMT_UNKNOWN;

//	rom->countryBits = releaseBitField(rom->countryBits);
//	rom->IODeviceBits = releaseBitField(rom->IODeviceBits);

	//optimization : just reset bits
	if(rom->countryBits)
		rom->countryBits = setBitFieldRange(rom->countryBits,REGION_COUNT,BIT_OFF);

	//optimization : just reset bits
	if(rom->IODeviceBits)
		rom->IODeviceBits = setBitFieldRange(rom->IODeviceBits,IODEVICE_COUNT,BIT_OFF);

	rom->chunkCount = 0;
	rom->streamAddress = 0;
	rom->romLength = 0;
	rom->checksum = 0;
	rom->romStartAddress = 0;
	rom->romEndAddress = 0;
	rom->ramStartAddress = 0;
	rom->ramEndAddress = 0;
	rom->saveMagic = 0;
	rom->saveFlags = 0;
	rom->saveStartAddress = 0;
	rom->saveEndAddress = 0;
	rom->modemMagic = 0;
	rom->loadRetries = 0;

	return rom;
}

RomInfo* SET_FUNC_ATTR(hot) SET_FUNC_ATTR(returns_twice) releaseRom(RomInfo* rom)
{
	if(!rom)
		return NULL;

	if(rom->scdHandle)
		fclose(rom->scdHandle);

	rom->scdHandle = NULL;

	if(rom->romData)
		free(rom->romData);

	rom->romData = NULL;

	rom->countryBits = releaseBitField(rom->countryBits);
	rom->IODeviceBits = releaseBitField(rom->IODeviceBits);

	free(rom);

	return NULL;
}

/*
	this step is very important for some roms.
	it appears to store the region in full text.

	For example instead of the region code( E , U , J ...) ,they
	store the full text ( EUROPE , USA , JAPAN )
*/
static unsigned char SET_FUNC_ATTR(hot) mustSkipCountryJumptable(const char* code,BitField* field)
{
	unsigned char result = 0;

/*
	#define check(__S__,__V__)\
		if(!memcmp(code,__S__,strlen(__S__)))\
		{\
			setBitField(field,__V__,BIT_ON);\
			result = 1;\
		}
*/

	//optimized this a bit to remove a couple of loops
	#define check(__S__,__V__,__L__)\
		if(!memcmp(code,__S__,__L__))\
		{\
			setBitField(field,__V__,BIT_ON);\
			result = 1;\
		}

	check("EUR\0",EUROPE,3);
	check("EUROPE\0",EUROPE,6);
	check("JAPAN\0",JAPAN,5);
	check("USA\0",USA,3);
	check("ASIA\0",ASIA,4);
	check("BRAZIL\0",BRAZIL,6);
	check("FRANCE\0",FRANCE,6);
	check("HONGKONG\0",HONGKONG,8);

	#undef check

	return result;
}

static void SET_FUNC_ATTR(hot) updateCountryBitflags(RomInfo* rom)
{

	unsigned short i ;
	writeToLogSimple("Updating country bitflags\n");
/*
	rom->countryBits = releaseBitField(rom->countryBits);
	rom->countryBits = createBitField(REGION_COUNT);
*/

	//optimization : just reset bits
	if(rom->countryBits)
		rom->countryBits = setBitFieldRange(rom->countryBits,REGION_COUNT,BIT_OFF);
	else
		rom->countryBits = createBitField(REGION_COUNT);

	if(mustSkipCountryJumptable(rom->supportedCountries,rom->countryBits))
		return;

	//const char* p = NULL;

	//for(p = rom->supportedCountries; *p; p++)
	for(i = 0; i < 0x10; i ++)
	{
		//better skip the whitespace ... safer for cache miss in th switch
		//skipWhitespace(rom->supportedCountries,&i);

		switch( *(rom->supportedCountries + i) ) //*p)
		{
			case 'E':
				setBitField(rom->countryBits,EUROPE,BIT_ON);
			break;

			case 'J':
				setBitField(rom->countryBits,JAPAN,BIT_ON);
			break;

			case 'U':
				setBitField(rom->countryBits,USA,BIT_ON);
			break;

			case 'A':
				setBitField(rom->countryBits,ASIA,BIT_ON);
			break;

			//case '4' - '0':
			case '4':
			case 'B':
				setBitField(rom->countryBits,BRAZIL,BIT_ON);
			break;

			case 'F':
				setBitField(rom->countryBits,FRANCE,BIT_ON);
			break;

			//case '8' - '0' :
			case '8' :
				setBitField(rom->countryBits,HONGKONG,BIT_ON);
			break;

			default :
				//printf("Unsupported country code : -%d-[%c]\n",(int)*(rom->supportedCountries + i),*(rom->supportedCountries + i));
			break;
		}
	}
}

static void SET_FUNC_ATTR(hot) updateIODeviceFlags(RomInfo* rom)
{
	unsigned short i;
	writeToLogSimple("Updating IO DEVICE bitflags\n");

/*
	rom->IODeviceBits = releaseBitField(rom->IODeviceBits);
	rom->IODeviceBits = createBitField(IODEVICE_COUNT);
*/
	//optimization : just reset bits
	if(rom->IODeviceBits)
		rom->IODeviceBits = setBitFieldRange(rom->IODeviceBits,IODEVICE_COUNT,BIT_OFF);
	else
		rom->IODeviceBits = createBitField(IODEVICE_COUNT);


	for(i = 0 ; i < 0x10; i ++)
	{
		//skipWhitespace(rom->controlerData,&i);

		//printf("%d[%c],",(int)*(rom->controlerData + i),toupper(*(rom->controlerData + i)) );
		switch(*(rom->controlerData + i))
		{
			case 'J':
				setBitField(rom->IODeviceBits,JOYPAD,BIT_ON);
			break;

			case '6':
				setBitField(rom->IODeviceBits,JOYPAD_6BTN,BIT_ON);
			break;

			case 'K':
				setBitField(rom->IODeviceBits,KEYBOARD,BIT_ON);
			break;

			case 'P':
				setBitField(rom->IODeviceBits,PRINTER,BIT_ON);
			break;

			case 'B':
				setBitField(rom->IODeviceBits,CONTROL_BALL,BIT_ON);
			break;

			case 'F':
				setBitField(rom->IODeviceBits,FLOPPY_DISK,BIT_ON);
			break;

			case 'L':
				setBitField(rom->IODeviceBits,ACTIVATOR,BIT_ON);
			break;

			case '4':
				setBitField(rom->IODeviceBits,TEAMPLAY,BIT_ON);
			break;

			case '0':
				setBitField(rom->IODeviceBits,JOYSTIC_MS,BIT_ON);
			break;

			case 'R':
				setBitField(rom->IODeviceBits,RS232C,BIT_ON);
			break;

			case 'T':
				setBitField(rom->IODeviceBits,TABLET,BIT_ON);
			break;

			case 'V':
				setBitField(rom->IODeviceBits,PADDLE_CONTROLLER,BIT_ON);
			break;

			//case 'D'://not 100% sure
			case 'C':
				setBitField(rom->IODeviceBits,CDROM,BIT_ON);
			break;

			case 'M':
				setBitField(rom->IODeviceBits,MEGA_MOUSE,BIT_ON);
			break;

			default:
			break;
				//printf("Unsupported IO DEV code : -%d-[%c]\n",(int)*(rom->controlerData + i),*(rom->controlerData + i));
			break;
		}
	}

}

int SET_FUNC_ATTR(hot) fixIdent(const char* code,const int size)
{
	char c;
	int len = size ;

	while(len)
	{
		/*if(ialpha(code[len]))
			break;
		else if(alnum(code[len]))
			break; <- vc6++ doesnt support them*/

		
		c = toupper(code[len]);

		if( (c>='A') && (c <= 'Z') )
			break;
		else if ( (c>='0') && (c <= '9') )
			break;

		len--;
	}

	return len + 1;
}

void SET_FUNC_ATTR(hot) SET_FUNC_ATTR(returns_twice) updateRomInfo(RomInfo* rom)//,const char* filename)
{
	writeToLogSimple("Updating rom info\n");

	if(!rom)
		return;

	if(!rom->romData)
		return;

	//#define nulli(__p__) rom->__p__[strlen(rom->__p__)] = '\0'

	/*
		moved this to the loader
		unsigned fnLen = strlen(filename);
		memcpy(rom->filename,filename,fnLen);
		nulli(filename,fnLen+1);
		nulli(filename,0xFF);
	*/

	if(getFrameworkConfigurationBitField())
	{
		if(getBitFieldStatus(getFrameworkConfigurationBitField(),AUTO_IPS_PATCH) == BIT_ON)
			applyIPSPatch(rom);

		if(getBitFieldStatus(getFrameworkConfigurationBitField(),AUTO_CHECKSUM_PATCH) == BIT_ON)
			applyChecksumPatchAutomatically(rom);
	}

	rom->loadRetries = 0;
	rom->streamAddress = 0x1f0+0x10;// at countries
	rom->checksum = getSavedChecksum(rom);
	rom->saveMagic = extractShort( (rom->romData + 0x1b0) );
	rom->saveFlags = extractShort( (rom->romData + 0x1b2) );

	rom->saveStartAddress = extractInt( (rom->romData+0x1b4) );
	rom->saveEndAddress = extractInt( (rom->romData+0x1b8) );
	rom->romStartAddress = extractInt( (rom->romData+0x1a0) );
	rom->romEndAddress = extractInt( (rom->romData+0x1a4) );
	rom->ramStartAddress  = extractInt( (rom->romData+0x1a8) );
	rom->ramEndAddress  = extractInt( (rom->romData+0x1ac) );

	memcpy(rom->system , (rom->romData + 0x100) , 0x10 );
	memcpy(rom->copyright , (rom->romData + 0x110) , 0x10 );
	memcpy(rom->domesticName , (rom->romData + 0x120) , 0x30 );
	memcpy(rom->overseasName , (rom->romData + 0x150) , 0x30 );
	memcpy(rom->productNo , (rom->romData + 0x180) , 0x0e );
	memcpy(rom->controlerData, (rom->romData + 0x190) , 0x10 );
	memcpy(rom->modemBrand , (rom->romData + 0x1BC) , 4);
	memcpy(rom->modemVersion , (rom->romData + (0x1BC + 4)) , 2);
	memcpy(rom->memo , (rom->romData + 0x1c8) , 0x28 );
	memcpy(rom->supportedCountries, (rom->romData + 0x1f0) , 0x10 );

	/*
	nulli(system);
	nulli(copyright);
	nulli(domesticName);
	nulli(overseasName);
	nulli(productNo);
	nulli(controlerData);
	nulli(memo);
	nulli(supportedCountries);
	nulli(modemBrand);
	nulli(modemVersion);
	*/

			#define nulli(__p__,__s__) rom->__p__[fixIdent(rom->__p__,__s__)] = '\0'
				nulli(romHeader,0x10);
				nulli(system,0x10);
				nulli(copyright,0x10);
				nulli(domesticName,0x30);
				nulli(overseasName,0x30);
				nulli(productNo,0x0e);
				nulli(controlerData,0x10);
				nulli(memo,0x28);
				nulli(supportedCountries,0x10);
				nulli(modemBrand,4);
				nulli(modemVersion,4);
				nulli(filename,0xFF);
			#undef nulli

	updateCountryBitflags(rom);
	updateIODeviceFlags(rom);
}

//just used for debugging
void SET_FUNC_ATTR(cold) debugDumpRomInfo(RomInfo* rom)
{

	if(!rom)
		return;

	#define p printf
		p("===============================================\n");
		p(":::::::::::::::::::::::::::::::::::::::::::::::\n");
		p("Debug dump of %s\n",rom->filename);
		p(":::::::::::::::::::::::::::::::::::::::::::::::\n");
		p("System  = %s\n",rom->system);
		p("Copyright = %s\n",rom->copyright);
		p("Domestic name =  %s\n",rom->domesticName);
		p("Overseas name = %s\n",rom->overseasName);
		p("Product No  = %s\n",rom->productNo);
		p("Modem brand(if supported!)  = [%s]\n",rom->modemBrand);
		p("Modem version(if supported!)  = [%s]\n",rom->modemVersion);
		p("Controler data = %s\n",rom->controlerData);
		p("Supported IO DEVICE BITS (BITFLAGS) [JOY:%d ,JOY6:%d ,KEYBOARD:%d ,PRINTER:%d ,CONTROL BALL:%d ,FLOPPY DISK:%d, \n",
			getBitFieldStatus(rom->IODeviceBits,JOYPAD),getBitFieldStatus(rom->IODeviceBits,JOYPAD_6BTN),
			getBitFieldStatus(rom->IODeviceBits,KEYBOARD),getBitFieldStatus(rom->IODeviceBits,PRINTER),
			getBitFieldStatus(rom->IODeviceBits,CONTROL_BALL),getBitFieldStatus(rom->IODeviceBits,FLOPPY_DISK)
		);

		p("ACTIVATOR: %d , TEAMPLAY: %d , JOYSTIC MS: %d ,RS232C:%d , TABLET:%d , PADDLE_CONTROLLER:%d, CDROM:%d , MOUSE:%d]\n",
			getBitFieldStatus(rom->IODeviceBits,ACTIVATOR),getBitFieldStatus(rom->IODeviceBits,TEAMPLAY),
			getBitFieldStatus(rom->IODeviceBits,JOYSTIC_MS),getBitFieldStatus(rom->IODeviceBits,RS232C),
			getBitFieldStatus(rom->IODeviceBits,TABLET),getBitFieldStatus(rom->IODeviceBits,PADDLE_CONTROLLER),
			getBitFieldStatus(rom->IODeviceBits,CDROM),getBitFieldStatus(rom->IODeviceBits,MEGA_MOUSE)
		);

		p("MEMO = %s\n",rom->memo);
		p("supported countries [%s]\n",rom->supportedCountries);
		p("supported countries(BITFLAGS) [EU:%d , JP:%d , US:%d, AA:%d, BR:%d, FR:%d, HK:%d]\n",
			getBitFieldStatus(rom->countryBits,EUROPE),getBitFieldStatus(rom->countryBits,JAPAN),
			getBitFieldStatus(rom->countryBits,USA),getBitFieldStatus(rom->countryBits,ASIA),getBitFieldStatus(rom->countryBits,BRAZIL),
			getBitFieldStatus(rom->countryBits,FRANCE),getBitFieldStatus(rom->countryBits,HONGKONG)
		);
		p("\n\n");
		p("Checksum = %08x\n",rom->checksum);
		p("Save magic = %08x\n",rom->saveMagic);
		p("Save flags = %08x\n",rom->saveFlags);
		p("Save Start address = %08x\n",rom->saveStartAddress);
		p("Save End address = %08x\n",rom->saveEndAddress);
		p("(diff( %d ))\n",rom->saveEndAddress-rom->saveStartAddress);
		p("Rom Start address = %08x\n",rom->romStartAddress);
		p("Rom End address = %08x\n",rom->romEndAddress);
		p("(diff( %d ))\n",rom->romEndAddress-rom->romStartAddress);
		p("Ram Start address = %08x\n",rom->ramStartAddress);
		p("Ram End address  = %08x\n",rom->ramEndAddress);
		p("(diff( %d ))\n",rom->ramEndAddress-rom->ramStartAddress);
		p("===============================================\n");
	#undef p
}

unsigned char SET_FUNC_ATTR(hot) SET_FUNC_ATTR(returns_twice) isSupportedRom(RomInfo* rom)
{
	int i = 0;

	if(!rom)
		return RST_FAILED;

	if(!rom->romData)
		return RST_FAILED;

	//Fix for unknown files
	if(rom->romLength < ROM_MIN_LENGTH )
	{
		writeToLog("Warning!! File [%s] is NOT A GEN/32X/SMS/Sega CD ROM!\n",rom->filename);
		return RST_FAILED;
	}

	memcpy(rom->system , (rom->romData + 0x100) , 0x10 );

	for(i = 0; i < 0x10;i++)
		rom->system[i] = toupper(rom->system[i]);

	rom->system[0x10] = '\0';

	if(!strstr(rom->system,"SEGA"))
		return RST_FAILED;

	if(strstr(rom->system,"MEGA"))
		return RST_SUCCESS;

	if(strstr(rom->system,"DRIVE"))
		return RST_SUCCESS;
	
	if(strstr(rom->system,"GENESIS"))
		return RST_SUCCESS;

	if(strstr(rom->system,"32X"))
		return RST_SUCCESS;

	if(strstr(rom->system,"MARS"))
		return RST_SUCCESS;
	
	return RST_FAILED;
}

unsigned char SET_FUNC_ATTR(hot) loadRom(RomInfo* rom , const char* filename)
{
	
	unsigned fnLen;
	writeToLog("Attempting to load rom [%s]\n",filename);

	if(!rom)
		return RST_FAILED;

	/*just in case*/
	if(rom->romData)
		free(rom->romData);

	rom->mode = getFileType(filename);

	fnLen = strlen(filename);

	if(fnLen > 0xFF-1)
		fnLen = 0xFF - 1;

	memcpy(rom->filename,filename,fnLen);

	*(rom->filename + (fnLen )) ='\0';

	switch(rom->mode)
	{
		case FMT_MD:
		{
			if(loadMDHrom(rom,filename) != RST_SUCCESS)
			{
				printf("Warning : unable to load rom %s\n",filename);
				writeToLog("Warning : unable to load rom %s\n",filename);
				return RST_FAILED;
			}

			if(isSupportedRom(rom) != RST_SUCCESS)
				return RST_FAILED;

			updateRomInfo(rom);
		}
		break;

		case FMT_SMD:
		{
			if(loadSMDrom(rom,filename) != RST_SUCCESS)
			{
				printf("Warning : unable to load rom %s\n",filename);
				writeToLog("Warning : unable to load rom %s\n",filename);
				return RST_FAILED;
			}

			if(isSupportedRom(rom) != RST_SUCCESS)
				return RST_FAILED;

			updateRomInfo(rom);//,filename);
		}
		break;

		case FMT_32X://same wth bin
		case FMT_GEN://same wth bin
		case FMT_BIN: //already decoded
		{
			if(loadBINrom(rom,filename) != RST_SUCCESS)
			{
				printf("Warning : unable to load rom %s\n",filename);
				writeToLog("Warning : unable to load rom %s\n",filename);
				return RST_FAILED;
			}

			if(isSupportedRom(rom) != RST_SUCCESS)
				return RST_FAILED;

			updateRomInfo(rom);//,filename);

		}
		break;

		default :
			break;
	}

	return RST_SUCCESS;
}

unsigned char SET_FUNC_ATTR(cold) mountRom(RomInfo* rom , unsigned char* ownMemory , const int ownMemoryLen,unsigned char mode)
{
	if(!rom || !ownMemory || !ownMemoryLen)
		return RST_FAILED;
	
	rom->mode = mode;
	rom->romData = ownMemory;
	rom->romLength = ownMemoryLen;
	
	return RST_SUCCESS;
}

//WARNING!!! ONLY FOR FOR MEMORY THAT YOU DONT OWN!
void SET_FUNC_ATTR(cold) unMountRom(RomInfo* rom)
{
	if(!rom)
		return;
		
	rom->mode = FMT_UNKNOWN;
	rom->romData = NULL;
	rom->romLength = 0;
	
}


