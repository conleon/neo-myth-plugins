
#ifndef __codeGenerator_hpp__
#define __codeGenerator_hpp__
	#include "..\\PlugBase.h"
	void genCode(NEOSNES::NeoResultBlock *src , std::vector<COpCode>& bc);
	const unsigned int buildHash(const std::string& in);
	const std::string& getPlugPath();
	const std::string& getRomSuffix();
	void muteOpCode(const int op,std::vector<COpCode>& bc);
#endif
