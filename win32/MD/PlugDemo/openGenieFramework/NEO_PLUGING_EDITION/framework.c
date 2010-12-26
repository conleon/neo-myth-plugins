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

#include "opengenie.h"

static BitField* globalConfiguration = NULL;

BitField* SET_FUNC_ATTR(cold) getFrameworkConfigurationBitField()
{
	return globalConfiguration;
}

void SET_FUNC_ATTR(cold) initFrameworkConfig(unsigned short stackSize)
{
	printf("Creating interface configuration\n");
	writeToLogSimple("Creating interface configuration\n");
	writeToLog("OpenGenie version : %s\n",getFrameworkVersionString());

	releaseFrameworkConfig();

	globalConfiguration = createBitField(stackSize);
	loadDefaultFrameworkConfig();

	writeToLogSimple("Installing exit hook func\n");
	//MY_EXIT_FUNC(frameworkExitFunc); <-for vc6
}

void SET_FUNC_ATTR(cold) loadDefaultFrameworkConfig()
{
	printf("Loading default interface configuration\n");
	writeToLogSimple("Loading default interface configuration\n");

	if(!globalConfiguration)
		initFrameworkConfig(MIN_FW_CONFIG_SIZE);

	//auto ips patching
	globalConfiguration = setBitField(globalConfiguration,AUTO_IPS_PATCH,BIT_ON);

	//auto checksum patching
	globalConfiguration = setBitField(globalConfiguration,AUTO_CHECKSUM_PATCH,BIT_ON);
}

void SET_FUNC_ATTR(cold) releaseFrameworkConfig()
{
	if(globalConfiguration)
		globalConfiguration = releaseBitField(globalConfiguration);

	globalConfiguration = NULL;
}

//a quick hack!!
const char* SET_FUNC_ATTR(cold) getFrameworkVersionString()
{
	static char versionData[64];
	versionData[0] = '\0';
	versionData[63] = '\0';

	sprintf(versionData,"OpenGenie v%s.%s.%s.%s [%s]\0",OPENGENIE_VERSION_TO_STRING(OPENGENIE_MAJOR_VERSION),
					 OPENGENIE_VERSION_TO_STRING(OPENGENIE_MINOR_VERSION),
					 OPENGENIE_VERSION_TO_STRING(OPENGENIE_MAINTENANCE_VERSION),
					 OPENGENIE_VERSION_TO_STRING(OPENGENIE_BULD_VERSION),
					 OPENGENIE_VERSION_TO_STRING(OPENGENIE_BUILD_DATE));

	versionData[ strlen(versionData) ] = '\0';

	return versionData;
}

void SET_FUNC_ATTR(cold) frameworkExitFunc()
{
	htmlgenRelease();
	releaseFrameworkConfig();
	releaseStackPatchList();
}



