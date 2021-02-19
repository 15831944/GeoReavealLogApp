/******************************************************************
*   MainFram.cpp - implementation of the CMainFram class          *           
*******************************************************************/

#include "stdafx.h"
#include "ImageMan.h"
#include "FIDio.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "ImageManView.h"
#include "ImageManDoc.h"
#include "ScaleFactor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDS_DEFAULT_FONT 103
#define m_ScaleNum 16

char m_ScaleName[m_ScaleNum][10] = 
    {"Window","250%","200%","180%","160%","140%","120%",
	 "100%","90%","80%","70%","60%","50%","40%","30%","User Set"};
                                                        
/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_COMMAND_EX(ID_VIEW_PLOTBAR, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PLOTBAR, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_DOCBAR, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DOCBAR, OnUpdateControlBarMenu)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(IDC_FIT_WINDOW, OnFitWindow)
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND_RANGE(33171,33186,OnScaleChange)	
	ON_UPDATE_COMMAND_UI(IDC_FIT_WINDOW, OnUpdateFitWindow)
	//User defined well_curve message
END_MESSAGE_MAP()

	
LONG CMainFrame::OnWellDataUpdate(UINT wParam, LONG lParam)
{
	CString szLogName;
	POSITION POS = AfxGetApp()->GetFirstDocTemplatePosition( );
	while (POS != NULL)
	{
		CDocTemplate* pDocTemplate = AfxGetApp()->GetNextDocTemplate( POS );
		POSITION POS1=pDocTemplate->GetFirstDocPosition( );
		while (POS1 != NULL)
		{
			CImageManDoc* pDoc =(CImageManDoc*) pDocTemplate->GetNextDoc( POS1 );
			pDoc->OnWellDataUpdate(szLogName);
		}
	}
	return 0L;
}

LONG CMainFrame::OnCurveDataUpdate(UINT wParam,  LONG lParam)
{
	CString szLogName;
	POSITION POS = AfxGetApp()->GetFirstDocTemplatePosition( );
	while (POS != NULL)
	{
		CDocTemplate* pDocTemplate = AfxGetApp()->GetNextDocTemplate( POS );
		POSITION POS1=pDocTemplate->GetFirstDocPosition( );
		while (POS1 != NULL)
		{
			CImageManDoc* pDoc =(CImageManDoc*) pDocTemplate->GetNextDoc( POS1 );
			pDoc->OnCurveDataUpdate(szLogName);
		}
	}
	return 0L;
}

//static UINT indicators[] ={ID_SEPARATOR};
static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};
/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_ScaleNo = 7;
	ScaleFactor[0] = 100;
	ScaleFactor[1] =250;
	ScaleFactor[2] =200;
	ScaleFactor[3] =180;
	ScaleFactor[4] =160;
	ScaleFactor[5] =140;
	ScaleFactor[6] =120;
	ScaleFactor[7] =100;
	ScaleFactor[8] =-111;
	ScaleFactor[9] =-125;
	ScaleFactor[10] =-143;
	ScaleFactor[11] =-167;
	ScaleFactor[12] =-200;
	ScaleFactor[13] =-250;
	ScaleFactor[14] =-334;
	ScaleFactor[15] =100;
	m_Fit=FALSE;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::GetScaleFactor(int ScaleNo)
{
	if ((ScaleNo < 1) || (ScaleNo>=m_ScaleNum)) return 100;
	return ScaleFactor[m_ScaleNo];
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// CG: This line was added by the Palette Support component
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	EnableDocking(CBRS_ALIGN_ANY);
	m_wndMenuBar.Create(this, WS_CHILD|WS_VISIBLE|CBRS_LEFT|CBRS_GRIPPER
		|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC);
	m_wndMenuBar.LoadMenu(IDR_LOGPLTTYPE);	
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndMenuBar,AFX_IDW_DOCKBAR_TOP);

	if (!m_wndToolBar.CreateEx(this,TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP|CBRS_TOOLTIPS,
		CRect(0,0,0,0),ID_VIEW_DOCBAR)
		||!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar.SetWindowText ("文档工具");
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar,AFX_IDW_DOCKBAR_LEFT);
	m_wndToolBar.SetBorders(1,1,1,1);

	CImageList	imageList;
	CBitmap		bitmap;
	bitmap.LoadBitmap(IDB_TOOLBAR_HOT);
	imageList.Create(15, 16, ILC_COLORDDB|ILC_MASK, 19, 1);
	imageList.Add(&bitmap, RGB(255,0,255));
	m_wndToolBar.SendMessage(TB_SETIMAGELIST, 0, (LPARAM)imageList.m_hImageList);
	imageList.Detach();
	bitmap.Detach();

	if (!m_wndPlotBar.CreateEx(this,TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP|CBRS_TOOLTIPS
		 | CBRS_TOOLTIPS | CBRS_FLYBY,
		CRect(0,0,0,0),ID_VIEW_PLOTBAR)
		||!m_wndPlotBar.LoadToolBar(IDR_PLOTTOOL))
	{
		TRACE0("Failed to create plotToolbar\n");
		return -1;      // fail to create
	}
	m_wndPlotBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndPlotBar,AFX_IDW_DOCKBAR_TOP);
	m_wndPlotBar.SetWindowText ("交互处理工具");
	m_wndPlotBar.SetBorders(1,1,1,1);

	bitmap.LoadBitmap(IDB_PLOTBAR_HOT);
	imageList.Create(16, 16, ILC_COLORDDB|ILC_MASK, 21, 1);
	imageList.Add(&bitmap, RGB(255,0,255));
	m_wndPlotBar.SendMessage(TB_SETIMAGELIST, 0, (LPARAM)imageList.m_hImageList);
	imageList.Detach();
	bitmap.Detach();
	
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
		
	CString Path=GetWorkPath("SoftSkin")+"theme.ini";
	m_skinWin.LoadSkin( Path );
	m_skinWin.InstallSkin( this );
	SetWindowPos( 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE |SWP_FRAMECHANGED );
	return 0;
}

void CMainFrame::OnScaleChange(UINT nCommandID)
{
	int m_ScaleNo0=m_ScaleNo;
	m_ScaleNo = nCommandID-33171;//m_ScaleBox.GetCurSel();
	if(m_ScaleNo==15)
	{
		CScaleFactor Dlg;
		Dlg.m_ScaleFactor =ScaleFactor [m_ScaleNo0];
		if(Dlg.DoModal ()==IDOK)
			ScaleFactor [15]= Dlg.m_ScaleFactor ;
		else
			m_ScaleNo=m_ScaleNo0;
	}
	CMDIChildWnd* pChildWnd = MDIGetActive();
	if (pChildWnd == NULL) return ;
	CImageManDoc* pDoc = (CImageManDoc*)(pChildWnd->GetActiveDocument());
	if(pDoc != NULL) 
	{
		pDoc->OnScaleChange();
		pChildWnd->SetFocus();
	}
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	int s, t, b, r, l;
	if (AfxGetApp()->GetProfileInt("Settings", "SavePosition", 1))
	{
		// only restore if there is a previously saved position
		if ( -1 != (s = AfxGetApp()->GetProfileInt("Settings", "FrameStatus",   -1)) &&
			 -1 != (t = AfxGetApp()->GetProfileInt("Settings", "FrameTop",      -1)) &&
			 -1 != (l = AfxGetApp()->GetProfileInt("Settings", "FrameLeft",     -1)) &&
			 -1 != (b = AfxGetApp()->GetProfileInt("Settings", "FrameBottom",   -1)) &&
			 -1 != (r = AfxGetApp()->GetProfileInt("Settings", "FrameRight",    -1))
		   ) 
		{
			// restore the window's status
			AfxGetApp()->m_nCmdShow = s;

			// restore the window's width and height
			cs.cx = r - l;
			cs.cy = b - t;

			// the following correction is needed when the taskbar is
			// at the left or top and it is not "auto-hidden"
			RECT workArea;
			SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
			l += workArea.left;
			t += workArea.top;

			cs.x = l;
			cs.y = t;
		}
	}
	cs.style &= ~FWS_ADDTOTITLE;
	return CMDIFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnClose() 
{
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	if(pMain != NULL)
	{
		CMDIChildWnd* pChildWnd = pMain->MDIGetActive();
		if (pChildWnd != NULL) 
		{
			CImageManDoc* pDoc = (CImageManDoc*)(pChildWnd->GetActiveDocument());
			if(pDoc != NULL) 
				pDoc->BufDataToWell();
		}
	}
	if (AfxGetApp()->GetProfileInt("Settings", "SavePosition", 1))
	{
		// Save main window position
		WINDOWPLACEMENT wp;
		GetWindowPlacement(&wp);
		AfxGetApp()->WriteProfileInt("Settings", "FrameStatus", wp.showCmd);
		AfxGetApp()->WriteProfileInt("Settings", "FrameTop",    wp.rcNormalPosition.top);
		AfxGetApp()->WriteProfileInt("Settings", "FrameLeft",   wp.rcNormalPosition.left);
		AfxGetApp()->WriteProfileInt("Settings", "FrameBottom", wp.rcNormalPosition.bottom);
		AfxGetApp()->WriteProfileInt("Settings", "FrameRight",  wp.rcNormalPosition.right);
	}	
	//SaveBarState(AfxGetApp()->m_pszProfileName);
	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnFitWindow() 
{
	// TODO: Add your command handler code here
	if(!m_Fit)
	{
		m_ScaleNo0=m_ScaleNo;
		m_ScaleNo = 0;
	}
	else
		m_ScaleNo=m_ScaleNo0;
	m_Fit=!m_Fit;
	CMDIChildWnd* pChildWnd = MDIGetActive();
	if (pChildWnd == NULL) return ;
	CImageManDoc* pDoc = (CImageManDoc*)(pChildWnd->GetActiveDocument());
	if(pDoc != NULL) 
	{
		pDoc->OnScaleChange();
		pChildWnd->SetFocus();
	}
}

void CMainFrame::OnUpdateFitWindow(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck (m_Fit);
}