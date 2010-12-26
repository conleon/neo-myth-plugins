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

INLINE unsigned getMDHLength(FILE* src)
{
	unsigned result = 0;

	fseek(src,0,SEEK_END);
	result = ftell(src);
	fseek(src,0,SEEK_SET);

	return result;
}

//TODO..
unsigned char SET_FUNC_ATTR(returns_twice) SET_FUNC_ATTR(hot) loadMDHrom(RomInfo* rom,const char* filename)
{
	FILE* f;

	REG int idx;
	REG int offs;
	int middle;
	unsigned char* buf;

	return RST_FAILED;//FOR NOW

	if(!rom)
		return RST_FAILED;

	f = fopen(filename,"rb");
	
	if(!f)
		return RST_FAILED;

	rom->romLength = getMDHLength(f);

	if(!rom->romLength)
	{
		fclose(f);
		return RST_FAILED;
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
		dumpToConsole("Warning : MDH reading - data missmatch \n");
		writeToLogSimple("Warning : MDH reading - data missmatch \n");
	}

	fclose(f);

	idx = 0;
	offs = 0;
	
	//double-buffered de-interleaving for now
	middle = (int)rom->romLength / 2;
	buf = (unsigned char*)xalloc((unsigned int)rom->romLength);

	//de interleave
	for(offs = 0; offs < middle; offs++)
	{
		*(buf + (idx++) ) = *(rom->romData +(middle + offs ));
		*(buf + (idx++) ) = *(rom->romData +(offs));
	}

	//TODO 

	free(rom->romData);

	rom->romData = buf;
/*
	FILE* dump = fopen("dmp.bin","wb");
	fwrite(rom->romData,1,(unsigned int)rom->romLength,dump);
	fclose(dump);
*/
	return RST_SUCCESS;
}

