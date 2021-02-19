// CurveBasePage.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "resource.h"
#include "ImageManDoc.h"
#include "CurveDraw.h"
#include "TrackDraw.h"
#include "CurveBasePage.h"
#include "FIDIO.h"
#include "Database.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCurveBasePage property page

IMPLEMENT_DYNCREATE(CCurveBasePage, CPropertyPage)

CCurveBasePage::CCurveBasePage(CCurveObj* pCurveObj) : CPropertyPage(CCurveBasePage::IDD)
{
	//{{AFX_DATA_INIT(CCurveBasePage)
	m_AliasName = _T("");
	m_Edep = 0.0f;
	m_Name = _T("");
	m_Name1 = _T("");
	m_Sdep = 0.0f;
	m_Unit = _T("");
	m_WellName = _T("");
	//}}AFX_DATA_INIT
	ASSERT(pCurveObj != NULL);
	pObj = pCurveObj;
}

CCurveBasePage::~CCurveBasePage()
{
}

void CCurveBasePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCurveBasePage)
	DDX_Control(pDX, IDC_SLIDER_SDEP, m_SliderSdep);
	DDX_Control(pDX, IDC_SLIDER_EDEP, m_SliderEdep);
	DDX_Control(pDX, IDC_SDEP, m_SdepCtrl);
	DDX_Control(pDX, IDC_EDEP, m_EdepCtrl);
	DDX_Text(pDX, IDC_ALIAS_NAME, m_AliasName);
	DDX_Text(pDX, IDC_EDEP, m_Edep);
	DDX_CBString(pDX, IDC_NAME, m_Name);
	DDX_Text(pDX, IDC_NAME1, m_Name1);
	DDX_Text(pDX, IDC_SDEP, m_Sdep);
	DDX_Text(pDX, IDC_UNIT, m_Unit);
	DDX_Text(pDX, IDC_WELLNAME, m_WellName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCurveBasePage, CPropertyPage)
	//{{AFX_MSG_MAP(CCurveBasePage)
	ON_BN_CLICKED(IDC_BROWSE_WELL, OnBrowseWell)
	ON_EN_CHANGE(IDC_EDEP, OnChangeEdep)
	ON_EN_CHANGE(IDC_SDEP, OnChangeSdep)
	ON_WM_HSCROLL()
	ON_EN_KILLFOCUS(IDC_WELLNAME, OnKillfocusWellname)
	ON_EN_CHANGE(IDC_WELLNAME, OnChangeWellname)
	ON_EN_SETFOCUS(IDC_WELLNAME, OnSetfocusWellname)
	ON_CBN_EDITCHANGE(IDC_NAME, OnEditchangeName)
	ON_CBN_CLOSEUP(IDC_NAME, OnCloseupName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCurveBasePage message handlers

BOOL CCurveBasePage::OnInitDialog() 
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
		pWnd = GetDlgItem(IDC_NAME);
		if (pWnd != NULL) pWnd->ShowWindow(FALSE);
		pWnd = GetDlgItem(IDC_NAME1);
		if (pWnd != NULL) pWnd->ShowWindow(TRUE);
	}
	else AddCurveList(pObj->m_strPathName);

	CFIDIO::GetFIDName(pObj->m_strPathName, m_Name.GetBuffer(512));
	m_Name.ReleaseBuffer();
	m_Name1 = m_Name;
	m_AliasName = pObj->m_AliasName;
	m_Unit = pObj->m_UnitText;
	m_Sdep = (float)pObj->m_PlotSdep;
	m_Edep = (float)pObj->m_PlotEdep;

	CPropertyPage::OnInitDialog();

	if (pObj->pDoc->GetDocumentType() == TEMPLATE_DOC)	HideDepth();
	else if (InitDepthSlider()) 
	{
		SetSliderDepth(m_Sdep,m_Edep);
		if ((pObj->GetParentObj() == NULL) || (pObj->GetParentType() != 0)) 
			EnableDepth(TRUE);
		else EnableDepth(FALSE);
	}
	else EnableDepth(FALSE);
	return TRUE; 
}

BOOL CCurveBasePage::OnKillActive() 
{
	CString m_Name2= m_Name;
	if (!UpdateData(TRUE)) return FALSE;
	m_Name = m_Name2;
	CString szHT, szHT1;
	szHT.LoadString(IDS_LEFTBOUNDARY);
	szHT1.LoadString(IDS_RIGHTBOUNDARY);
	if (m_WellName == "Template") m_Name = m_Name1;
	if (m_AliasName == "") m_AliasName = m_Name;
	if (m_AliasName == "")
	{
		AfxMessageBox(IDS_NO_CURVE,MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	else if ((m_AliasName == "左边界") || (m_AliasName == "右边界")
		|| (m_AliasName == szHT) || (m_AliasName == szHT1))
	{
		CString msg;
		AfxFormatString1(msg,IDS_CURVE_ALIASNAME_ER,m_AliasName);
		AfxMessageBox(msg,MB_OK | MB_ICONSTOP);
		//AfxMessageBox("曲线别名不能用保留名"+m_AliasName+",请重新输入!",MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	else if ((pObj->GetParentObj() != NULL) && (pObj->GetParentType() == 0))
	{	
		CCurveObj* pCurveObj = ((CTrackObj*)pObj->GetParentObj())->FindAliasCurve(m_AliasName);
		if ((pCurveObj != NULL) && (pCurveObj != pObj))
		{
			CString msg;
		    AfxFormatString1(msg,IDS_CURVE_ALIASNAME_ER,m_AliasName);
		    AfxMessageBox(msg,MB_OK | MB_ICONSTOP);
			return FALSE;
		}
	}	
	if (m_Name == "")
	{
		AfxMessageBox(IDS_NO_CURVE,MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	
    m_Name.MakeUpper();
	if (m_AliasName == "") m_AliasName = m_Name;	
	return TRUE;
}

void CCurveBasePage::OnOK() 
{
	CPropertyPage::OnOK();
	CDrawObj* pParentObj = pObj->GetParentObj();
	if ((pParentObj != NULL) && (pObj->GetParentType() == 0) && pParentObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) 
	{
		if (m_AliasName != pObj->m_AliasName)
		{
			((CTrackObj*)pParentObj)->RemoveFillCurve(pObj->m_AliasName);	
		}
	}
	pObj->m_PlotSdep = m_Sdep;
	pObj->m_PlotEdep = m_Edep;
	pObj->m_UnitText = m_Unit;
	pObj->m_AliasName = m_AliasName;
	if (m_WellName == "Template") m_WellName = "########";
	pObj->m_strPathName = m_WellName + "." + m_Name; 
	CCurveBasePage::EndDialog(IDOK);
}

void CCurveBasePage::AddCurveList(CString path)
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
		if(Type=="常规")
			pBox->AddString (Name);
	}
	m_dBaseFile.Close();
	pBox->SetWindowText(m_Name);
}

void CCurveBasePage::OnBrowseWell() 
{
	char path[512];
	CFIDIO::GetFileName(m_WellName,path,'I');
	strcat(path,".FID");

	CFileDialog dlgFile(TRUE);
	dlgFile.m_ofn.Flags |= OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	CString strFilter = "Flags File(*.FID)";
	strFilter += (TCHAR)'\0';   
	strFilter += _T("*.FID");
	strFilter += (TCHAR)'\0';   
	dlgFile.m_ofn.nMaxCustFilter++;
	strFilter += "Flags File(*.FID)";
	strFilter += (TCHAR)'\0';   
	strFilter += _T("*.FID");
	strFilter += (TCHAR)'\0';   
	dlgFile.m_ofn.nMaxCustFilter++;
	strFilter += _T("All File(*.*)");
	strFilter += (TCHAR)'\0';   
	strFilter += _T("*.*");
	strFilter += (TCHAR)'\0';   
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

void CCurveBasePage::OnChangeSdep() 
{
	CWnd* pWnd = GetFocus();
	if ((pWnd != NULL) && (pWnd == &m_SdepCtrl))
	{
		UpdateData(TRUE);
		m_SliderSdep.SetPos((int)m_Sdep);
	}
}

void CCurveBasePage::OnChangeEdep() 
{
	CWnd* pWnd = GetFocus();
	if ((pWnd != NULL) && (pWnd == &m_EdepCtrl))
	{
		UpdateData(TRUE);
		m_SliderEdep.SetPos((int)m_Edep);
		
	}
}

void CCurveBasePage::HideDepth()
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

void CCurveBasePage::EnableDepth(BOOL bf)
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

BOOL CCurveBasePage::InitDepthSlider()
{
	BOOL bf = FALSE;
	int dep1=0,dep2=9999;
	CFIDIO* pFIDIO = new CFIDIO(m_WellName + "." + m_Name );
	if (pFIDIO->Open() && (pFIDIO->GetDimenSion() == 2))
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

void CCurveBasePage::SetSliderDepth(float sdep, float edep)
{
	m_SliderSdep.SetPos((int)sdep);
	m_SliderEdep.SetPos((int)edep);
}


void CCurveBasePage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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


void CCurveBasePage::OnCloseupName()
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

void CCurveBasePage::OnKillfocusWellname() 
{
	CString str;
	if (m_WellName0 !=  m_WellName)	AddCurveList(m_WellName);
}

void CCurveBasePage::OnChangeWellname() 
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

void CCurveBasePage::OnSetfocusWellname() 
{
	m_WellName0 = m_WellName;	
}

void CCurveBasePage::OnEditchangeName() 
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

