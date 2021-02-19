// DipDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "ImageManDoc.h"
#include "DevDazDraw.h"
#include "style.h"
#include "math.h"
#include "DevDazPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDevDazPage property page

IMPLEMENT_DYNAMIC(CDevDazPage, CPropertyPage)

CDevDazPage::CDevDazPage(CDevDazObj* pDevDazObj) : CPropertyPage(CDevDazPage::IDD)
{
	//{{AFX_DATA_INIT(CDevDazPage)
	m_CircleRadius = 0;
	m_TailLength = 0;
	m_TailWidth = 0;
	m_CircleWidth = 0;
	m_DepLevel = 0;
	m_LeftVal = 0.0f;
	m_RightVal = 0.0f;
	//}}AFX_DATA_INIT
	ASSERT(pDevDazObj != NULL);
	pObj = pDevDazObj;
//	m_psp.pszTitle = "DevDaz";
//	m_psp.dwFlags |= PSP_USETITLE;
}

CDevDazPage::~CDevDazPage()
{
}

void CDevDazPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDevDazPage)
	DDX_Control(pDX, IDC_CIRCLE_COLOR, m_CircleColor);
	DDX_Text(pDX, IDC_CIRCLE_RADIUS, m_CircleRadius);
	DDV_MinMaxUInt(pDX, m_CircleRadius, 1, 50);
	DDX_Control(pDX, IDC_TAIL_COLOR, m_TailColor);
	DDX_Text(pDX, IDC_TAIL_LENGTH, m_TailLength);
	DDV_MinMaxUInt(pDX, m_TailLength, 1, 50);
	DDX_Text(pDX, IDC_TAIL_WIDTH, m_TailWidth);
	DDV_MinMaxUInt(pDX, m_TailWidth, 1, 10);
	DDX_Text(pDX, IDC_CIRCLE_WIDTH, m_CircleWidth);
	DDV_MinMaxUInt(pDX, m_CircleWidth, 1, 10);
	DDX_Text(pDX, IDC_DEPTH_LEVEL, m_DepLevel);
	DDX_Text(pDX, IDC_DEV_LEFT_VAL, m_LeftVal);
	DDX_Text(pDX, IDC_DEV_RIGHT_VAL, m_RightVal);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDevDazPage, CPropertyPage)
	//{{AFX_MSG_MAP(CDevDazPage)
	ON_EN_CHANGE(IDC_CIRCLE_WIDTH, OnChangeCircleWidth)
	ON_EN_CHANGE(IDC_TAIL_LENGTH, OnChangeTailLength)
	ON_EN_CHANGE(IDC_TAIL_WIDTH, OnChangeTailWidth)
	ON_EN_CHANGE(IDC_CIRCLE_RADIUS, OnChangeCircleRadius)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDevDazPage message handlers

void CDevDazPage::OnChangeCircleWidth() 
{
    m_CircleWidth = GetDlgItemInt(IDC_CIRCLE_WIDTH);
    Preview();
}

void CDevDazPage::OnChangeTailLength() 
{
    m_TailLength = GetDlgItemInt(IDC_TAIL_LENGTH);
    Preview();
}

void CDevDazPage::OnChangeTailWidth() 
{
    m_TailWidth = GetDlgItemInt(IDC_TAIL_WIDTH);
    Preview();
}

void CDevDazPage::OnChangeCircleRadius() 
{
    m_CircleRadius = GetDlgItemInt(IDC_CIRCLE_RADIUS);
    Preview();
}

void CDevDazPage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	Preview();
}

void CDevDazPage::Preview()
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
	pWnd->ReleaseDC(pDC);   
}

BOOL CDevDazPage::OnInitDialog() 
{
	m_DepLevel = pObj->m_DepLevel;
	m_TailWidth = pObj->m_TailWidth;
	m_TailLength = pObj->m_TailLength;
	m_TailColor .SetColor(pObj->m_TailColor);
	m_CircleRadius = pObj->m_CircleRadius;
	m_CircleWidth = pObj->m_CircleWidth;
	m_CircleColor.SetColor(pObj->m_CircleColor);
	m_LeftVal = pObj->m_LeftVal;
	m_RightVal = pObj->m_RightVal;
	CPropertyPage::OnInitDialog();
	return TRUE;  
}

void CDevDazPage::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertyPage::OnOK();
	pObj->m_DepLevel = m_DepLevel;
	pObj->m_TailLength = m_TailLength;
	pObj->m_TailWidth = m_TailWidth;
	pObj->m_TailColor = m_TailColor.GetColor();
	pObj->m_CircleRadius = m_CircleRadius;
	pObj->m_CircleWidth = m_CircleWidth;
	pObj->m_CircleColor = m_CircleColor.GetColor();
	pObj->m_LeftVal = m_LeftVal;
	pObj->m_RightVal = m_RightVal;
}
