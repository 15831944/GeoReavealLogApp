/******************************************************************
*   DrawObj.cpp - implementation of all draw object base class    *           
*******************************************************************/

#include "stdafx.h" 
#include "ImageMan.h"
#include "ImageManDoc.h"
#include "ImageManView.h"
#include "DrawObj.h" 
                                              
static UINT m_ObjectIndicators[] =
{
	ID_SEPARATOR,           // status line indicator
	IDS_OBJECT_POS,
	IDS_OBJECT_WH,
};

int CDrawObj::nDragHandle;
CPoint CDrawObj::c_down;
CPoint CDrawObj::c_last;
ToolShape CDrawObj::c_ToolShape = nonetool;
SelectMode CDrawObj::m_SelectMode = none; 
int CDrawObj::m_xMin;
int CDrawObj::m_xMax;
int CDrawObj::m_yMin;
int CDrawObj::m_yMax;

IMPLEMENT_SERIAL(CDrawObj, CObject, 0)
CDrawObj::CDrawObj()
{
	m_bSelected = FALSE;
	bEnableChildSizeMessage = TRUE;
	m_Rect.SetRect(0,0,0,0);
	m_pParentObj = NULL;
	m_ParentType = 0;
	m_pStatusBar = NULL;
	m_ShapeName = "Object";
}

CDrawObj::CDrawObj(const CRect& rect, CDrawObj* pParentObj,CImageManDoc* pDocument)
{
	m_bSelected = FALSE;
	bEnableChildSizeMessage = TRUE;
	m_Rect = rect;
	m_pParentObj = pParentObj;
	ASSERT(pDoc != NULL);
	pDoc = pDocument;
	m_pStatusBar = NULL;
	m_ShapeName = "Object";
}

CDrawObj::~CDrawObj()
{
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
		delete m_ObjList.GetNext(pos);
	m_ObjList.RemoveAll();
}

void CDrawObj::Remove()
{
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		pObj->Remove();
	}
	m_ObjList.RemoveAll();
	delete this;
}

void CDrawObj::OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point)
{
	c_down = point;                          
	c_last = point;
	pView->SetCapture();	
}

void CDrawObj::OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point)
{
	ReleaseCapture();	
    c_ToolShape = nonetool;
   	CImageManDoc* pDoc = (CImageManDoc*) pView->GetDocument();
   	if ((m_SelectMode == size) || (m_SelectMode == move))
   	{
	    m_SelectMode = none;
		if (c_last == c_down) return;
		POSITION pos = pDoc->GetFirstViewPosition();
		while (pos != NULL)
		{
			CView* pView1 = pDoc->GetNextView(pos);
			if (pView1->IsKindOf(RUNTIME_CLASS(CImageManView)))
				pView1->Invalidate(FALSE);
		}
  	}
    m_SelectMode = none;
}

void CDrawObj::OnMouseMove(CView* pView, UINT nFlags, const CPoint& point)
{   c_last = point;	}

void CDrawObj::OnLButtonDblClk(CView* pView, UINT nFlags, const CPoint& point)
{   Properties(pView);}

void CDrawObj::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	/*if(ar.IsStoring())
	{
		ar << m_Rect;
		ar << m_ParentType;
		m_ObjList.Serialize(ar);
	}
	else
	{
		pDoc = (CImageManDoc*)ar.m_pDocument;
		ASSERT_VALID(pDoc);
		ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CImageManDoc)));
		ar >> m_Rect;
		ar >> m_ParentType;
		m_ObjList.Serialize(ar);
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*) m_ObjList.GetNext(pos);
			if (pObj != NULL)
			{
		    	pObj->SetParentObj(this,pObj->GetParentType());
			} 
		}	
	}*/	
	if (ar.IsStoring())
	{
		ar << m_Rect;
		ar << m_ParentType;
		m_ObjList.Serialize(ar);
	}
	else
	{
		pDoc = (CImageManDoc*)ar.m_pDocument;
		ASSERT_VALID(pDoc);
		ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CImageManDoc)));
		ar >> m_Rect;
		ar >> m_ParentType;
		m_ObjList.Serialize(ar);
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*) m_ObjList.GetNext(pos);
			if (pObj != NULL)
			{
		    	pObj->SetParentObj(this,pObj->GetParentType());
			} 
		}	
	}
}

BOOL CDrawObj::Properties(CView* pView)
{
	return FALSE;
}

void CDrawObj::Move(const CPoint& point)
{
	ASSERT_VALID(this);           
	m_Rect += point;
}                             

void CDrawObj::SetRect(const CRect& rect)
{
	ASSERT_VALID(this);           
	m_Rect = rect;
}                             

CRect CDrawObj::GetRect()
{
	ASSERT_VALID(this);
	return m_Rect; 
}
        
CRect CDrawObj::GetMaxRect()
{
	CRect rect = m_Rect;
	CRect rect1;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if (pObj != NULL) 
		{
			rect1 = pObj->GetMaxRect();
			if (rect.left > rect1.left) rect.left = rect1.left;
			if (rect.right < rect1.right) rect.right = rect1.left;
			if (rect.top > rect1.top) rect.top = rect1.top;
			if (rect.bottom < rect1.bottom) rect.bottom = rect1.bottom;
		}
	}
	return rect;
}

void CDrawObj::OnChangeDepC(double depc, int flag)
{
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if (pObj != NULL) pObj->OnChangeDepC(depc, flag);
	}
}

void CDrawObj::DrawTracker(CView* pView, CDC* pDC, TrackerState state)
{
	ASSERT_VALID(this);
	if (!m_bSelected) return;
	switch (state)
	{
	case normal:
		break;

	case selected:
	case active:
		{
			int nHandleCount = GetHandleCount();
			for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
			{
				CPoint handle = GetHandlePoint(nHandle);
				pDoc->LPtoPP(&handle);
				pDC->PatBlt(handle.x - 3, handle.y - 3, 7, 7, DSTINVERT);
			}
		}
		break;
	}
}

CDrawObj* CDrawObj::ObjectAt(CView* pView,CPoint point)
{
	CRect rect;               
	CDrawObj* pObj1;
	if (HitTest(pView,point) >= 0) return this;
	POSITION pos = m_ObjList.GetTailPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_ObjList.GetPrev(pos);
		if (( pObj1 = pObj->ObjectAt(pView,point)) != NULL) return pObj1;
	}                  
	return NULL;
}

CDrawObj* CDrawObj::GetLocateObject(CDrawObj* pObj)
{
	ASSERT(pObj != NULL);
	CDrawObj* pObj1; 
	CDrawObj* pObj2;
	POSITION pos = m_ObjList.GetTailPosition();
	while (pos != NULL)
	{
		pObj1 = (CDrawObj*)m_ObjList.GetPrev(pos);
		if (pObj1 != NULL)
		{
			if ((pObj2 = pObj1->GetLocateObject(pObj)) != NULL) return pObj2;
		}	
	}          
	return NULL;        
}

BOOL CDrawObj::OnSetCursor(CView* pView,const CPoint& point)
{
	CRect rect;               
	int nHandle = HitTest(pView,point);
	if (nHandle>=0) 
	{
		SetCursor(GetHandleCursor(nHandle));
		return TRUE;
	}	
	POSITION pos = m_ObjList.GetTailPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_ObjList.GetPrev(pos);
		if (pObj->OnSetCursor(pView,point)) return TRUE;
	}                  
	return FALSE;
}

int CDrawObj::HitTest(CView* pView,const CPoint& point)
{
	ASSERT_VALID(this);
	ASSERT(pView != NULL);

	if (m_bSelected)
	{
		int nHandleCount = GetHandleCount();
		for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
		{
			CRect rc = GetHandleRect(pView,nHandle);
			if (rc.PtInRect(point)) return nHandle;
		}
	}
	if (m_Rect.PtInRect(point)) return 0;
	return -1;
}

BOOL CDrawObj::Intersects(const CRect& rect)
{
	ASSERT_VALID(this);
	CRect rectT = rect;
	rectT.NormalizeRect();
	return !(rectT & m_Rect).IsRectEmpty();
}   

int CDrawObj::GetHandleCount()
{
	ASSERT_VALID(this);
	return 8;
}

CPoint CDrawObj::GetHandlePoint(int nHandle)
{
	ASSERT_VALID(this);
	int x, y, xCenter, yCenter;

	// this gets the center regardless of left/right and top/bottom ordering
	xCenter = m_Rect.left + m_Rect.Width() / 2;
	yCenter = m_Rect.top + m_Rect.Height() / 2;

	switch (nHandle)
	{
	default:
		ASSERT(FALSE);

	case 1:
		x = m_Rect.left;
		y = m_Rect.top;
		break;

	case 2:
		x = xCenter;
		y = m_Rect.top;
		break;

	case 3:
		x = m_Rect.right;
		y = m_Rect.top;
		break;

	case 4:
		x = m_Rect.right;
		y = yCenter;
		break;

	case 5:
		x = m_Rect.right;
		y = m_Rect.bottom;
		break;

	case 6:
		x = xCenter;
		y = m_Rect.bottom;
		break;

	case 7:
		x = m_Rect.left;
		y = m_Rect.bottom;
		break;

	case 8:
		x = m_Rect.left;
		y = yCenter;
		break;
	}

	return CPoint(x, y);
}

CRect CDrawObj::GetHandleRect(CView* pView,int nHandleID)
{
	ASSERT_VALID(this);
	ASSERT(pView != NULL);

	CRect rect;
	CPoint point = GetHandlePoint(nHandleID);
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc); 
	rect.SetRect(point.x-3, point.y-3, point.x+3, point.y+3);
	return rect;
}

HCURSOR CDrawObj::GetHandleCursor(int nHandle)
{
	ASSERT_VALID(this);

	switch (nHandle)
	{
    case 0:
		return AfxGetApp()->LoadCursor(IDC_SELECT);
	case 8:
		return AfxGetApp()->LoadCursor(IDC_CURSOR_SIZEWE);
	case 4:
		return AfxGetApp()->LoadCursor(IDC_CURSOR_SIZEWE);
	default:
		return AfxGetApp()->LoadCursor(IDC_CURSOR_SIZENS);
	}
}

void CDrawObj::MoveHandle(CView* pView,int nHandle, const CPoint& point)
{
	ASSERT_VALID(this);
	if ((point.x == 0) && (point.y == 0 )) return;
    CRect rect = m_Rect;
	switch (nHandle)
	{
	default:
		ASSERT(FALSE);

	case 0:
		rect += point;             
		break;
	case 1:
		rect.left += point.x;
		rect.top += point.y;
		break;

	case 2:
		rect.top += point.y;
		break;

	case 3:
		rect.right += point.x;
		rect.top += point.y;
		break;

	case 4:
		rect.right += point.x;
		break;

	case 5:
		rect.right += point.x;
		rect.bottom += point.y;
		break;

	case 6:
		rect.bottom += point.y;
		break;

	case 7:
		rect.left += point.x;
		rect.bottom += point.y;
		break;

	case 8:
		rect.left += point.x;
		break;
	}                
    if (nHandle != 0)
    {                                                                  
    	CRect rect1;
		if (pView->IsKindOf(RUNTIME_CLASS(CImageManView)))
			rect1 = pDoc->GetRect(); 
		if (rect.left < rect1.left) rect.left = rect1.left;
		if (rect.left > rect1.right) rect.left = rect1.right;
		if (rect.right < rect1.left) rect.right = rect1.left;
		if (rect.right > rect1.right) rect.right = rect1.right;
		if (rect.top < rect1.top) rect.top = rect1.top;
		if (rect.top > rect1.bottom) rect.top = rect1.bottom;
		if (rect.bottom < rect1.top) rect.bottom = rect1.top;
		if (rect.bottom > rect1.bottom) rect.bottom = rect1.bottom;
    }
	if (m_Rect != rect)
		m_Rect = rect;
}

void CDrawObj::LPtoHIMM(CRect& rect)
{
	rect.left = (int) (rect.left * 2.54 + 0.5);
	rect.right = (int) (rect.right * 2.54 + 0.5);
	rect.top = (int) (rect.top * 2.54 + 0.5);
	rect.bottom = (int) (rect.bottom * 2.54 + 0.5);
}

void CDrawObj::LPtoHIMM(CPoint* point, int num)
{
	for (int i=0; i<num; i++)
	{
		point[i].x = (int) (point[i].x * 2.54 + 0.5);
		point[i].y = (int) (point[i].y * 2.54 + 0.5);
	}
}

void CDrawObj::InitStatusBar()
{
	CFrameWnd* pWnd = (CFrameWnd*) AfxGetApp()->m_pMainWnd;
	m_pStatusBar = (CStatusBar*) pWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
	if (m_pStatusBar == NULL) return;
	if (m_StatusBar.Create(pWnd) && 
		m_StatusBar.SetIndicators(m_ObjectIndicators,
		  sizeof(m_ObjectIndicators)/sizeof(UINT)))
	{
		m_pStatusBar->ShowWindow(FALSE);
		pWnd->RecalcLayout();
	}
	else m_pStatusBar = NULL;
	CString szHT, szHT1;
	szHT.LoadString(IDS_DRAGMOUSE);
	if (m_SelectMode == move)
	{
		szHT1.LoadString(IDS_LOCATION1);
		m_StatusBar.SetPaneText(0,szHT+m_ShapeName+szHT1);
	}
	else
	{
		szHT1.LoadString(IDS_SIZE);
		m_StatusBar.SetPaneText(0,szHT+m_ShapeName+szHT1);
	}
	int nSelectMode = m_SelectMode;
	m_SelectMode = none;
	CDrawObj::ShowStatus();
	m_SelectMode = (SelectMode) nSelectMode;
}

void CDrawObj::ReleaseStatusBar()
{
	if (m_StatusBar.m_hWnd != NULL) 
	{
		m_StatusBar.DestroyWindow();
		m_pStatusBar->ShowWindow(TRUE);
		((CFrameWnd*)(AfxGetApp()->m_pMainWnd))->RecalcLayout();
	}
}

void CDrawObj::ShowStatus()
{
	ShowStatusExt(m_Rect,nDragHandle);
}

void CDrawObj::ShowStatusExt(const CRect& rect1, int nHandle)
{
	CString text;
	if (m_StatusBar.m_hWnd == NULL) return;
	CRect rect = rect1;
	LPtoHIMM(rect);
	if (m_SelectMode != move)
	{
		char strWidth[10],strHeight[10];
		text.LoadString(IDS_WIDTHHEIGHT);;
		sprintf(strWidth,"%7.2f",rect.Width()*0.01);
		sprintf(strHeight,"%7.2f",rect.Height()*0.01);
		text = text + strWidth + "," + strHeight;
		m_StatusBar.SetPaneText(2,text);
	}
	if ((nHandle < 0) || (nHandle>8)) return;
	if (nHandle == 0) nHandle = 1;
	CPoint point = CDrawObj::GetHandlePoint(nHandle);
	LPtoHIMM(&point,1);
	char strX[20],strY[20];
	text.LoadString(IDS_LOCATION1);
	sprintf(strX,"%6.2f",point.x*0.01);
	sprintf(strY,"%6.2f",point.y*0.01);
	text = text + strX + "," + strY;
	m_StatusBar.SetPaneText(1,text);
}

void CDrawObj::Invalidate(CView* pView)
{
	ASSERT_VALID(this);
	CView* pView1 = pView;
	if (pView1 == NULL)
	{
		CMDIChildWnd* pWnd = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
		if (pWnd != NULL) pView1 = pWnd->GetActiveView();
		if (pView1 == NULL)
		{
			POSITION pos = pDoc->GetFirstViewPosition();
			while (pos != NULL)
			{
				CView* pView2 = pDoc->GetNextView(pos);
				InvalObj(pView2);
			}	
			return;
		}
	}
	if (pView1->IsKindOf(RUNTIME_CLASS(CImageManView)))
	{
		POSITION pos = pDoc->GetFirstViewPosition();
		while (pos != NULL)
		{
			CView* pView2 = pDoc->GetNextView(pos);
			if (pView2->IsKindOf(RUNTIME_CLASS(CImageManView)))
				InvalObj(pView2);
		}	
	}
	else
		InvalObj(pView1);
}                         


void CDrawObj::InvalObj(CView* pView)
{
	ASSERT(pView != NULL);
	CClientDC dc(pView);              
	pView->OnPrepareDC(&dc);
	CRect rect = m_Rect;
	rect.InflateRect(5,5);
	pDoc->LPtoPP(rect);
	dc.LPtoDP(rect);
	pView->InvalidateRect(rect,FALSE);
	pView->UpdateWindow();
}

CDrawObj* CDrawObj::GetParentObj()
{                                
	return m_pParentObj;
}                             

void CDrawObj::SetParentObj(CDrawObj* pParentObj,WORD nParentType)
{
	m_pParentObj = pParentObj;
	m_ParentType = nParentType;
}

void CDrawObj::DoScroll(CScrollView* pView)
{
    ASSERT(pView != NULL);
	CPoint point;
    GetCursorPos(&point);
	CPoint pt = point;
	pView->ScreenToClient(&pt);
	CRect rcClient; 
	pView->GetClientRect(&rcClient);
	CPoint ptScroll = pView->GetDeviceScrollPosition();
	CSize sizeTotal,sizePage,sizeLine;      
	int m_nMapMode;
	pView->GetDeviceScrollSizes(m_nMapMode,sizeTotal,sizePage,sizeLine);
	ASSERT((sizeTotal.cx >=0) && (sizeTotal.cy >=0));
	BOOL bScroll = FALSE;
	if ((rcClient.Height() < pt.y) && (ptScroll.y + rcClient.Height() < sizeTotal.cy -1))
	{
		ptScroll.y += 5;               
		if (ptScroll.y + rcClient.Height() > sizeTotal.cy) ptScroll.y = sizeTotal.cy - rcClient.Height();
		ASSERT(ptScroll.y > 0);
		bScroll = TRUE;
	}	
	else if ((pt.y < 0) && (ptScroll.y > 0))
	{
		ptScroll.y -= 5;               
		if (ptScroll.y < 0) ptScroll.y = 0;
		bScroll = TRUE;
	}                                            
	if ((rcClient.Width() < pt.x) && (ptScroll.x + rcClient.Width() < sizeTotal.cx -1))
	{
		ptScroll.x += 5;               
		if (ptScroll.x + rcClient.Width() > sizeTotal.cx) ptScroll.x = sizeTotal.cx - rcClient.Width();
		ASSERT(ptScroll.x > 0);
		bScroll = TRUE;
	}	
	else if ((pt.x < 0) && (ptScroll.x > 0))
	{
		ptScroll.x -= 5;
		if (ptScroll.x < 0) ptScroll.x = 0;
		bScroll = TRUE;
	}                                            
	if (bScroll)
	{
		if((c_ToolShape != foldtool)&&(c_ToolShape != polytool))
			DrawFocusObj(pView);
		int xOrig = pView->GetScrollPos(SB_HORZ);
		pView->SetScrollPos(SB_HORZ, ptScroll.x);
		int yOrig = pView->GetScrollPos(SB_VERT);
		pView->SetScrollPos(SB_VERT, ptScroll.y);
		pView->ScrollWindow(xOrig - ptScroll.x, yOrig - ptScroll.y);
		pView->UpdateWindow();
        SetCursorPos(point.x, point.y);
		if((c_ToolShape != foldtool)&&(c_ToolShape != polytool))
	       DrawFocusObj(pView);
	}
}

void CDrawObj::Draw(CView* pView,CDC* pDC)
{
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
 		if (pObj != NULL) pObj->Draw(pView,pDC);
	}	
}

void CDrawObj::DrawFill(CView* pView,CDC* pDC)
{
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
 		if (pObj != NULL) pObj->DrawFill(pView,pDC);
	}	
}

int CDrawObj::AddObj(CDrawObj* pObj,WORD nParentType)
{
	m_ObjList.AddTail(pObj);
	pObj->SetParentObj(this,nParentType);
	return 0;
}

int CDrawObj::RemoveObj(CDrawObj* pObj,int flag)
{
	POSITION pos = m_ObjList.Find(pObj,NULL);
	if (pos != NULL) 
	{                                        
		m_ObjList.RemoveAt(pos);
		return 0;
	}            
	return 1;
}       

int CDrawObj::MoveBack(CDrawObj* pObj)
{
	POSITION pos1; 
	POSITION pos = m_ObjList.Find(pObj,NULL);
	if (pos != NULL) 
	{
		pos1 = pos;
		m_ObjList.GetNext(pos1);
		if (pos1 != NULL)
		{
			m_ObjList.InsertAfter(pos1,pObj);
			m_ObjList.RemoveAt(pos);
			return 0;
		}
	}
	return 1;
}

int CDrawObj::MoveForward(CDrawObj* pObj)
{
	POSITION pos1; 
	POSITION pos = m_ObjList.Find(pObj,NULL);
	if (pos != NULL) 
	{
		pos1 = pos;
		m_ObjList.GetPrev(pos1);
		if (pos1 != NULL)
		{
			m_ObjList.InsertBefore(pos1,pObj);
			m_ObjList.RemoveAt(pos);
			return 0;
		}
	}
	return 1;
}

int CDrawObj::MoveToTail(CDrawObj* pObj)
{
	POSITION pos1; 
	POSITION pos = m_ObjList.Find(pObj,NULL);
	if (pos != NULL) 
	{
		pos1 = pos;
		m_ObjList.GetNext(pos1);
		if (pos1 != NULL)
		{
			m_ObjList.RemoveAt(pos);
			m_ObjList.AddTail(pObj);
			return 0;
		}
	}
	return 1;
}

int CDrawObj::MoveToHead(CDrawObj* pObj)
{
	POSITION pos1; 
	POSITION pos = m_ObjList.Find(pObj,NULL);
	if (pos != NULL) 
	{
		pos1 = pos;
		m_ObjList.GetPrev(pos1);
		if (pos1 != NULL)
		{
			m_ObjList.RemoveAt(pos);
			m_ObjList.AddHead(pObj);
			return 0;
		}
	}
	return 1;
}


void CDrawObj::dtos(double value, char* ss)
{
//	int i,j,k;
	sprintf(ss,"%g",value);
/*	k = strlen(ss);
	j = 0;
	for (i=0; i<k; i++)
		if (ss[i] != ' ')
		{
			ss[j] = ss[i];
			j++;
		}
	ss[j] = '\0';
	while (ss[j-1] == '0') j--;
	if (ss[j-1] == '.') j--;
	ss[j] = 0;*/
}

void CDrawObj::ZoomRect(const CRect& sRect, const CRect& zRect)
{
	if (sRect.Width() == 0)
	{
		m_Rect.left = zRect.left + m_Rect.left - sRect.left;
		m_Rect.right = zRect.left + m_Rect.right - sRect.left;
	}
	else
	{
		m_Rect.left = zRect.left + (m_Rect.left - sRect.left) * zRect.Width() / sRect.Width();
		m_Rect.right = zRect.left + (m_Rect.right - sRect.left) * zRect.Width() / sRect.Width();
	}
	if (sRect.Height() == 0)
	{
		m_Rect.top = zRect.top + m_Rect.top - sRect.top;
		m_Rect.bottom = zRect.top + m_Rect.bottom - sRect.top;
	}
	else
	{
		m_Rect.top = zRect.top + (m_Rect.top - sRect.top) * zRect.Height() / sRect.Height();
		m_Rect.bottom = zRect.top + (m_Rect.bottom - sRect.top) * zRect.Height() / sRect.Height();
	}
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
 		if (pObj != NULL) pObj->ZoomRect(sRect,zRect);
	}	
}

void CDrawObj::AlignToGrid(const CPoint& point, const CSize& size)
{
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
 		if (pObj != NULL) pObj->AlignToGrid(point,size);
	}	
}

BOOL CDrawObj::OnToolTip(CView* pView,CRect& rect,CString& text)
{
	POSITION pos = m_ObjList.GetTailPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_ObjList.GetPrev(pos);
		if (pObj->OnToolTip(pView,rect,text)) return TRUE;
	}                  
	return FALSE;
}

void CDrawObj::FindCurve(CObList* pList,CString strName)
{
	if (MatchCurve(strName))
		pList->AddTail(this);
	POSITION pos = m_ObjList.GetTailPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_ObjList.GetPrev(pos);
		pObj->FindCurve(pList,strName);	
	}                  
}

void CDrawObj::FindWell(CObList* pList,CString strName)
{
	if (MatchWell(strName))
		pList->AddTail(this);
	POSITION pos = m_ObjList.GetTailPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_ObjList.GetPrev(pos);
		pObj->FindWell(pList,strName);	
	}                  
}

void CDrawObj::InvalidateRect(CRect rect)
{
	CRect rect1;
	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = pDoc->GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CImageManView)))
		{
			CClientDC dc(pView);              
			pView->OnPrepareDC(&dc);
			rect1 = rect;
			pDoc->LPtoPP(&rect1);
			dc.LPtoDP(rect1);
			rect1.InflateRect(3,3);
			pView->InvalidateRect(rect1,FALSE);
			pView->UpdateWindow();
		}
	}	
}