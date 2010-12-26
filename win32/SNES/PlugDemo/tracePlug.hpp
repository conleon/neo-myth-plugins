
#ifndef __tracePlug_hpp__
#define __tracePlug_hpp__

#include <windows.h>
	const bool isSilentMode();
	void forceSilentMode(const bool silent);

	#define TRACE_PLUG_INFO(__S__) {if(!isSilentMode()){MessageBox(NULL, __S__, "SuperSNES NEO2 Edition plugin", MB_OK|MB_ICONINFORMATION);}}
	#define TRACE_PLUG_WARN(__S__) {if(!isSilentMode()){MessageBox(NULL, __S__, "SuperSNES NEO2 Edition plugin", MB_OK|MB_ICONWARNING);}}
	#define TRACE_PLUG_ERROR(__S__) {if(!isSilentMode()){MessageBox(NULL, __S__, "SuperSNES NEO2 Edition plugin", MB_OK|MB_ICONERROR);}}
	#define TRACE_PLUG_CONFIRM(__S__) {if(!isSilentMode()){MessageBox(NULL, __S__, "SuperSNES NEO2 Edition plugin", MB_YESNO|MB_ICONWARNING);}}
	#define TRACE_PLUG_RESULT_POSITIVE IDYES


	#undef SUPERSNES_PLUG_DEBUG
	#define FOLLOW(__OP__)
	#define FOLLOW2(__OP__,__S__)

	#ifdef SUPERSNES_PLUG_DEBUG
		#undef FOLLOW2
		#undef FOLLOW
		#define FOLLOW(__OP__) TRACE_PLUG_INFO(__OP__)
		#define FOLLOW2(__OP__,__S__) TRACE_PLUG_INFO(std::string(__OP__ + itoS<int>(__S__,std::dec)).c_str())
	#endif

	inline void writeDump(const std::string& filename , const std::string& buf)
	{
		std::ofstream out(filename.c_str());

		out.write(buf.c_str(),buf.length());
		out.close();

		std::string s = "A dump has been created in NEO2 ULTRA MENU directory.\n[" + filename + "]";
		TRACE_PLUG_INFO(s.c_str());
	}
#endif

