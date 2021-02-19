// DipDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "ImageManDoc.h"
#include "style.h"
#include "math.h"
#include "MarkVectorPage.h"
#include "MarkVectorDraw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMarkVectorPage property page

IMPLEMENT_DYNAMIC(CMarkVectorPage, CPropertyPage)

CMarkVectorPage::CMarkVectorPage(CMarkVectorObj* VectorObj) : CPropertyPage(CMarkVectorPage::IDD)
{
	//{{AFX_DATA_INIT(CMarkVectorPage)
	m_CircleRadius = 0;
	m_TailLength = 0;
	m_TailWidth = 0;
	m_CircleWidth = 0;
	m_DepLevel = 0;
	m_HeadStyle=0;
	m_bPrintOriginPara = FALSE;
	m_bPrintTruePara = TRUE;
	m_ParaFormat = _T("%.f %.f");
	m_bPrintTrueDip = TRUE;
	m_bPrintOriginDip = TRUE;
	//}}AFX_DATA_INIT
	ASSERT(pDangDdirObj != NULL);
	pObj = VectorObj;
}

CMarkVectorPage::~CMarkVectorPage()
{
}

void CMarkVectorPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMarkVectorPage)
	DDX_Text(pDX, IDC_CIRCLE_RADIUS, m_CircleRadius);
	DDV_MinMaxUInt(pDX, m_CircleRadius, 1, 50);
	DDX_Text(pDX, IDC_TAIL_LENGTH, m_TailLength);
	DDV_MinMaxUInt(pDX, m_TailLength, 1, 50);
	DDX_Text(pDX, IDC_TAIL_WIDTH, m_TailWidth);
	DDV_MinMaxUInt(pDX, m_TailWidth, 1, 10);
	DDX_Text(pDX, IDC_CIRCLE_WIDTH, m_CircleWidth);
	DDV_MinMaxUInt(pDX, m_CircleWidth, 1, 10);
	DDX_Text(pDX, IDC_EDIT1, m_DepLevel);
	DDV_MinMaxUInt(pDX, m_DepLevel, 1, 10000);
	DDX_Radio(pDX, IDC_HEAD_STYLE, m_HeadStyle);
	DDX_Check(pDX, IDC_CHECK1, m_bPrintOriginPara);
	DDX_Control(pDX, IDC_CIRCLE_COLOR, m_CircleColor);
	DDX_Control(pDX, IDC_TAIL_COLOR, m_TailColor);
	DDX_Text(pDX, IDC_EDIT2, m_ParaFormat);
	DDX_Check(pDX, IDC_CHECK3, m_bPrintTrueDip);
	DDX_Check(pDX, IDC_CHECK2, m_bPrintOriginDip);
	DDX_Check(pDX, IDC_CHECK4, m_bPrintTruePara);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMarkVectorPage, CPropertyPage)
	//{{AFX_MSG_MAP(CMarkVectorPage)
	ON_EN_CHANGE(IDC_CIRCLE_WIDTH, OnChangeCircleWidth)
	ON_EN_CHANGE(IDC_TAIL_LENGTH, OnChangeTailLength)
	ON_EN_CHANGE(IDC_TAIL_WIDTH, OnChangeTailWidth)
	ON_EN_CHANGE(IDC_CIRCLE_RADIUS, OnChangeCircleRadius)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_HEAD_STYLE, OnHeadStyle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMarkVectorPage message handlers

void CMarkVectorPage::OnChangeCircleWidth() 
{
    m_CircleWidth = GetDlgItemInt(IDC_CIRCLE_WIDTH);
    Preview();
}

void CMarkVectorPage::OnChangeTailLength() 
{
    m_TailLength = GetDlgItemInt(IDC_TAIL_LENGTH);
    Preview();
}

void CMarkVectorPage::OnChangeTailWidth() 
{
    m_TailWidth = GetDlgItemInt(IDC_TAIL_WIDTH);
    Preview();
}

void CMarkVectorPage::OnChangeCircleRadius() 
{
    m_CircleRadius = GetDlgItemInt(IDC_CIRCLE_RADIUS);
    Preview();
}

void CMarkVectorPage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	Preview();
}

void CMarkVectorPage::Preview()
{
	CWnd* pWnd = GetDlgItem(IDC_DIP_PREVIEW);
	if (pWnd == NULL) return;
	pWnd->Invalidate();
	pWnd->UpdateWindow();
	CRect rect ;
	pWnd->GetClientRect(&rect); 
	CDC* pDC = pWnd->GetDC();
	pDC->IntersectClipRect(rect);
	CBrush br(m_CircleColor.GetColor());
	CPen m_Pen;
	m_Pen.CreatePen(0,m_CircleWidth,m_CircleColor.GetColor());
	CPen* pOldPen = pDC->SelectObject(&m_Pen);
	CBrush* pOldBrush = pDC->SelectObject(&br);
	


	int x,y,x1,x2,y1,y2;
	x = (rect.left + rect.right) / 2;
	y = (rect.top + rect.bottom) / 2;
	x1 = x - m_CircleRadius; x2 = x + m_CircleRadius;
    y1 = y - m_CircleRadius; y2 = y + m_CircleRadius;
	if(m_HeadStyle==1)
		pDC->Rectangle (x1,y1,x2,y2);
	else
	    pDC->Ellipse(x1,y1,x2,y2);           
    double dd = 3.1415927 / 4;	
	x1 = x + (int)(m_CircleRadius * sin(dd)+0.5);
	y1 = y - (int)(m_CircleRadius * cos(dd)+0.5);
	x2 = x + (int)((m_TailLength + m_CircleRadius) * sin(dd)+0.5);
	y2 = y - (int)((m_TailLength + m_CircleRadius) * cos(dd)+0.5);
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);	
	CPen m_Pen1;
	m_Pen1.CreatePen(0,m_TailWidth,m_TailColor.GetColor());
	pOldPen = pDC->SelectObject(&m_Pen1);
	pDC->MoveTo(x1,y1); pDC->LineTo(x2,y2);
	pDC->SelectObject(pOldPen);
		
	if(m_bPrintTruePara||m_bPrintOriginPara)
	{
		CFont hTextFont;
		hTextFont.CreateFont(
			-14,					        //lfHeight
			0,							//lfWidth
			0,							//lfEscapement
			0,							//lfOrientation
			FW_BOLD,					//lfWeight
			0,							//lfItalic
			0,							//lfUnderLine;
			0,							//lfStrikeOut;	
			ANSI_CHARSET,				//lfCharSet
			OUT_TT_ONLY_PRECIS,			//lfOutPrecision
			CLIP_DEFAULT_PRECIS,		//lfClipPrecision
			DEFAULT_QUALITY,			//lfQuality
			VARIABLE_PITCH | FF_SWISS,	//lfPitchAndFamily
			"Courier New"           //"MS Serif" "Courier New""Times New Roman"
		);
		CFont* pOldFont=pDC->SelectObject (&hTextFont);
		pDC->SetTextAlign(TA_LEFT|TA_TOP);
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor (RGB(0,0,0));
		float dip=20.34f,dir=189.5f;
		CString para;	
		para.Format (m_ParaFormat,dip,dir);
		pDC->TextOut (x+m_CircleRadius+3,y,para);
		pDC->SelectObject (pOldFont);
	}
	pWnd->ReleaseDC(pDC);   
}

BOOL CMarkVectorPage::OnInitDialog() 
{
	m_TailLength = pObj->m_TailLength;
	m_TailWidth = pObj->m_TailWidth;
	m_TailColor.SetColor(pObj->m_TailColor);
	m_CircleRadius = pObj->m_CircleRadius;
	m_CircleWidth = pObj->m_CircleWidth;
	m_CircleColor.SetColor(pObj->m_CircleColor);
	m_HeadStyle=pObj->m_HeadStyle;
	m_bPrintOriginPara=pObj->m_bPrintOriginPara;
	m_bPrintTruePara=pObj->m_bPrintTruePara;
	m_ParaFormat=pObj->m_ParaFormat;
	m_bPrintTrueDip=pObj->m_bPrintTrueDip;
	m_bPrintOriginDip=pObj->m_bPrintOriginDip;
	CPropertyPage::OnInitDialog();
	return TRUE;  
}

void CMarkVectorPage::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertyPage::OnOK();
	pObj->m_TailLength = m_TailLength;
	pObj->m_TailWidth = m_TailWidth;
	pObj->m_TailColor = m_TailColor.GetColor();
	pObj->m_CircleRadius = m_CircleRadius;
	pObj->m_CircleWidth = m_CircleWidth;
	pObj->m_CircleColor = m_CircleColor.GetColor();
	pObj->m_HeadStyle=m_HeadStyle;
	pObj->m_bPrintOriginPara=m_bPrintOriginPara;
	pObj->m_bPrintTruePara=m_bPrintTruePara;
	pObj->m_ParaFormat=m_ParaFormat;
	pObj->m_bPrintTrueDip=m_bPrintTrueDip;
	pObj->m_bPrintOriginDip=m_bPrintOriginDip;
}

void CMarkVectorPage::OnRadio2() 
{
	// TODO: Add your control notification handler code here
	UpdateData();	
	Preview();
}

void CMarkVectorPage::OnHeadStyle() 
{
	// TODO: Add your control notification handler code here
	UpdateData();	
	Preview();
}
