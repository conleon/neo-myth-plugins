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
  Super Magic Drive Binary file-format (.BIN):
  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  This rom file-format is a simple rom dump. Nothing more to add!


  Super Magic Drive Interleaved file-format (.SMD):
  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  This is a much more complex file-format. It have a 512 bytes header
and is interleaved in 16KB blocks. These blocks have their even bytes
at the beginning and the odd bytes at the end of them.

  WHAT YOU FIND IN THE 512 BYTES HEADER:

0: Number of blocks                           1
1: H03                                        *
2: SPLIT?                                     2
8: HAA                                        *
9: HBB                                        *
ALL OTHER BYTES: H00

1: This first byte should have the number of 16KB blocks the rom has.
The header isn't part of the formula, so this number is:
            [size of rom-512]/16386
   If the size is more than 255, the value should be H00.

2: This byte indicates if the ROM is a part of a splitted rom series. If
the rom is the last part of the series (or isn't a splitted rom at all),
this byte should be H00. In other cases should be H40. See "CREATING
SPLITTED ROMS" for details on this format.

*: Fixed values


  THE DE-INTERLEAVING CODE (how to convert a SMD to a BIN):

1) Skip the 512 bytes header
2) Get 16KB from the ROM (16384 bytes)
3) Decode the block
4) Write the decoded block to the BIN file

  DECODING A SMD BLOCK (stating byte is 0):

1) Get Middlepoint (8192)
2) Get a byte from the block 
3) If the byte position is equal or smaller than middlepoint, put it
in the first unused EVEN position of the decoded buffer
4) If the byte position is greater than middlepoint, put it in the
first unused ODD position of the decoded buffer

  To convert a BIN to a SMD, just create a header (as explained before) and
then do the reverse process! 


  Multi Game Doctor file-format (.MD):
  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  The MD file format also doesn't have a header. The interleaving it uses
is equal to the SMD, but without the division in blocks! (Even bytes in
the end of file, odd bytes in the beginning).
  
  THE DECODING A MD (how to convert a MD to a BIN):

1) Get middlepoint ([size of rom]/2)
2) Get a byte from the ROM
3) If the byte position is equal or smaller than the middlepoint put the
byte in [byte position]*2 of the destination buffer
4) If the byte position is greater than the middlepoint, put the byte in 
([byte position]*2) - [size of rom] -1

*/
INLINE unsigned getSMDChunkCount(FILE* src)
{
	unsigned result = 0;

	//fseek(src,0,SEEK_SET);
	fseek(src,0,SEEK_END);
	result = ftell(src);
	fseek(src,0,SEEK_SET);

	if(!result)
		return 0;

	return ((result-512)/16384);
}

static unsigned char ALLIGNEDBY(16) chunkBuffer[16384];

unsigned char SET_FUNC_ATTR(cold) deinterleaveBuffer(char* out , const char* in , int len , int startAddress)
{
	REG int jmp_shl_j = 0;
	REG int j = 0;
	REG int jAddi = 0;
	int jump = 0;

	if(!out)
		out = (char*)malloc(len - startAddress);

	if(!out)
		return RST_FAILED;

	for(jump = 0; jump < len; jump += 16384)
	{
		for(j = 0; j < 8192; j++)
		{
			jmp_shl_j = jump + ( j << 1 );
			
			jAddi = startAddress+j;

			*(out +( jmp_shl_j + 1)) = *(in + jAddi);
			*(out +( jmp_shl_j + 0)) = *(in + (jAddi + 8192) );
		}
	}

	return RST_SUCCESS;
}

unsigned char SET_FUNC_ATTR(returns_twice) SET_FUNC_ATTR(hot) loadSMDrom(RomInfo* rom,const char* filename)
{
	
	unsigned jump = 0;
	//REG unsigned i = 0;
	REG unsigned j = 0;
	REG unsigned jmp_shl_j = 0;

	FILE* f;
	chunkBuffer[0] = '\0';
 
	if(!rom)
		return RST_FAILED;

	f = fopen(filename,"rb");
	
	if(!f)
		return RST_FAILED;

	rom->chunkCount = getSMDChunkCount(f);

	if(!rom->chunkCount)
	{
		fclose(f);
		return RST_FAILED;
	}
	
	if (fread(rom->romHeader,1,512,f)!=512)
	{
		rom->romLength = 0;
		fclose(f);

		return RST_FAILED;
	}

/*
	FILE* ff = fopen("../dumps/smd_header_dump.bin","wb");
	fwrite(rom->romHeader,1,512,ff);
	fclose(ff);
*/
	rom->romLength =  rom->chunkCount * 16384;
/*
	unsigned char* chunk = (unsigned char*)xalloc(16384);

	if(!chunk)
	{
		rom->romLength = 0;
		rom->chunkCount = 0;

		fclose(f);

		return RST_FAILED;
	}
*/
	//decoding
	if(rom->romData)
		free(rom->romData);

	rom->romData = (unsigned char*)xalloc((unsigned int)rom->romLength);

	if(!rom->romData)
	{
		dumpToConsole("SMD decoder : Out of memory!\n");
		writeToLogSimple("SMD decoder : Out of memory!\n");

		//free(chunk);
		fclose(f);
		rom->romLength = 0;
		rom->chunkCount = 0;
		
		return RST_FAILED;
	}

	//memset(rom->romData,0,rom->romLength);

	//dumpToConsole("SMD len [%d bytes](( * %d)) \n",rom->romLength,rom->chunkCount);
	//for(jump = 0 , i = 0 ; i < rom->chunkCount; i++ , jump += 16384) <- not needed (Conle)
	for(jump = 0; jump < rom->romLength;jump += 16384)
	{
		if(fread(chunkBuffer/*chunk*/,1,16384,f) != 16384)
		{
			dumpToConsole("Warning : reading chunk - invalid size\n");
			writeToLogSimple("Warning : reading chunk - invalid size\n");
		}
		
		for(j = 0; j < 8192; j++)
		{
			jmp_shl_j = jump + ( j << 1 );
			
			*(rom->romData +( jmp_shl_j + 1)) = /*chunk*/*(chunkBuffer+j);
			*(rom->romData +( jmp_shl_j + 0)) = /*chunk*/*(chunkBuffer +(j + 8192));
		}
	}

	//free(chunk);
	fclose(f);

	return RST_SUCCESS;
}



