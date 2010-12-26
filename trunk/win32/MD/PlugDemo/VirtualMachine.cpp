#include "stdafx.h"
#include "plugCommon.hpp"
#include "VirtualMachine.hpp"
#include "codeGenerator.hpp"
#include "tracePlug.hpp"
#include "MyPlug.h"
#include "shared//madmonkey//md5int.h"
#include "shared//madmonkey//autopatch_db.h"
#include "temp_code//utility.h"
#include "temp_code//cheat.h"
#include "EEPROM.hpp"

typedef enum TSMDType
{
	SMD_TYPE_UNKNOWN = 0,
	SMD_TYPE_LSB,
	SMD_TYPE_MSB
};

static unsigned char SMDCHUNK[8192 * 2];
static CALLER plugCaller = CALLER_NEOMDMYTH;

bool OP_CNT_FUNC(COpCode& opRef,NeoResultBlock* dst,RomInfo* rom);
bool OP_NOP_FUNC(COpCode& opRef,NeoResultBlock* dst,RomInfo* rom);
bool OP_IOD_FUNC(COpCode& opRef,NeoResultBlock* dst,RomInfo* rom);
bool OP_GGC_FUNC(COpCode& opRef,NeoResultBlock* dst,RomInfo* rom);
bool OP_CRC_FUNC(COpCode& opRef,NeoResultBlock* dst,RomInfo* rom);
bool OP_CPH_FUNC(COpCode& opRef,NeoResultBlock* dst,RomInfo* rom);
bool OP_SAV_FUNC(COpCode& opRef,NeoResultBlock* dst,RomInfo* rom);
bool OP_IPS_FUNC(COpCode& opRef,NeoResultBlock* dst,RomInfo* rom);
bool OP_DOC_FUNC(COpCode& opRef,NeoResultBlock* dst,RomInfo* rom);
bool OP_CFX_FUNC(COpCode& opRef,NeoResultBlock* dst,RomInfo* rom);

static bool(*OPCODE_FUNCS[OPCODES_SIZE])(COpCode& , NeoResultBlock*,RomInfo*) =
{ 
	OP_NOP_FUNC , OP_CNT_FUNC , OP_IOD_FUNC ,
	OP_GGC_FUNC , OP_CRC_FUNC , OP_CPH_FUNC , 
	OP_SAV_FUNC , OP_IPS_FUNC , OP_DOC_FUNC ,
	OP_CFX_FUNC
};

bool OP_NOP_FUNC(COpCode& opRef,NeoResultBlock* dst,RomInfo* rom)
{
	FOLLOW("OP_NOP");
	return true;
}

bool OP_GGC_FUNC(COpCode& opRef,NeoResultBlock* dst,RomInfo* rom)
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

bool OP_CFX_FUNC(COpCode& opRef,NeoResultBlock* dst,RomInfo* rom)
{
	FOLLOW("OP_CFX");
	FOLLOW(rom->copyright);
	patchRomCopyright(rom,"(C)SEGA 1994.JUN");
	FOLLOW(rom->copyright);
	/*return (*/applyChecksumPatchAutomatically(rom);// == RST_SUCCESS);
	return true;
}

bool OP_CRC_FUNC(COpCode& opRef,NeoResultBlock* dst,RomInfo* rom)
{
	FOLLOW("OP_CRC");
	FOLLOW2("CRC_OLD = ",(int)rom->checksum);
	/*return (*/applyChecksumPatchAutomatically(rom);// == RST_SUCCESS);
	FOLLOW2("CRC_NEW = ",(int)rom->checksum);
	return true;
}

bool OP_CPH_FUNC(COpCode& opRef,NeoResultBlock* dst,RomInfo* rom)
{
	FOLLOW("OP_CPH");
	FOLLOW(rom->copyright);
	patchRomCopyright(rom,"(C)SEGA 1994.JUN");

	return true;
}

bool OP_SAV_FUNC(COpCode& opRef,NeoResultBlock* dst,RomInfo* rom)
{
	FOLLOW("OP_SAV");

	dst->saveAllocationSize = 0;

	bool isSRAM = false;

	{
		if( (*(rom->romData + 0x1b1) == 'R') && (*(rom->romData + 0x1b0) == 'A'))
			isSRAM = true;
		else if( (*(rom->romData + 0x1b0) == 'R') && (*(rom->romData + 0x1b1) == 'A')) //swapped? ==should never happen
			isSRAM = true;
		else
			isSRAM = false;
	}

	if(isSRAM)
	{
		//Patch incorrect offsets (based on genplus)
		if ((rom->saveStartAddress> rom->saveEndAddress) || ((rom->saveEndAddress - rom->saveStartAddress) >= 0x10000))
		{
			rom->saveEndAddress = rom->saveStartAddress + 0xffff;
 			rom->saveStartAddress &= 0xfffffffe;
			rom->saveEndAddress |= 1;

			{
				char b[6];

				intToB(rom->saveStartAddress,b);
				patchRomAddress(rom,b,0x1b4,4);

				intToB(rom->saveEndAddress,b);
				patchRomAddress(rom,b,0x1b8,4);
			}
		}
	}

	FOLLOW( std::string( std::string("SRAM B1 = ") += ( *(rom->romData + 0x1b1) )).c_str() );
	FOLLOW( std::string( std::string("SRAM B0 = ") += ( *(rom->romData + 0x1b0) )).c_str() );
	FOLLOW2("isSRAM = ",(int)isSRAM);

	const int sd = abs(((int)rom->saveEndAddress - (int)rom->saveStartAddress));

	if(sd == 1)
	{
		dst->saveAllocationSize = -1;
		return true;
	}
	else if((!sd) )
	{
		dst->saveAllocationSize = 0;

		//pass #1 try our internal mini eeprom db
		for(int i = 0; ; i++)
		{
			if(EEPROM_MAPPERS[i] == NULL)
				break;

			const char* ep = (const char*)dst->romBuffer;
			ep = (const char*)(ep + 0x180); /*crazy MSVC++ compiler!!!*/

			if(memcmp(ep,EEPROM_MAPPERS[i],strlen(EEPROM_MAPPERS[i])) == 0)
			{
				//TRACE_PLUG_INFO("Here");
				dst->saveAllocationSize = -1;
				FOLLOW2("SAVE = ",dst->saveAllocationSize);
				return true;
			}
		}

		//pass #2 eeprom.inc
		const std::string incF = getPlugPath() + std::string("eeprom.inc");

		MessageBox(NULL,incF.c_str(),"",MB_OK);
		if(!fileExists(incF,true))
			return true;

		std::ifstream iStream;
		std::string code = "";
		iStream.open(incF.c_str());
		int addr;

		while( std::getline( iStream , code ) )
		{	
			addr = 0;

			//head ws
			while( (addr < code.length()) && (isspace(code[addr])))
				++addr;

			//comment?
			if( (code[addr] == '/')  && (code[addr + 1] == '/') )
				continue;

			if(addr != 0)
				code = code.substr(addr,code.length());

			//trailing ws?
			addr = code.length()-1;

			while(addr)
			{
				if(!isspace(code[addr]))
					break;

				--addr;
			}

			if(addr < 2)
				continue;

			if(addr != (code.length()-1))
				code = code.substr(0,addr);

			const char* ep = (const char*)dst->romBuffer;
			ep = (const char*)(ep + 0x180); /*crazy MSVC++ compiler!!!*/

			if(memcmp(ep,code.c_str(),code.length()) == 0)
			{
				//TRACE_PLUG_INFO("Here2");
				iStream.close();
				dst->saveAllocationSize = -1;
				FOLLOW2("((eeprom.inc))SAVE = ",dst->saveAllocationSize);
				return true;
			}
		}

		iStream.close();
		return true;
	}

	dst->saveAllocationSize = ( ((sd+2) / 1024) / 8 );

	FOLLOW2("SAVE = ",dst->saveAllocationSize);
	return true;
}

bool importIPSSafe(const std::string& ipsFile,NeoResultBlock *dst);

bool OP_IPS_FUNC(COpCode& opRef,NeoResultBlock* dst,RomInfo* rom)
{
	FOLLOW("OP_IPS");

	if(opRef.getArgs().empty())
	{
		std::string s = "Unable to install IPS file [ NULL ARGS ].";
		TRACE_PLUG_WARN(s.c_str());

		return true;
	}

	std::string srm = "";

	const int sd = abs((int)rom->saveEndAddress - (int)rom->saveStartAddress);
	const bool checkSrm = (bool)(sd>1);


	for(int i = 0; i < opRef.getHashedArgs().size(); i++)
	{
		const std::string& s = opRef.getArgs()[i];

		if(checkSrm)
		{
			srm = getFileExtension(s);
			strToLower(srm);

			FOLLOW(std::string( std::string("CHK SRM SRAM ==> ") + srm ).c_str());
			FOLLOW(srm.c_str());
			if(srm == "srm" || srm == "sav")
			{
				FOLLOW("INST SRM SRAM");
				if(importRawPatchFile(rom,rom->saveStartAddress,sd,sd,s.c_str()) != RST_SUCCESS)
				{
					if(importRawPatchFile(rom,rom->saveStartAddress,sd + 2,sd,s.c_str()) != RST_SUCCESS)
					{
						std::string ss = "Unable to install SRM file [ " + s + " ].";
						TRACE_PLUG_WARN(ss.c_str());
					}
				}
				continue;
			}
		}

		if(!importIPSSafe(s.c_str(),dst))
		{
			FOLLOW("INST IPS");
			std::string ss = "Unable to install IPS file [ " + s + " ].";
			TRACE_PLUG_WARN(ss.c_str());
		}
	}

	return true;
}

bool OP_DOC_FUNC(COpCode& opRef,NeoResultBlock* dst,RomInfo* rom)
{
	FOLLOW("OP_DOC");

	const unsigned int& sBeg = rom->saveStartAddress;
	const unsigned int& sEnd = rom->saveEndAddress;
	int sDelta = ((int)sEnd - (int)sBeg);

	FOLLOW2("sBeg = ",sBeg);
	FOLLOW2("sEnd = ",sEnd);
	FOLLOW2("sDelta = ",sDelta);

	bool patchSV = false;

	if(sDelta > ROM_SAVE_OVERFLOW )
	{
		/*FILE* f = fopen("kdump.bin","wb");
		fwrite((char*)dst->romBuffer,1,dst->romBufferLength,f);
		fclose(f);
		TRACE_PLUG_ERROR("Critical error! Rom save overflow detected.This happens when a rom is decoded wrong or not supported/bad dump.Aborting...");
		return false;*/

		rom->saveEndAddress = (rom->saveStartAddress + 0xFFFF)|1;
		rom->saveStartAddress &= 0xFFFFFFFE;

		patchSV = true;
	}

	if(sBeg > sEnd)
	{
		//if(TRACE_PLUG_CONFIRM("The header of the rom contains bad save data. I will try to fix this.If the rom doesn't work try another dump.Continue????") == TRACE_PLUG_RESULT_POSITIVE)
		{
			rom->saveEndAddress = (rom->saveStartAddress + 0xFFFF)|1;
			rom->saveStartAddress &= 0xFFFFFFFE;

			patchSV = true;
		}
		//else
		//	TRACE_PLUG_INFO("Okay , skipping!");
	}

	if(patchSV)
	{
			//TRACE_PLUG_INFO(std::string(std::string("Fixing memory overlap for rom") + std::string(rom->domesticName)).c_str());	
			char b[5];

			intToB(rom->saveStartAddress,b);
			patchRomAddress(rom,b,0x1b4,4);

			intToB(rom->saveEndAddress,b);
			patchRomAddress(rom,b,0x1b8,4);
	}

	/*
	const char* pN = rom->overseasName;
	bool needsNameFix = true;

	for(;*pN;pN++)
	{
		if(*pN != ' ' || *pN != '\0' )
		{
			needsNameFix = false;
			break;
		}
	}

	if(needsNameFix)
	{
		TRACE_PLUG_INFO(std::string(std::string("Fixing name information for \n rom = ") + std::string(rom->overseasName)).c_str()) ;

		patchRomOverseasName(rom,"SEGA\0");
		patchRomDomesticName(rom,"SEGA\0");
	}
	*/
	return true;
}

bool OP_IOD_FUNC(COpCode& opRef,NeoResultBlock* dst,RomInfo* rom)
{
	FOLLOW("OP_IOD");

	if(opRef.getHashedArgs().empty())
		return true;

	if((opRef.getArgs()[0] == std::string("UNKNOWN")) || ( opRef.getArgs()[0] == std::string("UNKNOW") ) )
		return true;

	setBitFieldRange(rom->IODeviceBits,IODEVICE_COUNT,BIT_OFF);

	int put = 0;

	for(int i = 0; i < opRef.getHashedArgs().size(); i++)
	{
		const unsigned int& codeHash = opRef.getHashedArgs()[i];

		switch(codeHash)
		{
			default:
				return true;

			case JOYPAD_HASH:
				setBitField(rom->IODeviceBits,JOYPAD,BIT_ON);
				++put;
			break;

			case JOYPAD_6BTN_HASH:
				setBitField(rom->IODeviceBits,JOYPAD_6BTN,BIT_ON);
				++put;
			break;

			case KEYBOARD_HASH:
				setBitField(rom->IODeviceBits,KEYBOARD,BIT_ON);
				++put;
			break;

			case PRINTER_HASH:
				setBitField(rom->IODeviceBits,PRINTER,BIT_ON);
				++put;
			break;

			case CONTROL_BALL_HASH:
				setBitField(rom->IODeviceBits,CONTROL_BALL,BIT_ON);
				++put;
			break;

			case FLOPPY_DISK_HASH:
				setBitField(rom->IODeviceBits,FLOPPY_DISK,BIT_ON);
				++put;
			break;

			case ACTIVATOR_HASH:
				setBitField(rom->IODeviceBits,ACTIVATOR,BIT_ON);
				++put;
			break;

			case TEAMPLAY_HASH:
				setBitField(rom->IODeviceBits,TEAMPLAY,BIT_ON);
				++put;
			break;

			case JOYSTIC_MS_HASH:
				setBitField(rom->IODeviceBits,JOYSTIC_MS,BIT_ON);
				++put;
			break;

			case RS232C_HASH:
				setBitField(rom->IODeviceBits,RS232C,BIT_ON);
				++put;
			break;

			case TABLET_HASH:
				setBitField(rom->IODeviceBits,TABLET,BIT_ON);
				++put;
			break;

			case PADDLE_CONTROLLER_HASH:
				setBitField(rom->IODeviceBits,PADDLE_CONTROLLER,BIT_ON);
				++put;
			break;

			case CDROM_HASH:
				setBitField(rom->IODeviceBits,CDROM,BIT_ON);
				++put;
			break;

			case MEGA_MOUSE_HASH:
				setBitField(rom->IODeviceBits,MEGA_MOUSE,BIT_ON);
				++put;
			break;
		}
	}

	if(put > 4)
		TRACE_PLUG_WARN("Warning : Over 4 IO device bits detected. The game might not boot on some genesis models.");

	patchRomIODeviceData(rom,rom->IODeviceBits);
	return true;
}

bool OP_CNT_FUNC(COpCode& opRef,NeoResultBlock* dst,RomInfo* rom)
{
	FOLLOW("OP_CNT");

	if(opRef.getHashedArgs().empty())
		return true;

	setBitFieldRange(rom->countryBits,REGION_COUNT,BIT_OFF);

	int put = 0;

	for(int i = 0; i < opRef.getHashedArgs().size(); i++)
	{
		const unsigned int& codeHash = opRef.getHashedArgs()[i];

		switch(codeHash)
		{
			default:
				return true;

			case EUROPE_HASH:
				setBitField(rom->countryBits,EUROPE,BIT_ON);
				++put;
			break;
					
			case FRANCE_HASH:
				setBitField(rom->countryBits,FRANCE,BIT_ON);
				++put;
			break;

			case BRAZIL_HASH:
				setBitField(rom->countryBits,BRAZIL,BIT_ON);
				++put;
			break;

			case HONGKONG_HASH:
				setBitField(rom->countryBits,HONGKONG,BIT_ON);
				++put;
			break;

			case ASIA_HASH:
				setBitField(rom->countryBits,ASIA,BIT_ON);
				++put;
			break;

			case JAPAN_HASH:
				setBitField(rom->countryBits,JAPAN,BIT_ON);
				++put;
			break;

			case USA_HASH:
				setBitField(rom->countryBits,USA,BIT_ON);
				++put;
			break;
		}
	}
	
	if(put > 3)
	{
		TRACE_PLUG_WARN("Warning : Over 3 Country bits detected. The game might not boot on some genesis models.Fixing...");

		setBitField(rom->countryBits,JAPAN,BIT_ON);
		setBitField(rom->countryBits,USA,BIT_ON);
		setBitField(rom->countryBits,EUROPE,BIT_ON);
	}

	patchRomCountryList(rom,rom->countryBits);

	return true;
}

void fixSMSBytecode(std::vector<COpCode>& bc)
{
	std::vector<COpCode> newBc;
	newBc.clear();

	for(int i = 0; i < bc.size(); i++)
	{
		if((bc[i].getOpCode() == OP_IPS) || (bc[i].getOpCode() == OP_GGC) )
			newBc.push_back( bc[i] );
	}

	bc.clear();
	bc = newBc;
	newBc.clear();
}


//Warning : Cycle must be completed otherwise this will fail
//This detects 32x/md . The bios & sms & everything else are done
//before entering the cycle.

int getRomTypeFromPtr(RomInfo* rom)
{
	char buf[0x10 + 1];
	//memcpy(buf,rom->romData + 0x100,0x10);

	int x;
	//copy + lcase 2in1
	for(x = 0; x < 0x10; x++)
	{
		//if(!*(rom->romData + (0x100 + x)))
		//	break;

		*(buf+x) = *(rom->romData + (0x100 + x));
		*(buf+x) = tolower(*(buf+x));
	}

	buf[x+1] = '\0';

	FOLLOW("ROM SCAN FROM PTR");
	FOLLOW(buf);


	//if(*(rom->romData + 0x203))
	{
		if(*(rom->romData + 0x203) == 0x88)
			return 2;
	}
	
	if(strstr(buf,"mega"))
		return 1;
	else if(strstr(buf,"drive"))
		return 1;
	else if(strstr(buf,"genesis"))
		return 1;
	else if(strstr(buf,"32x"))
		return 2;
	else if(strstr(buf,"mars"))
		return 2;
	else if(strstr(buf,"sega"))/*suggested by madmonkey*/
	{
	//	if(*(rom->romData + 0x203))/*ChillyWilly*/
		{
			if(*(rom->romData + 0x203) == 0x88)
				return 2;

			return 1;
		}
	}

	return -1;
}

bool scanRomType(NeoResultBlock* dst,RomInfo* rom)
{
	FOLLOW("BEG RT SCAN");

	updateRomInfo(rom);

	dst->romFormat = NEOMD_FMT_MD32X_1_32_NOS;

	const int type = getRomTypeFromPtr(rom);

	//TRACE_PLUG_INFO(itoS<int>(dst->saveAllocationSize,std::hex).c_str());
	if(type == -1)
		return false;

	/*
		The kind in flash that runs from flash. This can be up to 1MB with 1MB of static ram. It will have a rom/ram address range of:
		rom = 0x00000000 - 0x000FFFFF, ram = 0x00300000 - 0x003FFFFF

		The kind that can be in either flash or the SD card, and runs from ram. It will have a rom/ram range of:
		rom = 0x00300000 - 0x003FFFFF, ram = 0x00300000 - 0x00300000

		MD up to 16Mbit with save ram: 1
		MD up to 32Mbit with save ram: 2 (use patching)

		32X up to 32Mbit with save ram: 3 (use patching if > 16Mbit)

		MD or 32X game > 32Mbit: 4

		MD or 32X any size with EEPROM: 5 OK

		MD or 32X up to 32Mbit with no save ram: 6 OK

		SCD BIOS: 8
		SCD RAM: 9
		SCD BIOS+RAM: 10

		SMS: 0x13
	*/

	//test if 40M ( header data are usually invalid)
	int sizeMbit = ( (dst->romBufferLength+1) / 131072 );

	if(sizeMbit >= 40)
	{
		dst->romFormat = NEOMD_FMT_MD32X_32_40;
		return true;
	}

	if((rom->romStartAddress == 0x00000000) && (rom->romEndAddress == 0x000FFFFF))
	{
		if((rom->ramStartAddress == 0x00300000) && (rom->ramEndAddress == 0x003FFFFF))
		{
			dst->romFormat = NEOMD_FMT_MENU_EXT_FLASH;
			return true;
		}
	}
	else if((rom->romStartAddress == 0x00300000) && (rom->romEndAddress == 0x003FFFFF))
	{
		if((rom->ramStartAddress == 0x00300000) && (rom->ramEndAddress == 0x00300000))
		{
			dst->romFormat = NEOMD_FMT_MENU_EXT_SD;
			return true;
		}
	}

	//sizeMbit = ((rom->romEndAddress - rom->romStartAddress)+1) / 131072;

	/*BIOS + SMS HANDLED ALREADY*/
	if((!dst->saveAllocationSize))
	{
		if(dst->saveAllocationSize == -1)
		{
			dst->romFormat = NEOMD_FMT_MD32X_EEPROM;
			return true;
		}
		else if((sizeMbit  <= 32) && (dst->saveAllocationSize == 0) )
		{
			dst->romFormat = NEOMD_FMT_MD32X_1_32_NOS;
			return true;
		}
	}
	
	//with save
	if(type == 1)
	{
		if(sizeMbit <= 16)
		{
			dst->romFormat = NEOMD_FMT_MD_1_16_S;
			return true;
		}
		else if( (sizeMbit > 16) && (sizeMbit <= 32))
		{
			dst->romFormat = NEOMD_FMT_MD_1_32_S;
			return true;
		}
		return true;
	}

	if(type == 2)
	{
		if(sizeMbit <= 32)
		{
			dst->romFormat = NEOMD_FMT_32X_1_32_S;
			return true;
		}
	}

	FOLLOW("END RT SCAN");
	return false;
}

bool isSCDSramCart(const char* code , const int start , const int len)
{
	for(int i = start; i < len; i += 2)
	{
		if( *(code+i) != 0xFF && *(code+(i+1)) != 0x04 )
			return false;
	}

	return true;
}

//muted
bool isStaticLockonRom(RomInfo* rom,NeoResultBlock* dst)
{
	FOLLOW("LOCKON ?");

	unsigned char* code = rom->romData;
	unsigned char* saveCode = code;
	const int saveAddr = dst->romBufferLength;

	/*Move in stream*/
	int mov = (rom->romEndAddress - rom->romStartAddress);

	FOLLOW2("L = ",rom->romLength);
	FOLLOW2("K = ",mov);
	FOLLOW2("KnH = ",mov - 0xFF);

	if( (mov + 0xFF) < dst->romBufferLength - 0xFF)
	{
		FOLLOW2("ROM ADDR OVERLAP ",mov);

		/*Skip pad bytes*/
		while(mov < rom->romLength)
		{
			if( *(code + mov) == 0xFF )
				mov++;
			else
				break;
		}

		if(mov + 0x100 >= rom->romLength)
			return false;

		FOLLOW2("SHRD JMP =. ",mov);

		rom->romData = (code + mov);

		/*Let the opengenie framework to fetch the data*/
		updateRomInfo(rom);

		if(getRomTypeFromPtr(rom) == -1)
		{
			/*rollback*/
			rom->romData = saveCode;
			rom->romLength = saveAddr;

			/*fetch new stats -CRITICAL if rom is not LOCKON-*/
			updateRomInfo(rom);

			return false;
		}

		/*now we have the decoded data from the shared pointer*/
		rom->romLength = ((rom->romEndAddress - rom->romStartAddress));//+0x100);

		FOLLOW("LOCKON : MOVED POINTER");

		/*not recursive for now (not needed)*/

		/*Run patch-cycle*/
		return true;
	}

	return false;
}

/*Must be called at the end of the patch cycle!*/
bool patchSRAM68K(RomInfo* rom,NeoResultBlock* dst,const int sizeMbit)
{
	/*
		ChillyWilly's  >16mbit rom sram patching suggestion
		http://www.neoflash.com/forum/index.php/topic,5857.msg42206.html#msg42206
	*/

	if(isSMSStream())
		return false;

	if(sizeMbit <= 16)
		return false;

	/*dont bother with NO SAVE or EEPROM*/
	if(dst->saveAllocationSize <= 0)
		return false;


	unsigned char* base = 0;

	for(int addr = 0; addr < rom->romLength; addr += 2)
	{
		base = (rom->romData + addr);

		if(extractShort(base) != 0x00A1)
			continue;

		if( extractShort(base + 2) == 0x30F1)
			shortToB(0x3001, (char*)( rom->romData + (addr + 2) ));	
		
	}

	/*force-fix crc*/
	OP_CRC_FUNC(COpCode(),dst,rom);

	updateRomInfo(rom);

	return true;
}

void addSDMenuSupportCode(RomInfo* rom,NeoResultBlock *dst)
{
	FOLLOW("SD SUPPORT");

	if(isSMSStream())
		return;

	if( dst->romFormat == -1)
		return;

	if(plugCaller != CALLER_SUPERMD_FRAMEWORK)
		return;

	std::string finalCode;
	finalCode = "MYTH";

	if(dst->romFormat < 10)
		finalCode += '0';

	finalCode += itoS<int>(dst->romFormat,std::dec);

	if(dst->saveAllocationSize)
	{
		switch(dst->saveAllocationSize)
		{
			case 8:
				finalCode += "01";
			break;

			case 16:
				finalCode += "02";
			break;

			case 32:
				finalCode += "04";
			break;

			case 64:
				finalCode += "08";
			break;

			case 128:
				finalCode += "16";
			break;

			default:
				finalCode += "00";
			break;
		}

	}
	else
		finalCode += "00";

	finalCode += '\0';

	/*let the framework to patch & pad the bytes*/
	patchRomMemo(rom,finalCode.c_str());

	/*fetch data*/
	updateRomInfo(rom);
}

void addSDMenuSupportCode(NeoResultBlock *dst)
{
	FOLLOW("SDp SUPPORT");

	if(isSMSStream())
		return;

	if( dst->romFormat == -1)
		return;

	if(plugCaller != CALLER_SUPERMD_FRAMEWORK)
		return;


	RomInfo* rom = createRom();

	rom = resetRom(rom,RESET_FULL);

	mountRom(rom,(unsigned char*)dst->romBuffer,dst->romBufferLength,FMT_BIN);

	std::string finalCode = "MYTH";

	if(dst->romFormat < 10)
		finalCode += '0';

	finalCode += itoS<int>(dst->romFormat,std::dec);

	if(dst->saveAllocationSize)
	{
		
		switch(dst->saveAllocationSize)
		{
			case 8:
				finalCode += "01";
			break;

			case 16:
				finalCode += "02";
			break;

			case 32:
				finalCode += "04";
			break;

			case 64:
				finalCode += "08";
			break;

			case 128:
				finalCode += "16";
			break;

			default:
				finalCode += "00";
			break;
		}

	}
	else
		finalCode += "00";

	finalCode += '\0';

	/*let the framework to patch & pad the bytes*/
	patchRomMemo(rom,finalCode.c_str());

	/*fetch data*/
	updateRomInfo(rom);

	unMountRom(rom);

	rom = releaseRom(rom);
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

void decodeSMD(const TSMDType type,NeoResultBlock *dst)
{
	FOLLOW("DECODE SMD");
	unsigned int i = 0,j = 0,jump = 0;
	const char* p = (char*)(dst->romBuffer) + 0x200;
	char* out = (char*)(dst->romBuffer);
	
	if( (dst->romBufferLength <= 0x200) || (type == SMD_TYPE_UNKNOWN) )
		return;

	for(jump = 0; jump < dst->romBufferLength - 0x200;jump += 16384)
	{
		memcpy(SMDCHUNK,p + jump,16384);
		
		for(j = 0; j < 8192; j++)
		{
			switch(type)
			{
				case SMD_TYPE_LSB:
				{
					out[jump + ((j << 1) + 1) ] = SMDCHUNK[j];
					out[jump + ((j << 1) + 0) ] = SMDCHUNK[8192+j];
					break;
				}

				case SMD_TYPE_MSB:
				{
					out[jump + ((j << 1) + 0) ] = SMDCHUNK[j];
					out[jump + ((j << 1) + 1) ] = SMDCHUNK[8192+j];
					break;
				}
			}
		}
	}

	memset((char*)(out + (dst->romBufferLength-0x200)),0,0x200);
	dst->romBufferLength -= 0x200;//The programmer never really reads back this
}

const TSMDType getSMDType(NeoResultBlock *dst)
{
	FOLLOW("GET SMD TYPE");

	//0 = no smd
	//1 = least significant byte first , 2 = most significant first
	const char* p = ((const char*)(dst->romBuffer) + 0x200);
	const char* p2 = (const char*)(SMDCHUNK + 0x100);
	unsigned int i;

	//assume lsb...
	for(i = 0;i < 0x200; i++)
	{
		SMDCHUNK[(i << 1) + 1] = p[i];
		SMDCHUNK[(i << 1) + 0] = p[8192 + i];
	}

	if(!memcmp(p2,"SEGA",4))
		return SMD_TYPE_LSB;

	//if(!memcmp(p2,"ESAG",4))
		//return SMD_TYPE_MSB;

	for(i = 0;i < 0x200; i++)
	{
		SMDCHUNK[(i << 1) + 0] = p[i];
		SMDCHUNK[(i << 1) + 1] = p[8192 + i];
	}

	if(!memcmp(p2,"SEGA",4))
		return SMD_TYPE_MSB;

	return SMD_TYPE_UNKNOWN;
}

void calculateMD5(NeoResultBlock *dst,MD5Buf& result)
{
	FOLLOW("CALC MD5");
    MD5_CTX md5ctx;
    MD5Init(&md5ctx);
    MD5Update(&md5ctx,dst->romBuffer,dst->romBufferLength);
    MD5Final(&result,&md5ctx);
}

void applyNBAJamTE32xFix(RomInfo* rom)//Original fix provided by ChillyWilly
{
	FOLLOW("CHECK NBA JAM 32X");
	if( (memcmp(rom->romData + 0x180,"GM T-8104B",10) == 0) || (memcmp(rom->romData + 0x180,"GM T-81406",10) == 0) )
	{
		if(*(unsigned short*)(rom->romData + 0xeec) == 0xfe60)
			*(unsigned short*)(rom->romData + 0xeec) = 0x714e;//put a nop in BE format
	}
}

bool runCode(std::vector<COpCode>& opCodes,NeoResultBlock *dst)
{
	FOLLOW("RUN CODE");
	bool containsIdrHeader = false;
	bool definedIdr = isStreamEncoded();
	const char* safeCastBuffer = static_cast<char*>(dst->romBuffer);
	char sig[64];
	const int sizeMbit = ( (dst->romBufferLength+1) / 131072 );

	memset(sig,'\0',64);

	if( (dst->romBufferLength > (0x7FF0 + 8)))
	{
		{
			memcpy(sig,safeCastBuffer + 0x7FF0,8);
			sig[8] = '\0';

			FOLLOW(sig);
			if(!memcmp(sig, "TMR SEGA", 8))
			{
				fixSMSBytecode(opCodes);
				dst->romFormat = NEOMD_FMT_SMS;
				forceSMSStream();
				disableEncodedStream();
			}
			else
				disableSMSStream();
		}
	}
	
	{
		MD5Buf md5;

		calculateMD5(dst,md5);
		int a,b;

		//autopatch
		for(a = 0; a < auto_patch_table_size; a++)
		{
			if(memcmp(auto_patch_table[a].md5,md5,16 ) == 0)
			{
				char* addr = (char*)dst->romBuffer;

				if(auto_patch_table[a].type == DUMP_TYPE_SCDBIOS)
				{
					dst->romFormat = NEOMD_FMT_SCD_BIOS;

					if( sizeMbit == 16 )
					{
						if(isSCDSramCart(safeCastBuffer,131072,dst->romBufferLength))
						{
							FOLLOW("SCD SYSTEM CART");
							dst->romFormat = NEOMD_FMT_SCD_SYSTEM;
							dst->saveAllocationSize = 128;

							addSDMenuSupportCode(dst);
							return true;
						}
					}

				}

				bool atLeastOne = false;

				for(b = 0; b < PATCH_BUFFER_ENTRIES;b++)
				{
					if(auto_patch_table[a].patch[b].offset < 0)
						break;
					
					atLeastOne = true;
					addr[(auto_patch_table[a].patch[b].offset + 0)] = (auto_patch_table[a].patch[b].value >> 8);
					addr[(auto_patch_table[a].patch[b].offset + 1)] = auto_patch_table[a].patch[b].value & 0xff;
				}

				if(atLeastOne)
				{
					//disable critical patches
					muteOpCode(OP_CPH,opCodes);
					muteOpCode(OP_CRC,opCodes);
					muteOpCode(OP_CFX,opCodes);
					muteOpCode(OP_IOD,opCodes);
					muteOpCode(OP_CNT,opCodes);
					muteOpCode(OP_DOC,opCodes);
				}

				break;
			}
		}

		if(isSMSStream())
		{
			//sms save db
			for(a = 0; a < sms_save_table_size; a++)
			{
				if(memcmp(sms_save_table[a].md5,md5,16) == 0)
				{
					dst->saveAllocationSize = sms_save_table[a].unitsof8K;
					break;
				}
			}
		}
	}

	if(!isSMSStream())
	{
		const TSMDType smdt = getSMDType(dst);

		if(smdt != SMD_TYPE_UNKNOWN)
		{
			decodeSMD(smdt,dst);
			disableSMSStream();
			disableEncodedStream();
			definedIdr = false;
		}

		memcpy(sig,safeCastBuffer + 0x100,4);
		sig[4]='\0';

		if(!memcmp(sig,"SEGA",4))
		{
			disableSMSStream();
			disableEncodedStream();
		}
	}

	if( (!isSMSStream()) && (!definedIdr) && (dst->romFormat != NEOMD_FMT_SCD_BIOS) && (dst->romFormat != NEOMD_FMT_SCD_SYSTEM) )
	{
		if(dst->romBufferLength > (0x120 + 12))
		{
			memcpy(sig,safeCastBuffer + 0x120,12);
			sig[12] = '\0';
			FOLLOW(sig);

			if(strstr(sig, "CD2 BOOT ROM"))
				dst->romFormat = NEOMD_FMT_SCD_BIOS;
			else if(strstr(sig, "SEGA-CD BOOT"))
				dst->romFormat = NEOMD_FMT_SCD_BIOS;

			if(dst->romFormat == NEOMD_FMT_SCD_BIOS)
			{
				if( sizeMbit == 16 )
				{
					if(isSCDSramCart(safeCastBuffer,131072,dst->romBufferLength))
					{
						FOLLOW("SCD SYSTEM CART");
						dst->romFormat = NEOMD_FMT_SCD_SYSTEM;
						dst->saveAllocationSize = 128;

						addSDMenuSupportCode(dst);
						return true;
					}
				}
				
				return true;
			}
		}
	}

	if((sizeMbit == 16) && (!definedIdr))//check for sram cart
	{
		if(isSCDSramCart(safeCastBuffer,0,dst->romBufferLength))
		{
			FOLLOW("SCD SRAM CART");

			dst->romFormat = NEOMD_FMT_SCD_SRAM;
			dst->saveAllocationSize = 128;

			addSDMenuSupportCode(dst);

			return true;
		}
	}

	RomInfo* rom = createRom();

	resetRom(rom,RESET_FULL);

	if(mountRom(rom,static_cast<unsigned char*>(dst->romBuffer),dst->romBufferLength,FMT_BIN) != RST_SUCCESS)
	{
		TRACE_PLUG_ERROR("Unable to mount rom buffer!. Aborting");

		unMountRom(rom);
		releaseRom(rom);

		return false;
	}

	bool lockOn = false;

	if(!isSMSStream())
	{
		if(isSupportedRom(rom) != RST_SUCCESS)
		{
			if(definedIdr)
			{
				unMountRom(rom);
				releaseRom(rom);

				return false;
			}
			
			memcpy(sig,safeCastBuffer + 0x100,4);
			sig[4]='\0';

			if(memcmp(sig,"SEGA",4))
			{
				//maybe swapped?
				swap16(rom->romData,(int)rom->romLength);

				if(isSupportedRom(rom) != RST_SUCCESS)
				{
					unMountRom(rom);
					releaseRom(rom);

					//invert back...broken rom!
					swap16(rom->romData,(int)rom->romLength);

					if(getRomSuffix() == "sms")
					{
						TRACE_PLUG_INFO("The given rom does not contain a valid header , but '.sms' was found in the extension,hence the runmode will be based on the extension");
					
						dst->romFormat = NEOMD_FMT_SMS;
						return true;
					}
					else if(getRomSuffix() == "md" || getRomSuffix() == "gen"|| getRomSuffix() == "bin" || getRomSuffix() == "32x" || getRomSuffix() == "mars")
					{
						std::string s = "The given rom does not contain a valid header , but '.";
						s += getRomSuffix();
						s += "' was found in the extension,hence the runmode will be based on the extension";

						if(getRomSuffix() == "sms")
							dst->romFormat = NEOMD_FMT_SMS;
						else
						{
							if( (dst->romBufferLength > 0x10) && ( (dst->romBufferLength/131072) > 33 ) )
								dst->romFormat = NEOMD_FMT_MD32X_32_40;
							else
								dst->romFormat = NEOMD_FMT_MD32X_1_32_NOS;
						}

						TRACE_PLUG_INFO(s.c_str());
						
						return true;
					}

					TRACE_PLUG_ERROR("The given rom is not a GENESIS/MD/SMS/32X/BIOS rom. Aborting");
					return false;
				}
			}
		}
		
		updateRomInfo(rom);

		/*if(isStaticLockonRom(rom,dst)) not needed
		{
			lockOn = true;//sometime we might need to run patch cycle 2 times for all blocks
			//TRACE_PLUG_INFO("Detected rom with LOCK-ON data section.Overlap fix was successful!");
			//writeRom(rom,"c:\\md_lockon_last.bin",FMT_BIN);
		}
		*/
	}
	
	FOLLOW("BEG PATCH CYCLE");
	for(int i = 0; i < opCodes.size(); i++)
	{
		COpCode& op = opCodes[i];

		if(*(OPCODE_FUNCS + op.getOpCode()))
		{
			if(! (*(OPCODE_FUNCS + op.getOpCode()))(op,dst,rom) )
			{
				TRACE_PLUG_ERROR("Opcode execution failed! This is critical error.");
				break;
			}
		}
	}
	FOLLOW("END PATCH CYCLE");

	if(!isSMSStream())
	{
		FOLLOW("BEG PRE-RT SCAN");
		updateRomInfo(rom);

		if(!scanRomType(dst,rom))
		{	
			unMountRom(rom);
			releaseRom(rom);
			TRACE_PLUG_ERROR("Unable to detect the format of the rom. Please set the type manually.");
			return false;
		}

		FOLLOW("BEG KNUKLES PATCH");
		memcpy(sig,(((char*)dst->romBuffer) + 0x150),0x15F - 0x150);
		sig[0x15F - 0x150] = '\0';

		if(!memcmp(sig,"SONIC & KNUCKLES",0x15F - 0x150))
		{
			dst->romFormat = NEOMD_FMT_MD_1_32_S;
			dst->saveAllocationSize = 8;
		}

		FOLLOW("END KNUKLES PATCH");

		FOLLOW("END PRE-RT SCAN");
	}

	/*if(lockOn)
	{
		//patch previous block
		//patchSRAM68K(rom,dst,sizeMbit);

		//rollback
		NEOMD_RomFormat lastFormat = dst->romFormat;
		//int lastSave = dst->saveAllocationSize;

		rom->romData = (unsigned char*)dst->romBuffer;
		rom->romLength = dst->romBufferLength;
		updateRomInfo(rom);

		scanRomType(dst,rom);

		NEOMD_RomFormat newFormat = dst->romFormat;

		if(newFormat < lastFormat)
			dst->romFormat = newFormat;
		else
			dst->romFormat = lastFormat;

		//dst->saveAllocationSize += lastSave;

		//if(dst->saveAllocationSize > 128)
		//	dst->saveAllocationSize = 128;

		rom->romLength = dst->romBufferLength;
	}*/

	//patchSRAM68K(rom,dst,sizeMbit);

	applyNBAJamTE32xFix(rom);
	addSDMenuSupportCode(rom,dst);

	//TRACE_PLUG_INFO(rom->memo);
	//writeRom(rom,"c:\\md_dump.bin",FMT_BIN);

	unMountRom(rom);

	rom = releaseRom(rom);//remove only the interface - not our own memory

	return true;
}


