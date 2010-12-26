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

#ifndef _framework_h_
#define _framework_h_
	#ifdef __cplusplus
		extern "C" 
		{
	#endif 

	/*std*/
	#include <malloc.h>
	#include <stdio.h>
	#include <string.h>
	#include <stdarg.h>
	#include <stdlib.h>

	/*core*/
	#include "util.h"
	#include "fmt_smd.h"
	#include "fmt_bin.h"
	#include "fmt_multidoctor.h"
	#include "rom.h"
	#include "directory_iterator.h"
	#include "bit_field.h"
	#include "ips_patcher.h"
	#include "memory_patcher.h"
	#include "rom_writter.h"
	#include "html_gen.h"
	#include "port.h"
	#include "checksum_patcher.h"
	#include "logger.h"
	#include "game_genie.h"
	#include "logger.h"
	#include "stack_patch.h"
	#include "raw_patchfile.h"

	/* rom format */
	#define FMT_SMD (3)
	#define FMT_GEN (4)
	#define FMT_BIN (5)
	#define FMT_MD  (6)
	#define FMT_32X (7)
	#define FMT_SCD (8)
	#define FMT_UNKNOWN (2)
	#define LOADERS_COUNT  (6)

	/* result type */
	#define RST_SUCCESS (1)
	#define RST_FAILED (0)

	/*Framework config bitfield size*/
	#define MIN_FW_CONFIG_SIZE (7)

	/*for global config bitfield*/
	#define AUTO_IPS_PATCH (0)
	#define AUTO_CHECKSUM_PATCH (1)
	
	/* framework internal bitfield configuration*/
	void initFrameworkConfig(unsigned short stackSize);
	void loadDefaultFrameworkConfig();
	void releaseFrameworkConfig();
	void frameworkExitFunc();
	BitField* getFrameworkConfigurationBitField();
	const char* getFrameworkVersionString();

	#ifdef __cplusplus
		}
	#endif 
#endif


