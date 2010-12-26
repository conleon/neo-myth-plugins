// scd_ram_genDlg.cpp : implementation file
//

#include "stdafx.h"
#include "scd_ram_gen.h"
#include "scd_ram_genDlg.h"
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int CHUNK_128K = 131072;
static const int RAM_CART_LEN = (16 * CHUNK_128K);

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScd_ram_genDlg dialog

CScd_ram_genDlg::CScd_ram_genDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScd_ram_genDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScd_ram_genDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CScd_ram_genDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScd_ram_genDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CScd_ram_genDlg, CDialog)
	//{{AFX_MSG_MAP(CScd_ram_genDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, OnSystem)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScd_ram_genDlg message handlers

BOOL CScd_ram_genDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CScd_ram_genDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CScd_ram_genDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CScd_ram_genDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

static void writePadBytes(FILE* f)
{
	long start = ftell(f);

	while(start< RAM_CART_LEN)
	{
		fputc(0xFF,f);
		fputc(0x04,f);

		start += 2;
	}
}

void CScd_ram_genDlg::OnOK() 
{
	FILE* f = fopen("SCD_SRAM.bin","wb");

	writePadBytes(f);

	fclose(f);

}

void CScd_ram_genDlg::OnSystem() 
{

	CFileDialog fOpenDlg(TRUE,
                         "*.*",
                         NULL,
                         OFN_FILEMUSTEXIST,
                         "SegaCD/MegaCD bios files (*.bin)|*.bin|");


	fOpenDlg.SetWindowText("Select rom");

	if(fOpenDlg.DoModal()==IDOK)
	{
		std::string tmp = ((LPCTSTR)fOpenDlg.GetPathName());

		if(tmp.length())
		{
			FILE* f = fopen(tmp.c_str(),"rb");

			fseek(f,0,SEEK_END);
			int len = ftell(f);
			fseek(f,0,SEEK_SET);

			if(len == CHUNK_128K)
			{
				char* p = new char[len];
				fread(p,1,len,f);
				fclose(f);
				
				FILE* out = fopen("SCD_System.bin","wb");

				fwrite(p,1,len,out);
				writePadBytes(out);
				fclose(out);
				delete []p;
			}else
				fclose(f);
		}
	}
}

void CScd_ram_genDlg::OnButton1() 
{
	OnButton1();
}
