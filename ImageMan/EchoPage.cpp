// EchoPage.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "ImageManDoc.h"
#include "style.h"
#include "styledialog.h"
#include "EchoPage.h"
#include "EchoDraw.h"
#include "FIDIO.h"
#include "Database.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEchoPage property page

IMPLEMENT_DYNCREATE(CEchoPage, CPropertyPage)

CEchoPage::CEchoPage(CEchoObj* pEchoObj) : CPropertyPage(CEchoPage::IDD)
{
	//{{AFX_DATA_INIT(CEchoPage)
	m_WfMax = 10.f;
	m_Level = 0;
	m_PlotAmp = 0;
	m_EchoType = -1;
	m_LeftVal = 0;
	m_RightVal = 0;
	m_WfMin = 0.0f;
	m_EchoWidth = 1;
	PALSIZE = 0;
	m_FillScale = FALSE;
	m_EchoFilter = FALSE;
	//}}AFX_DATA_INIT
	ASSERT(pEchoObj != NULL);
	pObj = pEchoObj;
	m_bXlog=FALSE;
	m_bYlog=FALSE;
}

CEchoPage::~CEchoPage()  
{
}

void CEchoPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEchoPage)
	DDX_Text(pDX, IDC_WFAMP, m_WfMax);
	DDX_Text(pDX, IDC_LEVEL, m_Level);
	DDV_MinMaxUInt(pDX, m_Level, 0, 100);
	DDX_Text(pDX, IDC_PLOTAMP, m_PlotAmp);
	DDV_MinMaxUInt(pDX, m_PlotAmp, 0, 5000);
	DDX_Radio(pDX, IDC_WAVE, m_EchoType);
	DDX_Text(pDX, IDC_LEFT, m_LeftVal);
	DDX_Text(pDX, IDC_RIGHT, m_RightVal);
	DDX_Text(pDX, IDC_STARTIME, m_WfMin);
	DDX_Text(pDX, IDC_WAVE_WIDTH, m_EchoWidth);
	DDV_MinMaxInt(pDX, m_EchoWidth, 0, 20);
	DDX_Check(pDX, IDC_CHECK1, m_FillScale);
	DDX_Control(pDX, IDC_EOCH_COLOR, m_Color);
	DDX_Check(pDX, IDC_CHECK2, m_EchoFilter);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK3, m_bXlog);
	DDX_Check(pDX, IDC_CHECK4, m_bYlog);
	DDX_Control(pDX,IDC_EOCH_COLOR2,m_FillColor);
}


BEGIN_MESSAGE_MAP(CEchoPage, CPropertyPage)
	//{{AFX_MSG_MAP(CEchoPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEchoPage message handlers

BOOL CEchoPage::OnInitDialog() 
{
	m_Color.SetColor(pObj->m_EchoColor);
	m_FillColor.SetColor(pObj->m_FillColor);
	m_EchoType = pObj->m_EchoType;
	m_EchoWidth=pObj->m_EchoWidth;
	m_Level = pObj->m_Level;
	m_LeftVal = pObj->m_LeftValue;
	m_RightVal = pObj->m_RightValue;
	m_WfMax = pObj->m_WfMax;
	m_PlotAmp = pObj->m_Amp;
	m_WfMin=pObj->m_WfMin;
	m_FillScale=pObj->m_FillScale;
	m_EchoFilter=pObj->m_EchoFilter;
	m_bXlog=pObj->m_bXlog;
	m_bYlog=pObj->m_bYlog;
	CPropertyPage::OnInitDialog();
	return TRUE;
}

void CEchoPage::OnOK() 
{
	CString m_Name;
	pObj->m_EchoColor = m_Color.GetColor();
	pObj->m_FillColor=m_FillColor.GetColor();
	pObj->m_EchoType = m_EchoType;
	pObj->m_EchoWidth=m_EchoWidth;
	pObj->m_Level = m_Level;
	pObj->m_LeftValue = m_LeftVal;
	pObj->m_RightValue = m_RightVal;
	pObj->m_Amp = m_PlotAmp;
	pObj->m_WfMax = m_WfMax;
	pObj->m_WfMin=m_WfMin;
	pObj->m_FillScale=m_FillScale;
	pObj->m_EchoFilter=m_EchoFilter;
	pObj->m_bXlog=m_bXlog;
	pObj->m_bYlog=m_bYlog;
	CPropertyPage::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
// CEchoBase property page

IMPLEMENT_DYNCREATE(CEchoBase, CPropertyPage)

CEchoBase::CEchoBase(CEchoObj* pEchoObj) : CPropertyPage(CEchoBase::IDD)
{
	//{{AFX_DATA_INIT(CEchoBase)
	m_AliasName = _T("");
	m_Unit = _T("");
	m_WellName = _T("");
	m_Edep = 0.0f;
	m_Sdep = 0.0f;
	//}}AFX_DATA_INIT
	ASSERT(pEchoObj != NULL);
	pObj = pEchoObj;
}

CEchoBase::~CEchoBase()
{
}

void CEchoBase::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEchoBase)
	DDX_Control(pDX, IDC_SDEP, m_SdepCtrl);
	DDX_Control(pDX, IDC_EDEP, m_EdepCtrl);
	DDX_Control(pDX, IDC_SLIDER_SDEP, m_SliderSdep);
	DDX_Control(pDX, IDC_SLIDER_EDEP, m_SliderEdep);
	DDX_CBString(pDX, IDC_NAME, m_Name);
	DDX_Text(pDX, IDC_ALIAS_NAME, m_AliasName);
	DDX_Text(pDX, IDC_UNIT, m_Unit);
	DDX_Text(pDX, IDC_WELLNAME, m_WellName);
	DDX_Text(pDX, IDC_EDEP, m_Edep);
	DDX_Text(pDX, IDC_SDEP, m_Sdep);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEchoBase, CPropertyPage)
	//{{AFX_MSG_MAP(CEchoBase)
	ON_BN_CLICKED(IDC_BROWSE_WELL, OnBrowseWell)
	ON_EN_CHANGE(IDC_SDEP, OnChangeSdep)
	ON_EN_CHANGE(IDC_EDEP, OnChangeEdep)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_WELLNAME, OnChangeWellname)
	ON_CBN_CLOSEUP(IDC_NAME, OnCloseupName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEchoBase message handlers

void CEchoBase::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertyPage::OnOK();
	pObj->m_PlotSdep = m_Sdep;
	pObj->m_PlotEdep = m_Edep;
	pObj->m_UnitText = m_Unit;
	pObj->m_AliasName = m_AliasName;
	if (m_WellName == "Template") m_WellName = "########";
	pObj->m_strPathName = m_WellName + "." + m_Name; 
}

BOOL CEchoBase::OnInitDialog() 
{
	CString path;
	CFIDIO::GetFullWellName(pObj->m_strPathName,m_WellName.GetBuffer(512));
	m_WellName.ReleaseBuffer();
	CFIDIO::GetFIDName(pObj->m_strPathName,m_Name.GetBuffer(512));
	m_Name.ReleaseBuffer();
	if (pObj->pDoc->GetDocumentType() == TEMPLATE_DOC)
	{
		m_WellName = "Template";
		CWnd* pWnd = GetDlgItem(IDC_BROWSE_WELL);
		if (pWnd != NULL) pWnd->ShowWindow(FALSE);
		pWnd = GetDlgItem(IDC_WELLNAME);
		if (pWnd != NULL) pWnd->EnableWindow(FALSE);
	}
	else AddCurveList(pObj->m_strPathName);

	m_AliasName = pObj->m_AliasName;
	m_Unit = pObj->m_UnitText;

	m_Sdep = (float)pObj->m_PlotSdep;
	m_Edep = (float)pObj->m_PlotEdep;
	CPropertyPage::OnInitDialog();
	if (pObj->pDoc->GetDocumentType() == TEMPLATE_DOC)	HideDepth();
	else if (InitDepthSlider()) 
	{
		EnableDepth(TRUE);
		SetSliderDepth(m_Sdep,m_Edep);
	}
	else EnableDepth(FALSE);
	return TRUE; 
}

void CEchoBase::OnBrowseWell() 
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
	dlgFile.m_ofn.lpstrTitle = "Select Well File";
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

void CEchoBase::OnChangeSdep() 
{
	CWnd* pWnd = GetFocus();
	if ((pWnd != NULL) && (pWnd == &m_SdepCtrl))
	{
		UpdateData(TRUE);
		m_SliderSdep.SetPos((int)m_Sdep);
	}
}

void CEchoBase::OnChangeEdep() 
{
	CWnd* pWnd = GetFocus();
	if ((pWnd != NULL) && (pWnd == &m_EdepCtrl))
	{
		UpdateData(TRUE);
		m_SliderEdep.SetPos((int)m_Edep);
	}
}

void CEchoBase::HideDepth()
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

void CEchoBase::EnableDepth(BOOL bf)
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

BOOL CEchoBase::InitDepthSlider()
{
	BOOL bf = FALSE;
	int dep1=0,dep2=9999;
	CFIDIO* pFIDIO = new CFIDIO(m_WellName + "." + m_Name );
	if (pFIDIO->Open())
	{
		dep1 = (int) pFIDIO->GetStartDepth();
		dep2 = (int) pFIDIO->GetEndDepth();
		m_SliderSdep.SetRange(dep1,dep2);			
		m_SliderSdep.SetLineSize(1);			
		m_SliderSdep.SetPageSize((dep2-dep1) / 10);			
		m_SliderEdep.SetRange(dep1,dep2);			
		m_SliderSdep.SetLineSize(1);			
		m_SliderSdep.SetPageSize((dep2-dep1)/10);			
		bf = TRUE;
	}
	delete pFIDIO;
	char s1[10],s2[10];
	itoa(dep1,s1,10); 
	itoa(dep2,s2,10);
	GetDlgItem(IDC_STATIC_SDEP)->SetWindowText(s1);
	GetDlgItem(IDC_STATIC_EDEP)->SetWindowText(s2);
	return bf;
}

void CEchoBase::SetSliderDepth(float sdep, float edep)
{
	m_SliderSdep.SetPos((int)sdep);
	m_SliderEdep.SetPos((int)edep);
}

void CEchoBase::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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


void CEchoBase::OnChangeWellname() 
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

void CEchoBase::OnCloseupName() 
{
	// TODO: Add your control notification handler code here
	CString str;
	CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_NAME);
	int nIndex = pBox->GetCurSel();
	if (nIndex == CB_ERR)
		pBox->GetWindowText(str);
	else
		pBox->GetLBText(nIndex, str);
	str.MakeUpper();
	if (m_Name != str)
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

void CEchoBase::AddCurveList(CString path)
{
	CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_NAME);
	if (pBox == NULL) return;
	pBox->ResetContent();

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
		if(Type=="阵列")
			pBox->AddString (Name);
	}
	m_dBaseFile.Close();
	pBox->SetWindowText(m_Name);
}
