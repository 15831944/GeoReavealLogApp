/******************************************************************
*   DepSect.cpp - implementation of the line draw object class   *           
*******************************************************************/

#include "stdafx.h" 
#include <math.h>
#include "PreView.h"
#include "LogPlotDoc.h"
#include "LogPlotView.h"
#include "TrackDraw.h"
#include "DepSect.h"            

IMPLEMENT_SERIAL(CDepSect, CRemDrawObj, 0)

CDepSect::CDepSect()
{
	m_bMove=TRUE;
 	m_DrawShape = depsectobj;
}

CDepSect::CDepSect(const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument)
	: CRemDrawObj(rect,pParentObj,pDocument)
{
	m_bMove=TRUE;
 	m_DrawShape = depsectobj;	
 	m_Point1.x = rect.left; m_Point1.y = rect.top; m_Point2 = m_Point1;
}       

void CDepSect::Normallize()
{
	m_Rect.left = m_Point1.x;
	m_Rect.right = m_Point2.x;
	m_Rect.top = m_Point1.y;
	m_Rect.bottom = m_Point2.y;
	m_Rect.NormalizeRect();
}

void CDepSect::SetRect(const CRect& rect)
{
	CPoint point;
	point.x = rect.left - m_Rect.left;
	point.y = rect.top - m_Rect.top;  
	m_Point1.x += point.x;
	m_Point2.x += point.x;
	m_Point1.y += point.y;
	m_Point2.y += point.y;
	CRemDrawObj::SetRect(rect);
}

void CDepSect::ZoomRect(const CRect& sRect, const CRect& zRect)
{
	if (sRect.Width() == 0)
	{
		m_Point1.x = zRect.left + m_Point1.x - sRect.left;
		m_Point2.x = zRect.left + m_Point2.x - sRect.left;
	}
	else
	{
		m_Point1.x = zRect.left + (m_Point1.x - sRect.left) * zRect.Width() / sRect.Width();
		m_Point2.x = zRect.left + (m_Point2.x - sRect.left) * zRect.Width() / sRect.Width();
	}
	if (sRect.Height() == 0)
	{
		m_Point1.y = zRect.top + m_Point1.y - sRect.top;
		m_Point2.y = zRect.top + m_Point2.y - sRect.top;
	}
	else
	{
		m_Point1.y = zRect.top + (m_Point1.y - sRect.top) * zRect.Height() / sRect.Height();
		m_Point2.y = zRect.top + (m_Point2.y - sRect.top) * zRect.Height() / sRect.Height();
	}
	CDrawObj::ZoomRect(sRect,zRect);
}

void CDepSect::AlignToGrid(const CPoint& point, const CSize& size)
{
	CPoint sPoint1 = m_Point1;
	CPoint sPoint2 = m_Point2;
	if (size.cx > 0)
	{
		m_Point1.x = point.x + ((int) ((m_Point1.x - point.x) * 1.0 / size.cx + 0.5)) * size.cx;           
		m_Point2.x = point.x + ((int) ((m_Point2.x - point.x) * 1.0 / size.cx + 0.5)) * size.cx;           
	}
	if (size.cy > 0)
	{
		m_Point1.y = point.y + ((int) ((m_Point1.y - point.y) * 1.0 / size.cy + 0.5)) * size.cy;           
		m_Point2.y = point.y + ((int) ((m_Point2.y - point.y) * 1.0 / size.cy + 0.5)) * size.cy;           
	}
	if (m_Point1 == m_Point2)
	{
		if (abs(sPoint1.x - sPoint2.x) > abs(sPoint1.y - sPoint2.y))
			m_Point2.x += size.cx;
		else
			m_Point2.y += size.cy;
	}
	Normallize();
	CDrawObj::AlignToGrid(point,size);
}

void CDepSect::Serialize(CArchive& ar)
{
	CRemDrawObj::Serialize(ar);
	if (ar.IsStoring())
	{          
		ar << m_Point1;
		ar << m_Point2;
		SerializeLogPen(ar,m_LogPen);
	}
	else
	{
		ar >> m_Point1;
		ar >> m_Point2;
		SerializeLogPen(ar,m_LogPen);
	}
}

BOOL CDepSect::Properties(CView* pView)
{
	return FALSE;
}

void CDepSect::Draw(CView* pView,CDC* pDC)
{
	CPen pen(PS_SOLID,1,RGB(255,0,0));
	CPen* pOldPen = pDC->SelectObject(&pen);
	CBrush *OldBrush = (CBrush *)pDC->SelectStockObject(NULL_BRUSH);
	CPoint point1 = m_Point1;
	CPoint point2 = m_Point2;
	pDoc->LPtoPP(&point1);
	pDoc->LPtoPP(&point2);
	pDC->MoveTo (pDoc->m_TrackRect.left,point1.y);
	pDC->LineTo (pDoc->m_TrackRect.right,point1.y);
	pDC->MoveTo (pDoc->m_TrackRect.left,point2.y);
	pDC->LineTo (pDoc->m_TrackRect.right,point2.y);

	CPen pen1(PS_SOLID,1,RGB(255,0,0));
	pDC->SelectObject(&pen1);

	int HalfX=point2.x;
	pDC->MoveTo (HalfX,point1.y);pDC->LineTo (HalfX,point2.y);
	pDC->MoveTo (HalfX-3,point1.y-1);pDC->LineTo (HalfX+2,point1.y-1);
	pDC->MoveTo (HalfX-3,point1.y+1);pDC->LineTo (HalfX+2,point1.y+1);
	pDC->MoveTo (HalfX-3,point2.y-1);pDC->LineTo (HalfX+2,point2.y-1);
	pDC->MoveTo (HalfX-3,point2.y+1);pDC->LineTo (HalfX+2,point2.y+1);
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(OldBrush);
	CDrawObj::Draw(pView,pDC);
}

int CDepSect::GetHandleCount()
{   
	return 2;
}            

CPoint CDepSect::GetHandlePoint(int nHandle)
{
	if (nHandle == 2) return m_Point2;
	return m_Point1;
}

HCURSOR CDepSect::GetHandleCursor(int nHandle)
{
	ASSERT_VALID(this);
	if (nHandle == 0) return AfxGetApp()->LoadCursor(IDC_CURSOR_SELECT);
	return AfxGetApp()->LoadCursor(IDC_CURSOR_CROSS);
}   

void CDepSect::MoveHandle(CView* pView,int nHandle, const CPoint& point1)
{
	ASSERT_VALID(this);     
	if ((point1.x == 0) && (point1.y == 0)) return;
	if (nHandle == 2)
	{
    	CRect rect1;
		if (pView->IsKindOf(RUNTIME_CLASS(CLogPlotView)))
			rect1 = pDoc->GetRect(); 
		CPoint point = m_Point2;
		point.x += point1.x;
		point.y += point1.y;
		if (!rect1.PtInRect(point)) return;
		m_Point2 = point;
	}	
	else if (nHandle == 1)
	{
    	CRect rect1;
		if (pView->IsKindOf(RUNTIME_CLASS(CLogPlotView)))
			rect1 = pDoc->GetRect(); 
		CPoint point = m_Point1;
		point.x += point1.x;
		point.y += point1.y;
		if (!rect1.PtInRect(point)) return;
		m_Point1 = point;
	}	
	else if (nHandle == 0)
	{
		m_Point1.x += point1.x;
		m_Point1.y += point1.y;
		m_Point2.x += point1.x;
		m_Point2.y += point1.y;
	}
}

void CDepSect::CreateFocusObj(CView* pView)
{
	m_pFocusObj = new CDepSect(m_Rect,m_pParentObj,pDoc);
	m_pFocusObj->pDoc = pDoc;                
	m_pFocusObj->m_Point1 = m_Point1;
	m_pFocusObj->m_Point2 = m_Point2;
	m_pFocusObj->m_LogPen.lopnStyle = PS_DOT;
	m_pFocusObj->DrawFocus(pView);
}

void CDepSect::DeleteFocusObj(CView* pView)
{
	ASSERT((m_pFocusObj != NULL) && (pView != NULL));
	if (m_pFocusObj == NULL) return;
	if (pView != NULL) m_pFocusObj->DrawFocus(pView);
	delete m_pFocusObj;
	m_pFocusObj = NULL;
}   

void CDepSect::DrawFocusObj(CView* pView)
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

void CDepSect::DrawFocus(CView* pView)
{
	ASSERT(pView != NULL);
	if (pView == NULL) return;
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc,NULL);
	dc.SetROP2(R2_XORPEN);
	
	CPen m_Pen(PS_DOT,1,RGB(0,0,0));
    CPen *pOldPen = dc.SelectObject(&m_Pen);
	CPoint point1=m_Point1,point2=m_Point2;	
	pDoc->LPtoPP(&point1);
	pDoc->LPtoPP(&point2);
	dc.MoveTo (pDoc->m_TrackRect.left,point1.y);
	dc.LineTo (pDoc->m_TrackRect.right,point1.y);
	dc.MoveTo (pDoc->m_TrackRect.left,point2.y);
	dc.LineTo (pDoc->m_TrackRect.right,point2.y);
	int HalfX=point2.x;
	dc.MoveTo (HalfX,point1.y);dc.LineTo (HalfX,point2.y);
	dc.SelectObject(pOldPen);
	dc.SetROP2(R2_COPYPEN);
}

void CDepSect::OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point)
{
    if (m_bSelected)
    {
		if (c_ToolShape == depsecttool)
		{
			m_SelectMode = size;
			nDragHandle = 2;
		}              
		else
		{
			nDragHandle = HitTest(pView,point); 
			if (nDragHandle == 0) 
			{
				m_SelectMode = move;
				CRect rect;
				if (pView->IsKindOf(RUNTIME_CLASS(CLogPlotView)))
					rect = pDoc->GetRect(); 
				m_xMin = rect.left;	m_xMax = rect.right;
				m_yMin = rect.top;  m_yMax = rect.bottom;	
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

void CDepSect::OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point)
{
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
		if (c_last == c_down)
		{
			DeleteFocusObj(pView);     
		}
		else if (m_SelectMode == move)
		{   
			m_pFocusObj->Normallize();
			CRect m_FocusRect = m_pFocusObj->m_Rect;
			DeleteFocusObj(pView);     
	    	SetRect(m_FocusRect);
			if (m_pParentObj == NULL) pDoc->OnChildPosChange(this);
			else m_pParentObj->OnChildPosChange(this);
		}   
		else if (m_SelectMode == size)
		{
			CRect rect = m_Rect;
			m_Point1 = m_pFocusObj->m_Point1;
			m_Point2 = m_pFocusObj->m_Point2;
			DeleteFocusObj(pView);     
   			Normallize();
			if ((m_Rect.Width() < 5) && (m_Rect.Height() < 5))
			{
				if (nDragHandle == 1)
				{
					if (m_Point1.x >= m_Point2.x) m_Point1.x = m_Point1.x + 5;
					else m_Point1.x = m_Point2.x - 5;
					if (m_Point1.y >= m_Point2.y) m_Point1.y = m_Point1.y + 5;
					else m_Point1.y = m_Point2.y - 5;
				}
				else
				{
					if (m_Point2.x >= m_Point1.x) m_Point2.x = m_Point2.x + 5;
					else m_Point2.x = m_Point1.x - 5;
					if (m_Point2.y >= m_Point1.y) m_Point2.y = m_Point2.y + 5;
					else m_Point2.y = m_Point1.y - 5;
				}
			}
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
	m_Point2.x=m_Point1.x;
	CDrawObj::OnLButtonUp(pView,nFlags,point);
}

void CDepSect::OnMouseMove(CView* pView, UINT nFlags, const CPoint& point)
{
	CPoint pt=point;
	if(!m_bMove) pt.y=c_last.y;
	CPoint delta = (CPoint)(pt - c_last);
	m_pFocusObj->DrawFocus(pView);
	if (m_SelectMode == move)
	{
		CPoint point1 = pt;
		if (m_bSelected)
		{
			m_pFocusObj->Normallize();
			CRect rect = m_pFocusObj->m_Rect;
			if ((rect.right + delta.x > m_xMax) && (delta.x > 0)) 
				point1.x = m_xMax - rect.right + c_last.x;
			if ((rect.left + delta.x < m_xMin) && (delta.x < 0)) 
				point1.x = m_xMin - rect.left + c_last.x;
			if ((rect.bottom + delta.y > m_yMax) && (delta.y > 0)) 
				point1.y = m_yMax - rect.bottom + c_last.y;
			if ((rect.top + delta.y < m_yMin) && (delta.y < 0)) 
				point1.y = m_yMin - rect.top + c_last.y;
			if (point1.x != pt.x) delta.x = point1.x - c_last.x;
			if (point1.y != pt.y) delta.y = point1.y - c_last.y;
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
		CDrawObj::OnMouseMove(pView,nFlags,pt);
	}
}

int CDepSect::HitTest(CView* pView,const CPoint& point)
{
	ASSERT_VALID(this);
	if (m_bSelected)
	{
		int nHandleCount = GetHandleCount();
		for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
		{
			CRect rc = GetHandleRect(pView,nHandle);
			if (rc.PtInRect(point)) 
				return nHandle;
		}
	}                     
	CRect rect;
	rect.left=m_Point1.x-5;rect.top=m_Point1.y;
	rect.right=m_Point2.x+5;rect.bottom=m_Point2.y;
	if (rect.PtInRect(point))
	{                                     
		double k,x1,x2,dx,y1,y2,dy;
		if ((rect.Width()< 5) || (rect.Height() < 5)) return 0;
		k = ((double)(m_Point1.y - m_Point2.y)) / (m_Point1.x - m_Point2.x);
		x1 = m_Point1.x - m_Point1.y / k; 
		x2 = point.x - point.y / k; 
		y1 = m_Point1.y - m_Point1.x * k; 
		y2 = point.y - point.x * k; 
		dx = fabs(x1-x2);
		dy = fabs(y1-y2);
		if ((dx<5) || (dy<5)) return 0;
	}	
	return -1;
}
