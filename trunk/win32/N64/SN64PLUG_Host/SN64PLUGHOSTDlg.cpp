/*
	Copyright (c) 2010 - 2011 conleon1988 (conleon1988@gmail.com)

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

#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "SN64PLUGHOST.h"
#include "SN64PLUGHOSTDlg.h"
#include "..\\PlugBase.h"
#include "..\\SN64PLUG_Core\\MyPlug.h"
#include "..\\SuperN64_Engine\\n64_image.h"
#include "..\\SN64PLUG_ScriptCompiler\\NEON64_PlugScriptCompiler.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CMyPlug* plugin = 0;
static HMODULE pluginModule = 0;
static std::string modulePath = "";
static std::string patchFile = "*";
static CSN64PLUGHOSTDlg* hostApp;
static CStatic* runtimeInfoText = 0;
static int fixCRC = 0;
static int dmpProc = 0;
static int replaceSRC = 0;
static NEON64_Region region = NEON64_REGION_DEFAULT;
static NEON64DynamicStatsBlock* dynaSBP = NULL;
static CNEON64_ScriptCompiler compiler;

std::string getModulePath();

template <class T>
inline std::string itoS(T t, std::ios_base & (*f)(std::ios_base&))
{
	std::ostringstream oss;
	oss << f << t;
		
	return oss.str();
}

static void syncThread();

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
// CSN64PLUGHOSTDlg dialog

CSN64PLUGHOSTDlg::CSN64PLUGHOSTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSN64PLUGHOSTDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSN64PLUGHOSTDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSN64PLUGHOSTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSN64PLUGHOSTDlg)
	DDX_Control(pDX, IDC_PROGRESS1, m_PBar);
	DDX_Control(pDX, IDC_LIST2, m_DynaList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSN64PLUGHOSTDlg, CDialog)
	//{{AFX_MSG_MAP(CSN64PLUGHOSTDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, OnPrintRomInfo)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_BN_CLICKED(IDC_CHECK2, OnCheck2)
	ON_BN_CLICKED(IDC_CHECK3, OnCheck3)
	ON_BN_CLICKED(IDC_CHECK4, OnCheck4)
	ON_BN_CLICKED(IDC_CHECK5, OnCheck5)
	ON_BN_CLICKED(IDC_CHECK6, OnCheck6)
	ON_BN_CLICKED(IDC_CHECK7, OnCheck7)
	ON_BN_CLICKED(IDC_CHECK8, OnCheck8)
	ON_BN_CLICKED(IDC_CHECK15, OnCheck15)
	ON_BN_CLICKED(IDC_CHECK13, OnCheck13)
	ON_BN_CLICKED(IDC_CHECK12, OnCheck12)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSN64PLUGHOSTDlg message handlers

BOOL CSN64PLUGHOSTDlg::OnInitDialog()
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
	
	modulePath = getModulePath();

	std::string plugPath = modulePath;
	plugPath += "PlugDemo.dll";

	pluginModule = LoadLibrary(plugPath.c_str());

	if (pluginModule == NULL)
	{
		MessageBox("Can't load PlugDemo.dll",":(",MB_OK);
		plugin = 0;
		pluginModule = 0;
		OnCancel();

		return FALSE;
	}

	PFN_Plug_CreateObject allocAddr = (PFN_Plug_CreateObject)GetProcAddress (pluginModule, "Plug_CreateObject");

	if (allocAddr == NULL)
	{
		MessageBox("Can't locate alloc address in PlugDemo.dll",":(",MB_OK);

		FreeLibrary(pluginModule);
		pluginModule = 0;
		plugin = 0;
		OnCancel();

		return FALSE;
	}

	if(!allocAddr((void**)(&plugin)))
	{
		MessageBox("Can't locate alloc address in PlugDemo.dll",":(",MB_OK);

		FreeLibrary(pluginModule);
		pluginModule = 0;
		plugin = 0;
		OnCancel();

		return FALSE;
	}

	hostApp = this;

	runtimeInfoText = new CStatic;
	runtimeInfoText->Create("SuperN64 Plugin task : Idle", WS_CHILD | WS_VISIBLE,
	          CRect(CPoint(20,25),CSize(500,18)), this, 4096);

	CFont *fnt = new CFont;

	if(!fnt)
	{
		MessageBox("createFONT() failed.",":(",MB_OK);
		
		OnCancel();

		return FALSE;
	}

	fnt->CreatePointFont(88, "Verdana");
	runtimeInfoText->SetFont(fnt);

	((CButton*)GetDlgItem(IDC_CHECK4))->SetCheck(1);
	region = NEON64_REGION_DEFAULT;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSN64PLUGHOSTDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSN64PLUGHOSTDlg::OnPaint() 
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
HCURSOR CSN64PLUGHOSTDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSN64PLUGHOSTDlg::OnCancel() 
{ 
	if(plugin)
		plugin->Release();

	if(pluginModule)
		FreeLibrary(pluginModule);

	if(runtimeInfoText)
	{
		if(runtimeInfoText->GetFont())
			delete runtimeInfoText->GetFont();

		runtimeInfoText->SetFont(NULL);

		delete runtimeInfoText;
	}

	runtimeInfoText = NULL;
	pluginModule = NULL;
	plugin = NULL;

	OnCancel();
}

void realTimeStats(NEON64DynamicStatsBlock* dsBlock)
{
	static std::string s = "/~/";

	if(!dsBlock)
		return;
	
	if(s == "/~/")
	{
		s = "";
		s.reserve(256);
	}

	dynaSBP = dsBlock;

	if(dsBlock->taskNameNeedsUpdate)
	{
		s = "SuperN64 Plugin task : ";
		s += dsBlock->currentTaskName;

		runtimeInfoText->SetWindowText(s.c_str());
		hostApp->m_DynaList.AddString(s.c_str());
	}

	if(dsBlock->romNameNeedsUpdate)
	{
		s = " ( ";
		s += dsBlock->romGoodName;
		s += " )";

		runtimeInfoText->SetWindowText(s.c_str());
		hostApp->m_DynaList.AddString(s.c_str());
	}
	
	if(dsBlock->progressNeedsUpdate)
	{
		if(dmpProc)
		{
			s = "NEODSB_WriteTaskProgress() -> [";
			s += itoS(dsBlock->progress,std::dec);
			s += "%]";

			hostApp->m_DynaList.AddString(s.c_str());
		}

		hostApp->m_PBar.SetPos(dsBlock->progress);
	}
}

void CSN64PLUGHOSTDlg::OnPrintRomInfo() 
{
	((CButton*)GetDlgItem(IDC_CHECK1))->EnableWindow(0);
	((CButton*)GetDlgItem(IDC_CHECK2))->EnableWindow(0);
	((CButton*)GetDlgItem(IDC_CHECK3))->EnableWindow(0);
	((CButton*)GetDlgItem(IDC_CHECK12))->EnableWindow(0);
	((CButton*)GetDlgItem(IDC_CHECK13))->EnableWindow(0);
	((CButton*)GetDlgItem(IDC_CHECK15))->EnableWindow(0);
	((CButton*)GetDlgItem(IDC_CHECK8))->EnableWindow(0);
	((CButton*)GetDlgItem(IDOK))->EnableWindow(0);
	((CButton*)GetDlgItem(IDC_CHECK4))->EnableWindow(0);
	((CButton*)GetDlgItem(IDC_BUTTON1))->EnableWindow(0);

	dynaSBP = NULL;
 	m_PBar.SetRange(0, 100);
	m_PBar.SetPos(0);
	m_DynaList.ResetContent();

	CFileDialog fOpenDlg(TRUE,
                         "*.*",
                         NULL,
                         OFN_FILEMUSTEXIST,
                         "N64 rom files (*.z64;*.n64;*.v64)|*.z64;*.n64;*.v64|");


	fOpenDlg.SetWindowText("Select rom");

	if(fOpenDlg.DoModal() == IDOK)
	{
		compiler.clear();

		compiler.compile("SOURCE_FILE",(LPCTSTR)fOpenDlg.GetPathName());
		compiler.compile("DESTINATION_FILE",std::string(modulePath + std::string("outputRom.bin")).c_str());

		if(patchFile[0] != '*')
			compiler.compile("IMPORT_SRC",patchFile.c_str());
			

		if(replaceSRC)
		{
			if(region != NEON64_REGION_DEFAULT)
				compiler.compile("FORCE_REGION",NEON64_REGION_STRING_TABLE[region]);

			if(fixCRC)
				compiler.compile("FIX_CRC","1");
		}


		NEON64ResultBlock block;

		block.actionScript = compiler.getCodePtr();
		block.actionScriptLength = compiler.getCodeLength();
		block.myDynaStatsCB =  realTimeStats;

		if(plugin->Patching(&block))
		{
			std::string r = "";

			if(fixCRC)
				r += "Patched CRC!\n";

			if(replaceSRC && region != NEON64_REGION_DEFAULT)
				r += "Patched REGION!\n";

			r += "Database tasks complete!\n";

			switch(block.saveType)
			{
				case NEON64_SAVE_NONE:
					r += "Save type : None\n";
				break;

				case NEON64_SAVE_CONTROLLERPACK:
					r += "Save type : Controller pack\n";
				break;

				case NEON64_SAVE_EEPROM_4K:
					r += "Save type : EEPROM 4K\n";
				break;

				case NEON64_SAVE_EEPROM_16K:
					r += "Save type : EEPROM 16K\n";
				break;

				case NEON64_SAVE_SRAM_32K:
					r += "Save type : SRAM 32K\n";
				break;

				case NEON64_SAVE_FLASHRAM_1M:
					r += "Save type : FLASHRAM\n";
				break;

				default:
					r += "Save type : Unknown\n";
				break;
			}

			switch(block.bootChip)
			{
				case NEON64_CIC_6101:
					r += "BOOTCHIP : 6101\n";
				break;

				case NEON64_CIC_6102:
					r += "BOOTCHIP : 6102\n";
				break;

				case NEON64_CIC_6103:
					r += "BOOTCHIP : 6103\n";
				break;

				case NEON64_CIC_6105:
					r += "BOOTCHIP : 6105\n";
				break;

				case NEON64_CIC_6106:
					r += "BOOTCHIP : 6106\n";
				break;

				default:
					r += "BOOTCHIP : Unknown\n";
				break;
			}

			if(dynaSBP)
			{
				if(dynaSBP->userData)
				{
					RomHeader* head = (RomHeader*)dynaSBP->userData;

					r += "\n\n***DYNAMIC ROM INFO***\n";
					r += "Manufacturer : 0x" + itoS(head->manufacturerID,std::hex);
					r += "\n";
					r += "Cart ID : ";
					r += head->cartID;
					r += "\n";
					r += "Country code : ";
					r += head->countryCode;
					r += "\n";
					r += "Crcs : ";
					r += "0x" + itoS(head->crc1,std::hex);
					r += " , 0x" + itoS(head->crc2,std::hex);
					r += "\nClock rate : 0x" + itoS(head->clockRate,std::hex);
					r += "\nVersion : 0x" + itoS(head->version,std::hex);
					r += "\nPC : 0x" + itoS(head->pc,std::hex);
					r += "\n";
				}

				if(dynaSBP->userData2)
				{
					r += "\n\n***MD5***\n [";
					r += (char*)dynaSBP->userData2;
					r += "]\n\n\n";
				}

				if(replaceSRC)
				{
					//length of N64 images is always multiple of 4
					static char buf[32768 + 1];

					const std::string s = modulePath + "outputRom.bin";

					FILE* src = fopen(s.c_str(),"rb");

					if(src)
					{
						FILE* dst = fopen((LPCTSTR)fOpenDlg.GetPathName(),"wb");

						if(dst)
						{
							runtimeInfoText->SetWindowText("Copying patched rom to SOURCE file");
							hostApp->m_DynaList.AddString("Copying patched rom to SOURCE file");

							fseek(src,0,SEEK_SET);
							fseek(src,0,SEEK_END);
							int blocks = ftell(src);
							const float fLen = (float)blocks;
							fseek(src,0,SEEK_SET);

							blocks /= 32768;

							int prg = 0;
							while(blocks--)
							{
								syncThread();
								fread(buf,1,32768,src);

								syncThread();
								fwrite(buf,1,32768,dst);

								prg += 32768;
								m_PBar.SetPos( (int)( ( (float)(prg) / fLen) * 100 ) );
							}

							fclose(dst);
						}

						fclose(src);
					}
				}

				dynaSBP = NULL;

			}

			r += "\n\nEND\n";

			MessageBox(r.c_str(),"SuperN64 plugin info",MB_OK);

		}
		else
			MessageBox("Failed loading rom!");
	}

	patchFile = "*";

	((CButton*)GetDlgItem(IDC_CHECK1))->EnableWindow(1);
	((CButton*)GetDlgItem(IDC_CHECK2))->EnableWindow(1);
	((CButton*)GetDlgItem(IDC_CHECK3))->EnableWindow(1);
	((CButton*)GetDlgItem(IDC_CHECK4))->EnableWindow(1);
	((CButton*)GetDlgItem(IDC_CHECK12))->EnableWindow(1);
	((CButton*)GetDlgItem(IDC_CHECK13))->EnableWindow(1);
	((CButton*)GetDlgItem(IDC_CHECK15))->EnableWindow(1);
	((CButton*)GetDlgItem(IDC_CHECK8))->EnableWindow(1);
	((CButton*)GetDlgItem(IDOK))->EnableWindow(1);
	((CButton*)GetDlgItem(IDC_BUTTON1))->EnableWindow(1);
}

std::string getModulePath()
{
	static char rst[ 1024 ];
	std::string s = std::string( rst, GetModuleFileName( NULL, rst, 1024 ) );

	int i = s.length();

	while(i-- > 0)
	{
		if(s[i]=='\\')
		{
			s = s.substr(0,i+1);

			s += "Plug_In\\N64\\";
			break;
		}
	}

	return s;
}

void CSN64PLUGHOSTDlg::OnCheck1() 
{
	if(!replaceSRC)
	{
		fixCRC = 0;
		((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(0);
		MessageBox("Please tick \"Replace source with patched rom\" checkbox first!","Warning",MB_OK);
		return;
	}

	fixCRC = !fixCRC;	
}

void CSN64PLUGHOSTDlg::OnCheck2() 
{
	dmpProc = !dmpProc;	
}

void CSN64PLUGHOSTDlg::OnCheck3() 
{
	replaceSRC = !replaceSRC;
}

void CSN64PLUGHOSTDlg::OnCheck4() 
{
	region = NEON64_REGION_DEFAULT;
	((CButton*)GetDlgItem(IDC_CHECK8))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK13))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK15))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK12))->SetCheck(0);
}

void CSN64PLUGHOSTDlg::OnCheck5() 
{

}

void CSN64PLUGHOSTDlg::OnCheck6() 
{

}

void CSN64PLUGHOSTDlg::OnCheck7() 
{

}

static void syncThread()
{
	static MSG dispatch;

	while (PeekMessage( &dispatch, NULL, 0, 0, PM_NOREMOVE))
	{
		if (!AfxGetThread()->PumpMessage()){}
	}
}



void CSN64PLUGHOSTDlg::OnCheck8() 
{
	if(!replaceSRC)
	{
		((CButton*)GetDlgItem(IDC_CHECK8))->SetCheck(0);
		MessageBox("Please tick \"Replace source with patched rom\" checkbox first!","Warning",MB_OK);
		return;
	}

	region = NEON64_REGION_EUROPE;
	((CButton*)GetDlgItem(IDC_CHECK4))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK13))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK15))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK12))->SetCheck(0);
}

void CSN64PLUGHOSTDlg::OnCheck15() 
{
	if(!replaceSRC)
	{
		((CButton*)GetDlgItem(IDC_CHECK15))->SetCheck(0);
		MessageBox("Please tick \"Replace source with patched rom\" checkbox first!","Warning",MB_OK);
		return;
	}

	region = NEON64_REGION_JAPAN;
	((CButton*)GetDlgItem(IDC_CHECK4))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK8))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK13))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK12))->SetCheck(0);
}

void CSN64PLUGHOSTDlg::OnCheck13() 
{
	if(!replaceSRC)
	{
		((CButton*)GetDlgItem(IDC_CHECK13))->SetCheck(0);
		MessageBox("Please tick \"Replace source with patched rom\" checkbox first!","Warning",MB_OK);
		return;
	}

	region = NEON64_REGION_USA;
	((CButton*)GetDlgItem(IDC_CHECK4))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK8))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK15))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK12))->SetCheck(0);
}

void CSN64PLUGHOSTDlg::OnCheck12() 
{
	if(!replaceSRC)
	{
		((CButton*)GetDlgItem(IDC_CHECK12))->SetCheck(0);
		MessageBox("Please tick \"Replace source with patched rom\" checkbox first!","Warning",MB_OK);
		return;
	}

	region = NEON64_REGION_MODE7;
	((CButton*)GetDlgItem(IDC_CHECK4))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK8))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK13))->SetCheck(0);
	((CButton*)GetDlgItem(IDC_CHECK15))->SetCheck(0);
}

void CSN64PLUGHOSTDlg::OnButton1() 
{
	if(!replaceSRC)
	{
		((CButton*)GetDlgItem(IDC_CHECK12))->SetCheck(0);
		MessageBox("Please tick \"Replace source with patched rom\" checkbox first!","Warning",MB_OK);
		return;
	}

	CFileDialog fOpenDlg(TRUE,
                         "*.*",
                         NULL,
                         OFN_FILEMUSTEXIST,
                         "N64 IPS/APS patch files (*.ips;*.aps)|*.ips;*.aps|");


	fOpenDlg.SetWindowText("Select patch");

	if(fOpenDlg.DoModal() == IDOK)
		patchFile = (LPCTSTR)fOpenDlg.GetPathName();
	else
		patchFile = "*";
	
}
