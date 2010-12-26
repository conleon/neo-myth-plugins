//Header scoring based on byuu's bsnes emulator -- http://byuu.org/
//I've tried to keep the code as much closer to the original possible
#include "codeGenerator.hpp"
#include "image.hpp"
#include "..//shared//mic_//vgmz_encoder//vgm2spc_snes_plug_interface.hpp"
#include "..//relocs//smp_player_buffer.hpp"
using namespace NEOSNES;

  enum HeaderField {
    CartName    = 0x00,
    Mapper      = 0x15,
    RomType     = 0x16,
    RomSize     = 0x17,
    RamSize     = 0x18,
    CartRegion  = 0x19,
    Company     = 0x1a,
    Version     = 0x1b,
    Complement  = 0x1c,  //inverse checksum
    Checksum    = 0x1e,
    ResetVector = 0x3c,
  };


int rateHeader(const unsigned char* data,const unsigned int size,const unsigned int addr)
{
  if(size < addr + 64) return 0;  //image too small to contain header at this location?
  int score = 0;

  unsigned short resetvector = data[addr + ResetVector] | (data[addr + ResetVector + 1] << 8);
  unsigned short checksum    = data[addr + Checksum   ] | (data[addr + Checksum    + 1] << 8);
  unsigned short complement  = data[addr + Complement ] | (data[addr + Complement  + 1] << 8);

  unsigned char resetop = data[(addr & ~0x7fff) | (resetvector & 0x7fff)];  //first opcode executed upon reset
  unsigned char mapper  = data[addr + Mapper] & ~0x10;                      //mask off irrelevent FastROM-capable bit

  //$00:[000-7fff] contains uninitialized RAM and MMIO.
  //reset vector must point to ROM at $00:[8000-ffff] to be considered valid.
  if(resetvector < 0x8000) return 0;

  //some images duplicate the header in multiple locations, and others have completely
  //invalid header information that cannot be relied upon.
  //below code will analyze the first opcode executed at the specified reset vector to
  //determine the probability that this is the correct header.

  //most likely opcodes
  if(resetop == 0x78  //sei
  || resetop == 0x18  //clc (clc; xce)
  || resetop == 0x38  //sec (sec; xce)
  || resetop == 0x9c  //stz $nnnn (stz $4200)
  || resetop == 0x4c  //jmp $nnnn
  || resetop == 0x5c  //jml $nnnnnn
  ) score += 8;

  //plausible opcodes
  if(resetop == 0xc2  //rep #$nn
  || resetop == 0xe2  //sep #$nn
  || resetop == 0xad  //lda $nnnn
  || resetop == 0xae  //ldx $nnnn
  || resetop == 0xac  //ldy $nnnn
  || resetop == 0xaf  //lda $nnnnnn
  || resetop == 0xa9  //lda #$nn
  || resetop == 0xa2  //ldx #$nn
  || resetop == 0xa0  //ldy #$nn
  || resetop == 0x20  //jsr $nnnn
  || resetop == 0x22  //jsl $nnnnnn
  ) score += 4;

  //implausible opcodes
  if(resetop == 0x40  //rti
  || resetop == 0x60  //rts
  || resetop == 0x6b  //rtl
  || resetop == 0xcd  //cmp $nnnn
  || resetop == 0xec  //cpx $nnnn
  || resetop == 0xcc  //cpy $nnnn
  ) score -= 4;

  //least likely opcodes
  if(resetop == 0x00  //brk #$nn
  || resetop == 0x02  //cop #$nn
  || resetop == 0xdb  //stp
  || resetop == 0x42  //wdm
  || resetop == 0xff  //sbc $nnnnnn,x
  ) score -= 8;

  //at times, both the header and reset vector's first opcode will match ...
  //fallback and rely on info validity in these cases to determine more likely header.

  //a valid checksum is the biggest indicator of a valid header.
  if((checksum + complement) == 0xffff && (checksum != 0) && (complement != 0)) score += 4;

  if(addr == 0x007fc0 && mapper == 0x20) score += 2;  //0x20 is usually LoROM
  if(addr == 0x00ffc0 && mapper == 0x21) score += 2;  //0x21 is usually HiROM
  if(addr == 0x007fc0 && mapper == 0x22) score += 2;  //0x22 is usually ExLoROM
  if(addr == 0x40ffc0 && mapper == 0x25) score += 2;  //0x25 is usually ExHiROM

  if(data[addr + Company] == 0x33) score += 2;        //0x33 indicates extended header
  if(data[addr + RomType] < 0x08) score++;
  if(data[addr + RomSize] < 0x10) score++;
  if(data[addr + RamSize] < 0x08) score++;
  if(data[addr + CartRegion] < 14) score++;

  if(score < 0) score = 0;
  return score;
}

unsigned int getHeaderOffset(const unsigned char* data,const unsigned int size)
{
	int lo = rateHeader(data, size, 0x007fc0);
	int hi = rateHeader(data, size, 0x00ffc0);
	int ex = rateHeader(data, size, 0x40ffc0);

//	char ss[64];
	ex += (ex) ? 4 : 0;

//	sprintf(ss,"%d,%d,%d\n",lo,hi,ex);
//	TRACE_PLUG_INFO(ss);
//	if((!lo) && (!hi) && (!ex))
//		return 0;

	if((lo >= hi) && (lo >= ex))
		return 0x007fc0;
	else if(hi >= ex)
		return 0x00ffc0;

	return (ex > 4) ? 0x40ffc0 : 0;
}

/*For mic_'s encoder stuff*/
bool preCacheBuffer(const unsigned char* buf,const int size,const std::string& filename)
{
	const std::string path = getPlugPath() + filename;
	//TRACE_PLUG_INFO(path.c_str());
	FILE* f = fopen(path.c_str(),"wb");
	int r;

	if(!f)
		return false;

	r = fwrite((char*)buf,1,size,f);
	fclose(f);

	return (r == size);
}

inline bool generateSMPPlayerBinary()
{
	return preCacheBuffer((const unsigned char*)SMP_PLAYER_BUFFER,sizeof(SMP_PLAYER_BUFFER)/sizeof(SMP_PLAYER_BUFFER[0]),"s-smp_player.bin");
}

static void fixSPCBuffer(NeoResultBlock* block)
{
	static unsigned char spc_sh[0x100];

	if(block->romBufferLength < 0x10080 + 0x100)
	{
		TRACE_PLUG_WARN("WARNING : SPC BUFFER LENGTH DOES NOT REACH THE MINIMUM 0x10180");
		return;
	}

	memcpy((unsigned char*)spc_sh,(const unsigned char*)block->romBuffer,0x100);
	memmove(((unsigned char*)block->romBuffer),((const unsigned char*)block->romBuffer)+0x100,0x10080);
	memmove(((unsigned char*)block->romBuffer) + 0x10080,(const unsigned char*)spc_sh,0x100);
}

bool isExtendedFormat(NeoResultBlock* block,const unsigned char* md5) /*extended formats go here (vgm/spc/custom/hacks)*/
{	
	const unsigned char* data = (const unsigned char*)block->romBuffer;
	const unsigned int size = (const unsigned int)block->romBufferLength;
	unsigned char* p;

	static const unsigned char spcSignature[] = 
	{ //"SNES-SPC700"
		0x53,
		0x4e,
		0x45,
		0x53,
		0x2d,
		0x53,
		0x50,
		0x43,
		0x37,
		0x30,
		0x30
	};

	static const unsigned char vgmSignature[] = 
	{ //"Vgm "
		0x56,
		0x67,
		0x6d,
		0x20
	};

	//bad
	static const unsigned char vgzSignature[] =//{1F 8B} 08 08 9F 3E CE 4A 02
	{ 
		0x1F, 
		0x8B,
	};

	if(memcmp(data,spcSignature,sizeof(spcSignature) / sizeof(spcSignature[0]) ) == 0)
	{
		block->romFormat = NEOSNES_FMT_SPC_SAMPLE;
		block->romSizeDetected = NEOSNES_ROMSIZE_4M;
		fixSPCBuffer(block);
		return true;
	}

	//vgm -> spc
	if(memcmp(data,vgmSignature,sizeof(vgmSignature) / sizeof(vgmSignature[0])) == 0)
	{
		block->romFormat = NEOSNES_FMT_SPC_SAMPLE;
		block->romSizeDetected = NEOSNES_ROMSIZE_4M;

		//encode
		if(generateSMPPlayerBinary())
		{
			int n = 0;
			const std::string path = getPlugPath() + "cache.vgm";

			preCacheBuffer(data,(int)size,"cache.vgm");
			encoderSetPluginDirectory(getPlugPath());

			if(block->romBufferLength < 0x10080 + 0x100)
			{
				block->romBufferLength = 0x10080 + 0x100;
				
				p = new unsigned char[block->romBufferLength];

				delete block->romBuffer;
				block->romBuffer = (void*)p;
			}

			if(encodeVgm(path.c_str(),(char*)block->romBuffer,&n))
			{
				if(n <= (int)block->romBufferLength)
					block->romBufferLength = (unsigned int)n;

				fixSPCBuffer(block);
			}
			else
				TRACE_PLUG_WARN("FAILED CONVERTING VGM->SPC");
		}

		return true;
	}
	
	//vgz -> spc
	if(memcmp(data,vgzSignature,sizeof(vgzSignature) / sizeof(vgzSignature[0])) == 0)
	{
		block->romFormat = NEOSNES_FMT_SPC_SAMPLE;
		block->romSizeDetected = NEOSNES_ROMSIZE_4M;

		//encode
		if(generateSMPPlayerBinary())
		{
			int n = 0;
			const std::string path = getPlugPath() + "cache.vgz";

			preCacheBuffer(data,(int)size,"cache.vgz");
			encoderSetPluginDirectory(getPlugPath());

			if(block->romBufferLength < 0x10080 + 0x100)
			{
				block->romBufferLength = 0x10080 + 0x100;
				
				p = new unsigned char[block->romBufferLength];

				delete block->romBuffer;
				block->romBuffer = (void*)p;
			}

			if(encodeVgz(path.c_str(),(char*)block->romBuffer,&n))
			{
				if(n <= (int)block->romBufferLength)
					block->romBufferLength = (unsigned int)n;

				fixSPCBuffer(block);
			}
			else
				TRACE_PLUG_WARN("FAILED CONVERTING VGZ->SPC");
			
		}

		return true;
	}

		//Dai Kaiju Monogotari 2 ---snes9x sets the size to 40Mbit
	//Tales of Phantasia NTSC/J English Translated Lowercase DeJap V1.2 48Mbit

	/*
	//sgb rom
	if(size >= 0x0140) 
	{
		if(data[0x0104] == 0xce && data[0x0105] == 0xed && data[0x0106] == 0x66 && data[0x0107] == 0x66
			&& data[0x0108] == 0xcc && data[0x0109] == 0x0d && data[0x010a] == 0x00 && data[0x010b] == 0x0b) 
		{
			block->romFormat = NEOSNES_FMT_SUPERGAMEBOY_ROM;
			return true;
		}
	}*/
	
	return false;
}

void configureImage(NeoResultBlock* block,const unsigned char* md5)
{
	block->saveType = NEOSNES_ST_NO_SAVE;
	block->romFormat = NEOSNES_FMT_STANDARD_ROM;
	block->romMapping = NEOSNES_MAP_LoROM;
	block->ramSizeDetected = NEOSNES_RAMSIZE_0;
		
	if(isExtendedFormat(block,md5))
	{
		FOLLOW("EXT FORMAT");
		return;
	}

	const unsigned char* data = (const unsigned char*)block->romBuffer;
	const unsigned int size = (const unsigned int)block->romBufferLength;
	const unsigned int headOffs = getHeaderOffset(data,size);

	if(!headOffs)
	{
		return;
	}

	const unsigned char mapperid = data[headOffs+Mapper];
	const unsigned char rom_type = data[headOffs+RomType];
	const unsigned char rom_size = data[headOffs+RomSize];
	const unsigned char company  = data[headOffs+Company];
	const unsigned char regionid = data[headOffs+CartRegion] & 0x7f;
	const int sram = 1024 << (data[headOffs+RamSize] & 7);
	
	/*
	# $08 => $40000 bytes == 256 kilobytes, minimum for ROM
	# $09 => $80000 bytes == 512 kilobytes, amount of ROM in Super Mario World
	# $0a => $100000 bytes == 1 megabyte, amount of ROM in Mario Paint
	# $0b => $200000 bytes == 2 megabytes
	# $0c => $400000 bytes == 4 megabytes 
	*/

	switch(rom_size)
	{
		case 0x08:
			block->romSizeDetected = NEOSNES_ROMSIZE_4M;//2M; Core doesn't seem to support 2Mb ??????????
		break;

		case 0x09:
			block->romSizeDetected = NEOSNES_ROMSIZE_4M;
		break;

		case 0x0a:
			block->romSizeDetected = NEOSNES_ROMSIZE_8M;
		break;

		case 0x0b:
			block->romSizeDetected = NEOSNES_ROMSIZE_16M;
		break;

		case 0x0c:
			block->romSizeDetected = NEOSNES_ROMSIZE_32M;
		break;

		default://try a bit more
		{
		//	TRACE_PLUG_INFO("#M");
			const int mbit = ((int)block->romBufferLength) / 131072;

			if(mbit < 4)
				block->romSizeDetected = NEOSNES_ROMSIZE_4M;//2M; Core doesn't seem to support 2Mb ??????????
			else if( (mbit > 4) && (mbit <= 8))
				block->romSizeDetected = NEOSNES_ROMSIZE_8M;
			else if( (mbit > 8) && (mbit <= 16))
				block->romSizeDetected = NEOSNES_ROMSIZE_16M;
			else if( (mbit > 16) && (mbit <= 32))
				block->romSizeDetected = NEOSNES_ROMSIZE_32M;
			else if( (mbit > 32) && (mbit <= 40))
				block->romSizeDetected = NEOSNES_ROMSIZE_40M;
			else if( (mbit > 40) && (mbit <= 48))
				block->romSizeDetected = NEOSNES_ROMSIZE_48M;
			else if((mbit > 48 ) && (mbit <= 64) )
				block->romSizeDetected = NEOSNES_ROMSIZE_64M;
			else if( (mbit > 64) )
			{
				TRACE_PLUG_WARN("Sorry,the CORE does NOT support dumps larger than 64Mbit!.");
				return;
			}

			break;
		}
	}

	if(sram != 1024)
	{
		/*
				# $00 => no RAM
				# $01 => $800 bytes == 2 kilobytes, amount of RAM in Super Mario World
				# $02 => $1000 bytes == 4 kilobytes
				# $03 => $2000 bytes == 8 kilobytes
				# $04 => $4000 bytes == 16 kilobytes
				# $05 => $8000 bytes == 32 kilobytes, maximum for RAM, amount of RAM in Mario Paint
				# $06 => $10000 bytes == 64 kilobytes
				# $07 => $20000 bytes == 128 kilobytes 
		*/
		switch(sram)
		{
			default:
			{
				block->ramSizeDetected = NEOSNES_RAMSIZE_0;
				block->saveType = NEOSNES_ST_NO_SAVE;
				break;
			}

			case 0x800:
			{
				block->ramSizeDetected = NEOSNES_RAMSIZE_1;
				block->saveType=NEOSNES_ST_16K;
				break;
			}

			case 0x1000:
			{
				block->ramSizeDetected = NEOSNES_RAMSIZE_2;
				block->saveType=NEOSNES_ST_32K;
				break;
			}

			case 0x2000:
			{
				block->ramSizeDetected = NEOSNES_RAMSIZE_3;
				block->saveType=NEOSNES_ST_64K;
				break;
			}

			case 0x4000:
			{
				block->ramSizeDetected = NEOSNES_RAMSIZE_4;
				block->saveType=NEOSNES_ST_128K;
				break;
			}

			case 0x8000:
			{
				block->ramSizeDetected = NEOSNES_RAMSIZE_5;
				block->saveType=NEOSNES_ST_256K;
				break;
			}

			case 0x10000:
			{
				block->ramSizeDetected = NEOSNES_RAMSIZE_6;
				block->saveType=NEOSNES_ST_512K;
				break;
			}

			case 0x20000:
			{
				block->ramSizeDetected = NEOSNES_RAMSIZE_7;
				block->saveType=NEOSNES_ST_1M;
				break;
			}
		}
	}

	/*
	//BSX 
	{
		  if(data[0x13] == 0x00 || data[0x13] == 0xff) {
			if(data[0x14] == 0x00) {
			  const unsigned char n15 = data[0x15];
			  if(n15 == 0x00 || n15 == 0x80 || n15 == 0x84 || n15 == 0x9c || n15 == 0xbc || n15 == 0xfc) {
				if(data[0x1a] == 0x33 || data[0x1a] == 0xff) {
				  block->romFormat = NEOSNES_FMT_BSX_ROM;
				  return;
				}
			  }
			}
		  }
	}*/

	/*
	//sufami turbo
	{
		  if(!memcmp(dataHead, "BANDAI SFC-ADX", 14)) {
			if(!memcmp(dataHead + 16, "SFC-ADX BACKUP", 14)) {
			  block->romFormat = NEOSNES_FMT_SUFAMITURBO_BIOS;
			} else {
				block->romFormat = NEOSNES_FMT_SUFAMITURBO_ROM;
			}
			return;
		  }
	}*/

	/*
	//SGB BIOS
	{
		  if(!memcmp(data, "Super GAMEBOY2", 14)) {
			block->romFormat = NEOSNES_FMT_SUPERGAMEBOY2_BIOS;
			return;
		  }

		  if(!memcmp(data, "Super GAMEBOY", 13)) {
			block->romFormat = NEOSNES_FMT_SUPERGAMEBOY_BIOS;
			return;
		  }
	}*/

	/*
	//check for bsx slot
	{
		  if(*(data-14) == 'Z') {
			if(*(data-11) == 'J') {
			  unsigned char n13 = dataHead[13];
			  if((n13 >= 'A' && n13 <= 'Z') || (n13 >= '0' && n13 <= '9')) {
				if(company == 0x33 || ( *(data - 10) == 0x00 && *(data - 4) == 0x00)) {
				  return;
				}
			  }
			}
		  }
	}*/
		
	//setup dsp1
	bool foundDSP1 = false;
	{
		if(( (mapperid == 0x20) || (mapperid == 0x21)) && (rom_type == 0x03))
		{
			foundDSP1 = true;
			
			if(((mapperid & 0x2f) == 0x20) && (size <= 0x100000))
				block->romMapping = NEOSNES_MAP_DSP1_LoROM1MB;
			else if((mapperid & 0x2f) == 0x20)
				block->romMapping = NEOSNES_MAP_DSP1_LoROM2MB;
			else if((mapperid & 0x2f) == 0x21)
				block->romMapping = NEOSNES_MAP_DSP1_HiROM;
			else
				foundDSP1 = false;
		}
	}
	
	//map
	{
		if(!foundDSP1)
		{
			if((headOffs == 0x7fc0) && (size >= 0x401000))
			{
			//	TRACE_PLUG_INFO("NEOSNES_MAP_ExLoROM");
				block->romMapping = NEOSNES_MAP_ExLoROM;
			}
			else if((headOffs == 0x7fc0) && (mapperid == 0x32))
			{
			//	TRACE_PLUG_INFO("NEOSNES_MAP_ExLoROM");
				block->romMapping = NEOSNES_MAP_ExLoROM;
			}
			else if(headOffs == 0x7fc0)
			{
			//	TRACE_PLUG_INFO("NEOSNES_MAP_LoROM");
				block->romMapping = NEOSNES_MAP_LoROM;
			}
			else if(headOffs == 0xffc0)
			{
			//	TRACE_PLUG_INFO("NEOSNES_MAP_HiROM");
				block->romMapping = NEOSNES_MAP_HiROM;
			}
			else   //0x40ffc0
			{
				//TRACE_PLUG_INFO("NEOSNES_MAP_ExHiROM");
				block->romMapping = NEOSNES_MAP_ExHiROM;
			}
		}
	}


}
