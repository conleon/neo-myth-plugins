
#ifndef __codeGenerator_hpp__
#define __codeGenerator_hpp__
	#include "..\\PlugBase.h"
	void genCode(NeoResultBlock *src , std::vector<COpCode>& bc);
	const unsigned int buildHash(const std::string& in);
	bool isStreamEncoded();
	bool isSMSStream();
	const std::string& getPlugPath();
	const std::string& getRomSuffix();
	void forceEncodedStream();
	void forceSMSStream();
	void disableSMSStream();
	void disableEncodedStream();
	void muteOpCode(const int op,std::vector<COpCode>& bc);
#endif
