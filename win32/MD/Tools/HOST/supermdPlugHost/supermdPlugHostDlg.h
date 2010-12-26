// supermdPlugHostDlg.h : header file
//

#if !defined(AFX_SUPERMDPLUGHOSTDLG_H__08EC2D08_6E17_4565_9C5D_E9CD7DC98566__INCLUDED_)
#define AFX_SUPERMDPLUGHOSTDLG_H__08EC2D08_6E17_4565_9C5D_E9CD7DC98566__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSupermdPlugHostDlg dialog

class CSupermdPlugHostDlg : public CDialog
{
// Construction
public:
	CSupermdPlugHostDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSupermdPlugHostDlg)
	enum { IDD = IDD_SUPERMDPLUGHOST_DIALOG };
	CProgressCtrl	m_ProgressbarSmall;
	CProgressCtrl	m_ProgressbarBig;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSupermdPlugHostDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSupermdPlugHostDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnCheck1();
	afx_msg void OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void onMountDstDir();
	afx_msg void OnInfoButton();
	afx_msg void OnMountSrcDir();
	afx_msg void OnMountHtmlDir();
	afx_msg void OnEuropeClick();
	afx_msg void OnJapanClick();
	afx_msg void OnUSAClick();
	afx_msg void OnAsiaClick();
	afx_msg void OnFranceClick();
	afx_msg void OnBrazilClick();
	afx_msg void OnHongKongClick();
	afx_msg void OnJoypadClick();
	afx_msg void OnMouseClick();
	afx_msg void OnCDROMClick();
	afx_msg void OnTeamplayClick();
	afx_msg void On6buttonClick();
	afx_msg void OnKeyboardClick();
	afx_msg void OnRS232CClick();
	afx_msg void OnPrinterClick();
	afx_msg void OnChecksumClick();
	afx_msg void OnCommonClick();
	afx_msg void OnCopyrightClick();
	afx_msg void OnIPSClick();
	afx_msg void OnGGClick();
	afx_msg void OnBatchPatchClick();
	afx_msg void OnPrintDBSym();
	afx_msg void OnCopiedStreamVerify();
	afx_msg void OnCancelTask();
	afx_msg void OnCancel();
	afx_msg void OnSilentClick();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUPERMDPLUGHOSTDLG_H__08EC2D08_6E17_4565_9C5D_E9CD7DC98566__INCLUDED_)
