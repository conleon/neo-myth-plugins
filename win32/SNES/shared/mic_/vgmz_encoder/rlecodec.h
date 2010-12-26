// RLE codec interface for vgmpack
// Mic, 2009

#ifndef _RLECODEC_H_
#define _RLECODEC_H_

#include "codec.h"
#include <vector>

using namespace std;

// For mingw
#ifndef _MSC_VER
#define fopen_s(a,b,c) *(a)=fopen(b,c)
#endif

enum 
{
	RLE_MODE_FILE,
	RLE_MODE_MEM
};


class RleCodec : public Codec
{
public:
	~RleCodec() { if (altCodec) delete altCodec; }

	RleCodec() { run = 0; }
	
	// Write the output data to a file specified by a file name
	RleCodec(char *outFileName)
	{
		mode = RLE_MODE_FILE;
		outFile = fopen(outFileName, "wb");
		run = 0;
	}

	// Write the output data to a file specified by a file pointer
	RleCodec(FILE *fp)
	{
		mode = RLE_MODE_FILE;
		outFile = fp;
		run = 0;
	}

	// Write the output data to a char vector
	RleCodec(vector<unsigned char> *data)
	{
		mode = RLE_MODE_MEM;
		outData = data;
		run = 0;
	}

	
	void passThrough(unsigned char c);	// Write data to output stream unencoded
	void write(unsigned char c);		// Encode data and write to output stream
	void flush();						// Flush any encoded data to the output stream
	void *get(int g) { return NULL; }

	void arg(unsigned char c) { passThrough(c); }

private:
	void writeHelper(bool forceWrite);
    void rlePutc(unsigned char c);

	FILE *outFile;
	int run;
	int mode;
	vector<unsigned char> cmd8n;
	vector<unsigned char> *outData;
};


#endif
