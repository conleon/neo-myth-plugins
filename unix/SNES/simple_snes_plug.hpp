/*
	By conleon1988@gmail.com for madmonkey's opensource programmer
	Complete version of the plugin : http://code.google.com/p/neo-myth-plugins/
*/
#ifndef _SSP_HPP_
#define _SSP_HPP_

	#include <iostream>
	#include <sstream>
	#include <fstream>
	#include <string>
	#include <vector>
	#include <stdint.h>
	#include <string.h>

	typedef uint32_t u32;
	typedef uint16_t u16;
	typedef uint8_t u8;
	typedef int32_t s32;
	typedef int16_t s16;
	typedef int8_t s8;
	
	enum NEOSNES_RomMapping
	{
		NEOSNES_MAP_UNKNOWN = -1,
		NEOSNES_MAP_LoROM = 0x0,
		NEOSNES_MAP_HiROM,
		NEOSNES_MAP_ExLoROM,
		NEOSNES_MAP_ExHiROM,
		NEOSNES_MAP_DSP1_LoROM1MB,
		NEOSNES_MAP_DSP1_LoROM2MB,
		NEOSNES_MAP_DSP1_HiROM,
		NEOSNES_MAP_DSP2,
		NEOSNES_MAP_DSP3,
		NEOSNES_MAP_DSP4
	};

	enum NEOSNES_RomFormat
	{
		NEOSNES_FMT_UNKNOWN = -1,
		NEOSNES_FMT_STANDARD_ROM = 0x0,
		NEOSNES_FMT_SPC_SAMPLE,/*either vgm/z->spc or spc*/
		NEOSNES_FMT_NEOSHELL_EXT_RAM_MENU,/*sd ram menu*/
		NEOSNES_FMT_NEOSHELL_EXT_ROM_MENU
	};

	enum NEOSNES_SaveType
	{
		NEOSNES_ST_NO_SAVE = -1,
		NEOSNES_ST_16K = 0x0,
		NEOSNES_ST_32K,
		NEOSNES_ST_64K,
		NEOSNES_ST_128K,
		NEOSNES_ST_256K,
		NEOSNES_ST_512K,
		NEOSNES_ST_1M
	};
	
	enum NEOSNES_RAMSize /*NEW*/
	{
		NEOSNES_RAMSIZE_0 = 0x0,
		NEOSNES_RAMSIZE_1,
		NEOSNES_RAMSIZE_2,
		NEOSNES_RAMSIZE_3,
		NEOSNES_RAMSIZE_4,
		NEOSNES_RAMSIZE_5,
		NEOSNES_RAMSIZE_6,
		NEOSNES_RAMSIZE_7
	};

	enum NEOSNES_ROMSize  /*NEW*/
	{
		NEOSNES_ROMSIZE_4M = 0x0,
		NEOSNES_ROMSIZE_8M,
		NEOSNES_ROMSIZE_16M,
		NEOSNES_ROMSIZE_24M,
		NEOSNES_ROMSIZE_32M,
		NEOSNES_ROMSIZE_40M,
		NEOSNES_ROMSIZE_48M,
		NEOSNES_ROMSIZE_64M
	};

	struct NeoResultBlock
	{
		u8* romBuffer;
		s8* arguments;
		u32 romBufferLength;
		NEOSNES_ROMSize romSizeDetected; /*NEW*/
		NEOSNES_RAMSize ramSizeDetected; /*NEW*/
		NEOSNES_RomMapping romMapping;
		NEOSNES_RomFormat romFormat;
		NEOSNES_SaveType saveType;
	};

	bool snes_plug_detect(NeoResultBlock* p_block,const bool rm_header = true);
#endif

