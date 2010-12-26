// PlugDemo.h : main header file for the PLUGDEMO DLL
//

#if !defined(AFX_PLUGDEMO_H__45F67B2B_B715_4778_A39E_73586AD09BB8__INCLUDED_)
#define AFX_PLUGDEMO_H__45F67B2B_B715_4778_A39E_73586AD09BB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPlugDemoApp
// See PlugDemo.cpp for the implementation of this class
//

class CPlugDemoApp : public CWinApp
{
public:
	CPlugDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlugDemoApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPlugDemoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLUGDEMO_H__45F67B2B_B715_4778_A39E_73586AD09BB8__INCLUDED_)
