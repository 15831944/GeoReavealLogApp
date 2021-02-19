// LineListPage.cpp : implementation file
//

#include "stdafx.h"
#include "style.h"
#include "LineListPage.h"
#include "Helpid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLineListPage property page

IMPLEMENT_DYNCREATE(CLineListPage, CPropertyPage)

const DWORD CLineListPage::m_nHelpIDs[] = 
{
	IDC_LINE_ITEM, HID_LINELIST_ITEM,
	IDC_LINE_COLOR, HID_LINELIST_COLOR,
	IDC_LINE_STYLE, HID_LINELIST_STYLE,
	IDC_LINE_WIDTH, HID_LINELIST_WIDTH,
	IDC_LINE_WIDTH_SPIN, HID_LINELIST_WIDTHSPIN,
	IDC_LINE_PREVIEW, HID_LINELIST_PREVIEW,
	0, 0
};

CLineListPage::CLineListPage() : CCSPropertyPage(CLineListPage::IDD)
{
	//{{AFX_DATA_INIT(CLineListPage)
	m_Width = 0;
	m_Style = -1;
	m_ItemNo = -1;
	//}}AFX_DATA_INIT
	m_LineNum = 0;
	m_HelpPath += "Plot.hlp";
}

CLineListPage::~CLineListPage()
{
	
}

void CLineListPage::DoDataExchange(CDataExchange* pDX)
{
	CCSPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLineListPage)
	DDX_Text(pDX, IDC_LINE_WIDTH, m_Width);
	DDX_Control(pDX, IDC_LINE_COLOR, m_Color);
	DDX_CBIndex(pDX, IDC_LINE_STYLE, m_Style);
	DDX_Control(pDX, IDC_LINE_WIDTH_SPIN, m_WidthSpinCtrl);
	DDX_CBIndex(pDX, IDC_LINE_ITEM, m_ItemNo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLineListPage, CCSPropertyPage)
	//{{AFX_MSG_MAP(CLineListPage)
	ON_EN_CHANGE(IDC_LINE_WIDTH, OnChangeWidth)
	ON_CBN_SELCHANGE(IDC_LINE_STYLE, OnSelchangeStyle)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_LINE_ITEM, OnSelchangeLineItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLineListPage message handlers


BOOL CLineListPage::OnInitDialog()
{
	VERIFY(m_StyleBox.SubclassDlgItem(IDC_LINE_STYLE, this));
	m_StyleBox.AddItem();
	if(m_LineNum>0)
	{
		CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_LINE_ITEM);
		SetLineIndirect(&m_LineList[0]);
		for (int i=0; i<m_LineNum; i++)
			pBox->AddString(m_DescList[i]);
		m_ItemNo = 0;
	}                            
	CCSPropertyPage::OnInitDialog();
	CenterWindow();
	if(m_WidthSpinCtrl.m_hWnd != NULL) 
		m_WidthSpinCtrl.SetRange(0,5);
	return TRUE;
}                               

void CLineListPage::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	Preview();
}

void CLineListPage::Preview()
{
	CWnd* pWnd = GetDlgItem(IDC_LINE_PREVIEW);
	if (pWnd == NULL) return;
	pWnd->Invalidate();
	pWnd->UpdateWindow();
	CRect rect ;
	pWnd->GetClientRect(&rect); 
	CDC* pDC = pWnd->GetDC();
	pDC->IntersectClipRect(rect);
	CRect rect1 = rect;
	if (m_Style >= 0) 
	{
    	CPen m_Pen;
		LOGPEN m_LogPen;
		GetLineIndirect(&m_LogPen);
		if (m_LogPen.lopnWidth.x > 0)
		{
//			if ((m_Color < 0) || (m_Color > COLOR_NUM)) m_LogPen.lopnColor = 0;
			m_Pen.CreatePenIndirect(&m_LogPen);
			CPen* pOldPen = pDC->SelectObject(&m_Pen); 
			pDC->MoveTo(rect1.left + 5, (rect1.top + rect1.bottom)/2); 	
			pDC->LineTo(rect1.right - 5, (rect1.top + rect1.bottom)/2); 	
			pDC->SelectObject(pOldPen);
		}
	}	
	pWnd->ReleaseDC(pDC);
}

void CLineListPage::OnSelchangeStyle()
{
    CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_LINE_STYLE);
    m_Style = pBox->GetCurSel();
    Preview();
}

void CLineListPage::OnChangeWidth()
{
	// TODO: Add your control notification handler code here
	m_Width = GetDlgItemInt(IDC_LINE_WIDTH);
	if (m_Width > 5) m_Width = 5;
	if (m_Width < 0) m_Width = 0;
	Preview();
}

void CLineListPage::OnOK()
{
	CCSPropertyPage::OnOK();
	if (m_LineNum > 0)
	{
		GetLineIndirect(&m_LineList[m_ItemNo]);
		for (int i=0; i<m_LineNum; i++)
		{
			*m_PtrList[i] = m_LineList[i];
		}
	}	
}

void CLineListPage::SetLineIndirect(LOGPEN *m_pLogPen)
{                  
	m_Width = m_pLogPen->lopnWidth.x;
	m_Color.SetColor(m_pLogPen->lopnColor);
	m_Style = m_pLogPen->lopnStyle;
}                                    

void CLineListPage::GetLineIndirect(LOGPEN *m_pLogPen)
{
	m_pLogPen->lopnWidth.x = m_Width;
	m_pLogPen->lopnWidth.y = m_Width;
	m_pLogPen->lopnColor = m_Color.GetColor();
	m_pLogPen->lopnStyle = m_Style;
}

void CLineListPage::OnSelchangeLineItem() 
{
	CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_LINE_ITEM);
	if (pBox == NULL) return;
	int nIndex = pBox->GetCurSel();
	if (nIndex != CB_ERR)
	{
		if (m_ItemNo >= 0)
		{
			int i = m_ItemNo;
			UpdateData(TRUE);
			GetLineIndirect(&m_LineList[i]);
		}
		SetLineIndirect(&m_LineList[m_ItemNo]);
		UpdateData(FALSE);
		Preview();
	}
}

void CLineListPage::AddPen(LOGPEN* pLogPen,CString str)
{
	if(m_LineNum<20)
	{
		m_LineList[m_LineNum] = *pLogPen;
		m_DescList.Add(str);
		m_PtrList[m_LineNum] = pLogPen;
		m_LineNum ++;
	}
}
