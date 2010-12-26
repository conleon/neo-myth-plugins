/*
	SNES region patcher implementation
	Mic, 2010
*/

#include "SnesRegionPatcher.h"

// Check for this
const unsigned char NTSC_PATTERN0[] = {0xAD, 0x3F, 0x21, 0x29, 0x10};				// lda $213f / and #$10
// Replace with this
const unsigned char NTSC_TO_PAL_PATTERN0[] = {0xAD, 0x3F, 0x21, 0xA9, 0x00};		// lda $213f / lda #0

const unsigned char NTSC_PATTERN1[] = {0xAF, 0x3F, 0x21, 0x00, 0x89, 0x10};			// lda.l $00213f / bit #$10
const unsigned char NTSC_TO_PAL_PATTERN1[] = {0xAF, 0x3F, 0x21, 0x00, 0xA9, 0x00};	// lda.l $00213f / lda #0

const unsigned char NTSC_PATTERN2[] = {0xAD, 0x3F, 0x21, 0x89, 0x10};				// lda $213f / bit #$10
const unsigned char NTSC_TO_PAL_PATTERN2[] = {0xAD, 0x3F, 0x21, 0xA9, 0x00};		// lda $213f / lda #0

const unsigned char NTSC_PATTERN3[] = {0xAF, 0x3F, 0x21, 0x00, 0x29, 0x10};			// lda.l $00213f / and #$10
const unsigned char NTSC_TO_PAL_PATTERN3[] = {0xAF, 0x3F, 0x21, 0x00, 0xA9, 0x00};	// lda.l $00213f / lda #0

const unsigned char PAL_PATTERN0[] = {0xAD, 0x3F, 0x21, 0x29, 0x10};				// lda $213f / and #$10
const unsigned char PAL_TO_NTSC_PATTERN0[] = {0xAD, 0x3F, 0x21, 0xA9, 0x10};		// lda $213f / lda #$10

const unsigned char PAL_PATTERN1[] = {0xAF, 0x3F, 0x21, 0x00, 0x89, 0x10};			// lda.l $00213f / bit #$10
const unsigned char PAL_TO_NTSC_PATTERN1[] = {0xAF, 0x3F, 0x21, 0x00, 0xA9, 0x00};	// lda.l $00213f / lda #$10

const unsigned char PAL_PATTERN2[] = {0xAD, 0x3F, 0x21, 0x89, 0x10};				// lda $213f / bit #$10
const unsigned char PAL_TO_NTSC_PATTERN2[] = {0xAD, 0x3F, 0x21, 0xA9, 0x00};		// lda $213f / lda #$10

const unsigned char PAL_PATTERN3[] = {0xAF, 0x3F, 0x21, 0x00, 0x29, 0x10};			// lda.l $00213f / and #$10
const unsigned char PAL_TO_NTSC_PATTERN3[] = {0xAF, 0x3F, 0x21, 0x00, 0xA9, 0x00};	// lda.l $00213f / lda #$10


const unsigned char* const NTSC_PATTERNS[] =
{
	NTSC_PATTERN0,
	NTSC_PATTERN1,
	NTSC_PATTERN2,
	NTSC_PATTERN3,
	NULL
};

const int NTSC_PATTERN_LENGTHS[] = {5, 6, 5, 6};

const unsigned char* const PAL_PATTERNS[] =
{
	PAL_PATTERN0,
	PAL_PATTERN1,
	PAL_PATTERN2,
	PAL_PATTERN3,
	NULL
};

const int PAL_PATTERN_LENGTHS[] = {5, 6, 5, 6};

const unsigned char* const *SEARCH_PATTERNS[2] =
{
	NTSC_PATTERNS,
	PAL_PATTERNS
};

const int* const PATTERN_LENGTHS[2] =
{
	NTSC_PATTERN_LENGTHS,
	PAL_PATTERN_LENGTHS
};

const unsigned char* const NTSC_TO_PAL_PATTERNS[] =
{
	NTSC_TO_PAL_PATTERN0,
	NTSC_TO_PAL_PATTERN1,
	NTSC_TO_PAL_PATTERN2,
	NTSC_TO_PAL_PATTERN3,
	NULL
};

const unsigned char* const PAL_TO_NTSC_PATTERNS[] =
{
	PAL_TO_NTSC_PATTERN0,
	PAL_TO_NTSC_PATTERN1,
	PAL_TO_NTSC_PATTERN2,
	PAL_TO_NTSC_PATTERN3,
	NULL
};

const unsigned char* const *REPLACEMENT_PATTERNS[2] =
{
	PAL_TO_NTSC_PATTERNS,
	NTSC_TO_PAL_PATTERNS
};



int SnesRegionPatcher::Patch(const int newRegion, const char *smcFilename, const char *outputFilename)
{
	int oldRegion = 1 - newRegion;

	return SnesPatcher::Patch(SEARCH_PATTERNS[oldRegion],
			                  REPLACEMENT_PATTERNS[newRegion],
				              PATTERN_LENGTHS[oldRegion],
				              smcFilename,
				              outputFilename);
}

//Required for SNES plugin
int SnesRegionPatcher::Patch(const int newRegion, char* fileBuffer, const unsigned int fileSize)
{
	int oldRegion = 1 - newRegion;

	return SnesPatcher::Patch(SEARCH_PATTERNS[oldRegion],
			                  REPLACEMENT_PATTERNS[newRegion],
				              PATTERN_LENGTHS[oldRegion],
				              fileBuffer,
				              fileSize);
}
