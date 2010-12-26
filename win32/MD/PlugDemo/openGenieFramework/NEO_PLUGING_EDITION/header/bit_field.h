
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


#ifndef _bit_field_h_
#define _bit_field_h_
	#ifdef __cplusplus
		extern "C" 
		{
	#endif 

	#define BIT_UNKNOWN (0x2)
	#define BIT_ON  (0x1)
	#define BIT_OFF (0x0)

	typedef struct BitField BitField;

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

	#ifdef __cplusplus
		}
	#endif 
#endif


