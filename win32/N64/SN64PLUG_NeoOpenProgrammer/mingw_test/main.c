#include <stdio.h>
#include <stdlib.h>
#include "..\SN64PLUG_MM_Interface.h"
#include "..\SN64PLUG_MM_Interface.h"
#include "..\\..\\SuperN64_Engine\\utility.h"
#include <stdio.h>
#ifdef WINDOWS
    #include <direct.h>
    #define getDir _getcwd
#else
    #include <unistd.h>
    #define getDir getcwd
#endif


int main()
{
    char currPath[255 + 1];

    getDir(currPath);//must contain path separator at the end

    #ifdef WINDOWS
        UTIL_StringAppend(currPath,"\\\0");

        SN64PLUG_Init(currPath,"\\\0");
    #else
        UTIL_StringAppend(currPath,"/0");

        SN64PLUG_Init(currPath,"/0");
    #endif

    SN64PLUG_Begin();

    FILE* f = fopen("Castlevania (E) (M3) [!].z64","rb");

    fseek(f,0,SEEK_END);
    int len = ftell(f);
    fseek(f,0,SEEK_SET);

    char* buf = (char*)malloc(len + 1);
    fread(buf,1,len,f);
    fclose(f);

    int cic = 0 , saveType = 0 , crc[2] = {0,0};

    SN64PLUG_ProcessImage("Castlevania (E) (M3) [!].z64",buf,len);

    SN64PLUG_Get(&cic,SN64PLUG_OP_CIC);
    SN64PLUG_Get(&saveType,SN64PLUG_OP_SAVETYPE);
    SN64PLUG_Get(&crc,SN64PLUG_OP_CRC);

    printf("Rom : %s\n","Castlevania (E) (M3) [!].z64");
    printf("CIC = %d\n",cic);
    printf("CRCS = 0x%x 0x%x\n",crc[0],crc[1]);

    switch(saveType)
    {
        case NEON64_SAVE_CARD:
            printf("Save = card \n");
        break;

        case NEON64_SAVE_EEPROM_4K:
            printf("Save = EEP 4K \n");
        break;

        case NEON64_SAVE_EEPROM_16K:
            printf("Save = EEP16K \n");
        break;

        case NEON64_SAVE_SRAM_32K:
            printf("Save = SR32K \n");
        break;

        case NEON64_SAVE_FLASHRAM_1M:
            printf("Save = FRAM \n");
        break;

        case NEON64_SAVE_CONTROLLERPACK:
            printf("Save = CPACK \n");
        break;

        case NEON64_SAVE_NONE:
            printf("Save = NOSAVE \n");
        break;
    }

    f = fopen("output.z64","wb");
    fwrite(buf,1,len,f);
    fclose(f);

    SN64PLUG_End();

    free(buf);
    return 0;
}
