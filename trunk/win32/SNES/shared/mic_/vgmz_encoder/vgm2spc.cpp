// vgm2spc
// Mic, 2010

#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <time.h>
#include "..\\..\\..\\ext_libs\\zlib.h"
#include "psgcodec.h"
#include "nullcodec.h"
#include "rlecodec.h"
#include "vgm2spc_snes_plug_interface.hpp"
using namespace std;

namespace SNESPLUGIN_SHARED
{
	static std::string pluginDirectory = "";
};

int *destLen_;
unsigned char *outBuffer_;
FILE *outFile_;

void pack(const char *inFileName, char *outFileName, int *destLen, int mode);

void encoderSetPluginDirectory(const std::string& plugDir)
{
	SNESPLUGIN_SHARED::pluginDirectory = plugDir;
}

bool encodeVgm(const char* filename,char* outputBuffer,int* destLen)
{
	*destLen = 0;
	pack(filename,outputBuffer,destLen,1);
	return (*destLen) > 0;
}

bool encodeVgz(const char* filename,char* outputBuffer,int* destLen)
{
	*destLen = 0;
	pack(filename,outputBuffer,destLen,5);
	return (*destLen) > 0;
}

inline const std::string& getPluginDirectory()
{
	return SNESPLUGIN_SHARED::pluginDirectory;
}

void set_output(FILE *fp, unsigned char *ob, int *dl)
{
	outFile_ = fp;
	outBuffer_ = ob;
	destLen_ = dl;
}

void wrap_fputc(int ch, FILE *fp)
{
	if (destLen_ == NULL)
	{
		fputc(ch, fp);
	}
	else
	{
		outBuffer_[(*destLen_)++] = (unsigned char)ch;
	}
}

void wrap_fprint(FILE *fp, char *str)
{
	if (destLen_ == NULL)
	{
		fprintf(fp, str);
	}
	else
	{
		for (int i = 0; i < strlen(str); i++)
		{
			outBuffer_[(*destLen_)++] = str[i];
		}
	}
}

void wrap_fwrite(char *str, int s, int len, FILE *fp)
{
	if (destLen_ == NULL)
	{
		fwrite(str, s, len, fp);
	}
	else
	{
		for (int i = 0; i < len; i++)
		{
			outBuffer_[(*destLen_)++] = str[i];
		}
	}
}


void write_and_pad(FILE *fp, char *str, int len)
{
	int sl = strlen(str);
	int wl = (sl <= len) ? sl : len;

	wrap_fwrite(str, 1, wl, fp);
	for (; wl < len; wl++) wrap_fputc(0, fp);
}

#include <windows.h>

#define E(M) MessageBox(NULL,M,"ERROR",MB_OK);

void pack(const char *inFileName, char *outFileName, int *destLen, int mode)
{
    unsigned int i, inSize, origInSize, vgmOffs, preOffs, loopOffs, newLoopOffs, progress,
		         extraDataOffs, lastPcmOffs, unzippedBytes, playerSize, rawOutput;
	FILE *inFile, *outFile, *playerFile;
	gzFile inFileZ;
	unsigned char c, ch3Mode = 0;
	unsigned char *vgmData, *preProcessedData, *outBuffer;
	vector<unsigned char> outData;
	vector<unsigned char> extraDataBlock;
	vector<unsigned char> unzippedData;
	unsigned short *longWaitLut;
	Codec *codec;
	bool endOfVgmData = false;
	bool vgz = false;
	static char game[32] = "Unknown", song[32] = "Unknown", artist[32] = "Unknown";
	static char dateDumped[11] = "01/01/1990";
	static const char vgmSignature[] = "Vgm ";
	static unsigned char unzipBuffer[16384];

	rawOutput = mode & 8;
	mode &= 7;

	if (mode & 4)
	{
		vgz = true;
		mode &= 3;
	}
	else
	{
		if ((inFileName[strlen(inFileName) - 1] == 'z') || (inFileName[strlen(inFileName) - 1] == 'Z'))
		{
			vgz = true;
		}
	}

	// Try to auto-detect .vgz files that have been named .vgm based on the first four bytes
	if (!vgz)
	{
		inFile = fopen( inFileName, "rb");
		if (inFile == NULL)
		{
			printf("Error: Failed to open %s\n", inFileName);
			exit(0);
		}
		for (i = 0; i < 4; i++)
		{
			if (getc(inFile) != vgmSignature[i])
			{
				vgz = true;
				break;
			}
		}
		fclose(inFile);
	}

	if (!vgz)
	{
		inFile = fopen( inFileName, "rb");
		if (inFile == NULL)
		{
			printf("Error: Failed to open %s\n", inFileName);
			E("1");
			exit(0);
		}
		fseek(inFile, 0, SEEK_END);
		inSize = origInSize = ftell(inFile);
		fseek(inFile, 0, SEEK_SET);

		if ((vgmData = new unsigned char[inSize]) == NULL)
		{
			E("2");
			puts("Error: Failed to allocate memory");
			return;
		}
		if ((preProcessedData = new unsigned char[inSize]) == NULL)
		{
			E("3");
			puts("Error: Failed to allocate memory");
			return;
		}
		if (fread(vgmData, 1, inSize, inFile) != inSize)
		{
			E("4");
			puts("Error: Failed to read VGM data");
			exit(0);
		}
		fclose(inFile);
	}
	else
	{
		puts("Unzipping file");
		inFileZ = gzopen(inFileName, "rb");
		if (inFileZ == NULL)
		{
			E("5");
			printf("Error: Failed to gzopen %s\n", inFileName);
			exit(0);
		}
		while (true)
		{
			unzippedBytes = gzread(inFileZ, unzipBuffer, 16384);
			if (unzippedBytes > 0)
			{
				for (i = 0; i < unzippedBytes; i++)
				{
					unzippedData.push_back(unzipBuffer[i]);
				}
			}
			else
			{
				break;
			}
		}
		gzclose(inFileZ);
		inSize = origInSize = (unsigned int)unzippedData.size();
		if ((vgmData = new unsigned char[inSize]) == NULL)
		{
			E("6");
			printf("Error: Failed to allocate memory (wanted %d bytes)\n", inSize);
			return;
		}
		for (i = 0; i < inSize; i++)
		{
			vgmData[i] = unzippedData[i];
		}
		unzippedData.clear();	
		preProcessedData = new unsigned char[inSize];
	}

	if (vgmData[8] < 0x10)
	{
		vgmOffs = 0x40;
	}
	else
	{
		vgmOffs = vgmData[0x34] +
				  (vgmData[0x35] << 8) +
				  (vgmData[0x36] << 16) +
				  (vgmData[0x37] << 24) + 0x34;
		if (vgmOffs < 0x40) vgmOffs = 0x40;
	}
	extraDataOffs = vgmOffs;

	loopOffs = vgmData[0x1C] +
		       (vgmData[0x1D] << 8) +
			   (vgmData[0x1E] << 16) +
			   (vgmData[0x1F] << 24) + 0x1C;
	newLoopOffs = loopOffs;

	vgmData[8] = 0x52;	// To identify the VGM as compressed

	printf("Packing %s:", inFileName);

	for (i = 0; i < vgmOffs; i++)
	{
		outData.push_back(vgmData[i]);
	}

	if (mode == 0)
	{
		codec = new NullCodec(&outData);
	}
	else if (mode == 1)
	{
		codec = new PsgCodec(&outData);
	}

	for (i = 0; i < vgmOffs; i++)
	{
		preProcessedData[i] = vgmData[i];
	}
	preOffs = vgmOffs;
	lastPcmOffs = 0xFFFFFFFF;

	// Run a pre-processing stage to remove redundant commands
	while (!endOfVgmData)
	{
		if (vgmOffs == loopOffs)
		{
			newLoopOffs = preOffs;
			//printf("[1] loop %x -> %x\n", loopOffs, newLoopOffs);
		}

		c = vgmData[vgmOffs++];

		switch (c)
		{
			case 0x52:
				c = vgmData[vgmOffs++];
				if (c == 0x27)
				{
					c = vgmData[vgmOffs++];
					if ((c >> 6) != ch3Mode)
					{
						ch3Mode = c >> 6;
						preProcessedData[preOffs++] = 0x52;
						preProcessedData[preOffs++] = 0x27;
						preProcessedData[preOffs++] = c;
					}
				}
				else if ((c == 0x25) || (c == 0x26))
				{
					vgmOffs++;
				}
				else
				{
					preProcessedData[preOffs++] = 0x52;
					preProcessedData[preOffs++] = c;
					preProcessedData[preOffs++] = vgmData[vgmOffs++];
				}
				break;

			case 0x51:
			case 0x53:
			case 0x54:
			case 0x61:
				preProcessedData[preOffs++] = c;
				preProcessedData[preOffs++] = vgmData[vgmOffs++];
				preProcessedData[preOffs++] = vgmData[vgmOffs++];
				break;

			case 0x66:
				preProcessedData[preOffs++] = c;
				endOfVgmData = true;
				break;

			case 0x67:
				if (vgmData[vgmOffs] == 0x66)
				{
					unsigned int dataSize = vgmData[vgmOffs+2];
					dataSize += vgmData[vgmOffs+3] << 8;
					dataSize += vgmData[vgmOffs+4] << 16;
					dataSize += vgmData[vgmOffs+5] << 24;
					preProcessedData[preOffs++] = c;
					for (i = 0; i < dataSize + 6; i++)
					{
						preProcessedData[preOffs++] = vgmData[vgmOffs++];
					}
				}
				else
				{
					printf("Illegal command: 0x67 0x%02x\n", vgmData[vgmOffs]);
					delete [] vgmData;
					delete [] preProcessedData;
					delete codec;
					exit(0);
				}
				break;

			case 0xE0:
				i = vgmData[vgmOffs++];
				i += vgmData[vgmOffs++] << 8;
				i += vgmData[vgmOffs++] << 16;
				i += vgmData[vgmOffs++] << 24;
				if ((mode == 0) || (i != 0))
				{
					preProcessedData[preOffs++] = c;
					preProcessedData[preOffs++] = i & 0xFF;
					preProcessedData[preOffs++] = (i >> 8) & 0xFF;
					preProcessedData[preOffs++] = (i >> 16) & 0xFF;
					preProcessedData[preOffs++] = (i >> 24) & 0xFF;
					lastPcmOffs = i;
				}
				else if (mode != 0)
				{
					preProcessedData[preOffs++] = 0x4C;
				}
				break;

			case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: case 0x87:
			case 0x88: case 0x89: case 0x8A: case 0x8B: case 0x8C: case 0x8D: case 0x8E: case 0x8F:
				c &= 0x0F;
				if ((vgmData[vgmOffs] & 0xF0) == 0x70)
				{
					while ((vgmData[vgmOffs] & 0xF0) == 0x70)
					{
						if ((c + (vgmData[vgmOffs] & 0x0F)) < 0x10)
						{
							c += vgmData[vgmOffs] & 0x0F;
							vgmOffs++;
						}
						else
						{
							break;
						}
					}
					preProcessedData[preOffs++] = 0x80 | c;
				}
				else if (preOffs > 0)
				{
					if ((preProcessedData[preOffs - 1] & 0xF0) == 0x70)
					{
						if ((c + (preProcessedData[preOffs - 1] & 0x0F)) < 0x10)
						{
							c += preProcessedData[preOffs - 1] & 0x0F;
						}
						preProcessedData[preOffs - 1] = 0x80 | c;
					}
					else
					{
						preProcessedData[preOffs++] = 0x80 | c;
					}
				}
				else
				{
					preProcessedData[preOffs++] = 0x80 | c;
				}
				break;

			default:
				preProcessedData[preOffs++] = c;
				break;

		}
	}
	while (vgmOffs < inSize)
	{
		preProcessedData[preOffs++] = vgmData[vgmOffs++];
	}

	loopOffs = newLoopOffs;
	vgmOffs = extraDataOffs;
	endOfVgmData = false;
	delete [] vgmData;
	vgmData = preProcessedData;
	inSize = preOffs;

	progress = 0;

	// Now do the encoding stage
	while (!endOfVgmData)
	{
		if (((vgmOffs * 10) / inSize) > progress)
		{
			progress++;
			printf("%c", 177);
		}

		if (vgmOffs == loopOffs)
		{
			codec->flush();
			newLoopOffs = (unsigned int)outData.size();
			//printf("[2] loop %x -> %x\n", loopOffs, newLoopOffs);
		}

		c = vgmData[vgmOffs++];

		codec->write(c);

		switch (c)
		{
			case 0x4F: case 0x30:
				codec->arg(vgmData[vgmOffs++]);
				break;
			case 0x50:
				codec->arg(vgmData[vgmOffs++]);
				break;

			case 0x51:
			case 0x52:
			case 0x53:
			case 0x54:
			case 0x61:
				codec->arg(vgmData[vgmOffs++]);
				codec->arg(vgmData[vgmOffs++]);
				break;

			case 0x66:
				codec->flush();
				endOfVgmData = true;
				break;

			case 0x67:
				if (vgmData[vgmOffs] == 0x66)
				{
					unsigned int dataSize = vgmData[vgmOffs+2];
					dataSize += vgmData[vgmOffs+3] << 8;
					dataSize += vgmData[vgmOffs+4] << 16;
					dataSize += vgmData[vgmOffs+5] << 24;
					for (i = 0; i < dataSize + 6; i++)
					{
						codec->arg(vgmData[vgmOffs++]);
					}
				}
				else
				{
					printf("Illegal command: 0x67 0x%02x\n", vgmData[vgmOffs]);
					delete [] vgmData;
					delete codec;
					exit(0);
				}
				break;

			case 0xE0:
				codec->arg(vgmData[vgmOffs++]);
				codec->arg(vgmData[vgmOffs++]);
				codec->arg(vgmData[vgmOffs++]);
				codec->arg(vgmData[vgmOffs++]);
				break;
		}
	}

	if (mode == 1)
	{
		extraDataBlock.push_back(0x67);
		extraDataBlock.push_back(0x66);
		extraDataBlock.push_back(0x02);
		extraDataBlock.push_back(0x20);
		extraDataBlock.push_back(0x00);
		extraDataBlock.push_back(0x00);
		extraDataBlock.push_back(0x00);
		longWaitLut = (unsigned short*)(codec->get(PsgCodec::LONG_WAIT_LUT));
		for (i = 0; i < 16; i++)
		{
			extraDataBlock.push_back((unsigned char)longWaitLut[i]);
			extraDataBlock.push_back((unsigned char)(longWaitLut[i] >> 8));
		}
	}


	delete codec;

	// EOF offset
	i = (unsigned int)(outData.size() + extraDataBlock.size() - 4);
	outData[4] = i & 0xFF;
	outData[5] = (i >> 8) & 0xFF;
	outData[6] = (i >> 16) & 0xFF;
	outData[7] = (i >> 24) & 0xFF;

	// Read rest of data, if any (GD3)
	while (vgmOffs < inSize)
	{
		outData.push_back(vgmData[vgmOffs++]);
	}

	// GD3 offset
	i = outData[0x14] +
		(outData[0x15] << 8) +
		(outData[0x16] << 16) +
		(outData[0x17] << 24);
	if (i)
	{
		i -= (origInSize - (outData.size() + extraDataBlock.size()));
		outData[0x14] = i & 0xFF;
		outData[0x15] = (i >> 8) & 0xFF;
		outData[0x16] = (i >> 16) & 0xFF;
		outData[0x17] = (i >> 24) & 0xFF;

		// Read song title, game title and author from the GD3 header and convert from
		// wide chars to char.
		int j,k;
		j = i + 0x14 + 0x0C - extraDataBlock.size();
		for (k = 0; k < 32; k++)
		{
			if (outData[j] == 0) break;
			song[k] = outData[j];
			j += 2;
		}
		if (k > 0 && k < 32) song[k] = 0;
		while (outData[j] != 0) j += 2;		// Skip the rest of the title
		j += 2;

		while (outData[j] != 0) j += 2;		// Skip japanese title
		j += 2;

		for (k = 0; k < 32; k++)
		{
			if (outData[j] == 0) break;
			game[k] = outData[j];
			j += 2;
		}
		if (k > 0 && k < 32) game[k] = 0;
		while (outData[j] != 0) j += 2;		// Skip the rest of the game title
		j += 2;
		while (outData[j] != 0) j += 2;		// Skip japanese game title
		j += 2;

		while (outData[j] != 0) j += 2;		// Skip system name
		j += 2;	
		while (outData[j] != 0) j += 2;		// Skip japanese system name
		j += 2;

		for (k = 0; k < 32; k++)
		{
			if (outData[j] == 0) break;
			artist[k] = outData[j];
			j += 2;
		}
		if (k > 0 && k < 32) artist[k] = 0;
	}

	// Loop offset
	if (loopOffs > 0x1C)
	{
		newLoopOffs += (unsigned int)extraDataBlock.size();
		newLoopOffs -= 0x1C;
		outData[0x1C] = newLoopOffs & 0xFF;
		outData[0x1D] = (newLoopOffs >> 8) & 0xFF;
		outData[0x1E] = (newLoopOffs >> 16) & 0xFF;
		outData[0x1F] = (newLoopOffs >> 24) & 0xFF;
	}

	while (progress < 10)
		progress++;

	printf("\nInput size: %d bytes, output size: %d bytes\n", origInSize, outData.size() + extraDataBlock.size());

	// Open the player binary
	const std::string playerBinPath = getPluginDirectory() + std::string("s-smp_player.bin");
	playerFile = fopen(playerBinPath.c_str(),"rb");

	if (playerFile == NULL)
	{
		E("7");	
		delete [] vgmData;
		outData.clear();
		printf("Error: unable to open %s\n", "s-smp_player.bin");
		return;
	}
	fseek(playerFile, 0, SEEK_END);
	playerSize = ftell(playerFile);
	fseek(playerFile, 0, SEEK_SET);

	if ((outData.size() + extraDataBlock.size()) > (0xFFC0 - playerSize))
	{
		char S[512];
		sprintf(S,"ERROR #8 - outData VEC = 0x%x , extraDataBlock VEC = 0x%x",outData.size(),extraDataBlock.size());
		E(S);
		printf("Error: the vgm data is too large to fit. the maximum size after packing is %d bytes\n", (0xFFC0 - playerSize));
		delete [] vgmData;
		outData.clear();
		fclose(playerFile);
		return;
	}

	if (destLen == NULL)
	{
		outFile = fopen(outFileName, "wb");
	}
	else
	{
		outBuffer = (unsigned char*)outFileName;
		outFile = (FILE*)1;
	}

	set_output(outFile, outBuffer, destLen);

	if (outFile == NULL)
	{
		E("9");
		delete [] vgmData;
		printf("Error: unable to open %s\n", outFileName);
	}
	else
	{
		int spcRamRemain = 0x10000;

		if (!rawOutput)
		{
			wrap_fprint(outFile, "SNES-SPC700 Sound File Data v0.30");
			wrap_fputc(26, outFile);
			wrap_fputc(26, outFile);
			wrap_fputc(26, outFile);	// 27
			wrap_fputc(30, outFile);

			// SPC registers
			wrap_fputc(0, outFile);	// PC low
			wrap_fputc(3, outFile);	// PC high
			for (i = 0x27; i < 0x2E; i++)
			{
				wrap_fputc(0, outFile);
			}

			// Write ID666 tag
			write_and_pad(outFile, song, 32);
			write_and_pad(outFile, game, 32);
			write_and_pad(outFile, "Unknown", 16);
			write_and_pad(outFile, "Created with VGM2SPC", 32);
			write_and_pad(outFile, dateDumped, 11);

			for (i = 0; i < 8; i++)	wrap_fputc(0, outFile);
			
			write_and_pad(outFile, artist, 32);
			
			for (i = 0; i < 47; i++) wrap_fputc(0, outFile);

			for (i = 0; spcRamRemain > 0; i++)
			{
				int ch = fgetc(playerFile);
				if (ch == EOF) break;
				if (i == 0xF0) ch = 0x0A;	// SPC_TEST = 0x0A
				wrap_fputc(ch, outFile);
				spcRamRemain--;
			}
		}
		fclose(playerFile);

		for (i = 0; i < extraDataOffs; i++)
		{
			wrap_fputc(outData[i], outFile);
			spcRamRemain--;
		}
		for (unsigned int j = 0; j < extraDataBlock.size(); j++)
		{
			wrap_fputc(extraDataBlock[j], outFile);
			spcRamRemain--;
		}
		for (; i < outData.size(); i++)
		{
			wrap_fputc(outData[i], outFile);
			spcRamRemain--;
		}
		
		if (!rawOutput)
		{
			while (spcRamRemain > 0)
			{
				wrap_fputc(0, outFile);
				spcRamRemain--;
			}

			// DSP registers
			for (i = 0; i < 0x80; i++)
			{
				if (i == 0x6C)
				{
					wrap_fputc(0x20, outFile);
				}
				else
				{
					wrap_fputc(0, outFile);
				}
			}

			for (i = 0; i < 0x80; i++)
			{
				wrap_fputc(0, outFile);
			}
		}

		if (destLen == NULL)
		{
			fclose(outFile);
		}
	}

	E("PASSED");
	delete [] vgmData;
	outData.clear();
}

/*
int main(int argc, char *argv[])
{
	int i, mode = 1;
	char *inFn = NULL, *outFn = NULL;

	puts("VGM to SPC Converter by Mic, 2010");

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if ((strcmp(argv[i], "-h")==0) ||
				(strcmp(argv[i], "-help")==0) ||
				(strcmp(argv[i], "-?")==0))
			{
				printf("Usage: vgm2spc <input> <output>\n");
				return 0;
			}
			else if (strcmp(argv[i], "-raw")==0)
			{
				mode |= 8;
			}
		}
		else if (inFn == NULL)
		{
			inFn = argv[i];
			if (inFn[0] == '\"')
			{
				inFn[strlen(inFn) - 1] = '\0';
				inFn++;
			}
		}
		else if (outFn == NULL)
		{
			outFn = argv[i];
			if (outFn[0] == '\"')
			{
				outFn[strlen(outFn) - 1] = '\0';
				outFn++;
			}
		}
	}

	if ((inFn == NULL) || (outFn == NULL))
	{
		printf("Usage: vgm2spc <input> <output>\n");
        return 0;
    }

	pack(inFn, outFn, NULL, mode);

	return 0;
}

*/