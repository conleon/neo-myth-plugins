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

#ifndef _util_h
#define _util_h
	#ifdef __cplusplus
		extern "C" 
		{
	#endif 
	unsigned getCharacterPositionInString(const char* str,const char ch,unsigned start);
	unsigned getLastCharacterPositionInString(const char* str,const char ch,unsigned start);
	void skipWhitespace(const char* p , unsigned short* pos);
	unsigned short extractShort(unsigned char* p);
	unsigned extractInt(unsigned char* p);
	int compare(const void *first, const void *second);
	unsigned char getFileType(const char* path);
	int hexToInt(const char* hexString);
	const char* myStrrchr(const char* s , const char c);
	#ifdef __cplusplus
		}
	#endif 
#endif


