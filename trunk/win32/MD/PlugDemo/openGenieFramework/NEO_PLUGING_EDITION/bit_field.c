
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

BitField* SET_FUNC_ATTR(hot) createBitField(unsigned short stackLen) 
{

	BitField* field;//vc c89
	writeToLogSimple("Creating bitfield interface\n");

	field = (BitField*)xalloc(sizeof(BitField));

	field->bitsLen = stackLen + 1;
	field->bits = (unsigned char*)xalloc(field->bitsLen);

	memset(field->bits,BIT_OFF,field->bitsLen);
	field->bits[field->bitsLen-1]='\0';

	field->bitsPointer = 0;

	return field;
}

BitField* SET_FUNC_ATTR(returns_twice) setBitFieldSafe(BitField* field,unsigned short bit , unsigned char status)
{
	if(!field)
		return NULL;

	if(bit >= field->bitsLen-1)
		return field;

	field->bits[bit] = status;
	field->bitsPointer = bit;

	return field;
}

BitField* SET_FUNC_ATTR(hot) setBitField(BitField* field,unsigned short bit , unsigned char status)
{
	field->bits[bit] = status;
	field->bitsPointer = bit;

	return field;
}

BitField* SET_FUNC_ATTR(hot) setBitFieldRange(BitField* field,unsigned range,unsigned char status)
{
	unsigned i;

	for(i = 0; i < range; i++)
		field->bits[i] = status;

	field->bitsPointer = field->bits[i];

	return field;
}


unsigned char getLastBitFieldFlag(BitField* field)
{
	return field->bits[field->bitsPointer];
}

unsigned char SET_FUNC_ATTR(returns_twice) getBitFieldStatusSafe(BitField* field,unsigned short bit)
{
	if(!field)
		return BIT_UNKNOWN;

	if(bit > field->bitsLen-1)
		return field->bits[field->bitsLen-1];

	field->bitsPointer = bit;

	return field->bits[bit];
}


unsigned char SET_FUNC_ATTR(hot) getBitFieldStatus(BitField* field,unsigned short bit)
{
	field->bitsPointer = bit;

	return field->bits[bit];
}

BitField* SET_FUNC_ATTR(hot) releaseBitFieldFields(BitField* field)
{
	if(!field)
		return field;

	if(!field->bits)
		return field;

	free(field->bits);
	field->bits = NULL;

	return field;
}

BitField* SET_FUNC_ATTR(hot) releaseBitField(BitField* field)
{
	if(!field)
		return NULL;

	writeToLogSimple("Releasing bitfield interface\n");

	if(field->bits)
		free(field->bits);

	field->bits = NULL;
	field->bitsLen = 0;
	field->bitsPointer = 0;

	free(field);

	return NULL;
}



