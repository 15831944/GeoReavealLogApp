// InputBox.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "InputBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputBox dialog


CInputBox::CInputBox(CWnd* pParent /*=NULL*/)
	: CDialog(CInputBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputBox)
	m_Value = 0.0f;
	//}}AFX_DATA_INIT
}


void CInputBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputBox)
	DDX_Text(pDX, IDC_EDIT_INPUT, m_Value);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputBox, CDialog)
	//{{AFX_MSG_MAP(CInputBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputBox message handlers

BOOL CInputBox::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	GetDlgItem(IDC_STATIC1)->SetWindowText(m_Title);
	GetDlgItem(IDC_NAME)->SetWindowText(m_CurveName);
	GetDlgItem(IDC_DEP)->SetWindowText(m_DepRange);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
