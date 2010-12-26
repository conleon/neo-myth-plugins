// Null codec interface for vgmpack
// Mic, 2009

#ifndef _NULLCODEC_H_
#define _NULLCODEC_H_

#include "codec.h"
#include <vector>

using namespace std;

class NullCodec : public Codec
{
public:
	NullCodec() { }
	
	// Write the output data to a char vector
	NullCodec(vector<unsigned char> *data)
	{
		outData = data;
	}
	
	void passThrough(unsigned char c) { outData->push_back(c); }	
	void write(unsigned char c) { outData->push_back(c); }		
	void flush() {}						
	void *get(int g) { return NULL; }

	void arg(unsigned char c) { passThrough(c); }

private:
	vector<unsigned char> *outData;
};


#endif
