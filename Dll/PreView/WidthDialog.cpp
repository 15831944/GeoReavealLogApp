// WidthDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PreView.h"
#include "WidthDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWidthDialog dialog


CWidthDialog::CWidthDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CWidthDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWidthDialog)
	m_MaxWidth = 0;
	m_MinWidth = 0;
	m_Width = 0;
	//}}AFX_DATA_INIT
}


void CWidthDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWidthDialog)
//	DDX_Text(pDX, IDC_MAX_WIDTH, m_MaxWidth);
//	DDX_Text(pDX, IDC_MIN_WIDTH, m_MinWidth);
	DDX_Text(pDX, IDC_WIDTH, m_Width);
//	DDV_MinMaxUInt(pDX, m_Width, m_MinWidth, m_MaxWidth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWidthDialog, CDialog)
	//{{AFX_MSG_MAP(CWidthDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWidthDialog message handlers

void CWidthDialog::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

BOOL CWidthDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	char s[255];
	sprintf(s, "%d   µ½  %d", m_MinWidth, m_MaxWidth);
	GetDlgItem(IDC_MIN_WIDTH)->SetWindowText(s);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
