// CurveFillPage.cpp : implementation file
//

#include "stdafx.h"
#include "PreView.h"
#include "LogPlotDoc.h"
#include "TrackDraw.h"
#include "CurveDraw.h"
#include "CurveFillPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCurveFillPage dialog

IMPLEMENT_DYNCREATE(CCurveFillPage, CPropertyPage)

CCurveFillPage::CCurveFillPage(CCurveObj* pCurveObj)
	: CPropertyPage(CCurveFillPage::IDD)
{
	//{{AFX_DATA_INIT(CCurveFillPage)
	m_FillPattern = -1;
	m_FillText = _T("");
	m_SourceCurve = _T("");
	m_SelectCurve = _T("");
	m_FillMode = -1;
	m_SelectListName = _T("");
	m_LthCurveName = _T("");
	m_bHeadShowFill = FALSE;
	//}}AFX_DATA_INIT
	ASSERT ((pCurveObj != NULL) && (pCurveObj->IsKindOf(RUNTIME_CLASS(CCurveObj))));
	m_pCurveObj = pCurveObj;

	CDrawObj* pObj = m_pCurveObj->GetParentObj();
	m_pTrackObj = (CTrackObj*) pObj;
	m_FillPatternBox.SetStyleType(0);
}

CCurveFillPage::~CCurveFillPage()
{
	int k = m_FillList.GetSize();
	if (k>0)
	{
		for (int i=0; i<k; i++)
			delete m_FillList[i];
		m_FillList.RemoveAll();
	}
}

void CCurveFillPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCurveFillPage)
	DDX_Control(pDX, IDC_LEFT, m_FillLeftCtrl);
	DDX_Control(pDX, IDC_RIGHT, m_FillRightCtrl);
	DDX_Control(pDX, IDC_LR, m_FillLeftRightCtrl);
	DDX_Control(pDX, IDC_FILL_TEXT, m_FillTextCtrl);
	DDX_Control(pDX, IDC_UNSELECT_LIST, m_UnSelectList);
	DDX_Control(pDX, IDC_SELECT_LIST, m_SelectList);
	DDX_CBIndex(pDX, IDC_FILL_PATTERN, m_FillPattern);
	DDX_Text(pDX, IDC_FILL_TEXT, m_FillText);
	DDX_Text(pDX, IDC_SOURCE_CURVE, m_SourceCurve);
	DDX_Text(pDX, IDC_SELECT_CURVE, m_SelectCurve);
	DDX_Control(pDX, IDC_FILL_COLOR, m_FillColor);
	DDX_Radio(pDX, IDC_LEFT, m_FillMode);
	DDX_LBString(pDX, IDC_SELECT_LIST, m_SelectListName);
//	DDX_Text(pDX, IDC_LTH_CUTVE, m_LthCurveName);
	DDX_Check(pDX, IDC_HEAD_SHOW_FILL, m_bHeadShowFill);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCurveFillPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCurveFillPage)
	ON_BN_CLICKED(ID_ADD, OnAddCurve)
	ON_BN_CLICKED(ID_DEL, OnDeleteCurve)
	ON_CBN_SELCHANGE(IDC_FILL_COLOR, OnSelchangeFillColor)
	ON_CBN_SELCHANGE(IDC_FILL_PATTERN, OnSelchangeFillPattern)
	ON_LBN_SELCHANGE(IDC_SELECT_LIST, OnSelchangeSelectList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCurveFillPage message handlers

void CCurveFillPage::OnAddCurve() 
{
	
	if (m_UnSelectList.m_hWnd == NULL) return;
	int i = m_UnSelectList.GetCurSel();
	if (i != LB_ERR)
	{
		CString m_Name;
		m_UnSelectList.GetText(i,m_Name);
		m_SelectList.AddString(m_Name);
		CCurveFill* pObj = new CCurveFill;
		pObj->m_TargetCurveName = m_Name;
		pObj->m_SourceCurveName = m_SourceCurve;
		if (m_Name == "×ó±ß½ç") pObj->m_FillMode = m_FillMode = 0;
		else if (m_Name == "ÓÒ±ß½ç") pObj->m_FillMode = 1;
		m_FillList.Add(pObj);
		m_UnSelectList.DeleteString(i);
		m_SelectList.SelectString(-1,m_Name);
		if (m_SelectIndex == -1)
		{
			delButton.EnableWindow(TRUE);
			m_FillColorBox.EnableWindow(TRUE);
			m_FillPatternBox.EnableWindow(TRUE);
			m_FillTextCtrl.EnableWindow(TRUE);
		}
		else
		{
			UpdateData(TRUE);
			TransferData(TRUE);
		}
		m_SelectIndex = m_SelectList.GetCount()-1;
		TransferData(FALSE);
		UpdateData(FALSE);
		if ((m_SelectCurve == "×ó±ß½ç") || (m_SelectCurve == "ÓÒ±ß½ç"))
		{
			m_FillLeftCtrl.EnableWindow(FALSE);
			m_FillRightCtrl.EnableWindow(FALSE);
			m_FillLeftRightCtrl.EnableWindow(FALSE);
		}
		else
		{
			m_FillLeftCtrl.EnableWindow(TRUE);
			m_FillRightCtrl.EnableWindow(TRUE);
			m_FillLeftRightCtrl.EnableWindow(TRUE);
		}
	}
	int k = m_UnSelectList.GetCount();
	if (k == 0) addButton.EnableWindow(FALSE);
	else if (k > i) m_UnSelectList.SetCurSel(i);
	else m_UnSelectList.SetCurSel(k-1);
}

void CCurveFillPage::OnDeleteCurve() 
{
	if (m_SelectList.m_hWnd == NULL) return;
	int i = m_SelectList.GetCurSel();
	if (i != LB_ERR)
	{
		CString m_Name;
		m_SelectList.GetText(i,m_Name);
		m_UnSelectList.AddString(m_Name);
		m_SelectList.DeleteString(i);
		m_UnSelectList.SelectString(-1,m_Name);
		delete m_FillList[i];
		m_FillList.RemoveAt(i);
		addButton.EnableWindow(TRUE);
	}
	int k = m_SelectList.GetCount();
	if (k == 0) 
	{
		delButton.EnableWindow(FALSE);
		m_FillColorBox.EnableWindow(FALSE);
		m_FillPatternBox.EnableWindow(FALSE);
		m_FillTextCtrl.EnableWindow(FALSE);
		m_FillLeftCtrl.EnableWindow(FALSE);
		m_FillRightCtrl.EnableWindow(FALSE);
		m_FillLeftRightCtrl.EnableWindow(FALSE);
		m_SelectIndex = -1;
	}
	else
	{	
		if (k <= i) i = k-1;
		m_SelectList.SetCurSel(i);
		m_SelectIndex = i;
		TransferData(FALSE);
		UpdateData(FALSE);
		if ((m_SelectCurve == "×ó±ß½ç") || (m_SelectCurve == "×ó±ß½ç"))
		{
			m_FillLeftCtrl.EnableWindow(FALSE);
			m_FillRightCtrl.EnableWindow(FALSE);
			m_FillLeftRightCtrl.EnableWindow(FALSE);
		}
		else
		{
			m_FillLeftCtrl.EnableWindow(TRUE);
			m_FillRightCtrl.EnableWindow(TRUE);
			m_FillLeftRightCtrl.EnableWindow(TRUE);
		}
	}
}


void CCurveFillPage::OnOK() 
{
	UpdateData(TRUE);
	TransferData(TRUE);
	m_pCurveObj->RemoveAllFillCurve();
	for (int i=0; i<m_FillList.GetSize(); i++)
		m_pCurveObj->m_FillList.Add(m_FillList[i]);
	m_FillList.RemoveAll();
	m_pCurveObj->FindAllFillCurve();
	m_pCurveObj->m_bHeadShowFill = 	m_bHeadShowFill;
	CPropertyPage::EndDialog(IDOK);
}

void CCurveFillPage::OnSelchangeFillColor() 
{
}

void CCurveFillPage::OnSelchangeFillPattern() 
{
}

void CCurveFillPage::TransferData(BOOL bOutput)
{
	if (m_SelectIndex < 0) return;
	ASSERT(m_SelectIndex < m_FillList.GetSize());
	if (bOutput)
	{
		m_FillList[m_SelectIndex]->m_Color = m_FillColor.GetColor();
		m_FillList[m_SelectIndex]->m_LthNo = m_FillPattern;
		m_FillList[m_SelectIndex]->m_Text = m_FillText;
		m_FillList[m_SelectIndex]->m_TargetCurveName = m_SelectCurve;
		m_FillList[m_SelectIndex]->m_FillMode = m_FillMode;
		m_FillList[m_SelectIndex]->m_LthCurveName = m_LthCurveName;
	}
	else
	{
		m_FillColor.SetColor(m_FillList[m_SelectIndex]->m_Color);
		m_FillPattern = m_FillList[m_SelectIndex]->m_LthNo;
		m_FillText = m_FillList[m_SelectIndex]->m_Text;
		m_SelectCurve = m_FillList[m_SelectIndex]->m_TargetCurveName;
		m_FillMode = m_FillList[m_SelectIndex]->m_FillMode;
		m_LthCurveName = m_FillList[m_SelectIndex]->m_LthCurveName;
	}
}

void CCurveFillPage::OnSelchangeSelectList() 
{
	ASSERT(m_SelectList.m_hWnd != NULL);
	if (m_SelectList.m_hWnd == NULL) return;
	int i = m_SelectList.GetCurSel();
	if (i != LB_ERR)
	{
		UpdateData(TRUE);
		TransferData(TRUE);
		m_SelectIndex = i;
		TransferData(FALSE);
		UpdateData(FALSE);
		if ((m_SelectCurve == "×ó±ß½ç") || (m_SelectCurve == "ÓÒ±ß½ç"))
		{
			m_FillLeftCtrl.EnableWindow(FALSE);
			m_FillRightCtrl.EnableWindow(FALSE);
			m_FillLeftRightCtrl.EnableWindow(FALSE);
		}
		else
		{
			m_FillLeftCtrl.EnableWindow(TRUE);
			m_FillRightCtrl.EnableWindow(TRUE);
			m_FillLeftRightCtrl.EnableWindow(TRUE);
		}
	}
}


BOOL CCurveFillPage::OnInitDialog() 
{
	VERIFY(delButton.AutoLoad(ID_DEL, this));
	VERIFY(addButton.AutoLoad(ID_ADD, this));
	VERIFY(m_FillPatternBox.SubclassDlgItem(IDC_FILL_PATTERN, this));
	m_FillPatternBox.AddItem();
	for (int i=0; i<m_pCurveObj->m_FillList.GetSize(); i++)
	{
		CCurveFill* pCurveFill = new CCurveFill(*(m_pCurveObj->m_FillList[i]));
		m_FillList.Add(pCurveFill);
	}    
	m_SourceCurve = m_pCurveObj->m_AliasName;
	m_bHeadShowFill = m_pCurveObj->m_bHeadShowFill;
	
	if (m_FillList.GetSize()>0)
	{
		m_SelectIndex = 0;
		TransferData(FALSE);
	}
	else m_SelectIndex = -1;
	CPropertyPage::OnInitDialog();

	for (i=0; i<m_FillList.GetSize(); i++) 
		m_SelectList.AddString(m_FillList[i]->m_TargetCurveName); 
	POSITION pos = m_pTrackObj->m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_pTrackObj->m_ObjList.GetNext(pos);
		if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CCurveObj))) && (pObj->GetParentType() != 1))
			if (!m_pCurveObj->IsFillCurve(((CCurveObj*)pObj)->m_AliasName)) 
			{
				m_UnSelectList.AddString(((CCurveObj*)pObj)->m_AliasName);
			}
	}
	if (!m_pCurveObj->IsFillCurve("×ó±ß½ç")) m_UnSelectList.AddString("×ó±ß½ç"); 
	if (!m_pCurveObj->IsFillCurve("ÓÒ±ß½ç")) m_UnSelectList.AddString("ÓÒ±ß½ç"); 
	if (m_SelectIndex >= 0) 
		m_SelectList.SetCurSel(m_SelectIndex);
	else
	{
		m_FillLeftCtrl.EnableWindow(FALSE);
		m_FillRightCtrl.EnableWindow(FALSE);
		m_FillLeftRightCtrl.EnableWindow(FALSE);
		delButton.EnableWindow(FALSE);
		m_FillColorBox.EnableWindow(FALSE);
		m_FillPatternBox.EnableWindow(FALSE);
		m_FillTextCtrl.EnableWindow(FALSE);
	}	
	if (m_UnSelectList.GetCount() >= 0)	
		m_UnSelectList.SetCurSel(0);
	else 
		addButton.EnableWindow(FALSE);

	if ((m_SelectCurve == "×ó±ß½ç") || (m_SelectCurve == "ÓÒ±ß½ç"))
	{
		m_FillLeftCtrl.EnableWindow(FALSE);
		m_FillRightCtrl.EnableWindow(FALSE);
		m_FillLeftRightCtrl.EnableWindow(FALSE);
	}
	return TRUE;  
}
