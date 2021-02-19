// DIPComputeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "DIPComputeDlg.h"
#include "FIDio.h"
#include <io.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDIPComputeDlg dialog


CDIPComputeDlg::CDIPComputeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDIPComputeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDIPComputeDlg)
	m_WinLength = 3.0f;
	m_StepLength = 0.1f;
	m_SearchLength = 0.5f;
	m_DangMax = 80.0f;
	m_RMin = 0.5f;
	m_GradCons=100000.f;
	m_GradMin=10.f;
	//}}AFX_DATA_INIT
}


void CDIPComputeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDIPComputeDlg)
	DDX_Text(pDX, IDC_WIN_LENGTH, m_WinLength);
	DDX_Text(pDX, IDC_STEP_LENGTH, m_StepLength);
	DDX_Text(pDX, IDC_SEARCH_LENGTH, m_SearchLength);
	DDX_Text(pDX, IDC_MAX_DIP, m_DangMax);
	DDX_Text(pDX, IDC_MIN_COEF, m_RMin);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_MIN_COEF2,m_GradCons);
	DDX_Text(pDX, IDC_MIN_COEF3,m_GradMin);
}


BEGIN_MESSAGE_MAP(CDIPComputeDlg, CDialog)
	//{{AFX_MSG_MAP(CDIPComputeDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CDIPComputeDlg::OnInitDialog() 
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
	UpdateData(FALSE);
	m_Shadow.Create(GetSafeHwnd());
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
