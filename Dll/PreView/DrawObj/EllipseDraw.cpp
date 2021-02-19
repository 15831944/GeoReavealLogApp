/******************************************************************
*   ElliDrw.cpp - implementation of the ellipse draw object class *           
*******************************************************************/

#include "stdafx.h" 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "TrackDraw.h"
#include "EllipseDraw.h"            
//#include "HeadPlotView.h"

IMPLEMENT_SERIAL(CEllipseObj, CTextObj, 0)

CEllipseObj::CEllipseObj()
{
	m_ShapeName = "Ellipse";
}

CEllipseObj::CEllipseObj(const CRect& rect,CDrawObj* pObj,CLogPlotDoc* pDocument) 
		: CTextObj(rect,pObj,pDocument)
{
	m_ShapeName = "Ellipse";
}

void CEllipseObj::Serialize(CArchive& ar)
{
	CTextObj::Serialize(ar);
}

void CEllipseObj::Draw(CView* pView,CDC* pDC)
{
	CRect rect = m_Rect;	
	pDoc->LPtoPP(rect);
	if (!pDC->RectVisible(rect)) return;
	CBrush br, *pOldBrush;
	if (m_LogBrush.lbStyle == BS_OWNER)
	{
		if(m_LogBrush.lbHatch >= 0) 
			FillEllipse(rect,(int)m_LogBrush.lbHatch,m_LogPen.lopnColor,rect.TopLeft(),pDC,m_LogBrush.lbColor);
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

	pDC->Ellipse(rect);

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	
	CTextObj::Draw(pView,pDC);
	CDrawObj::Draw(pView,pDC);
}

int CEllipseObj::HitTest(CView* pView,const CPoint& point)
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

	rgn2.CreateEllipticRgnIndirect(m_Rect);
    if (rgn.CombineRgn(&rgn1,&rgn2,RGN_AND) != NULLREGION) 
    {
	 	if (m_LogBrush.lbStyle != BS_NULL) return 0;
	 	if (!rgn.EqualRgn(&rgn1)) return 0;
    }
	return -1;
}

void CEllipseObj::CreateFocusObj(CView* pView)
{
	m_pFocusObj = new CEllipseObj(m_Rect,m_pParentObj,pDoc);
	m_pFocusObj->pDoc = pDoc;     
	m_pFocusObj->m_TextRect = m_TextRect;   
	m_pFocusObj->m_TextHoriAlignMode = m_TextHoriAlignMode;     
	m_pFocusObj->m_TextVertAlignMode = m_TextVertAlignMode;     
	m_pFocusObj->DrawFocus(pView);
}

void CEllipseObj::DeleteFocusObj(CView* pView)
{
	ASSERT((m_pFocusObj != NULL) && (pView != NULL));
	if (m_pFocusObj == NULL) return;
	if (pView != NULL) m_pFocusObj->DrawFocus(pView);
 	delete m_pFocusObj;
	m_pFocusObj = NULL;
}   

void CEllipseObj::DrawFocusObj(CView* pView)
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

void CEllipseObj::DrawFocus(CView* pView)
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
	dc.Ellipse(rect);
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
	dc.SetROP2(R2_COPYPEN);
	if (m_SelectMode == move) CTextObj::DrawFocus(pView);
}

void CEllipseObj::OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point)
{
    if (m_bSelected)
    {
		if (c_ToolShape == ellipsetool)
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

void CEllipseObj::OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point)
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
		DeleteFocusObj(pView);     
		m_FocusRect.NormalizeRect();
		if (m_FocusRect.Width() < 5) m_FocusRect.right = m_FocusRect.left + 5;
		if (m_FocusRect.Height() < 5) m_FocusRect.bottom = m_FocusRect.top + 5;
		if (m_Rect == m_FocusRect)
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

void CEllipseObj::OnMouseMove(CView* pView, UINT nFlags, const CPoint& point)
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
