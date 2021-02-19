// ThickCpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "flowchart.h"
#include "ThickCpDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CThickCpDlg dialog


CThickCpDlg::CThickCpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CThickCpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CThickCpDlg)
	//}}AFX_DATA_INIT
	m_HighValue=4.f;
	m_LowValue=-4.f;
	m_BoxLength=60.f;
	m_TubeThick=5.f;
	m_NormalThick=5.f;
}


void CThickCpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CThickCpDlg)	
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT1, m_HighValue);
	DDX_Text(pDX, IDC_EDIT8, m_LowValue);
	DDX_Text(pDX, IDC_EDIT9, m_BoxLength);
	DDX_Text(pDX, IDC_EDIT10, m_TubeThick);
	DDX_Text(pDX, IDC_EDIT11, m_NormalThick);
}


BEGIN_MESSAGE_MAP(CThickCpDlg, CDialog)
	//{{AFX_MSG_MAP(CThickCpDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CThickCpDlg message handlers

BOOL CThickCpDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	CRect rect;
	// get the current window size and position
	GetWindowRect( rect );
	int h=rect.Height ();
	int w=rect.Width ();
	rect.left =(GetSystemMetrics(SM_CXSCREEN)-w)/2;
	rect.top =(GetSystemMetrics(SM_CYSCREEN)-h)/2;
	rect.bottom =rect.top+h;
	rect.right =rect.left+w;
	MoveWindow(&rect);
	m_Shadow.Create(GetSafeHwnd());
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
