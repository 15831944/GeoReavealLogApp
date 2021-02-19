// AzimFreqDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImageMan.h"
#include "resource.h"
#include "ImageManDoc.h"
#include "style.h"
#include "AzimFreq.h"
#include "math.h"
#include "AzimFreqPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAzimFreqPage dialog

IMPLEMENT_DYNAMIC(CAzimFreqPage, CPropertyPage)

CAzimFreqPage::CAzimFreqPage(CAzimFreqObj* pDangDdirObj)
	: CPropertyPage(CAzimFreqPage::IDD)
{
	//{{AFX_DATA_INIT(CAzimFreqPage)
	m_CircleWidth = 0;
	m_AzimNum = 0;
	m_PieWidth = 0;
	m_CircleRadius = 0;
	//}}AFX_DATA_INIT
	ASSERT(pDangDdirObj != NULL);
	pObj = pDangDdirObj;
}


void CAzimFreqPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAzimFreqPage)
	DDX_Text(pDX, IDC_CIRCLE_WIDTH, m_CircleWidth);
	DDV_MinMaxUInt(pDX, m_CircleWidth, 0, 30);
	DDX_Text(pDX, IDC_PIE_NUM, m_AzimNum);
	DDV_MinMaxUInt(pDX, m_AzimNum, 3, 360);
	DDX_Text(pDX, IDC_PIE_WIDTH, m_PieWidth);
	DDV_MinMaxUInt(pDX, m_PieWidth, 1, 30);
	DDX_Text(pDX, IDC_CIRCLE_RADIUS, m_CircleRadius);
	DDV_MinMaxUInt(pDX, m_CircleRadius, 10, 500);
	DDX_Control(pDX, IDC_CIRCLE_COLOR, m_CircleColor);
	DDX_Control(pDX, IDC_PIE_COLOR, m_PieColor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAzimFreqPage, CPropertyPage)
	//{{AFX_MSG_MAP(CAzimFreqPage)
	ON_EN_CHANGE(IDC_PIE_WIDTH, OnChangePieWidth)
	ON_EN_CHANGE(IDC_PIE_NUM, OnChangePieNum)
	ON_EN_CHANGE(IDC_CIRCLE_WIDTH, OnChangeCircleWidth)
	ON_WM_PAINT()
	ON_EN_CHANGE(IDC_CIRCLE_RADIUS, OnChangeCircleRadius)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAzimFreqPage message handlers

BOOL CAzimFreqPage::OnInitDialog() 
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

void CAzimFreqPage::OnChangePieNum() 
{
    m_AzimNum = GetDlgItemInt(IDC_PIE_WIDTH);
    Preview();
}

void CAzimFreqPage::OnChangePieWidth() 
{
    m_PieWidth = GetDlgItemInt(IDC_PIE_WIDTH);
    Preview();
}

void CAzimFreqPage::OnChangeCircleWidth() 
{
    m_CircleWidth = GetDlgItemInt(IDC_CIRCLE_WIDTH);
    Preview();
}

void CAzimFreqPage::OnChangeCircleRadius() 
{
    m_CircleRadius = GetDlgItemInt(IDC_CIRCLE_RADIUS);
    Preview();
}

void CAzimFreqPage::OnOK() 
{
	CPropertyPage::OnOK();
	pObj->m_CircleColor = m_CircleColor.GetColor();
	pObj->m_CircleRadius = m_CircleRadius;
	pObj->m_CircleWidth = m_CircleWidth;
	pObj->m_PieColor = m_PieColor.GetColor();
	pObj->m_PieWidth = m_PieWidth;
	pObj->m_AzimNum = m_AzimNum;

}

void CAzimFreqPage::OnPaint() 
{
	CPaintDC dc(this); 
	Preview();
}

void CAzimFreqPage::Preview() 
{
}

/////////////////////////////////////////////////////////////////////////////
// CDangFreqPage dialog

IMPLEMENT_DYNAMIC(CDangFreqPage, CPropertyPage)

CDangFreqPage::CDangFreqPage(CAzimFreqObj* pDangDdirObj)
	: CPropertyPage(CDangFreqPage::IDD)
{
	//{{AFX_DATA_INIT(CDangFreqPage)
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


void CDangFreqPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDangFreqPage)
	DDX_Text(pDX, IDC_HORI_LENGTH, m_HoriLength);
	DDX_Text(pDX, IDC_HORI_WIDTH, m_HoriWidth);
	DDX_Text(pDX, IDC_VERT_HEIGHT, m_VertHeight);
	DDX_Text(pDX, IDC_VERT_NUM, m_VertNum);
	DDX_Text(pDX, IDC_VERT_WIDTH, m_VertWidth);
	DDX_Text(pDX, IDC_MAX_DANG, m_MaxDang);
	DDX_Text(pDX, IDC_MIN_DANG, m_MinDang);
	DDX_Text(pDX, IDC_DEPLEVEL, m_DepLevel);
//	DDV_MinMaxUInt(pDX, m_DepLevel, 5, 100);
	DDX_Control(pDX, IDC_HORI_COLOR, m_HoriColor);
	DDX_Control(pDX, IDC_VERT_COLOR, m_VertColor);
	DDX_Check(pDX, IDC_CHECK1, m_DrawDirect);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDangFreqPage, CPropertyPage)
	//{{AFX_MSG_MAP(CDangFreqPage)
	ON_EN_CHANGE(IDC_HORI_LENGTH, OnChangeHoriLength)
	ON_EN_CHANGE(IDC_HORI_WIDTH, OnChangeHoriWidth)
	ON_EN_CHANGE(IDC_VERT_HEIGHT, OnChangeVertHeight)
	ON_EN_CHANGE(IDC_VERT_NUM, OnChangeVertNum)
	ON_EN_CHANGE(IDC_VERT_WIDTH, OnChangeVertWidth)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDangFreqPage message handlers
	
void CDangFreqPage::OnChangeHoriLength() 
{
    m_HoriWidth = GetDlgItemInt(IDC_HORI_LENGTH);
    Preview();
}

void CDangFreqPage::OnChangeHoriWidth() 
{
    m_HoriWidth = GetDlgItemInt(IDC_HORI_WIDTH);
    Preview();
}

void CDangFreqPage::OnChangeVertHeight() 
{
    m_VertHeight = GetDlgItemInt(IDC_VERT_HEIGHT);
    Preview();
}

void CDangFreqPage::OnChangeVertNum() 
{
    m_VertNum = GetDlgItemInt(IDC_VERT_NUM);
    Preview();
}

void CDangFreqPage::OnChangeVertWidth() 
{
    m_VertWidth = GetDlgItemInt(IDC_VERT_WIDTH);
    Preview();
}

BOOL CDangFreqPage::OnInitDialog() 
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
	if(pObj->m_DrawDirect==VERDIR)
		m_DrawDirect=TRUE;
	else
		m_DrawDirect=FALSE;
	CPropertyPage::OnInitDialog();
	return TRUE;
}

void CDangFreqPage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	Preview();
}

void CDangFreqPage::Preview() 
{
	
}

void CDangFreqPage::OnOK() 
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
	EndDialog(IDOK);
}



void CAzimFreqPage::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnCancel();
}
