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


#ifdef ALLOW_FILE_LOGGING
#include "opengenie.h""

static FILE* logFile = NULL;
static char logBuffer[MAX_PATH];

unsigned char SET_FUNC_ATTR(cold) initLogger(const char* file)
{	
	*(logBuffer + 0) = '\0';
	*(logBuffer + (MAX_PATH-1)) = '\0';

	logFile = NULL;
	logFile = fopen(file,"wb");

	if(!logFile)
		return RST_FAILED;

	strcpy(logBuffer,"[OpenGenie framework logfile.(c)2009 - 2010 conleon1988@gmail.com]\r\n(SUCCESSFULL INIT)\r\n\r\n\r\n");

	unsigned len = strlen(logBuffer);

	*(logBuffer + len) = '\0';

	if(fwrite(logBuffer,1,len,logFile) != len)
		dumpToConsole("warning : Unable to write to log all the data. DISK FULL???\n");

	*(logBuffer + 0) = '\0';

	return RST_SUCCESS;
}

unsigned char SET_FUNC_ATTR(hot) writeToLogRaw(unsigned line,const char* file,const char* s,...)
{
	if(!logFile)
		return RST_FAILED;

	va_list vl;
	va_start(vl,s);
	vsprintf(logBuffer,s,vl);
	va_end(vl);

	unsigned len = strlen(logBuffer);
	
	*(logBuffer + len) = '\0';

	static char infoText[MAX_PATH];

	*(infoText + (MAX_PATH-1)) = '\0';

	sprintf(infoText,"[(%s)->(%d)] : ",file,line);

	unsigned infoLen = strlen(infoText);

	*(infoText + infoLen) = '\0';

	if(fwrite(infoText,1,infoLen,logFile) != infoLen)
		dumpToConsole("warning : Unable to write to log all the data. DISK FULL???\n");

	if(fwrite(logBuffer,1,len,logFile) != len)
		dumpToConsole("warning : Unable to write to log all the data. DISK FULL???\n");

/*	if(fwrite("\r\n",1,2,logFile) != 2)
		dumpToConsole("warning : Unable to write to log all the data. DISK FULL???\n");
*/

	return RST_SUCCESS;
}


void SET_FUNC_ATTR(cold) releaseLogger()
{
	*(logBuffer + 0) = '\0';
	*(logBuffer + (MAX_PATH-1)) = '\0';

	if(logFile)
		fclose(logFile);

	logFile = NULL;
}
#endif



