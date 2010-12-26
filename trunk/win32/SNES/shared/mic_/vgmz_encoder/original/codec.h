#ifndef _CODEC_H_
#define _CODEC_H_

class Codec
{
public:
	Codec() {}
	
	virtual void passThrough(unsigned char c)=0;// Write data to output stream unencoded.
	virtual void write(unsigned char c)=0;		// Encode a VGM command and write to output stream.
	virtual void flush()=0;						// Flush any encoded data to the output stream.
	virtual void *get(int g)=0;
	virtual void arg(unsigned char c)=0;		// Used for sending the argument byte(s) for the previous VGM command
												// sent to the encoder.

	Codec *altCodec;
};

#endif

