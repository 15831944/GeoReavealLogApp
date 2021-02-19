// DipBasePage.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "FIDDraw.h"
#include "AngDraw.h"
#include "DipBasePage.h"
#include "ImageManDoc.h"
#include "Database.h"
#include "FIDIO.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDipBasePage property page
IMPLEMENT_DYNAMIC(CDipBasePage, CPropertyPage)

CDipBasePage::CDipBasePage(CDangDdirObj* pFIDObj) : CPropertyPage(CDipBasePage::IDD)
{
	//{{AFX_DATA_INIT(CDipBasePage)
	m_AliasName = _T("");
	m_Edep = 0.0f;
	m_Sdep = 0.0f;
	m_Unit = _T("");
	m_WellName = _T("");
	m_Name = _T("");
	m_MinGrade = 0.0f;
	m_GradName = _T("");
	m_DirName = _T("");
	//}}AFX_DATA_INIT
	ASSERT(pFIDObj != NULL);
	pObj = pFIDObj;
}

CDipBasePage::~CDipBasePage()
{
}

void CDipBasePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDipBasePage)
	DDX_Control(pDX, IDC_SLIDER_SDEP, m_SliderSdep);
	DDX_Control(pDX, IDC_SLIDER_EDEP, m_SliderEdep);
	DDX_Control(pDX, IDC_SDEP, m_SdepCtrl);
	DDX_Control(pDX, IDC_EDEP, m_EdepCtrl);
	DDX_Text(pDX, IDC_ALIAS_NAME, m_AliasName);
	DDX_Text(pDX, IDC_EDEP, m_Edep);
	DDX_Text(pDX, IDC_SDEP, m_Sdep);
	DDX_Text(pDX, IDC_EDIT1, m_Unit);
	DDX_Text(pDX, IDC_NAME1, m_DirName);
	DDX_Text(pDX, IDC_WELLNAME, m_WellName);
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDX_Text(pDX, IDC_STANDARD, m_MinGrade);
	DDX_CBString(pDX, IDC_DIPGRAD, m_GradName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDipBasePage, CPropertyPage)
	//{{AFX_MSG_MAP(CDipBasePage)
	ON_EN_CHANGE(IDC_SDEP, OnChangeSdep)
	ON_EN_CHANGE(IDC_EDEP, OnChangeEdep)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	ON_BN_CLICKED(IDC_BROWSE_WELL, OnBrowseWell)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_WELLNAME, OnChangeWellname)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDipBasePage message handlers

void CDipBasePage::OnBrowseWell() 
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

BOOL CDipBasePage::OnInitDialog() 
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

	CFIDIO::GetFIDName(pObj->m_strPathName, m_Name.GetBuffer(512));
	m_Name.ReleaseBuffer();
	CFIDIO::GetFIDName(pObj->m_DirName, m_DirName.GetBuffer(512));
	m_DirName.ReleaseBuffer();
	CFIDIO::GetFIDName(pObj->m_GradName, m_GradName.GetBuffer(512));
	m_GradName.ReleaseBuffer();
	m_AliasName = pObj->m_AliasName;
	m_Unit = pObj->m_UnitText;
	m_Sdep = (float)pObj->m_PlotSdep;
	m_Edep = (float)pObj->m_PlotEdep;
	m_MinGrade=pObj->m_MinGrade;
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

void CDipBasePage::OnOK() 
{
	CPropertyPage::OnOK();
	if ((m_Sdep < 0) || (m_Edep < m_Sdep))
	{
		::AfxMessageBox(IDS_DEPTH_SCOPE_ERROR,MB_OK | MB_ICONSTOP);
		return;
	}
	pObj->m_PlotSdep = m_Sdep;
	pObj->m_PlotEdep = m_Edep;	
	pObj->m_UnitText = m_Unit;
	pObj->m_AliasName = m_AliasName;
	pObj->m_MinGrade=m_MinGrade;
	if (m_WellName == "Template") m_WellName = "########";
//	if (pObj->m_strPathName != m_WellName + "." + m_Name)
	{
		pObj->m_strPathName = m_WellName + "." + m_Name;
		pObj->m_DirName = m_WellName + "." + m_DirName;
		pObj->m_GradName = m_WellName + "." + m_GradName;
		pObj->m_bFlush = TRUE;
	}
}

void CDipBasePage::OnChangeSdep() 
{
	CWnd* pWnd = GetFocus();
	if ((pWnd != NULL) && (pWnd == &m_SdepCtrl))
	{
		UpdateData(TRUE);
		m_SliderSdep.SetPos((int)m_Sdep);
	}
}

void CDipBasePage::OnChangeEdep() 
{
	CWnd* pWnd = GetFocus();
	if ((pWnd != NULL) && (pWnd == &m_EdepCtrl))
	{
		UpdateData(TRUE);
		m_SliderEdep.SetPos((int)m_Edep);
	}
}

void CDipBasePage::OnChangeName() 
{
	CString str;
	CEdit* pEdit = (CEdit*) GetDlgItem(IDC_NAME);
	if ((pEdit == NULL) || (pEdit != GetFocus())) return;
	pEdit->GetWindowText(str);
	if (m_Name !=  str)
	{
		m_Name = str;
		if (InitDepthSlider()) 
		{
			SetSliderDepth(m_Sdep,m_Edep);
			if ((pObj->GetParentObj() == NULL) || (pObj->GetParentType() != 0)) 
				EnableDepth(TRUE);
		}
		else EnableDepth(FALSE);
	}
}

void CDipBasePage::HideDepth()
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

void CDipBasePage::EnableDepth(BOOL bf)
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

BOOL CDipBasePage::InitDepthSlider()
{
	int dep1=0,dep2=9999;
	CFIDIO* pFIDIO = new CFIDIO(m_WellName + "." + m_Name );
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

void CDipBasePage::SetSliderDepth(float sdep, float edep)
{
	m_SliderSdep.SetPos((int)sdep);
	m_SliderEdep.SetPos((int)edep);
}

void CDipBasePage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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

void CDipBasePage::OnChangeWellname() 
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

void CDipBasePage::AddCurveList(CString path)
{
	CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_NAME);
	CComboBox* pBox1 = (CComboBox*) GetDlgItem(IDC_NAME1);
	CComboBox* pBox2 = (CComboBox*) GetDlgItem(IDC_DIPGRAD);
	if (pBox == NULL) return;
	pBox->ResetContent();
	if (pBox1 == NULL) return;
	pBox1->ResetContent();
	if (pBox2 == NULL) return;
	pBox2->ResetContent();
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
			pBox2->AddString(Name);
		}
	}
	m_dBaseFile.Close();
	pBox->SetWindowText(m_Name);
	pBox1->SetWindowText(m_DirName);
	pBox2->SetWindowText(m_GradName);
}
