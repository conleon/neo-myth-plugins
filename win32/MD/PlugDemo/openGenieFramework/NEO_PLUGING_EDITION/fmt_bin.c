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

INLINE unsigned getBINLength(FILE* src)
{
	unsigned result = 0;

	fseek(src,0,SEEK_END);
	result = ftell(src);
	fseek(src,0,SEEK_SET);

	return result;
}

unsigned char SET_FUNC_ATTR(hot) SET_FUNC_ATTR(returns_twice) loadBINrom(RomInfo* rom,const char* filename)
{
	FILE* f;//vc c89
	unsigned isSCD;
	unsigned mustJumpSCDISO;
	char buf[15];

	if(!rom)
		return RST_FAILED;

	f = fopen(filename,"rb");
	
	if(!f)
		return RST_FAILED;

	rom->romLength = getBINLength(f);

	if(!rom->romLength)
	{
		fclose(f);
		return RST_FAILED;
	}

	isSCD = 0;
	mustJumpSCDISO = 0;

	buf[0]='\0'; buf[14]='\0';
 	
	if(fread(buf,1,14,f) != 14)
	{
		dumpToConsole("Warning : bin reading - data missmatch \n");
		writeToLogSimple("Warning : bin reading - data missmatch \n");
	}

	
	buf[14]='\0';

	if(!memcmp(buf,"SEGADISCSYSTEM",14))
	{
		isSCD = 1;
		mustJumpSCDISO = 0;
	}else
	{
		fseek(f,0x10,SEEK_SET);

		if(fread(buf,1,14,f) != 14)
		{
			dumpToConsole("Warning : bin reading - data missmatch \n");
			writeToLogSimple("Warning : bin reading - data missmatch \n");
		}

	
		buf[14]='\0';

		if(!memcmp(buf,"SEGADISCSYSTEM",14))
		{
			isSCD = 1;
			mustJumpSCDISO = 1;
		}
	}

	if(isSCD)
	{
		dumpToConsole("SCD Detected.Setting handle... !\n");
		writeToLogSimple("SCD Detected.Setting handle... !\n");

		rom->scdHandle = f;

		if(mustJumpSCDISO)
			rom->scdJumpLevel = ftell(f);
		else
			rom->scdJumpLevel = 0;
	
		rom->mode = FMT_SCD;
		rom->romLength = ROM_MIN_LENGTH;
		fseek(f,0x10,SEEK_SET);
	}
	else
	{
		fseek(f,0,SEEK_SET);
		rom->scdJumpLevel = 0;
	}

	rom->chunkCount = 0;
	
	if(rom->romData)
		free(rom->romData);

	rom->romData = (unsigned char*)xalloc((unsigned int)rom->romLength);

	if(!rom->romData)
	{
		fclose(f);
		return RST_FAILED;
	}

	if(fread(rom->romData,1,(unsigned int)rom->romLength,f) != rom->romLength)
	{
		dumpToConsole("Warning : bin reading - data missmatch \n");
		writeToLogSimple("Warning : bin reading - data missmatch \n");
	}

	if(!isSCD)//we need this for stream patching
		fclose(f); // rom->scdHandle = f!
	else
		fseek( rom->scdHandle ,0,SEEK_SET);

	return RST_SUCCESS;
}




