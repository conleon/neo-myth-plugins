// SN64PLUGHOSTDlg.h : header file
//

#if !defined(AFX_SN64PLUGHOSTDLG_H__15046B2A_D89D_4865_BB88_A4097CDD502E__INCLUDED_)
#define AFX_SN64PLUGHOSTDLG_H__15046B2A_D89D_4865_BB88_A4097CDD502E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSN64PLUGHOSTDlg dialog

class CSN64PLUGHOSTDlg : public CDialog
{
// Construction
public:
	CSN64PLUGHOSTDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSN64PLUGHOSTDlg)
	enum { IDD = IDD_SN64PLUGHOST_DIALOG };
	CProgressCtrl	m_PBar;
	CProgressCtrl	m_Progress;
	CListBox	m_DynaList;
	CProgressCtrl	m_Progressbar;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSN64PLUGHOSTDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSN64PLUGHOSTDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnCancel();
	afx_msg void OnPrintRomInfo();
	afx_msg void OnCheck1();
	afx_msg void OnCheck2();
	afx_msg void OnCheck3();
	afx_msg void OnCheck4();
	afx_msg void OnCheck5();
	afx_msg void OnCheck6();
	afx_msg void OnCheck7();
	afx_msg void OnCheck8();
	afx_msg void OnCheck15();
	afx_msg void OnCheck13();
	afx_msg void OnCheck12();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SN64PLUGHOSTDLG_H__15046B2A_D89D_4865_BB88_A4097CDD502E__INCLUDED_)
