
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
/*
  CALCULATING THE CHECKSUM:
  ^^^^^^^^^^^^^^^^^^^^^^^^

  Genesis checksum is simple enough... All you need to do is:
1) Checksum starts as zero
2) Skip the first 512 bytes of the ROM
3) Read a byte from the rom and multiply its ascii value by 256, then sum
  it to the checksum
4) Read the next byte from the rom and just sum it to the checksum
5) If you're not in the end of file, goto step 3
6) Get the first 16 bits from the resulting checksum and discard the higher
  bits
7) That's your checksum!

*/

unsigned char SET_FUNC_ATTR(returns_twice) SET_FUNC_ATTR(cold) applyChecksumPatch(RomInfo* rom , unsigned short checksum)
{
	if(!rom)
		return RST_FAILED;

	if(rom->mode == FMT_SCD)
	{
		writeToLog("I cannot patch the checksum of SegaCD roms yet! [in = %s ]\n",rom->filename);
		return RST_FAILED;
	}

	if(!rom->romData)
		return RST_FAILED;

	*(rom->romData + 0x18E) = checksum >> 8;
	*(rom->romData + 0x18F) = checksum & 0xFF;

	return RST_SUCCESS;
}

INLINE FORCE_INLINE unsigned short internalGetChecksumOnly(RomInfo* rom)
{
	register unsigned i = 0;
	register unsigned short checksum = 0;

	for (i = 0x200; i < rom->romLength; i += 2)
	{
		//MC68000 is L.E
		checksum +=
			((unsigned short)(*(rom->romData + (i + 0))) << 8) +
			(unsigned short)(*(rom->romData + (i + 1)) );
	}

	return checksum;
}

unsigned char SET_FUNC_ATTR(returns_twice) SET_FUNC_ATTR(cold) applyChecksumPatchAutomatically(RomInfo* rom)
{
	//VC6 only supports C89 :(
	unsigned short saved;
	unsigned short calculated;
	unsigned short newCS;

	if(!rom)
		return RST_FAILED;

	writeToLog("Checking checksum of [%s]\n",rom->filename);
	dumpToConsole("Checking checksum of [%s]\n",rom->filename);

	if(rom->mode == FMT_SCD)
	{
		writeToLog("I cannot patch the checksum of SegaCD roms yet! [in = %s ]\n",rom->filename);
		return RST_FAILED;
	}


	if(!rom->romData)
		return RST_FAILED;

	saved = getSavedChecksum(rom);
	calculated = internalGetChecksumOnly(rom);
	newCS = saved;

	if(saved != calculated)
	{
		dumpToConsole("Checksum missmatch - fixing  .old[%03X] , new[%03X] .\n",saved,calculated);
		writeToLog("Checksum missmatch - fixing  .old[%03X] , new[%03X] .\n",saved,calculated);

		*(rom->romData + 0x18E) = calculated >> 8;
		*(rom->romData + 0x18F) = calculated & 0xFF;

		newCS = calculated;
	}else
	{
        	dumpToConsole("Checksum is correct.  .Current[%03X] , Re-Calculated[%03X] .\n",saved,calculated);
			writeToLog("Checksum is correct.  .Current[%03X] , Re-Calculated[%03X] .\n",saved,calculated);
	}

	rom->checksum = newCS;

	return RST_SUCCESS;
}

unsigned short SET_FUNC_ATTR(returns_twice) SET_FUNC_ATTR(cold) getSavedChecksum(RomInfo* rom)
{
	if(!rom)
		return 0;

	if(!rom->romData)
		return 0;

	return  extractShort( (rom->romData + 0x18e) );
}

unsigned short SET_FUNC_ATTR(returns_twice) SET_FUNC_ATTR(cold) calculateChecksum(RomInfo* rom)
{
	if(!rom)
		return 0;

	if(!rom->romData)
		return 0;

	return internalGetChecksumOnly(rom);
}


