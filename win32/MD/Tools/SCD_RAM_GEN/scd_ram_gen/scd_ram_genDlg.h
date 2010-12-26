// scd_ram_genDlg.h : header file
//

#if !defined(AFX_SCD_RAM_GENDLG_H__DE9B6FF5_AAEB_403C_A06F_82FB951A181D__INCLUDED_)
#define AFX_SCD_RAM_GENDLG_H__DE9B6FF5_AAEB_403C_A06F_82FB951A181D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CScd_ram_genDlg dialog

class CScd_ram_genDlg : public CDialog
{
// Construction
public:
	CScd_ram_genDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CScd_ram_genDlg)
	enum { IDD = IDD_SCD_RAM_GEN_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScd_ram_genDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CScd_ram_genDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnSystem();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCD_RAM_GENDLG_H__DE9B6FF5_AAEB_403C_A06F_82FB951A181D__INCLUDED_)
