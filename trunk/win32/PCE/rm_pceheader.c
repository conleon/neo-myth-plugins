#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdint.h>

static uint32_t file_size(FILE* f);
static uint8_t* file_to_mem(FILE* f,uint32_t start,uint32_t size);
static void rm_task(const char* src,const char* dst);

int main(int argc,char** argv)
{
	const char* src;
	const char* dst;

	printf("PCE Header removal - conleon1988@gmail.com --27/12/2010\n");

	if(argc < 3)
	{
		printf("Usage : rm_pceheader sourcefile destfile\n");
		return 1;
	}

	src = *(argv + 1);
	dst = *(argv + 2);

	rm_task(src,dst);

	return 0;
}

inline int32_t has_header(uint32_t size)
{
	return (( size & (( 0x200 << 1) - 1) ) != 0);
}

static uint32_t file_size(FILE* f)
{
	uint32_t r;
	//uint32_t s;

	if(!f)
		return 0;

	//s = ftell(f);
	fseek(f,0,SEEK_SET);
	fseek(f,0,SEEK_END);
	r = ftell(f);
	//fseek(f,s,SEEK_SET);

	return r;
}

static uint8_t* file_to_mem(FILE* f,uint32_t start,uint32_t size)
{
	uint8_t* block;

	block = (uint8_t*)malloc(size + 2);
	fseek(f,start,SEEK_SET);
	fread(block,1,size,f);

	return block;
}

static void rm_task(const char* src,const char* dst)
{
	FILE* f;
	FILE* out;
	uint8_t* block;
	uint32_t fsize;

	printf("Processing %s -> %s\n",src,dst);

	f = fopen(src,"rb");

	if(!f)
	{
		printf("%s not found\n",src);
		return;
	}

	fsize = file_size(f);

	if(!fsize)
	{
		printf("%s is empty\n",src);
		fclose(f);
		return;
	}
	
	if(!has_header(fsize))
	{
		printf("%s does not contain header.Aborting\n",src);
		fclose(f);
		return;
	}
	
	printf("%s contains header!\n",src);

	out = fopen(dst,"wb");

	if(!out)
	{
		printf("Can't open %s\n",dst);
		fclose(f);
		return;
	}

	printf("Allocating block..\n");
	block = file_to_mem(f,0x200,fsize - 0x200); /*lazy :D*/

	if(!block)
	{
		printf("Out of memory\n");
		fclose(f);
		fclose(out);
		return;
	}
	
	printf("Writing %s..\n",dst);	
	fwrite(block,1,fsize - 0x200,out);

	printf("Job done!\n");
	free(block);
	fclose(f);
	fclose(out);
}


