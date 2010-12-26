
#ifndef __plugCommon_hpp__
#define __plugCommon_hpp__

	#define SUPERMD_PLUG_DEBUG
	#pragma warning(disable:4786)
	#pragma warning(disable:4182)

	#include <iostream>
	#include <sstream>
	#include <fstream>
	#include <string>
	#include <vector>
	//#include <list> <- very buggy version :(
	#include "COpCode.hpp"
	#include "tracePlug.hpp"

	typedef enum CALLER
	{
		CALLER_SUPERMD_FRAMEWORK,
		CALLER_NEOMDMYTH,
		CALLER_3RDPARTY
	};

	template <class T>
	inline std::string itoS(T t, std::ios_base & (*f)(std::ios_base&))
	{
		std::ostringstream oss;
		oss << f << t;
		
		return oss.str();
	}

	inline std::string getModulePath()
	{
		static char rst[ 512 ];
		std::string s = std::string( rst, GetModuleFileName( NULL, rst, 512 ) );

		int i = s.length();

		while(i-- > 0)
		{
			if(s[i]=='\\')
			{
				s = s.substr(0,i+1);

				s += "Plug_In\\SNES\\";
				break;
			}
		}

		return s;
	}

	inline std::string getFileExtension(const std::string& in)
	{
		int i = in.rfind('.');

		if(i == std::string::npos)
			return "__q__";

		return in.substr( i+1 );
	}

	inline void strToLower(std::string& in)
	{
		for(int i = 0; i < in.length(); i++)
			in[i] = tolower(in[i]);
	}

	inline void strToUpper(std::string& in)
	{
		for(int i = 0; i < in.length(); i++)
			in[i] = toupper(in[i]);
	}

	inline const CALLER getModuleCaller()
	{
		CALLER result = CALLER_3RDPARTY;
		static char rst[ 512 ];
		std::string s = std::string( rst, GetModuleFileName( NULL, rst, 512 ) );

		strToLower(s);

		if(s.find("supermd") != std::string::npos)
			result = CALLER_SUPERMD_FRAMEWORK;
		else if(s.find("neo2client") != std::string::npos)
			result = CALLER_NEOMDMYTH;

		return result;

	
	}

#endif
