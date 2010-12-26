/*
	Copyright (c) 2009 - 2010 conleon1988 (conleon1988@gmail.com)

	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef _port_h_
#define _port_h_
	#ifdef __cplusplus
		extern "C" 
		{
	#endif 

	/*version*/
	#define	OPENGENIE_MAJOR_VERSION 2
	#define OPENGENIE_MINOR_VERSION 7
	#define OPENGENIE_MAINTENANCE_VERSION 7
	#define OPENGENIE_BULD_VERSION 7
	#define OPENGENIE_BUILD_DATE 10/10/09
	#define OPENGENIE_VERSION_STR_CONV(__V__) #__V__
	#define OPENGENIE_VERSION_TO_STRING(__V__) OPENGENIE_VERSION_STR_CONV(__V__)

	/*platform config*/
	#define WINDOWS_BUILD

	#if defined UNIX_BUILD
		#define PATH_SEPARATOR_C '/'
		#define PATH_SEPARATOR_S "/"
		#undef ALLOW_DEBUG_BLOCKS
		#undef ALIGN_MEMORY
		#define DEFAULT_MEMORY_ALIGNMENT (16)
		#define ALLOW_ATTRIBUTES
		#define ALLOW_REGISTERS
		#define extractFilename(s) myStrrchr(s, '/')
		#include <dirent.h>
		#undef ALLOW_FILE_LOGGING
	#elif defined WINDOWS_BUILD
		//vc6 hack
		static const int strncasecmp(const char* s , const char* fs,int len)
		{
			//TODO OPTIMIZE THIS!
			if(!memcmp(s,fs,len))
				return 0;
			else return memcmp(s,fs,len);
		}
		
		static const int strcasecmp(const char* s , const char* fs)
		{
			int len = strlen(fs);
			//TODO OPTIMIZE THIS!
			if(!memcmp(s,fs,len))
				return 0;
			else return memcmp(s,fs,len);
		}
		
		#undef ALLOW_FILE_LOGGING
		#define PATH_SEPARATOR_C '\\'
		#define PATH_SEPARATOR_S "\\"
		#undef ALLOW_DEBUG_BLOCKS
		#undef ALIGN_MEMORY
		#define DEFAULT_MEMORY_ALIGNMENT (16)
		#undef ALLOW_ATTRIBUTES
		#define ALLOW_REGISTERS
		#define extractFilename(s) myStrrchr(s, '\\')
		#include <windows.h>
	#endif
	
	/*my exit func*/
	#define MY_EXIT_FUNC atexit

	/*memory allocator*/
	#if defined (ALIGN_MEMORY)
		#define xalloc(__S__) memalign(DEFAULT_MEMORY_ALIGNMENT,__S__)
		#define xrealloc realloc
		#define ALLIGNEDBY(__S__) __attribute__ ((aligned (__S__)))
	#else
		#define xalloc malloc
		#define xrealloc realloc
		#define ALLIGNEDBY(__S__)
	#endif

	#define INLINE
	
	/*attributes*/
 	#if defined (ALLOW_ATTRIBUTES)
		#define ATTRIBUTE(__ATTR__)__attribute__ ((section (ATTR))) = { 0 }
		#define FORCE_INLINE __attribute__((always_INLINE))
		#define SET_FUNC_ATTR(__ATTR__) __attribute__ ((__ATTR__))
	#else
		#define ATTRIBUTE(__ATTR__)
		#define FORCE_INLINE
		#define SET_FUNC_ATTR(__ATTR__)
	#endif

	/* max path */
	#undef MAX_PATH
	#define MAX_PATH (512)

	/*misc*/
	#define getExtension(s) myStrrchr(s, '.')

	/*logging*/
	#if defined(ALLOW_CONSOLE_LOGGING)
		#define dumpToConsole printf
	#else
		#define dumpToConsole
	#endif

	/*registers?*/
	#if defined(ALLOW_REGISTERS)
		#define REG register
	#else
		#define REG
	#endif

	/*Dir iterator?*/
	#undef COMPILE_WITH_DIRECTORY_ITERATOR

	#ifdef __cplusplus
		}
	#endif 
#endif



