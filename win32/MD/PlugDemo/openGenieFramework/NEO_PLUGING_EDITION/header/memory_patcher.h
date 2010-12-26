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

#ifndef _memory_patcher_h_
#define _memory_patcher_h_
	#ifdef __cplusplus
		extern "C" 
		{
	#endif 
	#include "rom.h"
	#include "bit_field.h"

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
	#ifdef __cplusplus
		}
	#endif 
#endif



