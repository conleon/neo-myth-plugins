#include "stdafx.h"
#include "COpCode.hpp"
#include "codeGenerator.hpp"

COpCode::COpCode()
{
	clear();
	m_Code = -1;
}

COpCode::COpCode(int op)
{
	clear();
	m_Code = op;
}

COpCode::~COpCode()
{
	clear();
}

void COpCode::setOpCode(const int op)
{
		m_Code = op;
}

void COpCode::addDelimBuffer(const std::string& buf)
{
	int i = 0;
	std::string result = "";
	unsigned int hash = 1315423911;
	
	for( i = 0; i < buf.length(); i++)
	{
		if(buf[i] == ';')
		{
			if(!result.empty())
			{
				//optimization : hash + copy at once!
				m_Args.push_back(result);
				m_HashedArgs.push_back(hash);

				FOLLOW(result.c_str());

				//addArg(result);
				result = "";
				hash = 1315423911;
			}
		}
		else
		{
			result += buf[i];
			hash ^= ((hash << 5) + buf[i] + (hash >> 2));
		}
	}
	
	//maybe the string does not end with delim
	if(!result.empty())
	{
		FOLLOW(result.c_str());

		m_Args.push_back(result);
		m_HashedArgs.push_back(hash);
	}
}

void COpCode::addArg(const std::string& arg)
{
	FOLLOW("PUSH ARG");
	m_Args.push_back(arg);
	m_HashedArgs.push_back(buildHash(arg));
}
	
void COpCode::clear() 
{
	m_Args.clear();
	m_HashedArgs.clear();
}

const std::vector<std::string>& COpCode::getArgs()
{ 
	return m_Args; 
}

const std::vector<unsigned int>& COpCode::getHashedArgs()
{
	return m_HashedArgs; 
}

const int& COpCode::getOpCode() 
{
	return m_Code; 
}


