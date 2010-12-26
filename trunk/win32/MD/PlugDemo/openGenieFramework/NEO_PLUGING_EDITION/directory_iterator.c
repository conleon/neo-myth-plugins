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

#ifdef COMPILE_WITH_DIRECTORY_ITERATOR
#include <stdio.h>

static char*  directoryStack[DIRECTORY_STACK_SIZE];
static unsigned directoryStackLength = 0;
static unsigned directoryStackPointer = 0;
static unsigned directoryIndexPointer = 0;

static unsigned char (*myDirIteratorFileTypeFunc)(const char* filename) = NULL;

static void  SET_FUNC_ATTR(cold) popEntry()
{
	if(directoryStack[directoryStackPointer])
		free(directoryStack[directoryStackPointer]);

	directoryStack[directoryStackPointer] = NULL;

	--directoryStackPointer;
}

static void SET_FUNC_ATTR(hot) addEntry(const char* entry)
{
	//dumpToConsole("ADD %s\n",entry);
	if(directoryStackPointer >  DIRECTORY_STACK_SIZE - 1)
	{
		dumpToConsole("Dir iter out of memory.");
		return;
	}

	if(directoryStack[directoryStackPointer])
		free(directoryStack[directoryStackPointer]);

	directoryStack[directoryStackPointer++] = strdup(entry);
}

//not recursive ATM
static unsigned char SET_FUNC_ATTR(cold) scanDirectory(const char* directory)
{
	//chdir(directory);

	char ALLIGNEDBY(16) temp[MAX_PATH];
	temp[0]='\0' , temp[MAX_PATH-1]='\0';//memset(temp,'\0',MAX_PATH);

	unsigned dLen = strlen(directory) - 1;

	if(!myDirIteratorFileTypeFunc)
		myDirIteratorFileTypeFunc = getFileType;

	#if defined (UNIX_BUILD)
		DIR	*d = opendir(directory);

		if(!d)
		{
			dumpToConsole("error openning dir %s\n",temp );
			writeToLog("error openning dir %s\n",temp );

			return RST_FAILED;
		}

		struct dirent *dir;

		while( dir = readdir(d) )
		{
			if((dir->d_type != DT_DIR ))
			{
				unsigned char type = myDirIteratorFileTypeFunc(dir->d_name);

				if(type != FMT_UNKNOWN)
				{
					getcwd( temp, MAX_PATH );
					strcat(temp,"/");
					strcat(temp,directory);

					if(directory[dLen] != '/')
						strcat(temp,"/");

					strcat(temp,dir->d_name);
					temp[(strlen(temp))] = '\0';

					//temp[len+1]='\0';
					//snprintf( temp + len, MAX_PATH - len, "/%s", dir->d_name );
					addEntry(temp);
				}
			}
		}
		closedir( d );
	#elif defined (WINDOWS_BUILD)
		HANDLE hFind = INVALID_HANDLE_VALUE;
		WIN32_FIND_DATA ffd;

		strcat(temp,directory);

		if(directory[dLen] != '\\')
			strcat(temp,"\\");

		strcat(temp,"*.*");
		temp[(strlen(temp)+1)] = '\0';

		hFind = FindFirstFile(temp, &ffd);

		if (hFind == INVALID_HANDLE_VALUE)
		{
			dumpToConsole("error openning dir %s\n",temp );
			writeToLog("error openning dir %s\n",temp );

			FindClose(hFind);

			return RST_FAILED;
		}


		while (FindNextFile(hFind, &ffd))
		{
			if (! (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
			{
				unsigned char type = myDirIteratorFileTypeFunc(ffd.cFileName);

				if(type != FMT_UNKNOWN)
				{
				        getcwd( temp, MAX_PATH );
				        strcat(temp,"\\");
				        strcat(temp,directory);

				        if(directory[dLen] != '\\')
				            strcat(temp,"\\");

				        strcat(temp,ffd.cFileName);
				        temp[(strlen(temp))] = '\0';

				        //temp[len+1]='\0';
				        //snprintf( temp + len, MAX_PATH - len, "\\%s", dir->d_name );
				        addEntry(temp);
				}
			}
		}

		FindClose(hFind);
	#endif

	return RST_SUCCESS;
}

unsigned char SET_FUNC_ATTR(cold) dirIteratorInit(const char* dir)
{
	unsigned int i = 0;

	for(i = 0; i < DIRECTORY_STACK_SIZE; i++)
		directoryStack[i] = NULL;

	directoryIndexPointer = 0;
	directoryStackLength = 0;
	directoryStackPointer = 0;

	if(scanDirectory(dir) != RST_SUCCESS)
	{
		dirIteratorRelease();
		return RST_FAILED;
	}

	return RST_SUCCESS;
}

unsigned char SET_FUNC_ATTR(hot) dirIteratorGetStatus()
{
	if(directoryIndexPointer >= directoryStackPointer)
		return RST_FAILED;

	return RST_SUCCESS;
}

const char* SET_FUNC_ATTR(hot) dirIteratorGetEntry()
{
	//return directoryStack[directoryIndexPointer++];
	const char* e = directoryStack[directoryIndexPointer++];//safer
	return e;
}

void SET_FUNC_ATTR(cold) dirIteratorRewind()
{
	directoryIndexPointer = 0;
}

void SET_FUNC_ATTR(cold) dirIteratorRelease()
{
	unsigned i = 0;

	for(i = 0; i < DIRECTORY_STACK_SIZE; i++)
	{
		if(directoryStack[i])
			free(directoryStack[i]);

		directoryStack[i] = NULL;
	}

	directoryIndexPointer = 0;
	directoryStackLength = 0;
	directoryStackPointer = 0;

}

void SET_FUNC_ATTR(cold) dirIteratorSetFileScanCallback(unsigned char (*cb)(const char*))
{
	myDirIteratorFileTypeFunc = cb;
}

#endif


