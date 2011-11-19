/*
	By conleon1988@gmail.com for madmonkey's opensource programmer
	Complete version of the plugin : http://code.google.com/p/neo-myth-plugins/
*/

#include "simple_snes_plug.hpp"

int main()
{
	const char* cs_filename = "NOTHING";
	const bool cb_auto_remove_header = true;//you definitely want this ENABLED
	NeoResultBlock* p_res;
	FILE* p_fp;
	
	printf("Reading %s\n",cs_filename);
	p_fp = fopen(cs_filename,"rb");

	if(p_fp != 0)
	{
		p_res = new NeoResultBlock;
		memset(p_res,0,sizeof(NeoResultBlock));
		fseek(p_fp,0,SEEK_END);
		p_res->romBufferLength = ftell(p_fp);
		fseek(p_fp,0,SEEK_SET);
		p_res->romBuffer = new u8[p_res->romBufferLength];
		fread(p_res->romBuffer,1,p_res->romBufferLength,p_fp);
		fclose(p_fp);
		printf("Buffer size %u\n",p_res->romBufferLength);

		printf("Detecting configuration..\n");
		if(snes_plug_detect(p_res,cb_auto_remove_header))
		{
			printf("Detection was successful!\n");
			printf("Rom size mapped value : %d\n",p_res->romSizeDetected);
			printf("Ram size mapped value : %d\n",p_res->ramSizeDetected);
			printf("Rom mapper mapped value : %d\n",p_res->romMapping);
			printf("Rom format mapped value : %d\n",p_res->romFormat);
			printf("Save type mapped value : %d\n",p_res->saveType);
		}
		else
			printf("Detection failed!\n");

		delete p_res->romBuffer;
		delete p_res;
	}
	else 
		printf("Failed to open %s\n",cs_filename);

	return 0;
}

