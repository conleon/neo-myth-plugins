
// !NEON64 Plugin Interface (c)2010 by Conleon1988@gmail.com for WWW.NEOFLASH.COM MYTH Project
#ifndef _PLUGBASE_H
#define _PLUGBASE_H
#include "NEO_SN64Engine_Types.h"//includes definitions from the SuperN64 patching engine
#include "NEO_SN64Engine_DynaStatsShared.h"

typedef BOOL (WINAPI * PFN_Plug_CreateObject)(void ** pobj);

struct NEON64ResultBlock
{
	void(*myDynaStatsCB)(NEON64DynamicStatsBlock* dsBlock); //Dynamic stats

	const char* actionScript;//Script to run
	int actionScriptLength;//Script length in bytes

    NEON64_BootChip bootChip;//detected chip
    NEON64_SaveType saveType;//detected save type
    NEON64_RomFormat romFormat;//this is not needed but you can use it to update some text (maybe)
};

class CPlugBase 
{
public:
	CPlugBase(){};

public:
	virtual bool Patching(void* pBlock) = 0;
	virtual void Release() = 0;
};

typedef struct
{
	CPlugBase * pObj;
	HINSTANCE hIns;
}PLUG_ST, * LPPLUG_ST;

#endif