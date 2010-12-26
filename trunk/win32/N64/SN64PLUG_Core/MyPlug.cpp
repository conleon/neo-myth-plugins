/*
	Copyright (c) 2010 - 2011 conleon1988 (conleon1988@gmail.com)

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

#include "stdafx.h"
#pragma warning( disable : 4002 )
#include <iostream>
#include "MyPlug.h"
#include "..\\SuperN64_Engine\\utility.h"
#include "..\\SuperN64_Engine\\sharedBuffers.h"
#include "..\\SuperN64_Engine\\io.h"
#include "..\\SuperN64_Engine\\md5_IO.h"
#include "..\\SuperN64_Engine\\n64_image.h"
#include "..\\SuperN64_Engine\\checksum.h"
#include "..\\SuperN64_Engine\\database.h"
#include "..\\SuperN64_Engine\\patch_IO.h"
#include "..\\NEO_SN64Engine_DynaStatsShared.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//SuperN64 engine does not need destination file buffer , but the plugin does!
static char dstBuf[1024 + 1];
static char ptchBuf[1024 + 1];
static NEON64DynamicStatsBlock dsbBuf;
static RomHeader head;
static bool mpLoaded = false;
std::string getModulePath();
static void mySyncFunc();
static IOHandle handle;

void CMyPlug::Release()
{
		delete this;
}

#define TRACE_PLUG(__S__) //MessageBox(NULL,__S__,"SN64PLUG Tracer",MB_OK);

bool CMyPlug::Patching(void* pBlock)
{	
	NEON64ResultBlock* block = (NEON64ResultBlock*)pBlock;

	if(!block)
		return false;

	if(!block->actionScript)
		return false;

	if(!block->actionScriptLength)
		return false;

	//if(!block->myDynaStatsCB)
	//	return false;

	dsbBuf.userData = dsbBuf.userData2 = NULL;
	
	//SBFS_ClearFast();
	goodN64NameBuffer[0] = '\0';
	ptchBuf[0] = '\0';

	NEODSB_Bind(&dsbBuf,block->myDynaStatsCB,mySyncFunc);

	if(!mpLoaded)
	{
		NEODSB_WriteTaskName("Setting internal config\0");
		//UTIL_SetMemorySafe((char*)&head,0,sizeof(RomHeader));

		const std::string mp = getModulePath();

		if(mp.length())
		{
			mpLoaded = true;
			UTIL_CopyString(workDirectoryBuffer+0,mp.c_str(),"\0");
		}
	}

	//default config
	NEODSB_WriteTaskName("Loading script\0");
	unsigned int regionPatchByte = 0xFF;
	block->saveType = NEON64_SAVE_NONE;
	block->romFormat = NEON64_FMT_Z64;//will always be converted to BINARY
	block->bootChip = NEON64_CIC_6102;

	int fixCRC = 0;
	int pushImports = 0;

	const char* p = UTIL_StringFindConst(block->actionScript,"$SOURCE_FILE");

	if(!p)
		return false;

	UTIL_SubString(filenameBuffer,p,"$SOURCE_FILE=",";");

	NEODSB_WriteRomName(filenameBuffer);

	p = UTIL_StringFindConst(block->actionScript,"$DESTINATION_FILE");

	if(!p)
		return false;

	UTIL_SubString(dstBuf,p,"$DESTINATION_FILE=",";");

	TRACE_PLUG(filenameBuffer);
	TRACE_PLUG(dstBuf);	

	p = UTIL_StringFindConst(block->actionScript,"$FIX_CRC");

	if(p)
	{
		p += 9;

		if(*p)
			fixCRC = (int)(*p - '0');
	}

	p = UTIL_StringFindConst(block->actionScript,"$FORCE_REGION");

	if(p)
	{
		p += 14;

		for(int i = 0; i < NEON64_REGION_COUNT; i++)
		{
			if(UTIL_StringFindConst(p,NEON64_REGION_STRING_TABLE[i]))
			{
				regionPatchByte = NEON64_REGION_CODE_TABLE[i];
				break;
			}
		}
	}

	p = UTIL_StringFindConst(block->actionScript,"$IMPORT_SRC");

	if(p)
	{
		pushImports = 1;
		UTIL_SubString(ptchBuf+0,p,"$IMPORT_SRC=",";");
	}
	else
		pushImports = 0;


	NEODSB_WriteTaskName("Loading handles\0");
	//First copy to $destination
	FILE* src = fopen(filenameBuffer,"rb");

	if(!src)
		return false;

	FILE* dst = fopen(dstBuf,"wb");

	if(!dst)
	{
		fclose(src);
		return false;
	}

	NEODSB_WriteTaskName("Getting statistics\0");
	fseek(src,0,SEEK_END);
	int len = ftell(src);
	fseek(src,0,SEEK_SET);

	if(!UTIL_IsMultipleOf(len,4) || (len <= sizeof(RomHeader)))
	{
		fclose(src);
		fclose(dst);
		block->romFormat = NEON64_FMT_UNKNOWN;

		return false;
	}

	char inSig[4 + 1];
	fread(inSig,1,4,src);
	fseek(src,0,SEEK_SET);

	const NEON64_RomFormat romFmt = N64IMG_DetectFormatFromBuffer(inSig);

	if(romFmt == NEON64_FMT_UNKNOWN)
	{
		fclose(src);
		fclose(dst);
		block->romFormat = NEON64_FMT_UNKNOWN;

		return false;
	}

	int blocks = len / CHUNK_32K;

	TRACE_PLUG("Copy");

	int prg = 0;

	NEODSB_WriteTaskName("Generating output\0");

	const float fLen = (float)(len);

	MD5_BeginAdditiveCalculation();

	while(blocks--)
	{
		NEODSB_SyncThread();

		if(fread(chunkBuffer,1,CHUNK_32K,src) != CHUNK_32K)
		{
			fclose(src);
			fclose(dst);
			block->romFormat = NEON64_FMT_UNKNOWN;

			return false;
		}

		//Get original md5 from source and push it
		NEODSB_SyncThread();
		MD5_Push(chunkBuffer,CHUNK_32K);

		//Convert in this pass
		switch(romFmt)
		{
			case NEON64_FMT_Z64:
				break;

			case NEON64_FMT_V64:
				{
					NEODSB_SyncThread();
					UTIL_Byteswap16(chunkBuffer,CHUNK_32K);
					break;
				}

			case NEON64_FMT_N64:
				{
					NEODSB_SyncThread();
					UTIL_Byteswap32(chunkBuffer,CHUNK_32K);
					break;
				}
		}

		NEODSB_SyncThread();
		if(fwrite(chunkBuffer,1,CHUNK_32K,dst) != CHUNK_32K)
		{
			fclose(src);
			fclose(dst);
			block->romFormat = NEON64_FMT_UNKNOWN;

			return false;
		}

		NEODSB_WriteTaskProgress((int)(((float)(prg) / fLen) * 100));
		prg += CHUNK_32K;
	}

	MD5_EndAdditiveCalculation();

	TRACE_PLUG("Copy  OK");
	NEODSB_WriteTaskProgress(100);
	fclose(src);
	fclose(dst);

	NEODSB_WriteTaskName("Preparing engine\0");
	
	IO_Init();
	IO_BindHandle(&handle);

	TRACE_PLUG("Open");
	NEODSB_WriteTaskName("Openning target\0");
	if(!IO_Open(dstBuf,"r+b"))
		return false;

	TRACE_PLUG("Load MD5");
	NEODSB_WriteTaskName("Calculating MD5\0");
	if(!MD5_LoadToSharedBuffer())
	{
		IO_Close();
		IO_Shutdown();
		return false;
	}

	NEODSB_WriteTaskName("Checking compatibility\0");
	TRACE_PLUG("Is compatible?");
	if(!N64IMG_IsCompatible())
	{
		IO_Close();
		IO_Shutdown();
		return false;
	}

	TRACE_PLUG("Is compatible? = YES");

	NEODSB_WriteTaskName("Collecting results\0");

	TRACE_PLUG("FETCH header");
	if(!N64IMG_FetchHeader(&head))
	{
		IO_Close();
		IO_Shutdown();
		return false;
	}

	NEODSB_WriteTaskName("Calculating CIC\0");
	TRACE_PLUG("Get CIC");
	block->bootChip = N64IMG_DetectCICType(&head);

	NEODSB_WriteTaskName("Fetching SAVE TYPE\0");
	TRACE_PLUG("Get Save type");
	block->saveType = DB_GetSaveType();

	NEODSB_WriteRomName(goodN64NameBuffer);

	/*if(!IO_IsAlive())
	{
		IO_BindHandle(&handle);
		IO_Close();
		IO_Open(dstBuf,"r+b");
	}*/

	if(IO_IsAlive())
	{
		if(pushImports)
		{
			IOHandle* ptch = IO_OpenSingle(ptchBuf,"rb");

			if(ptch)
			{
				PIO_Import(IO_GetBindedStreamHandle(),ptch);

				IO_CloseSingle(ptch);
			}
			/*IOHandle* src = IO_GetBindedStreamHandle();
			IOHandle ptch;

			IO_BindHandle(&ptch);

			if(IO_Open(ptchBuf,"rb"))
			{
				PIO_Import(src,&ptch);

				if(IO_IsAlive())
				{
					IO_BindHandle(&ptch);
					IO_Close();
				}
			}
			else
				IO_BindHandle(src);
			*/
		}

		if(regionPatchByte != 0xFF)
		{
			NEODSB_WriteTaskName("Patching REGION \0");
			head.countryCode = regionPatchByte;
			N64IMG_WriteHeader(&head);
		}

		if(fixCRC)
		{
			if(!CRC_PatchAuto())
			{
				IO_Close();
				IO_Shutdown();
				return false;
			}

			TRACE_PLUG("RE-FETCH header");
			if(!N64IMG_FetchHeader(&head))
			{
				IO_Close();
				IO_Shutdown();
				return false;
			}
		}

		TRACE_PLUG("CLOSING");
		IO_Close();
		IO_Shutdown();

	}

	NEODSB_UnBind();
	TRACE_PLUG("ALL OK");
	NEODSB_WriteTaskName("All done0");

	dsbBuf.userData = (void*)&head;
	dsbBuf.userData2 = (void*)MD5CodeBuffer;

	NEODSB_WriteTaskProgress(100);
	return true;
}

std::string getModulePath()
{
	static char rst[ 1024 ];
	std::string s = std::string( rst, GetModuleFileName( NULL, rst, 1024 ) );

	int i = s.length();

	while(i-- > 0)
	{
		if(s[i]=='\\')
		{
			s = s.substr(0,i+1);

			s += "Plug_In\\N64\\";
			break;
		}
	}

	return s;
}

static void mySyncFunc()
{
	/*
	static MSG dispatch;

	while (PeekMessage( &dispatch, NULL, 0, 0, PM_NOREMOVE))
	{
		if (!AfxGetThread()->PumpMessage()){}
	}*/
}

