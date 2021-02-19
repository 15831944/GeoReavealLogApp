/******************************************************************
*   FoldDrw.cpp - implementation of Foldline draw object class    *           
*******************************************************************/

#include "stdafx.h" 
#include "ImageMan.h"
#include "ImageManDoc.h"
#include "FIDIO.h"
#include "TrackDraw.h"
#include "ImageManView.h"
#include "FoldDraw.h"            

IMPLEMENT_SERIAL(CFoldObj, CRemDrawObj, 0)

CFoldObj::CFoldObj()
{
 	m_DrawShape = foldobj;
	m_ShapeName = "FoldLine";
 	bChangePoint = FALSE;
}

CFoldObj::~CFoldObj()
{
	if (m_Points != NULL)
		delete m_Points;
}

BOOL CFoldObj::Properties(CView* pView)
{
	CRemDrawObj::OnLine();
	return TRUE;
}

CFoldObj::CFoldObj(const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDocument)
	: CRemDrawObj(rect,pParentObj,pDocument)
{
 	m_DrawShape = foldobj;
	m_Points = NULL;
	m_nPoints = 0;
	m_nAllocPoints = 0;
	bChangePoint = FALSE;
	m_ShapeName = "FoldLine";
}

void CFoldObj::Serialize(CArchive& ar)
{
	CRemDrawObj::Serialize(ar);
}

void CFoldObj::SetSelect(CView* pView,BOOL bf)
{
	bChangePoint = FALSE;
	CRemDrawObj::SetSelect(pView,bf);
}

void CFoldObj::OnEditFold()
{
	if (!bChangePoint)
	{
		bChangePoint = TRUE;
		Invalidate();
	}	
}

void CFoldObj::OnLineArc()
{
	Invalidate();
}

void CFoldObj::AddPoint(const CPoint& point, CView* pView)
{
	ASSERT_VALID(this);
	if (m_nPoints == m_nAllocPoints)
	{
		CPoint* newPoints = NewPoints(m_nAllocPoints + 10);
		if (m_Points != NULL)
		{
			memcpy(newPoints, m_Points, sizeof(CPoint) * m_nAllocPoints);
			delete m_Points;
		}
		m_Points = newPoints;
		m_nAllocPoints ++;
	}

	if (m_nPoints == 0 || m_Points[m_nPoints - 1] != point)
	{
		m_Points[m_nPoints++] = point;
		RecalcBounds();
		InvalObj(pView);
	}	
}

CPoint* CFoldObj::NewPoints(int nPoints)
{
	return (CPoint*)new BYTE[nPoints * sizeof(CPoint)];
}

BOOL CFoldObj::RecalcBounds()
{
	ASSERT_VALID(this);

	if (m_nPoints == 0)
		return FALSE;

	CRect bounds(m_Points[0], CSize(0, 0));
	for (int i = 1; i < m_nPoints; ++i)
	{
		if (m_Points[i].x < bounds.left)
			bounds.left = m_Points[i].x;
		if (m_Points[i].x > bounds.right)
			bounds.right = m_Points[i].x;
		if (m_Points[i].y < bounds.top)
			bounds.top = m_Points[i].y;
		if (m_Points[i].y > bounds.bottom)
			bounds.bottom = m_Points[i].y;
	}                       
	m_Rect = bounds;
	return TRUE;
}

void CFoldObj::Draw(CView* pView,CDC* pDC)
{
	CPen pen(PS_SOLID,1,RGB(255,0,0));
	CPen *pOldPen=pDC->SelectObject(&pen);
	pDoc->LPtoPP(m_Points,m_nPoints);
	pDC->Polyline(m_Points, m_nPoints);
	pDoc->PPtoLP(m_Points,m_nPoints);
	pDC->SelectObject(pOldPen);
	CDrawObj::Draw(pView,pDC);
}

int CFoldObj::GetHandleCount()
{   
	if (bChangePoint) 
		return m_nPoints;
	return CRemDrawObj::GetHandleCount();
}            

CPoint CFoldObj::GetHandlePoint(int nHandle)
{
	ASSERT_VALID(this);
	if (bChangePoint) 
	{
		ASSERT(nHandle >= 1 && nHandle <= m_nPoints);
		return m_Points[nHandle - 1];
	}	
	return CRemDrawObj::GetHandlePoint(nHandle);
}

HCURSOR CFoldObj::GetHandleCursor(int nHandle)
{
	ASSERT_VALID(this);
	if (nHandle == 0) return AfxGetApp()->LoadCursor(IDC_SELECT);
	if (bChangePoint) return AfxGetApp()->LoadCursor(IDC_CURSOR_CROSS);
	return CRemDrawObj::GetHandleCursor(nHandle);
}   

int CFoldObj::HitTest(CView* pView,const CPoint& point)
{
	int nHandle = CRemDrawObj::HitTest(pView,point);
	if (nHandle > 0 ) return nHandle;
	CRgn rgn,rgn1,rgn2;
    rgn.CreateRectRgn(0,0,0,0);
	CRect rect (point.x-3,point.y-3,point.x+3,point.y+3);
    rgn1.CreateRectRgnIndirect(rect);

	rgn2.CreatePolygonRgn(m_Points,m_nPoints,ALTERNATE);
    if (rgn.CombineRgn(&rgn1,&rgn2,RGN_AND) != NULLREGION) 
    {
	 	if (m_LogBrush.lbStyle != BS_NULL) return 0;
	 	if (!rgn.EqualRgn(&rgn1)) return 0;
    }
	return -1;
}

void CFoldObj::MoveHandle(CView* pView,int nHandle, const CPoint& point)
{
	ASSERT_VALID(this);
	ASSERT_VALID(this);
	if ((point.x == 0) && (point.y == 0))
		return;            
	if ((m_SelectMode == size) && (!bChangePoint))
	{	
		CRemDrawObj::MoveHandle(pView,nHandle,point);
		return;
	}	
	if (nHandle == 0)
	{
		for (int i = 0; i < m_nPoints; i += 1)
		{
			m_Points[i].x += point.x;
			m_Points[i].y += point.y;
		}
		m_Rect += point;
	}	
	else 
	{
		ASSERT(nHandle >= 0 && nHandle <= m_nPoints);
	  	CPoint point1 = m_Points[nHandle-1];
		point1.x += point.x;
		point1.y += point.y;
		CRect rect;
		if (pView->IsKindOf(RUNTIME_CLASS(CImageManView)))
			rect = pDoc->GetRect(); 
		if (!rect.PtInRect(point1)) return;
	  	m_Points[nHandle-1] = point1;
		RecalcBounds();
	}  	               
}

void CFoldObj::InitStatusBar()
{
	CDrawObj::InitStatusBar();
	if (m_StatusBar.m_hWnd == NULL) return;
	if ((!bChangePoint) || (m_SelectMode == move)) return;
	CString szHT, szHT1;
	szHT.LoadString(IDS_ADDPOINT);
	szHT1.LoadString(IDS_EDITPOINT);
	if (c_ToolShape == foldtool)
	{
		m_StatusBar.SetPaneText(0,szHT);
	}
	ShowStatus();
}

void CFoldObj::ShowStatus()
{
	if (m_StatusBar.m_hWnd == NULL) return;
	if ((!bChangePoint) || (m_SelectMode == move)) 
	{
		CDrawObj::ShowStatus();
		return;
	}
	
	char strX[20],strY[20];
	int nHandle;
	if (c_ToolShape == foldtool) nHandle = m_nPoints;
	else nHandle = nDragHandle;
	
	CString text;
	if (c_ToolShape == foldtool) text.LoadString(IDS_NUMBER);
	else text.LoadString(IDS_NO);
	sprintf(strX,"%3i",nHandle);
	text = text + strX;
	m_StatusBar.SetPaneText(1,text);
	
	CPoint point = GetHandlePoint(nHandle);
	LPtoHIMM(&point,1);

	sprintf(strX,"%7.2f",point.x*0.01);
	sprintf(strY,"%7.2f",point.y*0.01);
	text.LoadString(IDS_LOCATION1);
	text = text + strX + "," + strY;
	m_StatusBar.SetPaneText(2,text);
}

void CFoldObj::CreateFocusObj(CView* pView)
{
	m_pFocusObj = new CFoldObj(m_Rect,m_pParentObj,pDoc);
	m_pFocusObj->pDoc = pDoc;     
	m_pFocusObj->bChangePoint = bChangePoint;     
	if ((bChangePoint) || (m_SelectMode == move))
	{
		m_pFocusObj->m_Points = NewPoints(m_nAllocPoints);
		memcpy(m_pFocusObj->m_Points, m_Points, sizeof(CPoint) * m_nPoints);
		m_pFocusObj->m_nAllocPoints = m_nAllocPoints;
		m_pFocusObj->m_nPoints = m_nPoints;
	}
	m_pFocusObj->DrawFocus(pView);
}

void CFoldObj::SetRect(const CRect& rect)
{
	int x=0,y=0;
	CPoint point;
	point.x = rect.left - m_Rect.left;
	point.y = rect.top - m_Rect.top;
	CRect m_Rect1 = m_Rect;
	if ((rect.Width() != m_Rect.Width()) && (m_Rect.Width() != 0))
    {
	    x = 1;
	    for (int i=0; i<m_nPoints; i++)
    		m_Points[i].x = rect.left + Round(((double)rect.Width()) / m_Rect.Width() * (m_Points[i].x - m_Rect.left));
    }       
    else if (point.x != 0)
    {
 	    for (int i=0; i<m_nPoints; i++)
    		m_Points[i].x += point.x;
   	}                  
	if ((rect.Height() != m_Rect.Height()) && (m_Rect.Width() != 0))
    {
	    y = 1;
	    for (int i=0; i<m_nPoints; i++)
    		m_Points[i].y = rect.top + Round(((double)rect.Height()) / m_Rect.Height() * (m_Points[i].y - m_Rect.top));
    }                         
    else if (point.y != 0) 
    {
  	    for (int i=0; i<m_nPoints; i++)
    		m_Points[i].y += point.y;
    }
    m_Rect = m_Rect1;
    if (m_SelectMode == move) CRemDrawObj::SetRect(rect);
	if ((x != 0) || (y != 0)) RecalcBounds();
}

void CFoldObj::DeleteFocusObj(CView* pView)
{
	ASSERT((m_pFocusObj != NULL) && (pView != NULL));
	if (m_pFocusObj == NULL) return;
	if ((pView != NULL) && (m_pFocusObj->m_nPoints > 1)) 
		m_pFocusObj->DrawFocus(pView);
	delete m_pFocusObj;
	m_pFocusObj = NULL;
}   

void CFoldObj::DrawFocusObj(CView* pView)
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

void CFoldObj::DrawFocus(CView* pView)
{
	ASSERT((m_pFocusObj != NULL) && (pView != NULL));
	if (pView == NULL) return;
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc,NULL);
	CRect rect = m_Rect;
	pDoc->LPtoPP(rect);
	if (!dc.RectVisible(rect)) return;
	dc.SetROP2(R2_XORPEN);
	CPen Pen(PS_DOT,1,RGB(0,0,0));
	CPen *pOldPen= dc.SelectObject(&Pen);	
	CBrush* pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);	
	if(!bChangePoint)
		dc.Rectangle(rect);
	pDoc->LPtoPP(m_Points,m_nPoints);
	dc.Polyline(m_Points, m_nPoints);
	pDoc->PPtoLP(m_Points,m_nPoints);
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
	dc.SetROP2(R2_COPYPEN);
}   

void CFoldObj::OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point)
{
    if (c_ToolShape == foldtool) 
    {
		CDrawObj::OnLButtonDown(pView,nFlags,point);
		CPoint point1 = point;
    	if (m_nPoints == 0)
		{
			AddPoint(point,pView);
			m_SelectMode = size;
			bChangePoint = TRUE;
		}
    	else if (point == m_Points[0])
    	{
			ReleaseCapture();
			bChangePoint = FALSE;
		    c_ToolShape = nonetool;
    		m_SelectMode = none;
			m_nPoints -= 1;
			if (m_nPoints < 2) 
			{
		   		point1.x += 1;
   				AddPoint(point1,pView);
			}	
			Invalidate(pView);
			ReleaseStatusBar();
			return;
		}	
   		point1.x += 1;
   		c_last = point1;
   		AddPoint(point1,pView);
		nDragHandle = m_nPoints;
		if (m_nPoints == 2) InitStatusBar();
		return;
    }	
    if (m_bSelected)
    {
		nDragHandle = HitTest(pView,point); 
		if (nDragHandle == 0) 
		{
			m_SelectMode = move;
			CRect rect;
			if (pView->IsKindOf(RUNTIME_CLASS(CImageManView)))
				rect = pDoc->GetRect(); 
			m_xMin = rect.left;	m_xMax = rect.right;
			m_yMin = rect.top;  m_yMax = rect.bottom;	
		}	
		else if (nDragHandle > 0) m_SelectMode = size; 
		else return;
		SetCursor(GetHandleCursor(nDragHandle));
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
	if (m_bSelected) m_pFocusObj->InitStatusBar();
}

void CFoldObj::OnLButtonDblClk(CView* pView, UINT nFlags, const CPoint& point)
{                 
    if (c_ToolShape == foldtool) 
	{
		ReleaseStatusBar();
		ReleaseCapture();
		bChangePoint = FALSE;
	    c_ToolShape = nonetool;
   		m_SelectMode = none;
		int i = m_nPoints;
		if (i > 2 &&
			(m_Points[i-1] == m_Points[i-2] ||
			 m_Points[i-1].x - 1 == m_Points[i-2].x &&
			 m_Points[i-1].y == m_Points[i-2].y))
			m_nPoints -= 1;
		Invalidate(pView);
		return;	
	}
}

void CFoldObj::OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point)
{
    if (c_ToolShape == foldtool) return;
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
		m_pFocusObj->ReleaseStatusBar();
		CRect m_FocusRect = m_pFocusObj->m_Rect;
		if (c_last == c_down)
		{
			DeleteFocusObj(pView);     
		}
		else if (m_SelectMode == move)
		{
			DeleteFocusObj(pView);     
	    	SetRect(m_FocusRect);
			if (m_pParentObj == NULL) pDoc->OnChildPosChange(this);
			else m_pParentObj->OnChildPosChange(this);
		}   
		else if (m_SelectMode == size)
		{
			CRect rect = m_Rect;
			if (bChangePoint)
			{	
				m_pFocusObj->DrawFocus(pView);
				m_Rect = m_pFocusObj->m_Rect;
    			m_Rect.NormalizeRect();
				m_nAllocPoints = m_pFocusObj->m_nAllocPoints;
				m_nPoints = m_pFocusObj->m_nPoints ;
				delete m_Points;
				m_Points = m_pFocusObj->m_Points;
				m_pFocusObj->m_Points = NULL;
				m_pFocusObj->m_nPoints = 0;
				DeleteFocusObj(pView);   
			}	                             
			else
			{
				DeleteFocusObj(pView);   
				SetRect(m_FocusRect);
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
	CDrawObj::OnLButtonUp(pView,nFlags,point);
}

void CFoldObj::OnMouseMove(CView* pView, UINT nFlags, const CPoint& point)
{       
	if (point == c_last) return;
	CPoint delta = (CPoint)(point - c_last);
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
		m_pFocusObj->DrawFocus(pView);
		m_pFocusObj->MoveHandle(pView,nDragHandle, delta);
		m_pFocusObj->DrawFocus(pView);
	}	
    if ((c_ToolShape == polytool)||(c_ToolShape == foldtool))
    {
		if ((nFlags & MK_LBUTTON) != 0)
		{
			if (pDoc->GetRect().PtInRect(point))	
				AddPoint(point,pView);
		}	
		else                  
		{
		  	CPoint point1 = m_Points[m_nPoints-1];
			point1.x += delta.x;
			point1.y += delta.y;
			if (pDoc->GetRect().PtInRect(point1))
			{
			  	m_Points[m_nPoints-1] = point1;
				RecalcBounds();
				InvalObj(pView);
			}	
		}	
	}	
	else if (m_SelectMode == size)
	{
		m_pFocusObj->DrawFocus(pView);
		m_pFocusObj->MoveHandle(pView,nDragHandle, delta);
		m_pFocusObj->DrawFocus(pView);
	}
	if (m_bSelected) 
	{
		DoScroll((CScrollView*)pView);
		CDrawObj::OnMouseMove(pView,nFlags,point);
		if ((c_ToolShape == polytool)||(c_ToolShape == foldtool))
			ShowStatus();
		else m_pFocusObj->ShowStatus();
	}
}

void CFoldObj::DrawCopy(CView* pView,const CPoint& point)
{
	int i;
	if(c_ToolShape == foldtool) 
	{
		ReleaseStatusBar();
		ReleaseCapture();
		bChangePoint = FALSE;
	    c_ToolShape = nonetool;
  		m_SelectMode = none;
		i = m_nPoints;
		if (i > 2 &&
			(m_Points[i-1] == m_Points[i-2] ||
			 m_Points[i-1].x - 1 == m_Points[i-2].x &&
			 m_Points[i-1].y == m_Points[i-2].y))
			m_nPoints -= 1;
		Invalidate(pView);
	}
}

void CFoldObj::AddPartPoint(const CPoint& point)
{
	ASSERT_VALID(this);
	if (m_nPoints == m_nAllocPoints)
	{
		CPoint* newPoints = NewPoints(m_nAllocPoints + 10);
		if (m_Points != NULL)
		{
			memcpy(newPoints, m_Points, sizeof(CPoint) * m_nAllocPoints);
			delete m_Points;
		}
		m_Points = newPoints;
		m_nAllocPoints ++;
	}

	if (m_nPoints == 0 || m_Points[m_nPoints - 1] != point)
	{
		m_Points[m_nPoints++] = point;
		RecalcBounds();
	}	
}