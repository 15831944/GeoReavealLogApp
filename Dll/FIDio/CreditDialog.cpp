// CreditDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CreditDialog.h"
#include "FaderWnd.h"
char *pArrCredit =NULL;
/////////////////////////////////////////////////////////////////////////////
// CCreditDialog dialog

CCreditDialog::CCreditDialog(char* Editer,CWnd* pParent /*=NULL*/)
	: CDialog(CCreditDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreditDialog)
	//}}AFX_DATA_INIT
	m_Editer=Editer;
	CString strCredits = "\t极睿测井解释平台\n\n\n"				   
				   "\r版权所有\n\n"
				   "川庆钻探工程有限公司测井公司\n\n\n\n"
				   "地址:重庆市江北区大石坝大庆村400#\n"
				   "联系人:赵中明\n"
				   "Tel:023 67352978\n"
				   "Email:zhao978_cj@cnpc.com.cn";
	m_static.SetCredits(strCredits);
	m_Small=TRUE;
}


void CCreditDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreditDialog)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, FID_IDC_DISPLAY_STATIC, m_static);
//	DDX_Control(pDX, FID_IDC_STATIC_MAIL, m_MailLink);
	DDX_Control(pDX, FID_IDC_STATIC_LINK, m_HomePageLink);
}


BEGIN_MESSAGE_MAP(CCreditDialog, CDialog)
	//{{AFX_MSG_MAP(CCreditDialog)
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreditDialog message handlers

BOOL CCreditDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CWndShadow::Initialize(AfxGetInstanceHandle());
	m_Shadow.Create(GetSafeHwnd());
	GetDlgItem (FID_EDITOR)->SetWindowText(m_Editer);
	m_MailLink.SetURL("mailto:zhao978_cj@cnpc.com.cn");
	m_HomePageLink.SetURL("http://10.132.200.177");
	m_static.SetExponentialClouds(m_Small);

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

//	SetTimer(1,16500,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCreditDialog::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnLButtonDown(nFlags, point);
	m_Small=!m_Small;
	m_static.SetExponentialClouds(m_Small);	
}

void CCreditDialog::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnTimer(nIDEvent);
	new CFaderWnd(this,500,255);
//	EndDialog(IDOK);
}
