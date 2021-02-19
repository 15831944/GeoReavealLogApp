// CurveBasePage.cpp : implementation file
//

#include "stdafx.h"
#include "PreView.h"
#include "LogPlotDoc.h"
#include "CurveDraw.h"
#include "TrackDraw.h"
#include "CurveBasePage.h"
#include "ProjectBrowse.h"
#include "GeoFIDio.h"
#include "GeoAdo.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCurveBasePage property page

IMPLEMENT_DYNCREATE(CCurveBasePage, CPropertyPage)

CCurveBasePage::CCurveBasePage(CFIDDrawObj* pCurveObj) : CPropertyPage(CCurveBasePage::IDD)
{
	//{{AFX_DATA_INIT(CCurveBasePage)
	m_AliasName = _T("");
	m_Edep = 0.0f;
	m_Name = _T("");
	m_Name1 = _T("");
	m_Sdep = 0.0f;
	m_Unit = _T("");
	m_WellName = _T("");
	m_ScripFileName = _T("");
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
	DDX_Text(pDX, IDC_SCRIPNAME, m_ScripFileName);
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
	ON_BN_CLICKED(IDC_BROWSE_WELL2, OnBrowseWell2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCurveBasePage message handlers

BOOL CCurveBasePage::OnInitDialog() 
{
	CGeoFIDIO::GetFullWellName(pObj->m_strPathName, m_WellName.GetBuffer(512));
	m_WellName.ReleaseBuffer();
	if(pObj->pDoc->GetDocumentType()==TEMPLATE_DOC)
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

	CGeoFIDIO::GetFIDName(pObj->m_strPathName, m_Name.GetBuffer(512));
	m_Name.ReleaseBuffer();
	GetDlgItem(IDC_NAME)->SetWindowText(m_Name);
	m_Name1 = m_Name;
	m_AliasName = pObj->m_AliasName;
	m_ScripFileName=pObj->m_ScripFileName;
	m_Unit = pObj->m_UnitText;
	m_Sdep = (float)pObj->m_PlotSdep;
	m_Edep = (float)pObj->m_PlotEdep;

	CPropertyPage::OnInitDialog();

	if(pObj->pDoc->GetDocumentType() == TEMPLATE_DOC)	
		HideDepth();
	else if(InitDepthSlider()) 
	{
		SetSliderDepth(m_Sdep,m_Edep);
		if((pObj->GetParentObj()==NULL)||(pObj->GetParentType()!=0)) 
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
		return FALSE;
	}
	else if((pObj->GetParentObj() != NULL) && (pObj->GetParentType() == 0))
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
	pObj->m_ScripFileName=m_ScripFileName;
	if (m_WellName == "Template") m_WellName = "########";
	pObj->m_strPathName = m_WellName + "." + m_Name; 
	CCurveBasePage::EndDialog(IDOK);
}

void CCurveBasePage::AddCurveList(CString path)
{
	CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_NAME);
	if (pBox == NULL) return;
	pBox->ResetContent();
	
	CString str=m_WellName;
//	D:\\极睿解释平台2010\\WorkSpace\\[综合解释-MO206]\\#MO206ZH(3070-3240)\\MO206ZH(3070-3240)
	
	int x=str.ReverseFind('\\');
	if(x<1) return;
	str=str.Left(x);
	//取项目名
	x=str.ReverseFind('\\');
	if(x<1) return;
	CString ProjectName=str.Mid(x+1);
	str=str.Left(x);
	//取井名
	x=str.ReverseFind('\\');
	if(x<1) return;
	CString WellName=str.Mid(x+1);

	CGeoAdo	ado;
	if(!ado.InitData(pObj->pDoc->m_szSever)) 
		return;
	DWORD WellID,ProjectID;
	//寻找井ID号
	_RecordsetPtr pRS = ado.GetRS("SELECT * FROM Storage WHERE StorageName='" + WellName + "'");
	if(pRS->adoEOF)
	{
		ado.Disconnect();
		return;
	}
	else
		WellID=atol((LPCTSTR)(_bstr_t)pRS->GetCollect("StorageID"));
	if(WellID<1)
	{
		ado.Disconnect();
		return;
	}
	
	//寻找项目ID号
	CString cID;
	cID.Format("%lu",WellID);
	pRS = ado.GetRS("SELECT * FROM Storage WHERE UpperID=" + cID);
	while(pRS->adoEOF == 0)//执行SELECT语句
	{
		str=(LPCTSTR)(_bstr_t)pRS->GetCollect("StorageName");
		if(str==ProjectName)
		{
			ProjectID=atol((LPCTSTR)(_bstr_t)pRS->GetCollect("StorageID"));
			break;
		}
		pRS->MoveNext();
	}
	if(ProjectID<1)
	{
		ado.Disconnect();
		return;
	}

	int dim=2;
	//寻找曲线ID号
	cID.Format("%lu",ProjectID);
	pRS = ado.GetRS("SELECT * FROM Curve WHERE UpperID=" + cID);
	while(pRS->adoEOF == 0)//执行SELECT语句
	{
		str=(LPCTSTR)(_bstr_t)pRS->GetCollect("Dimension");
		str.Trim();dim=atoi(str);
		if(dim<3)
		{
			str=(LPCTSTR)(_bstr_t)pRS->GetCollect("CurveName");
			str.Trim();
			pBox->AddString (str);
		}
		pRS->MoveNext();
	}
		
	ado.Disconnect();
	pBox->SetWindowText(m_Name);
}

void CCurveBasePage::OnBrowseWell() 
{
	char path[512];
	CGeoFIDIO::GetFileName(m_WellName,path,'I');
	strcat(path,".FID");

	CProjectBrowse dlg;
	dlg.m_szSever=pObj->pDoc->m_szSever;
	dlg.m_RootID=pObj->pDoc->m_ProjectID;
	dlg.m_WellName=m_WellName;
	if(dlg.DoModal() == IDOK) 
	{
		m_WellName=dlg.m_WellName;
		CEdit* pEdit = (CEdit*) GetDlgItem(IDC_WELLNAME);
		if(pEdit != NULL) 
			pEdit->SetWindowText(m_WellName);
		if(pObj->pDoc->GetDocumentType() == TEMPLATE_DOC)	
			return;
		if(InitDepthSlider()) 
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
	if((pWnd != NULL) && (pWnd == &m_SdepCtrl))
	{
		UpdateData(TRUE);
		m_SliderSdep.SetPos((int)m_Sdep);
	}
}

void CCurveBasePage::OnChangeEdep() 
{
	CWnd* pWnd = GetFocus();
	if((pWnd != NULL) && (pWnd == &m_EdepCtrl))
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

	CGeoFIDIO* pFIDIO = new CGeoFIDIO(m_WellName + "." + m_Name );
	if(pFIDIO->Open(pObj->pDoc->m_szSever)&&(pFIDIO->GetDimenSion() == 2))
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
	if(pWnd == &m_SliderSdep)
	{
		m_Sdep = (float) m_SliderSdep.GetPos();
		sprintf(s,"%.1f",m_Sdep);
		m_SdepCtrl.SetWindowText(s);
	}
	else if(pWnd == &m_SliderEdep)
	{
		m_Edep = (float) m_SliderEdep.GetPos();
		sprintf(s,"%.1f",m_Edep);
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
			if((pObj->GetParentObj()==NULL)||(pObj->GetParentType()!=0)) 
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

void CCurveBasePage::OnBrowseWell2() 
{
	CString newName;
	static char BASED_CODE szFilter[] = 
	"描述文件(*.List)|*.List|Tab表文件(*.ccd)|*.ccd|所有文件(*.*)|*.*||";
	CFileDialog  Browse(TRUE, "描述文件(*.List)", "Scrip.List", 
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	Browse.m_ofn.lpstrTitle = "打开文件";
	Browse.m_ofn.lpstrInitialDir=GetCurrentUserPath();

	if (Browse.DoModal()==IDOK)
	{
		m_ScripFileName =Browse.GetPathName();
		UpdateData(FALSE);
	}
}
