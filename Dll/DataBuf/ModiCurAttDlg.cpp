// ModiCurAttDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModiCurAttDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModiCurAttDlg dialog


CModiCurAttDlg::CModiCurAttDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModiCurAttDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModiCurAttDlg)
	m_NewCurve = _T("");
	m_OldCurve = _T("");
	m_OldUnits = _T("");
	m_NewUnits = _T("");
	//}}AFX_DATA_INIT
}


void CModiCurAttDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModiCurAttDlg)
	DDX_Control(pDX, IDC_NEWUNITS_COMBO, m_ctlNewUnits);
	DDX_Text(pDX, IDC_NEWCURVE_EDIT, m_NewCurve);
	DDX_Text(pDX, IDC_OLDCURVE_EDIT, m_OldCurve);
	DDX_Text(pDX, IDC_OLDUNITS_EDIT, m_OldUnits);
	DDX_CBString(pDX, IDC_NEWUNITS_COMBO, m_NewUnits);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CModiCurAttDlg, CDialog)
	//{{AFX_MSG_MAP(CModiCurAttDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModiCurAttDlg message handlers

void CModiCurAttDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	m_NewCurve.TrimLeft();
	m_NewCurve.TrimRight();
	m_NewCurve.Remove(' ');
	UpdateData(false);
	if(m_NewCurve.IsEmpty())
	{
		AfxMessageBox("曲线名不能为空");
		return;
	}
	CDialog::OnOK();
}

BOOL CModiCurAttDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ctlNewUnits.AddString(m_NewUnits);
	m_ctlNewUnits.SetWindowText(m_NewUnits);
	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
