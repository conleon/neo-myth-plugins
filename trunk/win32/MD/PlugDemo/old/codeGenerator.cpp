#include "stdafx.h"
#include "codeGenerator.hpp"
#include "COpCode.hpp"

bool isSMS = false;
bool mustDecodeStream = false;
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

void disableEncodedStream()
{
	mustDecodeStream = false;
}

void forceSMSStream()
{
	isSMS = true;
}

void forceEncodedStream()
{
	mustDecodeStream = true;
}

bool isStreamEncoded()
{
	return mustDecodeStream;
}

bool isSMSStream()
{
	return isSMS;
}

void disableSMSStream()
{
	isSMS = false;
}

static int parseStream(const std::string& buf , const int addr , std::string& result)
{
	FOLLOW("FETCH STREAM");

	int pos = addr;

	if(buf[pos] != '\"')
	{
		TRACE_PLUG_ERROR("Unable to parse argument!. Missing start '\"' !");
		return -1;
	}
	else
		++pos;

	result = "";

	for(; (pos < buf.length()) && (buf[pos] != '\"'); pos++)
		result += buf[pos];

	if(buf[pos] != '\"')
	{
		TRACE_PLUG_ERROR("Unable to parse argument!. Missing end '\"' !");
		return -1;
	}
	else
		++pos;

	return (pos - addr);
}

static int parseStream(const std::string& buf , const int addr , int& result)
{
	FOLLOW("FETCH INT");

	bool neg = false;
	int pos = addr;
	int count = 0;
	int digit = 0;

	if(buf[pos] != '\"')
	{
		TRACE_PLUG_ERROR("Unable to parse argument!. Missing start '\"' !");
		return -1;
	}
	else
		++pos;

	while(isspace(buf[pos]) &&( pos < buf.length()) )
		++pos;

	if(buf[pos] == '-')
	{
		neg = true;
		++pos;
	}

	for(; (pos < buf.length()) && (buf[pos] != '\"'); pos++ , count++)
	{
		if(isspace(buf[pos]))
		{
			TRACE_PLUG_WARN("Warning : Error while parsing constant integer! Please don't put spaces!!!");
			continue;
		}

		if(count > 9)
		{
			TRACE_PLUG_WARN("Warning : Error while parsing constant integer! Constant too long! Truncating.");
			break;
		}

		digit = (int)buf[pos] - '0';

		if(digit < 0 || digit > 9)
		{
			TRACE_PLUG_WARN("Warning : Error while parsing constant integer! Character is not integer digit.");
			break;
		}

		if(!result)
			result = digit;
		else
			result *= 10 + digit;
	}

	if(neg)
		result *= -1;

	if(buf[pos] != '\"')
	{
		TRACE_PLUG_ERROR("Unable to parse argument!. Missing end '\"' !");
		return -1;
	}
	else
		++pos;

	return (pos - addr);
}

static void putBC(const int BC , std::vector<COpCode>& bc)
{
	bc.push_back( COpCode(BC) );
}

static void putBC(const int BC , const std::string& args, std::vector<COpCode>& bc)
{
	COpCode op(BC);

	op.addDelimBuffer(args);
	bc.push_back(op);
}

void loadDefaultBC(std::vector<COpCode>& bc)
{
	FOLLOW("LD DEF BC");

	bc.clear();

	putBC(OP_CPH,bc);
	putBC(OP_DOC,bc);
	putBC(OP_SAV,bc);
	putBC(OP_CRC,bc);
}

void genCode(NeoResultBlock *src , std::vector<COpCode>& bc)
{
	FOLLOW("GEN CODE");

	isSMS = false;

	if(!src->arguments)
	{
		loadDefaultBC(bc);
		return;
	}

	std::string safeBuf = std::string(src->arguments);
	std::string result = "";

	bool uType = false;
	bool gotOp = false;
	int op = OP_NOP;

	#define SET_BCs(__OP__) \
	{\
		while(isspace(safeBuf[i]) && (i < safeBuf.length())){i++;}\
		if(safeBuf[i] == '\"'){i++;}\
		\
		while((safeBuf[i] != '\"') && (i < safeBuf.length()))\
			{i++;}\
			\
		gotOp = true;\
		uType = false;\
		op = __OP__;\
	}

	//constant
	#define SET_BCc(__OP__) \
	{\
		while(isspace(safeBuf[i]) && (i < safeBuf.length())){i++;}\
		if(safeBuf[i] == '\"'){i++;}\
		\
		while((safeBuf[i] != '\"') && (i < safeBuf.length()))\
			{i++;}\
			\
		gotOp = true;\
		uType = true;\
		op = __OP__;\
	}

	FOLLOW("COMPILATION OF SYMBOLS");

	//fixes for sms support. Only IPS patches supported!
	bool smsFix = false;
	bool IPS = false;
	bool GGCheat = false;

	for(int i = 0; i < safeBuf.length(); i++)
	{
		if(safeBuf[i] == '-')
		{
			++i;

			//space should never exist , but you never know!
			while(isspace(safeBuf[i]) && (i < safeBuf.length()))
				i++;

			switch(safeBuf[i])
			{
				case 'q'://special command
				{
					while((safeBuf[i] != '\"') && (i < safeBuf.length()))
						i++;

					++i;//skip "

					if((safeBuf[i] - '0') == 1) //forceSilentMode((bool)(safeBuf[i] - '0')); <skip warnings
						forceSilentMode(true);
					else
						forceSilentMode(false);

					++i;//skip last "
					break;
				}

				case 'r':
					SET_BCs(OP_CNT);
				break;

				case 'd':
					SET_BCs(OP_IOD);
				break;

				case 'f':
					SET_BCc(OP_CFX);
				break;

				case 'c':
					SET_BCc(OP_CRC);
				break;

				case 'h':
					SET_BCc(OP_CPH);
				break;

				case 'g':
				{
					MessageBox(NULL,"YES","YES",MB_OK);
					GGCheat = true;//needed for sms fix!!
					SET_BCs(OP_GGC);
					break;
				}

				case 'p':
				{
					IPS = true;//needed for sms fix!!
					SET_BCs(OP_IPS);
					break;
				}

				case 's': //very special command ... not byte code!!
					{
					//	FOLLOW("S");
						++i;
						while((safeBuf[i] != '\"') && (i < safeBuf.length()))
							i++;

						std::string result = "";
						int fetch = parseStream(safeBuf,i,result);
						strToLower(result);
						romSuffix ="";

						if(fetch != -1)
						{
							romSuffix = result;
							if(result == "sms")
							{
								isSMS = true;//;quick hack.Will be verified in Virtualmachine.cpp
								smsFix = true;
							}
							else if(result == "smd")//DECODE IT!!(Will be checked anyway)
							{
								FOLLOW("MUST DEC ");
								forceEncodedStream();
							}

							i += fetch ;
						}
						break;
					}
				//end of switch
			}

			//add bc???
			if(gotOp)
			{
				if(uType) //constant 1 - 9
				{
					int result = 0;
					int fetch = parseStream(safeBuf,i,result);

					if(fetch != -1)
					{
						i += fetch;
						
						if(result)
						{
							COpCode opCode(op);
							//op.addNumerical(result); //off for now
							bc.push_back(opCode);
						}
					}
					else
					{
						TRACE_PLUG_WARN("Code generation failed. Loading default byte code");
					//	loadDefaultBC(bc);
					//	return;
					}
					//end of utype
				}
				else //string list
				{
					std::string result = "";

					int fetch = parseStream(safeBuf,i,result);

					if(fetch != -1)
					{
						i += fetch ;

						if(op == OP_GGC)
						MessageBox(NULL,result.c_str(),"",MB_OK);

						if(result.length())
						{
							if(op == OP_GGC)
								MessageBox(NULL,"OK","",MB_OK);

							COpCode opCode(op);
							FOLLOW(result.c_str());
							opCode.addDelimBuffer(result);
							opCode.setOpCode(op);

							if(op == OP_GGC)
							{
								for(int j = 0; j < opCode.getArgs().size();j++)
								{
									MessageBox(NULL,opCode.getArgs().at(j).c_str(),"",MB_OK);
								}
							}

							bc.push_back(opCode);
						}else
							TRACE_PLUG_WARN("Warning! String argument list specified but the length of string is zero.Skipping...");
					}
					else
					{
						TRACE_PLUG_WARN("Code generation failed. Loading default byte code");
						//loadDefaultBC(bc);
					//	return;
					}

					//end of !utype
				}

				uType = false;
				gotOp = false;
				op = OP_NOP;
				//end of gotOp
			}
			//end of loop
		}
	}

	std::vector<COpCode> newBc;
	newBc.clear();

	bool putCRC = false;

	for(int n = 0; i < bc.size(); i++)
	{
		if(bc[n].getOpCode() == OP_GGC)
			MessageBox(NULL,"GGC!","",MB_OK);

		if((bc[n].getOpCode() == OP_CRC))
			putCRC = true;
		else
			newBc.push_back( bc[i] );
	}

	newBc.push_back(OP_DOC);
	newBc.push_back(OP_SAV);

	if(putCRC )
		newBc.push_back( COpCode(OP_CRC) );

	bc.clear();
	bc = newBc;
}

const unsigned int buildHash(const std::string& in)
{
   unsigned int hash = 1315423911;

   for(int i = 0; i < in.length(); i++)
      hash ^= ((hash << 5) + in[i] + (hash >> 2));

   return hash;
}

#undef SET_BCc
#undef SET_BCs


