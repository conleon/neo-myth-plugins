// supermdPlugHost.h : main header file for the SUPERMDPLUGHOST application
//

#if !defined(AFX_SUPERMDPLUGHOST_H__19044A75_F2C5_4F80_A177_D557BD5DF77C__INCLUDED_)
#define AFX_SUPERMDPLUGHOST_H__19044A75_F2C5_4F80_A177_D557BD5DF77C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSupermdPlugHostApp:
// See supermdPlugHost.cpp for the implementation of this class
//

class CSupermdPlugHostApp : public CWinApp
{
public:
	CSupermdPlugHostApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSupermdPlugHostApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSupermdPlugHostApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUPERMDPLUGHOST_H__19044A75_F2C5_4F80_A177_D557BD5DF77C__INCLUDED_)
