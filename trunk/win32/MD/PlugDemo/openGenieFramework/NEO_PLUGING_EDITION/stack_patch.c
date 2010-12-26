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

static StackPatchDescription* stackPatchList = NULL;
static int stackPatchListLength = 0;
static int stackPatchListIndexPointer = -1;
static int stackPatchListIterator = -1;
static unsigned char stackPatchListNeedsSorting = 0;

static int SET_FUNC_ATTR(hot) comparePtrs(const void* a , const void* b)
{
	StackPatchDescription* first = (StackPatchDescription*)a;
	StackPatchDescription* second = (StackPatchDescription*)b;

	//make sure slots have patch data! XXX Found a way to remove the extra 2 comparisons by sorting only the active elements XXX
	return (second->patchAddress < first->patchAddress);//&& (first->patchData) && (second->patchData);
}

INLINE FORCE_INLINE void sortStackPatchList()
{
	if(stackPatchListNeedsSorting)
	{
		writeToLogSimple("Stack patch list needs sorting ... Will sort the list now.\n");
		qsort(stackPatchList, stackPatchListIndexPointer+1 /*stackPatchListLength*/, sizeof(StackPatchDescription), comparePtrs);
		stackPatchListNeedsSorting = 0;

		//printf("%s , 0x%03X\n",(stackPatchList + 0)->patchData,(stackPatchList + 0)->patchAddress );
	}
}

INLINE FORCE_INLINE StackPatchDescription* getPatchPtr(int index)
{
	return (stackPatchList + index);
}

INLINE FORCE_INLINE StackPatchDescription* getNextPatchPtr()
{
	return (stackPatchList + (++stackPatchListIndexPointer));
}

INLINE FORCE_INLINE void handlePatchMemory()
{
	int i;

	if(!stackPatchList)
	{
		stackPatchList = xalloc(STACK_PATCH_ALLOC_BLOCK * sizeof(StackPatchDescription));
		stackPatchListLength = STACK_PATCH_ALLOC_BLOCK;
		stackPatchListIndexPointer = -1;
		stackPatchListNeedsSorting = 0;
		stackPatchListIterator = -1;

		if(!stackPatchList)
			return;

		writeToLog("Patch stack is empty...allocating %d byte-block [total = %d[real = %d]]\n",
				STACK_PATCH_ALLOC_BLOCK,stackPatchListLength,stackPatchListLength* sizeof(StackPatchDescription));

		for(i = 0; i < STACK_PATCH_ALLOC_BLOCK; i++ )
		{
			getPatchPtr(i)->patchData = NULL;	
			getPatchPtr(i)->patchAddress = 0;
		}
		
		return;
	}

	if(stackPatchListIndexPointer >= stackPatchListLength-1)
	{
		stackPatchList = xrealloc(stackPatchList  ,STACK_PATCH_ALLOC_BLOCK  * sizeof(StackPatchDescription));

		if(stackPatchList)
			stackPatchListLength += STACK_PATCH_ALLOC_BLOCK;

		writeToLog("Patch stack running out of memory...re-allocated a %d byte-block [total = %d[real = %d]]\n",
				STACK_PATCH_ALLOC_BLOCK,stackPatchListLength,stackPatchListLength* sizeof(StackPatchDescription));

		if(!stackPatchList)
			return;

		for(i = stackPatchListIndexPointer+1; i < stackPatchListLength; i++ )
		{
			getPatchPtr(i)->patchData = NULL;	
			getPatchPtr(i)->patchAddress = 0;
		}
	}
}

void SET_FUNC_ATTR(cold) debugDumpStackPatchList()
{
	int i;

	if(!stackPatchList)
		return;

	sortStackPatchList();

	for(i = 0; i < stackPatchListIndexPointer + 1;  i++)
		printf("%d) [%s]:[%d]:[0x%lld]\n",i+1,stackPatchList[i].patchData,stackPatchList[i].patchDataLength,stackPatchList[i].patchAddress);
}

void SET_FUNC_ATTR(cold) releaseStackPatchList()
{

	int i;

	writeToLogSimple("Clearing up stack patch\n");

	stackPatchListIndexPointer = -1;
	stackPatchListNeedsSorting = 0;
	stackPatchListIterator = -1;

	if(!stackPatchList)
		return;



	for(i = 0; i < stackPatchListLength; /*<-maybe used rewind by mistake , so check them all .stackPatchListIndexPointer + 1 */ i++)
	{
		StackPatchDescription* p = getPatchPtr(i);

		if(p)
		{
			if(p->patchData)
				free(p->patchData);
		}
	}

	free(stackPatchList);
	stackPatchList = NULL;
}

int SET_FUNC_ATTR(cold) getPatchDescriptionListLength()
{
	return stackPatchListIndexPointer + 1;
}

void SET_FUNC_ATTR(cold) stackPatchRewind()
{
	stackPatchListIterator = 0;
}

StackPatchDescription* SET_FUNC_ATTR(hot) getPatchDescription(int index)
{
	sortStackPatchList();

	return (stackPatchList + index);
}

StackPatchDescription* SET_FUNC_ATTR(hot) getPatchDescriptionStackTop()
{
	sortStackPatchList();

	++stackPatchListIterator;

	if(stackPatchListIterator > stackPatchListIndexPointer )
		return NULL;

	return (stackPatchList + stackPatchListIterator);
}

//user is responsible for this!
StackPatchDescription* SET_FUNC_ATTR(hot) addStackPatchManually()
{
	StackPatchDescription* patch;

	handlePatchMemory();

	patch = getNextPatchPtr();

	if(patch->patchData)//maybe used rewind by mistake ?
		free(patch->patchData);

	patch->patchData = NULL;
	patch->patchDataLength = 0;
	patch->patchAddress = 0;

	stackPatchListNeedsSorting = 1;
	stackPatchListIterator = -1;

	return patch;
}

StackPatchDescription* SET_FUNC_ATTR(hot) addStackPatch(const char* data , unsigned int dataLen , unsigned __int64 address)
{
	StackPatchDescription* patch;

	handlePatchMemory();

	patch = getNextPatchPtr();

	if(patch->patchData)//maybe used rewind by mistake ?
		free(patch->patchData);

	patch->patchData = strdup(data);
	patch->patchDataLength = dataLen;
	patch->patchAddress = address;

	stackPatchListNeedsSorting = 1;
	stackPatchListIterator = -1;

	return patch;
}
 
