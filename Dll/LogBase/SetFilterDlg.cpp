// SetFilterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SetFilterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetFilterDlg dialog


CSetFilterDlg::CSetFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetFilterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetFilterDlg)
	m_FilterCriteria = _T("");
	m_FilterFieldName = _T("");
	//}}AFX_DATA_INIT
}


void CSetFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetFilterDlg)
	DDX_Text(pDX, IDC_EDIT1, m_FilterCriteria);
	DDX_CBString(pDX, IDC_COMBO1, m_FilterFieldName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetFilterDlg, CDialog)
	//{{AFX_MSG_MAP(CSetFilterDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetFilterDlg message handlers

BOOL CSetFilterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// TODO: Add extra initialization here
	// iterate through all arguments and create a list of all the fields
	CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_COMBO1);
	for (int i=0; i < m_strFieldArray.GetSize(); i++)
	{
		CString strBuff;
		AfxExtractSubString(strBuff, m_strFieldArray[i], 0, ',');
		strBuff.TrimLeft();
		strBuff.TrimRight();
		pBox->AddString(strBuff);
	}
	pBox->SetWindowText(m_FilterFieldName);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
