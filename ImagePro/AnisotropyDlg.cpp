// AnisotropyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "flowchart.h"
#include "AnisotropyDlg.h"
#include "Database.h"
#include "FIDio.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnisotropyDlg dialog


CAnisotropyDlg::CAnisotropyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAnisotropyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAnisotropyDlg)
	//}}AFX_DATA_INIT
	m_iImage=1;
}


void CAnisotropyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnisotropyDlg)
	DDX_Text(pDX, IDC_EDIT2, m_AzMax);
	DDX_Text(pDX, IDC_EDIT5, m_RxyMin);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT1, m_fWin);
	DDX_Text(pDX, IDC_EDIT4, m_fRlev);
	DDX_Text(pDX, IDC_EDIT6,m_NumMin);
	DDX_Radio(pDX,IDC_RADIO4, m_iImage);
}


BEGIN_MESSAGE_MAP(CAnisotropyDlg, CDialog)
	//{{AFX_MSG_MAP(CAnisotropyDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnisotropyDlg message handlers

BOOL CAnisotropyDlg::OnInitDialog() 
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