// LogPlot.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <winspool.h>
#include "ImageMan.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "IpFrame.h"
#include "ImageManDoc.h"
#include "ImageManView.h"
#include <locale.h>
#include <io.h>
#include <direct.h>
#include "FIDio.h"
#include "CheckLogin.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageManApp

BEGIN_MESSAGE_MAP(CImageManApp, CWinApp)
	//{{AFX_MSG_MAP(CImageManApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_PRINT_PAGE_SETUP, OnPrintPageSetup)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	// Standard print setup command
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageManApp construction

CImageManApp::CImageManApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_UnDoPath=GetWorkPath()+"Temp";
	m_TempPath=m_UnDoPath;
	_mkdir(m_TempPath);
	CTime time = CTime::GetCurrentTime();
	CString strTime = time.Format(_T("%I%M"));
	CString strTimeFull = time.Format(_T("%m%d%H%M%S"));
		
	m_UnDoPath+="\\#PMP_";
	m_UnDoPath+=strTimeFull;
	_mkdir(m_UnDoPath);

	m_UnDoFilePath=m_UnDoPath+"\\#UnDo"+strTime;
	_mkdir(m_UnDoFilePath);	
	m_DataBuf=m_UnDoFilePath+"\\#WELLDATA\\";
	_mkdir(m_DataBuf);
	m_UnDoFilePath=m_UnDoFilePath+"UnDo"+strTime;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CImageManApp object

CImageManApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CImageManApp initialization

int CImageManApp::ExitInstance()
{
	DelDirAllFile(m_UnDoPath);
	return CWinApp::ExitInstance();
}

BOOL CImageManApp::InitInstance()
{
	// Initialize OLE libraries
	// 读取成像处理模块的注册号
	/*CCheckLogin Login;
	if(!Login.ChechImageLogin())
		return FALSE;*/
	m_PlotFileName = "";
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	SetRegistryKey(_T("GeoReveal Soft WorkGroup"));
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_LOGPLTTYPE,
		RUNTIME_CLASS(CImageManDoc),
		RUNTIME_CLASS(CChildFrame), 
		RUNTIME_CLASS(CImageManView));
	pDocTemplate->SetContainerInfo(IDR_LOGPLTTYPE_CNTR_IP);
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
	// Enable DDE Execute open
	EnableShellOpen();
	CString s;
	pDocTemplate->GetDocString(s,CDocTemplate::regFileTypeId);
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Check to see if launched as OLE server
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		// Application was run with /Embedding or /Automation.  Don't show the
		//  main window in this case.
		return TRUE;
	}
	// The main window has been initialized, so show and update it.
	m_ProCall=FALSE;
	if(!cmdInfo.m_strFileName.IsEmpty())
	{
		cmdInfo.m_nShellCommand=CCommandLineInfo::FileNew;
		m_PlotFileName = cmdInfo.m_strFileName;
		if(m_PlotFileName.Find ("<")>0)
			m_ProCall=TRUE;
		ProcessShellCommand(cmdInfo);
	}
	else
		m_PlotFileName = "";
	pMainFrame->ShowWindow(SW_SHOWNORMAL);
	pMainFrame->UpdateWindow();
	m_MoveDep=FALSE;
	m_PointDep=FALSE;
	m_Nib=FALSE;
	m_DepPos=m_ThickPos=0;
	return TRUE;
	// Dispatch commands specified on the command line
}

// App command to run the dialog
void CImageManApp::OnAppAbout()
{
	SoftAbout("作者：赵中明 2015.4  读取文档版本：10.0");
}

/////////////////////////////////////////////////////////////////////////////
// CImageManApp commands

void CImageManApp::SetDevMode(CSize size) 
{
	UpdatePrinterSelection(FALSE);
	if (m_hDevMode != NULL)
	{
		LPDEVMODE lpDevMode = (LPDEVMODE)GlobalLock(m_hDevMode);
		lpDevMode->dmPaperSize = 256;
		lpDevMode->dmPaperWidth =(short)size.cx;
		lpDevMode->dmPaperLength = (short)size.cy;
		GlobalUnlock(m_hDevMode);
	}
}

void CImageManApp::OnFileOpen() 
{
	ASSERT(m_pDocManager != NULL);
	CString newName;

	static char BASED_CODE szFilter[] = 
	"测井绘图复合文档(*.LPD)|*.LPD|所有文件(*.*)|*.*||";
	CFileDialog  Browse(TRUE, "测井绘图复合文档(*.LPD)", "*.LPD", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	Browse.m_ofn.lpstrTitle = "打开文件";
	Browse.m_ofn.lpstrInitialDir=GetCurrentUserPath();
	if (Browse.DoModal()==IDOK)
	{
		m_pDocManager->CloseAllDocuments (TRUE);
		newName = Browse.GetFileName ();
		OpenDocumentFile(newName);
	}
}

void CImageManApp::OnFileNew() 
{
	ASSERT(m_pDocManager != NULL);
	m_pDocManager->CloseAllDocuments (TRUE);
		m_pDocManager->OnFileNew();
}

CDocument* CImageManApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	CString newName = lpszFileName;
	if (newName.Find('.') == -1)  
		newName += ".LPD";
	return CWinApp::OpenDocumentFile(newName);
}

void CImageManApp::OnPrintPageSetup() 
{
	CPageSetupDialog MyPageSetupDialog;
	MyPageSetupDialog.DoModal();
}

void CImageManApp::DelDirAllFile(CString DirName)
{
	CString DelFileName=DirName+"\\*.*";
	struct _finddata_t c_file;
    long hFile;

    if( (hFile = _findfirst( DelFileName, &c_file )) != -1L )
    {
		_findnext( hFile, &c_file );
        while( _findnext( hFile, &c_file ) == 0 )
		{
			DelFileName=DirName+"\\"+c_file.name;
			if(c_file.attrib&_A_SUBDIR)
			{		
				DelDirAllFile(DelFileName);
				rmdir(DelFileName);
			}
		    else
				remove(DelFileName);
		}
        _findclose( hFile );
	}
	rmdir(DirName);
}
