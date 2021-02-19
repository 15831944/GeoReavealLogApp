// XGRAPH.cpp : Definiert den Einsprungpunkt für die DLL-Anwendung.
//

#include "stdafx.h"
#include "XGRAPH.h"
#include "XGraphLabel.h"
#include "AxisDlg.h"
#include "ChartPage.h"
#include "CurveDlg.h"
#include "ChartDlg.h"
#include <afxpriv.h>

#include "float.h"
#include "GfxUtils.h"
#include "math.h"
#include "MemDC.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// XGraph.cpp: Implementation
//

/////////////////////////////////////////////////////////////////////////////
// CXGraph

IMPLEMENT_SERIAL( CXGraph, CWnd, 1 )
  
#pragma warning (disable : 4244)
#pragma warning (disable : 4800)
	
CXGraph::CXGraph()
{
	WNDCLASS wndcls;

	// setup drawing function pointers
	m_pDrawFn[0] = DrawRect;
	m_pDrawFn[1] = DrawCircle;
	m_pDrawFn[2] = DrawCross;
	m_pDrawFn[3] = DrawRhombus;
	m_pDrawFn[4] = DrawLeftTriangle;
	m_pDrawFn[5] = DrawUpTriangle;
	m_pDrawFn[6] = DrawRightTriangle;
	m_pDrawFn[7] = DrawDownTriangle;
	
    HINSTANCE hInst   = AfxGetInstanceHandle();
	m_crBackColor     = RGB(255,255,255);
	m_crGraphColor    = RGB(255,255,255);
	m_crInnerColor    = RGB(240,240,240);
	m_nLeftMargin     = 
	m_nTopMargin      = 
	m_nRightMargin    =
	m_nBottomMargin   = 5;
	m_LegendAlignment = right;
	m_bLButtonDown    = false;
	m_bObjectSelected = false;
	m_bDoubleBuffer   = true;
	m_bShowLegend     = true;
	m_bInteraction    = true;
	m_nSelectedSerie  = -1;
	m_pTracker        = NULL;
	m_pCurrentObject  = NULL;
	m_pPrintDC        = NULL;
	m_nAxisSpace      = 5;
	m_OldPoint        = CPoint(0,0);
	m_opOperation	  = opNone;
	m_bSnapCursor     = true;
	m_nSnapRange      = 10;
	m_cPrintHeader    = _T("");
	m_cPrintFooter    = _T("");
	m_pCurrentObject  = NULL;
	
	m_OldCursorRect.SetRectEmpty();

	// Create one legend for the cursor
	m_CursorLabel.m_bVisible = false;
	m_CursorLabel.m_bCanEdit = false;
	m_CursorLabel.m_bBorder  = true;
	m_CursorLabel.m_clRect.SetRect(65,10,180,40);

	m_Objects.AddHead (&m_CursorLabel);

    // Register window class
    if (!(::GetClassInfo(hInst, "XGraph", &wndcls)))
    {
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = 0;
		wndcls.cbWndExtra		= 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = "XGraph";

        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return ;
        }
    }
}

CXGraph::~CXGraph()
{
	ResetAll();
}


bool CXGraph::DeleteXAxis(int nAxis)
{
	if (nAxis < 0 || nAxis >= m_XAxis.size())
		return false;

	m_XAxis.erase (&m_XAxis[nAxis]);
	
	return true;
}

bool CXGraph::DeleteYAxis(int nAxis)
{
	if (nAxis < 0 || nAxis >= m_YAxis.size())
		return false;

	m_YAxis.erase (&m_YAxis[nAxis]);
	
	return true;
}

bool CXGraph::DeleteCurve(int nCurve)
{
	if (nCurve < 0 || nCurve >= m_Data.size())
		return false;
	
	m_Data.erase (&m_Data[nCurve]);

	return true;
}

CXGraphDataSerie& CXGraph::SetData(TDataPoint* pValues, long nCount, int nCurve, int nXAxis, int nYAxis, bool bAutoDelete)
{
	VERIFY (nCurve <= m_Data.size());
	VERIFY (nXAxis <= m_XAxis.size());
	VERIFY (nYAxis <= m_YAxis.size());
		
	if (nCurve == m_Data.size ())
	{
		// New data serie

		CXGraphDataSerie serie;
		
		serie.m_bAutoDelete = bAutoDelete;
		serie.m_nXAxis		= nXAxis;
		serie.m_nYAxis		= nYAxis;
		serie.m_nCount		= nCount;
		serie.m_nIndex      = nCurve;

		if (bAutoDelete)
		{
			serie.m_pData = (TDataPoint*) new TDataPoint[nCount];
			memcpy(serie.m_pData, pValues, sizeof(TDataPoint) * nCount);
		}
		else
			serie.m_pData = pValues;
		
		serie.m_crColor = BASECOLORTABLE[m_Data.size () % 12];

		m_Data.push_back (serie);
	}
	else
	{
		// existing data serie, just update

		m_Data[nCurve].m_nXAxis = nXAxis;
		m_Data[nCurve].m_nYAxis = nYAxis;
		m_Data[nCurve].m_pData  = pValues;
		m_Data[nCurve].m_nCount = nCount;
		m_Data[nCurve].m_bAutoDelete = bAutoDelete;
	}

	// Need additional X-Axis
	if (nXAxis == m_XAxis.size())
	{
		CXGraphAxis axis;
		axis.m_pGraph   = this;
		axis.m_AxisKind = CXGraphAxis::xAxis;
		m_XAxis.push_back (axis);
	}
	
	// Need additional Y-Axis
	if (nYAxis == m_YAxis.size())
	{
		CXGraphAxis axis;
		axis.m_pGraph   = this;
		axis.m_AxisKind = CXGraphAxis::yAxis;
		m_YAxis.push_back (axis);
	}

	m_Data[nCurve].m_pGraph = this;
	
	// Get min/max for data serie
	double fxMin = m_XAxis[nXAxis].m_fMin;
	double fxMax = m_XAxis[nXAxis].m_fMax;
	double fyMin = m_YAxis[nYAxis].m_fMin;
	double fyMax = m_YAxis[nYAxis].m_fMax;

	GetMinMaxForData(m_Data[nCurve], fxMin, 
									 fxMax,
									 fyMin, 
									 fyMax);

	// Set axis ranges
	VERIFY(m_XAxis[nXAxis].SetRange(fxMin, fxMax));
	VERIFY(m_YAxis[nYAxis].SetRange(fyMin, fyMax));
	
	Invalidate(TRUE);
		
	return m_Data[nCurve];
}


CXGraphAxis& CXGraph::GetXAxis(int nAxis)
{
	ASSERT(nAxis >= 0 && nAxis < m_XAxis.size());
	return m_XAxis[nAxis]; 
}

CXGraphAxis& CXGraph::GetYAxis(int nAxis)
{
	ASSERT(nAxis >= 0 && nAxis < m_YAxis.size());
	return m_YAxis[nAxis]; 
}

CXGraphDataSerie& CXGraph::GetCurve(int nCurve)
{
	ASSERT(nCurve >= 0 && nCurve < m_Data.size());
	return m_Data[nCurve];
}

void CXGraph::GetMinMaxForData (CXGraphDataSerie& serie, double& fXMin, double& fXMax, double& fYMin, double& fYMax)
{	
	// check mins and maxs
	for (long i = 0; i < serie.m_nCount; i++)
	{
		if (serie.m_pData[i].fXVal > fXMax)
			fXMax = serie.m_pData[i].fXVal;

		if (serie.m_pData[i].fXVal < fXMin)
			fXMin = serie.m_pData[i].fXVal;

		if (serie.m_pData[i].fYVal > fYMax)
			fYMax = serie.m_pData[i].fYVal;

		if (serie.m_pData[i].fYVal < fYMin)
			fYMin = serie.m_pData[i].fYVal;
	}
}

// Returns the index for the given axis
int CXGraph::GetAxisIndex(CXGraphAxis* pAxis)
{
	for (int x = 0; x < m_XAxis.size(); x++)
		if (pAxis == &m_XAxis[x])
			return x;
	
	for (int y = 0; y < m_YAxis.size(); y++)
		if (pAxis == &m_YAxis[y])
			return y;

	return -1;
}

// Returns the index for the given curve
int CXGraph::GetCurveIndex(CXGraphDataSerie* pSerie)
{
	for (int i = 0; i < m_Data.size (); i++)
		if (pSerie == &m_Data[i])
			return i;

	return -1;
}

void CXGraph::SetGraphMargins(int nLeft, int nTop, int nRight, int nBottom)
{
	m_nLeftMargin   = nLeft;
	m_nTopMargin    = nTop;
	m_nRightMargin  = nRight;
	m_nBottomMargin = nBottom;
}

void CXGraph::DrawZoom (CDCEx* pDC)
{
	CRect zoomRect(m_MouseDownPoint.x, m_MouseDownPoint.y, m_CurrentPoint.x, m_CurrentPoint.y);

	zoomRect.NormalizeRect ();

	zoomRect.left = max(zoomRect.left, m_clInnerRect.left);
	zoomRect.top = max(zoomRect.top, m_clInnerRect.top);
	zoomRect.right = min(zoomRect.right, m_clInnerRect.right);
	zoomRect.bottom = min(zoomRect.bottom, m_clInnerRect.bottom);


	// If not in doublebuffer mode and zoomrect is not empty delete old zoomrect
	if (!m_bDoubleBuffer && !m_clCurrentZoom.IsRectEmpty ())
		pDC->DrawFocusRect (m_clCurrentZoom);

	m_clCurrentZoom = zoomRect;

	if (m_bDoubleBuffer)
	{
		// In db-mode we use a semi-transparent zoomrect ...
		CBrushSelector bs(RGB(150,150,255), pDC);//RGB(150,150,255)
		COLORREF crOldBkColor = pDC->SetBkColor (m_crInnerColor); 
		int	nOldROP2 = pDC->SetROP2 (R2_NOTXORPEN);

		pDC->Rectangle(zoomRect);
		pDC->SetBkColor (crOldBkColor);
		pDC->SetROP2 (nOldROP2);
	}
	else
		// ... otherwise a simple focusrect
		pDC->DrawFocusRect (m_clCurrentZoom);
	
}

CXGraphLabel& CXGraph::InsertLabel(CRect rect, CString cText)
{
	CXGraphLabel *pLabel = new CXGraphLabel;

	pLabel->m_clRect = rect;
	pLabel->m_cText  = cText;
	pLabel->m_pGraph = this;
	
	m_Objects.AddTail (pLabel);

	return *pLabel;
}

CXGraphLabel& CXGraph::InsertLabel(CString cText)
{
	CXGraphLabel *pLabel = new CXGraphLabel;

	CRect rect(m_clInnerRect.CenterPoint().x - 50,m_clInnerRect.CenterPoint().y - 50,
		       m_clInnerRect.CenterPoint().x + 50,m_clInnerRect.CenterPoint().y + 50);
	
	pLabel->m_clRect = rect;
	pLabel->m_cText  = cText;
	pLabel->m_pGraph = this;
	
	m_Objects.AddTail (pLabel);

	return *pLabel;
}

void CXGraph::Cursor()
{
//	Pan();
	m_nSnappedCurve = -1;
	m_oldCursorPoint = CPoint(-1, -1);
	m_opOperation = opCursor;
}

void CXGraph::Zoom()
{
	m_CursorLabel.m_bVisible = false;
	m_clCurrentZoom.SetRectEmpty ();
	m_opOperation = opZoom;
	Invalidate();
}

void CXGraph::ResetZoom()
{
	m_CursorLabel.m_bVisible = false;
	for (int y = 0; y < m_YAxis.size(); y++)
		m_YAxis[y].Reset();
	for (int x = 0; x < m_XAxis.size(); x++)
		m_XAxis[x].Reset();
	Invalidate();
}

void CXGraph::NoOp()
{
	m_CursorLabel.m_bVisible = false;
	m_opOperation = opNone;
	Invalidate();
}

void CXGraph::Pan()
{
	m_CursorLabel.m_bVisible = false;
	m_opOperation = opPan;

	for (int y = 0; y < m_YAxis.size (); y++)
		m_YAxis[y].m_bAutoScale = false;
	
	for (int x = 0; x < m_XAxis.size (); x++)
		m_XAxis[x].m_bAutoScale = false;

	Invalidate();
}

void CXGraph::DoPan(CPoint point)
{
	for (int y = 0; y < m_YAxis.size (); y++)
	{
		double fY1 = m_YAxis[y].GetValueForPos (point.y);
		double fY2 = m_YAxis[y].GetValueForPos (m_OldPoint.y);
		double fOffset = fY1 - fY2;
		VERIFY(m_YAxis[y].SetCurrentRange(m_YAxis[y].m_fCurMin - fOffset,m_YAxis[y].m_fCurMax - fOffset));
	}

	for (int x = 0; x < m_XAxis.size (); x++)
	{
		double fX1 = m_XAxis[x].GetValueForPos (point.x);
		double fX2 = m_XAxis[x].GetValueForPos (m_OldPoint.x);
		double fOffset = fX1 - fX2;
		VERIFY(m_XAxis[x].SetCurrentRange(m_XAxis[x].m_fCurMin - fOffset,m_XAxis[x].m_fCurMax - fOffset));
	}

	m_OldPoint = point;

	Invalidate();
}

void CXGraph::DoZoom()
{		
	if (m_clCurrentZoom.Width () < MIN_ZOOM_PIXELS ||
		m_clCurrentZoom.Height () < MIN_ZOOM_PIXELS)
		return;

	ZOOM zoom;
			
	for (int y = 0; y < m_YAxis.size(); y++)
	{
		double fMin, fMax;

		fMin = m_YAxis[y].GetValueForPos(min(m_clCurrentZoom.bottom, m_clInnerRect.bottom));
		fMax = m_YAxis[y].GetValueForPos(max(m_clCurrentZoom.top, m_clInnerRect.top));

		if ((fMax - fMin) != 0.0)
		{			
			zoom.fMin = m_YAxis[y].m_fCurMin;
			zoom.fMax = m_YAxis[y].m_fCurMax;
	
			m_YAxis[y].m_ZoomHistory.push_back (zoom);

			m_YAxis[y].m_fCurMin = fMin;
			m_YAxis[y].m_fCurMax = fMax;
			m_YAxis[y].SetBestStep ();

		}
	}

	for (int x = 0; x < m_XAxis.size(); x++)
	{
		double fMin, fMax;

		fMin = m_XAxis[x].GetValueForPos(max(m_clCurrentZoom.left, m_clInnerRect.left));
		fMax = m_XAxis[x].GetValueForPos(min(m_clCurrentZoom.right, m_clInnerRect.right));
		
		if ((fMax - fMin) != 0.0)
		{
			zoom.fMin = m_XAxis[x].m_fCurMin;
			zoom.fMax = m_XAxis[x].m_fCurMax;
	
			m_XAxis[x].m_ZoomHistory.push_back (zoom);
	
			m_XAxis[x].m_fCurMin = fMin;
			m_XAxis[x].m_fCurMax = fMax;
			m_XAxis[x].SetBestStep();
			
		}
	}

	::PostMessage(GetParent()->m_hWnd, XG_ZOOMCHANGE, 0, (long) this);
	
	m_clCurrentZoom.SetRectEmpty ();
}

int CXGraph::GetZoomDepth()
{
	if (m_XAxis.size() > 0)
		return m_XAxis[0].m_ZoomHistory.size();

	return 0;

}
void CXGraph::RestoreLastZoom()
{		
	for (int y = 0; y < m_YAxis.size(); y++)
	{
		if (m_YAxis[y].m_ZoomHistory.size() > 0)
		{
			m_YAxis[y].m_fCurMin = m_YAxis[y].m_ZoomHistory[m_YAxis[y].m_ZoomHistory.size() - 1].fMin;
			m_YAxis[y].m_fCurMax = m_YAxis[y].m_ZoomHistory[m_YAxis[y].m_ZoomHistory.size() - 1].fMax;
			m_YAxis[y].m_ZoomHistory.pop_back ();
			m_YAxis[y].SetBestStep ();

		}
	}

	for (int x = 0; x < m_XAxis.size(); x++)
	{
		if (m_XAxis[x].m_ZoomHistory.size() > 0)
		{
			m_XAxis[x].m_fCurMin = m_XAxis[x].m_ZoomHistory[m_XAxis[x].m_ZoomHistory.size() - 1].fMin;
			m_XAxis[x].m_fCurMax = m_XAxis[x].m_ZoomHistory[m_XAxis[x].m_ZoomHistory.size() - 1].fMax;
			m_XAxis[x].m_ZoomHistory.pop_back ();
			m_XAxis[x].SetBestStep ();

		}
	}
	
	::PostMessage(GetParent()->m_hWnd, XG_ZOOMCHANGE, 0, (long) this);
	
	m_clCurrentZoom.SetRectEmpty ();

}

BEGIN_MESSAGE_MAP(CXGraph, CWnd)
	//{{AFX_MSG_MAP(CXGraph)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETCURSOR()
	ON_WM_RBUTTONUP()
	ON_WM_KEYUP()
	ON_COMMAND(IDM_ZOOM, Zoom )
	ON_COMMAND(IDM_PAN, Pan )
	ON_COMMAND(IDM_CURSOR, Cursor )
	ON_COMMAND(IDM_SELECT, NoOp )
	ON_COMMAND(IDM_RESET, ResetZoom )
	ON_COMMAND(IDM_INSERTLABEL, InsertEmptyLabel)
	ON_COMMAND(IDM_PROPERTIES, OnProperties)
	ON_COMMAND(IDM_PRINT, OnPrint)
	ON_COMMAND(IDM_LINEARTREND, LinearTrend)
	ON_COMMAND(IDM_CUBICTREND, CubicTrend)
	ON_COMMAND(IDM_PARENTCALLBACK, ParentCallback)
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CXGraph 

void CXGraph::DrawLegend(CDCEx *pDC, CRect& ChartRect)
{
	int nLegendWidth = 0;
	int nLegendHeight = 0;
	int nItemHeight = 0;

	CQuickFont font("Times New Roman", 13, FW_NORMAL);//Arial 13
	{
		CFontSelector fs(&font, pDC, false);

		for (int i = 0; i < m_Data.size(); i++)
		{
			if (!m_Data[i].m_bVisible)
				continue;

			CString cLegend;
			cLegend.Format("%02d %s", i + 1, m_Data[i].m_cLabel);
			nLegendWidth = max(nLegendWidth, pDC->GetTextExtent (cLegend).cx);
			nLegendHeight += (nItemHeight = pDC->GetTextExtent (cLegend).cy);
		}
	}

	CFontSelector fs(&font, pDC);

	nLegendWidth += 16;

	CRect legendRect;
	
	if (m_LegendAlignment == left)
	{
		legendRect.SetRect (ChartRect.left + 10, ChartRect.top + 10, ChartRect.left + 10 + nLegendWidth, ChartRect.top + 12 + nLegendHeight);
		ChartRect.left += (nLegendWidth + 20);
	}
	
	if (m_LegendAlignment == right)
	{
		legendRect.SetRect (ChartRect.right - 10 - nLegendWidth, ChartRect.top + 10, ChartRect.right - 10, ChartRect.top + 12 + nLegendHeight);
		ChartRect.right -= (nLegendWidth + 20);
	}

	CPenSelector ps(0, 1, pDC);

	COLORREF brushColor = pDC->m_bMono ? RGB(255,255,255) : ::GetSysColor(COLOR_WINDOW);

	CBrushSelector bs(brushColor, pDC);
	
	pDC->Rectangle(legendRect);

	legendRect.DeflateRect (1,1,1,1);
	
	int nItem = 0;

	m_SelectByMarker.clear();

	for (int i = 0; i < m_Data.size(); i++)
	{
		SelectByMarker sbm;

		if (!m_Data[i].m_bVisible)
			continue;

		CString cLegend;
		cLegend.Format("%02d %s", i + 1, m_Data[i].m_cLabel);
		CRect itemRect(legendRect.left + 12, legendRect.top + nItemHeight * nItem, legendRect.right, legendRect.top + nItemHeight * nItem + nItemHeight);

		sbm.markerRect = itemRect;
		sbm.markerRect.left -= 12;
		sbm.pObject = &m_Data[i];

		m_SelectByMarker.push_back(sbm);
				
		if (pDC->m_bMono)
			pDC->SetBkColor(RGB(255,255,255));
		else
			pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));

		pDC->DrawText(cLegend, itemRect, DT_LEFT);

		if (m_Data[i].m_bSelected)
			pDC->DrawFocusRect (sbm.markerRect);

		itemRect.left = legendRect.left + 2;
		itemRect.right = legendRect.left + 10;
		itemRect.DeflateRect (0,1,0,2);

		if (m_Data[i].m_nMarkerType == 0)
			pDC->FillSolidRect (itemRect, pDC->m_bMono ? 0L : m_Data[i].m_crColor);
		else
			m_YAxis[0].DrawMarker (pDC, CPoint(itemRect.left - 1, itemRect.top + 1), m_Data[i].m_nMarker, 8, pDC->m_bMono ? 0L : m_Data[i].m_crColor, true);
	
		nItem++;
	}
}

void CXGraph::OnPaint() 
{
	CDCEx*    pDC;
	CMemDC*   pmdc  = NULL;
	CPaintDC* pdc   = new CPaintDC(this); 

	m_oldCursorPoint = CPoint(-1, -1);

	if (m_pPrintDC != NULL)
	{
		pDC = (CDCEx*) m_pPrintDC;
		pDC->m_bPrinting = true;
	}
	else
	if (m_bDoubleBuffer)
	{
		pmdc = new CMemDC(pdc);
		pDC = (CDCEx*) pmdc;
		pDC->m_bMono = false;
		pDC->m_bPrinting = false;
		pDC->m_bPrintPreview = false;
	}
	else
	{
		pDC = (CDCEx*) pdc;
		pDC->m_bMono = false;
		pDC->m_bPrinting = false;
		pDC->m_bPrintPreview = false;
	}

	CRect clRect, clChartRect;

	// Background
	if (pDC->m_bPrinting)
		clRect = m_clPrintRect;
	else
		GetClientRect(clRect);

	if (pDC->m_bMono)
		pDC->FillSolidRect(clRect, RGB(255,255,255));
	else
		pDC->FillSolidRect(clRect, m_crBackColor);
	
	// Chart
	clChartRect.left   = clRect.left + m_nLeftMargin;
	clChartRect.top    = clRect.top + m_nTopMargin;
	clChartRect.right  = clRect.right - m_nRightMargin;
	clChartRect.bottom = clRect.bottom - m_nBottomMargin;

	if (m_bShowLegend)
		DrawLegend(pDC, clChartRect);

	if (pDC->m_bMono)
		pDC->FillSolidRect(clChartRect, RGB(255,255,255));
	else
		pDC->FillSolidRect(clChartRect, m_crGraphColor);

	int nLeft   = clChartRect.left;
	int nRight  = clChartRect.right;
	int nBottom = clChartRect.bottom;

	CXGraphAxis::EAxisPlacement lastPlacement = CXGraphAxis::apAutomatic;

	// Calculate layout, prepare automatic axis
	for (int nYAxis = 0; nYAxis < m_YAxis.size (); nYAxis++)
	{
		if (m_YAxis[nYAxis].m_Placement == CXGraphAxis::apAutomatic)
		{
			if (lastPlacement == CXGraphAxis::apLeft)
			{
				lastPlacement = CXGraphAxis::apRight;
				m_YAxis[nYAxis].m_Placement = CXGraphAxis::apRight;
			}
			else
			{
				lastPlacement = CXGraphAxis::apLeft;
				m_YAxis[nYAxis].m_Placement = CXGraphAxis::apLeft;
			}
		}
	}

	// Y-Axis, left side
	for (nYAxis = 0; nYAxis < m_YAxis.size (); nYAxis++)
	{
		if (m_YAxis[nYAxis].m_Placement != CXGraphAxis::apLeft)
			continue;
	
		int nTickWidth  = m_YAxis[nYAxis].GetMaxLabelWidth (pDC); 
		int nLabelWidth = (m_YAxis[nYAxis].GetTitleSize (pDC).cx + m_YAxis[nYAxis].m_nArcSize);

		if (m_YAxis[nYAxis].m_bVisible)
			nLeft += (max (nTickWidth, nLabelWidth) + m_nAxisSpace );

		m_YAxis[nYAxis].m_nLeft   = nLeft;
		m_YAxis[nYAxis].m_clChart = clChartRect;
	}
	
	// Y-Axis, right side
	for (nYAxis = 0; nYAxis < m_YAxis.size (); nYAxis++)
	{
		if (m_YAxis[nYAxis].m_Placement != CXGraphAxis::apRight)
			continue;

		int nTickWidth  = m_YAxis[nYAxis].GetMaxLabelWidth (pDC); 
		int nLabelWidth = m_YAxis[nYAxis].GetTitleSize (pDC).cx;
	
		if (m_YAxis[nYAxis].m_bVisible)
			nRight -= (max (nTickWidth, nLabelWidth) + m_nAxisSpace );
		
		m_YAxis[nYAxis].m_nLeft   = nRight;
		m_YAxis[nYAxis].m_clChart = clChartRect;
	}

	// X-Axis
	for (int nXAxis = 0; nXAxis < m_XAxis.size (); nXAxis++)
	{
		if (m_XAxis[nXAxis].m_bVisible)
			nBottom -= (m_XAxis[nXAxis].GetMaxLabelHeight (pDC) + m_nAxisSpace);

		m_XAxis[nXAxis].m_nTop    = nBottom;
		m_XAxis[nXAxis].m_nLeft   = nLeft;
		m_XAxis[nXAxis].m_clChart = clChartRect;
	}

	// Draw Inner Rect
	// 

	m_clInnerRect.SetRect (nLeft + 1, clChartRect.top, nRight, nBottom);
	if (pDC->m_bMono)
		pDC->FillSolidRect(m_clInnerRect, RGB(255,255,255));
	else
		pDC->FillSolidRect(m_clInnerRect, m_crInnerColor);
	
	// Draw axis
	// Y-Axis
	for (nYAxis = 0; nYAxis < m_YAxis.size (); nYAxis++)
	{
		m_YAxis[nYAxis].m_nTop = nBottom;
		m_YAxis[nYAxis].Draw(pDC);
	}
	
	// X-Axis
	for (nXAxis = 0; nXAxis < m_XAxis.size (); nXAxis++)
		m_XAxis[nXAxis].Draw(pDC);
	
	// Setup clipping
	CRgn  clipRegion;
	CRect rectClip(m_clInnerRect.left, m_clInnerRect.top, m_clInnerRect.right, m_clInnerRect.bottom);
	
	// Should move inside CDCEx in future
	if (pDC->m_bPrinting && !pDC->m_bPrintPreview)
		pDC->AdjustRatio (rectClip);

	
	clipRegion.CreateRectRgn (rectClip.left, rectClip.top, rectClip.right, rectClip.bottom);


	// PRB: Clipping doesn't work in print preview, no idea why ...
	pDC->SelectClipRgn (&clipRegion);
		

	// Draw curves
	for (int nCurve = 0; nCurve < m_Data.size (); nCurve++)
		if (m_Data[nCurve].m_bVisible)
			m_Data[nCurve].Draw(pDC);

	// Draw curve markers
	for (nCurve = 0; nCurve < m_Data.size (); nCurve++)
		if (m_Data[nCurve].m_bVisible && m_Data[nCurve].m_bShowMarker)
			m_Data[nCurve].DrawMarker(pDC);

	// Draw zoom if active
	if (m_opOperation == opZoom && m_bLButtonDown)
		DrawZoom(pDC);

	// Draw cursor if active
	if (m_opOperation == opCursor)
		DrawCursor(pDC);

	// DrawObjects
	for (POSITION pos = m_Objects.GetHeadPosition (); pos != NULL; )
	{
		CXGraphObject *pObject = (CXGraphObject*) m_Objects.GetNext (pos);
		pObject->Draw (pDC);
	}

	if (pmdc)
		delete pmdc;
	if (pdc)
		delete pdc;	
}

void CXGraph::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	Invalidate(TRUE);
}

void CXGraph::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_CurrentPoint = point;
	m_bLButtonDown = (nFlags & MK_LBUTTON);

	if (m_opOperation == opCursor)
	{
		CDCEx dc;
		dc.Attach(GetDC()->m_hDC);
		DrawCursor(&dc);
		ReleaseDC(&dc);
		dc.Detach ();
		::PostMessage(GetParent()->m_hWnd, XG_CURSORMOVED, 0, (long) this);
	}	

	m_bObjectSelected = CheckObjectSelection(false, true);

	if (m_bLButtonDown && m_opOperation == opPan && m_clInnerRect.PtInRect (point))
	{
		if (m_OldPoint == CPoint(0,0))
			m_OldPoint = point;
		else
			DoPan(point);
	}

	if (m_bRButtonDown && m_opOperation == opCursor && m_clInnerRect.PtInRect (point))
	{
		if (m_OldPoint == CPoint(0,0))
			m_OldPoint = point;
		else
			DoPan(point);
	}

	if (m_bLButtonDown && m_opOperation == opZoom /*&& m_clInnerRect.PtInRect (point)*/)
	{
		if (m_bDoubleBuffer)
			Invalidate();
		else
		{
			CDCEx dc;
			dc.Attach(GetDC()->m_hDC);
			DrawZoom(&dc);
			ReleaseDC(&dc);
			dc.Detach ();
		}
	}
	
	CWnd::OnMouseMove(nFlags, point);
}


bool CXGraph::CheckObjectSelection(bool bEditAction, bool bCheckFocusOnly)
{
	int i;

	bool bHasNotified = false;

	if (!m_bInteraction)
		return false;

	// Check y axis selection
	for (i = 0; i < m_YAxis.size(); i++)
	{
		if (bCheckFocusOnly)
			if (m_YAxis[i].m_clRect.PtInRect (m_CurrentPoint))
				return true;
			else
				continue;
			
		m_YAxis[i].m_bSelected = m_YAxis[i].m_clRect.PtInRect (m_CurrentPoint);

		if (m_YAxis[i].m_bSelected)
		{
			bHasNotified = true;
			::PostMessage(GetParent()->m_hWnd, bEditAction ? XG_YAXISDBLCLICK :XG_YAXISCLICK, i, (long) this);
			if (bEditAction && m_bInteraction)
			{
				CChartPage dlg;
				dlg.m_pGraph = this;
				dlg.m_pGraphAxis = &m_YAxis[i];
				dlg.DoModal();		
				Invalidate();
			}
		}
	}
			 
	// Check x axis selection
	for (i = 0; i < m_XAxis.size(); i++)
	{
		if (bCheckFocusOnly)
			if (m_XAxis[i].m_clRect.PtInRect (m_CurrentPoint))
				return true;
			else
				continue;
		
		m_XAxis[i].m_bSelected = m_XAxis[i].m_clRect.PtInRect (m_CurrentPoint);
		if (m_XAxis[i].m_bSelected && !bHasNotified)
		{
			bHasNotified = true;
			::PostMessage(GetParent()->m_hWnd, bEditAction ? XG_XAXISDBLCLICK : XG_XAXISCLICK, i, (long) this);
			if (bEditAction && m_bInteraction)
			{
				CChartPage dlg;
				dlg.m_pGraph = this;
				dlg.m_pGraphAxis = &m_XAxis[i];
				dlg.DoModal();		
				Invalidate();
			}
		}
	}

	bool bCheckCurves = true;

	// Check object selection
	for (POSITION pos = m_Objects.GetHeadPosition (); pos != NULL; )
	{
		CXGraphObject *pObject = (CXGraphObject*) m_Objects.GetNext (pos);

		if (bCheckFocusOnly)
			if (pObject->m_clRect.PtInRect (m_CurrentPoint) && pObject->m_bVisible)
				return true;
			else
				continue;
		
		pObject->m_bSelected = (pObject->m_clRect.PtInRect (m_CurrentPoint) && pObject->m_bVisible);
		
		if (bEditAction && pObject->m_bSelected )
		{
			bHasNotified = true;
			bCheckCurves = false;
			pObject->Edit ();
		}
		
		if (pObject->m_bSelected && !pObject->m_bEditing && m_pTracker == NULL)
		{
			bHasNotified = true;
			pObject->BeginSize ();
			m_pTracker = &pObject->m_Tracker; 
			bCheckCurves = false;
		}

		if (!pObject->m_bSelected && pObject->m_bEditing)
			pObject->EndEdit ();

		if (!pObject->m_bSelected && pObject->m_bSizing)
		{
			m_pTracker = NULL;
			pObject->EndSize ();
		}
		
	}

	if (!bCheckFocusOnly)
		m_nSelectedSerie = -1;
	
	// check curve selection
	for (i = 0; i < m_Data.size(); i++)
	{
		CXGraphDataSerie& serie = m_Data[i];
		
		if (!serie.m_bVisible)
			continue;

		if (!bCheckFocusOnly)
			serie.m_bSelected = false;

		if (!bCheckCurves)
			continue;

		for (int j = 0; j < serie.m_CurveRegions.size(); j++)
		{
			CRect hitRect(serie.m_CurveRegions[j].p1.x, serie.m_CurveRegions[j].p1.y,serie.m_CurveRegions[j].p2.x, serie.m_CurveRegions[j].p2.y);
			hitRect.NormalizeRect ();
			hitRect.InflateRect (2,2,2,2);
			
			if (bCheckFocusOnly)
				if (hitRect.PtInRect (m_CurrentPoint) && serie.HitTestLine(serie.m_CurveRegions[j].p1, serie.m_CurveRegions[j].p2, m_CurrentPoint, 4))
					return true;
				else
					continue;
			
			if (hitRect.PtInRect (m_CurrentPoint) && serie.HitTestLine(serie.m_CurveRegions[j].p1, serie.m_CurveRegions[j].p2,m_CurrentPoint, 4) && !bHasNotified)
			{
				::PostMessage(GetParent()->m_hWnd, bEditAction ? XG_CURVEDBLCLICK : XG_CURVECLICK, i, (long) this);
				m_nSelectedSerie = i;
				serie.m_bSelected = true;
				bHasNotified = true;
				if (bEditAction && m_bInteraction)
				{
					CChartPage dlg;
					dlg.m_pGraph = this;
					dlg.m_pGraphDataSerie = &serie;
					dlg.DoModal();			
					Invalidate();
				}
				break;
			}
		}
	}

	if (!bHasNotified)
	for (i = 0; i < m_SelectByMarker.size(); i++)
	{
		bool bSelected = m_SelectByMarker[i].markerRect.PtInRect (m_CurrentPoint);
		
		if (bCheckFocusOnly && bSelected)
			return true;
		
		m_SelectByMarker[i].pObject->m_bSelected = bSelected;

		if (bSelected)
		{
			bHasNotified = true;
			if (bEditAction && m_bInteraction)
			{
				CXGraphDataSerie& serie = m_Data[i];
				m_nSelectedSerie = i;
				CChartPage dlg;
				dlg.m_pGraph = this;
				dlg.m_pGraphDataSerie = &serie;
				dlg.DoModal();			
				Invalidate();
			}
			break;
		}
	}
	
	if (!bHasNotified && bEditAction && m_bInteraction)
	{
		::PostMessage(GetParent()->m_hWnd, XG_GRAPHDBLCLICK, 0, (long) this);
		CChartPage dlg;
		dlg.m_pGraph = this;
		dlg.DoModal();
		Invalidate();
	}

		
	return false;
}

void CXGraph::InsertDataNotation(int nCurve, int nIndex)
{
	CString cText, cXFmt, cYFmt, cFmt;

	bool bXDT = m_XAxis[m_Data[nCurve].m_nXAxis].m_bDateTime;
	bool bYDT = m_YAxis[m_Data[nCurve].m_nYAxis].m_bDateTime;

	cYFmt = cXFmt = "%s";
	cYFmt = "%s";
	cFmt  = " %d " + cXFmt + "%s" ;//+ cYFmt + "%s ";

	if (bXDT)
		cXFmt = COleDateTime(m_Data[nCurve].m_pData[nIndex].fXVal).Format(m_XAxis[m_Data[nCurve].m_nXAxis].m_cDisplayFmt);
	else
		cXFmt.Format(m_XAxis[m_Data[nCurve].m_nXAxis].m_cDisplayFmt, m_Data[nCurve].m_pData[nIndex].fXVal);

	if (bYDT)
		cYFmt = COleDateTime(m_Data[nCurve].m_pData[nIndex].fYVal).Format(m_YAxis[m_Data[nCurve].m_nYAxis].m_cDisplayFmt);
	else
		cYFmt.Format(m_YAxis[m_Data[nCurve].m_nYAxis].m_cDisplayFmt, m_Data[nCurve].m_pData[nIndex].fYVal);

//	cText.Format(cFmt, nIndex + 1,  m_XAxis[m_Data[nCurve].m_nXAxis].m_cLabel);//, cYFmt, m_YAxis[m_Data[nCurve].m_nYAxis].m_cLabel);
	cXFmt=" "+cXFmt+" ";
	cText.Format(cXFmt,m_XAxis[m_Data[nCurve].m_nXAxis].m_cLabel);
	CXGraphDataNotation *pNotation = (CXGraphDataNotation*) new CXGraphDataNotation;
	
	pNotation->m_fXVal  = m_Data[nCurve].m_pData[nIndex].fXVal;
	pNotation->m_fYVal  = m_Data[nCurve].m_pData[nIndex].fYVal;
	pNotation->m_cText  = cText;
	pNotation->m_nCurve = nCurve;
	pNotation->m_nIndex = nIndex;
	pNotation->m_pGraph = this;

	m_Objects.AddTail (pNotation);

	Invalidate();
}

void CXGraph::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (!m_bLButtonDown)
		return;

	ReleaseCapture();

	m_OldPoint     = CPoint(0,0);
	m_MouseUpPoint = point;
	m_bLButtonDown = false;

	if (m_opOperation == opCursor)
	{
		if (m_nSnappedCurve != -1)// !CheckObjectSelection() &&
		{
			long nIndex;
			m_XAxis[m_Data[m_nSnappedCurve].m_nXAxis].GetIndexByXVal(nIndex, m_fSnappedXVal, m_nSnappedCurve);
			InsertDataNotation(m_nSnappedCurve, nIndex);
		}
	}
	
	if (m_opOperation == opZoom)
		DoZoom();
	
//	if (m_opOperation == opNone)
//		CheckObjectSelection();

	Invalidate();

	CWnd::OnLButtonUp(nFlags, point);
}

void CXGraph::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();
	SetCapture();

	m_MouseDownPoint = point;
	m_bLButtonDown   = true;
	m_pCurrentObject = NULL;

	// Check objects
	for (POSITION pos = m_Objects.GetHeadPosition (); pos != NULL; )
	{
		CXGraphObject *pObject = (CXGraphObject*) m_Objects.GetNext (pos);

		if (pObject->m_clRect.PtInRect(point))
			m_pCurrentObject = pObject;
				
		if (pObject->m_bSizing && pObject->m_Tracker.HitTest(point) != CRectTracker::hitNothing)
		{
			ReleaseCapture();
			
			if (pObject->m_Tracker.Track(this, point) && pObject->m_bCanMove && pObject->m_bCanResize)
			{
				pObject->m_clRect = pObject->m_Tracker.m_rect;
				Invalidate(FALSE);
			}
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}

BOOL CXGraph::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CXGraph::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
/*	if (m_opOperation == opNone)
	{
		m_pTracker = NULL;
		CheckObjectSelection(true);
	}
	*/
	CWnd::OnLButtonDblClk(nFlags, point);
}

BOOL CXGraph::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{	
	if (m_pTracker && m_pTracker->SetCursor(this,  nHitTest))
        return TRUE;

	if (m_opOperation == opPan)
	{
		HCURSOR hCur = AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);
		if (hCur)
		{
			::SetCursor(hCur);			 
			return TRUE;
		}
	}
	else
	if (m_opOperation == opZoom || m_opOperation == opCursor)
	{
		HCURSOR hCur = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		if (hCur)
		{
			::SetCursor(hCur);			 
			return TRUE;
		}
	}
	else
	if (m_bObjectSelected && m_opOperation == opNone)
	{
		HCURSOR hCur = AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(32649));
		if (hCur)
		{
			::SetCursor(hCur);			 
			return TRUE;
		}
	}
	
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CXGraph::PreTranslateMessage(MSG* pMsg) 
{
	if ((pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP) && pMsg->wParam == VK_DELETE)
	{
		if (m_pCurrentObject && m_pCurrentObject->m_bSizing)
		{
			if (m_Objects.GetHead() != m_pCurrentObject)
			{
				m_Objects.RemoveAt(m_Objects.Find(m_pCurrentObject));
				delete m_pCurrentObject;
				m_pCurrentObject = NULL;
				m_pTracker = NULL;
				Invalidate();
			}
		}
	}
		
	return CWnd::PreTranslateMessage(pMsg);
}

void CXGraph::OnRButtonUp(UINT nFlags, CPoint point) 
{
	m_bRButtonDown = false;

	if (m_opOperation == opCursor)
	{
		m_oldCursorPoint = CPoint(-1,-1);
		Invalidate();
	}

	if (m_pCurrentObject && m_pCurrentObject->m_clRect.PtInRect (point))
		m_pCurrentObject->InvokeProperties();
	else
		::PostMessage(GetParent()->m_hWnd, XG_RBUTTONUP, 0, (long) this);
		

	CWnd::OnRButtonUp(nFlags, point);
}

void CXGraph::InsertEmptyLabel()
{
	InsertLabel();
	Invalidate();
}

void CXGraph::OnProperties()
{
//	CheckObjectSelection(true);
}

void CXGraph::OnPrint()
{
	AFX_MANAGE_STATE(AfxGetModuleState());

	CPrintDialog dlg(FALSE);

	if (dlg.DoModal () == IDOK)
	{
		CDCEx dc;
		
		dc.Attach (dlg.GetPrinterDC ());

		dc.Prepare (dc.m_hAttribDC);

		int nWidth  = dc.GetDeviceCaps (HORZRES);
		int nHeight = dc.GetDeviceCaps (VERTRES);

		m_clPrintRect.SetRect(0, 0, nWidth / dc.m_fScaleX, nHeight / dc.m_fScaleY);

		m_pPrintDC = &dc;
		m_pPrintDC->m_bPrinting = true;
		m_pPrintDC->StartDoc ("Graph"); 
		m_pPrintDC->StartPage ();
		OnPaint();
		m_pPrintDC->EndPage ();
		m_pPrintDC->EndDoc ();
		m_pPrintDC = NULL;
		dc.Detach ();
	}
	
	//Invalidate();
}

void CXGraph::PrintGraph(CDC *pDC)
{ 
	CDCEx dc;
	
	dc.Attach (pDC->m_hDC);

	dc.Prepare (pDC->m_hAttribDC);
	
	int nWidth, nHeight;

	if (pDC->m_hDC == pDC->m_hAttribDC)
	{
		m_pPrintDC = &dc;
		nWidth  = pDC->GetDeviceCaps (HORZRES);
		nHeight = pDC->GetDeviceCaps (VERTRES);
		dc.m_bPrintPreview = false;
		dc.m_bPrinting = true;
	    DOCINFO docinfo;
	    memset(&docinfo, 0, sizeof(docinfo));
	    docinfo.cbSize = sizeof(docinfo);
        docinfo.lpszDocName = _T("Graph");
		int nRet;//= dc.StartDoc(&docinfo);
		nRet = GetLastError();
		nRet = dc.StartPage();
		m_clPrintRect.SetRect(0, 0, nWidth / dc.m_fScaleX, nHeight / dc.m_fScaleY);
		OnPaint();
		dc.EndPage ();
		dc.EndDoc ();
		m_pPrintDC = NULL;

	}
	else
	{
		nWidth  = ::GetDeviceCaps (pDC->m_hAttribDC ,HORZRES);
		nHeight = ::GetDeviceCaps (pDC->m_hAttribDC ,VERTRES);
		dc.m_bPrintPreview = true;
		m_clPrintRect.SetRect(0, 0, nWidth / dc.m_fScaleX, nHeight / dc.m_fScaleY);
		m_pPrintDC = &dc;
		m_pPrintDC->m_bPrinting = true;
		OnPaint();
		m_pPrintDC = NULL;
	}
}

void CXGraph::PrintGraph(CDC *pDC, CRect printRect)
{ 
	CDCEx dc;
	
	dc.Attach (pDC->m_hDC);

	dc.Prepare (pDC->m_hAttribDC);

	int nWidth, nHeight;

	nWidth  = pDC->GetDeviceCaps (HORZRES);
	nHeight = pDC->GetDeviceCaps (VERTRES);
		
	if (pDC->m_hDC == pDC->m_hAttribDC)
	{
		m_pPrintDC = &dc;
		dc.m_bPrintPreview = false;
		dc.m_bPrinting = true;
	    DOCINFO docinfo;
	    memset(&docinfo, 0, sizeof(docinfo));
	    docinfo.cbSize = sizeof(docinfo);
        docinfo.lpszDocName = _T("Graph");
		int nRet;//= dc.StartDoc(&docinfo);
		nRet = GetLastError();
		nRet = dc.StartPage();
		m_clPrintRect.SetRect(printRect.left / dc.m_fScaleX, printRect.top / dc.m_fScaleX, printRect.right / dc.m_fScaleX, printRect.bottom / dc.m_fScaleX);
		OnPaint();
		dc.EndPage ();
		dc.EndDoc ();
		m_pPrintDC = NULL;

	}
	else
	{
		dc.m_bPrintPreview = true;
		m_clPrintRect.SetRect(printRect.left / dc.m_fScaleX, printRect.top / dc.m_fScaleX, printRect.right / dc.m_fScaleX, printRect.bottom / dc.m_fScaleX);
		m_pPrintDC = &dc;
		m_pPrintDC->m_bPrinting = true;
		OnPaint();
		m_pPrintDC = NULL;
	}

}

void CXGraph::CubicTrend()
{
	AddCubicTrend(m_Data[m_nSelectedSerie]);
}

void CXGraph::LinearTrend()
{
	AddLinearTrend(m_Data[m_nSelectedSerie]);
}

void CXGraph::AddLinearTrend(CXGraphDataSerie& serie)
{
	TDataPoint* pTrend = serie.GetLinearTrend ();
	int			nCurveCount = GetCurveCount ();
	CString cLabel = serie.m_cLabel;
	CXGraphDataSerie &ret = SetData (pTrend, serie.m_nCount, nCurveCount, serie.m_nXAxis, serie.m_nYAxis, true);
	ret.m_cLabel = cLabel + " linear trend";
	delete pTrend;
}

void CXGraph::AddCubicTrend(CXGraphDataSerie& serie)
{
	TDataPoint* pTrend = serie.GetCubicTrend ();
	int			nCurveCount = GetCurveCount ();
	CString cLabel = serie.m_cLabel;
	CXGraphDataSerie &ret = SetData (pTrend, serie.m_nCount, nCurveCount, serie.m_nXAxis, serie.m_nYAxis, true);
	ret.m_cLabel = cLabel + " cubic trend";
	delete pTrend;
}

void CXGraph::DrawCursor(CDCEx* pDC)
{
//	CPenSelector ps(0xffff00, 1, pDC);
	long		 nIndex;
	TDataPoint   point;
//	int			 nOldROP2 = pDC->SetROP2 (R2_XORPEN);//R2_NOTXORPEN);

	m_nSnappedCurve = -1;

	if (m_bSnapCursor)
	// Snap cursor to nearest curve
	for (int i = 0; i < m_Data.size(); i++)
	{
		// Get xval for current position
		m_fSnappedXVal = m_XAxis[m_Data[i].m_nXAxis].GetValueForPos (m_CurrentPoint.x);
		// Find index for this value
		m_XAxis[m_Data[i].m_nXAxis].GetIndexByXVal(nIndex, m_fSnappedXVal, i);
		// get yval for index
		m_fSnappedYVal = m_Data[i].m_pData[nIndex].fYVal;
		point.fYVal = m_fSnappedYVal;
		// Check if cursor is in snap-range
		int y = m_YAxis[m_Data[i].m_nYAxis].GetPointForValue(&point).y;
		if (abs(m_CurrentPoint.y - y) < m_nSnapRange)
		{
			m_nSnappedCurve = i;
			m_CurrentPoint.y = y;
			break;
		}
	}
/*	if(!m_clInnerRect.PtInRect (m_CurrentPoint))
	{
		pDC->MoveTo (m_oldCursorPoint.x, m_clInnerRect.top);
		pDC->LineTo (m_oldCursorPoint.x, m_clInnerRect.bottom);
		pDC->MoveTo (m_clInnerRect.left, m_oldCursorPoint.y);
		pDC->LineTo (m_clInnerRect.right, m_oldCursorPoint.y);
	}
	else
	{
		if (m_oldCursorPoint != CPoint(-1,-1))
		{
			pDC->MoveTo (m_oldCursorPoint.x, m_clInnerRect.top);
			pDC->LineTo (m_oldCursorPoint.x, m_clInnerRect.bottom);
			pDC->MoveTo (m_clInnerRect.left, m_oldCursorPoint.y);
			pDC->LineTo (m_clInnerRect.right, m_oldCursorPoint.y);
		}
		pDC->MoveTo (m_CurrentPoint.x, m_clInnerRect.top);
		pDC->LineTo (m_CurrentPoint.x, m_clInnerRect.bottom);
		pDC->MoveTo (m_clInnerRect.left, m_CurrentPoint.y);
		pDC->LineTo (m_clInnerRect.right, m_CurrentPoint.y);
		m_oldCursorPoint = m_CurrentPoint;
	}
	pDC->SetROP2 (nOldROP2);*/
	DrawCursorLegend(pDC);
}

void CXGraph::DrawCursorLegend (CDCEx* pDC)
{
	int    i;
    double fVal;

    CString cLabel = "", cFmt, cItem;

    m_CursorLabel.m_bVisible = true;
    
    for (i = 0; i < m_XAxis.size(); i++)
    {
        //CHANGED: bugfix, correct handling of X axis in time mode
        fVal = m_XAxis[i].GetValueForPos (m_CurrentPoint.x);
        if(m_XAxis[i].m_bDateTime)
        {
			cFmt = m_XAxis[i].m_cDisplayFmt;
			cFmt.Replace ("\r"," ");
			cFmt.Replace ("\n"," ");
            cFmt =  "X : " +COleDateTime(fVal).Format(cFmt) + " %s";
            cItem.Format(cFmt,  m_XAxis[i].m_cLabel);
            cLabel += (" " + cItem + "\r\n");
        }
        else
        {
            cFmt = /*"X: " +*/m_XAxis[i].m_cDisplayFmt + " %s"; 
            cItem.Format(cFmt, fVal, m_XAxis[i].m_cLabel);
            cLabel += (" " + cItem+ "\r\n");
        }
        //ENDCHANGES
    }
    
    for (i = 0; i < m_YAxis.size(); i++)
    {
        fVal = m_YAxis[i].GetValueForPos (m_CurrentPoint.y);
        cFmt = /*"Y: " + */m_YAxis[i].m_cDisplayFmt + " %s";
        cItem.Format(cFmt,  fVal, m_YAxis[i].m_cLabel);
        cLabel += (" " + cItem + "\r\n");
    }
/*
    if (m_nSnappedCurve != -1)
    {
        //CHANGED: bugfix, correct handling of X axis in time mode
        CString cXFmt = m_XAxis[m_Data[m_nSnappedCurve].m_nXAxis].m_cDisplayFmt;
		cXFmt.Replace ("\r"," ");
		cXFmt.Replace ("\n"," ");
        CString cYFmt = m_YAxis[m_Data[m_nSnappedCurve].m_nYAxis].m_cDisplayFmt;
        if(m_XAxis[m_Data[m_nSnappedCurve].m_nXAxis].m_bDateTime)
        {			
            cFmt = "%s[%d]: " + COleDateTime(m_fSnappedXVal).Format(cXFmt) + ", " + cYFmt;
            cItem.Format(cFmt, m_Data[m_nSnappedCurve].m_cLabel, m_nSnappedCurve + 1, m_fSnappedYVal);
            cLabel += (" " + cItem + "\r\n");
        }
        else
        {
            cFmt = "%s[%d]: " + cXFmt + ", " + cYFmt;
            cItem.Format(cFmt, m_Data[m_nSnappedCurve].m_cLabel, m_nSnappedCurve + 1, m_fSnappedXVal, m_fSnappedYVal);
            cLabel += (" " + cItem + "\r\n");
        }
        //ENDCHANGES
    }
*/
    m_CursorLabel.m_cText = cLabel;

    m_CursorLabel.Draw (pDC);
}

LRESULT CXGraph::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	return CWnd::WindowProc(message, wParam, lParam);
}

bool CXGraph::SelectCurve(int nCurve)
{
	if (nCurve < 0 || nCurve >= m_Data.size())
		return false;

	for (int i = 0; i < m_Data.size(); i++)
		m_Data[i].m_bSelected = (i == nCurve);

	Invalidate();

	return true;
}

bool CXGraph::SelectXAxis(int nAxis)
{
	if (nAxis < 0 || nAxis >= m_XAxis.size())
		return false;

	for (int i = 0; i < m_XAxis.size(); i++)
		m_XAxis[i].m_bSelected = (i == nAxis);

	Invalidate();

	return true;
}

bool CXGraph::SelectYAxis(int nAxis)
{
	if (nAxis < 0 || nAxis >= m_YAxis.size())
		return false;

	for (int i = 0; i < m_YAxis.size(); i++)
		m_YAxis[i].m_bSelected = (i == nAxis);

	Invalidate();
	
	return true;
}

void CXGraph::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_HOME)
	{
		for (int i = 0; i < m_XAxis.size(); i++)
			m_XAxis[i].Reset ();
		
		for (i = 0; i < m_YAxis.size(); i++)
			m_YAxis[i].Reset();
		
		Invalidate();
	}

	if (nChar == VK_ADD)
	{
		for (int i = 0; i < m_XAxis.size(); i++)
		{
			double fStep = m_XAxis[i].m_fRange / 10.0;
			VERIFY(m_XAxis[i].SetCurrentRange (m_XAxis[i].m_fCurMin + fStep, m_XAxis[i].m_fCurMax - fStep));
		}
		for (i = 0; i < m_YAxis.size(); i++)
		{
			double fStep = m_YAxis[i].m_fRange / 10.0;
			VERIFY(m_YAxis[i].SetCurrentRange (m_YAxis[i].m_fCurMin + fStep, m_YAxis[i].m_fCurMax - fStep));
		}
		
		Invalidate();
	}

	if (nChar == VK_SUBTRACT)
	{
		for (int i = 0; i < m_XAxis.size(); i++)
		{
			double fStep = m_XAxis[i].m_fRange / 10.0;
			VERIFY(m_XAxis[i].SetCurrentRange (m_XAxis[i].m_fCurMin - fStep, m_XAxis[i].m_fCurMax + fStep));
		}
		for (i = 0; i < m_YAxis.size(); i++)
		{
			double fStep = m_YAxis[i].m_fRange / 10.0;
			VERIFY(m_YAxis[i].SetCurrentRange (m_YAxis[i].m_fCurMin - fStep, m_YAxis[i].m_fCurMax + fStep));
		}
		
		Invalidate();
	}

	if (nChar == VK_LEFT)
	{
		for (int i = 0; i < m_XAxis.size(); i++)
		{
			double fStep = m_XAxis[i].m_fRange / 10.0;
			VERIFY(m_XAxis[i].SetCurrentRange (m_XAxis[i].m_fCurMin + fStep, m_XAxis[i].m_fCurMax + fStep));
		}

		Invalidate();
	}
	
	if (nChar == VK_RIGHT)
	{
		for (int i = 0; i < m_XAxis.size(); i++)
		{
			double fStep = m_XAxis[i].m_fRange / 10.0;
			VERIFY(m_XAxis[i].SetCurrentRange (m_XAxis[i].m_fCurMin - fStep, m_XAxis[i].m_fCurMax - fStep));
		}

		Invalidate();
	}

	if (nChar == VK_UP)
	{
		for (int i = 0; i < m_YAxis.size(); i++)
		{
			double fStep = m_YAxis[i].m_fRange / 10.0;
			VERIFY(m_YAxis[i].SetCurrentRange (m_YAxis[i].m_fCurMin - fStep, m_YAxis[i].m_fCurMax - fStep));
		}

		Invalidate();
	}

	if (nChar == VK_DOWN)
	{
		for (int i = 0; i < m_YAxis.size(); i++)
		{
			double fStep = m_YAxis[i].m_fRange / 10.0;
			VERIFY(m_YAxis[i].SetCurrentRange (m_YAxis[i].m_fCurMin + fStep, m_YAxis[i].m_fCurMax + fStep));
		}

		Invalidate();
	}
	
	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CXGraph::ResetAll()
{
	m_XAxis.clear ();
	m_YAxis.clear ();
	m_Data.clear ();

	// Delete all objects
	// Spare 1st object (CursorLabel)
	while (m_Objects.GetCount () > 1)
		delete m_Objects.RemoveTail ();

}

void CXGraph::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_bRButtonDown = true;
	
	CWnd::OnRButtonDown(nFlags, point);
}

void CXGraph::Serialize( CArchive& archive, UINT nFlags)
{	
	int nHelper;
	
	CWnd::Serialize (archive);

    if( archive.IsStoring() )
    {
		archive << nFlags;
		
		if (nFlags & PERSIST_PROPERTIES)
		{
			archive << m_crBackColor;
			archive << m_crGraphColor;
			archive << m_crInnerColor;
			archive << m_bDoubleBuffer;
			archive << m_bShowLegend;
			archive << (int) m_LegendAlignment;
			archive << m_bSnapCursor;
			archive << m_nSnapRange;
			archive << m_nLeftMargin;
			archive << m_nTopMargin;
			archive << m_nRightMargin;
			archive << m_nBottomMargin;
			archive << m_nAxisSpace;
			archive << m_nSelectedSerie;
		}

		if (nFlags & PERSIST_DATA)
		{
			archive << m_Data.size ();
			for (int i = 0; i < m_Data.size(); i++)
				m_Data[i].Serialize (archive);

			archive << m_XAxis.size ();
			for (i = 0; i < m_XAxis.size(); i++)
				m_XAxis[i].Serialize (archive);

			archive << m_YAxis.size ();
			for (i = 0; i < m_YAxis.size(); i++)
				m_YAxis[i].Serialize (archive);
		}

		if (nFlags & PERSIST_OBJECTS)
		{
			// Spare cursor label (1st object), created within constructor
			archive << m_Objects.GetCount () - 1;

			int i = 0;

			for (POSITION pos = m_Objects.GetHeadPosition (); pos != NULL; i++ )
			{			
				CXGraphObject *pObject = (CXGraphObject*) m_Objects.GetNext (pos);
				if ( i > 0)
					archive << pObject;
		
			}
		}
	}
	else
    {
		archive >> nFlags;

		if (nFlags & PERSIST_PROPERTIES)
		{
			archive >> m_crBackColor;
			archive >> m_crGraphColor;
			archive >> m_crInnerColor;
			archive >> m_bDoubleBuffer;
			archive >> m_bShowLegend;
			archive >> nHelper;
			m_LegendAlignment = (EAlignment) nHelper;
			archive >> m_bSnapCursor;
			archive >> m_nSnapRange;
			archive >> m_nLeftMargin;
			archive >> m_nTopMargin;
			archive >> m_nRightMargin;
			archive >> m_nBottomMargin;
			archive >> m_nAxisSpace;
			archive >> m_nSelectedSerie;
		}

		if (nFlags & PERSIST_DATA)
		{
			archive >> nHelper;
			for (int i = 0; i < nHelper; i++)
			{
				CXGraphDataSerie serie;
				serie.Serialize (archive);
				serie.m_pGraph = this;
				m_Data.push_back (serie);
			}

			archive >> nHelper;
			for (i = 0; i < nHelper; i++)
			{
				CXGraphAxis axis;
				axis.Serialize (archive);
				axis.m_pGraph = this;
				m_XAxis.push_back (axis);
			}

			archive >> nHelper;
			for (i = 0; i < nHelper; i++)
			{
				CXGraphAxis axis;
				axis.Serialize (archive);
				axis.m_pGraph = this;
				m_YAxis.push_back (axis);
			}
		}

		if (nFlags & PERSIST_OBJECTS)
		{
			archive >> nHelper;

			for (int i = 0; i < nHelper; i++)
			{			
				CXGraphObject *pObject;
				archive >> pObject;
				pObject->m_pGraph = this;
				m_Objects.AddTail (pObject);
			}
		}

		Invalidate();
	}
}

bool CXGraph::Save(const CString cFile, UINT nFlags)
{
	CFile file;

	if (nFlags == 0)
		return false;

	m_pTracker = NULL;
	m_pCurrentObject = NULL;

	if (!file.Open (cFile, CFile::modeCreate | CFile::modeWrite ))
		return false;

	try	
	{
		CArchive ar(&file, CArchive::store);
		Serialize(ar, nFlags);
	} 
	catch (CException* e)	
	{
		e->Delete ();
		return false;
	}

	return true;
}

bool CXGraph::Load(const CString cFile)
{
	CFile file;
	
	if (!file.Open (cFile, CFile::modeRead ))
		return false;

	ResetAll();

	try
	{
		CArchive ar(&file, CArchive::load);
		Serialize(ar, 0);
	} 
	catch (CException* e)
	{
		e->Delete ();
		return false;
	}

	return true;
}

void CXGraph::ParentCallback()
{
	::PostMessage(GetParent()->m_hWnd, IDM_PARENTCALLBACK, 0, (long) this);		
}

HANDLE CXGraph::DDBToDIB( CBitmap& bitmap, DWORD dwCompression, CPalette* pPal ) 
{
	BITMAP			bm;
	BITMAPINFOHEADER	bi;
	LPBITMAPINFOHEADER 	lpbi;
	DWORD			dwLen;
	HANDLE			hDIB;
	HANDLE			handle;
	HDC 			hDC;
	HPALETTE		hPal;


	ASSERT( bitmap.GetSafeHandle() );

	// The function has no arg for bitfields
	if( dwCompression == BI_BITFIELDS )
		return NULL;

	// If a palette has not been supplied use defaul palette
	hPal = (HPALETTE) pPal->GetSafeHandle();
	if (hPal==NULL)
		hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE);

	// Get bitmap information
	bitmap.GetObject(sizeof(bm),(LPSTR)&bm);

	// Initialize the bitmapinfoheader
	bi.biSize		= sizeof(BITMAPINFOHEADER);
	bi.biWidth		= bm.bmWidth;
	bi.biHeight 		= bm.bmHeight;
	bi.biPlanes 		= 1;
	bi.biBitCount		= bm.bmPlanes * bm.bmBitsPixel;
	bi.biCompression	= dwCompression;
	bi.biSizeImage		= 0;
	bi.biXPelsPerMeter	= 0;
	bi.biYPelsPerMeter	= 0;
	bi.biClrUsed		= 0;
	bi.biClrImportant	= 0;

	// Compute the size of the  infoheader and the color table
	int nColors = (1 << bi.biBitCount);
	if( nColors > 256 ) 
		nColors = 0;
	dwLen  = bi.biSize + nColors * sizeof(RGBQUAD);

	// We need a device context to get the DIB from
	hDC = ::GetDC(NULL);
	hPal = SelectPalette(hDC,hPal,FALSE);
	RealizePalette(hDC);

	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED,dwLen);

	if (!hDIB){
		SelectPalette(hDC,hPal,FALSE);
		::ReleaseDC(NULL,hDC);
		return NULL;
	}

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	*lpbi = bi;

	// Call GetDIBits with a NULL lpBits param, so the device driver 
	// will calculate the biSizeImage field 
	GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(), 0L, (DWORD)bi.biHeight,
			(LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

	bi = *lpbi;

	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if (bi.biSizeImage == 0){
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8) 
						* bi.biHeight;

		// If a compression scheme is used the result may infact be larger
		// Increase the size to account for this.
		if (dwCompression != BI_RGB)
			bi.biSizeImage = (bi.biSizeImage * 3) / 2;
	}

	// Realloc the buffer so that it can hold all the bits
	dwLen += bi.biSizeImage;
	if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE))
		hDIB = handle;
	else{
		GlobalFree(hDIB);

		// Reselect the original palette
		SelectPalette(hDC,hPal,FALSE);
		::ReleaseDC(NULL,hDC);
		return NULL;
	}

	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;

	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits( hDC, (HBITMAP)bitmap.GetSafeHandle(),
				0L,				// Start scan line
				(DWORD)bi.biHeight,		// # of scan lines
				(LPBYTE)lpbi 			// address for bitmap bits
				+ (bi.biSize + nColors * sizeof(RGBQUAD)),
				(LPBITMAPINFO)lpbi,		// address of bitmapinfo
				(DWORD)DIB_RGB_COLORS);		// Use RGB for color table

	if( !bGotBits )
	{
		GlobalFree(hDIB);
		
		SelectPalette(hDC,hPal,FALSE);
		::ReleaseDC(NULL,hDC);
		return NULL;
	}

	SelectPalette(hDC,hPal,FALSE);
	::ReleaseDC(NULL,hDC);
	return hDIB;
}

BOOL CXGraph::WriteDIB( LPTSTR szFile, HANDLE hDIB)
{
	BITMAPFILEHEADER	hdr;
	LPBITMAPINFOHEADER	lpbi;

	if (!hDIB)
		return FALSE;

	CFile file;
	if( !file.Open( szFile, CFile::modeWrite|CFile::modeCreate) )
		return FALSE;

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	int nColors = 1 << lpbi->biBitCount;

	// Fill in the fields of the file header 
	hdr.bfType		= ((WORD) ('M' << 8) | 'B');	// is always "BM"
	hdr.bfSize		= GlobalSize (hDIB) + sizeof( hdr );
	hdr.bfReserved1 	= 0;
	hdr.bfReserved2 	= 0;
	hdr.bfOffBits		= (DWORD) (sizeof( hdr ) + lpbi->biSize +
						nColors * sizeof(RGBQUAD));

	// Write the file header 
	file.Write( &hdr, sizeof(hdr) );

	// Write the DIB header and the bits 
	file.Write( lpbi, GlobalSize(hDIB) );

	return TRUE;
}

BOOL CXGraph::SaveBitmap( const CString cFile )
{
	CWnd*       pWnd = this;
	CBitmap 	bitmap;
	CWindowDC	dc(pWnd);
	CDC 		memDC;
	CRect		rect;

	
	memDC.CreateCompatibleDC(&dc); 

	pWnd->GetWindowRect(rect);

	bitmap.CreateCompatibleBitmap(&dc, rect.Width(),rect.Height() );
	
	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
	memDC.BitBlt(0, 0, rect.Width(),rect.Height(), &dc, 0, 0, SRCCOPY); 

	// Create logical palette if device support a palette
	CPalette pal;
	if( dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE )
	{
		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * 256);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
		pLP->palVersion = 0x300;

		pLP->palNumEntries = 
			GetSystemPaletteEntries( dc, 0, 255, pLP->palPalEntry );

		// Create the palette
		pal.CreatePalette( pLP );

		delete[] pLP;
	}

	memDC.SelectObject(pOldBitmap);

	// Convert the bitmap to a DIB
	HANDLE hDIB = DDBToDIB( bitmap, BI_RGB, &pal );

	if( hDIB == NULL )
		return FALSE;

	// Write it to file
	WriteDIB( const_cast <char*>( (LPCTSTR) cFile ), hDIB );

	// Free the memory allocated by DDBToDIB for the DIB
	GlobalFree( hDIB );
	return TRUE;
}

#pragma warning (default : 4244)
#pragma warning (default : 4800)

