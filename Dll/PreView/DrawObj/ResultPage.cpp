// ResultPage.cpp : implementation file
//
#include "stdafx.h"
#include "Preview.h"
#include "ResultPage.h"
#include "LogPlotDoc.h"
#include "GeoAdo.h"
#include "WellBrowse.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResultPage property page

IMPLEMENT_DYNAMIC(CResultPage, CPropertyPage)

CResultPage::CResultPage() : CPropertyPage(CResultPage::IDD)
{
	//{{AFX_DATA_INIT(CResultPage)
	m_ResultLR = -1;
	m_FileName = _T("");
	
	//}}AFX_DATA_INIT
	m_WellID=0;
	m_szSever=_T("");
	m_GroupCaption="��עλ��";
	m_LeftCaption="���";
	m_RightCaption="�ұ�";
}

void CResultPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResultPage)
	DDX_Control(pDX, IDC_STATIC1, m_Static);
	DDX_Radio(pDX, IDC_LEFT, m_ResultLR);
	DDX_Text(pDX, IDC_WELLNAME, m_FileName);	
	//}}AFX_DATA_MA
	DDX_Control(pDX, IDC_RESULTLIST, m_ResultCtrl);
}


BEGIN_MESSAGE_MAP(CResultPage, CPropertyPage)
	//{{AFX_MSG_MAP(CResultPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BROWSE_WELL, OnBrowseWell)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResultPage message handlers

BOOL CResultPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	ListView_SetExtendedListViewStyle(m_ResultCtrl,LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ResultCtrl.ModifyStyle(0,LVS_REPORT);
	m_ResultCtrl.InsertColumn(0,"���", LVCFMT_LEFT, 40);
	m_ResultCtrl.InsertColumn(1,"��λ", LVCFMT_LEFT, 80);
	m_ResultCtrl.InsertColumn(2,"��ʼ���", LVCFMT_LEFT, 60);
	m_ResultCtrl.InsertColumn(3,"�������", LVCFMT_LEFT, 60);
	m_ResultCtrl.InsertColumn(4,"���ͽ���", LVCFMT_LEFT, 60);
	m_ResultCtrl.InsertColumn(5,"��϶��", LVCFMT_LEFT, 60);
	m_ResultCtrl.InsertColumn(6,"��͸��", LVCFMT_LEFT, 60);
	m_ResultCtrl.InsertColumn(7,"���Ͷ�", LVCFMT_LEFT, 60);
	m_ResultCtrl.InsertColumn(8,"٤��", LVCFMT_LEFT, 50);
	m_ResultCtrl.InsertColumn(9,"�ܶ�", LVCFMT_LEFT, 50);
	m_ResultCtrl.InsertColumn(10,"����", LVCFMT_LEFT, 50);
	m_ResultCtrl.InsertColumn(11,"����", LVCFMT_LEFT, 50);
	m_ResultCtrl.InsertColumn(12,"������", LVCFMT_LEFT, 60);
	m_ResultCtrl.InsertColumn(13,"��ע", LVCFMT_LEFT, 80);

	LoadResult();

	m_Static.SetWindowText(m_GroupCaption);
	SetDlgItemText(IDC_LEFT,m_LeftCaption);
	SetDlgItemText(IDC_RIGHT,m_RightCaption);
	if(m_LeftCaption=="����"&&m_RightCaption=="��ɫ")
		this->GetDlgItem(IDC_RIGHT2)->ShowWindow(SW_SHOW);
	return TRUE; 
}

void CResultPage::OnOK() 
{
	CPropertyPage::OnOK();
}

void CResultPage::OnBrowseWell() 
{
	CWellBrowse dlg;
	dlg.m_szSever=m_szSever;
	dlg.m_RootID=m_WellID;
	dlg.m_WellName=m_FileName;
	if(dlg.DoModal() == IDOK) 
	{
		m_FileName=dlg.m_WellName;
		UpdateData(FALSE);
		LoadResult();
	}
}

void CResultPage::LoadResult()
{
	m_ResultCtrl.DeleteAllItems();
	if(m_szSever.IsEmpty())
		return;
	CGeoAdo	ado;
	if(!ado.InitData(m_szSever)) 
		return;

	CString WellName=m_FileName.Mid(m_FileName.Find("\\")+1);
	if(WellName.Find("\\")>0)
		WellName=WellName.Left(WellName.Find("\\"));

	_RecordsetPtr pRS = ado.GetRS("SELECT * FROM Storage WHERE StorageName='" + WellName + "'");
	if(pRS->adoEOF)
	{
		ado.Disconnect();
		return;
	}
	else
	{
		m_WellID=atol((LPCTSTR)(_bstr_t)pRS->GetCollect("StorageID"));
		if(m_WellID>0)
		{
			CString cID;
			cID.Format("%lu",m_WellID);
			pRS = ado.GetRS("SELECT * FROM Result WHERE UpperID=" + cID);
			CString str;int nRow=0;
			while(pRS->adoEOF==0)
			{
				str=(LPCTSTR)(_bstr_t)pRS->GetCollect("Number");str.Trim();	
				m_ResultCtrl.InsertItem(nRow,str);
				str=(LPCTSTR)(_bstr_t)pRS->GetCollect("LayerName");str.Trim();
				m_ResultCtrl.SetItemText(nRow,1,str);
				str=(LPCTSTR)(_bstr_t)pRS->GetCollect("DepSt");str.Trim();
				m_ResultCtrl.SetItemText(nRow,2,str);
				str=(LPCTSTR)(_bstr_t)pRS->GetCollect("DepEn");str.Trim();
				m_ResultCtrl.SetItemText(nRow,3,str);
				str=(LPCTSTR)(_bstr_t)pRS->GetCollect("Conclusion");str.Trim();
				m_ResultCtrl.SetItemText(nRow,4,str);
				str=(LPCTSTR)(_bstr_t)pRS->GetCollect("POR");str.Trim();
				m_ResultCtrl.SetItemText(nRow,5,str);
				str=(LPCTSTR)(_bstr_t)pRS->GetCollect("PERMA");str.Trim();
				m_ResultCtrl.SetItemText(nRow,6,str);
				str=(LPCTSTR)(_bstr_t)pRS->GetCollect("SW");str.Trim();
				m_ResultCtrl.SetItemText(nRow,7,str);
				str=(LPCTSTR)(_bstr_t)pRS->GetCollect("GR");str.Trim();
				m_ResultCtrl.SetItemText(nRow,8,str);
				str=(LPCTSTR)(_bstr_t)pRS->GetCollect("DEN");str.Trim();
				m_ResultCtrl.SetItemText(nRow,9,str);
				str=(LPCTSTR)(_bstr_t)pRS->GetCollect("CNL");str.Trim();
				m_ResultCtrl.SetItemText(nRow,10,str);
				str=(LPCTSTR)(_bstr_t)pRS->GetCollect("AC");str.Trim();
				m_ResultCtrl.SetItemText(nRow,11,str);
				str=(LPCTSTR)(_bstr_t)pRS->GetCollect("RT");str.Trim();
				m_ResultCtrl.SetItemText(nRow,12,str);
				str=(LPCTSTR)(_bstr_t)pRS->GetCollect("Describe");str.Trim();
				m_ResultCtrl.SetItemText(nRow,13,str);
				pRS->MoveNext();nRow++;
			}
		}
	}
	ado.Disconnect();
}