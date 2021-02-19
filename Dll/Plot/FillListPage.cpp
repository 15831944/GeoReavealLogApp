// FillListPage.cpp : implementation file
//

#include "stdafx.h"
#include "style.h"
#include "FillListPage.h"
#include "helpid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CFillListPage dialog
IMPLEMENT_DYNCREATE(CFillListPage, CPropertyPage)

const DWORD CFillListPage::m_nHelpIDs[] = 
{
	IDC_FILL_ITEM, HID_FILLLIST_ITEM,
	IDC_PATTERN_COLOR, HID_FILLLIST_COLOR,
	IDC_PATTERN_STYLE, HID_FILLLIST_STYLE,
	IDC_PATTERN_SYSTEM, HID_FILLLIST_SYSTEM,
	IDC_PATTERN_LTH, HID_FILLLIST_LITH,
	IDC_PATTERN_PREVIEW, HID_FILLLIST_PREVIEW,
	0, 0
};

CFillListPage::CFillListPage()
	: CCSPropertyPage(CFillListPage::IDD)
{
	//{{AFX_DATA_INIT(CFillListPage)
	m_Style = -1;
	m_ItemNo = -1;
	m_PatternType = -1;
	//}}AFX_DATA_INIT
	m_FillNum = 0;
	m_HelpPath += "Plot.hlp";
}

void CFillListPage::DoDataExchange(CDataExchange* pDX)
{
	CCSPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFillListPage)
	DDX_Control(pDX, IDC_PATTERN_COLOR, m_Color);
	DDX_CBIndex(pDX, IDC_PATTERN_STYLE, m_Style);
	DDX_CBIndex(pDX, IDC_FILL_ITEM, m_ItemNo);
	DDX_Radio(pDX, IDC_PATTERN_SYSTEM, m_PatternType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFillListPage, CCSPropertyPage)
	//{{AFX_MSG_MAP(CFillListPage)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_PATTERN_STYLE, OnSelchangeStyle)
	ON_CBN_SELCHANGE(IDC_FILL_ITEM, OnSelchangeFillItem)
	ON_BN_CLICKED(IDC_PATTERN_LTH, OnPatternLth)
	ON_BN_CLICKED(IDC_PATTERN_SYSTEM, OnPatternSystem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFillListPage message handlers

BOOL CFillListPage::OnInitDialog()
{
	if (m_FillNum > 0)
	{
		CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_FILL_ITEM);
		SetPatternIndirect(&m_FillList[0]);
		for (int i=0; i<m_FillNum; i++)
			pBox->AddString(m_DescList[i]);
		m_ItemNo = 0;
	}                            
	if (m_bWindowsPattern) m_PatternType = 0;
	else m_PatternType = 1;
	m_StyleBox.SetStyleType(m_bWindowsPattern);
	VERIFY(m_StyleBox.SubclassDlgItem(IDC_PATTERN_STYLE, this));
	m_StyleBox.AddItem();
	CCSPropertyPage::OnInitDialog();
	return TRUE;
}                               

void CFillListPage::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	Preview();
}

void CFillListPage::Preview()
{
	CWnd* pWnd = GetDlgItem(IDC_PATTERN_PREVIEW);
	if (pWnd == NULL) return;
	pWnd->Invalidate();
	pWnd->UpdateWindow();
	CRect rect ;
	pWnd->GetClientRect(&rect); 
	CDC* pDC = pWnd->GetDC();
	pDC->IntersectClipRect(rect);
	CRect rect1 = rect;
	
	rect1.InflateRect(-10,-10);
	if (m_Style > 0)
	{
	    if (m_bWindowsPattern)
    	{                                     
    		LOGBRUSH m_LogBrush;
    		GetPatternIndirect(&m_LogBrush);
    		CBrush br1;
    		if (m_LogBrush.lbHatch == BS_HATCHED) 
    			br1.CreateHatchBrush(m_LogBrush.lbHatch,m_LogBrush.lbColor);
    		else br1.CreateBrushIndirect(&m_LogBrush);
    		pDC->FillRect(rect1,&br1);
	    }
    	else
  			LithFillBox(rect1,m_Style,m_Color.GetColor(),rect1.TopLeft(),pDC);
	}
	pWnd->ReleaseDC(pDC);   
}

void CFillListPage::OnSelchangeStyle()
{
    CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_PATTERN_STYLE);
    if (pBox == NULL) return;
	m_Style = pBox->GetCurSel();
    Preview();
}

void CFillListPage::OnOK()
{
	CCSPropertyPage::OnOK();
	if (m_FillNum > 0)
	{
		GetPatternIndirect(&m_FillList[m_ItemNo]);
		for (int i=0; i<m_FillNum; i++)
		{
			*m_PtrList[i] = m_FillList[i];
		}
	}	
}

void CFillListPage::SetPatternIndirect(LOGBRUSH *pLogBrush)
{                  
	if (pLogBrush->lbStyle ==BS_NULL) 
	{ 
		m_Style = 0;
	}
//	else if (pLogBrush->lbStyle == BS_SOLID) m_Style = 1;
	else if (pLogBrush->lbStyle == BS_HATCHED)	m_Style = pLogBrush->lbHatch + 1;
	else if (pLogBrush->lbStyle == BS_OWNER) m_Style = pLogBrush->lbHatch;
	m_bWindowsPattern = (pLogBrush->lbStyle != BS_OWNER);
	m_Color.SetColor(pLogBrush->lbColor);
	if (m_bWindowsPattern) m_PatternType = 0;
	else m_PatternType = 1;
}

void CFillListPage::GetPatternIndirect(LOGBRUSH *pLogBrush)
{
	if (m_PatternType == 0) m_bWindowsPattern =  TRUE;
	else m_bWindowsPattern = FALSE;
	if (m_Style == 0) pLogBrush->lbStyle = BS_NULL;
//	else if (m_Style == 1) pLogBrush->lbStyle = BS_SOLID;
	else pLogBrush->lbStyle = BS_HATCHED;
	if (!m_bWindowsPattern) pLogBrush->lbStyle = BS_OWNER;
	pLogBrush->lbColor = m_Color.GetColor();
	if (pLogBrush->lbStyle == BS_HATCHED) pLogBrush->lbHatch = m_Style - 1;
	else pLogBrush->lbHatch = m_Style;
}

void CFillListPage::AddFill(LOGBRUSH* pLogBrush,CString str)
{
	if (m_FillNum < 20)
	{
		m_FillList[m_FillNum] = *pLogBrush;
		m_DescList.Add(str);
		m_PtrList[m_FillNum] = pLogBrush;
		m_FillNum ++;
	}
}

void CFillListPage::OnSelchangeFillItem() 
{
	CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_FILL_ITEM);
	if (pBox == NULL) return;
	int nIndex = pBox->GetCurSel();
	if (nIndex != CB_ERR)
	{
		if (m_ItemNo >= 0)
		{
			int i = m_ItemNo;
			UpdateData(TRUE);
			GetPatternIndirect(&m_FillList[i]);
		}
		SetPatternIndirect(&m_FillList[m_ItemNo]);
		m_StyleBox.ResetContent();
		m_StyleBox.SetStyleType(m_bWindowsPattern);
		m_StyleBox.AddItem();
		UpdateData(FALSE);
		Preview();
	}
}

void CFillListPage::OnPatternLth() 
{
	m_PatternType = 0;
	m_bWindowsPattern = FALSE;
	m_StyleBox.ResetContent();
	m_StyleBox.SetStyleType(m_bWindowsPattern);
	m_StyleBox.AddItem();
	if (m_Style > 1) m_Style = 1;	
	m_StyleBox.SetCurSel(m_Style);
	Preview();
}

void CFillListPage::OnPatternSystem() 
{
	m_bWindowsPattern = TRUE;
	m_PatternType = 0;
	m_StyleBox.ResetContent();
	m_StyleBox.SetStyleType(m_bWindowsPattern);
	m_StyleBox.AddItem();
	if (m_Style > 1) m_Style = 1;	
	m_StyleBox.SetCurSel(m_Style);
	Preview();
}
