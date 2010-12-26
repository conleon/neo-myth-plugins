
//MUPENPLUS64 -> SuperN64 plugin exporter (c)2010 conleon1988@gmail.com.
//TASK : Parse & dump the contents of an ini file with unknown section.
//Language : ANSI C

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>

#define PATH_SEPARATOR "\\"
#define MUPENPLUS_DBFILE "mupen64plus.ini"
#define SUPERN64_EXPORT_PATH "export"

static char section[34];
static char token[512];
static char code[128];
static unsigned int tokenLen = 0;
static const char* result = NULL;
static FILE* stream = NULL;

static const char fileInfo[]=
{
    "#SuperN64 Plugin MUPENPLUS DB to SuperN64 plugin optimized database\n(C) 2010 conleon1988@gmail.com\n\0"
};

static void parseSectionName()
{
    tokenLen = 0;
    section[0] = '\0';

    if(code[0] == '[')
    {
        char* p = code;

        for(p = (p + 1); *p; p++)
        {
            if(*p == ']')
                break;

            token[tokenLen++] = *p;
        }

        token[tokenLen] = '\0';

        strcpy(section,token);
        section[33] = '\0';
    }
}

static void parseSymbols()
{
    tokenLen = 0;

    unsigned int laddr;

    static char buf[1024];

    strcpy(&buf[0],SUPERN64_EXPORT_PATH);
    strcat(buf,PATH_SEPARATOR);
    strcat(buf,section);
    strcat(buf,".sn64plug");

    FILE* f = fopen(buf,"wb");

    if(!f)
        return;

    while( (!feof(stream)))
    {
        laddr = ftell(stream);
        fgets(token,128,stream);

        if(token[0] == '[')
        {
            //rollback
            fseek(stream,laddr,SEEK_SET);

            //all done
            break;
        }

        //fwrite(fileInfo,1,sizeof(fileInfo),f);
        fwrite(token,1,strlen(token),f);
       // fwrite("\n",1,1,f);
    }

    fclose(f);

    tokenLen = 0;
    token[0] = '\0';
}

int main()
{
    memset(&code[0],'\0',128);

    stream = fopen(MUPENPLUS_DBFILE,"rb");

    while(!feof(stream))
    {
        result = fgets(code,128,stream);

        if(result)
        {
            printf("%s\n",code);

            if(code[0] == '[')
            {
                token[0] = '\0';

                parseSectionName();

                if(tokenLen)
                    parseSymbols();
            }
        }
    }

    fclose(stream);
    return 0;
}
