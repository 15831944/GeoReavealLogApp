// FreqBasePage.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "AzimFreq.h"
#include "FreqBasePage.h"
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
// CFreqBasePage property page
IMPLEMENT_DYNAMIC(CFreqBasePage, CPropertyPage)

CFreqBasePage::CFreqBasePage(CAzimFreqObj* pFIDObj) : CPropertyPage(CFreqBasePage::IDD)
{
	//{{AFX_DATA_INIT(CFreqBasePage)
	m_AliasName = _T("");
	m_Edep = 0.0f;
	m_Sdep = 0.0f;
	m_Unit = _T("");
	m_WellName = _T("");
	m_DipName = _T("");
	m_AziName=_T("");
	//}}AFX_DATA_INIT
	ASSERT(pFIDObj != NULL);
	pObj = pFIDObj;
}

CFreqBasePage::~CFreqBasePage()
{
}

void CFreqBasePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFreqBasePage)
	DDX_Control(pDX, IDC_SLIDER_SDEP, m_SliderSdep);
	DDX_Control(pDX, IDC_SLIDER_EDEP, m_SliderEdep);
	DDX_Control(pDX, IDC_SDEP, m_SdepCtrl);
	DDX_Control(pDX, IDC_EDEP, m_EdepCtrl);
	DDX_Text(pDX, IDC_ALIAS_NAME, m_AliasName);
	DDX_Text(pDX, IDC_EDEP, m_Edep);
	DDX_Text(pDX, IDC_SDEP, m_Sdep);
	DDX_Text(pDX, IDC_WELLNAME, m_WellName);
	DDX_Text(pDX, IDC_NAME, m_DipName);
	DDX_Text(pDX, IDC_NAME1, m_AziName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFreqBasePage, CPropertyPage)
	//{{AFX_MSG_MAP(CFreqBasePage)
	ON_EN_CHANGE(IDC_SDEP, OnChangeSdep)
	ON_EN_CHANGE(IDC_EDEP, OnChangeEdep)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	ON_BN_CLICKED(IDC_BROWSE_WELL, OnBrowseWell)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_WELLNAME, OnChangeWellname)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFreqBasePage message handlers

void CFreqBasePage::OnBrowseWell() 
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

BOOL CFreqBasePage::OnInitDialog() 
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

	CFIDIO::GetFIDName(pObj->m_strPathName, m_DipName.GetBuffer(512));
	m_DipName.ReleaseBuffer();
	m_AliasName = pObj->m_AliasName;
	m_Unit = pObj->m_UnitText;
	CFIDIO::GetFIDName(pObj->m_AziName, m_AziName.GetBuffer(512));
	m_AziName.ReleaseBuffer();
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

void CFreqBasePage::OnOK() 
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
	if (m_WellName == "Template") m_WellName = "########";
//	if (pObj->m_strPathName != m_WellName + "." + m_DipName)
	{
		pObj->m_strPathName = m_WellName + "." + m_DipName;
		pObj->m_AziName = m_WellName + "." + m_AziName;
		pObj->m_bFlush = TRUE;
	}
}

void CFreqBasePage::OnChangeSdep() 
{
	CWnd* pWnd = GetFocus();
	if ((pWnd != NULL) && (pWnd == &m_SdepCtrl))
	{
		UpdateData(TRUE);
		m_SliderSdep.SetPos((int)m_Sdep);
	}
}

void CFreqBasePage::OnChangeEdep() 
{
	CWnd* pWnd = GetFocus();
	if ((pWnd != NULL) && (pWnd == &m_EdepCtrl))
	{
		UpdateData(TRUE);
		m_SliderEdep.SetPos((int)m_Edep);
	}
}

void CFreqBasePage::OnChangeName() 
{
	CString str;
	CEdit* pEdit = (CEdit*) GetDlgItem(IDC_NAME);
	if ((pEdit == NULL) || (pEdit != GetFocus())) return;
	pEdit->GetWindowText(str);
	if (m_DipName !=  str)
	{
		m_DipName = str;
		if (InitDepthSlider()) 
		{
			SetSliderDepth(m_Sdep,m_Edep);
			if ((pObj->GetParentObj() == NULL) || (pObj->GetParentType() != 0)) 
				EnableDepth(TRUE);
		}
		else EnableDepth(FALSE);
	}
}

void CFreqBasePage::HideDepth()
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

void CFreqBasePage::EnableDepth(BOOL bf)
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

BOOL CFreqBasePage::InitDepthSlider()
{
	int dep1=0,dep2=9999;
	CFIDIO* pFIDIO = new CFIDIO(m_WellName + "." + m_DipName );
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

void CFreqBasePage::SetSliderDepth(float sdep, float edep)
{
	m_SliderSdep.SetPos((int)sdep);
	m_SliderEdep.SetPos((int)edep);
}

void CFreqBasePage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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

void CFreqBasePage::OnChangeWellname() 
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

void CFreqBasePage::AddCurveList(CString path)
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
	pBox->SetWindowText(m_DipName);
	pBox1->SetWindowText(m_AziName);
}
