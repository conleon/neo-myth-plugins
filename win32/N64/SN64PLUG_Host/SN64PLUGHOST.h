// SN64PLUGHOST.h : main header file for the SN64PLUGHOST application
//

#if !defined(AFX_SN64PLUGHOST_H__B36CC98F_B42E_4F1B_AFA1_23F90C277EDB__INCLUDED_)
#define AFX_SN64PLUGHOST_H__B36CC98F_B42E_4F1B_AFA1_23F90C277EDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSN64PLUGHOSTApp:
// See SN64PLUGHOST.cpp for the implementation of this class
//

class CSN64PLUGHOSTApp : public CWinApp
{
public:
	CSN64PLUGHOSTApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSN64PLUGHOSTApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSN64PLUGHOSTApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SN64PLUGHOST_H__B36CC98F_B42E_4F1B_AFA1_23F90C277EDB__INCLUDED_)
