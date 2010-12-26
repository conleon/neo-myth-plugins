// scd_ram_gen.h : main header file for the SCD_RAM_GEN application
//

#if !defined(AFX_SCD_RAM_GEN_H__CF94BCE1_18FC_43CE_AA3C_12084525F749__INCLUDED_)
#define AFX_SCD_RAM_GEN_H__CF94BCE1_18FC_43CE_AA3C_12084525F749__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CScd_ram_genApp:
// See scd_ram_gen.cpp for the implementation of this class
//

class CScd_ram_genApp : public CWinApp
{
public:
	CScd_ram_genApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScd_ram_genApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CScd_ram_genApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCD_RAM_GEN_H__CF94BCE1_18FC_43CE_AA3C_12084525F749__INCLUDED_)
