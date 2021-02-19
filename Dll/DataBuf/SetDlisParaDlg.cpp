// SetDlisParaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SetDlisParaDlg.h"
#include "CodeSP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetDlisParaDlg dialog


CSetDlisParaDlg::CSetDlisParaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetDlisParaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetDlisParaDlg)
	m_Company = _T("");
	m_DefaultVal = 0.0f;
	m_Field = _T("");
	m_Well = _T("");
	m_FileName = _T("");
	//}}AFX_DATA_INIT
}


void CSetDlisParaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetDlisParaDlg)
	DDX_Text(pDX, IDC_COMPANY_EDIT, m_Company);
	DDV_MaxChars(pDX, m_Company, 127);
	DDX_Text(pDX, IDC_DEFAULTVAL_EDIT, m_DefaultVal);
	DDX_Text(pDX, IDC_FIELD_EDIT, m_Field);
	DDV_MaxChars(pDX, m_Field, 127);
	DDX_Text(pDX, IDC_WELL_EDIT, m_Well);
	DDV_MaxChars(pDX, m_Well, 127);
	DDX_Text(pDX, IDC_FILENAME_EDIT, m_FileName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSetDlisParaDlg, CDialog)
	//{{AFX_MSG_MAP(CSetDlisParaDlg)
	ON_BN_CLICKED(IDC_ALLIGNORE_BTN, OnAllignoreBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetDlisParaDlg message handlers

void CSetDlisParaDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	m_Company.TrimLeft();
	m_Company.TrimRight();
	m_Field.TrimLeft();
	m_Field.TrimRight();
	m_Well.TrimLeft();
	m_Well.TrimRight();
	UpdateData(false);
	if(!IsAsciiString(m_Company))
	{
		AfxMessageBox("公司名不合适");
		return;
	}
	if(!IsAsciiString(m_Field))
	{
		AfxMessageBox("油田名不合适");
		return;
	}
	if(!IsAsciiString(m_Well))
	{
		AfxMessageBox("井名不合适");
		return;
	}
	CDialog::OnOK();
}

void CSetDlisParaDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	UpdateData(true);
	if(IsNullString((const char*)m_Company))
	{
		m_Company="Sichuan Petroleum Administration";
	}
	if(IsNullString((const char*)m_Field))
	{
		m_Field="noFieldName";
	}
	if(IsNullString((const char*)m_Well))
	{
		m_Well="noWellName";
	}	
	CDialog::OnCancel();
}

BOOL CSetDlisParaDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_bAllIgnore=false;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetDlisParaDlg::OnAllignoreBtn() 
{
	m_bAllIgnore=true;
	OnCancel();
}
