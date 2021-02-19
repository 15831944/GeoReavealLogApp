/******************************************************************
*   OleDrw.cpp - implementation of the ole draw object class      *           
*******************************************************************/

#include "stdafx.h" 
#include "ImageMan.h"
#include "ImageManDoc.h"
#include "TrackDraw.h"
#include "CntrItem.h"
#include "OleDraw.h"            

IMPLEMENT_SERIAL(COleObj, CRemDrawObj, 0)

BOOL COleObj::c_bShowItems = TRUE;

COleObj::COleObj() : m_extent(0,0)
{
	m_pClientItem = NULL;
}

COleObj::COleObj(const CRect& rect,CDrawObj* pObj,CImageManDoc* pDoc)
	: CRemDrawObj(rect,pObj,pDoc), m_extent(0, 0)
{
	m_pClientItem = NULL;
}


COleObj::~COleObj()
{
}

void COleObj::Remove()
{
	if (m_pClientItem != NULL)
	{
		m_pClientItem->Delete();
		m_pClientItem = NULL;
	}
	CDrawObj::Remove();
}

void COleObj::Serialize( CArchive& ar )
{
	ASSERT_VALID(this);

	CDrawObj::Serialize(ar);

	if(!ar.IsStoring())
	{
		ar >> m_extent;
		ar >> m_pClientItem;
		m_pClientItem->m_pDrawObj = this;
	}
}

void COleObj::Draw(CView* pView, CDC* pDC)
{
	ASSERT_VALID(this);

	CRect rect = m_Rect;
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;

	CDrawItem* pItem = m_pClientItem;

	if (pItem != NULL)
	{
		// draw the OLE item itself
		pItem->Draw(pDC, rect);
	}
}

BOOL COleObj::Properties(CView* pView)
{
	AfxGetApp()->BeginWaitCursor();
	m_pClientItem->DoVerb(
		GetKeyState(VK_CONTROL) < 0 ? OLEIVERB_OPEN : OLEIVERB_PRIMARY,
		pView);
	AfxGetApp()->EndWaitCursor();
	return FALSE;
}

CDrawObj* COleObj::GetFocusObj()
{
	return m_pFocusObj;
}

void COleObj::CreateFocusObj(CView* pView)
{
	m_pFocusObj = new COleObj(m_Rect,m_pParentObj,pDoc);
	m_pFocusObj->pDoc = pDoc;     
	m_pFocusObj->DrawFocus(pView);
}

void COleObj::DeleteFocusObj(CView* pView)
{
	ASSERT((m_pFocusObj != NULL) && (pView != NULL));
	if (m_pFocusObj == NULL) return;
	if (pView != NULL) m_pFocusObj->DrawFocus(pView);
 	delete m_pFocusObj;
	m_pFocusObj = NULL;
}   

void COleObj::DrawFocusObj(CView* pView)
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

void COleObj::DrawFocus(CView* pView)
{
	ASSERT(pView != NULL);
	if (pView == NULL) return;
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc,NULL);
	dc.SetROP2(R2_XORPEN);
    CPen m_Pen(PS_DOT,1,RGB(0,0,0));
    CPen *pOldPen = dc.SelectObject(&m_Pen);
    CBrush* pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	dc.Rectangle(m_Rect);
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
	dc.SetROP2(R2_COPYPEN);
}   

void COleObj::OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point)
{
    if (m_bSelected)
    {
		{
			nDragHandle = HitTest(pView,point); 
			if (nDragHandle == 0) 
			{
				m_SelectMode = move;
				CRect rect = pDoc->GetRect();
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

void COleObj::OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point)
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
		if (m_SelectMode == move)
		{
	    	SetRect(m_pFocusObj->m_Rect);
			if (m_pClientItem->IsInPlaceActive())
				m_pClientItem->SetItemRects();
			DeleteFocusObj(pView);     
		}   
		else if (m_SelectMode == size)
		{
			m_Rect = m_pFocusObj->m_Rect;
			if ((m_Rect.Width() < 5) && (m_Rect.Height() < 5))
				m_Rect.InflateRect(5,5);
   			m_Rect.NormalizeRect();
			if (m_pClientItem->IsInPlaceActive())
				m_pClientItem->SetItemRects();
			DeleteFocusObj(pView);     
		}
	}
	CDrawObj::OnLButtonUp(pView,nFlags,point);
}

void COleObj::OnMouseMove(CView* pView, UINT nFlags, const CPoint& point)
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
	if(m_bSelected) 
	{
		DoScroll((CScrollView*)pView);
		CDrawObj::OnMouseMove(pView,nFlags,point);
	}
}
/////////////////////////////////////////////////////////////////////////////
