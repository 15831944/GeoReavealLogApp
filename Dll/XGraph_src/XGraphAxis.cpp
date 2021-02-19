// XGraphAxis.cpp: Implementierung der Klasse CXGraphAxis.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XGraphAxis.h"
#include "XGraph.h"
#include "GfxUtils.h"
#include "Math.h"
#include "float.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#pragma warning (disable : 4244)
#pragma warning (disable : 4800)

double CXGraphAxis::TimeStepTable[] = {  0, 1, 2, 5, 10, 15, 20, 30, 60, 
										 2*60, 5*60, 10*60, 15*60, 30*60, 3600,
										 2*3600, 4*3600, 8*3600, 10*3600, 12*3600, 24*3600,
										 2*24*3600, 4*24*3600, 7*24*3600,
										 2*7*24*3600, 4*7*24*3600, 8*7*24*3600, 12*7*24*3600,
										 26*7*24*3600, 52*7*24*3600,
										 2*52*7*24*3600, 5*52*7*24*3600, 10*52*7*24*3600,
										 20*52*7*24*3600, 50*52*7*24*3600, -1} ;


IMPLEMENT_SERIAL( CXGraphAxis, CXGraphObject, 1 )

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CXGraphAxis::CXGraphAxis()
{
	m_Placement         = apAutomatic;
	m_bAutoScale		= true;
	m_bShowMarker       = true;
	m_bVisible			= true;
	m_bShowGrid			= true;
	m_bDateTime			= false;
	m_cLabel			= " ";
	m_cDisplayFmt		= "%g";
	m_fMax				= -DBL_MAX;
	m_fMin				= DBL_MAX;
	m_fCurMin			= DBL_MAX;
	m_fCurMax			= -DBL_MAX;
	m_fStep				= 0.1;
	m_fRange            = 1.0;
	m_nRange            = 1;
	m_pGraph			= NULL;
	m_nArcSize			= 5;
	m_nTickSize			= 5;
	m_crColor			= 0L;
	m_crGridColor		= RGB(0xC0, 0xC0, 0xC0);//RGB(150,150,150);//
	m_nGridStyle		= PS_DOT;//PS_SOLID;
	m_fSpareRange		= 0;
	m_fMarkerSpareSize  = 0;
	m_nMarkerSize       = 10;

	SetFont("Arial", 13, FW_NORMAL);
	SetTitleFont("Arial", 14, FW_BOLD);
}

CXGraphAxis::CXGraphAxis(const CXGraphAxis& copy)
{
	*this = copy;
}

CXGraphAxis& CXGraphAxis::operator = (const CXGraphAxis& copy)
{
	m_bAutoScale		= copy.m_bAutoScale;
	m_bVisible			= copy.m_bVisible;
	m_bShowGrid			= copy.m_bShowGrid;
	m_cDisplayFmt		= copy.m_cDisplayFmt;
	m_cLabel			= copy.m_cLabel;
	m_fMax				= copy.m_fMax;
	m_fMin				= copy.m_fMin;
	m_fCurMax			= copy.m_fCurMax;
	m_fCurMin			= copy.m_fCurMin;
	m_fStep				= copy.m_fStep;
	m_nTop				= copy.m_nTop;
	m_nLeft				= copy.m_nLeft;
	m_AxisKind			= copy.m_AxisKind;
	m_nArcSize			= copy.m_nArcSize;
	m_nTickSize			= copy.m_nTickSize;
	m_pGraph			= copy.m_pGraph;
	m_crColor			= copy.m_crColor;
	m_crGridColor		= copy.m_crGridColor;
	m_nGridStyle		= copy.m_nGridStyle;
	m_fSpareRange		= copy.m_fSpareRange;
	m_bDateTime			= copy.m_bDateTime;
	m_fMarkerSpareSize  = copy.m_fMarkerSpareSize;
	m_fRange            = copy.m_fRange;
	m_nRange            = copy.m_nRange;
	m_nMarkerSize       = copy.m_nMarkerSize;
	m_Placement         = copy.m_Placement;
	m_bShowMarker       = copy.m_bShowMarker;
	m_ColorRanges       = copy.m_ColorRanges;
	m_ZoomHistory       = copy.m_ZoomHistory;
			
	LOGFONT font;

	if (m_TitleFont.m_hObject != NULL)
		m_TitleFont.DeleteObject ();

	if (m_Font.m_hObject != NULL)
		m_Font.DeleteObject ();

	((CFont&)copy.m_Font).GetLogFont (&font);
	m_Font.CreateFontIndirect (&font);

	((CFont&)copy.m_TitleFont).GetLogFont (&font);
	m_TitleFont.CreateFontIndirect (&font);


	return *this;
}

CXGraphAxis::~CXGraphAxis()
{
	if (m_Font.m_hObject != NULL)
		m_Font.DeleteObject ();

	if (m_TitleFont.m_hObject != NULL)
		m_TitleFont.DeleteObject ();

}

void CXGraphAxis::DeleteColorRange(int nRange)
{
	if (nRange < m_ColorRanges.size())
		m_ColorRanges.erase (&m_ColorRanges[nRange]);

}

void CXGraphAxis::DeleteAllColorRanges()
{
	m_ColorRanges.clear ();
}

void CXGraphAxis::SetColorRange(int nRange, double fMin, double fMax, COLORREF crColor, UINT nStyle)
{	
	COLORRANGE range;

	range.fMin = fMin;
	range.fMax = fMax;
	range.crColor = crColor;
	range.nStyle = nStyle;

	if (nRange == m_ColorRanges.size())
		m_ColorRanges.push_back (range);
	else
		m_ColorRanges[nRange] = range;

	m_pGraph->Invalidate ();

}

void CXGraphAxis::SetTitleFont (LPLOGFONT pLogFont)
{
	if (m_TitleFont.m_hObject != NULL)
		m_TitleFont.DeleteObject ();
	
	m_TitleFont.CreateFontIndirect (pLogFont);
}


void CXGraphAxis::SetFont (LPLOGFONT pLogFont)
{
	if (m_Font.m_hObject != NULL)
		m_Font.DeleteObject ();
	
	m_Font.CreateFontIndirect (pLogFont);
}

void CXGraphAxis::SetFont (CString cFontName, int nFontHeight, int nFontStyle)
{
	if (m_Font.m_hObject != NULL)
		m_Font.DeleteObject ();
	
	m_Font.CreateFont(nFontHeight, 0, 0, 0, nFontStyle, 0, 0, 0, DEFAULT_CHARSET,
			   OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, ANTIALIASED_QUALITY,
			   DEFAULT_PITCH | FF_DONTCARE, cFontName);
}

void CXGraphAxis::SetTitleFont (CString cFontName, int nFontHeight, int nFontStyle)
{
	if (m_TitleFont.m_hObject != NULL)
		m_TitleFont.DeleteObject ();
	
	m_TitleFont.CreateFont(nFontHeight, 0, 0, 0, nFontStyle, 0, 0, 0, DEFAULT_CHARSET,
			   OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, ANTIALIASED_QUALITY,
			   DEFAULT_PITCH | FF_DONTCARE, cFontName);
}

int CXGraphAxis::GetMaxLabelWidth(CDCEx *pDC)
{
	CFontSelector fs(&m_Font, pDC, false);
	
	int			  nMinWidth, nMaxWidth;
	CString       cTxt;

	if (!m_bDateTime)
	{	
		cTxt.Format (m_cDisplayFmt, m_fCurMin);
		nMinWidth = GetTextExtentEx (pDC, cTxt).cx;
		cTxt.Format (m_cDisplayFmt, m_fCurMax);
		nMaxWidth = GetTextExtentEx (pDC, cTxt).cx;
	}
	else
	{
		cTxt = COleDateTime(m_fCurMin).Format (m_cDisplayFmt);
		nMinWidth = GetTextExtentEx (pDC, cTxt).cx;
		cTxt = COleDateTime(m_fCurMax).Format (m_cDisplayFmt);
		nMaxWidth = GetTextExtentEx (pDC, cTxt).cx;
	}


	return max(nMinWidth + m_nTickSize, nMaxWidth + m_nTickSize);
}

int CXGraphAxis::GetMaxLabelHeight(CDCEx *pDC)
{
	CFontSelector fs(&m_Font, pDC, false);

	int			  nHeight;
	CString       cTxt;

	if (!m_bDateTime)
		cTxt.Format (m_cDisplayFmt, m_fMin);
	else
		cTxt = COleDateTime(m_fMin).Format (m_cDisplayFmt);

	nHeight = GetTextExtentEx (pDC, cTxt).cy;
	
	return nHeight + m_nTickSize;
}

CSize CXGraphAxis::GetTitleSize(CDCEx *pDC)
{
	CFontSelector fs(&m_TitleFont, pDC, false);
	
	return GetTextExtentEx (pDC, m_cLabel);
}


void CXGraphAxis::DrawMarker(CDCEx *pDC, CPoint point, int nMarker, int nSize, COLORREF crColor, bool bSymbol)
{
	if (bSymbol)
	{
		m_pGraph->m_pDrawFn[nMarker](point, nSize, crColor, false, pDC);
		return;
	}

	CRect clMarker(point.x, point.y, point.x + nSize, point.y + nSize);

	if (pDC->m_bMono)
		pDC->FillSolidRect(clMarker, 0L);
	else
		pDC->FillSolidRect(clMarker, crColor);

	CQuickFont font("Arial", nSize, FW_THIN);
	CFontSelector fs(&font, pDC);
	CString cMarker;
	cMarker.Format("%d", nMarker + 1);

	COLORREF crText;

	if (!pDC->m_bMono)
	{
		int nLuminance = max (GetRValue(crColor), GetGValue(crColor));
		nLuminance = max(nLuminance, GetBValue(crColor) / 2);

		if (nLuminance > 128)
			crText = 0L;
		else
			crText = RGB(0xff,0xff,0xff);
	}
	else
		crText = RGB(255,255,255);
		
	COLORREF crOldText = pDC->GetTextColor();
	pDC->SetTextColor (crText);
	pDC->DrawText(cMarker, clMarker, DT_CENTER);
	pDC->SetTextColor (crOldText);
}


void CXGraphAxis::DrawArc(CDCEx *pDC, CPoint point, EDirection direction, int nSize)
{
	switch (direction)
	{
	case up :
		{
			int x = 0;
			for (int y = point.y; y < (point.y + nSize); y++, x++)
			{
				pDC->MoveTo(point.x - x, y);
				pDC->LineTo(point.x + x, y);
			}
			break;
		}
	case down :
		{
			int x = nSize;
			for (int y = point.y; y < (point.y + nSize); y++, x--)
			{
				pDC->MoveTo(point.x - x, y);
				pDC->LineTo(point.x + x, y);
			}
			break;
		}
	case left :
		{
			int y = 0;

			for (int x = point.x; x < (point.x + nSize); x++, y++)
			{
				pDC->MoveTo(x,point.y - y);
				pDC->LineTo(x,point.y + y);
			}
			break;
		}
	case right :
		{
			int y = nSize;

			for (int x = (point.x - nSize); x < point.x; x++, y--)
			{
				pDC->MoveTo(x,point.y - y);
				pDC->LineTo(x,point.y + y);
			}
			break;
		}

	}
}

int CXGraphAxis::DrawCurveMarkers(CDCEx *pDC, bool bDraw)
{
	if (!m_bShowMarker)
		return 0;

	if (m_AxisKind == yAxis)
	{
		int nHeight = 0;
		int nWidth  = 0;
		int nMarker = 0;

		for (int i = 0; i < m_pGraph->m_Data.size(); i++)
		{
			CXGraphDataSerie& serie = m_pGraph->m_Data[i];
						
			if (serie.m_bVisible && serie.m_nYAxis == m_pGraph->GetAxisIndex(this))
			{
				CPoint point;

				if (m_Placement == apRight)
					point = CPoint(m_nLeft - m_nMarkerSize / 2 + nWidth, m_clChart.top + nHeight);
				else
					point = CPoint(m_nLeft - m_nMarkerSize / 2 - nWidth, m_clChart.top + nHeight);


				if (bDraw)
					DrawMarker(pDC, point, serie.m_nMarkerType == 0 ? i : serie.m_nMarker, m_nMarkerSize, serie.m_crColor, serie.m_nMarkerType == 1);

				nMarker++;

				if (!(nMarker % 3))
				{
					nHeight += m_nMarkerSize;
					nWidth = 0;
				}
				else
					nWidth += m_nMarkerSize;

			}
		}

		if ((nMarker % 3))
			nHeight += m_nMarkerSize;
				
		return nHeight;
	}
	else
	{
		int nWidth  = m_nMarkerSize;
		int nHeight = 0;
		int nMarker = 0;

		for (int i = 0; i < m_pGraph->m_Data.size(); i++)
		{
			CXGraphDataSerie& serie = m_pGraph->m_Data[i];
			
			if (serie.m_bVisible && serie.m_nXAxis == m_pGraph->GetAxisIndex(this))
			{
				CPoint point (m_pGraph->m_clInnerRect.right - nWidth, m_nTop - m_nMarkerSize / 2 + nHeight);

				if (bDraw)
					DrawMarker(pDC, point, serie.m_nMarkerType == 0 ? i : serie.m_nMarker, m_nMarkerSize, serie.m_crColor, serie.m_nMarkerType == 1);

				nMarker++;
				if (!(nMarker % 2))
				{
					nWidth += m_nMarkerSize;
					nHeight = 0;
				}
				else
					nHeight += m_nMarkerSize;
			}
		}
		
		if ((nMarker % 2))
			nWidth += m_nMarkerSize;
		
		return nWidth;
	}
		
	return 0;
}

void CXGraphAxis::DrawColorRanges(CDCEx *pDC)
{	
	CQuickFont    font("Arial", 12, FW_BOLD);
	CFontSelector fs(&font, pDC);

	int nOldBkMode = pDC->GetBkMode ();
	pDC->SetBkMode (TRANSPARENT);
	if (m_AxisKind == yAxis)
	{
		for (int i = 0; i < m_ColorRanges.size(); i++)
		{
			TDataPoint point;
			point.fYVal = m_ColorRanges[i].fMin;
			int nMin = GetPointForValue (&point).y;
			point.fYVal = m_ColorRanges[i].fMax;
			int nMax = GetPointForValue (&point).y;
			CRect colorRect(m_pGraph->m_clInnerRect.left, 
				            min(max(nMax, m_pGraph->m_clInnerRect.top), m_pGraph->m_clInnerRect.bottom), 
							m_pGraph->m_clInnerRect.right, 
							max(min(nMin, m_pGraph->m_clInnerRect.bottom), m_pGraph->m_clInnerRect.top));
	
			if (m_ColorRanges[i].nStyle == HS_SOLID)
			{
				CBrush brush(m_ColorRanges[i].crColor);
				pDC->FillRect(colorRect, &brush);
			}
			else
			{
				CPenSelector ps(m_ColorRanges[i].crColor, 1, pDC);
				CBrushSelector bs(m_ColorRanges[i].crColor, m_ColorRanges[i].nStyle, pDC);
				pDC->Rectangle(colorRect);
			}
			
			
		}
	}
	else
	{ 
		for (int i = 0; i < m_ColorRanges.size(); i++)
		{
			TDataPoint point;
			point.fXVal = m_ColorRanges[i].fMin;
			int nMin = GetPointForValue (&point).x;
			point.fXVal = m_ColorRanges[i].fMax;
			int nMax = GetPointForValue (&point).x;
			CRect colorRect(min(max(nMin,m_pGraph->m_clInnerRect.left),m_pGraph->m_clInnerRect.right) ,
				            m_pGraph->m_clInnerRect.top, 
							max(min(nMax,m_pGraph->m_clInnerRect.right), m_pGraph->m_clInnerRect.left),
							m_pGraph->m_clInnerRect.bottom);

			if (m_ColorRanges[i].nStyle == HS_SOLID)
			{
				CBrush brush(m_ColorRanges[i].crColor);
				pDC->FillRect(colorRect, &brush);
			}
			else
			{
				CPenSelector ps(m_ColorRanges[i].crColor, 1, pDC);
				CBrushSelector bs(m_ColorRanges[i].crColor, m_ColorRanges[i].nStyle, pDC);
				pDC->Rectangle(colorRect);
			}			
			
		}
	}

	pDC->SetBkMode (nOldBkMode);
}


void CXGraphAxis::Draw(CDCEx* pDC)
{
	CDCEx *pTmpDC = NULL;

	if (!m_bVisible)
	{
		pTmpDC = new CDCEx;
		pTmpDC->CreateCompatibleDC (pDC);
		pDC = pTmpDC;
	}

	if (m_bAutoScale)
		AutoScale(pDC);

	m_fRange = m_fCurMax - m_fCurMin;

	if (m_AxisKind == yAxis)
	{	
		m_fSpareRange = (2*m_nArcSize);

		m_fMarkerSpareSize = DrawCurveMarkers(pDC, false);

		if (m_cLabel != "")
		{
			CSize titleSize;
			{
				CFontSelector fs(&m_TitleFont, pDC, false);
				titleSize = GetTitleSize(pDC);
				m_fSpareRange += titleSize.cy;
			}
		}

		int		nChartTop = m_clChart.top + m_fMarkerSpareSize;
		int		nTop      = m_pGraph->m_clInnerRect.top + m_fSpareRange + m_fMarkerSpareSize;
		int		nBottom   = m_pGraph->m_clInnerRect.bottom;
		double  fSteps	  = (m_fCurMax - m_fCurMin) / m_fStep;
		double  fStep     = ((double)(m_nTop - nChartTop - m_fSpareRange ) / fSteps);
		double  fnY       = m_nTop;
		double  fnYC      = fnY;
		
		for (double f = m_fCurMin; f <= m_fCurMax; f += m_fStep, fnYC -= fStep);
		
		m_nRange = nBottom - nTop;
		//m_nRange = m_nTop - fnYC - fStep;
	
		DrawColorRanges(pDC);

		m_fMarkerSpareSize = DrawCurveMarkers(pDC);
				
		if (pDC->m_bMono)
			pDC->SetBkColor(RGB(255,255,255));
		else
			pDC->SetBkColor(m_pGraph->m_crGraphColor);

		pDC->SetBkMode(OPAQUE);
	
		if (m_cLabel != "")
		{
			CSize titleSize;

			{
				CFontSelector fs(&m_TitleFont, pDC, false);
				titleSize = GetTitleSize(pDC);
				m_fSpareRange += titleSize.cy;
			}
			
			CFontSelector fs(&m_TitleFont, pDC);
			
			if (m_Placement == apLeft)
			{
				CRect clTitle(m_nLeft - titleSize.cx - m_nTickSize, nChartTop, m_nLeft - m_nTickSize, nChartTop + titleSize.cy);
				pDC->DrawText(m_cLabel, clTitle, DT_RIGHT);
			}
			else
			{
				CRect clTitle(m_nLeft + m_nTickSize, nChartTop, m_nLeft + titleSize.cx + m_nTickSize, nChartTop + titleSize.cy);
				pDC->DrawText(m_cLabel, clTitle, DT_LEFT);
			}
		}
		
		CString cTxt;
		CSize   csText;
	
		for (double fY = m_fCurMin; fY <= m_fCurMax; fY += m_fStep, fnY -= fStep)
		{
			{
				CFontSelector fs(&m_Font, pDC, false);
					
				if (m_bDateTime)
					cTxt = COleDateTime(fY).Format(m_cDisplayFmt);
				else
					cTxt.Format(m_cDisplayFmt, fY);

				csText = GetTextExtentEx (pDC, cTxt);
			}
			
			CFontSelector fs(&m_Font, pDC);

			if (m_Placement == apLeft)
			{
				CRect crText(m_nLeft - csText.cx - m_nTickSize, fnY - csText.cy / 2, m_nLeft - m_nTickSize, fnY + csText.cy / 2);
				pDC->DrawText(cTxt, crText, DT_RIGHT);
				CPenSelector ps(m_crGridColor, 1, pDC, m_nGridStyle);
				pDC->MoveTo(m_nLeft - m_nTickSize, fnY);
				// Spare last gridline to prevent axis to be overdrawn
				if (!m_bShowGrid || (int)fnY == m_nTop)
					pDC->LineTo(m_nLeft, fnY);
				else
					pDC->LineTo(m_pGraph->m_clInnerRect.right, fnY);
		
			}
			
			if (m_Placement == apRight)
			{

				CRect crText(m_nLeft + m_nTickSize, fnY - csText.cy / 2, m_nLeft + csText.cx + m_nTickSize, fnY + csText.cy / 2);
				pDC->DrawText(cTxt, crText, DT_LEFT);
				CPenSelector ps(m_crGridColor, 1, pDC, m_nGridStyle);
				pDC->MoveTo(m_nLeft + m_nTickSize, fnY);
				// Spare last gridline to prevent axis to be overdrawn
				if (!m_bShowGrid || (int)fnY == m_nTop)
					pDC->LineTo(m_nLeft, fnY);
				else
					pDC->LineTo(m_pGraph->m_clInnerRect.left, fnY);
			}
			
		}

		CPenSelector ps(m_crColor, 1, pDC);
		pDC->MoveTo (m_nLeft, m_nTop);
		pDC->LineTo (m_nLeft, nChartTop);
		DrawArc(pDC, CPoint(m_nLeft, nChartTop), up, m_nArcSize);
		m_clRect.SetRect(m_nLeft - 5, nChartTop, m_nLeft + 5, m_nTop);
	//	m_nRange = m_nTop - fnY - fStep;

	}
	else
	{
		m_fSpareRange = (4*m_nArcSize);
		m_fMarkerSpareSize = DrawCurveMarkers(pDC, false);
		
		if (m_cLabel != "")
		{	
			CSize titleSize;
			{
				CFontSelector fs(&m_TitleFont, pDC, false);
				titleSize = GetTitleSize(pDC);
				m_fSpareRange += titleSize.cx;
			}
		}

		int    nChartRight = m_pGraph->m_clInnerRect.right - m_fMarkerSpareSize; 
		int    nLeft       = m_pGraph->m_clInnerRect.left;
		int	   nRight      = m_pGraph->m_clInnerRect.right - m_fSpareRange -  m_fMarkerSpareSize;
		double fSteps	   = ((m_fCurMax - m_fCurMin) / m_fStep);
		double fStep	   = ((double)(nChartRight - m_nLeft - m_fSpareRange) / fSteps);
		double fnX		   = m_nLeft;
		double fnXC        = fnX;
				
		for (double f = m_fCurMin; f < m_fCurMax; f += m_fStep, fnXC += fStep);
				
		m_nRange = fSteps * fStep;
		//m_nRange = nChartRight - nLeft;
		//m_nRange = fnXC - m_nLeft - fStep;
	
		DrawColorRanges(pDC);

		m_fMarkerSpareSize = DrawCurveMarkers(pDC);

		if (pDC->m_bMono)
			pDC->SetBkColor(RGB(255,255,255));
		else
			pDC->SetBkColor(m_pGraph->m_crGraphColor);

		pDC->SetBkMode(OPAQUE);

		CString cTxt;
		CSize   csText;
		
		if (m_cLabel != "")
		{	
			CSize titleSize;
			{
				CFontSelector fs(&m_TitleFont, pDC, false);
				titleSize = GetTitleSize(pDC);
				m_fSpareRange += titleSize.cx;
			}
			CFontSelector fs(&m_TitleFont, pDC);
			CRect clTitle(nChartRight - titleSize.cx, m_nTop + m_nTickSize, nChartRight, m_nTop + m_nTickSize + titleSize.cy);
			pDC->DrawText(m_cLabel, clTitle, DT_LEFT);
		}

		for (double fX = m_fCurMin; fX < m_fCurMax; fX += m_fStep, fnX += fStep)
		{
			if (m_bDateTime)
				cTxt = COleDateTime(fX).Format(m_cDisplayFmt);
			else
				cTxt.Format(m_cDisplayFmt, fX);

			{
				CFontSelector fs(&m_Font, pDC, false);
				csText = GetTextExtentEx (pDC, cTxt);
			}

			CFontSelector fs(&m_Font, pDC);

			CRect crText(fnX - csText.cx / 2, m_nTop + m_nTickSize, fnX + csText.cx / 2+4, m_nTop + csText.cy + m_nTickSize);
			pDC->DrawText(cTxt, crText, DT_CENTER);
			CPenSelector ps(m_crGridColor, 1, pDC, m_nGridStyle);
			pDC->MoveTo(fnX, m_nTop + m_nTickSize);
			
			// Spare last gridline to prevent axis to be overdrawn
			if (!m_bShowGrid || (int)fnX == m_nLeft)
				pDC->LineTo(fnX, m_nTop);
			else
				pDC->LineTo(fnX, m_clChart.top);

		}

		CPenSelector ps(m_crColor, 1, pDC);
		pDC->MoveTo (m_nLeft, m_nTop);
		pDC->LineTo (nChartRight, m_nTop);
		DrawArc(pDC, CPoint(nChartRight, m_nTop), right, m_nArcSize);
		m_clRect.SetRect(m_nLeft, m_nTop - 5, nChartRight, m_nTop + 5);
//		m_nRange = fnX - m_nLeft - fStep;
	}

	if (m_bSelected)
		pDC->DrawFocusRect (m_clRect);
	
	if (pTmpDC)
		delete pTmpDC;
}


void CXGraphAxis::SetBestStep()
{
	CDCEx *pDC = (CDCEx*) m_pGraph->GetDC();

	if (m_AxisKind == yAxis)
	{
		
		int nTop    = m_pGraph->m_clInnerRect.top + m_fSpareRange + m_fMarkerSpareSize;
		int nBottom = m_pGraph->m_clInnerRect.bottom;
		int nRange  = nBottom - nTop;
		if (nRange == 0)
			nRange = 1;
		int nLabelHeight = GetMaxLabelHeight(pDC);
		m_fStep = (m_fCurMax - m_fCurMin) / (double) nRange * (double) nLabelHeight;
	}
	else
	{		
		int nLeft  = m_pGraph->m_clInnerRect.left;
		int nRight = m_pGraph->m_clInnerRect.right - m_fSpareRange -  m_fMarkerSpareSize;
		int nRange = nRight - nLeft;  
		if (nRange == 0)
			nRange = 1;

		int nLabelWidth = GetMaxLabelWidth(pDC);
		m_fStep = (m_fCurMax - m_fCurMin) / (double) nRange * (double) nLabelWidth;
	}

	m_pGraph->ReleaseDC(pDC);
	
}

bool CXGraphAxis::SetRange(double fMin, double fMax)
{
	if (fMin > fMax)
		return false;

	if (fMin == fMax)
	{
		fMin -= 1.0;
		fMax += 1.0;
	}

	// Switch to exponential display if values exceed 10^6 or 10^-6
	if ((fabs(fMin) > 1E6 || fabs(fMax) > 1E6) && !m_bDateTime)
		m_cDisplayFmt = "%5.2e";
	 
	m_fCurMin = m_fMin = fMin;
	m_fCurMax = m_fMax = fMax;

	if (m_pGraph->IsWindowVisible())
		SetBestStep();
	else
		m_fStep = (m_fCurMax - m_fCurMin) / 10.0;

	return true;
}

bool CXGraphAxis::SetCurrentRange(double fMin, double fMax, double fStep)
{
	if (fMin > fMax)
		return false;

	if (fMin == fMax)
	{
		fMin -= 1.0;
		fMax += 1.0;
	}

	// Switch to exponential display if values exceed 10^6 or 10^-6
	if ((fabs(fMin) > 1E6 || fabs(fMax) > 1E6) && !m_bDateTime)
		m_cDisplayFmt = "%5.2e";
	
	m_fCurMin = fMin;
	m_fCurMax = fMax;
	
	if (fStep != 0.0)
		m_fStep = fStep;
	else
	{
		if (m_pGraph->IsWindowVisible())
			SetBestStep();
		else
			m_fStep = (m_fCurMax - m_fCurMin) / 10.0;
	}

	return true;
}

void CXGraphAxis::Reset()
{
	m_fCurMin = m_fMin;
	m_fCurMax = m_fMax;

	m_fStep = (m_fMax - m_fMin) / 10.0f;
}

void CXGraphAxis::AutoScale(CDCEx *pDC)
{
	int nScaleTicks;

	if (m_AxisKind == yAxis)
	{
		m_fSpareRange = (2 * m_nArcSize);
		m_fMarkerSpareSize = DrawCurveMarkers(pDC, false);
	
		int nTop    = m_pGraph->m_clInnerRect.top + m_fSpareRange + m_fMarkerSpareSize;
		int nBottom = m_pGraph->m_clInnerRect.bottom;
		int nHeight = GetMaxLabelHeight(pDC);
		int nRange  = nBottom - nTop;
		
		nScaleTicks = max(2, (int)((double) nRange / (double)  nHeight));
	}
	else
	{
		m_fSpareRange = (4 * m_nArcSize);
		m_fMarkerSpareSize = DrawCurveMarkers(pDC, false);
	
		int nLeft  = m_pGraph->m_clInnerRect.left;
		int nRight = m_pGraph->m_clInnerRect.right - m_fSpareRange -  m_fMarkerSpareSize;
		int nWidth = GetMaxLabelWidth(pDC);
		int nRange = nRight - nLeft;  

		nScaleTicks = max(2, (int)((double) nRange / (double) nWidth));
	}

	double fStep;

	if (m_bDateTime)
	{
		FitTimeScale(&fStep, nScaleTicks, m_fCurMin, m_fCurMax);
		if (fStep > 0.0)
		{
			m_fStep = fStep; 
			AdaptTimeAxis(&m_fCurMin, &m_fCurMax, m_fStep);
		}
	}
	else
	{
		FitScale(&fStep, nScaleTicks, m_fCurMin, m_fCurMax);
		if (fStep > 0.0)
		{
			m_fStep = fStep;
			AdaptAxis(&m_fCurMin, &m_fCurMax, m_fStep);
		}
	}
}


double CXGraphAxis::GetValueForPos(int nPos)
{
	double fVal;
	
	m_fRange = m_fCurMax - m_fCurMin;
	
	if (m_AxisKind == yAxis)
		fVal = (-(nPos - m_pGraph->m_clInnerRect.bottom)) / ((double) m_nRange / m_fRange) + m_fCurMin;
	else
		fVal = (nPos - m_pGraph->m_clInnerRect.left) / ((double) m_nRange / m_fRange) + m_fCurMin;

	return fVal;

}

CPoint CXGraphAxis::GetPointForValue(TDataPoint* point)
{
	CPoint res;
	

	m_fRange = m_fCurMax - m_fCurMin;		

	if (m_AxisKind == yAxis)
		res.y = m_pGraph->m_clInnerRect.bottom - ((point->fYVal - m_fCurMin) * ((double) m_nRange / m_fRange));
	else
		res.x = m_pGraph->m_clInnerRect.left + ((point->fXVal - m_fCurMin) * ((double) m_nRange / m_fRange));

	return res;
}

void CXGraphAxis::GetRange(double&fMin, double& fMax, double& fStep)
{
	fMin = m_fCurMin;
	fMax = m_fCurMax;
	fStep = m_fStep;
}

void CXGraphAxis::Scale(double fFactor)
{
	if (!m_bDateTime)
		SetCurrentRange(m_fCurMin / fFactor, m_fCurMax / fFactor);
	else
	{
		COleDateTimeSpan range (m_fCurMax - m_fCurMin);

		int nSeconds = range.GetTotalSeconds () / fFactor;

		m_fCurMin += COleDateTimeSpan(0,0,0, nSeconds);
		m_fCurMax -= COleDateTimeSpan(0,0,0, nSeconds);

	}
}


void CXGraphAxis::GetIndexByXVal(long& nIndex, double x, int nCurve)
{
	double dStep;

	dStep = fabs(m_pGraph->m_Data[nCurve].m_pData[1].fXVal - m_pGraph->m_Data[nCurve].m_pData[0].fXVal);

	nIndex = (long)((x - m_pGraph->m_Data[nCurve].m_pData[0].fXVal) / dStep);

	if (nIndex < 0)
		nIndex = 0;

	if (nIndex >= m_pGraph->m_Data[nCurve].m_nCount)
		nIndex = m_pGraph->m_Data[nCurve].m_nCount - 1;

	if (m_pGraph->m_Data[nCurve].m_pData[nIndex].fXVal > x)
	{
		while (nIndex > 0 && m_pGraph->m_Data[nCurve].m_pData[nIndex].fXVal > x)
			nIndex --;
	}
	else
	{
		while (nIndex < m_pGraph->m_Data[nCurve].m_nCount && m_pGraph->m_Data[nCurve].m_pData[nIndex].fXVal <= x)
			nIndex ++;

	}
}

double CXGraphAxis::RoundDouble(double doValue, int nPrecision)
{
	static const double doBase = 10.0f;

	double doComplete5, doComplete5i;
	
	doComplete5 = doValue * pow(doBase, (double) (nPrecision + 1));
	
	if(doValue < 0.0f)
		doComplete5 -= 5.0f;
	else
		doComplete5 += 5.0f;
	
	doComplete5 /= doBase;

	modf(doComplete5, &doComplete5i);
	
	return doComplete5i / pow(doBase, (double) nPrecision);
}

// bypass that silly fmod problem (results unexpected due to binary representation)
// saveFmod(1.0,0.2) correctly returns 0.0 instead of 0.2
// complexity is O(x/y)

double saveFmod(double x, double y)
{
	double fRes = 0.0;

	while ((fRes+y) <= fabs(x))
		fRes += y;

	if (x < 0.0)
		return x + fRes;
	else
		return x - fRes;
}

void CXGraphAxis::AdaptAxis(double* fStart, double* fEnd, double fStep)
{
	double fRemainder;
	
	fRemainder = saveFmod(*fStart-fStep, fStep);

	if (fRemainder != 0.0)
		*fStart -= fRemainder + fStep;
	
	fRemainder = saveFmod(*fEnd+fStep, fStep);
	
	if (fRemainder != 0.0)
		*fEnd -= fRemainder - fStep;
	
}

void CXGraphAxis::AdaptTimeAxis(double * pfStart, double * pfEnd, double fStep)
{
	double			 fDummy;
	unsigned long	 iDummy;
	int				 iModulo = 0;
	COleDateTimeSpan cSpan;
	COleDateTime     cOffset, 
					 cStart, 
					 cEnd;
	double			 lSeconds;

	cOffset.SetDateTime (1970, 1, 1, 0, 0, 0);
	
	// Get Step in seconds
	fStep	 *= 24 * 3600;
	cStart	 = *pfStart;
	cSpan	 = cStart - cOffset;
	lSeconds = cSpan.GetTotalSeconds ();

	if (fStep > 0.0)
	   iModulo = ((long) lSeconds - (long) fStep) % (long)fStep;
	
	if (iModulo != 0)
	{
		iDummy = (unsigned long) lSeconds / (unsigned long) fStep;
		fDummy = (double) ((iDummy) * (unsigned long)fStep);
		*pfStart = (DATE)(COleDateTime(fDummy / 24.0 / 3600.0)) + cOffset ;
	}

	CString ccStart = COleDateTime(*pfStart).Format ();
	cEnd     = *pfEnd;
	cSpan    = cEnd - cOffset;
	lSeconds = cSpan.GetTotalSeconds ();
	
	if (fStep > 0.0)
		iModulo  = ((long) lSeconds - (long) fStep) % (long)fStep;

	if (iModulo != 0)
	{
		iDummy = (unsigned long) lSeconds / (unsigned long) fStep;
		fDummy = (double) ((iDummy+1) * (unsigned long)fStep);
		CString ccEnd = COleDateTime(*pfEnd).Format ();
		*pfEnd = (DATE)(COleDateTime(fDummy / 24.0 / 3600.0)) + cOffset ;
	}

	CString ccEnd = COleDateTime(*pfEnd).Format ();


}

void CXGraphAxis::FitTimeScale(double *fStepWidth, int nBestCount, double fStart, double fEnd)
{
	COleDateTime	 cStart, cEnd;
	COleDateTimeSpan cSpan;

	double fStep;                                           
	double fSpan;
		
	cStart = fStart;
	cEnd   = fEnd;
	cSpan  = cEnd - cStart;
	fSpan  = cSpan.GetTotalSeconds ();
	
	if(nBestCount > 0) 
		fStep = fSpan / (double)nBestCount;
	else
		fStep = 0;
	
	for (int i = 1; TimeStepTable[i] != -1; i++)
	{
		if (fStep > TimeStepTable[i-1] && fStep <= TimeStepTable[i])
		{
			fStep = TimeStepTable[i];
			break;
		}
	}
	
	*fStepWidth = fStep;
	*fStepWidth /= 24.0 * 3600;
}									


void CXGraphAxis::FitScale (double *fStepWidth, int nBestCount, double fStart, double fEnd)
{
	double fSize;
	double fStep;                                           
	double fDivider;
	int    nDecimalCount = 0;
                             
	fSize = fEnd - fStart;
	fStep = fSize / (double) nBestCount;		                                     

	if (fStep == 0.0)
		return;

	if (fStep < 1.0 && (int)fStep == 0)
		fDivider = 10.0;
	else               
		fDivider = 0.1;
       
	if (fStep >= 1.0)
	{
		for (nDecimalCount = 0; fStep >= 1.0; fStep *= fDivider, nDecimalCount++);
		fStep = fStep * (double)10.0;
		nDecimalCount--;
	}
	else
	{
		for (nDecimalCount = 0; fStep < 1.0; fStep *= fDivider, nDecimalCount--);
	}

	
	if (fStep > 0.0f && fStep <= 1.0)
		fStep = 1.0;
	if (fStep > 1.0f && fStep <= 2.0)
		fStep = 2.0;
	if (fStep > 2.0f && fStep <= 5.0)
		fStep = 5.0;
	if (fStep > 5.0f && fStep <= 10.0)
		fStep = 10.0;
	
	*fStepWidth = (double)((int)fStep * pow (10.0, (double)nDecimalCount));

	if (*fStepWidth == 0.0)
		*fStepWidth = 0.1;
}									

void CXGraphAxis::Serialize( CArchive& archive )
{
	LOGFONT font;
	int     nHelper;

	CObject::Serialize (archive);

    if( archive.IsStoring() )
    {
		archive << m_bAutoScale;
		archive << m_bVisible;
		archive << m_bShowGrid;
		archive << m_cDisplayFmt;
		archive << m_cLabel;
		archive << m_fMax;
		archive << m_fMin;
		archive << m_fCurMax;
		archive << m_fCurMin;
		archive << m_fStep;
		archive << m_nTop;
		archive << m_nLeft;
		
		nHelper = (int) m_AxisKind;
		archive << nHelper;
		
		archive << m_nArcSize;
		archive << m_nTickSize;
		archive << m_crColor;
		archive << m_crGridColor;
		archive << m_nGridStyle;
		archive << m_fSpareRange;
		archive << m_bDateTime;
		archive << m_fMarkerSpareSize;
		archive << m_fRange;
		archive << m_nRange;
		archive << m_nMarkerSize;

		nHelper = (int) m_Placement;
		archive << nHelper;
		archive << m_bShowMarker;
		
		archive << m_ColorRanges.size();
		for (int i = 0; i < m_ColorRanges.size(); i++)
			archive.Write(&m_ColorRanges[i], sizeof(COLORRANGE));
		
		m_Font.GetLogFont (&font);
		archive.Write(&font, sizeof(LOGFONT));

		m_TitleFont.GetLogFont (&font);
		archive.Write(&font, sizeof(LOGFONT));
		
    }
	else
    {
		archive >> m_bAutoScale;
		archive >> m_bVisible;
		archive >> m_bShowGrid;
		archive >> m_cDisplayFmt;
		archive >> m_cLabel;
		archive >> m_fMax;
		archive >> m_fMin;
		archive >> m_fCurMax;
		archive >> m_fCurMin;
		archive >> m_fStep;
		archive >> m_nTop;
		archive >> m_nLeft;
		
		archive >> nHelper;
		m_AxisKind = (EAxisKind) nHelper;
		
		archive >> m_nArcSize;
		archive >> m_nTickSize;
		archive >> m_crColor;
		archive >> m_crGridColor;
		archive >> m_nGridStyle;
		archive >> m_fSpareRange;
		archive >> m_bDateTime;
		archive >> m_fMarkerSpareSize;
		archive >> m_fRange;
		archive >> m_nRange;
		archive >> m_nMarkerSize;
		
		archive >> nHelper;
		m_Placement = (EAxisPlacement) nHelper;
		
		archive >> m_bShowMarker;
		
		archive >> nHelper;

		for (int i = 0; i < nHelper; i++)
		{
			COLORRANGE range;
			archive.Read(&range, sizeof(COLORRANGE));
			m_ColorRanges.push_back (range);
		}
		
		if (m_Font.m_hObject != NULL)
			m_Font.DeleteObject ();

		if (m_TitleFont.m_hObject != NULL)
			m_TitleFont.DeleteObject ();

		archive.Read(&font, sizeof(LOGFONT));
		m_Font.CreateFontIndirect (&font);
		archive.Read(&font, sizeof(LOGFONT));
		m_TitleFont.CreateFontIndirect (&font);

    }
}

#pragma warning (default : 4244)
#pragma warning (default : 4800)
