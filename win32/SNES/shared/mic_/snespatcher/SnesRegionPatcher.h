/*
	SNES region patcher interface
	Mic, 2010
*/

#ifndef SNESREGIONPATCHER_H
#define SNESREGIONPATCHER_H

#include "SnesPatcher.h"

static const int REGION_NTSC = 0;
static const int REGION_PAL = 1;

class SnesRegionPatcher : public SnesPatcher
{
public:
	int Patch(const int newRegion, const char *smcFilename, const char *outputFilename=NULL);

	//Required for SNES plugin
	int Patch(const int newRegion,char* fileBuffer, const unsigned int fileSize);
};

#endif
