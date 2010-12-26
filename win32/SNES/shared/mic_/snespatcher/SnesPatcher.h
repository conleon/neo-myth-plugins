#ifndef SNESPATCHER_H
#define SNESPATCHER_H

#include <iostream>
#include <fstream>

class SnesPatcher;

static const int IO_ERROR = -1;

class SnesPatcher
{
public:
	virtual int Patch(const unsigned char* const *searchPatterns,
		      const unsigned char* const *replacementPatterns,
			  const int *patternLengths,
			  const char *smcFilename,
			  const char *outputFilename=NULL);

	//Required for SNES plugin
	virtual int Patch(const unsigned char* const *searchPatterns, 
					   const unsigned char* const *replacementPatterns,
			           const int *patternLengths,
			           char* fileBuffer,
			           const unsigned int fileSize);

 

private:
	static int PatternReplace(const unsigned char* const *searchPatterns,
		                      const unsigned char* const *replacementPatterns,
							  const int *patternLengths,
							  char *dataToPatch,
							  int dataSize);

};

#endif
