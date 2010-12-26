// supermdPlugHostDlg.cpp : implementation file
//

#include "stdafx.h"
#include "supermdPlugHost.h"
#include "supermdPlugHostDlg.h"
#include "..\\..\\plugBase.h"
#include "..\\..\\PlugDemo\\MyPlug.h"
#include "..\\..\\PlugDemo\\openGenieFramework\\NEO_PLUGING_EDITION\\opengenie.h"
#include <vector>
#include <stack>
#include <list>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

/*patch code*/
enum
{
	PATCH_CRC = 0,
	PATCH_COMMON,
	PATCH_COPYRIGHT,
	PATCH_IPS,
	PATCH_GG
};


static char smartBuffer[0x60 + 1];
static int badFilesFound = 0;
static int taskStatus = 0;
static unsigned char plugSilentMode = 0;
static unsigned char taskConfig[ 2 ] = {0 , 0};
static unsigned char countryConfig[ 6 ]  = {0,0,0,0,0,0};
static unsigned char deviceConfig[ IODEVICE_COUNT ] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
static unsigned char patchConfig[ 5 ] = {0,0,0,0,0};

static const std::string infoText = "Opengenie framework , SuperMD Plugin , SuperMD Host , SuperMD SCD SYSTEM/RAM generator & EEPROM hash generator:\n"
"Designed and coded by Conleon(conleon1988@gmail.com).\n"
"\nSpecial Thanks :\n"
"ChillyWilly(Joseph Fenton - CREATOR of new MD MYTH MENU(SD/FC)) for his suggestions(sram 68k patch etc)\n and bug reports\n"
"Dr.Neo(www.neoflash.com) & ShivenYu(NEOTEAM member) for the Plugin demo code & support.\n"
"\nAnd a huge thanks to all OLDSCHOOL SEGA community & emu authors\n for their wonderful contribution."
"\n\nBonus credits :\nMadmonkey(bug reports & dumps)\n"
"Mugenmidget( dumps & reports )\n"
"DJoen( dumps for the framework)\n"
"Keropi ( dumps & reports )\n"
"King of Chaos (PAR format info)\n"
"mic_ (VGM/VGZ packer/player)\n";

static std::string destinationDir = "";
static std::string sourceDir = "";
static std::string htmlDir = "";

//ChillyWilly's suggestion
static const std::string newRegionPatchCode[] = 
{
	"E",
	"J",
	"b1",
	"U",
	"b1b3",
	"ffffff"
};

static const std::string devicePatchCode[ IODEVICE_COUNT ] =
{
	"JOYPAD",
	"JOYPAD_6BTN",
	"KEYBOARD",
	"PRINTER",
	"CONTROL_BALL",
	"FLOPPY_DISK",
	"ACTIVATOR",
	"TEAMPLAY",
	"JOYSTICK",
	"RS232C",
	"TABLET",
	"PADDLE_CONTROLLER",
	"CDROM",
	"MEGA_MOUSE"
};

static const std::string supportedExtensions[] =
{
	".md",
	".bin",
	".gen",
	".smd",
	".sms",
	".32x"
};

static CStatic* runtimeInfoText = 0;
static CMyPlug* superMDPlugin = 0;
static RomInfo* sharedRom = 0;
static HMODULE superMDPluginModule = 0;


static void syncThread()
{
	static MSG dispatch;

	while (PeekMessage( &dispatch, NULL, 0, 0, PM_NOREMOVE))
	{
		if (!AfxGetThread()->PumpMessage()){}
	}
}

static void decStatusRef()
{
	--taskStatus;

	if(taskStatus<0)
		taskStatus = 0;
}

inline bool isPow2(const int n)
{
    return (n) && !(n & (n - 1));
}

inline int getLengthOfFile(const std::string& path)
{
	FILE* f = fopen(path.c_str(),"r");

	if(!f)
		return 0;

	fseek(f,0,SEEK_END);

	int length = ftell(f);

	fclose(f);

	return length;
}

inline bool fileExists(const std::string& filename)
{
	FILE* f = fopen(filename.c_str(),"r");

	if(!f)
		return false;

	fclose(f);

	return true;
}

inline bool directoryExists(const std::string& s)
{   
    return GetFileAttributes(s.c_str()) == FILE_ATTRIBUTE_DIRECTORY;  
}

inline bool isSupportedExtension(const std::string& in,std::string& suffix)
{
	suffix = "";

	std::string ext = in;
	strToLower(ext);

	for(int i = 0; i < 6; i++)
	{
		if(ext.rfind(supportedExtensions[i]) != std::string::npos)
		{
			suffix = supportedExtensions[i];
			return true;
		}
	}

	return false;
}

inline std::string getFilename(const std::string& filename)
{
	std::string result;

	int addr = filename.rfind(".");

	if(addr != std::string::npos)
		result = filename.substr(0,addr-1);

	return result;
}

inline std::string getExtensionFromPath(const std::string& path)
{
	std::string result;

	int addr = path.rfind(".");

	if(addr != std::string::npos)
		result = path.substr(addr,path.length());

	return result;
}

inline std::string stringReplace(const std::string& base,const std::string& s1,const std::string& s2)
{
	std::string result = base;

	int addr = base.rfind(s1);

	if(addr != std::string::npos)
		result.replace(addr,s1.length(),s2);

	return result;
}

inline bool makeDirectory(const std::string& path)
{
	if(directoryExists(path))
		return true;

	return (CreateDirectory(path.c_str(),NULL) == TRUE);
}

inline int findInString(const std::string& base,const std::string& s)
{
	return base.find(s);
}

inline int rfindInString(const std::string& base,const std::string& s)
{
	return base.rfind(s);
}

inline void splitStr(const std::string& base,const std::string& search,std::vector<std::string>& tokens)
{
	tokens.clear();

	int addr = 0;
	int ld = 0;

	std::string tok = "";

	while(addr++ < base.length())
	{
		if(base.find(search,addr) != std::string::npos)
		{
			tok = base.substr(ld,addr);

			if(!tok.empty())
				tokens.push_back(tok);

			addr += tok.length();
			ld = addr;
		}
	}
}

//XXX TODO REPLACE WITH PRELOADER
inline bool isValidFile(FILE* f,const std::string& suffix,bool& bypassSF)
{
	bypassSF = false;

	const int lda = ftell(f);

	fseek(f,0,SEEK_SET);
	fseek(f,0,SEEK_END);
	
	const int len = ftell(f);

	if(len < 0x200)
	{
		fseek(f,lda,SEEK_SET);
		return false;
	}

	fseek(f,0x100,SEEK_SET);
	fread(smartBuffer,1,0x60,f);
	smartBuffer[0x60] = '\0';

	bool result = false;

	
	if(suffix == ".smd")
	{
		if(strstr(smartBuffer,"MT137 0P"))
			result = false;
		else if(strstr(smartBuffer,"/ 1991 C.Y.X /"))
			result = false;
	}

	if(strstr(smartBuffer,"SEGA"))
		result = true;

	fseek(f,lda,SEEK_SET);

	if(result)
		bypassSF = true;

	return result;
}

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
// CSupermdPlugHostDlg dialog

CSupermdPlugHostDlg::CSupermdPlugHostDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSupermdPlugHostDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSupermdPlugHostDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSupermdPlugHostDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSupermdPlugHostDlg)
	DDX_Control(pDX, IDC_PROGRESS2, m_ProgressbarSmall);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressbarBig);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSupermdPlugHostDlg, CDialog)
	//{{AFX_MSG_MAP(CSupermdPlugHostDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, onMountDstDir)
	ON_BN_CLICKED(IDC_BUTTON5, OnInfoButton)
	ON_BN_CLICKED(IDC_BUTTON1, OnMountSrcDir)
	ON_BN_CLICKED(IDC_BUTTON4, OnMountHtmlDir)
	ON_BN_CLICKED(IDC_CHECK3, OnEuropeClick)
	ON_BN_CLICKED(IDC_CHECK4, OnJapanClick)
	ON_BN_CLICKED(IDC_CHECK5, OnUSAClick)
	ON_BN_CLICKED(IDC_CHECK6, OnAsiaClick)
	ON_BN_CLICKED(IDC_CHECK7, OnFranceClick)
	ON_BN_CLICKED(IDC_CHECK8, OnBrazilClick)
	ON_BN_CLICKED(IDC_CHECK10, OnJoypadClick)
	ON_BN_CLICKED(IDC_CHECK12, OnMouseClick)
	ON_BN_CLICKED(IDC_CHECK13, OnCDROMClick)
	ON_BN_CLICKED(IDC_CHECK16, OnTeamplayClick)
	ON_BN_CLICKED(IDC_CHECK11, On6buttonClick)
	ON_BN_CLICKED(IDC_CHECK14, OnKeyboardClick)
	ON_BN_CLICKED(IDC_CHECK15, OnRS232CClick)
	ON_BN_CLICKED(IDC_CHECK17, OnPrinterClick)
	ON_BN_CLICKED(IDC_CHECK18, OnChecksumClick)
	ON_BN_CLICKED(IDC_CHECK19, OnCommonClick)
	ON_BN_CLICKED(IDC_CHECK20, OnCopyrightClick)
	ON_BN_CLICKED(IDC_CHECK21, OnIPSClick)
	ON_BN_CLICKED(IDC_CHECK22, OnGGClick)
	ON_BN_CLICKED(IDC_BUTTON3, OnBatchPatchClick)
	ON_BN_CLICKED(IDC_CHECK1, OnPrintDBSym)
	ON_BN_CLICKED(IDC_CHECK2, OnCopiedStreamVerify)
	ON_BN_CLICKED(IDC_BUTTON6, OnCancelTask)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	ON_BN_CLICKED(IDC_CHECK9, OnSilentClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSupermdPlugHostDlg message handlers

BOOL CSupermdPlugHostDlg::OnInitDialog()
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

	//hardcoded info label
	runtimeInfoText = new CStatic;

	if(!runtimeInfoText)
	{
		MessageBox("createRTI() failed.",":(",MB_OK);

		OnCancel();

		return FALSE;
	}

	runtimeInfoText->Create("SuperMD Plugin task : Idle", WS_CHILD | WS_VISIBLE,
	          CRect(CPoint(21,275),CSize(500,18)), this, 4096);

	CFont *fnt = new CFont;

	if(!fnt)
	{
		MessageBox("createFONT() failed.",":(",MB_OK);
		
		OnCancel();

		return FALSE;
	}

	fnt->CreatePointFont(88, "Verdana");
	runtimeInfoText->SetFont(fnt);

	sharedRom = createRom();

	if(!sharedRom)
	{
		MessageBox("createRom() failed.",":(",MB_OK);

		sharedRom = 0;
		superMDPlugin = 0;
		OnCancel();

		return FALSE;
	}

	resetRom(sharedRom,RESET_FULL);

	superMDPluginModule = LoadLibrary ("Plug_in\\MD\\PlugDemo.dll");

	if (superMDPluginModule == NULL)
	{
		MessageBox("Can't load Plug_in\\MD\\PlugDemo.dll",":(",MB_OK);

		superMDPlugin = 0;
		OnCancel();

		return FALSE;
	}

	PFN_Plug_CreateObject allocAddr = (PFN_Plug_CreateObject)GetProcAddress (superMDPluginModule, "Plug_CreateObject");

	if (allocAddr == NULL)
	{
		MessageBox("Can't locate alloc address in Plug_in\\MD\\PlugDemo.dll",":(",MB_OK);

		FreeLibrary(superMDPluginModule);
		superMDPlugin = 0;
		OnCancel();

		return FALSE;
	}

	if(!allocAddr((void**)(&superMDPlugin)))
	{
		MessageBox("Can't locate alloc address in Plug_in\\MD\\PlugDemo.dll",":(",MB_OK);

		FreeLibrary(superMDPluginModule);
		superMDPlugin = 0;
		OnCancel();

		return FALSE;
	}

	//send sync packet

	NeoResultBlock sync;
	
	char syncCmd[] = "HsTp\0";

	sync.romBuffer = 0;
	sync.romBufferLength = 0;
	sync.arguments = syncCmd;

	superMDPlugin->Patching(&sync);


	m_ProgressbarBig.SetPos(-1);
	m_ProgressbarSmall.SetPos(-1);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSupermdPlugHostDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSupermdPlugHostDlg::OnPaint() 
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
HCURSOR CSupermdPlugHostDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSupermdPlugHostDlg::OnOK() 
{

}

int CALLBACK BrowseCB(HWND hwnd,UINT msg,LPARAM lp, LPARAM data)
{
      switch(msg)
      {
		case BFFM_INITIALIZED:
				if ( data != NULL )
				{
					  SendMessage( hwnd, BFFM_SETSELECTION, TRUE, data );
				}
				break;    
				
		case BFFM_SELCHANGED:
				{
					  char szDir[ MAX_PATH ];
					  if ( SHGetPathFromIDList( (LPITEMIDLIST)lp ,szDir ) != FALSE )
					  {
							SendMessage( hwnd, BFFM_SETSTATUSTEXT, 0,(LPARAM)szDir );
					  }
				}
				break;                                      
      }

      return 0;
}

BOOL BrowseForFolder(CString& strPath)
{
      BROWSEINFO bi = {0};
      bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
      bi.lpfn = BrowseCB;      
      bi.lParam = strPath.IsEmpty() ? NULL : (LPARAM)( strPath.GetBuffer( strPath.GetLength() ) );

      strPath.ReleaseBuffer();

      if ( LPITEMIDLIST il = SHBrowseForFolder( &bi ) )
      {
            SHGetPathFromIDList(il, strPath.GetBufferSetLength(MAX_PATH));
            strPath.ReleaseBuffer();

            return TRUE;
      }

      return FALSE;
}

/*MOUNT*/
void CSupermdPlugHostDlg::onMountDstDir() 
{
	CString shack = "";

	if(BrowseForFolder(shack))
	{
		destinationDir = (LPCTSTR)shack;
		if(destinationDir[destinationDir.length()] != '\\')
			destinationDir += "\\";

		std::string final = "Mounted DESTINATION directory :\n" + destinationDir + "\n";

		MessageBox(final.c_str(),"Information",MB_OK);
	}
	else
	{
		destinationDir = "";
		MessageBox("Unable to mount DESTINATION directory","Error",MB_OK);
	}
}

void CSupermdPlugHostDlg::OnMountSrcDir() 
{
	CString shack = "";

	if(BrowseForFolder(shack))
	{
		sourceDir = (LPCTSTR)shack;

		if(sourceDir[sourceDir.length()] != '\\')
			sourceDir += "\\";

		std::string final = "Mounted ROM SOURCE directory :\n" + sourceDir + "\n";

		MessageBox(final.c_str(),"Information",MB_OK);
	}
	else
	{
		sourceDir = "";
		MessageBox("Unable to mount ROM DESTINATION directory","Error",MB_OK);
	}
}

void CSupermdPlugHostDlg::OnMountHtmlDir() 
{
	CString shack = "";

	if(BrowseForFolder(shack))
	{
		htmlDir = (LPCTSTR)shack;

		if(htmlDir[htmlDir.length()]!= '\\')
			htmlDir += "\\";

		std::string final = "Mounted HTML-INFO DESTINATION directory :\n" + htmlDir + "\n";

		MessageBox(final.c_str(),"Information",MB_OK);
	}
	else
	{
		htmlDir = "";
		MessageBox("Unable to mount HTML-INFO DESTINATION directory","Error",MB_OK);
	}
}

void CSupermdPlugHostDlg::OnInfoButton() 
{
	MessageBox(infoText.c_str(),"SuperMD Plugin Credits",MB_OK);
}


/*Country config*/
void CSupermdPlugHostDlg::OnEuropeClick() 
{
	countryConfig[0] = !countryConfig[0];
}

void CSupermdPlugHostDlg::OnJapanClick() 
{
	countryConfig[1] = !countryConfig[1];
}

void CSupermdPlugHostDlg::OnUSAClick() 
{
	countryConfig[3] = !countryConfig[3];
}

void CSupermdPlugHostDlg::OnAsiaClick() 
{
	countryConfig[2] = !countryConfig[2];
}

void CSupermdPlugHostDlg::OnFranceClick() 
{
	countryConfig[5] = !countryConfig[5];
}

void CSupermdPlugHostDlg::OnBrazilClick() 
{
	countryConfig[4] = !countryConfig[4];
}

/*IODevice config*/
void CSupermdPlugHostDlg::OnJoypadClick() 
{
	deviceConfig[JOYPAD] = !deviceConfig[JOYPAD];
}

void CSupermdPlugHostDlg::OnMouseClick() 
{
	deviceConfig[MEGA_MOUSE] = !deviceConfig[MEGA_MOUSE];	
}

void CSupermdPlugHostDlg::OnCDROMClick() 
{
	deviceConfig[CDROM] = !deviceConfig[CDROM];	
}

void CSupermdPlugHostDlg::OnTeamplayClick() 
{
	deviceConfig[TEAMPLAY] = !deviceConfig[TEAMPLAY];	
}

void CSupermdPlugHostDlg::On6buttonClick() 
{
	deviceConfig[JOYPAD_6BTN] = !deviceConfig[JOYPAD_6BTN];	
}

void CSupermdPlugHostDlg::OnKeyboardClick() 
{
	deviceConfig[KEYBOARD] = !deviceConfig[KEYBOARD];	
}

void CSupermdPlugHostDlg::OnRS232CClick() 
{
	deviceConfig[RS232C] = !deviceConfig[RS232C];	
}

void CSupermdPlugHostDlg::OnPrinterClick() 
{
	deviceConfig[PRINTER] = !deviceConfig[PRINTER];	
}


/*PATCH config*/
void CSupermdPlugHostDlg::OnChecksumClick() 
{
	patchConfig[PATCH_CRC] = !patchConfig[PATCH_CRC];
}

void CSupermdPlugHostDlg::OnCommonClick() 
{
	patchConfig[PATCH_COMMON] = !patchConfig[PATCH_COMMON];	
}

void CSupermdPlugHostDlg::OnCopyrightClick() 
{
	patchConfig[PATCH_COPYRIGHT] = !patchConfig[PATCH_COPYRIGHT];		
}

void CSupermdPlugHostDlg::OnIPSClick() 
{
	patchConfig[PATCH_IPS] = !patchConfig[PATCH_IPS];		
}

void CSupermdPlugHostDlg::OnGGClick() 
{
	patchConfig[PATCH_GG] = !patchConfig[PATCH_GG];		
}

static void overrideRegionConfig(NeoResultBlock* block , RomInfo* out)
{
	if(block->romFormat == NEOMD_FMT_SCD_SRAM)
		return;
	else if(block->romFormat == NEOMD_FMT_SMS)
		return;

	std::string rpc = "";

	for(int i = 0; i < 6; i ++)
	{
		if(countryConfig[i])
			rpc += newRegionPatchCode[i];
	}


	patchRomCountryListS(out,rpc.c_str());
}

static void putConstantsToBuffer(std::string& args)
{
	args = "";
	int i;

	args += " -d \"";
	
	for(i = 0; i < IODEVICE_COUNT; i++)
	{
		if(deviceConfig[i])
		{
			args += devicePatchCode[i];
			args += ";";
		}
	}

	args += "\"";

	if(patchConfig[PATCH_COMMON])
		args += " -f \"1\"";
	else
		args += " -f \"0\"";

	if(patchConfig[PATCH_CRC])
		args += " -c \"1\"";
	else
		args += " -c \"0\"";

	if(patchConfig[PATCH_COPYRIGHT])
		args += " -h \"1\"";
	else
		args += " -h \"0\"";

	if(plugSilentMode)
		args += " -q \"1\"";
}

static void putDynaToBuffer(std::string& args,const std::string& masterDir , const std::string& entity, const std::string& suffix,const bool ips,const bool cheat,const bool bpSF)
{
	if(!bpSF)
	{
		const char* skipSuffDot = (suffix.c_str()) + 1;

		args += " -s \"";
		args += skipSuffDot;
		args += "\"";
	}
	else
		args += " -s \"bin\"";

	if(ips)
	{
		std::string patchFile = masterDir + stringReplace(entity,suffix,".ips");

		if(fileExists(patchFile))
		{
			args += " -p \"";
			args += patchFile;
			args += "\"";
		}
	}

	if(ips)
	{
		std::string cheatFile = masterDir + stringReplace(entity,suffix,".smdcht");

		if(fileExists(cheatFile))
		{
			args += " -g \"";
			args += cheatFile;
			args += "\"";
		}
	}
}

void CSupermdPlugHostDlg::OnPrintDBSym() 
{
	taskConfig[0] = !taskConfig[0];	
}

void CSupermdPlugHostDlg::OnCopiedStreamVerify() 
{
	taskConfig[1] = !taskConfig[1];
}

void CSupermdPlugHostDlg::OnCancelTask() 
{
	if(!taskStatus)
	{
		MessageBox("No running task found.","Task status");
		return;
	}

	MessageBox("Force-cancel task in progress...","Task status");
	taskStatus = 0;	
}

void CSupermdPlugHostDlg::OnCancel()
{
	while(taskStatus)
	{
		syncThread();
		decStatusRef();
	}

	if(runtimeInfoText)
		delete runtimeInfoText;
	
	if(sharedRom)
		releaseRom(sharedRom);

	if(superMDPlugin)
		superMDPlugin->Release();

	if(superMDPluginModule)
		FreeLibrary(superMDPluginModule);

	sharedRom = 0;
	superMDPluginModule = 0;
	superMDPlugin = 0;
	runtimeInfoText = 0;

	OnCancel();
}

static void getFSStats(std::string root,int& totalFiles,__int64& totalLength)
{
	taskStatus = 1;
	runtimeInfoText->SetWindowText("SuperMD Plugin task : Generating graph...");

	totalLength = 0;
	totalFiles = 0;

	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA ffd;

	std::stack<std::string> dir_stack;

	dir_stack.push(root);

	std::string s,tmp,suffix;

	while (!dir_stack.empty())
	{
		if(!taskStatus)
		{
			MessageBox(NULL,"Task cancelled!","Task status",MB_OK);
			break;
		}

		syncThread();

		root = dir_stack.top();

		tmp  = root + "\\*.*";

        dir_stack.pop();

        hFind = ::FindFirstFile(tmp.c_str(), &ffd);

        if (hFind == INVALID_HANDLE_VALUE)
            return;

		while (::FindNextFile(hFind, &ffd))
		{
			if( strstr(ffd.cFileName,"..") )
				continue;

			s = CString(ffd.cFileName);

            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				dir_stack.push(root + s +"\\");
				makeDirectory(destinationDir + s);

				if(!htmlDir.empty())
					makeDirectory(htmlDir + s);
			}
			else if(isSupportedExtension(s,suffix))
			{
				runtimeInfoText->SetWindowText((std::string("SuperMD Plugin task : Scanning :" + s)).c_str());

				totalLength += getLengthOfFile(root + s);
			}

			if(!taskStatus)
			{
				MessageBox(NULL,"Task cancelled!","Task status",MB_OK);
				break;
			}
		}

	}//while !dir empty

	::FindClose(hFind);
}

static void mainRootDirThreadCycle(CSupermdPlugHostDlg* host,const std::string& rootDir,const std::string& dstDir,const std::string& singleDir,const bool ips,const bool cheat,std::stack<std::string>& ds,const __int64 lengthOfAll)
{
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA ffd;

	std::string root = rootDir;
	std::string tmp = root + "\\*.*";

	hFind = ::FindFirstFile(tmp.c_str(), &ffd);

	if (hFind == INVALID_HANDLE_VALUE)
            return;

	NeoResultBlock block;
	block.romBuffer = 0;
	block.romBufferLength = 0;

	std::string sdir,file,baseArgs,dynaArgs,s,suffix;

	char* buf = 0;
	int len = 0;

	resetRom(sharedRom,RESET_FULL);
	putConstantsToBuffer(baseArgs);

	taskStatus = 1;

	FILE* f = NULL;
	FILE* out = NULL;

	host->m_ProgressbarSmall.SetPos(-1);

	bool bpSF = false;

	while (::FindNextFile(hFind, &ffd))
	{
		if(!taskStatus)
		{
			MessageBox(NULL,"Task cancelled!","Task status",MB_OK);
			break;
		}

		if( strstr(ffd.cFileName,"..") )
				continue;

			s = CString(ffd.cFileName);

            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				ds.push(root + s +"\\");
			else if(isSupportedExtension(s,suffix))
			{	
				char* buf = NULL;

				f = fopen((std::string(rootDir + s)).c_str(),"rb");

				if(!f)
					continue;

				int len = 0;

				fseek(f,0,SEEK_END);
				len = ftell(f);
				fseek(f,0,SEEK_SET);

				if(!len)
				{
					fclose(f);
					continue;
				}

				host->m_ProgressbarBig.SetPos( (host->m_ProgressbarBig.GetPos() + len) * 100/lengthOfAll );

				if((!isValidFile(f,suffix,bpSF)) )
				{
					++badFilesFound;
					fclose(f);
					continue;
				}

				buf = new char[len + 1];
				
				if(!buf)
				{
					fclose(f);
					continue;
				}

				if(fread(buf,1,len,f) != len)
				{
					fclose(f);
					delete buf;
					continue;
				}

				fclose(f);

				std::string outPath;
				
				if(suffix != ".bin")
					outPath = dstDir + stringReplace(s,suffix,".bin");
				else
					outPath = dstDir + s;

				//MessageBox(NULL,outPath.c_str()," ",MB_OK);
				dynaArgs = baseArgs;
				putDynaToBuffer(dynaArgs,sourceDir,s,suffix,ips,cheat,bpSF);

				runtimeInfoText->SetWindowText((std::string("SuperMD Plugin task : Burn :" + outPath)).c_str());

				block.arguments = const_cast<char*>(dynaArgs.c_str());
				block.romBuffer = buf;
				block.romBufferLength = len;

				if(!superMDPlugin->Patching(&block))
				{
					//MessageBox(NULL,"FAIL","FAIL",MB_OK);
					delete buf;
					continue;
				}

				if(taskConfig[0])
				{
					std::string str = "DEBUG SYMBOLS\n";
					str += "Rom = \n";
					str += s;
					str += "\nFormat = 0x";
					str += itoS<int>(block.romFormat,std::hex).c_str();
					str += "\nAlloc = ";
					str += itoS<int>(block.saveAllocationSize,std::hex).c_str();
					str += "\nArgs = \n ";
					str += block.arguments;

					MessageBox(NULL,str.c_str(),"Debug symbols",MB_OK);
				}

				mountRom(sharedRom,(unsigned char*)buf,len,FMT_BIN);
				updateRomInfo(sharedRom);

				overrideRegionConfig(&block,sharedRom);

				out = fopen(outPath.c_str(),"wb");

				if(!out)
				{
					delete buf;
					continue;
				}

				if(taskConfig[1])//1:1
				{
					for(int i = 0; i < len; i++)
					{
						syncThread();

						host->m_ProgressbarSmall.SetPos( i * 100/len );

						if(fputc(buf[i],out) != buf[i])
						{
							//error
						}
					}
					
				}
				else
				{
					for(int i = 0; i < len; i+= 2048)
					{
						syncThread();

						host->m_ProgressbarSmall.SetPos( i * 100/len );

						if(fwrite(buf+i,1,2048,out) != 2048)
						{

						}
					}
				}

				fclose(out);

				if(!htmlDir.empty() && (block.romFormat != NEOMD_FMT_SCD_SRAM) && (block.romFormat != NEOMD_FMT_SMS) )
				{
					runtimeInfoText->SetWindowText("SuperMD Plugin task : Generating html info");

					if(!singleDir.empty())
						htmlgenInit(std::string(htmlDir + singleDir + "\\" + stringReplace(s,suffix,".html")).c_str());
					else
						htmlgenInit(std::string(htmlDir + stringReplace(s,suffix,".html")).c_str());

					htmlgenWriteFrontPart(s.c_str());
					htmlgenWriteRom(sharedRom);
					htmlgenWriteBottomPart();
					htmlgenRelease();
				}

				unMountRom(sharedRom);

				delete buf;

			}

			if(!taskStatus)
			{
				MessageBox(NULL,"Task cancelled!","Task status",MB_OK);
				break;
			}
		}

	::FindClose(hFind);
}

/*CODE*/
void CSupermdPlugHostDlg::OnBatchPatchClick() 
{
	if(taskStatus)
	{
		MessageBox("Already running!!!!","Task status",MB_OK);
		return;
	}

	//check if any of IO devices are selected
	bool done = false;

	int i = 0;

	for(i = 0; i < IODEVICE_COUNT; i++)
	{
		if(deviceConfig[i])
		{
			done = true;
			break;
		}
	}

	if(!done)
	{
		MessageBox("Please select at least one DEVICE from the IO DEVICE list.","Info",MB_OK);
		return;
	}

	//now check region
	done = false;

	for(i = 0; i < REGION_COUNT; i++)
	{
		if(countryConfig[i])
		{
			done = true;
			break;
		}
	}

	if(!done)
	{
		MessageBox("Please select at least one COUNTRY from the REGION list.","Info",MB_OK);
		return;
	}

	if(sourceDir.empty())
	{
		MessageBox("Please provide ROM SOURCE directory to continue","Info",MB_OK);
		return;
	}

	if(destinationDir.empty())
	{
		MessageBox("Please provide ROM DESTINATION directory to continue","Info",MB_OK);
		return;
	}

	if(sourceDir == destinationDir)
		MessageBox("Warning : Destination dir matches Source dir. All roms will be overwritten.","Info",MB_OK);

	if(htmlDir.empty())
		MessageBox("Warning : HTML info directory not set. Information will not be generated.","Info",MB_OK);

	if(!plugSilentMode)
		MessageBox("Warning : SILENT MODE is not active.All errors will be reported.","Info",MB_OK);
		;
	m_ProgressbarSmall.SetPos(-1);
	m_ProgressbarBig.SetPos(-1);

	NeoResultBlock block;
	block.romBuffer = 0;
	block.romBufferLength = 0;

	std::string sdir,file,baseArgs,dynaArgs;

	FILE* f = NULL;
	char* buf = 0;
	int len = 0;

	resetRom(sharedRom,RESET_FULL);
	putConstantsToBuffer(baseArgs);

	//We need the old values to avoid changes at loop
	const bool imIPS = (bool)(patchConfig[PATCH_IPS] == 1);
	const bool imCheat = (bool)(patchConfig[PATCH_GG] == 1);

	taskStatus = 1;

	__int64 lengthOfAll = 0;
	int totalFiles = 0;

	getFSStats(sourceDir,totalFiles,lengthOfAll);

	// =======
	std::string root = sourceDir;

	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA ffd;

	std::stack<std::string> dir_stack;

	std::string s,tmp,suffix,dirNow;

	//run *this
	mainRootDirThreadCycle(this,sourceDir,destinationDir,"",imIPS,imCheat,dir_stack,lengthOfAll);

	dir_stack.push(root);

	badFilesFound = 0;

	m_ProgressbarBig.SetRange(0,lengthOfAll);

	memset(smartBuffer,'\0',0x60);

	while (!dir_stack.empty())
	{
		if(!taskStatus)
		{
			MessageBox("Task cancelled!","Task status",MB_OK);
			break;
		}

		syncThread();

		root = dir_stack.top();

		tmp  = root + "\\*.*";

        dir_stack.pop();

        hFind = ::FindFirstFile(tmp.c_str(), &ffd);

        if (hFind == INVALID_HANDLE_VALUE)
            return;

		while (::FindNextFile(hFind, &ffd))
		{
			if(!taskStatus)
			{
				MessageBox("Task cancelled!","Task status",MB_OK);
				break;
			}

			if( strstr(ffd.cFileName,"..") )
				continue;

			s = CString(ffd.cFileName);

            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				mainRootDirThreadCycle(this,root + s + "\\",destinationDir + s + "\\",s,imIPS,imCheat,dir_stack,lengthOfAll);

			if(!taskStatus)
			{
				MessageBox("Task cancelled!","Task status",MB_OK);
				break;
			}
		}

	}//while !dir empty

	::FindClose(hFind);

	runtimeInfoText->SetWindowText("SuperMD Plugin task : Finished");
	taskStatus = 0;

	std::string text = "Task finished.\nBad files found (and skipped) : " + itoS<int>(badFilesFound,std::dec);

	MessageBox(text.c_str(),"End of cycle",MB_OK);
}

void CSupermdPlugHostDlg::OnSilentClick()
{
	plugSilentMode = !plugSilentMode;
}
