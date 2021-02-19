// Curve3DPage.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "ImageManDoc.h"
#include "style.h"
#include "styledialog.h"
#include "Curve3DPage.h"
#include "Curve3D.h"
#include "Database.h"
#include "FIDIO.h"
#include "Database.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCurve3DPage property page

IMPLEMENT_DYNCREATE(CCurve3DPage, CPropertyPage)

CCurve3DPage::CCurve3DPage(CCurve3D* pCurve3D) : CPropertyPage(CCurve3DPage::IDD)
{
	//{{AFX_DATA_INIT(CCurve3DPage)	
	m_ColorGrade = 0;
	m_FilterType = 4;
	m_LeftVal = 0;
	m_RightVal = 128;
	m_bColor = FALSE;
	//}}AFX_DATA_INIT
	ASSERT(pCurve3D != NULL);
	pObj = pCurve3D;
	m_CrMode = 0;
}

CCurve3DPage::~CCurve3DPage()  
{
}

void CCurve3DPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Radio(pDX,IDC_RADIO1,m_CrMode);
	//{{AFX_DATA_MAP(CCurve3DPage)	
	DDX_Radio(pDX, IDC_COLOR_GRADE, m_ColorGrade);
	DDX_Radio(pDX, IDC_IMAGE, m_FilterType);
	DDX_Text(pDX, IDC_LEFT, m_LeftVal);
	DDX_Text(pDX, IDC_RIGHT, m_RightVal);
	DDX_Check(pDX, IDC_CHECK1, m_bColor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCurve3DPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCurve3DPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCurve3DPage message handlers

BOOL CCurve3DPage::OnInitDialog() 
{
	m_FilterType = pObj->m_FilterType;
	m_LeftVal = pObj->m_LeftValue;
	m_RightVal = pObj->m_RightValue;
	m_CrMode = pObj->m_CrMode;
	m_ColorGrade = pObj->m_ColorGrade;
	m_bColor=pObj->m_bColor ;
	CPropertyPage::OnInitDialog();
	return TRUE;
}

void CCurve3DPage::OnOK() 
{
	CString m_Name;
	CPropertyPage::OnOK();
	pObj->m_FilterType = m_FilterType;
	pObj->m_LeftValue = m_LeftVal;
	pObj->m_RightValue = m_RightVal;
	pObj->m_ColorGrade = m_ColorGrade;
	pObj->m_CrMode = m_CrMode;
	pObj->m_bColor=m_bColor;	
}
/////////////////////////////////////////////////////////////////////////////
// CCurve3DBase property page

IMPLEMENT_DYNCREATE(CCurve3DBase, CPropertyPage)

CCurve3DBase::CCurve3DBase(CCurve3D* pCurve3D) : CPropertyPage(CCurve3DBase::IDD)
{
	//{{AFX_DATA_INIT(CCurve3DBase)
	m_AliasName = _T("");
	m_Unit = _T("");
	m_WellName = _T("");
	m_Space = 5;
	m_Daz = FALSE;
	m_Name = _T("");
	m_PoleArray = FALSE;
	//}}AFX_DATA_INIT
	ASSERT(pCurve3D != NULL);
	pObj = pCurve3D;
	m_LookAngle=0.f;
}

CCurve3DBase::~CCurve3DBase()
{
}

void CCurve3DBase::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCurve3DBase)
	DDX_Control(pDX, IDC_CURVE_LIST, m_CurveList);
	DDX_Text(pDX, IDC_ALIAS_NAME, m_AliasName);
	DDX_Text(pDX, IDC_UNIT, m_Unit);
	DDX_Text(pDX, IDC_WELLNAME, m_WellName);
	DDX_Text(pDX, IDC_SPACE, m_Space);
	DDV_MinMaxUInt(pDX, m_Space, 0, 100);
	DDX_Check(pDX, IDC_DAZ, m_Daz);
	DDX_CBString(pDX, IDC_NAME, m_Name);	
	DDX_Check(pDX, IDC_POLE_ARRAY, m_PoleArray);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT1, m_LookAngle);
	DDX_Text(pDX, IDC_HOR_SCALE, m_HorScale);
	DDV_MinMaxInt(pDX, m_HorScale, 5, 1000);
	DDX_Text(pDX, IDC_STARTPOS, m_StartPos);
	DDV_MinMaxInt(pDX, m_StartPos, 0, 360);
	DDX_Text(pDX, IDC_BIT, m_Bit);
	DDV_MinMaxInt(pDX, m_Bit, 50, 500);
}


BEGIN_MESSAGE_MAP(CCurve3DBase, CPropertyPage)
	//{{AFX_MSG_MAP(CCurve3DBase)
	ON_BN_CLICKED(IDC_BROWSE_WELL, OnBrowseWell)
	ON_EN_CHANGE(IDC_WELLNAME, OnChangeWellname)
	ON_CBN_CLOSEUP(IDC_NAME, OnCloseupName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCurve3DBase message handlers

void CCurve3DBase::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertyPage::OnOK();
	pObj->m_UnitText = m_Unit;
	pObj->m_AliasName = m_AliasName;
	if (m_WellName == "Template") m_WellName = "########";
	pObj->m_strPathName = m_WellName+"."+m_Name;
	pObj->m_Space=m_Space;
	pObj->m_Daz=m_Daz;
	pObj->m_PoleArray=m_PoleArray;
	pObj->m_CurveArray.RemoveAll();
	pObj->m_LookAngle=m_LookAngle;

	pObj->m_Bit =m_Bit;
	pObj->m_HorScale =m_HorScale;
	pObj->m_StartPos 	=m_StartPos;
	int num = m_CurveList.GetCount();
	CString s;
	for (int i=0; i<num; i++)
	{
		if (m_CurveList.GetCheck(i)) 
		{
			m_CurveList.GetText(i,s);
			pObj->m_CurveArray.Add(s);
		}
	}
}

BOOL CCurve3DBase::OnInitDialog() 
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
	m_Space=pObj->m_Space;
	m_Daz=pObj->m_Daz;
	m_PoleArray=pObj->m_PoleArray;
	m_LookAngle=pObj->m_LookAngle;

	m_Bit=pObj->m_Bit ;
	m_HorScale=pObj->m_HorScale ;
	m_StartPos=pObj->m_StartPos ;
	this->UpdateData (FALSE);
	return TRUE; 
}

void CCurve3DBase::OnBrowseWell() 
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
		AddCurveList(m_WellName);
	}
}

void CCurve3DBase::OnChangeWellname() 
{
	CString str;
	CEdit* pEdit = (CEdit*) GetDlgItem(IDC_WELLNAME);
	if ((pEdit == NULL) || (pEdit != GetFocus())) return;
	pEdit->GetWindowText(str);
	if (m_WellName !=  str)
		m_WellName = str;
}

void CCurve3DBase::AddCurveList(CString path)
{
	CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_NAME);
	if (pBox == NULL) return;
	pBox->ResetContent();
	m_CurveList.ResetContent ();

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
			m_CurveList.AddString(Name);
			pBox->AddString (Name);
		}
	}
	m_dBaseFile.Close();
	int i,k;
	for (i=0; i<pObj->m_CurveArray.GetSize(); i++)
	{
		k = m_CurveList.FindString(-1,pObj->m_CurveArray[i]);
		if (k != LB_ERR) m_CurveList.SetCheck(k,1);
	}
	pBox->SetWindowText(m_Name);
}

void CCurve3DBase::OnCloseupName()
{
	CString str;
	CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_NAME);
	int nIndex = pBox->GetCurSel();
	if (nIndex == CB_ERR)
		pBox->GetWindowText(str);
	else
		pBox->GetLBText(nIndex, str);
	str.MakeUpper();
	m_Name = str;
}
