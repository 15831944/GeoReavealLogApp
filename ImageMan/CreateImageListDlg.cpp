// CreateImageList.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageMan.h"
#include "CreateImageListDlg.h"
#include "FIDIO.h"
#include "Database.h"

// CCreateImageListDlg 对话框

IMPLEMENT_DYNAMIC(CCreateImageListDlg, CDialog)
CCreateImageListDlg::CCreateImageListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateImageListDlg::IDD, pParent)
{
	m_szPorName=_T("POR");
	m_MinValue=3.f;
	m_MinThick=1.f;
	m_szFVDC="FVDC";//裂缝密度 
	m_szFVPA="FVPA";//裂缝孔隙度曲线

}

CCreateImageListDlg::~CCreateImageListDlg()
{
}

void CCreateImageListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_NAME, m_szPorName);
	DDX_CBString(pDX, IDC_NAME2, m_szFVDC);
	DDX_CBString(pDX, IDC_NAME3, m_szFVPA);
	DDX_Text(pDX, IDC_EDIT1, m_MinValue);
	DDX_Text(pDX, IDC_EDIT2, m_MinThick);
}

BEGIN_MESSAGE_MAP(CCreateImageListDlg, CDialog)
END_MESSAGE_MAP()

BOOL CCreateImageListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_NAME);
	if (pBox == NULL) return FALSE;
	pBox->ResetContent();

	CComboBox* pBox2 = (CComboBox*) GetDlgItem(IDC_NAME2);
	if (pBox2 == NULL) return FALSE;
	pBox2->ResetContent();
	CComboBox* pBox3 = (CComboBox*) GetDlgItem(IDC_NAME3);
	if (pBox3 == NULL) return FALSE;
	pBox3->ResetContent();

	CString szDir;
	CFIDIO::GetPath(m_szProjectName,szDir.GetBuffer(512));
	szDir.ReleaseBuffer();

	CDbaseFile m_dBaseFile;
	if(m_dBaseFile.Open(szDir+"Curve.dbf")!=DBASE_SUCCESS)
		return FALSE;
	char szBuff[255];
	CString Name,Type;
	for(int rc=m_dBaseFile.GetFirstRecord();rc==DBASE_SUCCESS;rc=m_dBaseFile.GetNextRecord())
  	{
		if (m_dBaseFile.IsRecordDeleted())
			continue;
		m_dBaseFile.GetField(1, szBuff);
		Name=szBuff;
		Name.TrimLeft();Name.TrimRight();
		m_dBaseFile.GetField("曲线类型", szBuff);
		Type=szBuff;
		Type.TrimLeft();Type.TrimRight();
		if(Type=="常规")
		{
			pBox->AddString (Name);
			pBox2->AddString (Name);
			pBox3->AddString (Name);
		}
		if(m_szPorName==Name)
			pBox->SetWindowText(m_szPorName);
		if(m_szFVDC==Name)
			pBox2->SetWindowText(m_szFVDC);
		if(m_szFVPA==Name)
			pBox3->SetWindowText(m_szFVPA);
	}
	m_dBaseFile.Close();
	return TRUE; 
}
