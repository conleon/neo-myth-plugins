
#ifndef __VIRTUAL_MACHINE_H__
#define __VIRTUAL_MACHINE_H__

	#include "..\\PlugBase.h"

	void setCaller(const CALLER caller);	
	bool runCode(std::vector<COpCode>& opCodes,NEOSNES::NeoResultBlock *dst);
	bool runCode(NEOSNES::NeoResultBlock *dst);
#endif
