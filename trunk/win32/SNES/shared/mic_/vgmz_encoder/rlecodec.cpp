// RLE codec implementation for vgmpack
// Mic, 2009

#include "rlecodec.h"


void RleCodec::rlePutc(unsigned char c)
{
	if (mode == RLE_MODE_FILE)
	{
		fputc(c, outFile);
	}
	else
	{
		outData->push_back(c);
	}
}


void RleCodec::passThrough(unsigned char c)
{
	rlePutc(c);
}


void RleCodec::writeHelper(bool forceWrite)
{
	char *p = NULL;
	int j, len, lenp;

	if ((run > 2) && (run == cmd8n.size()))
	{
		rlePutc(0xA0 | (cmd8n[0] & 0x0F));
		rlePutc(run - 1);
		cmd8n.clear();
		run = 0;
	}
	else
	{
		len = (int)cmd8n.size();
		if (run > 2)
		{
			len -= run;
		}
		if (len >= 6)
		{
			lenp = (len / 2) + (len & 1) + 2;
			p = new char[lenp];
			p[0] = 0x4E;
			p[1] = lenp - 2;
			j = 2;
			for (int i = 0; i < (len / 2) * 2; i += 2)
			{
				p[j++] = (cmd8n[i] & 0x0F) | ((cmd8n[i + 1] & 0x0F) << 4);
			}
			if (len & 1)
			{
				p[j] = cmd8n[len - 1];
			}
			if ((p[1] >= 254) || forceWrite)
			{
				if (mode == RLE_MODE_FILE)
				{
					fwrite(p, 1, lenp, outFile);
				}
				else
				{
					for (int i = 0; i < lenp; i++)
					{
						outData->push_back(p[i]);
					}
				}
				if (run > 2)
				{
					rlePutc(0xA0 | (cmd8n[len] & 0x0F));
					rlePutc(run - 1);
				}
				cmd8n.clear();
				run = 0;
			}
			delete [] p;
		}
		else if ((len = (int)cmd8n.size()) > 0)
		{
			if (run > 2)
			{
				len -= run;
			}
			if (forceWrite)
			{
				for (int i = 0; i < len; i++)
				{
					rlePutc(cmd8n[i]);
				}
				if (run > 2)
				{
					rlePutc(0xA0 | (cmd8n[len] & 0x0F));
					rlePutc(run - 1);
				}
				cmd8n.clear();
				run = 0;
			}
		}
	}
}
			


void RleCodec::write(unsigned char c)
{
	if ((c >= 0x80) && (c <= 0x8F))
	{
		if ((cmd8n.size()) && (c == cmd8n.back()))
		{
			if (run == 256)
			{
				writeHelper(true);
			}
			cmd8n.push_back(c);
			run++;
		}
		else
		{
			writeHelper(false);
			cmd8n.push_back(c);
			run = 1;
		}
	}
	else
	{
		writeHelper(true);
		passThrough(c);
	}
}


void RleCodec::flush()
{
	writeHelper(true);
}
