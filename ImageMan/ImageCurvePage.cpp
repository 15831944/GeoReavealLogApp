// ImagePage.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "ImageManDoc.h"
#include "style.h"
#include "styledialog.h"
#include "ImageCurvePage.h"
#include "ImageCurveDraw.h"
#include "Database.h"
#include "FIDIO.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageCurvePage property page

IMPLEMENT_DYNCREATE(CImageCurvePage, CPropertyPage)

CImageCurvePage::CImageCurvePage(CImageCurveObj* pImageObj) : CPropertyPage(CImageCurvePage::IDD)
{
	//{{AFX_DATA_INIT(CImageCurvePage)	
	m_LeftValue = 0.f;
	m_RightValue = 1000.f;
	m_sCurveArray = _T("");
	m_HorOff = 10;
	m_ScaleType = 0;
	m_bGDIPlus = FALSE;
	m_HorOffVal = 0.0f;
	//}}AFX_DATA_INIT
	ASSERT(pImageObj != NULL);
	pObj = pImageObj;
}

CImageCurvePage::~CImageCurvePage()  
{
}

void CImageCurvePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImageCurvePage)	
	DDX_Text(pDX, IDC_LEFT, m_LeftValue);
	DDX_Text(pDX, IDC_RIGHT, m_RightValue);
	DDX_Text(pDX, IDC_BIT, m_sCurveArray);
	DDX_Text(pDX, IDC_HOR_SCALE, m_HorOff);
	DDX_CBIndex(pDX, IDC_SCALE_TYPE, m_ScaleType);
	DDX_Check(pDX, IDC_CHECK1, m_bGDIPlus);
	DDX_Text(pDX, IDC_EDIT1, m_HorOffVal);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_CURVE_COLOR, m_CurveColorCtrl);
	DDX_Control(pDX, IDC_CURVE_COLOR2,m_FillColorCtrl);
}


BEGIN_MESSAGE_MAP(CImageCurvePage, CPropertyPage)
	//{{AFX_MSG_MAP(CImageCurvePage)
	//}}AFX_MSG_MAP
	ON_CBN_CLOSEUP(IDC_SCALE_TYPE, OnCbnCloseupScaleType)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageCurvePage message handlers

BOOL CImageCurvePage::OnInitDialog() 
{
	m_LeftValue = pObj->m_LeftVal;
	m_RightValue = pObj->m_RightVal;
	m_sCurveArray=pObj->m_sCurveArray ;
	m_HorOff=pObj->m_HorOff ;
	m_ScaleType=pObj->m_ScaleType;	
	m_bGDIPlus=pObj->m_bGDIPlus;
	m_HorOffVal=pObj->m_HorOffVal;
	CPropertyPage::OnInitDialog();
	m_CurveColorCtrl.SetColor(pObj->m_CurveColor);
	m_FillColorCtrl.SetColor(pObj->m_FillColor);
	this->GetDlgItem(IDC_EDIT1)->EnableWindow(m_ScaleType==0);
	return TRUE;
}

void CImageCurvePage::OnOK() 
{
	CString m_Name;	
	pObj->m_LeftVal = m_LeftValue;
	pObj->m_RightVal = m_RightValue;
	pObj->m_sCurveArray =m_sCurveArray;
	pObj->m_HorOff =m_HorOff;
	pObj->m_ScaleType =m_ScaleType;	
	pObj->m_bGDIPlus=m_bGDIPlus;
	pObj->m_HorOffVal=m_HorOffVal;
	CPropertyPage::OnOK();
	pObj->m_CurveColor = m_CurveColorCtrl.GetColor();
	pObj->m_FillColor = m_FillColorCtrl.GetColor();
}

void CImageCurvePage::OnCbnCloseupScaleType()
{
	// TODO: 在此添加控件通知处理程序代码
	this->UpdateData();
	this->GetDlgItem(IDC_EDIT1)->EnableWindow(m_ScaleType==0);
}

/////////////////////////////////////////////////////////////////////////////
// CImageCurveBase property page

IMPLEMENT_DYNCREATE(CImageCurveBase, CPropertyPage)

CImageCurveBase::CImageCurveBase(CImageCurveObj* pImageObj) : CPropertyPage(CImageCurveBase::IDD)
{
	//{{AFX_DATA_INIT(CImageCurveBase)
	m_AliasName = _T("");
	m_Unit = _T("");
	m_WellName = _T("");
	m_Name = _T("");
	//}}AFX_DATA_INIT
	ASSERT(pImageObj != NULL);
	pObj = pImageObj;
}

CImageCurveBase::~CImageCurveBase()
{
}

void CImageCurveBase::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImageCurveBase)
	DDX_Text(pDX, IDC_ALIAS_NAME, m_AliasName);
	DDX_Text(pDX, IDC_UNIT, m_Unit);
	DDX_Text(pDX, IDC_WELLNAME, m_WellName);
	DDX_CBString(pDX, IDC_NAME, m_Name);	
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SLIDER_SDEP, m_SliderSdep);
	DDX_Control(pDX, IDC_SLIDER_EDEP, m_SliderEdep);
	DDX_Control(pDX, IDC_SDEP, m_SdepCtrl);
	DDX_Control(pDX, IDC_EDEP, m_EdepCtrl);
	DDX_Text(pDX, IDC_EDEP, m_Edep);
	DDX_Text(pDX, IDC_SDEP, m_Sdep);
}


BEGIN_MESSAGE_MAP(CImageCurveBase, CPropertyPage)
	//{{AFX_MSG_MAP(CImageCurveBase)
	ON_BN_CLICKED(IDC_BROWSE_WELL, OnBrowseWell)
	ON_EN_CHANGE(IDC_WELLNAME, OnChangeWellname)
	ON_CBN_CLOSEUP(IDC_NAME, OnCloseupName)
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_EDEP, OnChangeEdep)
	ON_EN_CHANGE(IDC_SDEP, OnChangeSdep)
	ON_WM_HSCROLL()
	ON_CBN_EDITCHANGE(IDC_NAME, OnEditchangeName)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageCurveBase message handlers

void CImageCurveBase::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class	
	pObj->m_UnitText = m_Unit;
	pObj->m_AliasName = m_AliasName;
	pObj->m_strPathName = m_WellName+"."+m_Name;
	CPropertyPage::OnOK();
}

BOOL CImageCurveBase::OnInitDialog() 
{
	CString path;
	CFIDIO::GetFullWellName(pObj->m_strPathName,m_WellName.GetBuffer(512));
	m_WellName.ReleaseBuffer();
	CFIDIO::GetFIDName(pObj->m_strPathName,m_Name.GetBuffer(512));
	m_Name.ReleaseBuffer();
	CPropertyPage::OnInitDialog();
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

	if (pObj->pDoc->GetDocumentType() == TEMPLATE_DOC)	HideDepth();
	else if (InitDepthSlider()) 
	{
		SetSliderDepth(m_Sdep,m_Edep);
		if ((pObj->GetParentObj() == NULL) || (pObj->GetParentType() != 0)) 
			EnableDepth(TRUE);
		else EnableDepth(FALSE);
	}
	else EnableDepth(FALSE);
	UpdateData (FALSE);
	return TRUE; 
}

void CImageCurveBase::OnBrowseWell() 
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

void CImageCurveBase::AddCurveList(CString path)
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

void CImageCurveBase::OnCloseupName()
{
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

void CImageCurveBase::OnChangeSdep() 
{
	CWnd* pWnd = GetFocus();
	if ((pWnd != NULL) && (pWnd == &m_SdepCtrl))
	{
		UpdateData(TRUE);
		m_SliderSdep.SetPos((int)m_Sdep);
	}
}

void CImageCurveBase::OnChangeEdep() 
{
	CWnd* pWnd = GetFocus();
	if ((pWnd != NULL) && (pWnd == &m_EdepCtrl))
	{
		UpdateData(TRUE);
		m_SliderEdep.SetPos((int)m_Edep);
	}
}

void CImageCurveBase::HideDepth()
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

void CImageCurveBase::EnableDepth(BOOL bf)
{
	CWnd* pWnd1 = GetFocus();
	CWnd* pWnd = GetDlgItem(IDC_SDEP);
	if ((pWnd == pWnd1) && (!bf)) SetFocus();
	pWnd->EnableWindow(bf);
	pWnd = GetDlgItem(IDC_EDEP);
	if ((pWnd == pWnd1) && (!bf)) SetFocus();
	pWnd->EnableWindow(bf);
	pWnd = GetDlgItem(IDC_SLIDER_EDEP);
	if ((pWnd == pWnd1) && (!bf)) SetFocus();
	pWnd->EnableWindow(bf);
	pWnd = GetDlgItem(IDC_SLIDER_SDEP);
	if ((pWnd == pWnd1) && (!bf)) SetFocus();
	pWnd->EnableWindow(bf);
}

BOOL CImageCurveBase::InitDepthSlider()
{
	BOOL bf = FALSE;
	int dep1=0,dep2=9999;
	CFIDIO* pFIDIO = new CFIDIO(m_WellName + "." + m_Name );
	if (pFIDIO->Open() && (pFIDIO->GetDimenSion() == 3))
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

void CImageCurveBase::SetSliderDepth(float sdep, float edep)
{
	m_SliderSdep.SetPos((int)sdep);
	m_SliderEdep.SetPos((int)edep);
}


void CImageCurveBase::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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

void CImageCurveBase::OnChangeWellname() 
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

void CImageCurveBase::OnEditchangeName() 
{
	CString str;
	CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_NAME);
	if (pBox == NULL) return;
	pBox->GetWindowText(str);
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

