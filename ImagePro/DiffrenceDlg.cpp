// DiffrenceDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FlowChart.h"
#include "DiffrenceDlg.h"


// CDiffrenceDlg 对话框

IMPLEMENT_DYNAMIC(CDiffrenceDlg, CDialog)
CDiffrenceDlg::CDiffrenceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDiffrenceDlg::IDD, pParent)
{
	m_DaltaDep=0.0762f;
	m_szStr=_T("");
}

CDiffrenceDlg::~CDiffrenceDlg()
{
}

void CDiffrenceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_DaltaDep);
	DDX_CBString(pDX, IDC_COMBO1, m_szDifName);
}

BEGIN_MESSAGE_MAP(CDiffrenceDlg, CDialog)
END_MESSAGE_MAP()

BOOL CDiffrenceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if(m_szStr.IsEmpty())
		return TRUE;
	CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_COMBO1);
	if(pBox == NULL) 
		return TRUE;
	pBox->ResetContent(); 
	CString szStr=m_szStr;
	szStr.Trim();
	int x=szStr.Find(",");
	CString szTmp;
	while(x>0)
	{
		szTmp=szStr.Left(x);
		pBox->AddString(szTmp);
		szStr=szStr.Mid(x+1);
		szStr.Trim();
		x=szStr.Find(",");
	}
	if(!szStr.IsEmpty())
		pBox->AddString(szStr);
	pBox->SetWindowText(m_szDifName);
	return TRUE; 
}
