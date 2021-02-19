// TrackBasePage.cpp : implementation file
//

#include "stdafx.h"
#include "PreView.h"
#include "LogPlotDoc.h"
#include "TrackDraw.h"
#include "TrackBasePage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrackBasePage property page

IMPLEMENT_DYNCREATE(CTrackBasePage, CPropertyPage)

CTrackBasePage::CTrackBasePage(CTrackObj* pTrackObj) : CPropertyPage(CTrackBasePage::IDD)
{
	//{{AFX_DATA_INIT(CTrackBasePage)
	m_DepthTrackPos = -1;
	m_DepthValuePos = -1;
	m_bShowDepth = FALSE;
	m_bShowTitle = FALSE;
	m_bTrackResize=FALSE;
	m_PlotEdep = 0.0;
	m_PlotSdep = 0.0;
	//}}AFX_DATA_INIT
	pObj = pTrackObj;
	m_DepthTrackPosition = pObj->m_DepthTrackPosition;
	m_DepthValuePosition = pObj->m_DepthPosition;
	bDrawDepth = pObj->bDrawDepth;
	bDrawTitle = pObj->bDrawTitle;
	bTrackResize=pObj->bTrackResize;
	m_bDrawRuler=FALSE;
	m_bLayerHi=TRUE;
}

CTrackBasePage::~CTrackBasePage()
{
}

void CTrackBasePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrackBasePage)
	DDX_Radio(pDX, IDC_DEPTH_TRACK_LEFT, m_DepthTrackPos);
	DDX_Radio(pDX, IDC_DEPTH_VALUE_LEFT, m_DepthValuePos);
	DDX_Check(pDX, IDC_SHOW_DEPTH, m_bShowDepth);
	DDX_Check(pDX, IDC_SHOW_TITLE, m_bShowTitle);
	DDX_Check(pDX, IDC_TRACKRESIZE, m_bTrackResize);
	DDX_Text(pDX, IDC_TRACK_EDEP, m_PlotEdep);
	DDX_Text(pDX, IDC_TRACK_SDEP, m_PlotSdep);
	//}}AFX_DATA_MAP
	DDX_Check(pDX,IDC_CHECK2,m_bDrawRuler);
	DDX_Check(pDX,IDC_CHECK1,m_bLayerHi);
}


BEGIN_MESSAGE_MAP(CTrackBasePage, CPropertyPage)
	//{{AFX_MSG_MAP(CTrackBasePage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrackBasePage message handlers

BOOL CTrackBasePage::OnInitDialog() 
{
	m_bShowDepth = bDrawDepth;
	m_bShowTitle = bDrawTitle;
	m_bTrackResize=bTrackResize;

	if (m_DepthTrackPosition == IDM_LEFT) m_DepthTrackPos = 0;
	else if (m_DepthTrackPosition == IDM_RIGHT) m_DepthTrackPos = 1;
	else if (m_DepthTrackPosition == IDM_CENTER) m_DepthTrackPos = 2;
	if (m_DepthValuePosition == IDM_LEFT) m_DepthValuePos = 0;
	else if (m_DepthValuePosition == IDM_RIGHT) m_DepthValuePos = 1;
	else if (m_DepthValuePosition == IDM_CENTER) m_DepthValuePos = 2;
	else if (m_DepthValuePosition == IDM_NONE) m_DepthValuePos = 3;
	m_PlotSdep =  pObj->m_PlotSdep;
	m_PlotEdep =  pObj->m_PlotEdep;
	m_bDrawRuler=pObj->m_bDrawRuler;
	CPropertyPage::OnInitDialog();
	if (pObj->GetParentObj() == NULL) 
	{
		bSetRange = (!pObj->pDoc->bVertAutoAlign) || (!pObj->pDoc->m_bVertAlignDepth);
	}	
	else
		bSetRange = !(pObj->GetParentObj()->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	if (!bSetRange) 
	{
		CWnd* pWnd = GetDlgItem(IDC_TRACK_SDEP);
		pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_TRACK_EDEP);
		pWnd->EnableWindow(FALSE);
	}
	return TRUE;
}

void CTrackBasePage::OnOK() 
{
	CPropertyPage::OnOK();
	bDrawDepth = m_bShowDepth;
	bDrawTitle = m_bShowTitle ;
	bTrackResize=m_bTrackResize;
	if (m_DepthTrackPos == 0) m_DepthTrackPosition = IDM_LEFT;
	else if (m_DepthTrackPos == 1) m_DepthTrackPosition = IDM_RIGHT;
	else if (m_DepthTrackPos == 2) m_DepthTrackPosition = IDM_CENTER;
	if (m_DepthValuePos == 0) m_DepthValuePosition = IDM_LEFT;
	else if (m_DepthValuePos == 1) m_DepthValuePosition = IDM_RIGHT;
	else if (m_DepthValuePos == 2) m_DepthValuePosition = IDM_CENTER;
	else if (m_DepthValuePos == 3) m_DepthValuePosition = IDM_NONE;
	if (bSetRange)
	{
		pObj->m_PlotSdep = m_PlotSdep;
		pObj->m_PlotEdep = m_PlotEdep;
	}
	pObj->m_bDrawRuler=m_bDrawRuler;
}
