// ImagePage.cpp : implementation file
//

#include "stdafx.h"
#include "PreView.h"
#include "LogPlotDoc.h"
#include "style.h"
//#include "styledialog.h"
#include "ImagePage.h"
#include "ImageDraw.h"
#include "GeoFIDIO.h"
#include "Database.h"
#include "Gradient.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImagePage property page

IMPLEMENT_DYNCREATE(CImagePage, CPropertyPage)

CImagePage::CImagePage(CImageObj* pImageObj) : CPropertyPage(CImagePage::IDD)
{
	//{{AFX_DATA_INIT(CImagePage)	
	m_LeftVal = 0;
	m_RightVal = 128;
	m_bColor = FALSE;
	m_Bit = 0;
	m_ColorGrade = 0;
	m_SchemeFile = _T("");
	m_HorScale = 10;
	//}}AFX_DATA_INIT
	ASSERT(pImageObj != NULL);
	pObj = pImageObj;
	m_SchemeFile="Wood";
}

CImagePage::~CImagePage()  
{
}

void CImagePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImagePage)	
	DDX_Control(pDX, IDC_COLORGRADE, m_GradeCtrl);
	DDX_Control(pDX, IDC_COMBO_CRMOLDL, m_CrMoldCtrl);
	DDX_Text(pDX, IDC_LEFT, m_LeftVal);
	DDX_Text(pDX, IDC_RIGHT, m_RightVal);
	DDX_Check(pDX, IDC_CHECK1, m_bColor);
	DDX_Text(pDX, IDC_BIT, m_Bit);
	DDV_MinMaxInt(pDX, m_Bit, 1, 500);
	DDX_Text(pDX, IDC_COLORGRADE, m_ColorGrade);
	DDV_MinMaxInt(pDX, m_ColorGrade, 4, 1024);
	DDX_CBString(pDX, IDC_COMBO_CRMOLDL, m_SchemeFile);
	DDX_Text(pDX, IDC_HOR_SCALE, m_HorScale);
	DDV_MinMaxInt(pDX, m_HorScale, 1, 1000);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_GRADIENT, m_wndGradientCtrl);
	DDX_Control(pDX, IDC_BKCOLOR, m_BkColorCtrl);
}


BEGIN_MESSAGE_MAP(CImagePage, CPropertyPage)
	//{{AFX_MSG_MAP(CImagePage)
	ON_CBN_CLOSEUP(IDC_COMBO_CRMOLDL, OnCloseupComboCrmoldl)
	ON_EN_CHANGE(IDC_COLORGRADE, OnChangeColorgrade)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImagePage message handlers

BOOL CImagePage::OnInitDialog() 
{
	CGeoFIDIO* pFIDIO = new CGeoFIDIO(pObj->m_strPathName);
	if(pFIDIO->Open(pObj->pDoc->m_szSever))
	{
		FIDCONTENT* Content = pFIDIO->GetContent(2);
		m_Bit = (int)Content->rlev;
	}
	else
		m_Bit=pObj->m_Bit ;
	delete pFIDIO;
	m_LeftVal = pObj->m_LeftVal;
	m_RightVal = pObj->m_RightVal;
	m_ColorGrade = pObj->m_ColorGrade;
	m_bColor=pObj->m_bColor ;
	m_HorScale=pObj->m_HorScale;
	m_SchemeFile=pObj->m_SchemeFile;
	m_BkColorCtrl.SetColor(pObj->m_BkColor);
	CPropertyPage::OnInitDialog();
	m_CrMoldCtrl.SetWindowText(m_SchemeFile);

	AddColorScheme();
	CGradient &gradient = m_wndGradientCtrl.GetGradient();	
	m_wndGradientCtrl.SetPegSide(true,false);
	CString ColorFile=GetWorkPath("Color")+m_SchemeFile+".cip";
	CFile file(ColorFile, CFile::modeRead | CFile::shareDenyWrite );
	CArchive ar(&file, CArchive::load);	
	gradient.Serialize(ar);
	gradient.SetQuantization(m_ColorGrade);
	m_wndGradientCtrl.Invalidate();
	return TRUE;
}

void CImagePage::OnOK() 
{
	CString m_Name;	
	pObj->m_LeftVal = m_LeftVal;
	pObj->m_RightVal = m_RightVal;
	pObj->m_ColorGrade = m_ColorGrade;
	pObj->m_bColor=m_bColor;
	pObj->m_Bit =m_Bit;
	pObj->m_HorScale =m_HorScale;
	pObj->m_SchemeFile=m_SchemeFile;
	pObj->m_BkColor=m_BkColorCtrl.GetColor();
	CPropertyPage::OnOK();
}

void CImagePage::OnCloseupComboCrmoldl() 
{
	// TODO: Add your control notification handler code here
	CString str;
	int nIndex=m_CrMoldCtrl.GetCurSel();
	if (nIndex == CB_ERR)
		m_CrMoldCtrl.GetWindowText(str);
	else
		m_CrMoldCtrl.GetLBText(nIndex, str);
	if(m_SchemeFile != str)
	{
		m_SchemeFile = str;
		CGradient &gradient = m_wndGradientCtrl.GetGradient();	
		CString ColorFile=GetWorkPath("Color")+m_SchemeFile+".cip";
		CFile file(ColorFile, CFile::modeRead | CFile::shareDenyWrite );
		CArchive ar(&file, CArchive::load);	
		gradient.Serialize(ar);
		m_wndGradientCtrl.Invalidate();
	}
}

void CImagePage::OnChangeColorgrade() 
{
	CString cs;
	m_GradeCtrl.GetWindowText(cs);
	int xxx=atoi(cs);
	if(xxx>3&&xxx<1024)
	{
		m_wndGradientCtrl.GetGradient().SetQuantization(xxx);
		m_wndGradientCtrl.Invalidate();
	}
}

void CImagePage::AddColorScheme()
{
	m_CrMoldCtrl.ResetContent ();
	CString szFind=GetWorkPath("Color")+"*.cip";
	struct _finddata_t c_file;
	long hFile;
	CString str;
	if ((hFile = _findfirst(szFind, &c_file )) != -1L)
	{
		str= c_file.name;
		str.MakeUpper ();
		str=str.Left (str.Find (".CIP"));
		m_CrMoldCtrl.AddString (str);
		while(_findnext(hFile,&c_file) == 0 ) 
		{
			str= c_file.name;
			str.MakeUpper ();
			str=str.Left (str.Find (".CIP"));
			m_CrMoldCtrl.AddString (str);
		}	        
		_findclose( hFile );
	}
	m_CrMoldCtrl.SetWindowText(m_SchemeFile);	
}
/////////////////////////////////////////////////////////////////////////////
// CImageBase property page

IMPLEMENT_DYNCREATE(CImageBase, CPropertyPage)

CImageBase::CImageBase(CImageObj* pImageObj) : CPropertyPage(CImageBase::IDD)
{
	//{{AFX_DATA_INIT(CImageBase)
	m_AliasName = _T("");
	m_Unit = _T("");
	m_WellName = _T("");
	m_Name = _T("");
	//}}AFX_DATA_INIT
	ASSERT(pImageObj != NULL);
	pObj = pImageObj;
}

CImageBase::~CImageBase()
{
}

void CImageBase::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImageBase)
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


BEGIN_MESSAGE_MAP(CImageBase, CPropertyPage)
	//{{AFX_MSG_MAP(CImageBase)
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
// CImageBase message handlers

void CImageBase::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class	
	CPropertyPage::OnOK();
	pObj->m_UnitText = m_Unit;
	pObj->m_AliasName = m_AliasName;
	pObj->m_strPathName = m_WellName+"."+m_Name;
	pObj->m_PlotSdep = m_Sdep;
	pObj->m_PlotEdep = m_Edep;
	if (m_WellName == "Template") m_WellName = "########";
}

BOOL CImageBase::OnInitDialog() 
{
	CString path;
	CGeoFIDIO::GetFullWellName(pObj->m_strPathName,m_WellName.GetBuffer(512));
	m_WellName.ReleaseBuffer();
	CGeoFIDIO::GetFIDName(pObj->m_strPathName,m_Name.GetBuffer(512));
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
	if (pObj->pDoc->GetDocumentType() == TEMPLATE_DOC)	
		HideDepth();
	else if (InitDepthSlider()) 
	{
		EnableDepth(TRUE);
		SetSliderDepth(m_Sdep,m_Edep);
	}
	else EnableDepth(FALSE);
	return TRUE; 
}

void CImageBase::OnBrowseWell() 
{
	char path[512];
	CGeoFIDIO::GetFileName(m_WellName,path,'I');
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
		CGeoFIDIO::GetFullWellName(path,m_WellName.GetBuffer(512));
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

void CImageBase::AddCurveList(CString path)
{
	CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_NAME);
	if (pBox == NULL) return;
	pBox->ResetContent();

	CString szDir;
	CGeoFIDIO::GetPath(m_WellName,szDir.GetBuffer(512));
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
			pBox->AddString(Name); 
	}
	m_dBaseFile.Close();
	pBox->SetWindowText(m_Name);
}

void CImageBase::OnCloseupName()
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

void CImageBase::OnChangeSdep() 
{
	CWnd* pWnd = GetFocus();
	if ((pWnd != NULL) && (pWnd == &m_SdepCtrl))
	{
		UpdateData(TRUE);
		m_SliderSdep.SetPos((int)m_Sdep);
	}
}

void CImageBase::OnChangeEdep() 
{
	CWnd* pWnd = GetFocus();
	if ((pWnd != NULL) && (pWnd == &m_EdepCtrl))
	{
		UpdateData(TRUE);
		m_SliderEdep.SetPos((int)m_Edep);
	}
}

void CImageBase::HideDepth()
{
	CWnd* pWnd = GetDlgItem(IDC_SDEP);
	pWnd->ShowWindow(FALSE);
	pWnd = GetDlgItem(IDC_EDEP);
	pWnd->ShowWindow(FALSE);
	/*pWnd = GetDlgItem(IDC_STATIC_DEPTH);
	pWnd->ShowWindow(FALSE);*/
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

void CImageBase::EnableDepth(BOOL bf)
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

BOOL CImageBase::InitDepthSlider()
{
	BOOL bf = FALSE;
	int dep1=0,dep2=9999;
	CGeoFIDIO* pFIDIO = new CGeoFIDIO(pObj->m_strPathName);
	if(pFIDIO->Open(pObj->pDoc->m_szSever)&&pFIDIO->GetDimenSion() == 3)
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

void CImageBase::SetSliderDepth(float sdep, float edep)
{
	m_SliderSdep.SetPos((int)sdep);
	m_SliderEdep.SetPos((int)edep);
}


void CImageBase::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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

void CImageBase::OnChangeWellname() 
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

void CImageBase::OnEditchangeName() 
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
