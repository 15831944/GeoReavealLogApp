/******************************************************************
*   RectDrw.cpp - implementation of Arrow draw object class   *           
*******************************************************************/

#include "stdafx.h" 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "TrackDraw.h"
#include "ArrowDraw.h"
#include "CharacterDraw.h"            
//#include "HeadPlotView.h"

IMPLEMENT_SERIAL(CArrowObj, CTextObj, 0)

CArrowObj::CArrowObj()
{
	m_ShapeName = "Arrow";
	m_LogPen.lopnWidth.x=2;
	m_LogPen.lopnWidth.y=2;
	m_bRotate = TRUE;
	m_TextVertAlignMode = IDM_CENTER;
	m_TextHoriAlignMode = IDM_RIGHT;
}

CArrowObj::CArrowObj(const CRect& rect,CDrawObj* pObj,CLogPlotDoc* pDocument) 
		: CTextObj(rect,pObj,pDocument)
{
	m_ShapeName = "Arrow";
	m_LogPen.lopnWidth.x=2;
	m_LogPen.lopnWidth.y=2;
	m_bRotate = TRUE;
	m_TextVertAlignMode = IDM_CENTER;
	m_TextHoriAlignMode = IDM_RIGHT;
}

void CArrowObj::Serialize(CArchive& ar)
{
	CTextObj::Serialize(ar);
}

void CArrowObj::Draw(CView* pView,CDC* pDC)
{
	CRect rect = m_Rect;	
	pDoc->LPtoPP(rect);
	if (!pDC->RectVisible(rect)) return;
	CBrush br, *pOldBrush;
	if (m_LogBrush.lbStyle == BS_OWNER)
	{
		if (m_LogBrush.lbHatch >= 0) 
			LithFillBox(rect,(int)m_LogBrush.lbHatch,m_LogBrush.lbColor,rect.TopLeft(),pDC);
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

/*	pDC->MoveTo(rect.TopLeft()); 
	pDC->LineTo(rect.left+(int)(rect.Width()*0.67),rect.top);
	pDC->MoveTo(rect.right-rect.Width()/3,rect.bottom); 
	pDC->LineTo(rect.left,rect.bottom);
	pDC->MoveTo(rect.left+rect.Width()/3,rect.top); 
	pDC->LineTo(rect.left+rect.Width()/3,rect.bottom);

	pDC->MoveTo(rect.left+rect.Width()/3-4,rect.top+5); 
	pDC->LineTo(rect.left+rect.Width()/3,rect.top+1);
	pDC->LineTo(rect.left+rect.Width()/3+5,rect.top+6);

	pDC->MoveTo(rect.left+rect.Width()/3-4,rect.bottom-5); 
	pDC->LineTo(rect.left+rect.Width()/3,rect.bottom-1);
	pDC->LineTo(rect.left+rect.Width()/3+5,rect.bottom-6);*/

	CPoint point1 = rect.TopLeft();
	CPoint point2 = rect.BottomRight ();
	pDC->MoveTo (point1);pDC->LineTo (point2.x-10,point1.y);
	pDC->MoveTo (point2.x-10,point2.y);pDC->LineTo (point1.x,point2.y);
	int HalfX=point1.x+(point2.x-point1.x)/2-5;
	int PenWidth=m_LogPen.lopnWidth.y ; 
	pDC->MoveTo (HalfX,point1.y);pDC->LineTo (HalfX,point2.y);
	pDC->MoveTo (HalfX-PenWidth-1,point1.y+PenWidth+3);pDC->LineTo (HalfX+2,point1.y+PenWidth+3);
	pDC->MoveTo (HalfX-PenWidth-1,point2.y-PenWidth-2);pDC->LineTo (HalfX+2,point2.y-PenWidth-2);

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
 	
	CTextObj::Draw(pView,pDC);
	CDrawObj::Draw(pView,pDC);
}


int CArrowObj::HitTest(CView* pView,const CPoint& point)
{
	ASSERT_VALID(this);
	ASSERT(pView != NULL);
	int nHandle =  CTextObj::HitTest(pView,point);
	if (nHandle >=0 ) return nHandle;
	//if ((pView->IsKindOf(RUNTIME_CLASS(CHeadPlotView))) && (!((CHeadPlotView*)pView)->m_bEditEnable)) return -1;
	CRgn rgn,rgn1,rgn2;
	CRect rect (point.x-3,point.y-3,point.x+3,point.y+3);
	rgn1.CreateRectRgnIndirect(rect);
	CRect rect1 = m_Rect;
	if (bInput || (m_bSelected && (!IsTextEmpty()))) 
		rect1.InflateRect(3,3);
    rgn2.CreateRectRgnIndirect(rect1);
    rgn.CreateRectRgn(0,0,0,0);
    if (rgn.CombineRgn(&rgn1,&rgn2,RGN_AND) != NULLREGION) 
    {
	 	if (m_LogBrush.lbStyle != BS_NULL) return 0;
	 	if (!rgn.EqualRgn(&rgn1)) return 0;
    }                          
    return -1;
}

void CArrowObj::CreateFocusObj(CView* pView)
{
	m_pFocusObj = new CArrowObj(m_Rect,m_pParentObj,pDoc);
	m_pFocusObj->pDoc = pDoc;     
	m_pFocusObj->m_TextRect = m_TextRect;     
	m_pFocusObj->m_TextHoriAlignMode = m_TextHoriAlignMode;     
	m_pFocusObj->m_TextVertAlignMode = m_TextVertAlignMode;     
	m_pFocusObj->DrawFocus(pView);
}

void CArrowObj::DeleteFocusObj(CView* pView)
{
	ASSERT((m_pFocusObj != NULL) && (pView != NULL));
	if (m_pFocusObj == NULL) return;
	if (pView != NULL) m_pFocusObj->DrawFocus(pView);
 	delete m_pFocusObj;
	m_pFocusObj = NULL;
}   

void CArrowObj::DrawFocusObj(CView* pView)
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

void CArrowObj::DrawFocus(CView* pView)
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
	dc.Rectangle(rect);
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
	dc.SetROP2(R2_COPYPEN);
	if (m_SelectMode == move) CTextObj::DrawFocus(pView);
}   

void CArrowObj::OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point)
{
    if (m_bSelected)
    {
		
		if (c_ToolShape == arrowtool)
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

void CArrowObj::OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point)
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

void CArrowObj::OnMouseMove(CView* pView, UINT nFlags, const CPoint& point)
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

