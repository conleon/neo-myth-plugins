
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

//TODO : find if the last 3 bytes represent EOF and do an optimized logic
//otherwise keep calling strcasecmp until the end of the stream - DONE 20/SEP/09 (conle)-

//ADDED 6/9/08
//HERE IS THE BUFFERED VERSION (conle)
// 10 SEP 09 added support for rom.ips format
unsigned char SET_FUNC_ATTR(returns_twice) SET_FUNC_ATTR(hot) applyIPSPatch(RomInfo* rom)
{
	FILE* f;
	char ALLIGNEDBY(16) name[MAX_PATH];
	unsigned char ALLIGNEDBY(16) buf[6];
	unsigned strLen = 0;
	unsigned delta;
	unsigned bufLen;
	unsigned char* ipsBuffer;
	unsigned length = 0;
	unsigned char useOptimizedLoop;
	unsigned pos;
	REG unsigned address = 0;
	REG unsigned i = 0;
	REG unsigned indexPointer = 5;
	const char* pbE;
	const char* pbO;
	const char* pbF;
	StackPatchDescription* sp;


	/* .ips file template :  rom.ext.ips*/
	if(!rom)
		return RST_FAILED;


	name[MAX_PATH-1]='\0';//memset(name,'\0',MAX_PATH);


	*(buf + 5)='\0';//memset(buf,'\0',6);

	/*add ext*/

	
	strcpy(name,rom->filename);
	strcat(name,".ips");	
	strLen = strlen(name);
	*(name+strLen) = '\0';

	f = fopen(name,"rb");

	if(!f)
	{
		//OK!! Here's what im going to do to avoid a few memory copies.
		//The string should now look like this  /path/filename.bin.ips
		//Now we are going to replace ".ext.ips" part with ".ips".
		pos = getCharacterPositionInString(name,'.',0);////(strLen>>1)); <-this will not work if there is no path

		if(!pos)
			return RST_FAILED;

		delta = strLen - 1;

		if(!delta)
			return RST_FAILED;

		pos++;//more over '.'

		memset( (name + pos) , '\0' , delta);
		strcat(name,"ips");
		*(name+(pos+4)) ='\0';

		//dumpToConsole("%s : %d \n",name,pos);
		f = fopen(name,"rb");

	}

	if(!f)
		return RST_FAILED;

	dumpToConsole("Applying IPS patch for %s with %s \n",rom->filename,name);
	writeToLog("Applying IPS patch for %s with %s \n",rom->filename,name);

	fseek(f, 0, SEEK_SET);
	if(fread(buf, 1, 5, f) != 5)
	{
		dumpToConsole("IPS PATCHER : DATA MIGHT BE CORRUPTED \n");
		writeToLogSimple("IPS PATCHER : DATA MIGHT BE CORRUPTED\n");
		fclose(f);

		return RST_FAILED;
	}

	*(buf + 5) = '\0';

	if(strcasecmp(buf, "patch"))
	{
		dumpToConsole("IPS PATCHER : Not a patch file!.\n");
		writeToLogSimple("IPS PATCHER : Not a patch file!.\n");
		fclose(f);

		return RST_FAILED;
	}

	bufLen = 0;

	fseek(f, 0, SEEK_SET);
	fseek(f,0,SEEK_END);
	bufLen = ftell(f);
	fseek(f,0,SEEK_SET);

	if(!bufLen)
	{
		dumpToConsole("IPS PATCHER : No data found\n");
		writeToLogSimple("IPS PATCHER : No data found\n");
		fclose(f);

		return RST_FAILED;
	}

	ipsBuffer = (unsigned char*)xalloc(bufLen);

	if(!ipsBuffer)
	{
		dumpToConsole("IPS PATCHER : OUT OF MEMORY \n");
		writeToLogSimple("IPS PATCHER : OUT OF MEMORY\n");
		fclose(f);

		return RST_FAILED;
	}

	if(fread(ipsBuffer,1,bufLen,f)!=bufLen)
	{
		dumpToConsole("IPS PATCHER : DATA MIGHT BE CORRUPTED \n");
		writeToLogSimple("IPS PATCHER : DATA MIGHT BE CORRUPTED\n");
		fclose(f);
		free(ipsBuffer);

		return RST_FAILED;
	}

	fclose(f);

	useOptimizedLoop = 0;

	pbE = (ipsBuffer + (bufLen - 3));

	if( (*pbE == 'E') || (*pbE == 'e') )
	{
		pbO = (ipsBuffer + (bufLen - 2));
	
		if( (*pbO == 'O') || (*pbO == 'o') )
		{
			pbF = (ipsBuffer + (bufLen - 1));

			if( (*pbF == 'F') || (*pbF == 'f') )
			{
				bufLen -= 3;
				useOptimizedLoop = 1;
			}
		}
	}

	sp = NULL; //for SCD ISO

	if(!useOptimizedLoop)
	{
		while((indexPointer < bufLen))
		{
			*(buf + 0) = *(ipsBuffer + (indexPointer++));
			*(buf + 1) = *(ipsBuffer + (indexPointer++));
			*(buf + 2) = *(ipsBuffer + (indexPointer++));
			*(buf + 3) = '\0';

			if(!strncasecmp(buf , "eof" , 3))
				break;

			address = (unsigned)*(buf + 2);
			address +=(unsigned)(*(buf + 1) << 8);
			address +=(unsigned)(*(buf + 0) << 16);

			//     EOF			eof
      			//if ((address == 0x454f46) || (address == 0x656f66) )
       				// break;

			*(buf + 0) = *(ipsBuffer + (indexPointer++));
			*(buf + 1) = *(ipsBuffer + (indexPointer++));
			//*(buf + 2) = '\0';

			length = (unsigned)*(buf + 1);
			length +=(unsigned)((*(buf + 0)) << 8);

			//*(buf + 1) = '\0';
			if(length == 0) //rle?
			{
				length = (unsigned)*(buf + 1);
				length +=(unsigned)((*(buf + 0)) << 8);
			}

			if(rom->mode != FMT_SCD)
			{
				for( i = 0; i < length; i++)
				{
					//*(buf + 0) = *(ipsBuffer + (indexPointer++));

					if((address < rom->romLength))
						*(rom->romData + (address++)) = *(ipsBuffer + (indexPointer++)); //*(buf + 0);  
				}
			}
			else
			{//SCD MODE
				if(length)
				{
					sp = addStackPatchManually();
					sp->patchAddress = address;
					sp->patchDataLength = length;
					sp->patchData = (char*)xalloc(length);

					if(sp->patchData)
						memcpy(sp->patchData,(rom->romData + (address)),length);
				}
			}
		}
	}
	else
	{
		while((indexPointer < bufLen))
		{
			*(buf + 0) = *(ipsBuffer + (indexPointer++));
			*(buf + 1) = *(ipsBuffer + (indexPointer++));
			*(buf + 2) = *(ipsBuffer + (indexPointer++));
			//*(buf + 3) = '\0';

			address = (unsigned)*(buf + 2);
			address +=(unsigned)(*(buf + 1) << 8);
			address +=(unsigned)(*(buf + 0) << 16);

			*(buf + 0) = *(ipsBuffer + (indexPointer++));
			*(buf + 1) = *(ipsBuffer + (indexPointer++));
			//*(buf + 2) = '\0';

			length = (unsigned)*(buf + 1);
			length +=(unsigned)((*(buf + 0)) << 8);

			if(length == 0) //rle?
			{
				length = (unsigned)*(buf + 1);
				length +=(unsigned)((*(buf + 0)) << 8);
			}
			//*(buf + 1) = '\0';

			if(rom->mode != FMT_SCD)
			{
				for( i = 0; i < length; i++)
				{
					//*(buf + 0) = *(ipsBuffer + (indexPointer++));

					if((address < rom->romLength))
						*(rom->romData + (address++)) = *(ipsBuffer + (indexPointer++)); //*(buf + 0);  
				}
			}
			else
			{//SCD MODE
				if(length)
				{
					sp = addStackPatchManually();
					sp->patchAddress = address;
					sp->patchDataLength = length;
					sp->patchData = (char*)xalloc(length);

					if(sp->patchData)
						memcpy(sp->patchData,(rom->romData + (address)),length);
				}
			}
		}
	}

	free(ipsBuffer);

	return RST_SUCCESS;
}

//vc6 plugin support
unsigned char SET_FUNC_ATTR(returns_twice) SET_FUNC_ATTR(hot) applyIPSPatch2(RomInfo* rom,const char* ipsFile)
{
	unsigned strLen = 0;
	unsigned delta = 0;
	unsigned bufLen = 0;
	unsigned char* ipsBuffer;
	unsigned length = 0;
	unsigned char useOptimizedLoop;
	unsigned pos = 0;
	REG unsigned address = 0;
	REG unsigned i = 0;
	REG unsigned indexPointer = 5;
	const char* pbE;
	const char* pbO;
	const char* pbF;
	StackPatchDescription* sp;
	char ALLIGNEDBY(16) name[MAX_PATH];
	unsigned char ALLIGNEDBY(16) buf[6];
	FILE* f;

	/* .ips file template :  rom.ext.ips*/
	if(!rom)
		return RST_FAILED;


	name[MAX_PATH-1]='\0';//memset(name,'\0',MAX_PATH);


	*(buf + 5)='\0';//memset(buf,'\0',6);

	/*add ext*/

	f = fopen(ipsFile,"rb");

	if(!f)
		return RST_FAILED;

	dumpToConsole("Applying IPS patch for %s with %s \n",rom->filename,name);
	writeToLog("Applying IPS patch for %s with %s \n",rom->filename,name);

	fseek(f, 0, SEEK_SET);

	if(fread(buf, 1, 5, f) != 5)
	{
		dumpToConsole("IPS PATCHER : DATA MIGHT BE CORRUPTED \n");
		writeToLogSimple("IPS PATCHER : DATA MIGHT BE CORRUPTED\n");
		fclose(f);

		return RST_FAILED;
	}

	*(buf + 5) = '\0';

	if(strcasecmp(buf, "patch"))
	{
		dumpToConsole("IPS PATCHER : Not a patch file!.\n");
		writeToLogSimple("IPS PATCHER : Not a patch file!.\n");
		fclose(f);

		return RST_FAILED;
	}

	bufLen = 0;

	fseek(f, 0, SEEK_SET);
	fseek(f,0,SEEK_END);
	bufLen = ftell(f);
	fseek(f,0,SEEK_SET);

	if(!bufLen)
	{
		dumpToConsole("IPS PATCHER : No data found\n");
		writeToLogSimple("IPS PATCHER : No data found\n");
		fclose(f);

		return RST_FAILED;
	}

	ipsBuffer = (unsigned char*)xalloc(bufLen);

	if(!ipsBuffer)
	{
		dumpToConsole("IPS PATCHER : OUT OF MEMORY \n");
		writeToLogSimple("IPS PATCHER : OUT OF MEMORY\n");
		fclose(f);

		return RST_FAILED;
	}

	if(fread(ipsBuffer,1,bufLen,f)!=bufLen)
	{
		dumpToConsole("IPS PATCHER : DATA MIGHT BE CORRUPTED \n");
		writeToLogSimple("IPS PATCHER : DATA MIGHT BE CORRUPTED\n");
		fclose(f);
		free(ipsBuffer);

		return RST_FAILED;
	}

	fclose(f);

	useOptimizedLoop = 0;

	pbE = (ipsBuffer + (bufLen - 3));

	if( (*pbE == 'E') || (*pbE == 'e') )
	{
		pbO = (ipsBuffer + (bufLen - 2));
	
		if( (*pbO == 'O') || (*pbO == 'o') )
		{
			pbF = (ipsBuffer + (bufLen - 1));

			if( (*pbF == 'F') || (*pbF == 'f') )
			{
				bufLen -= 3;
				useOptimizedLoop = 1;
			}
		}
	}

	sp = NULL; //for SCD ISO

	if(!useOptimizedLoop)
	{
		while((indexPointer < bufLen))
		{
			*(buf + 0) = *(ipsBuffer + (indexPointer++));
			*(buf + 1) = *(ipsBuffer + (indexPointer++));
			*(buf + 2) = *(ipsBuffer + (indexPointer++));
			*(buf + 3) = '\0';

			if(!strncasecmp(buf , "eof" , 3))
				break;

			address = (unsigned)*(buf + 2);
			address +=(unsigned)(*(buf + 1) << 8);
			address +=(unsigned)(*(buf + 0) << 16);

			//     EOF			eof
      			//if ((address == 0x454f46) || (address == 0x656f66) )
       				// break;

			*(buf + 0) = *(ipsBuffer + (indexPointer++));
			*(buf + 1) = *(ipsBuffer + (indexPointer++));
			//*(buf + 2) = '\0';

			length = (unsigned)*(buf + 1);
			length +=(unsigned)((*(buf + 0)) << 8);

			//*(buf + 1) = '\0';
			if(length == 0) //rle?
			{
				length = (unsigned)*(buf + 1);
				length +=(unsigned)((*(buf + 0)) << 8);
			}

			if(rom->mode != FMT_SCD)
			{
				for( i = 0; i < length; i++)
				{
					//*(buf + 0) = *(ipsBuffer + (indexPointer++));

					if((address < rom->romLength))
						*(rom->romData + (address++)) = *(ipsBuffer + (indexPointer++)); //*(buf + 0);  
				}
			}
			else
			{//SCD MODE
				if(length)
				{
					sp = addStackPatchManually();
					sp->patchAddress = address;
					sp->patchDataLength = length;
					sp->patchData = (char*)xalloc(length);

					if(sp->patchData)
						memcpy(sp->patchData,(rom->romData + (address)),length);
				}
			}
		}
	}
	else
	{
		while((indexPointer < bufLen))
		{
			*(buf + 0) = *(ipsBuffer + (indexPointer++));
			*(buf + 1) = *(ipsBuffer + (indexPointer++));
			*(buf + 2) = *(ipsBuffer + (indexPointer++));
			//*(buf + 3) = '\0';

			address = (unsigned)*(buf + 2);
			address +=(unsigned)(*(buf + 1) << 8);
			address +=(unsigned)(*(buf + 0) << 16);

			*(buf + 0) = *(ipsBuffer + (indexPointer++));
			*(buf + 1) = *(ipsBuffer + (indexPointer++));
			//*(buf + 2) = '\0';

			length = (unsigned)*(buf + 1);
			length +=(unsigned)((*(buf + 0)) << 8);

			if(length == 0) //rle?
			{
				length = (unsigned)*(buf + 1);
				length +=(unsigned)((*(buf + 0)) << 8);
			}
			//*(buf + 1) = '\0';

			if(rom->mode != FMT_SCD)
			{
				for( i = 0; i < length; i++)
				{
					//*(buf + 0) = *(ipsBuffer + (indexPointer++));

					if((address < rom->romLength))
						*(rom->romData + (address++)) = *(ipsBuffer + (indexPointer++)); //*(buf + 0);  
				}
			}
			else
			{//SCD MODE
				if(length)
				{
					sp = addStackPatchManually();
					sp->patchAddress = address;
					sp->patchDataLength = length;
					sp->patchData = (char*)xalloc(length);

					if(sp->patchData)
						memcpy(sp->patchData,(rom->romData + (address)),length);
				}
			}
		}
	}

	free(ipsBuffer);

	return RST_SUCCESS;
}
