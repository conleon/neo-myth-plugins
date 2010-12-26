
#ifndef __COPCODE_HPP__
#define __COPCODE_HPP__
	
	#include "..\\PlugBase.h"

	enum 
	{
		OP_NOP = 0,
		OP_GGC, 
		OP_IPS,
		OP_CRC,
		OP_PALFIX,
		OP_NTSCFIX,
		OPCODES_SIZE
	};

	
	class COpCode
	{
		private:
		int m_Code;
		std::vector<std::string> m_Args;
		std::vector<unsigned int> m_HashedArgs;
		//std::vector<int> m_NumArgs; off for now

		public:
		COpCode();
		COpCode(int op );
		~COpCode();

		public:
		void setOpCode(const int op);
		void addDelimBuffer(const std::string& buf);
		void addArg(const std::string& arg);
		void clear();
		const std::vector<std::string>& getArgs();
		const std::vector<unsigned int>& getHashedArgs();
		const int& getOpCode();
	};

	
#endif
