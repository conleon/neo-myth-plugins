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
#include <string.h>
unsigned SET_FUNC_ATTR(hot) getCharacterPositionInString(const char* str,const char ch,unsigned start)
{
	const char* startAddr = (str + start);

	//faster version! 4 / OCT / 09 conle
	for(str = startAddr;*str;++str)
	{
		if(*str == ch)
			return (str - startAddr);
	}

	return 0;
/*
	unsigned i = 0;

	for(i = start; i < strlen(str); i++)
	{
		if( *(str + i) == ch)
			return i;
	}

	return 0;
*/
}

unsigned SET_FUNC_ATTR(hot) getLastCharacterPositionInString(const char* str,const char ch,unsigned start)
{
	const char* startAddr = (str + start);
	const char* addr = startAddr;

	//faster version! 4 / OCT / 09 conle
	for(str = startAddr;*str;++str)
	{
		if(*str == ch)
			addr = str;
	}

	return (addr - startAddr);
/*

	unsigned i = 0;
	unsigned lastFound = 0;

	for(i = start; i < strlen(str); i++)
	{
		if( *(str + i) == ch)
			lastFound = i;
	}

	return lastFound;
*/
}

/*
' '	(0x20)	space (SPC)
'\t'	(0x09)	horizontal tab (TAB)
'\n'	(0x0a)	newline (LF)
'\v'	(0x0b)	vertical tab (VT)
'\f'	(0x0c)	feed (FF)
'\r'	(0x0d)	carriage return (CR)
*/
void SET_FUNC_ATTR(hot) skipWhitespace(const char* p , unsigned short* pos)
{
	while( *(p + (*pos)) && isspace( *(p + (*pos)) ) )
		*pos++;
}

unsigned short SET_FUNC_ATTR(hot) extractShort(unsigned char* p)
{
	unsigned short usa = ((unsigned short)*(p + 0) << 8);
	unsigned short usb = ((unsigned short)*(p + 1));

	return usa | usb;
}

unsigned SET_FUNC_ATTR(hot) extractInt(unsigned char* p)
{
	unsigned ua = ((unsigned)*(p + 0) << 24);
	unsigned ub = ((unsigned)*(p + 1) << 16);
	unsigned uc = ((unsigned)*(p + 2) << 8);
	unsigned ud = ((unsigned)*(p + 3));	

	return ua | ub | uc | ud;
}

#include <windows.h>

void SET_FUNC_ATTR(cold) shortToB(unsigned short in,char* dst2b)
{
    *(dst2b + 0) = (in >> 8);
	*(dst2b + 1) = (in&0xFF);
}

void SET_FUNC_ATTR(cold) intToB(unsigned int in,char* dst4b)
{
    *(dst4b + 0) = (in >> 24)& 0xFF;
	*(dst4b + 1) = (in >> 16)  & 0xFF;
	*(dst4b + 2) = (in >> 8) & 0xFF;
	*(dst4b + 3) = (in & 0xFF);

	/*
    *(dst4b + 0) = (in & 0xFF);
	*(dst4b + 1) = (in >> 8)  & 0xFF;
	*(dst4b + 2) = (in >> 16) & 0xFF;
	*(dst4b + 3) = (in >> 24) & 0xFF;
	*/
}

int compare(const void *first, const void *second)
{
	return *(char*)first - *(char*)second;
}

void swap16(void *p, const int len)
{
	unsigned char temp = 0;
	unsigned char *ptr = (unsigned char*)p;
	unsigned char *nextPtr = NULL;
	int i = 0;

	for(i = 0; i < len; i += 2)
	{
		nextPtr = (ptr + i);
		temp = *(nextPtr);

		*(ptr + 1) = *(nextPtr);
		*(nextPtr) = temp;
	}
}

//only 32bit hex! 0x-8chars
int SET_FUNC_ATTR(hot) hexToInt(const char* hexString)
{
	//vc6 - c89
	REG int factor = 1;
	REG int result = 0;
	REG int v = 0;

	if( (*(hexString + 0) == '0') && ((*(hexString + 1) == 'x') || (*(hexString + 1) == 'X') ) )
		hexString = (hexString + 2);
	else if( *(hexString + 0) == '$')
		hexString = (hexString + 1);

	for(;*hexString; ++hexString)
	{
		if( *hexString > 64 )
			v = ( *hexString - 65 ) + 10;
		else if( *hexString > 96 )
			v = ( *hexString - 97 ) + 10;
		else v = *hexString - 48;

		result += v * factor;
		factor *= 16;
	}
	
	return result;
}

	//fastest possible implementation : find the last character...starting by the end of the string
const char* SET_FUNC_ATTR(hot) myStrrchr(const char* s , const char c)
{
	int len = strlen(s) - 1;
	int i = 0;

	for(i=len; i > -1; --i)
	{
		if( *(s + i) == c)
			break;
	}

	if((s + (i+1)) != NULL)
		return (s + (i+1));
	else
		return (s + i);

	return s;
}


unsigned char SET_FUNC_ATTR(hot) SET_FUNC_ATTR(returns_twice) getFileType(const char* path)
{
	const char* ext = getExtension(path);

	//No extension fix provided by Chilly Willy 
	//This works because even if my macro fails , it will either return a null pointer
	//or pointer address + 1 XXX XXX XXX this doesn't happen anymore i added my own wrapper 3/OCT/09 XXX XXX XXX

	if((!ext))// || (ext == (char*)1))
	{
		writeToLog("Critical error. I can't enumerate rom file [%s] . Extension could not be found\n",path);
		return FMT_UNKNOWN;
	}

	//optimized to remove a couple of loops since the size is constant!
	if(!strncasecmp(ext,"smd",3))
		return FMT_SMD;
	else if(!strncasecmp(ext,"bin",3))
		return FMT_BIN;
	else if(!strncasecmp(ext,"gen",3))
		return FMT_GEN;
	else if(!strncasecmp(ext,"md",2))
		return FMT_BIN;
	else if(!strncasecmp(ext,"mgd",3))
		return FMT_MD;
	else if(!strncasecmp(ext,"mgd2",4))
		return FMT_MD;
	else if(!strncasecmp(ext,"32x",3))
		return FMT_32X;
	else if(!strncasecmp(ext,"MARS",4))
		return FMT_32X;

	return FMT_UNKNOWN;
}


