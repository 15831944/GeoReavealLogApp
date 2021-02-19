// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "FlowChart.h"

#include "MainFrm.h"
#include "FIDio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_WM_MOVE()
	ON_WM_NCMOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_COMMAND(ID_SHOW_FRAM,Show)
END_MESSAGE_MAP()

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
	// TODO: add member initialization code here
	m_bDocked = FALSE;
	m_bHided = FALSE;
	m_bStart = FALSE;
	GetSizeOfScreen();
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_wndMenuBar.CreateEx (this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP ,
		CRect(0,0,0,0),ID_VIEW_MENUBAR);
	m_wndMenuBar.LoadMenu(IDR_MAINFRAME);

	if (!m_wndToolBar.CreateEx(this,TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP|CBRS_TOOLTIPS,
		CRect(0,0,0,0),ID_VIEW_TOOLBAR)
		||!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar.SetBorders(1,1,1,1);
	CImageList	imageList;
	CBitmap		bitmap;
	bitmap.LoadBitmap(IDB_TOOLBAR_HOT);
	imageList.Create(15, 15, ILC_COLORDDB|ILC_MASK, 8, 1);
	imageList.Add(&bitmap, RGB(255,0,255));
	m_wndToolBar.SendMessage(TB_SETIMAGELIST, 0, (LPARAM)imageList.m_hImageList);
	imageList.Detach();
	bitmap.Detach();

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	EnableDocking(CBRS_ALIGN_ANY);
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndMenuBar);

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	m_wndToolBar.SetWindowText ("工具栏");

	CString Path=GetWorkPath("SoftSkin")+"theme.ini";
	m_skinWin.LoadSkin( Path );
	m_skinWin.InstallSkin( this );
	SetWindowPos( 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE |SWP_FRAMECHANGED );
	if (AfxGetApp()->GetProfileInt("Settings", "ToolStatus", 1))
	{
		m_wndStatusBar.ShowWindow(AfxGetApp()->GetProfileInt("Settings", "m_wndStatusBar",-1));
		m_wndMenuBar.ShowWindow(AfxGetApp()->GetProfileInt("Settings", "m_wndMenuBar",-1));
		m_wndToolBar.ShowWindow(AfxGetApp()->GetProfileInt("Settings", "m_wndToolBar",-1));
	}
	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
	SetWindowLong(GetSafeHwnd(),GWL_EXSTYLE,WS_EX_TOOLWINDOW);
	return 0;
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
	cs.style &= ~WS_MINIMIZEBOX;
	cs.style &= ~WS_MAXIMIZEBOX;
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
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
	if (AfxGetApp()->GetProfileInt("Settings", "ToolStatus", 2))
	{
		AfxGetApp()->WriteProfileInt("Settings", "m_wndStatusBar", m_wndStatusBar.IsWindowVisible( ));
		AfxGetApp()->WriteProfileInt("Settings", "m_wndMenuBar",    m_wndMenuBar.IsWindowVisible( ));
		AfxGetApp()->WriteProfileInt("Settings", "m_wndToolBar",   m_wndToolBar.IsWindowVisible( ));
	}
	CFrameWnd::OnClose();
}

void CMainFrame::GetSizeOfScreen()
{
	CRect rc;
	::SystemParametersInfo(SPI_GETWORKAREA,0,&rc,0);
	m_sizeScreen = rc.Size();
}

void CMainFrame::InitInter()
{
	GetWindowRect(m_curRect);
	m_curPos = m_curRect.TopLeft();
}

void CMainFrame::Hide()
{
	if(!m_bHided)
	{
		m_bHided = TRUE;
		int iWidth,iHeight;
		switch(m_iDockState)
		{
		case DOCK_LEFT:
			iWidth = m_curRect.Width();
			m_curRect.OffsetRect(-iWidth+5,0);
			break;
		case DOCK_TOP:
			iHeight = m_curRect.Height();
			m_curRect.OffsetRect(0,-iHeight+5);
			break;
		case DOCK_RIGHT:
			iWidth = m_curRect.Width();
			m_curRect.OffsetRect(iWidth-5,0);
			break;
		}
		MoveWindow(m_curRect);
	}
}

void CMainFrame::OnMove(int x, int y) 
{
	CFrameWnd::OnMove(x, y);
	
	GetWindowRect(m_curRect);
	m_curPos = m_curRect.TopLeft();
	
	if(m_bStart)
		AdjustPosition();
	m_bStart = TRUE;
}

void CMainFrame::AdjustPosition()
{
	//如果窗口处于隐藏状态后,窗口的位置的调整不影响停靠的调整.
	if(!m_bHided)
	{
		//离屏幕左边小于10
		if(m_curPos.x < 10)
		{
			m_curRect.OffsetRect(-m_curPos.x,0);
			m_iDockState = DOCK_LEFT;
			m_bDocked = TRUE;
			MoveWindow(m_curRect);
		}
		//离屏幕上边小于10 
		else if(m_curPos.y < 10)
		{
			m_curRect.OffsetRect(0,-m_curPos.y);
			m_iDockState = DOCK_TOP;
			m_bDocked = TRUE;
			MoveWindow(m_curRect);
		}
		//离屏幕右边小于10 
		else if((m_curPos.x + m_curRect.Width()) > (m_sizeScreen.cx -10)) 
		{
			m_curRect.OffsetRect((m_sizeScreen.cx - m_curPos.x - m_curRect.Width()),0);
			m_iDockState = DOCK_RIGHT;
			m_bDocked = TRUE;
			MoveWindow(m_curRect);
		}
		//不在下边停靠
		else
			m_bDocked = FALSE;
	}
}

void CMainFrame::OnNcMouseMove(UINT nHitTest, CPoint point) 
{
	SetTimer(1,500,NULL);
	OnTimer(1);
	CFrameWnd::OnNcMouseMove(nHitTest, point);
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	CRect rc;
	GetWindowRect(rc);
	CPoint pos;
	GetCursorPos(&pos);

	if(!rc.PtInRect(pos))//鼠标是否位于窗口内
	{
		if(m_bDocked)//是否处于停靠状态
		{
			Hide(); 
		}
	}
	else
	{
		if(m_bDocked)//是否处于停靠状态
		{
			Show();
		}
	}	
	CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::Show()
{
	if(m_bHided)
	{
		m_bHided = FALSE;
		int iWidth,iHeight;
		switch(m_iDockState)
		{
		case DOCK_LEFT:
			iWidth = m_curRect.Width();
			m_curRect.OffsetRect(iWidth-5,0);
			break;
		case DOCK_TOP:
			iHeight = m_curRect.Height();
			m_curRect.OffsetRect(0,iHeight-5);
			break;
		case DOCK_RIGHT:
			iWidth = m_curRect.Width();
			m_curRect.OffsetRect(-iWidth+5,0);
			break;
		}
		MoveWindow(m_curRect);
	}
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
	GetWindowRect(m_curRect);	
}
