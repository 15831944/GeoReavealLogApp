// ModifyHandWorkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "ModifyHandWorkDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModifyHandWorkDlg dialog


CModifyHandWorkDlg::CModifyHandWorkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModifyHandWorkDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModifyHandWorkDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_TypeSin=0;
}


void CModifyHandWorkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModifyHandWorkDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Radio(pDX, IDC_TYPESIN1, m_TypeSin);
}


BEGIN_MESSAGE_MAP(CModifyHandWorkDlg, CDialog)
	//{{AFX_MSG_MAP(CModifyHandWorkDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModifyHandWorkDlg message handlers

BOOL CModifyHandWorkDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CWndShadow::Initialize(AfxGetInstanceHandle());
	m_Shadow.Create(GetSafeHwnd());
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
