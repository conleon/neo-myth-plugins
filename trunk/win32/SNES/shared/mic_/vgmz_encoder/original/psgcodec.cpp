#include "psgcodec.h"


void PsgCodec::flush()
{
	if (numCmds)
	{
		while (numCmds < 8) 
		{
			tempData.push_back(0x4E);		// Output "NOP" commands
			numCmds++;
		}
		outData->push_back(flags);
		for (unsigned int i = 0; i < tempData.size(); i++)
		{
			outData->push_back(tempData[i]);
		}
		flags = 0;
		numCmds = 0;
		tempData.clear();
	}
}



void PsgCodec::write(unsigned char c)
{
	if (numCmds == 8)
	{
		this->flush();
	}

	if (c == 0x50)							// Is it a PSG command?
	{
		flags |= (1 << numCmds);
	}
	else
	{
		if (c == 0x61)						// Is it a long wait command?
		{
			awaitingLongWaitPeriod = 2;
			longWaitPeriod = 0;
		}
		else
		{
			tempData.push_back(c);
		}
	}
	numCmds++;
}


void PsgCodec::arg(unsigned char c)
{
	int i;

	if (awaitingLongWaitPeriod > 0)
	{
		longWaitPeriod |= c << ((2 - awaitingLongWaitPeriod) * 8);
		if (--awaitingLongWaitPeriod == 0)
		{
			for (i = 0; i < valuesInLut; i++)
			{
				if (longWaitPeriod == longWaitLut[i])
				{
					break;
				}
			}
			if (i < valuesInLut)
			{
				tempData.push_back(0x90 | i);
			}
			else if (valuesInLut < 16)
			{
				tempData.push_back(0x90 | valuesInLut);
				longWaitLut[valuesInLut++] = longWaitPeriod;
			}
			else
			{
				// No match could be found in the table. Store the entire command uncompressed.
				tempData.push_back(0x61);
				tempData.push_back((unsigned char)longWaitPeriod);
				tempData.push_back((unsigned char)(longWaitPeriod >> 8));
			}
		}
	}
	else
	{
		tempData.push_back(c);
	}
}
