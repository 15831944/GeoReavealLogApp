
#include "stdafx.h" 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "CurveDraw.h"
#include "Style.h"     
#include "CurvePage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCurvePage dialog

CCurvePage::CCurvePage(CCurveObj* pCurveObj /*=NULL*/)
	: CPropertyPage(CCurvePage::IDD)
{
	m_CurveStyleBox.SetStyleType(0);
	m_pCurveObj = pCurveObj;
	//{{AFX_DATA_INIT(CCurvePage)
	m_LeftValue = 0.f;
	m_RightValue = 0.f;
	m_Factor = 0;
	m_ScaleType = -1;
	m_CurveStyle = 1;
	m_CurveWidth = 1;
	m_bEditEnable = FALSE;
	m_FilterNum = 0;
	//}}AFX_DATA_INIT
}          

CCurvePage::~CCurvePage()
{
}              

BOOL CCurvePage::OnInitDialog()
{                  
	CComboBox* pBox;
	pBox = (CComboBox*) GetDlgItem(IDC_SCALE_TYPE);
	ASSERT(pBox != NULL);
	if (pBox != NULL)
	{
		pBox->AddString("线性");
		pBox->AddString("对数");
	}
	flag = 0;
	m_LeftValue = m_pCurveObj->m_LeftVal;
	m_RightValue = m_pCurveObj->m_RightVal;
	m_ScaleType = m_pCurveObj->m_CalType;
	m_Factor = m_pCurveObj->m_Factor;
	m_CurveStyle = m_pCurveObj->m_CurveStyleNo - 1;
	m_CurveWidth = m_pCurveObj->m_CurveWidth;
	m_bEditEnable = m_pCurveObj->m_bDataEdit;
	m_FilterNum = m_pCurveObj->m_FilterNum;
	m_CurveColor.SetColor(m_pCurveObj->m_CurveColor);
	VERIFY(m_CurveStyleBox.SubclassDlgItem(IDC_CURVE_STYLE, this));
	m_CurveStyleBox.AddItem();
	CPropertyPage::OnInitDialog();
	CenterWindow();
	m_CurveWidth_Spin.SetRange(0,50);
	return TRUE;
}

void CCurvePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCurvePage)
	DDX_Text(pDX, IDC_LEFT_VALUE, m_LeftValue);
	DDX_Text(pDX, IDC_RIGHT_VALUE, m_RightValue);
	DDX_Radio(pDX, IDC_RADIO1, m_Factor);
	DDX_CBIndex(pDX, IDC_SCALE_TYPE, m_ScaleType);
	DDX_CBIndex(pDX, IDC_CURVE_STYLE, m_CurveStyle);
	DDX_Text(pDX, IDC_CURVE_WIDTH, m_CurveWidth);
	DDX_Control(pDX, IDC_CURVEWIDTH_SPIN, m_CurveWidth_Spin);
	DDX_Check(pDX, IDC_EBANLE_EDIT, m_bEditEnable);
	DDX_Text(pDX, IDC_FILTERNUM, m_FilterNum);
	DDX_Control(pDX, IDC_CURVE_COLOR, m_CurveColor);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCurvePage, CPropertyPage)
	//{{AFX_MSG_MAP(CCurvePage)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_CURVE_STYLE, OnSelchangeCurveStyle)
	ON_EN_CHANGE(IDC_CURVE_WIDTH, OnChangeCurveWidth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCurvePage message handlers

void CCurvePage::OnOK()
{
	CPropertyPage::OnOK();
	m_pCurveObj->m_LeftVal = m_LeftValue;
	m_pCurveObj->m_RightVal = m_RightValue;
	m_pCurveObj->m_CalType = m_ScaleType;
	m_pCurveObj->m_Factor = m_Factor;
	m_pCurveObj->m_CurveColor = m_CurveColor.GetColor();
	m_pCurveObj->m_CurveStyleNo = m_CurveStyle + 1;
	m_pCurveObj->m_CurveWidth = m_CurveWidth;
	m_pCurveObj->m_bDataEdit = m_bEditEnable;
	m_pCurveObj->m_FilterNum=m_FilterNum ;
}

void CCurvePage::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	Preview();
}

void CCurvePage::Preview()
{
	CWnd* pWnd = GetDlgItem(IDC_PREVIEW);
	if (pWnd == NULL) return;
	pWnd->Invalidate();
	pWnd->UpdateWindow();
	CRect rect ;
	pWnd->GetClientRect(&rect); 
	CDC* pDC = pWnd->GetDC();
	pDC->IntersectClipRect(rect);
	CRect rect1 = rect;	
	if(m_CurveStyle >= 0) 
	{
		CLinePen m_LinePen(m_CurveStyle+1,m_CurveWidth,m_CurveColor.GetColor());
		DrawHorLine(rect1.left+5,rect1.right-5,(rect1.top + rect1.bottom)/2,&m_LinePen,pDC);
	}	
	pWnd->ReleaseDC(pDC);
}

void CCurvePage::OnSelchangeCurveStyle()
{
    CComboBox* pBox = (CComboBox*) GetDlgItem(IDC_CURVE_STYLE);
    if (pBox == NULL) return;
	m_CurveStyle = pBox->GetCurSel();
    Preview();
}

void CCurvePage::OnChangeCurveWidth()
{
	m_CurveWidth = GetDlgItemInt(IDC_CURVE_WIDTH);
	Preview();
}

