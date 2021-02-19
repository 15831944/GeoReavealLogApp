
#include "stdafx.h"
#include "Style.h" 
#include "StyleDialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CLinePage::CLinePage(LOGPEN* pLogPen /*=NULL*/)
	: CPropertyPage(CLinePage::IDD)
{
	//{{AFX_DATA_INIT(CLinePage)
	m_Width = 0;
	m_Style = -1;
	//}}AFX_DATA_INIT
	if ((m_pLogPen = pLogPen) != NULL)
	{
		SetLineIndirect(pLogPen);
	}                            
	m_bWindowsPen = TRUE;
	m_szObjName=_T("");
}

void CLinePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinePage)
	DDX_Text(pDX, IDC_LINE_WIDTH, m_Width);
	DDX_Control(pDX, IDC_LINE_WIDTH_SPIN, m_WidthSpinCtrl);
	DDX_Control(pDX, IDC_LINE_COLOR, m_Color);
	DDX_CBIndex(pDX, IDC_LINE_STYLE, m_Style);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_OBJNAME, m_szObjName);
}


/////////////////////////////////////////////////////////////////////////////
// CLinePage message handlers

BOOL CLinePage::OnInitDialog()
{
	VERIFY(m_StyleBox.SubclassDlgItem(IDC_LINE_STYLE, this));
	m_StyleBox.AddItem();
	if (m_pLogPen != NULL)
	{
		SetLineIndirect(m_pLogPen);
	}                            
	CPropertyPage::OnInitDialog();
	CenterWindow();
	if (m_WidthSpinCtrl.m_hWnd != NULL) m_WidthSpinCtrl.SetRange(0,5);
	return TRUE;
}                               

void CLinePage::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	Preview();
}

void CLinePage::Preview()
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
		if (m_bWindowsPen)
		{
	    	CPen m_Pen;
			LOGPEN m_LogPen;
			GetLineIndirect(&m_LogPen);
			if (m_LogPen.lopnWidth.x > 0)
			{
			//	if ((m_Color < 0) || (m_Color > COLOR_NUM)) m_LogPen.lopnColor = 0;
				m_Pen.CreatePenIndirect(&m_LogPen);
				CPen* pOldPen = pDC->SelectObject(&m_Pen); 
				pDC->MoveTo(rect1.left + 5, (rect1.top + rect1.bottom)/2); 	
				pDC->LineTo(rect1.right - 5, (rect1.top + rect1.bottom)/2); 	
				pDC->SelectObject(pOldPen);
			}
		}
		else
	    {
			LOGPEN m_LogPen;
			GetLineIndirect(&m_LogPen);
    		CLinePen m_LinePen(m_LogPen.lopnStyle+1,m_LogPen.lopnWidth.x,m_LogPen.lopnColor);
			DrawHorLine(rect1.left+5,rect1.right-5,(rect1.top + rect1.bottom)/2,&m_LinePen,pDC);
	    }
	}	
	pWnd->ReleaseDC(pDC);
}

void CLinePage::OnSelchangeStyle()
{
    CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_LINE_STYLE);
    m_Style = pBox->GetCurSel();
    Preview();
}

void CLinePage::OnChangeWidth()
{
	// TODO: Add your control notification handler code here
	m_Width = GetDlgItemInt(IDC_LINE_WIDTH);
	if (m_Width > 5) m_Width = 5;
	if (m_Width < 0) m_Width = 0;
	Preview();
}

void CLinePage::OnOK()
{
	CPropertyPage::OnOK();
	if (m_pLogPen != NULL)
	{
		m_pLogPen->lopnWidth.x = m_Width;
		m_pLogPen->lopnWidth.y = m_Width;
		m_pLogPen->lopnColor = m_Color.GetColor();
		m_pLogPen->lopnStyle = m_Style;
	}	
}

void CLinePage::SetLineIndirect(LOGPEN *m_pLogPen)
{                  
	m_Width = m_pLogPen->lopnWidth.x;
	m_Color .SetColor(m_pLogPen->lopnColor);
	m_Style = m_pLogPen->lopnStyle;
}                                    

void CLinePage::GetLineIndirect(LOGPEN *m_pLogPen)
{
	m_pLogPen->lopnWidth.x = m_Width;
	m_pLogPen->lopnWidth.y = m_Width;
	m_pLogPen->lopnColor = m_Color.GetColor();
	m_pLogPen->lopnStyle = m_Style;
}

void CLinePage::SetPenType(BOOL bPenType)
{
	m_bWindowsPen = bPenType;
	m_StyleBox.SetStyleType(bPenType);	
}


BEGIN_MESSAGE_MAP(CLinePage, CPropertyPage)
	//{{AFX_MSG_MAP(CLinePage)
	ON_WM_PAINT()
	ON_EN_CHANGE(IDC_LINE_WIDTH, OnChangeWidth)
	ON_CBN_SELCHANGE(IDC_LINE_STYLE, OnSelchangeStyle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPatternPage dialog

CPatternPage::CPatternPage(LOGBRUSH* pLogBrush /*=NULL*/)
	: CPropertyPage(CPatternPage::IDD)
{
	//{{AFX_DATA_INIT(CPatternPage)
	m_Style = -1;
	//}}AFX_DATA_INIT
	if ((m_pLogBrush = pLogBrush) != NULL) 
	{             
		SetPatternIndirect(pLogBrush);
	}   
	else m_bWindowsPattern = TRUE;
	m_psp.pszTitle = "Fill";
	m_psp.dwFlags |= PSP_USETITLE;
}

void CPatternPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatternPage)
	DDX_Control(pDX, IDC_PATTERN_COLOR, m_Color);
	DDX_CBIndex(pDX, IDC_PATTERN_STYLE, m_Style);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPatternPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPatternPage)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_PATTERN_STYLE, OnSelchangeStyle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatternPage message handlers

BOOL CPatternPage::OnInitDialog()
{
	m_StyleBox.SetStyleType(m_bWindowsPattern);
	VERIFY(m_StyleBox.SubclassDlgItem(IDC_PATTERN_STYLE, this));
	m_StyleBox.AddItem();
	CPropertyPage::OnInitDialog();
	return TRUE;
}                               

void CPatternPage::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	Preview();
}

void CPatternPage::Preview()
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
   			LithFillBox(rect1.left,rect1.top,rect1.right,rect1.bottom,m_Style,m_Color.GetColor(),rect1.TopLeft (),pDC);
	}
	pWnd->ReleaseDC(pDC);   
}

void CPatternPage::OnSelchangeStyle()
{
    CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_PATTERN_STYLE);
    m_Style = pBox->GetCurSel();
    Preview();
}

void CPatternPage::OnOK()
{
	CPropertyPage::OnOK();
	if (m_pLogBrush != NULL)
	{
		GetPatternIndirect(m_pLogBrush);
	}	
}

void CPatternPage::SetPatternIndirect(LOGBRUSH *pLogBrush)
{                  
/*	if (pLogBrush->lbStyle == BS_SOLID) 
	{
		m_Style = 1;
	}*/
	if (pLogBrush->lbStyle == BS_NULL) 
	{ 
		m_Style = 0;
	}
//	else if (pLogBrush->lbStyle == BS_SOLID) m_Style = 1;
	else if ((pLogBrush->lbStyle == BS_HATCHED) || (pLogBrush->lbStyle == BS_OWNER)) 
		m_Style = pLogBrush->lbHatch + 1;
	m_bWindowsPattern = (pLogBrush->lbStyle != BS_OWNER);
	m_Color.SetColor(pLogBrush->lbColor);
}

void CPatternPage::GetPatternIndirect(LOGBRUSH *pLogBrush)
{
	if (m_Style == 0) pLogBrush->lbStyle = BS_NULL;
//	else if (m_Style == 1) pLogBrush->lbStyle = BS_SOLID;
	else pLogBrush->lbStyle = BS_HATCHED;
	if (!m_bWindowsPattern) pLogBrush->lbStyle = BS_OWNER;
	pLogBrush->lbColor = m_Color.GetColor();
	pLogBrush->lbHatch = m_Style - 1;
}

void CPatternPage::SetPatternType(BOOL bPatternType)
{
	m_bWindowsPattern = bPatternType;
	m_StyleBox.SetStyleType(bPatternType);
}


/////////////////////////////////////////////////////////////////////////////
// CInputTextDialog dialog

const DWORD CInputTextDialog::m_nHelpIDs[] = 
{
	0, 0
};

CInputTextDialog::CInputTextDialog(CString msg, CWnd* pParent)
	: CCSDialog(CInputTextDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputTextDialog)
	m_Text = "";
	m_StaticText = _T("");
	//}}AFX_DATA_INIT
	m_StaticText=msg;
	m_HelpPath += "Plot.hlp";
}

void CInputTextDialog::DoDataExchange(CDataExchange* pDX)
{
	CCSDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputTextDialog)
	DDX_Text(pDX, IDC_EDIT_TEXT, m_Text);
	DDX_Text(pDX, IDC_STATIC_TEXT, m_StaticText);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInputTextDialog, CCSDialog)
	//{{AFX_MSG_MAP(CInputTextDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CInputTextDialog::SetText(CString text)
{
	m_Text = text;
}

CString CInputTextDialog::GetText()
{
	return m_Text;
}

BOOL CInputTextDialog::OnInitDialog() 
{
	CCSDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowLong(GetSafeHwnd(),GWL_EXSTYLE,WS_EX_TOOLWINDOW);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
