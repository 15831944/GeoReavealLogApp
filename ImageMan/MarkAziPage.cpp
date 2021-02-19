// AzimFreqDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "resource.h"
#include "ImageManDoc.h"
#include "style.h"
#include "MarkFreq.h"
#include "math.h"
#include "MarkAziPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMarkAziPage dialog

IMPLEMENT_DYNAMIC(CMarkAziPage, CPropertyPage)

CMarkAziPage::CMarkAziPage(CMarkFreqObj* pDangDdirObj)
	: CPropertyPage(CMarkAziPage::IDD)
{
	//{{AFX_DATA_INIT(CMarkAziPage)
	m_CircleWidth = 0;
	m_AzimNum = 0;
	m_PieWidth = 0;
	m_CircleRadius = 0;
	//}}AFX_DATA_INIT
	ASSERT(pDangDdirObj != NULL);
	pObj = pDangDdirObj;
}


void CMarkAziPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMarkAziPage)
	DDX_Text(pDX, IDC_CIRCLE_WIDTH, m_CircleWidth);
	DDV_MinMaxUInt(pDX, m_CircleWidth, 0, 3);
	DDX_Text(pDX, IDC_PIE_NUM, m_AzimNum);
	DDV_MinMaxUInt(pDX, m_AzimNum, 3, 30);
	DDX_Text(pDX, IDC_PIE_WIDTH, m_PieWidth);
	DDV_MinMaxUInt(pDX, m_PieWidth, 0, 3);
	DDX_Text(pDX, IDC_CIRCLE_RADIUS, m_CircleRadius);
	DDV_MinMaxUInt(pDX, m_CircleRadius, 10, 100);
	DDX_Control(pDX, IDC_CIRCLE_COLOR, m_CircleColor);
	DDX_Control(pDX, IDC_PIE_COLOR, m_PieColor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMarkAziPage, CPropertyPage)
	//{{AFX_MSG_MAP(CMarkAziPage)
	ON_EN_CHANGE(IDC_PIE_WIDTH, OnChangePieWidth)
	ON_EN_CHANGE(IDC_PIE_NUM, OnChangePieNum)
	ON_EN_CHANGE(IDC_CIRCLE_WIDTH, OnChangeCircleWidth)
	ON_EN_CHANGE(IDC_CIRCLE_RADIUS, OnChangeCircleRadius)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMarkAziPage message handlers

BOOL CMarkAziPage::OnInitDialog() 
{
	m_CircleColor .SetColor(pObj->m_CircleColor);
	m_CircleRadius = pObj->m_CircleRadius;
	m_CircleWidth = pObj->m_CircleWidth;
	m_PieColor .SetColor(pObj->m_PieColor);
	m_PieWidth = pObj->m_PieWidth;
	m_AzimNum = pObj->m_AzimNum;
	CPropertyPage::OnInitDialog();
	return TRUE; 
}

void CMarkAziPage::OnChangePieNum() 
{
    m_AzimNum = GetDlgItemInt(IDC_PIE_WIDTH);
}

void CMarkAziPage::OnChangePieWidth() 
{
    m_PieWidth = GetDlgItemInt(IDC_PIE_WIDTH);
}

void CMarkAziPage::OnChangeCircleWidth() 
{
    m_CircleWidth = GetDlgItemInt(IDC_CIRCLE_WIDTH);
}

void CMarkAziPage::OnChangeCircleRadius() 
{
    m_CircleRadius = GetDlgItemInt(IDC_CIRCLE_RADIUS);
}

void CMarkAziPage::OnOK() 
{
	CPropertyPage::OnOK();
	pObj->m_CircleColor = m_CircleColor.GetColor();
	pObj->m_CircleRadius = m_CircleRadius;
	pObj->m_CircleWidth = m_CircleWidth;
	pObj->m_PieColor = m_PieColor.GetColor();
	pObj->m_PieWidth = m_PieWidth;
	pObj->m_AzimNum = m_AzimNum;
}

/////////////////////////////////////////////////////////////////////////////
// CMarkDipPage dialog

IMPLEMENT_DYNAMIC(CMarkDipPage, CPropertyPage)

CMarkDipPage::CMarkDipPage(CMarkFreqObj* pDangDdirObj)
	: CPropertyPage(CMarkDipPage::IDD)
{
	//{{AFX_DATA_INIT(CMarkDipPage)
	m_HoriLength = 0;
	m_HoriWidth = 0;
	m_VertHeight = 0;
	m_VertNum = 0;
	m_VertWidth = 0;
	m_MaxDang = 90;
	m_MinDang = 0;
	m_DepLevel = 10;
	m_DrawDirect = FALSE;
	//}}AFX_DATA_INIT
	ASSERT(pDangDdirObj != NULL);
	pObj = pDangDdirObj;
}


void CMarkDipPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMarkDipPage)
	DDX_Text(pDX, IDC_HORI_LENGTH, m_HoriLength);
	DDX_Text(pDX, IDC_HORI_WIDTH, m_HoriWidth);
	DDX_Text(pDX, IDC_VERT_HEIGHT, m_VertHeight);
	DDX_Text(pDX, IDC_VERT_NUM, m_VertNum);
	DDX_Text(pDX, IDC_VERT_WIDTH, m_VertWidth);
	DDX_Text(pDX, IDC_MAX_DANG, m_MaxDang);
	DDX_Text(pDX, IDC_MIN_DANG, m_MinDang);
	DDX_Text(pDX, IDC_DEPLEVEL, m_DepLevel);
	DDV_MinMaxUInt(pDX, m_DepLevel, 1, 100);
	DDX_Control(pDX, IDC_HORI_COLOR, m_HoriColor);
	DDX_Control(pDX, IDC_VERT_COLOR, m_VertColor);
	DDX_Check(pDX, IDC_CHECK1, m_DrawDirect);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK2, m_bDrawDipFreq);
}


BEGIN_MESSAGE_MAP(CMarkDipPage, CPropertyPage)
	//{{AFX_MSG_MAP(CMarkDipPage)
	ON_EN_CHANGE(IDC_HORI_LENGTH, OnChangeHoriLength)
	ON_EN_CHANGE(IDC_HORI_WIDTH, OnChangeHoriWidth)
	ON_EN_CHANGE(IDC_VERT_HEIGHT, OnChangeVertHeight)
	ON_EN_CHANGE(IDC_VERT_NUM, OnChangeVertNum)
	ON_EN_CHANGE(IDC_VERT_WIDTH, OnChangeVertWidth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMarkDipPage message handlers
	
void CMarkDipPage::OnChangeHoriLength() 
{
    m_HoriWidth = GetDlgItemInt(IDC_HORI_LENGTH);
}

void CMarkDipPage::OnChangeHoriWidth() 
{
    m_HoriWidth = GetDlgItemInt(IDC_HORI_WIDTH);
}

void CMarkDipPage::OnChangeVertHeight() 
{
    m_VertHeight = GetDlgItemInt(IDC_VERT_HEIGHT);
}

void CMarkDipPage::OnChangeVertNum() 
{
    m_VertNum = GetDlgItemInt(IDC_VERT_NUM);
}

void CMarkDipPage::OnChangeVertWidth() 
{
    m_VertWidth = GetDlgItemInt(IDC_VERT_WIDTH);
}

BOOL CMarkDipPage::OnInitDialog() 
{
	m_HoriColor .SetColor(pObj->m_DangBaseLineColor);
	m_HoriLength = pObj->m_DangBaseLineLength;
	m_HoriWidth = pObj->m_DangBaseLineWidth;
	m_VertColor .SetColor(pObj->m_DangFreqLineColor);
	m_VertHeight = pObj->m_DangFreqLineLength;
	m_VertWidth = pObj->m_DangFreqLineWidth;
	m_VertNum = pObj->m_DangNum;
	m_MinDang = (UINT)pObj->m_LeftVal;
	m_MaxDang = (UINT)pObj->m_RightVal;
	m_DepLevel = (UINT)pObj->m_DepLevel;
	if(pObj->m_DrawDirect == VERDIR)
		m_DrawDirect=TRUE;
	else
		m_DrawDirect=FALSE;
	m_bDrawDipFreq=pObj->m_bDrawDipFreq;
	CPropertyPage::OnInitDialog();
	return TRUE;
}

void CMarkDipPage::OnOK() 
{
	UpdateData(TRUE);
	if (m_MinDang >= m_MaxDang)
	{
		AfxMessageBox(IDS_DIP_SCOPE_ERROR);
		return;
	}
	pObj->m_DangBaseLineColor = m_HoriColor.GetColor();
	pObj->m_DangBaseLineLength = m_HoriLength;
	pObj->m_DangBaseLineWidth = m_HoriWidth;
	pObj->m_DangFreqLineColor = m_VertColor.GetColor();
	pObj->m_DangFreqLineLength = m_VertHeight;
	pObj->m_DangFreqLineWidth = m_VertWidth;
	pObj->m_DangNum = m_VertNum;
	pObj->m_LeftVal = (float)m_MinDang;
	pObj->m_RightVal = (float)m_MaxDang;
	pObj->m_DepLevel = m_DepLevel;
	if(m_DrawDirect)
		pObj->m_DrawDirect=VERDIR;
	else
		pObj->m_DrawDirect=HORDIR;
	pObj->m_bDrawDipFreq =m_bDrawDipFreq;
	EndDialog(IDOK);
}



void CMarkAziPage::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class	
	CPropertyPage::OnCancel();
}
