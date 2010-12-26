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


//A very basic html generator
//When i get the chance i will improve it more by re-writting all the code from scratch.
//Currently supports : html text /fonts& tables
static FILE* out = NULL;
static unsigned bufferOut = 0;

INLINE void outWrite(const char* data)
{
	unsigned len ;

	if(!out)
		return;

	len = strlen(data);

	bufferOut += len;

	fwrite(data,1,len,out);

	if(bufferOut > 192)
	{
		bufferOut = 0;
		outWrite("\n");
	}

}

INLINE void outWriteWhitespace()
{
	bufferOut = 0;
	outWrite("\r\t");
}

INLINE void outWriteNextLine()
{	
	bufferOut=0;
	outWrite("<br>");
}

INLINE void outWriteH(const char* heading)
{
	outWrite("<h1>");
	outWrite(heading);
	outWrite("</h1>");
}

INLINE void outWriteHC(const char* heading)
{
	outWrite("<center>");
	outWriteH(heading);
	outWrite("</center>");
}

INLINE void outWriteH5(const char* heading)
{
	outWrite("<h5>");
	outWrite(heading);
	outWrite("</h5>");
}

INLINE void outWriteH52(const char* heading,const char* heading2)
{
	outWrite("<h5>");
	outWrite(heading);
	outWrite(heading2);
	outWrite("</h5>");
}

INLINE void outWriteHC5(const char* heading)
{
	outWrite("<center>");
	outWriteH5(heading);
	outWrite("</center>");
}

INLINE void outWriteHC52(const char* heading,const char* heading2)
{
	outWrite("<center>");
	outWriteH52(heading,heading2);
	outWrite("</center>");
}

INLINE void outWriteH4(const char* heading)
{
	outWrite("<h4>");
	outWrite(heading);
	outWrite("</h4>");
}

INLINE void outWriteH42(const char* heading,const char* heading2)
{
	outWrite("<h4>");
	outWrite(heading);
	outWrite(heading2);
	outWrite("</h4>");
}

INLINE void outWritePH(const char* heading,const char* heading2)
{
	outWrite("<center>");
	outWriteH42(heading,heading2);
	outWrite("</center>");
}

INLINE void outWriteHC4(const char* heading)
{
	outWrite("<center>");
	outWriteH4(heading);
	outWrite("</center>");
}

INLINE void outWritePG1(const char* paragraph)
{
	//outWrite("<p>");
	outWrite(paragraph);
	outWriteNextLine();//TEMP
	//outWrite("</p>");
}

INLINE void outWritePG2(const char* p1,const char* p2)
{
	//outWrite("<p>");
	outWrite(p1);
	outWrite(p2);
	outWriteNextLine();//TEMP
	//outWrite("</p>");
}

INLINE void outWriteColorREDFrontPart()
{
	outWrite("<font color = \"Red\">");
}

INLINE void outWriteColorFrontPart(const char* color)
{
	outWrite("<font color = \"");
	outWrite(color);
	outWrite("\">");
}

INLINE void outWriteColorBottomPart()
{
	outWrite("</font>");
}

INLINE void outWriteSizeFrontPart(const char* size)
{
	outWrite("<font size = \"");
	outWrite(size);
	outWrite("\">");
}

INLINE void outWriteSmallSizeFrontPart()
{
	outWrite("<font size = \"1\">");
}

INLINE void outWriteFontSizeBottomPart()
{
	outWrite("</font>");
}


void htmlgenInit(const char* dest)
{
	writeToLog("Starting html generator(dst = [%s])\n",dest);

	bufferOut = 0;

	if(out)
		htmlgenRelease();

	out = fopen(dest,"wb");
}

void htmlgenWriteFrontPart( const char* title )
{
	outWrite("<!DOCTYPE HTML PUBLIC '-//W3C//DTD HTML 4.01 Transitional//EN'>");
	outWriteNextLine();
	outWrite("<html><head><title>");
	outWrite(title);
	outWrite("</title></head><body>");
	outWriteNextLine();
	outWrite("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />");
	outWriteNextLine();
	outWrite("<style type=\"text/css\">body {color:green} p {margin-left: 10px}  h5 { color: #9A0AA1; }"
	"h4 { color: red; } </style>");
	outWriteNextLine();
	//outWrite("<style type=\"text/css\">body {color:green} h4 { font-family: sans-serif; } p { font-size: 10%; } h4 { color: red; } </style>");
	outWriteNextLine();
	outWrite("<table style=\"table-layout: fixed; width: 100px;\">");


	outWriteHC(title);
	outWriteNextLine();
}

void htmlgenWriteBottomPart()
{
	outWrite("</body></html>");
}

void htmlgenWriteBorderFrontPart()
{
	outWrite("<table border=\"1\">");
}

void htmlgenWriteBorderBottomPart()
{
	outWrite("</table>");
}

INLINE void writeBorderHeadingTH1(const char* heading)
{
	outWrite("<tr> <th>");
	outWrite(heading);
	outWrite("</th></tr>");
}

INLINE void writeBorderHeadingH1(const char* heading)
{
	outWrite("<th>");
	outWrite(heading);
	outWrite("</th>");
}

INLINE void writeBorderCaption(const char* cap)
{
	outWrite("<th>");
	outWriteSmallSizeFrontPart();
	outWriteColorFrontPart("Orange");
	outWrite(cap);
	outWriteFontSizeBottomPart();
	outWriteColorBottomPart();
	outWrite("</th>");
}

static void writeComplexString(const char* beginTag,const char* endTag,
			       const char* beginSubTag,const char* endSubTag,
			       const char* list,const char separator)
{
	const char* p;
	unsigned len = 0;
	
	char buf[MAX_PATH];

	outWrite(beginTag);

	*(buf + (MAX_PATH-1) ) = '\0';
	
	for( p = list; *p ; p++)
	{
		if( *p != separator)
			*(buf + (len++) ) = *p;
		else
		{
			//anything left to write?
			if(len)
			{
				*(buf + (len) ) = '\0';

				outWrite(beginSubTag);
				outWrite(buf);
				outWrite(endSubTag);
			}

			len = 0;
		}
	}

	outWrite(endTag);
}

INLINE void writeBorderHeadingList(const char* list,const char separator)
{
	writeComplexString("<tr>","</tr>","<th>","</th>",list,separator);
}

void htmlgenWriteBorderHeadingList()
{
	htmlgenWriteBorderFrontPart();
	writeBorderHeadingList("ROM;System;Copyrights;Dom. name;Ov. name;Product NO;Modem brand;Modem version;Controller"
			       " data;Countries;Checksum;Save alloc;",';');

	htmlgenWriteBorderBottomPart();

	outWriteNextLine();
	//outWrite("<style type=\"text/css\">body {color:green} h4 { font-family: sans-serif; } p { font-size: 10%; } h4 { color: red; } </style>");
 
}

INLINE void outWriteCenterFrontPart()
{
	outWrite("<center>");
}


INLINE void outWriteCenterBottomPart()
{
	outWrite("</center>");
}

INLINE void outWriteBoldFrontPart()
{
	outWrite("<b>");
}

INLINE void outWriteBoldBottomPart()
{
	outWrite("</b>");
}

unsigned char htmlgenWriteRom(RomInfo* rom)
{
	const char* fnOnly;
	unsigned char ALLIGNEDBY(16) buf[255] ;

	writeToLog("Generating HTML info for %s\n",rom->filename);

	if(!rom)
		return RST_FAILED;
	
	if(!rom->romData)
		return RST_FAILED;

	if(!out)
		return RST_FAILED;


	updateRomInfo(rom);

	buf[0]='\0';
	buf[254]='\0';

/* testing tables..
	htmlgenWriteBorderFrontPart();
 
	writeBorderCaption(extractFilename(rom->filename));
	writeBorderCaption(rom->system);
	writeBorderCaption(rom->copyright);
	writeBorderCaption(rom->domesticName);
	writeBorderCaption(rom->overseasName);
	writeBorderCaption(rom->productNo);

	
	htmlgenWriteBorderBottomPart();
 */

	//writeBorderCaption(rom->system);
	//writeBorderCaption(rom->system);

	outWriteCenterFrontPart();
	outWriteColorFrontPart("Red");
	outWriteSizeFrontPart("3");
	outWriteBoldFrontPart();
	fnOnly = extractFilename(rom->filename);

	//if((!fnOnly) || (fnOnly == (char*)1)) <- not needed anymore i wrapped strrchr 3/OCT/09
	//	fnOnly = rom->filename;

	outWritePG2("ROM Filename : ", fnOnly);
	outWriteBoldBottomPart();
	outWritePG2("System: ",rom->system);
	outWritePG2("Copyrights: ",rom->copyright);
	outWritePG2("Domestic name: ",rom->domesticName);
	outWritePG2("Overseas name: ",rom->overseasName);
	outWritePG2("Product No: ",rom->productNo);
	outWritePG2("Modem brand(if supported!): ",rom->modemBrand);
	outWritePG2("Modem version(if supported!): ",rom->modemVersion);
	outWritePG2("Controler data: ",rom->controlerData);
	outWritePG2("Memo: ",rom->memo);
	outWritePG2("Supported countries: ",rom->supportedCountries);
	sprintf(buf,"%03x",rom->checksum);
	buf[strlen(buf)]='\0';
	outWritePG2("Checksum: ",buf);buf[0]='\0';
	sprintf(buf,"%d",(rom->saveEndAddress-rom->saveStartAddress));
	buf[strlen(buf)]='\0';
	outWritePG2("Save alloc : ",buf);buf[0]='\0';
	sprintf(buf,"Supported IO DEVICE BITS (BITFLAGS) [JOY:%d ,JOY6:%d ,KEYBOARD:%d ,PRINTER:%d ,CONTROL BALL:%d ,FLOPPY DISK:%d",
			getBitFieldStatus(rom->IODeviceBits,JOYPAD),getBitFieldStatus(rom->IODeviceBits,JOYPAD_6BTN),
			getBitFieldStatus(rom->IODeviceBits,KEYBOARD),getBitFieldStatus(rom->IODeviceBits,PRINTER),
			getBitFieldStatus(rom->IODeviceBits,CONTROL_BALL),getBitFieldStatus(rom->IODeviceBits,FLOPPY_DISK)
		);

	buf[strlen(buf)]='\0';
	outWritePG1(buf);buf[0]='\0';


	sprintf(buf,"ACTIVATOR: %d , TEAMPLAY: %d , JOYSTIC MS: %d ,RS232C:%d , TABLET:%d , PADDLE_CONTROLLER:%d, CDROM:%d , MOUSE:%d]\n",
			getBitFieldStatus(rom->IODeviceBits,ACTIVATOR),getBitFieldStatus(rom->IODeviceBits,TEAMPLAY),
			getBitFieldStatus(rom->IODeviceBits,JOYSTIC_MS),getBitFieldStatus(rom->IODeviceBits,RS232C),
			getBitFieldStatus(rom->IODeviceBits,TABLET),getBitFieldStatus(rom->IODeviceBits,PADDLE_CONTROLLER),
			getBitFieldStatus(rom->IODeviceBits,CDROM),getBitFieldStatus(rom->IODeviceBits,MEGA_MOUSE)
		);

	buf[strlen(buf)]='\0';
	outWritePG1(buf);buf[0]='\0';

	outWriteFontSizeBottomPart();
	outWriteColorBottomPart();
	outWriteCenterBottomPart();

	outWriteNextLine();
	outWriteNextLine();

	return RST_SUCCESS;
}

void htmlgenWriteCopyrightInfo()
{
	outWriteNextLine();
	outWriteNextLine();
	outWriteNextLine();
	outWriteColorFrontPart("Black");
	outWriteSizeFrontPart("2");
	outWritePG1("<b>Opengenie framework & SuperMD tool - (C)2009-2010 (<a href=\"mailto:conleon1988@gmail.com\">Conleon1988</a></b>).");
	outWriteFontSizeBottomPart();
	outWriteColorBottomPart();
	outWriteCenterBottomPart();
	outWriteNextLine();
}

void htmlgenRelease()
{
	writeToLogSimple("Releasing html generator\n");

	if(out)
		fclose(out);

	bufferOut = 0;
	out = NULL;
}

