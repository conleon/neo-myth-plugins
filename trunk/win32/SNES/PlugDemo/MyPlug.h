// MyPlug.h: interface for the CMyPlug class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYPLUG_H__2D257F93_5EE6_4246_AA0A_CD5F5CFB4566__INCLUDED_)
#define AFX_MYPLUG_H__2D257F93_5EE6_4246_AA0A_CD5F5CFB4566__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include "resource.h"
#include "../PlugBase.h"


class CMyPlug : public CPlugBase  
{
public:
	CMyPlug(){};

	virtual ~CMyPlug(){};

public:

	bool  Patching(void* pBlock);
void  Release();

private:
};

#endif // !defined(AFX_MYPLUG_H__2D257F93_5EE6_4246_AA0A_CD5F5CFB4566__INCLUDED_)
