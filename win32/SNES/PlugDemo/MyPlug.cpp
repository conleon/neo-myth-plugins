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

bool  CMyPlug::Patching(void* pBlock)
{
	NEOSNES::NeoResultBlock* block = (NEOSNES::NeoResultBlock*)pBlock;

	//MessageBox(NULL,pBlock->arguments,"ARGS",MB_OK);
	FOLLOW("STARTING");
	FOLLOW(getModulePath().c_str());

	if(!block)
	{
		TRACE_PLUG_ERROR("Got NULL NeoResultBlock. The structure is broken!");
		return false;
	}

	if(!block->romBuffer)
	{
		TRACE_PLUG_ERROR("Got NULL ROM BUFFER. The rom is not loaded!");
		return false;
	}

	if(block->romBufferLength < 0x100)
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

	if((block->romBufferLength & 0x3FF) != 0)
	{
		unsigned char* p = (unsigned char*)block->romBuffer;

		block->romBufferLength -= 0x200;

		if(!block->romBufferLength)
		{
			FOLLOW("...............................");
			return false;
		}

		memmove(p,p + 0x200,block->romBufferLength);//reindex region 512bytes backward
		memset(p + block->romBufferLength,'\0',0x200);//...
	}

	if(block->arguments)
	{
		std::vector<COpCode> bc;
		bc.clear();

		FOLLOW(block->arguments);

		genCode(block,bc);

		bool ret = runCode(bc,block);
		bc.clear();

		return ret;
	}

	return runCode(block);
}

void  CMyPlug::Release()
{
	delete this;
}


