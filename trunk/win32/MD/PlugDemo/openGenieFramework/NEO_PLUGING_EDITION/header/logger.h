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

#ifndef _logger_h_
#define _logger_h_
	
	#ifdef __cplusplus
		extern "C" 
		{
	#endif 
	#include "port.h"
	#ifdef ALLOW_FILE_LOGGING
	#define writeToLog(__S__,...) writeToLogRaw(__LINE__,__FILE__,__S__,__VA_ARGS__)
	#define writeToLogSimple(__S__) writeToLogRaw(__LINE__,__FILE__,__S__,NULL)

	unsigned char initLogger(const char* file);
	unsigned char writeToLogRaw(unsigned line,const char* file,const char* s,...);

	void releaseLogger();
	#else//for vc6
		static void dummyWriteToLog(const char* s , ...){return;}
		#define writeToLog dummyWriteToLog
		#define writeToLogSimple(__S__) dummyWriteToLog(__S__,NULL);
	#endif

	#ifdef __cplusplus
		}
	#endif
#endif

