/*
	Copyright (c) 2009 - 2010 conleon1988 (conleon1988@gmail.com)

	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.
*/

#include "opengenie.h"

static const char GG_CHARS[] = "AaBbCcDdEeFfGgHhJjKkLlMmNnPpRrSsTtVvWwXxYyZz0O1I2233445566778899";

GameGenieCode* SET_FUNC_ATTR(cold) createGameGenieCodeInterface(const char* gameName)
{
	GameGenieCode* gg = xalloc(sizeof(GameGenieCode));
	unsigned len;

	gg->address = 0;
	gg->data = 0;

	gg->gamePatchCode[0] = '\0';
	gg->gamePatchCode[GAME_GENIE_MAX_CODE_LENGTH - 1] = '\0';

	len = strlen(gameName)-1;

	if(len >GAME_GENIE_MAX_NAME_LENGTH - 1)
		len = GAME_GENIE_MAX_NAME_LENGTH - 1;

	memcpy(gg->gameName,gameName,len);
	gg->gameName[len] = '\0';
	gg->gameName[GAME_GENIE_MAX_NAME_LENGTH - 1] = '\0';

	return gg;
}

GameGenieCode* SET_FUNC_ATTR(cold) releaseGameGenieCodeInterface(GameGenieCode* gg)
{
	if(gg)
		free(gg);

	return NULL;
}

//slightly faster than strchr. in most cases there is no speed difference
INLINE FORCE_INLINE const char* getChrAddress(const char* buf,const char* c)
{
	for(;(*buf) && (*buf != *c); ++buf)
		;

	return buf;
}

//must be clean string!
unsigned char SET_FUNC_ATTR(hot) SET_FUNC_ATTR(returns_twice) decodeGenieCode(GameGenieCode* gg)
{
	int i = 0;
	REG int v = 0;

	const char* c;

	if(!gg)
		return RST_FAILED;

	gg->data = 0;
	gg->address = 0;

	for(i = 0; i < 8; ++i)
	{
		if(!(c = getChrAddress(GG_CHARS, (gg->gamePatchCode+i) ) ) )
		{
			dumpToConsole("WARNING : UNABLE TO DECODE CHEAT %s :%d\n",gg->gamePatchCode,i);

			gg->data = 0;
			gg->address = 0;

			return RST_FAILED;
		}

		v = ( c - GG_CHARS ) >> 1;

		switch(i)
		{
			case 0:
				gg->data |= (v << 3);
			break;

			case 1:
			{
				gg->data |= (v >> 2);
				gg->address |= (v & 3) << 14;
			}
			break;

			case 2:
				gg->address |= (v << 9);
			break;

			case 3:
				gg->address |= ((v & 0xF) << 20)  | ((v >> 4) << 8);
			break;

			case 4:
			{
				gg->data |= ((v & 1) << 12);
				gg->address |= ((v >> 1) << 16);
			}
			break;

			case 5:
				gg->data |= ((v & 1) << 15) | ((v >> 1) << 8);
			break;

			case 6:
			{
				gg->data |= ((v >> 3) << 13);
				gg->address |= ((v & 7) << 5);
			}
			break;

			case 7:
				gg->address |= v;
			break;
		}
	}

	return RST_SUCCESS;
}

unsigned char SET_FUNC_ATTR(hot) loadGameGenieCodeFromFile(GameGenieCode* gg,const char* cheatFile)
{
	return RST_SUCCESS;
}

//Decodes the cheat from a clean stream buffer!
unsigned char SET_FUNC_ATTR(hot) SET_FUNC_ATTR(returns_twice) loadGameGenieCodeFromStream(GameGenieCode* gg,const char* cheatStream)
{
	unsigned len = strlen(cheatStream) - 1;

	if(len < 8)
	{
		dumpToConsole("Invalid cheat length. Got [%d] , but expected [%d] or [%d] \n",len,8,11);

		return RST_FAILED;
	}

	if(len > 11)
	{
		dumpToConsole("Invalid cheat length. Got [%d] , but expected [%d] or [%d] \n",len,8,11);

		return RST_FAILED;
	}


	if( (len == 8) && (*(cheatStream + 4) == '-') )
	{
		//address : 4 chars
		//data : 4 chars
		//delim : '-'
		*(gg->gamePatchCode + 0) = *(cheatStream + 0);
		*(gg->gamePatchCode + 1) = *(cheatStream + 1);
		*(gg->gamePatchCode + 2) = *(cheatStream + 2);
		*(gg->gamePatchCode + 3) = *(cheatStream + 3);
		*(gg->gamePatchCode + 4) = *(cheatStream + 5);// 4 = '-'
		*(gg->gamePatchCode + 5) = *(cheatStream + 6);
		*(gg->gamePatchCode + 6) = *(cheatStream + 7);
		*(gg->gamePatchCode + 7) = *(cheatStream + 8);
		*(gg->gamePatchCode + 8) = '\0';

		return decodeGenieCode(gg);
	}
	else if( (len == 10) && (*(cheatStream + 6) == ':') )
	{
		//address : 6 chars
		//data : 4 chars
		//delim : ':'
		*(gg->gamePatchCode + 0) = *(cheatStream + 0);
		*(gg->gamePatchCode + 1) = *(cheatStream + 1);
		*(gg->gamePatchCode + 2) = *(cheatStream + 2);
		*(gg->gamePatchCode + 3) = *(cheatStream + 3);
		*(gg->gamePatchCode + 4) = *(cheatStream + 4);
		*(gg->gamePatchCode + 5) = *(cheatStream + 5);
		*(gg->gamePatchCode + 6) = *(cheatStream + 7);//#6 =':'
		*(gg->gamePatchCode + 7) = *(cheatStream + 8);
		*(gg->gamePatchCode + 8) = *(cheatStream + 9);
		*(gg->gamePatchCode + 9) = *(cheatStream + 10);
		*(gg->gamePatchCode + 10) = '\0';

		return RST_SUCCESS;//todo!!
	}
	else
	{
		dumpToConsole("Unsupported cheat format. [got s=%s [%d]]\n",cheatStream,len+1);
		return RST_FAILED;
	}

/*
	REG int len = 0;
	const char* p;

	for(p = cheatStream; *p; p++)
	{
		if(*p != '-')
			*(gg->gamePatchCode + (len++)) = *p;
	}

	if(len < GAME_GENIE_MAX_CODE_LENGTH-1)
		gg->gamePatchCode[len] = '\0';
	else
		gg->gamePatchCode[GAME_GENIE_MAX_CODE_LENGTH - 1 ] = '\0';

	decodeGenieCode(gg);
*/
	return RST_FAILED;
}

unsigned char applyGameGenieCode(RomInfo* rom , GameGenieCode* gg)
{
	if(!rom || !gg)
		return RST_FAILED;

	if(!rom->romData)
		return RST_FAILED;

	if(rom->mode == FMT_SCD)
	{
		#define ADD_SP_BYTE_NEW_SCOPE(__B__,__A__)\
		{\
			StackPatchDescription* sp = NULL;\
			sp = addStackPatchManually();\
			sp->patchAddress = __A__;\
			sp->patchDataLength = 1;\
			sp->patchData = (char*)xalloc(1);\
							\
			if(sp->patchData)\
				sp->patchData[0] = (char)__B__;\
		}

		ADD_SP_BYTE_NEW_SCOPE( (gg->data >> 8) , gg->address);

		ADD_SP_BYTE_NEW_SCOPE( ((gg->data & 0xFF)) , (gg->address+1));

		#undef ADD_SP_BYTE_NEW_SCOPE
	}
	else
	{
		if( !(*(rom->romData + gg->address)) || !(*(rom->romData + (gg->address+1))) )
			return RST_FAILED;

		*(rom->romData + (gg->address )) = (gg->data >> 8);
		*(rom->romData + gg->address + 1) = (gg->data & 0xFF);

	}

	return RST_SUCCESS;
}


