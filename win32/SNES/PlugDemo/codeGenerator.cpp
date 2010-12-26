#include "stdafx.h"
#include "codeGenerator.hpp"
#include "COpCode.hpp"

static std::string  modPath = "";
static std::string romSuffix = "";

const std::string& getRomSuffix()
{
	return romSuffix;
}

const std::string& getPlugPath()
{
	if(modPath.empty())
		modPath = getModulePath();

	return modPath;
}

inline void putBC(const int BC , std::vector<COpCode>& bc)
{
	bc.push_back( COpCode(BC) );
}

/*inline void putBC(const int BC , const int arg , std::vector<COpCode>& bc)
{

}*/


inline void putBC(const int BC , const std::string& args, std::vector<COpCode>& bc)
{
	COpCode op(BC);

	op.addDelimBuffer(args);
	bc.push_back(op);
}

static bool readCommand(const std::string& src,const std::string& command,std::string& result)
{
	int offs;

	result = "";
	offs = src.find(command.c_str());

	if(offs == std::string::npos)
		return false;
	
	offs += command.length();

	while(offs < src.length())
	{
		if(!isspace(src[offs]))
			break;

		++offs;
	}

	if(src[offs] != '"')
		return false;

	++offs;

	while(offs < src.length())
	{
		if(src[offs] == '"')
			break;

		result += src[offs++];
	}

	return (!(result.empty()));
}

static bool readCommand(const std::string& src,const std::string& command,int& result)
{
	int offs;

	result = 0;
	offs = src.find(command.c_str());

	if(offs == std::string::npos)
		return false;
	
	offs += command.length();

	while(offs < src.length())
	{
		if(!isspace(src[offs]))
			break;

		++offs;
	}

	if(src[offs] != '"')
		return false;

	++offs;

	while(offs < src.length())
	{
		if(src[offs] == '"')
			break;

		result = (result == 0) ? ((int)(src[offs++] - '0')) : (result * 10 + (int)(src[offs++] - '0'));
	}

	return (result > 0);
}

void muteOpCode(const int op,std::vector<COpCode>& bc)
{
	FOLLOW("MUTE OP");

	for(int i = 0; i < bc.size(); i++)
	{
		if(bc[i].getOpCode() == op)
		{
			bc[i].clear();
			bc[i].setOpCode(OP_NOP);
		}
	}
}

void genCode(NEOSNES::NeoResultBlock *src , std::vector<COpCode>& bc)
{
	FOLLOW("GEN CODE");
	const std::string code = std::string(src->arguments);
	int a = 0;
	bool gotIPS = false;
	bool gotRegion = false;
	std::string pa = "";

	bc.clear();

	if(readCommand(code,"-r",a))
	{
		if((a == 1) && (!gotRegion))
		{
			gotRegion = true;
			putBC(OP_PALFIX,bc);
		}
		else if((a == 2) && (!gotRegion))
		{
			gotRegion = true;
			putBC(OP_NTSCFIX,bc);
		}
	}

	gotIPS = readCommand(code,"-p",pa);

	if(gotIPS)
		putBC(OP_IPS,pa,bc);

	if(readCommand(code,"-g",pa))
		putBC(OP_GGC,pa,bc);

	//if(readCommand(code,"-s",pa)) //suffix , we don't need it
		//suffix = pa;

	//if(readCommand(code,"-f",a))
	//	putBC(OP_CFX,bc);

	if(readCommand(code,"-q",a))
		forceSilentMode((bool)(a != 0));

	//crc always last
	if(!gotIPS)
	{
		if(readCommand(code,"-c",a))
			putBC(OP_CRC,bc);

		return;
	}

	//adjust for ips
	muteOpCode(OP_CRC,bc);
}

const unsigned int buildHash(const std::string& in)
{
   unsigned int hash = 1315423911;

   for(int i = 0; i < in.length(); i++)
      hash ^= ((hash << 5) + in[i] + (hash >> 2));

   return hash;
}
