// MyPlug.cpp: implementation of the CMyPlug class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyPlug.h"
#include "VirtualMachine.hpp"
#include "codeGenerator.hpp"
#include "plugCommon.hpp"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static CALLER lastCaller = CALLER_3RDPARTY;

bool  CMyPlug::Patching(NeoResultBlock *pBlock)
{
	//MessageBox(NULL,pBlock->arguments,"ARGS",MB_OK);
	disableSMSStream();
	disableEncodedStream();

	FOLLOW("STARTING");
	FOLLOW(getModulePath().c_str());

	if(!pBlock)
	{
		TRACE_PLUG_ERROR("Got NULL NeoResultBlock. The structure is broken!");
		return false;
	}

	if(!pBlock->arguments)
	{
		TRACE_PLUG_ERROR("Got NULL ARGUMENT BUFFER. ABORTING!");
		return false;
	}

	//SuperMD plug host special signal to set initial path 1 time "HsTp"
	if( (pBlock->arguments[0] == 'H') && (pBlock->arguments[1] == 's')
		&& (pBlock->arguments[2] == 'T') && (pBlock->arguments[3] == 'p') )
	{
		//TRACE_PLUG_INFO("Sync!");
		getPlugPath();
		setCaller(getModuleCaller());

		return true;
	}

	if(!pBlock->romBuffer)
	{
		TRACE_PLUG_ERROR("Got NULL ROM BUFFER. The rom is not loaded!");
		return false;
	}

	if(pBlock->romBufferLength < 0x100)
	{
		TRACE_PLUG_ERROR("The buffer size of the ROM is invalid. Does not match with the minimum!");
		return false;
	}

	if(lastCaller == CALLER_3RDPARTY)
	{
		lastCaller = getModuleCaller();
		setCaller(lastCaller);
	}

	/*
	switch(getModuleCaller())
	{
		case CALLER_SUPERMD_FRAMEWORK:
			TRACE_PLUG_INFO("Caller = SUPERMD tool/framework series");
		break;

		case CALLER_NEOMDMYTH:
			TRACE_PLUG_INFO("Caller = NEO MD Myth programmer");
		break;

		case CALLER_3RDPARTY:
			TRACE_PLUG_INFO("Caller = 3RDPARTY(UNSUPPORTED)");
		break;

	}*/

	pBlock->romFormat = NEOMD_FMT_UNKNOWN;

	std::vector<COpCode> bc;
	bc.clear();

	FOLLOW(pBlock->arguments);

	genCode(pBlock,bc);

//	if(( isSMSStream() && bc.empty()))//sms support is only IPS/GGC or no OP CODES..
//		return true;

	bool ret = runCode(bc,pBlock);

	bc.clear();

	if(pBlock->romFormat == NEOMD_FMT_UNKNOWN) //still??
		pBlock->romFormat = NEOMD_FMT_MD32X_1_32_NOS;

	//pad rom to 1Mbit if needed
	if(pBlock->romBufferLength < 131072)
	{
		unsigned char* p = new unsigned char[131072 + 2];

		if(p)
		{
			memcpy(p,pBlock->romBuffer,pBlock->romBufferLength);
			memset(p+pBlock->romBufferLength,'\0',131072-pBlock->romBufferLength);

			pBlock->romBufferLength = 131072;

			delete pBlock->romBuffer;

			pBlock->romBuffer = (void*)p;
		}
	}

	return ret;
}

void  CMyPlug::Release()
{
	delete this;
}


