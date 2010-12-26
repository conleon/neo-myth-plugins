#include "stdafx.h"
#include "plugCommon.hpp"
#include "VirtualMachine.hpp"
#include "codeGenerator.hpp"
#include "tracePlug.hpp"
#include "MyPlug.h"
#include "image.hpp"
#include "..//shared//madmonkey//md5int.h"
#include "..//shared//mic_//snespatcher//SnesPatcher.h"
#include "..//shared//mic_//snespatcher//SnesRegionPatcher.h"
#include "..//C//C89//utility.h"
#include "..//C//C89//cheat.h"
using namespace NEOSNES;

static CALLER plugCaller = CALLER_NEOMDMYTH;

bool OP_NOP_FUNC(COpCode& opRef,NeoResultBlock* dst);
bool OP_GGC_FUNC(COpCode& opRef,NeoResultBlock* dst);
bool OP_IPS_FUNC(COpCode& opRef,NeoResultBlock* dst);
bool OP_CRC_FUNC(COpCode& opRef,NeoResultBlock* dst);
bool OP_PALFIX_FUNC(COpCode& opRef,NeoResultBlock* dst);
bool OP_NTSCFIX_FUNC(COpCode& opRef,NeoResultBlock* dst);

static bool(*OPCODE_FUNCS[OPCODES_SIZE])(COpCode& , NeoResultBlock*) =
{ 
	OP_NOP_FUNC ,
	OP_GGC_FUNC , 
	OP_IPS_FUNC,
	OP_CRC_FUNC ,
	OP_PALFIX_FUNC ,
	OP_NTSCFIX_FUNC
};

bool OP_NOP_FUNC(COpCode& opRef,NeoResultBlock* dst)
{
	FOLLOW("OP_NOP");
	return true;
}

bool OP_NTSCFIX_FUNC(COpCode& opRef,NeoResultBlock* dst)
{
	FOLLOW("OP_NTSCFIX");
	SnesRegionPatcher srp;
	srp.Patch(REGION_NTSC,(char*)dst->romBuffer,dst->romBufferLength);

	return true;
}

bool OP_PALFIX_FUNC(COpCode& opRef,NeoResultBlock* dst)
{
	FOLLOW("OP_PALFIX");
	SnesRegionPatcher srp;
	srp.Patch(REGION_PAL,(char*)dst->romBuffer,dst->romBufferLength);

	return true;
}

bool OP_GGC_FUNC(COpCode& opRef,NeoResultBlock* dst)
{
	CheatPair pair;
	pair.addr = pair.data = 0;
	char* ptr = (char*)dst->romBuffer;

	FOLLOW("OP_GGC");

	if(opRef.getArgs().empty())
	{
		std::string s = "Unable to read DeluxeCheat file [ NULL ARGS ].";
		TRACE_PLUG_WARN(s.c_str());

		return true;
	}

	std::ifstream iStream;
	std::string code = "";

	for(int i = 0; i < opRef.getArgs().size(); i++)
	{
		const std::string& codeFile = opRef.getArgs()[i];

		if( ! codeFile.length())
			continue;//silent

		//MessageBox(NULL,codeFile.c_str(),"",MB_OK);
		FOLLOW(codeFile.c_str());
		iStream.open(codeFile.c_str());
		
		if(!iStream.good())
		{
			std::string s = "Unable to load cheat file [" + codeFile + " (bad stream)]!";
			TRACE_PLUG_ERROR(s.c_str());

			iStream.close();
			continue;
		}

		while( std::getline( iStream , code ) )
		{
			if(code.length())
			{
				size_t i = code.find("$Code(",0);
				std::string tok;
			
				if(i == std::string::npos)
					continue;

				i += 6;

				if( (i >= (code.length()-1)) || (code[i] == ')') )
					continue;

				tok = "";

				while(i < code.length())
				{
					if((code[i] == '\r') || (code[i] == '\n'))
						break;

					if((code[i] == ',') || (code[i] == ')') )
					{
						if(!tok.empty())
						{
							//std::string s = "["; s += tok; s += "]";

							if(!cheat_decode(tok.c_str(),&pair))
							{
								//MessageBox(NULL,s.c_str(),"tok",MB_OK);
								ptr[pair.addr] = ((pair.data&0xff00) >> 8);
								ptr[pair.addr+1] = pair.data & 0xFF;
							}
						}

						pair.addr = pair.data = 0;
						tok = "";
						++i;
						continue;
					}
					
					tok += (char)(code[i++]);
				}
			}
		}

		iStream.close();
		code = "";
	}

	iStream.close();
	return true;
}

bool OP_CRC_FUNC(COpCode& opRef,NeoResultBlock* dst)
{
	return true;
}

bool importIPSSafe(const std::string& ipsFile,NeoResultBlock *dst);

bool OP_IPS_FUNC(COpCode& opRef,NeoResultBlock* dst)
{
	FOLLOW("OP_IPS");

	if(opRef.getArgs().empty())
	{
		std::string s = "Unable to install IPS file [ NULL ARGS ].";
		TRACE_PLUG_WARN(s.c_str());

		return true;
	}

	for(int i = 0; i < opRef.getHashedArgs().size(); i++)
	{
		const std::string& s = opRef.getArgs()[i];

		if(!importIPSSafe(s.c_str(),dst))
		{
			FOLLOW("INST IPS");
			std::string ss = "Unable to install IPS file [ " + s + " ].";
			TRACE_PLUG_WARN(ss.c_str());
		}
	}

	return true;
}

//a safe ips importer
bool importIPSSafe(const std::string& ipsFile,NeoResultBlock *dst)
{
	FOLLOW("IMPORT IPS");

	FILE* f;
	unsigned int size = 0,len = 0,addr = 0, written = 0;
	unsigned char buf[6];
	char* p = (char*)dst->romBuffer;

	f = fopen(ipsFile.c_str(),"rb");

	if(!f)
		goto _exit_ips;

	fseek(f,0,SEEK_END);
	size = ftell(f);
	fseek(f,0,SEEK_SET);

	size -= (5 + 3);// "patch" + "eof" 

	if(! ((int)size) )
		goto _exit_ips;

	if(fread(buf,1,5,f) != 5)
		goto _exit_ips;

	if(memcmp((char*)buf,"PATCH",5))
	{
		if(memcmp((char*)buf,"patch",5))
			goto _exit_ips;
	}

	while(written < size)
	{
		fread(buf,1,3,f); written += 3;
        addr = (unsigned int)buf[2];
        addr += (unsigned int)buf[1] << 8;
        addr += (unsigned int)buf[0] << 16;

		fread(buf,1,2,f); written += 2;
        len = (unsigned int)buf[1];
        len +=(unsigned int)buf[0] << 8;

		if(!len)//rle
		{
			fread(buf,1,2,f); written += 2;
			len = (unsigned int)buf[1];
			len +=(unsigned int)buf[0] << 8;

			buf[0] = (unsigned char)fgetc(f); written += 1;

			while((len--) > 0)
			{
				if(addr < (unsigned int)dst->romBufferLength)
					p[addr++] = buf[0];
			}
		}
		else
		{
			written += len;

			while((len--) > 0)
			{
				if(addr < (unsigned int)dst->romBufferLength)
					p[addr++] = (unsigned char)fgetc(f);
			}
		}
	}

	fclose(f);
	return true;

	//used in case an error has appeared
_exit_ips:
	if(f)
		fclose(f);

	return false;
}

void setCaller(const CALLER caller)
{
	FOLLOW("SET CALLER");
	plugCaller = caller;
}

void calculateMD5(NeoResultBlock *dst,MD5Buf& result)
{
	FOLLOW("CALC MD5");
    MD5_CTX md5ctx;
    MD5Init(&md5ctx);
    MD5Update(&md5ctx,dst->romBuffer,dst->romBufferLength);
    MD5Final(&result,&md5ctx);
}

bool runCode(std::vector<COpCode>& opCodes,NeoResultBlock *dst)
{
	MD5Buf md5;
	calculateMD5(dst,md5);

	configureImage(dst,md5);

	for(int pc = 0; pc < opCodes.size(); pc++)
	{
		COpCode& op = opCodes[pc];

		if(! (*(OPCODE_FUNCS + op.getOpCode()))(op,dst) )
		{
			//ouch
				//return false;
		}
	}

	return true;
}

bool runCode(NeoResultBlock *dst)
{
	MD5Buf md5;
	calculateMD5(dst,md5);

	configureImage(dst,md5);

	return true;
}


