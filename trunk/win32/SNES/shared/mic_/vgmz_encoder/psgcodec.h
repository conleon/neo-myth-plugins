/*
 * A VGM compressor focusing mainly on PSG commands (0x50 0xnn).
 * Each group of 8 commands is prepended with a flag byte, where each bit specifies
 * if the corresponding command is a PSG command or not. The command byte (0x50) is stripped
 * and only the argument byte is written to the output.
 *
 * The compressor also tries to shorten long wait commands (0x61 0xmm 0xnn) down to one byte.
 * A table with 16 entries is filled with distinct wait lengths (0xnnmm) as they are found in
 * the VGM. A long wait command for which the length is found in the table is replaced by the
 * byte 0x9n, where n is the position in the table.
 * The table is stored in the output as a data block, right after the VGM header (i.e. offset 0x40).
 *
 * Mic, 2010
 */

#ifndef _PSGCODEC_H_
#define _PSGCODEC_H_

#include "codec.h"
#include <vector>

using namespace std;

class PsgCodec : public Codec
{
public:
	PsgCodec() { }
	
	// Write the output data to a char vector
	PsgCodec(vector<unsigned char> *data)
	{
		outData = data;
		flags = 0;
		numCmds = 0;
		valuesInLut = 0;
		awaitingLongWaitPeriod = 0;
	}
	
	static enum {
		LONG_WAIT_LUT
	};

	void passThrough(unsigned char c) { outData->push_back(c); }	
	void write(unsigned char c);
	void flush();			
	void *get(int g) { if (g == LONG_WAIT_LUT) return &longWaitLut; else return NULL; }

	void arg(unsigned char c);

private:
	unsigned char flags;
	int numCmds;
	unsigned short longWaitLut[16];
	int valuesInLut;
	int awaitingLongWaitPeriod;
	unsigned short longWaitPeriod;
	vector<unsigned char> *outData;
	vector<unsigned char> tempData;
};


#endif