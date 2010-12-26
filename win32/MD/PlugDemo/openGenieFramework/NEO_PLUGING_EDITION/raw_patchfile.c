
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

unsigned char SET_FUNC_ATTR(cold) importRawPatchFile(RomInfo* rom , unsigned __int64 address,unsigned __int64 limitLen,unsigned __int64 realLimitLen,const char* patchFile )
{
	StackPatchDescription* sp;
	FILE* pf ;
	unsigned __int64 len;
	unsigned char* buf;

	if(!rom)
		return RST_FAILED;

	if(!rom->romData)
		return RST_FAILED;

	pf = fopen(patchFile,"rb");

	if(!pf)
		return RST_FAILED;

	fseek(pf,0,SEEK_END);
	len = (unsigned __int64)ftell(pf);
	fseek(pf,0,SEEK_SET);

	if(limitLen)
	{
		if(len != limitLen)
			return RST_FAILED;
	}

	if(len > (int)rom->romLength)
		return RST_FAILED;

	if(rom->mode == FMT_SCD)
	{
		buf = xalloc((unsigned int)len);

		if(!buf)
		{
			fclose(pf);
			return RST_FAILED;
		}

		sp = addStackPatchManually();
		sp->patchAddress = address;
		sp->patchDataLength = (unsigned int)len;
		sp->patchData = buf;

		if(fread(sp->patchData,1,(unsigned int)len,pf) != len)
			writeToLogSimple("Warning unable to read all required bytes\n");
	}
	else
	{
		if(len != realLimitLen)
			len = realLimitLen;

		if(fread((rom->romData + address),1,(unsigned int)len,pf) != len)
			writeToLogSimple("Warning unable to read all required bytes\n");
		/*
		unsigned char* buf = xalloc(len);

		if(!buf)
		{
			fclose(pf);
			return RST_FAILED;
		}

		if(fread(buf,1,len,pf) != len)
			writeToLogSimple("Warning unable to read all required bytes\n");

		memcpy((rom->romData + address),buf,len);
		free(buf);
		*/
	}

	fclose(pf);

	return RST_SUCCESS;
}


