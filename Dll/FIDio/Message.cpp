// Message.cpp : implementation file
//

#include "stdafx.h"
#include "Message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma comment(lib, "winmm.lib")
#include <Mmsystem.h>

/////////////////////////////////////////////////////////////////////////////
// CMessage dialog


CMessage::CMessage(BOOL bOK,BOOL bCancel,CWnd* pParent /*=NULL*/)
	: CDialog(CMessage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMessage)
	m_szMessage = _T("");
	//}}AFX_DATA_INIT
	m_bOK=bOK;
	m_bCancel=bCancel;
}


void CMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMessage)
	DDX_Text(pDX, IDC_MSG, m_szMessage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMessage, CDialog)
	//{{AFX_MSG_MAP(CMessage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessage message handlers

BOOL CMessage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CWndShadow::Initialize(AfxGetInstanceHandle());
	m_Shadow.Create(GetSafeHwnd());

	if(!m_bOK)
		GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
	if(!m_bCancel)
		GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);
	CRect rect;
	// get the current window size and position
	GetWindowRect( rect );
	// now change the size, position, and Z order 
	// of the window.
	int h=rect.Height ();
	int w=rect.Width ();
	rect.left =(GetSystemMetrics(SM_CXSCREEN)-w)/2;
	rect.top =(GetSystemMetrics(SM_CYSCREEN)-h)/2;
	rect.bottom =rect.top+h;
	rect.right =rect.left+w;
	MoveWindow(&rect);
	::SetWindowPos(
		m_hWnd ,        // handle to window
		HWND_TOPMOST,   // placement-order handle
		rect.left,      // horizontal position
		rect.top,       // vertical position
		rect.Width(),	// width
		rect.Height(),  // height
		SWP_NOACTIVATE  // window-positioning options  SWP_SHOWWINDOW
	);
	Beep(700,150);
  	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
