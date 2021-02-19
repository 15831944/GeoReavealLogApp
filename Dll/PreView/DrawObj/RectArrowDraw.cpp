/******************************************************************
*   RoundDrw.cpp - implementation of RoundRect draw object class  *           
*******************************************************************/

#include "stdafx.h" 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "GeoFIDio.h"
#include "TrackDraw.h"
#include "RectArrowDraw.h"            

#include "LogPlotView.h"
#include <math.h> 

//#include "HeadPlotView.h"

IMPLEMENT_SERIAL(CRectArrowObj, CTextObj, 0)

CRectArrowObj::CRectArrowObj()
{
	m_ShapeName = "RectArrow";
}

CRectArrowObj::CRectArrowObj(const CRect& rect,CDrawObj* pObj,CLogPlotDoc* pDocument) 
		: CTextObj(rect,pObj,pDocument)
{
	m_ShapeName = "RectArrow";
	m_ArrowPoint1.x = m_Rect.left;	m_ArrowPoint1.y = m_Rect.top+5;
	m_ArrowPoint2.x = m_Rect.left+10;	m_ArrowPoint2.y = m_Rect.top;
	m_ArrowPoint3.x = m_Rect.left-10; m_ArrowPoint3.y = m_Rect.top-20;
	m_nHandle1 = 1; m_nHandle2 = 3;
	m_LogBrush.lbStyle=BS_OWNER;
	m_LogBrush.lbHatch=0;
	CreatePolyPoint();
}

void CRectArrowObj::Serialize(CArchive& ar)
{
	CTextObj::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << m_ArrowPoint1;
		ar << m_ArrowPoint2;
		ar << m_ArrowPoint3;
		ar << m_nHandle1;
		ar << m_nHandle2;
	}
	else
	{
   		ar >> m_ArrowPoint1;
   		ar >> m_ArrowPoint2;
   		ar >> m_ArrowPoint3;
		ar >> m_nHandle1;
		ar >> m_nHandle2;
		CreatePolyPoint();
	}
}

void CRectArrowObj::LimitCoor(int nHandle, CPoint& point)
{
	CRect rect = m_Rect;
	rect.NormalizeRect();
	if (nHandle == 1) point.x = rect.left;
	if (nHandle == 2) point.x = rect.right;
	if (nHandle == 3) point.y = rect.top;
	if (nHandle == 4) point.y = rect.bottom;
}


void CRectArrowObj::CreatePolyPoint()
{
	CRect rect = m_Rect;
	rect.NormalizeRect();
	CPoint point[4];
	point[0] = rect.TopLeft();
	point[1].x = rect.right;
	point[1].y = rect.top;
	point[2] = rect.BottomRight();
	point[3].x = rect.left;
	point[3].y = rect.bottom;

	if (m_nHandle1 == 1)
	{
		if (m_nHandle2 == 3)
		{
			m_BezierPoint[0] = m_ArrowPoint1;
			m_BezierPoint[6] = m_ArrowPoint2;
			m_RectPoint[1] = point[1];
			m_RectPoint[2] = point[2];
			m_RectPoint[3] = point[3];
		}
		else if (m_nHandle2 == 4)
		{
			m_BezierPoint[0] = m_ArrowPoint2;
			m_BezierPoint[6] = m_ArrowPoint1;
			m_RectPoint[1] = point[0];
			m_RectPoint[2] = point[1];
			m_RectPoint[3] = point[2];
		}
		else if (m_nHandle2 == 1)
		{
			if (m_ArrowPoint1.y > m_ArrowPoint2.y)
			{
				m_BezierPoint[0] = m_ArrowPoint1;
				m_BezierPoint[6] = m_ArrowPoint2;
			}
			else
			{
				m_BezierPoint[0] = m_ArrowPoint2;
				m_BezierPoint[6] = m_ArrowPoint1;
			}
			m_RectPoint[1] = point[0];
			m_RectPoint[2] = point[1];
			m_RectPoint[3] = point[2];
			m_RectPoint[4] = point[3];
		}
		else ASSERT(FALSE);
	}
	else if (m_nHandle1 == 2)
	{
		if (m_nHandle2 == 4)
		{
			m_BezierPoint[0] = m_ArrowPoint1;
			m_BezierPoint[6] = m_ArrowPoint2;
			m_RectPoint[1] = point[3];
			m_RectPoint[2] = point[0];
			m_RectPoint[3] = point[1];
		}
		else if (m_nHandle2 == 3)
		{
			m_BezierPoint[0] = m_ArrowPoint2;
			m_BezierPoint[6] = m_ArrowPoint1;
			m_RectPoint[1] = point[2];
			m_RectPoint[2] = point[3];
			m_RectPoint[3] = point[0];
		}
		else if (m_nHandle2 == 2)
		{
			if (m_ArrowPoint1.y < m_ArrowPoint2.y)
			{
				m_BezierPoint[0] = m_ArrowPoint1;
				m_BezierPoint[6] = m_ArrowPoint2;
			}
			else
			{
				m_BezierPoint[0] = m_ArrowPoint2;
				m_BezierPoint[6] = m_ArrowPoint1;
			}
			m_RectPoint[1] = point[2];
			m_RectPoint[2] = point[3];
			m_RectPoint[3] = point[0];
			m_RectPoint[4] = point[1];
		}
		else ASSERT(FALSE);
	}
	else if (m_nHandle1 == 3)
	{
		if (m_nHandle2 == 2)
		{
			m_BezierPoint[0] = m_ArrowPoint1;
			m_BezierPoint[6] = m_ArrowPoint2;
			m_RectPoint[1] = point[2];
			m_RectPoint[2] = point[3];
			m_RectPoint[3] = point[0];
		}
		else if (m_nHandle2 == 1)
		{
			m_BezierPoint[0] = m_ArrowPoint2;
			m_BezierPoint[6] = m_ArrowPoint1;
			m_RectPoint[1] = point[1];
			m_RectPoint[2] = point[2];
			m_RectPoint[3] = point[3];
		}
		else if (m_nHandle2 == 3)
		{
			if (m_ArrowPoint1.x < m_ArrowPoint2.x)
			{
				m_BezierPoint[0] = m_ArrowPoint1;
				m_BezierPoint[6] = m_ArrowPoint2;
			}
			else
			{
				m_BezierPoint[0] = m_ArrowPoint2;
				m_BezierPoint[6] = m_ArrowPoint1;
			}
			m_RectPoint[1] = point[1];
			m_RectPoint[2] = point[2];
			m_RectPoint[3] = point[3];
			m_RectPoint[4] = point[0];
		}
		else ASSERT(FALSE);
	}
	else if (m_nHandle1 == 4)
	{
		if (m_nHandle2 == 1)
		{
			m_BezierPoint[0] = m_ArrowPoint1;
			m_BezierPoint[6] = m_ArrowPoint2;
			m_RectPoint[1] = point[0];
			m_RectPoint[2] = point[1];
			m_RectPoint[3] = point[2];
		}
		else if (m_nHandle2 == 2)
		{
			m_BezierPoint[0] = m_ArrowPoint2;
			m_BezierPoint[6] = m_ArrowPoint1;
			m_RectPoint[1] = point[3];
			m_RectPoint[2] = point[0];
			m_RectPoint[3] = point[1];
		}
		else if (m_nHandle2 == 4)
		{
			if (m_ArrowPoint1.x > m_ArrowPoint2.x)
			{
				m_BezierPoint[0] = m_ArrowPoint1;
				m_BezierPoint[6] = m_ArrowPoint2;
			}
			else
			{
				m_BezierPoint[0] = m_ArrowPoint2;
				m_BezierPoint[6] = m_ArrowPoint1;
			}
			m_RectPoint[1] = point[3];
			m_RectPoint[2] = point[0];
			m_RectPoint[3] = point[1];
			m_RectPoint[4] = point[2];
		}
		else ASSERT(FALSE);
	}
	else ASSERT(FALSE);
	m_RectPoint[0] = m_BezierPoint[6];
	int num = (m_nHandle1 == m_nHandle2) ? 5:4;
	m_RectPoint[num] = m_BezierPoint[0];
	if ((m_ArrowPoint1.x == m_ArrowPoint2.x) && (m_ArrowPoint1.y == m_ArrowPoint2.y))
	{
		point[0].x = (m_ArrowPoint1.x + m_ArrowPoint3.x) / 2;
		point[0].y = (m_ArrowPoint1.y + m_ArrowPoint3.y) / 2;
	}
	else if (m_ArrowPoint1.x == m_ArrowPoint2.x)
	{
		point[0].y = (m_ArrowPoint1.y + m_ArrowPoint2.y) / 2;
		point[0].x = (m_ArrowPoint1.x + m_ArrowPoint3.x) / 2;
	}
	else if (m_ArrowPoint1.y == m_ArrowPoint2.y)
	{
		point[0].x = (m_ArrowPoint1.x + m_ArrowPoint2.x) / 2;
		point[0].y = (m_ArrowPoint1.y + m_ArrowPoint3.y) / 2;
	}
	else
	{
		int dy = m_ArrowPoint1.y - m_ArrowPoint2.y;
		int dx = m_ArrowPoint1.x - m_ArrowPoint2.x;
		CPoint pt;
		double x = (m_ArrowPoint1.x + m_ArrowPoint2.x) / 2.0;
		double y = (m_ArrowPoint1.y + m_ArrowPoint2.y) / 2.0;
		double k1 = ((double)dy) / dx;
		double b1 = m_ArrowPoint3.y - k1 * m_ArrowPoint3.x;
		double k2 = -1.0 / k1;
		double b2 = y  - k2 * x;
		x = (b2 - b1) / (k1 - k2);
		y = k1 * x + b1;
		point[0].x = (int) (x+0.5);
		point[0].y = (int) (y+0.5);
	}
	m_BezierPoint[1].x = (point[0].x + m_BezierPoint[0].x) / 2;
	m_BezierPoint[1].y = (point[0].y + m_BezierPoint[0].y) / 2;
	m_BezierPoint[2] = m_BezierPoint[1];
	m_BezierPoint[3] = m_ArrowPoint3;
	m_BezierPoint[4].x = (point[0].x + m_BezierPoint[6].x) / 2;
	m_BezierPoint[4].y = (point[0].y + m_BezierPoint[6].y) / 2;
	m_BezierPoint[5] = m_BezierPoint[4];
}

void CRectArrowObj::InvalObj(CView* pView)
{
	ASSERT(pView != NULL);
	CClientDC dc(pView);              
	pView->OnPrepareDC(&dc);
	CRect rect = m_Rect;
	if (rect.left > m_ArrowPoint3.x) rect.left = m_ArrowPoint3.x;
	else if (rect.right < m_ArrowPoint3.x) rect.right = m_ArrowPoint3.x;
	if (rect.top > m_ArrowPoint3.y) rect.top = m_ArrowPoint3.y;
	else if (rect.bottom < m_ArrowPoint3.y) rect.bottom = m_ArrowPoint3.y;
	rect.InflateRect(5,5);
	pDoc->LPtoPP(rect);
	dc.LPtoDP(rect);
	pView->InvalidateRect(rect,FALSE);
	pView->UpdateWindow();
}

void CRectArrowObj::Draw(CView* pView,CDC* pDC)
{
	if (m_bSelected && (c_ToolShape == rectarrowtool))
		return; 	
	CRect rect = m_Rect;
	if (rect.left > m_ArrowPoint3.x) 
		rect.left = m_ArrowPoint3.x;
	else if (rect.right < m_ArrowPoint3.x) 
		rect.right = m_ArrowPoint3.x;
	if (rect.top > m_ArrowPoint3.y) 
		rect.top = m_ArrowPoint3.y;
	else if (rect.bottom < m_ArrowPoint3.y) 
		rect.bottom = m_ArrowPoint3.y;
	rect.InflateRect(10,10);
	pDoc->LPtoPP(rect);
	if (!pDC->RectVisible(rect)) return;
	int k = (m_nHandle1 == m_nHandle2) ? 6:5;

	CBrush br, *pOldBrush;
	if (m_LogBrush.lbStyle == BS_OWNER)
		pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	else
	{
	 	if (m_LogBrush.lbStyle == BS_HATCHED) 
		 	br.CreateHatchBrush((int)m_LogBrush.lbHatch,m_LogBrush.lbColor);
	   	else br.CreateBrushIndirect(&m_LogBrush);
		pOldBrush = pDC->SelectObject(&br);
	}

	CPen pen,*pOldPen;
	if (m_LogPen.lopnWidth.x > 0)
	{
		if (!pen.CreatePenIndirect(&m_LogPen))
			return;
		pOldPen = pDC->SelectObject(&pen);
	}
	else 
		pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);
	pDoc->LPtoPP(m_BezierPoint,7);
	pDoc->LPtoPP(m_RectPoint,k);
	CRgn Drgn;
	Drgn.CreatePolygonRgn( m_RectPoint, k, ALTERNATE);
	CRect rc;
	Drgn.GetRgnBox(&rc);
	rc.bottom+=1;
	pDC->BeginPath();
	pDC->PolyBezier(m_BezierPoint,7);
	pDC->RoundRect(rc,CPoint(16,16));
	pDC->EndPath();
	if(m_LogBrush.lbStyle == BS_OWNER)
	{
		if(m_LogBrush.lbHatch >= 0)
		{
			FillRoundRect(rc,CPoint(16,16),(int)m_LogBrush.lbHatch, m_LogPen.lopnColor,rect.TopLeft(),pDC,m_LogBrush.lbColor);
			LithFillPoly(m_BezierPoint,7,(int)m_LogBrush.lbHatch,m_LogPen.lopnColor,m_Rect.TopLeft(),pDC,m_LogBrush.lbColor);
		}
	}
	else pDC->FillPath();
	pDC->PolyBezier(m_BezierPoint,7);
	pDC->RoundRect(rc,CPoint(16,16));

	if (m_LogPen.lopnWidth.x > 0)
	{
		CPen pl(0,m_LogPen.lopnWidth.x,m_LogBrush.lbColor);
		pDC->SelectObject(&pl);
		pDC->MoveTo(m_ArrowPoint1.x+1,m_ArrowPoint1.y);
		pDC->LineTo(m_ArrowPoint2);
	}
	
	pDoc->PPtoLP(m_BezierPoint,7);
	pDoc->PPtoLP(m_RectPoint,k);	
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen); 

	CTextObj::Draw(pView,pDC);
	CDrawObj::Draw(pView,pDC);
}

int CRectArrowObj::GetHandleCount()
{   
	return 11;
}            

CPoint CRectArrowObj::GetHandlePoint(int nHandle)
{
	CPoint point(0,0);
	if (nHandle == 9) return m_ArrowPoint1;
	if (nHandle == 10) return m_ArrowPoint2;
	if (nHandle == 11) return m_ArrowPoint3;
	return CTextObj::GetHandlePoint(nHandle);	
}

int CRectArrowObj::HitTest(CView* pView,const CPoint& point)
{
	ASSERT_VALID(this);
	ASSERT(pView != NULL);
	int nHandle =  CTextObj::HitTest(pView,point);
	if (nHandle >=0 ) return nHandle;
	//if ((pView->IsKindOf(RUNTIME_CLASS(CHeadPlotView))) && (!((CHeadPlotView*)pView)->m_bEditEnable)) return -1;
	CRgn rgn,rgn1,rgn2;
    rgn.CreateRectRgn(0,0,0,0);
	CRect rect (point.x-3,point.y-3,point.x+3,point.y+3);
    rgn1.CreateRectRgnIndirect(rect);
	rgn2.CreateRectRgn(m_Rect.left,m_Rect.top,m_Rect.right,m_Rect.bottom);
    if (rgn.CombineRgn(&rgn1,&rgn2,RGN_AND) != NULLREGION) 
    {
	 	if (m_LogBrush.lbStyle != BS_NULL) return 0;
	 	if (!rgn.EqualRgn(&rgn1)) return 0;
    }
	return -1;
}

HCURSOR CRectArrowObj::GetHandleCursor(int nHandle)
{
	ASSERT_VALID(this);
	if (nHandle >= 9)
		return AfxGetApp()->LoadCursor(IDC_CURSOR_CROSS);
	return CTextObj::GetHandleCursor(nHandle);
}   

void CRectArrowObj::MoveHandle(CView* pView,int nHandle, const CPoint& point2)
{
	ASSERT_VALID(this);
	CPoint point1 = point2;
	if ((nHandle >= 9) || (nHandle == 0))
	{
		CPoint point;
		if (nHandle != 0) point = GetHandlePoint(nHandle); 
		CPoint point3; 
		point3.x = point.x + point1.x;
		point3.y = point.y + point1.y;
		if (nHandle != 0) 
		{
			CRect rect;
			if (pView->IsKindOf(RUNTIME_CLASS(CLogPlotView)))
				rect = pDoc->GetRect(); 
			else
				rect = pDoc->GetHeadRect();
			if (!rect.PtInRect(point3)) return;
		}
		if (((nHandle == 9) || (nHandle == 10)) && (m_nHandle1 != m_nHandle2))
		{
			if (point3.x < m_Rect.left) point3.x = m_Rect.left + 1 ;
			if (point3.x > m_Rect.right) point3.x = m_Rect.right - 1 ;
			if (point3.y < m_Rect.top) point3.y = m_Rect.top + 1 ;
			if (point3.y > m_Rect.bottom) point3.y = m_Rect.bottom - 1 ;
			if (nHandle == 9) point = m_ArrowPoint2;
			else point = m_ArrowPoint1;
			int dy = point.y - point3.y;
			int dx = point.x - point3.x;
			if (dx == 0) dx = 1;
			double k = ((double)dy) / dx;
			double b = point.y - k * point.x;
			double d = m_ArrowPoint3.y - k*m_ArrowPoint3.x - b;
			if ((((m_nHandle1 == 3) || (m_nHandle2 == 3)) && (d>0)) ||
			    (((m_nHandle1 == 4) || (m_nHandle2 == 4)) && (d<=0)))
				return;
		}
		switch (nHandle)
		{
		case 0:
			m_Rect += point1;
			m_ArrowPoint1.x += point1.x; m_ArrowPoint1.y += point1.y;
			m_ArrowPoint2.x += point1.x; m_ArrowPoint2.y += point1.y;
			m_ArrowPoint3.x += point1.x; m_ArrowPoint3.y += point1.y;
			break;
		case 9:
			if ((m_nHandle1 == 1) || (m_nHandle1 == 2))
			{
				if (m_ArrowPoint1.y + point1.y <= m_Rect.top)
				{
					if (m_nHandle2 != 4)
					{
						m_ArrowPoint1.y = m_Rect.top;
						m_nHandle1 = 3;
					}
					else return;
				}
				else if (m_ArrowPoint1.y + point1.y >= m_Rect.bottom)
				{
					if (m_nHandle2 != 3)
					{
						m_ArrowPoint1.y = m_Rect.bottom;
						m_nHandle1 = 4;
					}
					else return;
				}
				else m_ArrowPoint1.y += point1.y;
			}
			else if ((m_nHandle1 == 3) || (m_nHandle1 == 4))
			{
				if (m_ArrowPoint1.x + point1.x <= m_Rect.left)
				{
					if (m_nHandle2 != 2)
					{
						m_ArrowPoint1.x = m_Rect.left;
						m_nHandle1 = 1;
					}
					else return;
				}
				else if (m_ArrowPoint1.x + point1.x >= m_Rect.right)
				{
					if (m_nHandle2 != 1)
					{
						m_ArrowPoint1.x = m_Rect.right;
						m_nHandle1 = 2;
					}
					else return;
				}
				else m_ArrowPoint1.x += point1.x;
			}
			break;
		case 10:
			if ((m_nHandle2 == 1) || (m_nHandle2== 2))
			{
				if (m_ArrowPoint2.y + point1.y <= m_Rect.top)
				{
					if (m_nHandle1 != 4)
					{
						m_ArrowPoint2.y = m_Rect.top;
						m_nHandle2 = 3;
					}
					else return;
				}
				else if (m_ArrowPoint2.y + point1.y >= m_Rect.bottom)
				{
					if (m_nHandle1 != 3)
					{
						m_ArrowPoint2.y = m_Rect.bottom;
						m_nHandle2 = 4;
					}
					else return;
				}
				else m_ArrowPoint2.y += point1.y;
			}
			else if ((m_nHandle2 == 3) || (m_nHandle2 == 4))
			{
				if (m_ArrowPoint2.x + point1.x <= m_Rect.left)
				{
					if (m_nHandle1 != 2)
					{
						m_ArrowPoint2.x = m_Rect.left;
						m_nHandle2 = 1;
					}
					else return;
				}
				else if (m_ArrowPoint2.x + point1.x >= m_Rect.right)
				{
					if (m_nHandle1 != 1)
					{
						m_ArrowPoint2.x = m_Rect.right;
						m_nHandle2 = 2;
					}
					else return;
				}
				else m_ArrowPoint2.x += point1.x;
			}
			break;
		case 11:
			{
				BOOL bMirror = FALSE;
				if (m_nHandle1 == m_nHandle2)
				{
					if (m_nHandle1 == 1)
					{
						if (m_ArrowPoint3.x + point1.x > m_Rect.left)
						{
							m_ArrowPoint3.x = m_Rect.right + m_Rect.left - m_ArrowPoint3.x;
							m_ArrowPoint1.x = m_ArrowPoint2.x = m_Rect.right;
							m_nHandle1 = m_nHandle2 = 2;
							bMirror = TRUE;
						}
					}
					else if (m_nHandle1 == 2)
					{
						if (m_ArrowPoint3.x + point1.x < m_Rect.right)
						{
							m_ArrowPoint3.x = m_Rect.left + m_Rect.right - m_ArrowPoint3.x;
							m_ArrowPoint1.x = m_ArrowPoint2.x = m_Rect.left;
							m_nHandle1 = m_nHandle2 = 1;
							bMirror = TRUE;
						}
					}
					else if (m_nHandle1 == 3)
					{
						if (m_ArrowPoint3.y + point1.y > m_Rect.top)
						{
							m_ArrowPoint3.y = m_Rect.bottom + m_Rect.top - m_ArrowPoint3.y;
							m_ArrowPoint1.y = m_ArrowPoint2.y = m_Rect.bottom;
							m_nHandle1 = m_nHandle2 = 4;
							bMirror = TRUE;
						}
					}
					else if (m_nHandle1 == 4)
					{
						if (m_ArrowPoint3.y + point1.y < m_Rect.bottom)
						{
							m_ArrowPoint3.y = m_Rect.top + m_Rect.bottom - m_ArrowPoint3.y;
							m_ArrowPoint1.y = m_ArrowPoint2.y = m_Rect.top;
							m_nHandle1 = m_nHandle2 = 3;
							bMirror = TRUE;
						}
					}
				}
				else
				{
					CPoint pt = m_ArrowPoint3;
					m_ArrowPoint3.x += point1.x; 
					m_ArrowPoint3.y += point1.y;
					int dy = m_ArrowPoint1.y - m_ArrowPoint2.y;
					int dx = m_ArrowPoint1.x - m_ArrowPoint2.x;
					if (dx == 0) dx = 1;
					double k = ((double)dy) / dx;
					double b = m_ArrowPoint1.y - k * m_ArrowPoint1.x;
					double d = m_ArrowPoint3.y - k*m_ArrowPoint3.x - b;
					if ((((m_nHandle1 == 3) || (m_nHandle2 == 3)) && (d>0)) ||
					    (((m_nHandle1 == 4) || (m_nHandle2 == 4)) && (d<=0)))
					{
						if (m_Rect.PtInRect(m_ArrowPoint3))
						{
							m_ArrowPoint1.x = m_Rect.right + m_Rect.left - m_ArrowPoint1.x;
							m_ArrowPoint1.y = m_Rect.top + m_Rect.bottom - m_ArrowPoint1.y;
							m_ArrowPoint2.x = m_Rect.right + m_Rect.left - m_ArrowPoint2.x;
							m_ArrowPoint2.y = m_Rect.top + m_Rect.bottom - m_ArrowPoint2.y;
							m_ArrowPoint3.x = m_Rect.right + m_Rect.left - m_ArrowPoint3.x;
							m_ArrowPoint3.y = m_Rect.top + m_Rect.bottom - m_ArrowPoint3.y;
							if (m_nHandle1 < 3) m_nHandle1 = 3 - m_nHandle1;
							else m_nHandle1 = 7 - m_nHandle1;
							if (m_nHandle2 < 3) m_nHandle2 = 3 - m_nHandle2;
							else m_nHandle2 = 7 - m_nHandle2;
						}
						else if (((d > 0) && (m_ArrowPoint3.y < m_Rect.top)) || ((d < 0) && (m_ArrowPoint3.y > m_Rect.bottom)))
						{
							if ((m_nHandle1 == 1) || (m_nHandle1 == 2))
							{
								m_ArrowPoint1.x = m_Rect.left + m_Rect.right - m_ArrowPoint1.x;
								m_nHandle1 = 3 - m_nHandle1;
							}
							else if ((m_nHandle2 == 1) || (m_nHandle2 == 2))
							{
								m_ArrowPoint2.x = m_Rect.left + m_Rect.right - m_ArrowPoint2.x;
								m_nHandle2 = 3 - m_nHandle2;
							}
						}
						else 
						{
							if ((m_nHandle1 == 3) || (m_nHandle1 == 4))
							{
								m_ArrowPoint1.y = m_Rect.top + m_Rect.bottom - m_ArrowPoint1.y;
								m_nHandle1 = 7 - m_nHandle1;
							}
							else if ((m_nHandle2 == 3) || (m_nHandle2 == 4))
							{
								m_ArrowPoint2.y = m_Rect.top + m_Rect.bottom - m_ArrowPoint2.y;
								m_nHandle2 = 7 - m_nHandle2;
							}
						}
						bMirror = TRUE;
					}
					else m_ArrowPoint3 = pt;
				}
				if (!bMirror)
				{
					m_ArrowPoint3.x += point1.x; 
					m_ArrowPoint3.y += point1.y;
				}
			}
			break;
		default : ASSERT(FALSE);
		}
		CreatePolyPoint();
		pDoc->SetModifiedFlag(); 
		return;
	}   
	CRect rect = m_Rect;
	CDrawObj::MoveHandle(pView,nHandle, point1);
	if ((rect == m_Rect) || (m_Rect.left > m_Rect.right) || (m_Rect.top > m_Rect.bottom)) return;
	if (m_Rect.PtInRect(m_ArrowPoint3))
	{
		m_Rect = rect;
		return;
	}
	else if (m_nHandle1 != m_nHandle2)
	{
		int dy = m_ArrowPoint1.y - m_ArrowPoint2.y;
		int dx = m_ArrowPoint1.x - m_ArrowPoint2.x;
		if (dx == 0) dx = 1;
		double k = ((double)dy) / dx;
		double b = m_ArrowPoint1.y - k * m_ArrowPoint1.x;
		double d = m_ArrowPoint3.y - k*m_ArrowPoint3.x - b;
		if ((((m_nHandle1 == 3) || (m_nHandle2 == 3)) && (d>0)) ||
		    (((m_nHandle1 == 4) || (m_nHandle2 == 4)) && (d<=0)))
		{
			m_Rect = rect;
			return;
		}
	}
    if (c_ToolShape == rectarrowtool) return;
    double cx = 1.0 * m_Rect.Width() / m_pFocusObj->m_Rect.Width();
    double cy = 1.0 * m_Rect.Height() / m_pFocusObj->m_Rect.Height();
    CPoint point;
    point.x = m_pFocusObj->m_Rect.left - m_Rect.left;
    point.y = m_pFocusObj->m_Rect.top - m_Rect.top;
    m_ArrowPoint1.x = m_Rect.left + Round((m_pFocusObj->m_ArrowPoint1.x - m_pFocusObj->m_Rect.left) * cx);
    m_ArrowPoint1.y = m_Rect.top + Round((m_pFocusObj->m_ArrowPoint1.y - m_pFocusObj->m_Rect.top) * cy);
    m_ArrowPoint2.x = m_Rect.left + Round((m_pFocusObj->m_ArrowPoint2.x - m_pFocusObj->m_Rect.left) * cx);
    m_ArrowPoint2.y = m_Rect.top + Round((m_pFocusObj->m_ArrowPoint2.y - m_pFocusObj->m_Rect.top) * cy);
//  m_ArrowPoint3.x = m_Rect.left + Round((m_pFocusObj->m_ArrowPoint3.x - m_pFocusObj->m_Rect.left) * cx);
//  m_ArrowPoint3.y = m_Rect.top + Round((m_pFocusObj->m_ArrowPoint3.y - m_pFocusObj->m_Rect.top) * cy);
	LimitCoor(m_nHandle1,m_ArrowPoint1);
	LimitCoor(m_nHandle2,m_ArrowPoint2);
	CreatePolyPoint();
	pDoc->SetModifiedFlag(); 
}

void CRectArrowObj::CreateFocusObj(CView* pView)
{
	m_pFocusObj = new CRectArrowObj(m_Rect,m_pParentObj,pDoc);
	m_pFocusObj->pDoc = pDoc;     
	m_pFocusObj->m_TextRect = m_TextRect;
	m_pFocusObj->m_ArrowPoint1 = m_ArrowPoint1;
	m_pFocusObj->m_ArrowPoint2 = m_ArrowPoint2;
	m_pFocusObj->m_ArrowPoint3 = m_ArrowPoint3;
	m_pFocusObj->m_nHandle1 = m_nHandle1;
	m_pFocusObj->m_nHandle2 = m_nHandle2;
	m_pFocusObj->m_bSelected = m_bSelected;
	m_pFocusObj->CreatePolyPoint();

	m_pFocusObj->m_pFocusObj = this;
	m_pFocusObj->DrawFocus(pView);
}

void CRectArrowObj::DeleteFocusObj(CView* pView)
{
	ASSERT((m_pFocusObj != NULL) && (pView != NULL));
	if (m_pFocusObj == NULL) return;
	if (pView != NULL) m_pFocusObj->DrawFocus(pView);
 	delete m_pFocusObj;
	m_pFocusObj = NULL;
}   

void CRectArrowObj::DrawFocusObj(CView* pView)
{
	ASSERT((m_pFocusObj != NULL) && (pView != NULL));
	if ((m_pFocusObj == NULL) || (pView == NULL)) return;
	m_pFocusObj->DrawFocus(pView);
	if (m_SelectMode != move) return;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if (pObj != NULL) pObj->DrawFocusObj(pView);
	}                  
}

void CRectArrowObj::DrawFocus(CView* pView)
{
	ASSERT(pView != NULL);
	if (pView == NULL) return;
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc,NULL);
	dc.SetROP2(R2_XORPEN);
    CPen m_Pen(PS_DOT,1,RGB(0,0,0));
    CPen *pOldPen = dc.SelectObject(&m_Pen);
	int k = (m_nHandle1 == m_nHandle2) ? 6:5;
	if (m_bSelected && (c_ToolShape == rectarrowtool))
	{
	    CBrush* pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
		dc.Rectangle(m_Rect);
		dc.SelectObject(pOldBrush);
	}
	else
	{
		CRect rect = m_Rect;
		if (rect.left > m_ArrowPoint3.x) rect.left = m_ArrowPoint3.x;
		else if (rect.right < m_ArrowPoint3.x) rect.right = m_ArrowPoint3.x;
		if (rect.top > m_ArrowPoint3.y) rect.top = m_ArrowPoint3.y;
		else if (rect.bottom < m_ArrowPoint3.y) rect.bottom = m_ArrowPoint3.y;
		rect.InflateRect(5,5);
		pDoc->LPtoPP(rect);
		
		if (!dc.RectVisible(rect)) return;
		pDoc->LPtoPP(m_BezierPoint,7);
		pDoc->LPtoPP(m_RectPoint,k);
		
		dc.Polyline(m_RectPoint,k);
		dc.PolyBezier(m_BezierPoint,7);
		pDoc->PPtoLP(m_BezierPoint,7);
		pDoc->PPtoLP(m_RectPoint,k);
		
	}
	dc.SelectObject(pOldPen);
	dc.SetROP2(R2_COPYPEN);
	if (m_SelectMode == move) CTextObj::DrawFocus(pView);
}

void CRectArrowObj::SetRect(const CRect& rect)
{
	CPoint point;
	point.x = rect.left - m_Rect.left;
	point.y = rect.top - m_Rect.top;
	CTextObj::SetRect(rect);
	m_ArrowPoint1.x += point.x;
	m_ArrowPoint1.y += point.y;
	m_ArrowPoint2.x += point.x;
	m_ArrowPoint2.y += point.y;
	m_ArrowPoint3.x += point.x;
	m_ArrowPoint3.y += point.y;
	CreatePolyPoint();
}

void CRectArrowObj::OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point)
{
    if (m_bSelected)
    {
		if (c_ToolShape == rectarrowtool)
		{
			m_SelectMode = size;
			nDragHandle = 5;
		}              
		else
		{
			nDragHandle = HitTest(pView,point); 
			if (nDragHandle == 0) 
			{
				CTextObj::OnLButtonDown(pView,nFlags,point);
				if (bInput) return;
			}	
			else if (nDragHandle > 0) m_SelectMode = size; 
			else return;
			SetCursor(GetHandleCursor(nDragHandle));
		}	
	    CDrawObj::OnLButtonDown(pView,nFlags,point);
	}	
	if (m_SelectMode == move)
	{
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if (pObj != NULL) pObj->OnLButtonDown(pView, nFlags, point);
		}                  
	}	
	CreateFocusObj(pView);
}

void CRectArrowObj::OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point)
{
	if (bInput) return;
	if (m_SelectMode == move)
	{
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if (pObj != NULL) pObj->OnLButtonUp(pView, nFlags, point);
		}                  
		if (!m_bSelected)
		{
			DeleteFocusObj(pView);
			return;
		}	
	}	
	if (m_bSelected)
	{
		CRect m_FocusRect = m_pFocusObj->m_Rect;
		if ((m_FocusRect == m_Rect) && (nDragHandle<9))
		{
			DeleteFocusObj(pView);     
		}
		else if (m_SelectMode == move)
		{
			DeleteFocusObj(pView);     
	    	SetRect(m_FocusRect);
   			m_Rect.NormalizeRect();
			if (m_pParentObj == NULL) pDoc->OnChildPosChange(this);
			else m_pParentObj->OnChildPosChange(this);
		}   
		else if (m_SelectMode == size)
		{
			m_ArrowPoint1 = m_pFocusObj->m_ArrowPoint1;
			m_ArrowPoint2 = m_pFocusObj->m_ArrowPoint2;
			m_ArrowPoint3 = m_pFocusObj->m_ArrowPoint3;
			m_nHandle1 = m_pFocusObj->m_nHandle1;
			m_nHandle2 = m_pFocusObj->m_nHandle2;
			DeleteFocusObj(pView);     
			CRect rect = m_Rect;
			m_Rect = m_FocusRect;
   			m_Rect.NormalizeRect();
			CreatePolyPoint();
			if (rect.Width() != m_Rect.Width())
			{
				if (m_pParentObj == NULL) pDoc->OnChildWidthChange(this);
				else m_pParentObj->OnChildWidthChange(this);
			}
			if (rect.Height() != m_Rect.Height())
			{
				if (m_pParentObj == NULL) pDoc->OnChildHeightChange(this);
				else m_pParentObj->OnChildHeightChange(this);
			}
		}
	}
	CDrawObj::OnLButtonUp(pView,nFlags,point);
}

void CRectArrowObj::OnMouseMove(CView* pView, UINT nFlags, const CPoint& point)
{
	CPoint delta = (CPoint)(point - c_last);
	m_pFocusObj->DrawFocus(pView);
	if (m_SelectMode == move)
	{
		CPoint point1 = point;
		if (m_bSelected)
		{
			CRect rect = m_pFocusObj->m_Rect;
			rect.NormalizeRect();
			if ((rect.right + delta.x > m_xMax) && (delta.x > 0)) 
				point1.x = m_xMax - rect.right + c_last.x;
			if ((rect.left + delta.x < m_xMin) && (delta.x < 0)) 
				point1.x = m_xMin - rect.left + c_last.x;
			if ((rect.bottom + delta.y > m_yMax) && (delta.y > 0)) 
				point1.y = m_yMax - rect.bottom + c_last.y;
			if ((rect.top + delta.y < m_yMin) && (delta.y < 0)) 
				point1.y = m_yMin - rect.top + c_last.y;
			if (point1.x != point.x) delta.x = point1.x - c_last.x;
			if (point1.y != point.y) delta.y = point1.y - c_last.y;
		}
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if (pObj != NULL) pObj->OnMouseMove(pView, nFlags, point1);
		}
		m_pFocusObj->MoveHandle(pView,nDragHandle, delta);
	}	
	if (m_SelectMode == size)
		m_pFocusObj->MoveHandle(pView,nDragHandle, delta);
	m_pFocusObj->DrawFocus(pView);
	if (m_bSelected) 
	{
		DoScroll((CScrollView*)pView);
		CDrawObj::OnMouseMove(pView,nFlags,point);
	}
}

