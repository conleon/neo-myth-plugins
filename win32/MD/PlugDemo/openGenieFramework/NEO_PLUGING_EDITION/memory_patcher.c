
#include "opengenie.h"

#define LIMIT_BUF_LEN(__L__,__M__) \
	if(__L__ > __M__)\
		__L__ = __M__;

#define CHECK_FOR_SCD(__A__,__M__)\
	if(rom->mode == FMT_SCD)\
	{\
		patchRomStreamAddress(rom,data,__A__,len,__M__);\
		return;\
	}

void SET_FUNC_ATTR(hot) patchRomAddress(RomInfo* rom,const char* data , unsigned address, unsigned len)
{
	memcpy((rom->romData + address) , data , len);
}

void SET_FUNC_ATTR(hot) patchRomStringGarbage(RomInfo* rom,unsigned startAddress,unsigned endAddress)
{
	memset((rom->romData + startAddress) , ' ',  (endAddress - startAddress) );
}

//for scd
void SET_FUNC_ATTR(hot) patchRomStreamAddress(RomInfo* rom,const char* data , unsigned address, unsigned len, unsigned maxLen)
{
	int delta;
	StackPatchDescription* sp = addStackPatchManually();

	if(rom->scdJumpLevel)
		sp->patchAddress = address + 0x10;//must do jump
	else
		sp->patchAddress = address;

	sp->patchDataLength = len;
	sp->patchData = (char*)xalloc(maxLen+1);

	if(sp->patchData)
		memcpy(sp->patchData,(rom->romData + (address)),len);

	delta = (int)(maxLen - len);

	if(delta)
		memset(sp->patchData + len,' ',delta);	

	memcpy(sp->patchData , data , len);

	sp->patchData[maxLen] = '\0';

}

void patchRomSystemName(RomInfo* rom , const char* data)
{
	unsigned len = (unsigned)strlen(data);

	LIMIT_BUF_LEN(len,0x10);
	CHECK_FOR_SCD(0X100,0x10);
	patchRomStringGarbage(rom,0x100,0x100 + 0x10);
	patchRomAddress(rom,data,0x100, len);
	memcpy(rom->system,data,len);

}

void patchRomCopyright(RomInfo* rom , const char* data)
{
	unsigned len = (unsigned)strlen(data);

	LIMIT_BUF_LEN(len,0x10);
	CHECK_FOR_SCD(0X110,0x10);
	patchRomStringGarbage(rom,0x110,0x110 + 0x10);
	patchRomAddress(rom,data,0x110, len );
	memcpy(rom->copyright,data,len);



}

void patchRomDomesticName(RomInfo* rom , const char* data)
{
	unsigned len = (unsigned)strlen(data);

	LIMIT_BUF_LEN(len,0x30);
	CHECK_FOR_SCD(0X120,0x30);
	patchRomStringGarbage(rom,0x120,0x120 + 0x30);
	patchRomAddress(rom,data,0x120, len);
	memcpy(rom->domesticName,data,len);


}

void patchRomOverseasName(RomInfo* rom , const char* data)
{
	unsigned len = (unsigned)strlen(data);

	LIMIT_BUF_LEN(len,0x30);
	CHECK_FOR_SCD(0X150,0x30);
	patchRomStringGarbage(rom,0x150,0x150 + 0x30);
	patchRomAddress(rom,data,0x150, len);
	memcpy(rom->overseasName,data,len);

}

void patchRomProductNumber(RomInfo* rom , const char* data)
{
	unsigned len = (unsigned)strlen(data);

	LIMIT_BUF_LEN(len,0x0e);
	CHECK_FOR_SCD(0X180,0x0e);
	patchRomStringGarbage(rom,0x180,0x180 + 0x0e);
	patchRomAddress(rom,data,0x180,  len );
	memcpy(rom->productNo,data,len);


}

void patchRomModemBrand(RomInfo* rom , const char* data)
{
	unsigned len = (unsigned)strlen(data);

	LIMIT_BUF_LEN(len,4);
	CHECK_FOR_SCD(0X1BC,4);
	patchRomStringGarbage(rom,0x1BC,0x1BC + 4);
	patchRomAddress(rom,data,0x1BC, len);

}

void patchRomModemVersion(RomInfo* rom , const char* data)
{
	unsigned len = (unsigned)strlen(data);
	unsigned addr = (0x1BC + 4);

	LIMIT_BUF_LEN(len,4);
	CHECK_FOR_SCD(addr,4);
	patchRomStringGarbage(rom,addr,addr + 4);
	patchRomAddress(rom,data,addr, len);

}

void patchRomMemo(RomInfo* rom , const char* data)
{
	unsigned len = (unsigned)strlen(data);

	LIMIT_BUF_LEN(len,0x28);
	CHECK_FOR_SCD(0X1c8,0x28);
	patchRomStringGarbage(rom,0x1c8,0x1c8 + 0x28);
	patchRomAddress(rom,data,0x1c8, len);
	memcpy(rom->memo,data,len);

}

void patchRomIODeviceDataS(RomInfo* rom , const char* data)
{
	unsigned len = (unsigned)strlen(data);

	LIMIT_BUF_LEN(len,0x10);
	CHECK_FOR_SCD(0X190,0x10);
	patchRomStringGarbage(rom,0x190,0x190 + 0x10);
	patchRomAddress(rom,data,0x190, len);

}

void patchRomCountryListS(RomInfo* rom , const char* data)
{
	unsigned len = (unsigned)strlen(data);

	LIMIT_BUF_LEN(len,0x10);
	CHECK_FOR_SCD(0X1f0,0x10);
	patchRomStringGarbage(rom,0x1f0,0x1f0 + 0x10);
	patchRomAddress(rom,data,0x1f0, len);

}

#undef CHECK_FOR_SCD
#undef LIMIT_BUF_LEN

void patchRomIODeviceData(RomInfo* rom , BitField* field)
{
	char ALLIGNEDBY(16) buf[0x10 + 1];
	unsigned short next = 0;
	register unsigned short i = 0;

	memset(buf,' ',0x10);

	for(i = 0; i < IODEVICE_COUNT; i++ )
	{
		if(next > (0x10))
			break;

		if(getBitFieldStatus(field,i) == BIT_ON)
			buf[next++] = IODEVICE_DATA[i];
	}

	buf[0x10] = '\0';
	patchRomIODeviceDataS(rom,buf);
}

void patchRomCountryList(RomInfo* rom , BitField* field)
{
	char ALLIGNEDBY(16) buf[0x10 + 1];
	unsigned short next = 0;
	register unsigned short i = 0;

	memset(buf,' ',0x10);

	for(i = 0; i < REGION_COUNT; i++ )
	{
		if(next > (0x10 ))
			break;

		if(getBitFieldStatus(field,i) == BIT_ON)
			buf[next++] = REGION_DATA[i];

	}

	buf[0x10] = '\0';
	patchRomCountryListS(rom,buf);
}




