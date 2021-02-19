// DevBasePage.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "DevDazDraw.h"
#include "DevBasePage.h"
#include "ImageManDoc.h"
#include "FIDIO.h"
#include "Database.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDevBasePage property page
IMPLEMENT_DYNAMIC(CDevBasePage, CPropertyPage)

CDevBasePage::CDevBasePage(CDevDazObj* pFIDObj) : CPropertyPage(CDevBasePage::IDD)
{
	//{{AFX_DATA_INIT(CDevBasePage)
	m_AliasName = _T("");
	m_Edep = 0.0f;
	m_Sdep = 0.0f;
	m_Unit = _T("");
	m_WellName = _T("");
	m_DevName = _T("");
	m_DazName=_T("");
	//}}AFX_DATA_INIT
	ASSERT(pFIDObj != NULL);
	pObj = pFIDObj;
}

CDevBasePage::~CDevBasePage()
{
}

void CDevBasePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDevBasePage)
	DDX_Control(pDX, IDC_SLIDER_SDEP, m_SliderSdep);
	DDX_Control(pDX, IDC_SLIDER_EDEP, m_SliderEdep);
	DDX_Control(pDX, IDC_SDEP, m_SdepCtrl);
	DDX_Control(pDX, IDC_EDEP, m_EdepCtrl);
	DDX_Text(pDX, IDC_ALIAS_NAME, m_AliasName);
	DDX_Text(pDX, IDC_EDEP, m_Edep);
	DDX_Text(pDX, IDC_SDEP, m_Sdep);
	DDX_Text(pDX, IDC_EDIT1, m_Unit);
	DDX_Text(pDX, IDC_WELLNAME, m_WellName);
	DDX_Text(pDX, IDC_NAME, m_DevName);
	DDX_Text(pDX, IDC_NAME1, m_DazName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDevBasePage, CPropertyPage)
	//{{AFX_MSG_MAP(CDevBasePage)
	ON_EN_CHANGE(IDC_SDEP, OnChangeSdep)
	ON_EN_CHANGE(IDC_EDEP, OnChangeEdep)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	ON_BN_CLICKED(IDC_BROWSE_WELL, OnBrowseWell)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_WELLNAME, OnChangeWellname)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDevBasePage message handlers

void CDevBasePage::OnBrowseWell() 
{
	char path[512];
	CFIDIO::GetFileName(m_WellName,path,'I');
	strcat(path,".FID");

	CFileDialog dlgFile(TRUE);
	dlgFile.m_ofn.Flags |= OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	CString strFilter = "Well File(*.FID)";
	strFilter += (TCHAR)'\0';   // next string please
	strFilter += _T("*.FID");
	strFilter += (TCHAR)'\0';   // last string
	dlgFile.m_ofn.nMaxCustFilter++;
	strFilter += _T("All File(*.*)");
	strFilter += (TCHAR)'\0';   // next string please
	strFilter += _T("*.*");
	strFilter += (TCHAR)'\0';   // last string
	dlgFile.m_ofn.nMaxCustFilter++;

	dlgFile.m_ofn.lpstrFilter = strFilter;
	dlgFile.m_ofn.lpstrTitle = "选择井名";
	dlgFile.m_ofn.lpstrFile = path;

	dlgFile.m_ofn.lpstrInitialDir=GetCurrentUserPath();

	BOOL bResult = dlgFile.DoModal() == IDOK ? TRUE : FALSE;
	if (bResult) 
	{
		CFIDIO::GetFullWellName(path,m_WellName.GetBuffer(512));
		m_WellName.ReleaseBuffer();
		CEdit* pEdit = (CEdit*) GetDlgItem(IDC_WELLNAME);
		if (pEdit != NULL) pEdit->SetWindowText(m_WellName);
		if (pObj->pDoc->GetDocumentType() == TEMPLATE_DOC)	return;
		if (InitDepthSlider()) 
		{
			SetSliderDepth(m_Sdep,m_Edep);
			if ((pObj->GetParentObj() == NULL) || (pObj->GetParentType() != 0)) 
				EnableDepth(TRUE);
		}
		else EnableDepth(FALSE);
		AddCurveList(m_WellName);
	}
}

BOOL CDevBasePage::OnInitDialog() 
{
	CFIDIO::GetFullWellName(pObj->m_strPathName, m_WellName.GetBuffer(512));
	m_WellName.ReleaseBuffer();
	if (pObj->pDoc->GetDocumentType() == TEMPLATE_DOC)
	{
		m_WellName = "Template";
		CWnd* pWnd = GetDlgItem(IDC_BROWSE_WELL);
		if (pWnd != NULL) pWnd->ShowWindow(FALSE);
		pWnd = GetDlgItem(IDC_WELLNAME);
		if (pWnd != NULL) pWnd->EnableWindow(FALSE);
	}
	else AddCurveList(pObj->m_strPathName);

	CFIDIO::GetFIDName(pObj->m_strPathName, m_DevName.GetBuffer(512));
	m_DevName.ReleaseBuffer();
	CFIDIO::GetFIDName(pObj->m_DazName, m_DazName.GetBuffer(512));
	m_DazName.ReleaseBuffer();
	m_AliasName = pObj->m_AliasName;
	m_Unit = pObj->m_UnitText;
	m_Sdep = (float)pObj->m_PlotSdep;
	m_Edep = (float)pObj->m_PlotEdep;
	CPropertyPage::OnInitDialog();
	if (pObj->pDoc->GetDocumentType() == TEMPLATE_DOC)	HideDepth();
	else if (InitDepthSlider()) 
	{
		SetSliderDepth(m_Sdep,m_Edep);
		pObj->m_PlotSdep=m_Sdep;
		pObj->m_PlotEdep=m_Edep;
		if ((pObj->GetParentObj() == NULL) || (pObj->GetParentType() != 0)) 
			EnableDepth(TRUE);
		else EnableDepth(FALSE);
	}
	else EnableDepth(FALSE);
	return TRUE; 
}

void CDevBasePage::OnOK() 
{
	CPropertyPage::OnOK();
	if ((m_Sdep < 0) || (m_Edep < m_Sdep))
	{
		::AfxMessageBox(IDS_DEPTH_SCOPE_ERROR,MB_OK | MB_ICONSTOP);
		//::MessageBox(NULL,"无效的深度范围,请重新输入!","信息框",MB_OK | MB_ICONSTOP);
		return;
	}
	pObj->m_PlotSdep = m_Sdep;
	pObj->m_PlotEdep = m_Edep;	
	pObj->m_UnitText = m_Unit;
	pObj->m_AliasName = m_AliasName;
	if (m_WellName == "Template") m_WellName = "########";
//	if (pObj->m_strPathName != m_WellName + "." + m_DevName)
	{
		pObj->m_strPathName = m_WellName + "." + m_DevName;
		pObj->m_DazName = m_WellName + "." + m_DazName;
		pObj->m_bFlush = TRUE;
	}
}

void CDevBasePage::OnChangeSdep() 
{
	CWnd* pWnd = GetFocus();
	if ((pWnd != NULL) && (pWnd == &m_SdepCtrl))
	{
		UpdateData(TRUE);
		m_SliderSdep.SetPos((int)m_Sdep);
	}
}

void CDevBasePage::OnChangeEdep() 
{
	CWnd* pWnd = GetFocus();
	if ((pWnd != NULL) && (pWnd == &m_EdepCtrl))
	{
		UpdateData(TRUE);
		m_SliderEdep.SetPos((int)m_Edep);
	}
}

void CDevBasePage::OnChangeName() 
{
	CString str;
	CEdit* pEdit = (CEdit*) GetDlgItem(IDC_NAME);
	if ((pEdit == NULL) || (pEdit != GetFocus())) return;
	pEdit->GetWindowText(str);
	if (m_DevName !=  str)
	{
		m_DevName = str;
		if (InitDepthSlider()) 
		{
			SetSliderDepth(m_Sdep,m_Edep);
			if ((pObj->GetParentObj() == NULL) || (pObj->GetParentType() != 0)) 
				EnableDepth(TRUE);
		}
		else EnableDepth(FALSE);
	}
}

void CDevBasePage::HideDepth()
{
	CWnd* pWnd = GetDlgItem(IDC_SDEP);
	pWnd->ShowWindow(FALSE);
	pWnd = GetDlgItem(IDC_EDEP);
	pWnd->ShowWindow(FALSE);
	pWnd = GetDlgItem(IDC_STATIC_DEPTH);
	pWnd->ShowWindow(FALSE);
	pWnd = GetDlgItem(IDC_STATIC_DEPTO);
	pWnd->ShowWindow(FALSE);
	pWnd = GetDlgItem(IDC_SLIDER_EDEP);
	pWnd->ShowWindow(FALSE);
	pWnd = GetDlgItem(IDC_SLIDER_SDEP);
	pWnd->ShowWindow(FALSE);
	pWnd = GetDlgItem(IDC_STATIC_SDEP);
	pWnd->ShowWindow(FALSE);
	pWnd = GetDlgItem(IDC_STATIC_EDEP);
	pWnd->ShowWindow(FALSE);
}

void CDevBasePage::EnableDepth(BOOL bf)
{
	CWnd* pWnd = GetDlgItem(IDC_SDEP);
	pWnd->EnableWindow(bf);
	pWnd = GetDlgItem(IDC_EDEP);
	pWnd->EnableWindow(bf);
	pWnd = GetDlgItem(IDC_SLIDER_EDEP);
	pWnd->EnableWindow(bf);
	pWnd = GetDlgItem(IDC_SLIDER_SDEP);
	pWnd->EnableWindow(bf);
}

BOOL CDevBasePage::InitDepthSlider()
{
	int dep1=0,dep2=9999;
	CFIDIO* pFIDIO = new CFIDIO(m_WellName + "." + m_DevName );
	if (pFIDIO->Open())
	{
		dep1 = (int) pFIDIO->GetStartDepth();
		dep2 = (int) pFIDIO->GetEndDepth();
	}
	else
	{
		double sdep,edep;
		pObj->pDoc->GetDepthRangle(sdep, edep);
		dep1 = (int)sdep; 
		dep2 = (int)edep;
	}
	m_SliderSdep.SetRange(dep1,dep2);			
	m_SliderSdep.SetLineSize(1);			
	m_SliderSdep.SetPageSize((dep2-dep1) / 10);			
	m_SliderEdep.SetRange(dep1,dep2);			
	m_SliderSdep.SetLineSize(1);			
	m_SliderSdep.SetPageSize((dep2-dep1)/10);			
	delete pFIDIO;
	char s1[10],s2[10];
	itoa(dep1,s1,10); 
	itoa(dep2,s2,10);
	GetDlgItem(IDC_STATIC_SDEP)->SetWindowText(s1);
	GetDlgItem(IDC_STATIC_EDEP)->SetWindowText(s2);
	return TRUE;
}

void CDevBasePage::SetSliderDepth(float sdep, float edep)
{
	m_SliderSdep.SetPos((int)sdep);
	m_SliderEdep.SetPos((int)edep);
}

void CDevBasePage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CWnd* pWnd = GetFocus();
	if (pWnd == NULL) return;
	char s[10]; 
	if (pWnd == &m_SliderSdep)
	{
		m_Sdep = (float) m_SliderSdep.GetPos();
		sprintf(s,"%7.2f",m_Sdep);
		m_SdepCtrl.SetWindowText(s);
	}
	else if (pWnd == &m_SliderEdep)
	{
		m_Edep = (float) m_SliderEdep.GetPos();
		sprintf(s,"%7.2f",m_Edep);
		m_EdepCtrl.SetWindowText(s);
	}
}

void CDevBasePage::OnChangeWellname() 
{
	CString str;
	CEdit* pEdit = (CEdit*) GetDlgItem(IDC_WELLNAME);
	if ((pEdit == NULL) || (pEdit != GetFocus())) return;
	pEdit->GetWindowText(str);
	if (m_WellName !=  str)
	{
		m_WellName = str;
		if (InitDepthSlider()) 
		{
			SetSliderDepth(m_Sdep,m_Edep);
			if ((pObj->GetParentObj() == NULL) || (pObj->GetParentType() != 0)) 
				EnableDepth(TRUE);
		}
		else EnableDepth(FALSE);
	}
}

void CDevBasePage::AddCurveList(CString path)
{
	CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_NAME);
	CComboBox* pBox1 = (CComboBox*) GetDlgItem(IDC_NAME1);
	if (pBox == NULL) return;
	pBox->ResetContent();
	if (pBox1 == NULL) return;
	pBox1->ResetContent();

	CString szDir;
	CFIDIO::GetPath(m_WellName,szDir.GetBuffer(512));
	szDir.ReleaseBuffer();

	CDbaseFile m_dBaseFile;
	if(m_dBaseFile.Open(szDir+"Curve.dbf")!=DBASE_SUCCESS)
		return;
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
			pBox1->AddString(Name);
		}
	}
	m_dBaseFile.Close();
	pBox->SetWindowText(m_DevName);
	pBox1->SetWindowText(m_DazName);
}
