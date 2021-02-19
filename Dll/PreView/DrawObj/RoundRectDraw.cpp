/******************************************************************
*   RoundDrw.cpp - implementation of RoundRect draw object class  *           
*******************************************************************/

#include "stdafx.h" 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "GeoFIDio.h"
#include "TrackDraw.h"
#include "RoundRectDraw.h"            
#include "LogPlotView.h"
#include <math.h>       

//#include "HeadPlotView.h"

IMPLEMENT_SERIAL(CRoundRectObj, CTextObj, 0)

CRoundRectObj::CRoundRectObj()
{
	m_ShapeName = "RoundRect";
}

CRoundRectObj::CRoundRectObj(const CRect& rect,CDrawObj* pObj,CLogPlotDoc* pDocument) 
		: CTextObj(rect,pObj,pDocument)
{
	m_ShapeName = "RoundRect";
	m_Roundness.x = 16;
	m_Roundness.y = 16;
}

void CRoundRectObj::Serialize(CArchive& ar)
{
	CTextObj::Serialize(ar);
	if (ar.IsStoring())
		ar << m_Roundness;
	else
   		ar >> m_Roundness;
}

void CRoundRectObj::Draw(CView* pView,CDC* pDC)
{
	CRect rect = m_Rect;	
	pDoc->LPtoPP(rect);
	if (!pDC->RectVisible(rect)) return;
	CBrush br, *pOldBrush;
	if (m_LogBrush.lbStyle == BS_OWNER)
	{
		if(m_LogBrush.lbHatch >= 0) 
			FillRoundRect(rect,m_Roundness,(int)m_LogBrush.lbHatch,m_LogPen.lopnColor,rect.TopLeft(),pDC, m_LogBrush.lbColor);
		pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	}
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
	else pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);

	pDC->RoundRect(rect,m_Roundness);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);        


	CTextObj::Draw(pView,pDC);
	CDrawObj::Draw(pView,pDC);
}

int CRoundRectObj::GetHandleCount()
{   
	return 9;
}            

CPoint CRoundRectObj::GetHandlePoint(int nHandle)
{
	CPoint point(0,0);
	if (nHandle == 9)
	{
		CRect rect = m_Rect;
		rect.NormalizeRect();
		point = rect.BottomRight();
		point.x -= m_Roundness.x / 2;
		point.y -= m_Roundness.y / 2;
		return point;
	}
	return CTextObj::GetHandlePoint(nHandle);	
}

int CRoundRectObj::HitTest(CView* pView,const CPoint& point)
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

	rgn2.CreateRoundRectRgn(m_Rect.left,m_Rect.top,m_Rect.right,m_Rect.bottom,m_Roundness.x,m_Roundness.y);
    if (rgn.CombineRgn(&rgn1,&rgn2,RGN_AND) != NULLREGION) 
    {
	 	if (m_LogBrush.lbStyle != BS_NULL) return 0;
	 	if (!rgn.EqualRgn(&rgn1)) return 0;
    }
	return -1;
}

HCURSOR CRoundRectObj::GetHandleCursor(int nHandle)
{
	ASSERT_VALID(this);
	if (nHandle >= 9)
		return AfxGetApp()->LoadCursor(IDC_CURSOR_CROSS);
	return CTextObj::GetHandleCursor(nHandle);
}   

void CRoundRectObj::MoveHandle(CView* pView,int nHandle, const CPoint& point2)
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
		CRect rect = m_Rect;
		rect.NormalizeRect();
		if (nHandle == 0)
		{
			m_Rect += point1;
		}
		else if (nHandle == 9)
		{
			point.x += point1.x; point.y += point1.y;
			if (point.x > rect.right - 1)
				point.x = rect.right - 1;
			else if (point.x < rect.left + rect.Width() / 2)
				point.x = rect.left + rect.Width() / 2;
			if (point.y > rect.bottom - 1)
				point.y = rect.bottom - 1;
			else if (point.y < rect.top + rect.Height() / 2)
				point.y = rect.top + rect.Height() / 2;
			m_Roundness.x = 2 * (rect.right - point.x);
			m_Roundness.y = 2 * (rect.bottom - point.y);
		}
		pDoc->SetModifiedFlag(); 
		return;
	}   
	CRect rect = m_Rect;
	CDrawObj::MoveHandle(pView,nHandle, point1);
	if (rect == m_Rect) return;
    if (c_ToolShape == roundrecttool) return;
    double cx = 1.0 * m_Rect.Width() / m_pFocusObj->m_Rect.Width();
    double cy = 1.0 * m_Rect.Height() / m_pFocusObj->m_Rect.Height();
    CPoint point;
    point.x = m_pFocusObj->m_Rect.left - m_Rect.left;
    point.y = m_pFocusObj->m_Rect.top - m_Rect.top;
    m_Roundness.x = Round(m_pFocusObj->m_Roundness.x * fabs(cx));
    m_Roundness.y = Round(m_pFocusObj->m_Roundness.y * fabs(cy));
}

void CRoundRectObj::CreateFocusObj(CView* pView)
{
	m_pFocusObj = new CRoundRectObj(m_Rect,m_pParentObj,pDoc);
	m_pFocusObj->pDoc = pDoc;     
	m_pFocusObj->m_Roundness = m_Roundness;
	m_pFocusObj->m_TextRect = m_TextRect;
	m_pFocusObj->m_TextHoriAlignMode = m_TextHoriAlignMode;     
	m_pFocusObj->m_TextVertAlignMode = m_TextVertAlignMode;     
	m_pFocusObj->m_pFocusObj = this;
	m_pFocusObj->DrawFocus(pView);
}

void CRoundRectObj::DeleteFocusObj(CView* pView)
{
	ASSERT((m_pFocusObj != NULL) && (pView != NULL));
	if (m_pFocusObj == NULL) return;
	if (pView != NULL) m_pFocusObj->DrawFocus(pView);
 	delete m_pFocusObj;
	m_pFocusObj = NULL;
}   

void CRoundRectObj::DrawFocusObj(CView* pView)
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

void CRoundRectObj::DrawFocus(CView* pView)
{
	ASSERT(pView != NULL);
	if (pView == NULL) return;
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc,NULL);
	dc.SetROP2(R2_XORPEN);
    CPen m_Pen(PS_DOT,1,RGB(0,0,0));
    CPen *pOldPen = dc.SelectObject(&m_Pen);
    CBrush* pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	CRect rect = m_Rect;	
	pDoc->LPtoPP(rect);
	dc.RoundRect(rect,m_Roundness);
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
	dc.SetROP2(R2_COPYPEN);
	if (m_SelectMode == move) CTextObj::DrawFocus(pView);
}

void CRoundRectObj::SetRect(const CRect& rect)
{
	CPoint point;
	point.x = rect.left - m_Rect.left;
	point.y = rect.top - m_Rect.top;
	CTextObj::SetRect(rect);
}

void CRoundRectObj::OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point)
{
    if (m_bSelected)
    {
		if (c_ToolShape == roundrecttool)
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
				if (m_SelectMode != move) return;
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

void CRoundRectObj::OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point)
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
		m_Roundness = m_pFocusObj->m_Roundness;
		DeleteFocusObj(pView);     
		m_FocusRect.NormalizeRect();
		if (m_FocusRect.Width() < 5) m_FocusRect.right = m_FocusRect.left + 5;
		if (m_FocusRect.Height() < 5) m_FocusRect.bottom = m_FocusRect.top + 5;
		if ((m_Rect == m_FocusRect) && (nDragHandle<9))
		{
		}
		else if (m_SelectMode == move)
		{
	    	SetRect(m_FocusRect);
			if (m_pParentObj == NULL) pDoc->OnChildPosChange(this);
			else m_pParentObj->OnChildPosChange(this);
		}   
		else if (m_SelectMode == size)
		{
			CRect rect = m_Rect;
			m_Rect = m_FocusRect;
			if ((m_Rect.Width() < 5) && (m_Rect.Height() < 5))
				m_Rect.InflateRect(5,5);
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

void CRoundRectObj::OnMouseMove(CView* pView, UINT nFlags, const CPoint& point)
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

