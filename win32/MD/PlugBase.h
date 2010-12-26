#ifndef _PLUGBASE_H
#define _PLUGBASE_H
#include "PlugDemo\\plugCommon.hpp"

typedef BOOL (WINAPI * PFN_Plug_CreateObject)(void ** pobj);

/*Fixed table*/
typedef enum NEOMD_RomFormat
{
	NEOMD_FMT_UNKNOWN = -1,
	/*SKIP ZERO - GBA CART BUS RESERVED -*/
	NEOMD_FMT_MD_1_16_S = 0x01,
	NEOMD_FMT_MD_1_32_S,
	NEOMD_FMT_32X_1_32_S,
	NEOMD_FMT_MD32X_32_40,
	NEOMD_FMT_MD32X_EEPROM,
	NEOMD_FMT_MD32X_1_32_NOS,
	NEOMD_FMT_MENU_EXT_FLASH,
	NEOMD_FMT_SCD_BIOS,
	NEOMD_FMT_SCD_SRAM,
	NEOMD_FMT_SCD_SYSTEM,
	NEOMD_FMT_SMS = 0x13,
	NEOMD_FMT_MENU_EXT_SD = 0x27
};

struct NeoResultBlock
{
	void* romBuffer;		// points to the loaded and decoded buffer without header . Just how it is before you add it to the cart filesystem.
	char* arguments;		// the configuration arguments. Set it to NULL if there are no arguments.

	int romBufferLength;	// the length of the rom starting from the header
	int saveAllocationSize; /* saveAllocationSize is the save size selected in the list view
							 0	- no save
							 8	- 8kbit(1KB)
							 16   - 16KBIT(2KB)
							 32	- 32 kbit(4KB)
							 64   - 64kbit(8KB)
							 128	- 128kbit(16KB)
							 -1	- 24C01*/
	NEOMD_RomFormat romFormat;
};

class CPlugBase {
public:
	CPlugBase(){};

public:
	virtual bool Patching(NeoResultBlock *pBlock) = 0;
	virtual void Release() = 0;
};

typedef struct{
	CPlugBase * pObj;
	HINSTANCE hIns;
}PLUG_ST, * LPPLUG_ST;

#endif
