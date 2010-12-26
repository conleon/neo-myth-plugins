#include "SnesPatcher.h"

int SnesPatcher::PatternReplace(const unsigned char *const *searchPatterns,
									  const unsigned char *const *replacementPatterns,
									  const int *patternLengths,
									  char *dataToPatch,
									  int dataSize)
{
	int numPatches = 0;
	int *stillValid;
	int numPatterns;
	int dataPos;

	if ((searchPatterns != NULL) && (replacementPatterns != NULL))
	{
		for (numPatterns = 0; searchPatterns[numPatterns] != NULL; numPatterns++);
		stillValid = new int[numPatterns + 1];

		dataPos = 0;

		while (dataPos < dataSize)
		{
			stillValid[numPatterns] = numPatterns;
			for (int i = 0; i < numPatterns; i++)
			{
				stillValid[i] = 0;
			}
			while (stillValid[numPatterns] > 0)
			{
				for (int i = 0; i < numPatterns; i++)
				{
					if ((stillValid[i] >= 0) && (stillValid[numPatterns] > 0))
					{
						if (dataPos + stillValid[i] < dataSize)
						{
							if ((unsigned char)dataToPatch[dataPos + stillValid[i]] == (unsigned char)searchPatterns[i][stillValid[i]])
							{
								if ((++stillValid[i]) >= patternLengths[i])
								{
									// We've found a match
									for (int r = 0; r < patternLengths[i]; r++)
									{
										dataToPatch[dataPos + r] = replacementPatterns[i][r];
									}
									dataPos += patternLengths[i] - 1;
									stillValid[numPatterns] = 0;
									numPatches++;
									break;
								}
							}
							else
							{
								stillValid[i] = -1;
								stillValid[numPatterns]--;
							}
						}
						else
						{
							stillValid[i] = -1;
							stillValid[numPatterns]--;
						}
					}
				}
			}
			dataPos++;
		}

		delete [] stillValid;
	}

	return numPatches;
}


int SnesPatcher::Patch(const unsigned char* const *searchPatterns,
					   const unsigned char* const *replacementPatterns,
			           const int *patternLengths,
			           const char *smcFilename,
			           const char *outputFilename)
{
	std::ifstream smcFile;
	std::ifstream::pos_type fileSize;
	char *fileBuffer;
	int numPatches;

	if (outputFilename == NULL)
	{
		outputFilename = smcFilename;
	}

	smcFile.open(smcFilename, std::ios::in | std::ios::binary | std::ios::ate);
    if (smcFile.is_open())
    {
		fileSize = smcFile.tellg();
		fileBuffer = new char[fileSize];
		smcFile.seekg (0, std::ios::beg);
		smcFile.read(fileBuffer, fileSize);
		smcFile.close();
		numPatches = PatternReplace(searchPatterns,
			                        replacementPatterns,
									patternLengths,
									fileBuffer,
									fileSize);
		if ((numPatches > 0) || (strcmp(smcFilename, outputFilename) != 0))
		{
			std::ofstream outputFile(outputFilename, std::ios::out | std::ios::binary);
			outputFile.write(fileBuffer, fileSize);
			outputFile.close();
		}
		delete [] fileBuffer;
	}
	else
	{
		numPatches = IO_ERROR;
	}

	return numPatches;
}

//Required for SNES plugin
int SnesPatcher::Patch(const unsigned char* const *searchPatterns,
					   const unsigned char* const *replacementPatterns,
			           const int *patternLengths,
			           char* fileBuffer,
			           const unsigned int fileSize)
{
	return
	PatternReplace(searchPatterns,
			                        replacementPatterns,
									patternLengths,
									fileBuffer,
									fileSize);
}
