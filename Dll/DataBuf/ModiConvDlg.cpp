// ModiConvDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModiConvDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModiConvDlg dialog


CModiConvDlg::CModiConvDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModiConvDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModiConvDlg)
	m_NewRlev = 0.0f;
	m_NewStart = 0.0f;
	m_NewStop = 0.0f;
	m_OldRlev = 0.0f;
	m_OldStart = 0.0f;
	m_OldStop = 0.0f;
	//}}AFX_DATA_INIT
}


void CModiConvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModiConvDlg)
	DDX_Text(pDX, IDC_NEWRLEV_EDIT, m_NewRlev);
	DDX_Text(pDX, IDC_NEWSTART_EDIT, m_NewStart);
	DDX_Text(pDX, IDC_NEWSTOP_EDIT, m_NewStop);
	DDX_Text(pDX, IDC_OLDRLEV_EDIT, m_OldRlev);
	DDX_Text(pDX, IDC_OLDSTART_EDIT, m_OldStart);
	DDX_Text(pDX, IDC_OLDSTOP_EDIT, m_OldStop);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModiConvDlg, CDialog)
	//{{AFX_MSG_MAP(CModiConvDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModiConvDlg message handlers

BOOL CModiConvDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_NewRlev = m_OldRlev;
	m_NewStart = m_OldStart;
	m_NewStop = m_OldStop;
	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CModiConvDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	if(m_NewStart>m_NewStop)
	{
		AfxMessageBox("起始索引必须小于终止索引");
		return;
	}
	if(m_NewRlev<=0)
	{
		AfxMessageBox("采样间隔必须大于0");
		return;
	}
	CDialog::OnOK();
}
