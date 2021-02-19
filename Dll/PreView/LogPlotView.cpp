// LogPlotView.cpp : 实现文件
//
#include "stdafx.h"
#include "GeoFIDio.h"
#include "PreView.h"
#include "LogPlotDoc.h"
#include "LogPlotView.h"
#include "CntrItem.h"
#include "TextDraw.h"
#include "RoundRectDraw.h"
#include "RectangleDraw.h"
#include "CharacterDraw.h"
#include "EllipseDraw.h"
#include "RectArrowDraw.h"
#include "LineDraw.h"
#include "ArrowDraw.h"
#include "PolyDraw.h"
#include "FoldDraw.h"
#include "OleDraw.h"
#include "TrackDraw.h"
#include "FIDDraw.h"
#include "CurveDraw.h"
#include "CoreDraw.h"
#include "ResultDraw.h"
#include "LthDraw.h"
#include "LthNoteDraw.h"
#include "AngDraw.h"
#include "AzimFreq.h"
#include "DevDazDraw.h"
#include "WaveDraw.h"
#include "EchoDraw.h"
#include "ArrayWaveDraw.h"
#include "ImageDraw.h"
#include "Image3D.h"
#include "Curve3D.h"
#include "StratumDraw.h"
#include "ResultListDraw.h"
#include "MarkVectorDraw.h"
#include "MarkFreq.h"
#include "ImageCurveDraw.h"
#include "YieldDraw.h"
#include "PoleDraw.h"
#include "DSCDraw.h"
#include "CCLDraw.h"
#include "MakResultDraw.h"
#include "GrDenResultDraw.h"
#include "PickupHoleDraw.h"
#include "DepthDialog.h"
#include "ProgDlg.h"
#include "DepSect.h"
#include "DirDialog.h"
#include <afxadv.h>            // For CSharedFile
#include <direct.h>


IMPLEMENT_DYNCREATE(CLogPlotView, CScrollView)

CLogPlotView::CLogPlotView()
{
	m_pSelectObj = NULL;
	bObjectLink = FALSE;
	m_LeftSpace = m_TopSpace = 20;
	bFirstUpdate = TRUE;
	m_pDoc = NULL;
	CDrawObj::c_ToolShape = nonetool;
	m_bActive=TRUE;  //可交互操作
}

CLogPlotView::~CLogPlotView()
{
}

BEGIN_MESSAGE_MAP(CLogPlotView, CScrollView)
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_DROPFILES()
	ON_WM_MOUSEWHEEL()
	ON_WM_CONTEXTMENU()

	ON_COMMAND(ID_SETUP_PAGE, OnSetupPage)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_OBJECT_PROPTERTIES, OnObjectPropterties)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_PROPTERTIES, OnUpdateObjectPropterties)
	
	ON_COMMAND(ID_FILE_SAVE_BMP,SaveDocToBmpfile)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_BMP, OnUpdateDocSaveBmp)
	ON_COMMAND(ID_FILE_SAVE_DATA,SaveDocToDatafile)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_DATA, OnUpdateDocSaveData)

	ON_COMMAND(ID_SET_DEPTH, OnSetDepth)
	ON_UPDATE_COMMAND_UI(ID_SET_DEPTH, OnUpdateSetDepth)
	ON_COMMAND(ID_DOCDATA_UPDATE, OnDocdataUpdate)
	ON_COMMAND(ID_MATCH_ALLOBJECT, OnMatchAllobject)
	ON_COMMAND(ID_ADD_CURVE, OnAddCurve)
	ON_UPDATE_COMMAND_UI(ID_ADD_CURVE, OnUpdateAddCurve)
	ON_COMMAND(ID_ADD_TRACK, OnAddTrack)
	ON_UPDATE_COMMAND_UI(ID_ADD_TRACK, OnUpdateAddTrack)
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()


// CLogPlotView 绘图

void CLogPlotView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	if(m_pDoc==NULL) return;
	CRect rect(100,100,150,150);
	m_tooltip.Create(this);
	m_tooltip.AddTool(this,"0.1234",rect,ID_CURVE_VALUE);
	m_tooltip.Activate(FALSE);

	CSize sizeTotal(100,100);
	CClientDC dc(this);
	if (m_pDoc->m_ScaleNo != 0)
	{
		if (m_pDoc->m_ScaleFactor < 0)
		{
			sizeTotal.cx = MulDiv(m_pDoc->m_Rect.Width()+2*m_LeftSpace,dc.GetDeviceCaps(LOGPIXELSX),-m_pDoc->m_ScaleFactor);
			sizeTotal.cy = MulDiv(m_pDoc->m_Rect.Height()+2*m_TopSpace,dc.GetDeviceCaps(LOGPIXELSY),-m_pDoc->m_ScaleFactor);
		}
		else
		{
			sizeTotal.cx = MulDiv(m_pDoc->m_Rect.Width()+2*m_LeftSpace,dc.GetDeviceCaps(LOGPIXELSX)*m_pDoc->m_ScaleFactor,10000);
			sizeTotal.cy = MulDiv(m_pDoc->m_Rect.Height()+2*m_TopSpace,dc.GetDeviceCaps(LOGPIXELSY)*m_pDoc->m_ScaleFactor,10000);
		}
	}
	else
	{
		m_LeftSpace = 10;
		m_TopSpace = 10;
		sizeTotal.cx = rect.right;
		sizeTotal.cy = MulDiv(m_pDoc->m_Rect.Height()+2*m_TopSpace,rect.right,m_pDoc->m_Rect.Width()+2*m_LeftSpace);
	}
	SetScrollSizes(MM_TEXT,sizeTotal);
}

void CLogPlotView::OnDraw(CDC* pDC)
{
	// TODO: 在此添加绘制代码
	if(m_pDoc == NULL)
		return;
	CDC m_MemDC;
	CDC* pDrawDC = pDC;
	CBitmap bitmap;
	CBitmap* pOldBitmap;
	
	CRect rect,m_ClipRect;
    pDC->GetClipBox(&rect);        
	rect.InflateRect(5,5);
	m_ClipRect = rect;
    pDC->LPtoDP(&rect);

	// draw to offscreen bitmap for fast looking repaints
	if(m_MemDC.CreateCompatibleDC(pDC))
	{
		if (bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height()))
		{
			OnPrepareDC(&m_MemDC, NULL);
			pDrawDC = &m_MemDC;
            m_MemDC.OffsetViewportOrg(-rect.left, -rect.top);
			pOldBitmap = m_MemDC.SelectObject(&bitmap);
            m_MemDC.SetBrushOrg(rect.left % 8, rect.top % 8);
            m_MemDC.IntersectClipRect(m_ClipRect);
		}
	}
    CBrush brush;
	if (!brush.CreateSolidBrush(0xC0C0C0)) return;//0xAE9990
	brush.UnrealizeObject();     
	pDrawDC->FillRect(m_ClipRect , &brush);	

    brush.DeleteObject();
	if (!brush.CreateSolidBrush(m_pDoc->m_paperColor)) return;
	brush.UnrealizeObject();     
	CRect m_Rect = m_pDoc->GetRect();
	m_pDoc->LPtoPP(m_Rect);
	pDrawDC->FillRect(m_ClipRect & m_Rect , &brush);

	CPen m_Pen;
    m_Pen.CreatePen(PS_SOLID,1,RGB(0,0,0));
   	CPen *pOldPen = pDrawDC->SelectObject(&m_Pen);
    CBrush* pOldBrush = (CBrush*)pDrawDC->SelectStockObject(NULL_BRUSH);
	pDrawDC->Rectangle(m_Rect);           
	pDrawDC->SelectObject(pOldPen);
	m_Pen.DeleteObject();                              
	
	m_Pen.CreatePen(PS_SOLID,2,RGB(0,0,0));
   	pDrawDC->SelectObject(&m_Pen);
   	pDrawDC->MoveTo(m_Rect.left+3,m_Rect.bottom+1);
   	pDrawDC->LineTo(m_Rect.right,m_Rect.bottom+1);
   	pDrawDC->MoveTo(m_Rect.right+1,m_Rect.top+3);
   	pDrawDC->LineTo(m_Rect.right+1,m_Rect.bottom+1);
	pDrawDC->SelectObject(pOldPen);
	m_Pen.DeleteObject();                              
	
	m_Pen.CreatePen(PS_DOT,1,RGB(0,0,0));
   	pDrawDC->SelectObject(&m_Pen);
	CRect rect1 = m_pDoc->m_TrackRect;
	m_pDoc->LPtoPP(rect1);
	rect1.InflateRect(4,4);
	pDrawDC->Rectangle(rect1);           
	pDrawDC->SelectObject(pOldPen);
	pDrawDC->SelectObject(pOldBrush);

	m_ClipRect &= m_Rect;
	pDrawDC->IntersectClipRect(m_ClipRect);
	m_pDoc->Draw(this,pDrawDC);
	
	if(m_pSelectObj != NULL)
	{
		m_pSelectObj->Draw(this,pDrawDC);
		if ((CDrawObj::m_SelectMode != size) && (CDrawObj::m_SelectMode != move))
			m_pSelectObj->DrawTracker(this,pDrawDC,selected);
	}
	if(pDrawDC != pDC)
	{
		pDC->SetViewportOrg(0,0);
		pDC->SetWindowOrg(0,0);
		pDC->SetMapMode(MM_TEXT);
		m_MemDC.SetViewportOrg(0, 0);
		m_MemDC.SetWindowOrg(0,0);
		m_MemDC.SetMapMode(MM_TEXT);
		pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),
			&m_MemDC, 0, 0, SRCCOPY);
		m_MemDC.SelectObject(pOldBitmap);
	}
}

BOOL CLogPlotView::PreCreateWindow(CREATESTRUCT& cs)
{
	ASSERT(cs.style & WS_CHILD);
	if (cs.lpszClass == NULL)
		cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS);
	return TRUE;
}

void CLogPlotView::OnActivateView(BOOL bActivate, CView* pActiveView,
	CView* pDeactiveView)
{
	CView::OnActivateView(bActivate, pActiveView, pDeactiveView);

	// invalidate selections when active status changes
	if (m_bActive != bActivate)
	{
		if (bActivate)  // if becoming active update as if active
			m_bActive = bActivate;
		
		if ((m_pSelectObj != NULL) && 
			((CDrawObj::c_ToolShape == polytool)||(CDrawObj::c_ToolShape == foldtool)))
		{
			CPoint point;
		    GetCursorPos(&point);
			ScreenToClient(&point);
			CClientDC dc(this);
			OnPrepareDC(&dc,NULL);
			dc.DPtoLP(&point);
			m_pDoc->PPtoLP(&point);
			m_pSelectObj->OnLButtonDblClk(this,0,point);
		}
		DeSelectObj();
		m_bActive = bActivate;
	}
}

void CLogPlotView::DeSelectObj()
{
	if(m_pSelectObj != NULL)
	{
		m_pSelectObj->SetSelect(this,FALSE);
		CDrawObj* pObj = m_pSelectObj;
		m_pSelectObj = NULL;
		pObj->InvalObj(this);
	}
}

void CLogPlotView::OnUpdate(CView* , LPARAM lHint, CObject* pHint)
{
	if(bFirstUpdate) 
		lHint = HINT_UPDATE_SIZE; 
	switch (lHint)
	{
	case HINT_UPDATE_WINDOW:    // redraw entire window
		Invalidate(FALSE);
		break;
	case HINT_UPDATE_SIZE:
        {
			if(m_pDoc == NULL) return;
			bFirstUpdate = FALSE;
			CSize sizeTotal;
			CClientDC dc(this);
			CRect rect;
			GetClientRect(rect);
			if (m_pDoc->m_ScaleNo != 0)
			{
				if (m_pDoc->m_ScaleFactor < 0)
				{
					int cx = MulDiv(rect.right,(-m_pDoc->m_ScaleFactor),dc.GetDeviceCaps(LOGPIXELSX));
					int nLeftSpace = (cx - m_pDoc->m_Rect.Width()) / 2;
					if (nLeftSpace < 10) nLeftSpace = 10;
					if (m_LeftSpace != nLeftSpace) m_LeftSpace = nLeftSpace;
					sizeTotal.cx = MulDiv(m_pDoc->m_Rect.Width()+2*m_LeftSpace,dc.GetDeviceCaps(LOGPIXELSX),-m_pDoc->m_ScaleFactor);
					sizeTotal.cy = MulDiv(m_pDoc->m_Rect.Height()+2*m_TopSpace,dc.GetDeviceCaps(LOGPIXELSY),-m_pDoc->m_ScaleFactor);
				}
				else
				{
					int cx = MulDiv(rect.right,100*100,dc.GetDeviceCaps(LOGPIXELSX)*m_pDoc->m_ScaleFactor);
					int nLeftSpace = (cx - m_pDoc->m_Rect.Width()) / 2;
					if (nLeftSpace < 10) nLeftSpace = 10;
					if (m_LeftSpace != nLeftSpace) m_LeftSpace = nLeftSpace;
					sizeTotal.cx = MulDiv(m_pDoc->m_Rect.Width()+2*m_LeftSpace,dc.GetDeviceCaps(LOGPIXELSX)*m_pDoc->m_ScaleFactor,10000);
					sizeTotal.cy = MulDiv(m_pDoc->m_Rect.Height()+2*m_TopSpace,dc.GetDeviceCaps(LOGPIXELSY)*m_pDoc->m_ScaleFactor,10000);
				}
			}
			else
			{
				m_LeftSpace = 10;
				m_TopSpace = 10;
				sizeTotal.cx = rect.right;
				sizeTotal.cy = MulDiv(m_pDoc->m_Rect.Height()+2*m_TopSpace,rect.right,m_pDoc->m_Rect.Width()+2*m_LeftSpace);
			}
			SetScrollSizes(MM_TEXT, sizeTotal);
			Invalidate(FALSE);
		}
        break;
	case HINT_UPDATE_DRAWOBJ:   // a single object has changed
		((CDrawObj*)pHint)->InvalObj(this);
		break;
	case HINT_UPDATE_HEADOBJ:   // a single object has changed
		break;
	default:
		ASSERT(FALSE);
		break;
	}	
}

void CLogPlotView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	CScrollView::OnPrepareDC(pDC, pInfo);
	CPoint point;
	pDC->SetMapMode(MM_ANISOTROPIC); 
	point.x = pDC->GetDeviceCaps(LOGPIXELSX);
	point.y = pDC->GetDeviceCaps(LOGPIXELSY);
	CPoint point1;
	if((pInfo==NULL)&&(m_pDoc->m_ScaleNo==7))
	{
		point1.x=96;
		point1.y=96;
	}
	else
	{	
		point1.x=100;
		point1.y=100;
	}
	if(m_pDoc->m_ScaleNo != 0)
	{
		if (m_pDoc->m_ScaleFactor < 0) 
		{
			point1.x = -m_pDoc->m_ScaleFactor;
			point1.y = -m_pDoc->m_ScaleFactor;
		}
		else
		{
			point.x = point.x * m_pDoc->m_ScaleFactor / 100;
			point.y = point.y * m_pDoc->m_ScaleFactor / 100;
		}
	}
	else
	{
		CRect rect;
		GetClientRect(rect);
		point.x = point.y = rect.right;
		point1.x = point1.y = m_pDoc->m_Rect.Width() + 2*m_LeftSpace;
	}
	pDC->SetViewportExt(point.x,point.y),
	pDC->SetWindowExt(point1.x, point1.y);              
	// Windows限制设备坐标最大值为32768,使用逻辑坐标原点偏移.
	// 以50英寸(5000个逻辑单位)为页单位实现偏移;
	if(pInfo==NULL)
	{
		CSize m_Page;
		if (point.x > point1.x)
		{
			m_Page.cx = 32600 / point.x;
			m_Page.cy = 32600 / point.y;
		}
		else
		{
			m_Page.cx = 32600 / point1.x;
			m_Page.cy = 32600 / point1.y;
		}
		CSize m_WinPage(m_Page.cx * point.x, m_Page.cy * point.y);
		m_pDoc->m_WinPage.cx = m_Page.cx * point1.x;
		m_pDoc->m_WinPage.cy = m_Page.cy * point1.y;
		CPoint org;
		org.x = GetScrollPos(SB_HORZ);
		org.y = GetScrollPos(SB_VERT);
		int k = (int)(1.0 * org.x / m_WinPage.cx + 0.5);
		m_pDoc->m_WinOrg.x = m_pDoc->m_WinPage.cx * k;
		org.x = org.x - k * m_WinPage.cx;
		k = (int)(1.0 * org.y / m_WinPage.cy + 0.5); 
		m_pDoc->m_WinOrg.y =m_pDoc->m_WinPage.cy * k;
		org.y = org.y - k * m_WinPage.cy;
		pDC->SetViewportOrg(-org.x,-org.y);
 		CSize size(m_LeftSpace,m_TopSpace);
		pDC->LPtoDP(&size);
		pDC->OffsetViewportOrg(size.cx,size.cy);
	}
	else
	{
		int col=pInfo->m_nCurPage/m_pDoc->m_NumRows+(pInfo->m_nCurPage%m_pDoc->m_NumRows>0);
		int row=(pInfo->m_nCurPage-1)%m_pDoc->m_NumRows+1;
		CSize m_Page;
		int offs;
		if(point.x>point1.x)
		{
			m_Page.cx=32600/point.x;
			m_Page.cy=32600/point.y;
			offs=3200;
		}
		else
		{
			m_Page.cx=32600/point1.x;
			m_Page.cy=32600/point1.y;
			offs=3200*point.x/point1.x;
		}
		CSize m_WinPage(m_Page.cx*point.x,m_Page.cy*point.y);
		m_pDoc->m_WinPage.cx=m_Page.cx*point1.x;
		m_pDoc->m_WinPage.cy=m_Page.cy*point1.y;

		CPoint org(m_pDoc->m_PageWidth*(col-1),m_pDoc->m_PageHeight*(row-1));
		m_pDoc->m_WinOrg.x=0;
		m_pDoc->m_WinOrg.y=(org.y+offs)*point1.x/point.x;
		pDC->SetViewportOrg(-org.x,-org.y);
	}
}

CRect CLogPlotView::GetInitialPosition()
{
	CClientDC dc(this);
	OnPrepareDC(&dc,NULL);
	CRect rect(10, 10, 40, 40);
	dc.DPtoLP(rect);
	m_pDoc->PPtoLP(rect);
	return rect;
}

void CLogPlotView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);
	if(m_pDoc == NULL) return;
	UpdateActiveItem();                
	CSize size(cx,cy);
	if (m_pDoc->m_ScaleNo == 0)
	{
		OnUpdate(NULL,HINT_UPDATE_SIZE,NULL);
		return;
	}
	CClientDC dc(this);
	OnPrepareDC(&dc,NULL);
	dc.DPtoLP(&size);
	int nLeftSpace = (size.cx - m_pDoc->m_Rect.Width()) / 2;
	if (nLeftSpace < 10) nLeftSpace = 10;
	if (m_LeftSpace != nLeftSpace)
	{
		m_LeftSpace = nLeftSpace;
		OnUpdate(NULL,HINT_UPDATE_SIZE,NULL);
	}
}

void CLogPlotView::SetScrollSizes(int nMapMode, SIZE sizeTotal)
{
	int nOldMapMode = m_nMapMode;
	m_nMapMode = nMapMode;
	m_totalLog = sizeTotal;
	if(m_pDoc->m_ScaleNo==7)
	{
		m_totalLog.cx=m_totalLog.cx*100/96;
		m_totalLog.cy=m_totalLog.cy*100/96;
	}
	//BLOCK: convert logical coordinate space to device coordinates
	{
		CWindowDC dc(NULL);
		ASSERT(m_nMapMode > 0);
		dc.SetMapMode(m_nMapMode);
		// total size
		m_totalDev = m_totalLog;
		dc.LPtoDP(&m_totalDev);
	} // release DC here
	// now adjust device speFIDic sizes
	m_pageDev.cx = m_totalDev.cx / 30;
	m_pageDev.cy = m_totalDev.cy / 30;
	m_lineDev.cx = m_pageDev.cx / 30;
	m_lineDev.cy = m_pageDev.cy / 30;
	if (m_hWnd != NULL)
	{
		// window has been created, invalidate now
		UpdateBars();
		if (nOldMapMode != m_nMapMode)
			Invalidate(TRUE);
	}
}

void CLogPlotView::UpdateActiveItem()
{
	if (m_pDoc == NULL) return;
	COleClientItem* pActiveItem = m_pDoc->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL &&
		pActiveItem->GetItemState() == COleClientItem::activeUIState)
	    pActiveItem->SetItemRects();
}

void CLogPlotView::SelectObj(CDrawObj* pObj)
{
	DeSelectObj();
	m_pSelectObj = pObj;
	pObj->SetSelect(this,TRUE);
	pObj->InvalObj(this);
}  

void CLogPlotView::GetCenterPoint(LPPOINT pt)
{
	CClientDC dc(this);
	OnPrepareDC(&dc,NULL);
	CRect rect;
	GetClientRect(&rect);
	pt->x = rect.Width() / 2;
	pt->y = rect.Height() / 2;
	dc.DPtoLP(pt);
	m_pDoc->PPtoLP(pt);
}

void CLogPlotView::DeleteContents()
{
	m_pSelectObj = NULL;
	bObjectLink = FALSE;
	m_LeftSpace = m_TopSpace = 10;
}

BOOL CLogPlotView::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following block was added by the ToolTips component.
	{
		m_tooltip.RelayEvent(pMsg);
		return CView::PreTranslateMessage(pMsg);
	}
}

int CLogPlotView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	DragAcceptFiles();
 	return(0);
} 

void CLogPlotView::OnSetFocus(CWnd* pOldWnd)
{
	if (m_pDoc == NULL) return;
	COleClientItem* pActiveItem = m_pDoc->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL &&
		pActiveItem->GetItemState() == COleClientItem::activeUIState)
	{
		CWnd* pWnd = pActiveItem->GetInPlaceWindow();
		if (pWnd != NULL)
		{
			pWnd->SetFocus();
			return;                 
		}
	}
	else
	{
		CDrawObj* pObj = m_pSelectObj; 
		if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CTextObj))) && (((CTextObj*)pObj)->bInput)) 
		{
			pObj->InvalObj(this);
		}
	}
	CScrollView::OnSetFocus(pOldWnd);
}

BOOL CLogPlotView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
{
	// do the scroll
	if (!CScrollView::OnScrollBy(sizeScroll, bDoScroll))
		return FALSE;
	// update the position of any in-place active item
	if (bDoScroll)
	{
		UpdateActiveItem();
		UpdateWindow();
	}
	return TRUE;
}

void CLogPlotView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CScrollView::OnLButtonDown(nFlags, point);
	if(!m_bActive) return;
	CClientDC dc(this);
	OnPrepareDC(&dc,NULL);
	dc.DPtoLP(&point);
	m_pDoc->PPtoLP(&point);
	
	COleClientItem* pActiveItem = m_pDoc->GetInPlaceActiveItem(this);
	if(pActiveItem != NULL)
	{
		pActiveItem->Close();
		ASSERT(m_pDoc->GetInPlaceActiveItem(this) == NULL);
	}	    
	if ((CDrawObj::m_SelectMode == size) && 
		((CDrawObj::c_ToolShape == polytool)||(CDrawObj::c_ToolShape == foldtool)))
	{
		ASSERT(m_pSelectObj);
		m_pSelectObj->OnLButtonDown(this,nFlags,point); 
		return;
	}   
	if (CDrawObj::c_ToolShape != none)
	{
		bObjectLink = FALSE;
		CDrawObj* pObj = NULL;  
		CRect rect(point.x,point.y,point.x,point.y);
		switch (CDrawObj::c_ToolShape)
		{
			case linetool:
				pObj = (CDrawObj*) new CLineObj(rect,NULL,m_pDoc);
				break;
			case arrowtool:
				pObj = (CDrawObj*) new CArrowObj(rect,NULL,m_pDoc);
				break;
			case texttool:
				pObj = (CDrawObj*) new CharacterObj(rect,NULL,m_pDoc);
				break;
			case rectangletool:
				pObj = (CDrawObj*) new CRectangleObj(rect,NULL,m_pDoc);
				break;
			case roundrecttool:
				pObj = (CDrawObj*) new CRoundRectObj(rect,NULL,m_pDoc);
				break;
			case ellipsetool:                                            
				pObj = (CDrawObj*) new CEllipseObj(rect,NULL,m_pDoc);
				break;
			case polytool:
				pObj = (CDrawObj*) new CPolyObj(rect,NULL,m_pDoc);
				break; 
			case foldtool:
				pObj = (CDrawObj*) new CFoldObj(rect,NULL,m_pDoc);
				break; 
			case BmpDrawtool:
				break; 
			case rectarrowtool:
				pObj = (CDrawObj*) new CRectArrowObj(rect,NULL,m_pDoc);
				break; 
		}	
		if(pObj != NULL)
		{                   
			DeSelectObj();
			m_pDoc->AddObj(pObj);
			SelectObj(pObj);
			pObj->OnLButtonDown(this,nFlags,point);
			return;
		}
	}		
	if(m_pSelectObj != NULL)
		if(m_pSelectObj->IsKindOf (RUNTIME_CLASS(CharacterObj)))
		{
			CharacterObj* pTObj=(CharacterObj*)m_pSelectObj;
			int Num=(int)pTObj->m_Text.GetSize (); 
			BOOL Empty=TRUE;
			CString str="";
			for(int i=0;i<Num;i++)
			{
				str=pTObj->m_Text.GetAt (i);
				while(str.Left (1)==" ")
					str.TrimLeft ();
				while(str.Right (1)==" ")
					str.TrimRight ();
				Empty=str.IsEmpty ();
			}
			if(Empty) 
			{
				CDrawObj* pObj = m_pSelectObj;
				CDrawObj* pParentObj = m_pSelectObj->GetParentObj();
				int nParentType = m_pSelectObj->GetParentType();
				m_pDoc->RemoveObj(m_pSelectObj);
				m_pSelectObj = NULL;
				if ((pParentObj != NULL) && (pParentObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (nParentType == 0))
				{
					((CTrackObj*)pParentObj)->Arrangle();
					pParentObj->Invalidate(this);
				}
				else if ((pParentObj == NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))))
				{
					if (m_pDoc->bHoriAutoArrangle) 
						m_pDoc->DoHoriArrangle(m_pDoc->m_HoriArrangleMode);
					if (m_pDoc->bVertAutoAlign) 
						m_pDoc->DoVertAlign();
					m_pDoc->UpdateAllViews(NULL,HINT_UPDATE_WINDOW);
				}
				else pObj->Invalidate(this);
				pObj->Remove();
			}
		}

	CDrawObj* pDrawObj = NULL;
	CDrawObj::m_SelectMode = none;  
	if (m_pSelectObj != NULL) pDrawObj = m_pSelectObj->ObjectAt(this,point);
	if (pDrawObj == NULL) pDrawObj = m_pDoc->ObjectAt(this,point);
	if (pDrawObj != NULL)
	{
		if (pDrawObj != m_pSelectObj)
		{
			if (bObjectLink)
			{                                       
				CDrawObj* pObj = pDrawObj;
				while ((pObj != NULL) && (pObj != m_pSelectObj)) pObj = pObj->GetParentObj();
				if (pObj == NULL) 
				{
					m_pDoc->RemoveObj(m_pSelectObj);
					pDrawObj->AddObj(m_pSelectObj,2);
					bObjectLink = FALSE;
				}	
				return;
			}
			SelectObj(pDrawObj);
		}	
		pDrawObj->OnLButtonDown(this,nFlags,point);
	}       
	else DeSelectObj();
	bObjectLink = FALSE;
}

void CLogPlotView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CScrollView::OnLButtonDblClk(nFlags, point);
	if(!m_bActive) return;
	if(m_pSelectObj!= NULL)
	{
		CClientDC dc(this);
		OnPrepareDC(&dc,NULL);
		dc.DPtoLP(&point);
		m_pDoc->PPtoLP(&point);
		m_pSelectObj->OnLButtonDblClk(this,nFlags,point);
	}
	else
		OnSetupPage();
}

void CLogPlotView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CScrollView::OnLButtonUp(nFlags, point);
	if (!m_bActive) return;
	if(CDrawObj::m_SelectMode == none) return;
	if(m_pSelectObj != NULL)
	{
		CClientDC dc(this);
		OnPrepareDC(&dc,NULL);
		dc.DPtoLP(&point);
		m_pDoc->PPtoLP(&point);
		m_pSelectObj->OnLButtonUp(this,nFlags,point); 
		return;
	} 
}

void CLogPlotView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bActive) return;
	CClientDC dc(this);
	OnPrepareDC(&dc,NULL);            
	dc.DPtoLP(&point);
	m_pDoc->PPtoLP(&point);
	if(GetCapture() != this)
	{
		if (!m_pDoc->OnSetCursor(this,point)) 
			SetCursor(LoadCursor(NULL,IDC_ARROW));
		CRect rect(point.x,point.y,point.x,point.y);
		if (m_pDoc->OnToolTip(this,rect,TipText))
		{
			if (!bToolTipActive)
			{
				SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_CROSS));
				m_pDoc->LPtoPP(rect);
				dc.LPtoDP(rect);
				rect.InflateRect(5,5);
				m_tooltip.SetToolRect(this,ID_CURVE_VALUE,rect);
				m_tooltip.Activate(TRUE);
				m_tooltip.UpdateTipText((LPCTSTR)TipText,this,ID_CURVE_VALUE);
				bToolTipActive = TRUE;
			    CPoint ps;
				GetCursorPos(&ps);
			    SetCursorPos(ps.x,ps.y);
			}
		}
		else
		{
			m_tooltip.Activate(FALSE);
			bToolTipActive = FALSE;
		}
		return;
	}	
	if (bToolTipActive)
	{
		m_tooltip.Activate(FALSE);
		bToolTipActive = FALSE;
	}
	if (m_pSelectObj != NULL)
		m_pSelectObj->OnMouseMove(this,nFlags,point); 
}

void CLogPlotView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CClientDC dc(this);
	CRect cRect;
	GetClientRect(cRect);
	if ((m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTextObj))))
	{
		CTextObj* pObj = (CTextObj*) m_pSelectObj;
		pObj->OnKeyDown(this,nChar, nRepCnt,nFlags);
	}
	else
	{
		CPoint ptScroll = GetDeviceScrollPosition();
		CPoint ptSave = ptScroll;	
		switch (nChar)
		{
			case VK_LEFT: 
				ptScroll.x -= 20;	
				break;
			case VK_RIGHT:
				ptScroll.x += 20;
				break;
			case VK_UP: 
				ptScroll.y -= 20;
				break;
			case VK_DOWN: 
				ptScroll.y += 20;
				break;
			case VK_PRIOR:
				ptScroll.y -= cRect.Height();
				break;
			case VK_NEXT:
				ptScroll.y += cRect.Height();
				break;
		}
		if (ptScroll.y < 0) ptScroll.y = 0;
		else if (ptScroll.y+cRect.Height() > m_totalDev.cy) ptScroll.y = m_totalDev.cy - cRect.Height();
		if (ptScroll.x < 0) ptScroll.x = 0;
		else if (ptScroll.x+cRect.Width() > m_totalDev.cx) ptScroll.x = m_totalDev.cx - cRect.Width();
		if ((ptSave.x != ptScroll.x) && (cRect.Width() < m_totalDev.cx))
		{
			SetScrollPos(SB_HORZ, ptScroll.x);
			ScrollWindow(ptSave.x - ptScroll.x,0);
			UpdateWindow();
		}
		if ((ptSave.y != ptScroll.y) && (cRect.Height() < m_totalDev.cy))
		{
			SetScrollPos(SB_VERT, ptScroll.y);
			ScrollWindow(0,ptSave.y - ptScroll.y);
			UpdateWindow();
		}
	}                   
	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CLogPlotView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ((m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTextObj))))
	{
		CTextObj* pObj = (CTextObj*) m_pSelectObj;
		pObj->OnChar(this,nChar,nRepCnt,nFlags); 
	}
	CScrollView::OnChar(nChar, nRepCnt, nFlags);
}

void CLogPlotView::OnDropFiles(HDROP hDropInfo)
{
	char szFileName[512];
	CString szFIDName;
	CFIDDrawObj* pObj;
	CPoint pt;

    DragQueryPoint (hDropInfo,(LPPOINT) &pt);
	CClientDC dc(this);
	OnPrepareDC(&dc,NULL);
	dc.DPtoLP(&pt);
	m_pDoc->PPtoLP(&pt);

    int gwFilesDropped = DragQueryFile (hDropInfo,(UINT)-1,NULL,0);
    for (int i=0; i < gwFilesDropped; i++)
    {
		DragQueryFile (hDropInfo,i,(LPSTR) szFileName,512);
		CGeoFIDIO::GetFIDName(szFileName,szFIDName.GetBuffer(128));
		szFIDName.ReleaseBuffer();
		pObj = NULL;
		if(szFIDName != "")
		{
			CString path=szFileName;
			path.MakeUpper();
			CRect rect(pt.x,pt.y,pt.x,pt.y);
			double sdep,edep;
			m_pDoc->GetDepthRangle(sdep, edep);
			if(path.Find(".LIST")>0)
			{
				if(path.Find("RESULT")>0)
					pObj = new CResultObj(szFileName,rect,NULL,m_pDoc);
				else if (path.Find("DSC")>0)
					pObj = new CDSCObj(szFileName,rect,NULL,m_pDoc);
				else if(path.Find("NOTE")>0)
					pObj = new CLthNoteObj(szFileName,rect,NULL,m_pDoc);
				else if(path.Find("HANDWORK")>0)
					pObj = new CPoleObj(szFileName,rect,NULL,m_pDoc);
				else if(path.Find("RESULTDSC")>0)
					pObj = new CDSCObj(szFileName,rect,NULL,m_pDoc);
				else if(path.Find("VDL_RESULT")>0)
					pObj = new CGrDenResultObj(szFileName,rect,NULL,m_pDoc);
				else
					pObj = new CLthObj(szFileName,rect,NULL,m_pDoc);
			}
			else
			{
				CString filename=path.Mid(path.ReverseFind('>')+1);
				filename=filename.Left(filename.ReverseFind('+'));
				filename=path.Left(path.ReverseFind('.')+1)+filename;
				CGeoFIDIO* pFIDIO = new CGeoFIDIO(filename);
				if(pFIDIO->Open(m_pDoc->m_szSever)&&(pFIDIO->GetDimenSion()==2))
				{
					pObj = new CCurveObj(filename,rect,NULL,m_pDoc);
					pObj->SetDepth(sdep,edep);
					if(!pObj->Properties(this)) 
					{
						delete pObj;
						pObj = NULL;
					}
				}
				else
				{
		         	CString  msg="不能打文件"+filename;
					AfxMessageBox(msg,MB_OK | MB_ICONSTOP);
				}
				delete pFIDIO;
         	}	
			if(pObj != NULL)
			{
				if (!pObj->IsKindOf(RUNTIME_CLASS(CCurveObj)))
					pObj->SetDepth(sdep,edep);
				m_pDoc->AddObj(pObj);
				SelectObj(pObj);
			}
         }	
    }
    DragFinish (hDropInfo);	
}

BOOL CLogPlotView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int iScrollBarPos = GetScrollPos( SB_VERT )-zDelta;
	SetScrollPos( SB_VERT, iScrollBarPos );
	Invalidate();
	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}

void CLogPlotView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	//GetParentFrame()->ActivateFrame();
	CPoint local = point;
	ScreenToClient(&local);m_LocalPt=local;
	CClientDC dc(this);
	OnPrepareDC(&dc,NULL);
	dc.DPtoLP(&local);
	m_pDoc->PPtoLP(&local);
	
	CDrawObj* pDrawObj = NULL;
	CDrawObj::m_SelectMode = none;  
	if (m_pSelectObj != NULL) 
		pDrawObj = m_pSelectObj->ObjectAt(this,local);
	if (pDrawObj == NULL) 
		pDrawObj = m_pDoc->ObjectAt(this,local);
	CMenu menu;
	if(pDrawObj != NULL)
	{
		if(pDrawObj != m_pSelectObj) 
			SelectObj(pDrawObj);
		if(menu.LoadMenu(IDR_POP_MENU))
		{
			CMenu* pPopup = menu.GetSubMenu(0);
			if(m_pDoc->m_szRole=="SuperAdmin"||m_pDoc->m_szRole=="Admin")
			{
				if(m_pSelectObj != NULL)
				{
					pPopup->AppendMenu (MF_BYPOSITION,MF_SEPARATOR);
					pPopup->AppendMenu (MF_BYPOSITION,ID_OBJECT_PROPTERTIES,"属性");
					pPopup->AppendMenu (MF_BYPOSITION,MF_SEPARATOR);
					pPopup->AppendMenu (MF_BYPOSITION,ID_EDIT_CUT,"删除");
				}
			}
			pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN,
									point.x, point.y,
									this->GetWindow(0)); 
		}
	}
	else
	{
		if(menu.LoadMenu(IDR_POP_MENU))
		{
			CMenu* pPopup = menu.GetSubMenu(0);
			pPopup->AppendMenu (MF_BYPOSITION,ID_MATCH_ALLOBJECT,"匹配当前项目");
			pPopup->AppendMenu (MF_BYPOSITION,MF_SEPARATOR);
			pPopup->AppendMenu (MF_BYPOSITION,ID_SETUP_PAGE,"版面设置");
			pPopup->AppendMenu (MF_BYPOSITION,ID_SET_DEPTH,"深度设置");
			/*
			ID_FILE_SAVE_BMP                32871
			ID_FILE_SAVE_DATA				32872
			用户角色
			"SuperAdmin"	超级管理员
			"Admin"			管理员
			"Author"		员工
			m_szRole=_T("");
			*/
			if(m_pDoc->m_szRole=="SuperAdmin"||m_pDoc->m_szRole=="Admin")
			{
				pPopup->AppendMenu (MF_BYPOSITION,MF_SEPARATOR);
				pPopup->AppendMenu (MF_BYPOSITION,ID_ADD_TRACK, "添加格线道");
				pPopup->AppendMenu (MF_BYPOSITION,ID_ADD_CURVE, "添加常规曲线");
				pPopup->AppendMenu (MF_BYPOSITION,MF_SEPARATOR);
				pPopup->AppendMenu (MF_BYPOSITION,ID_FILE_SAVE_BMP,"保存BMP文件到本地");
				pPopup->AppendMenu (MF_BYPOSITION,ID_FILE_SAVE_DATA,"保存当前项目到本地");
			}
			pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN,
									point.x, point.y,
									this->GetWindow(0)); 
		}
	}
}

void CLogPlotView::OnEditCut()
{
	if(m_pSelectObj != NULL) 
	{
		CDrawObj* pObj = m_pSelectObj;
		CDrawObj* pParentObj = m_pSelectObj->GetParentObj();
		int nParentType = m_pSelectObj->GetParentType();
		m_pDoc->RemoveObj(m_pSelectObj);
		m_pSelectObj = NULL;
		if((pParentObj != NULL) && (pParentObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (nParentType == 0))
		{
			((CTrackObj*)pParentObj)->Arrangle();
			pParentObj->Invalidate(this);
		}
		else if((pParentObj == NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))))
		{
			if(m_pDoc->bHoriAutoArrangle) 
				m_pDoc->DoHoriArrangle(m_pDoc->m_HoriArrangleMode);
			if(m_pDoc->bVertAutoAlign) 
				m_pDoc->DoVertAlign();
			m_pDoc->UpdateAllViews(NULL,HINT_UPDATE_WINDOW);
		}
		else pObj->Invalidate(this);
	}	
}

void CLogPlotView::OnObjectPropterties() 
{
	if(m_pSelectObj != NULL)
		m_pSelectObj->OnLButtonDblClk(this, 0, CPoint(0,0));
}

void CLogPlotView::OnUpdateObjectPropterties(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pSelectObj != NULL);	
}

void CLogPlotView::SaveDocToBmpfile()
{       
	if (m_pDoc == NULL) return;
	CString BmpFileName=m_pDoc->m_szProjectName;
	if(BmpFileName.GetAt(0)=='#')
		BmpFileName=BmpFileName.Mid(1);
	if(BmpFileName.IsEmpty())
	{
		CTime time = CTime::GetCurrentTime();
		BmpFileName=time.Format(_T("%m%d%H%M%S"));
	}
	
	WORD ColorNum=24;
	static char BASED_CODE szFilter[] = 
	"位图文件(*.bmp)|*.bmp|所有文件(*.*)|*.*||";
	CFileDialog  Browse(FALSE, "位图文件(*.bmp)",BmpFileName,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	Browse.m_ofn.lpstrTitle = "保存为位图文件...";

	if(Browse.DoModal()==IDOK)
		BmpFileName = Browse.GetPathName ();
	else return;

	CRect TallRect;
	TallRect = m_pDoc->GetRect();
	if(m_pDoc->m_bPlotHead)
		TallRect.bottom +=m_pDoc->m_HeadRect.Height ();

	CDC* pDrawDC0 = this->GetDC();
	OnPrepareSaveBmp(pDrawDC0);
	pDrawDC0->LPtoDP (&TallRect);

	CRect BmpRect;	
	BmpRect.left=TallRect.left;
	BmpRect.right =TallRect.right ;

	HBITMAP hBitmap;
	HDIB m_hDIB;
		
	int Num=(TallRect.Height ())/1600;
	if(TallRect.Height ()%1600!=0) Num++;

	CProgressDlg* pGuage = new CProgressDlg;
	pGuage->Create(GetWindow(0));
	pGuage->SetRange (0,Num);
	pGuage->SetStep (1);
	pGuage->SetPropMesg ("写位图文件到本地...");

	FILE *fpBMP=fopen(BmpFileName,"w+b");

	BITMAPFILEHEADER bmfHdr; 
	LPBITMAPINFOHEADER lpBI; 
	BOOL WriteHead=TRUE;
	for(int i=Num-1;i>=0;i--)
	{
		pGuage->SetPos (Num-i);
		if (pGuage->CheckCancelButton()) break;

		BmpRect.top=TallRect.top+1600*i;
		BmpRect.bottom =TallRect.top+1600*i+1600;
		if(BmpRect.bottom>TallRect.bottom )
			BmpRect.bottom=TallRect.bottom;
		CBitmap m_Bitmap;
		CDC m_MemDC;
		CDC* pDrawDC=GetDC();
		if (!m_MemDC.CreateCompatibleDC(pDrawDC))
		{
			AfxMessageBox("初始化内存失败",MB_ICONINFORMATION);
			fclose(fpBMP);delete pGuage;return;
		}
		if (m_Bitmap.CreateCompatibleBitmap(pDrawDC, BmpRect.Width(), BmpRect.Height()))
		{
			pDrawDC = &m_MemDC;	
			m_MemDC.OffsetViewportOrg(-BmpRect.left, -BmpRect.top);
			m_MemDC.SelectObject(&m_Bitmap);
		}
		else	
		{
			AfxMessageBox("初始化位图失败",MB_ICONINFORMATION);
			fclose(fpBMP);delete pGuage;return;
		}

		CBrush brush;
		if (!brush.CreateSolidBrush(m_pDoc->m_paperColor)) 
		{
			fclose(fpBMP);delete pGuage;return;
		}
		brush.UnrealizeObject();  
		pDrawDC->FillRect(BmpRect, &brush);
		OnPrepareSaveBmp(pDrawDC);
		m_pDoc->Print(pDrawDC,this);
		////////////////////////////////////////////////////////////////////////////////////		
		hBitmap = (HBITMAP) m_Bitmap.GetSafeHandle();
		if(!hBitmap)
		{
			AfxMessageBox("从内存中获取图形失败",MB_ICONINFORMATION);
			fclose(fpBMP);delete pGuage;return;
		}	
		m_hDIB = BitmapToDIB(hBitmap,ColorNum);//1,4,8,24
		if (m_hDIB == NULL)
		{
			AfxMessageBox("图形文件 HBITMAP 转换 HDIB 失败",MB_ICONINFORMATION);
			fclose(fpBMP);delete pGuage;return;
		}
		///////////////////////////////////////////////////////////////////////////////////
		lpBI = (LPBITMAPINFOHEADER) ::GlobalLock((HGLOBAL) m_hDIB);
		if(lpBI == NULL)
		{
			fclose(fpBMP);delete pGuage;return;
		}
		// Now calculate the size of the image
		if(! ((lpBI->biCompression == BI_RLE8) || (lpBI->biCompression == BI_RLE4)))
		{
			DWORD dwBmBitsSize;  
			dwBmBitsSize = WIDTHBYTES((lpBI->biWidth)*((DWORD)lpBI->biBitCount)) *lpBI->biHeight;
			lpBI->biSizeImage= dwBmBitsSize;
		}
		DWORD BulkBISizePale=lpBI->biSize+ PaletteSize((LPSTR)lpBI);
		if(WriteHead)
		{
			// Write the file header
			bmfHdr.bfType = DIB_HEADER_MARKER;
			bmfHdr.bfReserved1 = 0;
			bmfHdr.bfReserved2 = 0;
			bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) +BulkBISizePale;
			bmfHdr.bfSize =bmfHdr.bfOffBits+WIDTHBYTES((TallRect.Width ())*
								((DWORD)lpBI->biBitCount))* TallRect.Height ();			
			fwrite((LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER),1,fpBMP);
			// Write the DIB header 
			lpBI->biWidth=TallRect.Width ();
			lpBI->biHeight=TallRect.Height ();
			WriteHead=FALSE;
			fwrite(lpBI, BulkBISizePale+lpBI->biSizeImage,1,fpBMP);
		}
		else
		{
			char* Buf;
			Buf=(char*)lpBI;
			Buf+=BulkBISizePale;
			fwrite(Buf, lpBI->biSizeImage,1,fpBMP);
		}
		if(i==0)
			fwrite(lpBI, lpBI->biSizeImage,1,fpBMP);
		::GlobalUnlock((HGLOBAL) m_hDIB);

		m_Bitmap.DeleteObject ();
		m_MemDC.DeleteDC ();
		pDrawDC->DeleteDC ();
	}
	fclose(fpBMP);
	delete pGuage;
}

void CLogPlotView::OnUpdateDocSaveBmp(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable (m_pDoc != NULL);
}

DWORD CLogPlotView::DIBNumColors(LPSTR lpbi)
{
	WORD wBitCount;  
	if (IS_WIN30_DIB(lpbi))
	{
		DWORD dwClrUsed;

		dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;
		if (dwClrUsed != 0)
			return (WORD)dwClrUsed;
	}

	if (IS_WIN30_DIB(lpbi))
		wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
	else
		wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;

	/* return number of colors based on bits per pixel */
	switch (wBitCount)
	{
		case 1:
			return 2;
		case 4:
			return 16;
		case 8:
			return 256;
//		case 16:
//			return 65536;
//		case 24:
//			return 16777216;
		default:
			return 0;
	}
}

DWORD CLogPlotView::PaletteSize(LPSTR lpbi)
{
   /* calculate the size required by the palette */
   if (IS_WIN30_DIB (lpbi))
	  return (DWORD)(DIBNumColors(lpbi) * sizeof(RGBQUAD));
   else
	  return (DWORD)(DIBNumColors(lpbi) * sizeof(RGBTRIPLE));
}

HDIB CLogPlotView::BitmapToDIB(HBITMAP hBitmap, WORD biBits)
{
    BITMAP              bm;         // bitmap structure
    BITMAPINFOHEADER    bi;         // bitmap header
    LPBITMAPINFOHEADER  lpbi;       // pointer to BITMAPINFOHEADER
    DWORD               dwLen;      // size of memory block
    HANDLE              hDIB, h;    // handle to DIB, temp handle
    HDC                 hDC;        // handle to DC
  
    if (!hBitmap)
        return NULL;
    if (!GetObject(hBitmap, sizeof(bm), (LPSTR)&bm))
        return NULL;
    // initialize BITMAPINFOHEADER

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bm.bmWidth;
    bi.biHeight = bm.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = biBits;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;
    // calculate size of memory block required to store BITMAPINFO
    dwLen = bi.biSize + PaletteSize((LPSTR)&bi);

    // get a DC
    hDC = ::GetDC(this->m_hWnd );

    // alloc memory block to store our bitmap
    hDIB = GlobalAlloc(GHND, dwLen);

    // if we couldn't get memory block
    if (!hDIB)
    {
      // clean up and return NULL
	  ::ReleaseDC(NULL,hDC);
      return NULL;
    }

    // lock memory and get pointer to it
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIB);

    /// use our bitmap info. to fill BITMAPINFOHEADER
   *lpbi = bi;

    // call GetDIBits with a NULL lpBits param, so it will calculate the
    // biSizeImage field for us    
    GetDIBits(hDC, hBitmap, 0, (UINT)bi.biHeight, NULL, (LPBITMAPINFO)lpbi,
        DIB_RGB_COLORS);

    // get the info. returned by GetDIBits and unlock memory block
    bi = *lpbi;
    GlobalUnlock(hDIB);

    // if the driver did not fill in the biSizeImage field, make one up 
    if (bi.biSizeImage == 0)
        bi.biSizeImage = WIDTHBYTES((DWORD)bm.bmWidth * biBits) * bm.bmHeight;

    // realloc the buffer big enough to hold all the bits
    dwLen = bi.biSize + PaletteSize((LPSTR)&bi) + bi.biSizeImage;

    if (h = GlobalReAlloc(hDIB, dwLen, 0))
        hDIB = h;
    else
    {
        // clean up and return NULL
        GlobalFree(hDIB);
        hDIB = NULL;
		::ReleaseDC(NULL,hDC);
        return NULL;
    }

    // lock memory block and get pointer to it 
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIB);

    // call GetDIBits with a NON-NULL lpBits param, and actualy get the
    // bits this time
    if (GetDIBits(hDC, hBitmap, 0, (UINT)bi.biHeight, (LPSTR)lpbi +
            (WORD)lpbi->biSize + PaletteSize((LPSTR)lpbi), (LPBITMAPINFO)lpbi,
            DIB_RGB_COLORS) == 0)
    {
        // clean up and return NULL
        GlobalUnlock(hDIB);
        hDIB = NULL;
		::ReleaseDC(NULL,hDC);
        return NULL;
    }
    bi = *lpbi;

    // clean up 
    GlobalUnlock(hDIB);
	::ReleaseDC(NULL,hDC);
    // return handle to the DIB
    return (HDIB)hDIB;
}

void CLogPlotView::OnPrepareSaveBmp(CDC *pDrawDC)
{
	pDrawDC->SetMapMode(MM_ANISOTROPIC); 
	CPoint point;
	point.x = pDrawDC->GetDeviceCaps(LOGPIXELSX);
	point.y = pDrawDC->GetDeviceCaps(LOGPIXELSY);
	CPoint point1(100,100);
	if(m_pDoc->GetImageObject()!=NULL)
	{
		point1.x=96;
		point1.y=96;
	}
	if (m_pDoc->m_ScaleNo != 0)
	{
		if (m_pDoc->m_ScaleFactor < 0) 
		{
			point1.x = -m_pDoc->m_ScaleFactor;
			point1.y = -m_pDoc->m_ScaleFactor;
		}
		else
		{
			point.x = point.x * m_pDoc->m_ScaleFactor / 100;
			point.y = point.y * m_pDoc->m_ScaleFactor / 100;
		}
	}
	else
	{
		CRect rect;
		GetClientRect(rect);
		point.x = point.y = rect.right;
		point1.x = point1.y = m_pDoc->m_Rect.Width() + 2*m_LeftSpace;
	}
	pDrawDC->SetViewportExt(point.x,point.y),
	pDrawDC->SetWindowExt(point1.x, point1.y);
}

void CLogPlotView::OnSetDepth() 
{
	if(m_pDoc==NULL) return;
	CPlotDepthDialog dialog;
	double sdep,edep;
	dialog.m_DepthScale=m_pDoc->m_DepthScale;
	m_pDoc->GetDepthRangle(sdep,edep);
	dialog.SetDepth(sdep,edep);
	if(dialog.DoModal() == IDOK)
	{
		dialog.GetDepth(sdep,edep);	 		
		m_pDoc->SetDepth(sdep,edep);
		m_pDoc->SetDepthScale(dialog.m_DepthScale);
		m_pDoc->OnLayerFlush();
	}
	OnUpdate(NULL,HINT_UPDATE_SIZE,NULL);
}

void CLogPlotView::OnUpdateSetDepth(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pDoc!=NULL);
}

void CLogPlotView::OnDocdataUpdate()
{
	OnUpdate(NULL,HINT_UPDATE_SIZE,NULL);
}

void CLogPlotView::OnMatchAllobject() 
{
	// TODO: Add your command handler code here
	if(m_pDoc->MatchAllObject())
		OnUpdate(NULL,HINT_UPDATE_SIZE,NULL);
}
void CLogPlotView::OnSetupPage()
{
	if(m_pDoc->SetupPage())
		OnUpdate(NULL,HINT_UPDATE_SIZE,NULL);
}
//下载数据到本地保存为FID格式
void CLogPlotView::SaveDocToDatafile()
{ 
	if(m_pDoc->m_ProjectID<1) return;
	CDirDialog dlg;
	dlg.SetWindowTitle("选择数据路径");
	if(dlg.DoModal() == IDOK)
	{
		CString ProjectName=m_pDoc->m_szProjectName;
		if(ProjectName.GetAt(0)=='#')
			ProjectName=ProjectName.Mid(1);
		CString LocalPath=dlg.GetPathname()+"\\#"+ProjectName;
		_mkdir(LocalPath);

		ProjectName=LocalPath+"\\"+ProjectName;
		DownCurve(ProjectName,m_pDoc->m_ProjectID);
		DownProduction(ProjectName,m_pDoc->m_ProjectID);
	}
}
void CLogPlotView::OnUpdateDocSaveData(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable (m_pDoc != NULL);
}

BOOL CLogPlotView::DownCurve(CString SaveName,int ProjectID)
{
	CGeoAdo	ado;
	if(!ado.InitData(m_pDoc->m_szSever)) return FALSE;

	CString cID;
	cID.Format("%lu",ProjectID);
	_RecordsetPtr pRS = ado.GetRS("SELECT * FROM Storage WHERE StorageID=" + cID);

	//返回各列的值
	if(pRS->adoEOF)
	{
		ado.Disconnect();
		return FALSE;
	}
	else
	{
		CString StorageKind=(LPCTSTR)(_bstr_t)pRS->GetCollect("StorageKind");
		StorageKind.Trim();
		if(StorageKind=="测井项目")
		{
			pRS = ado.GetRS("SELECT * FROM Curve WHERE UpperID=" + cID);
			
			CString Name,str;
			int CurveNum=0;
			//得到项目中曲线条数，进度条使用
			while(pRS->adoEOF==0)
			{
				CurveNum++;
				pRS->MoveNext();
			}
	
			int k=SaveName.ReverseFind('\\');
			CString ProjectName=SaveName.Mid(k+1);
			CString Path=SaveName.Left(k);
			CString CurveName;
			//初始化进度指示器
			CProgressDlg *pGuage = new CProgressDlg;
			pGuage->Create();
			pGuage->SetPropMesg("下载测井曲线到本地...");
			pGuage->SetRange(0,CurveNum);
			CString inProject=m_pDoc->m_szProjectName;
			if(inProject.GetAt(0)=='#')
				inProject=str.Mid(1);
			CString m_szPathName="@GeoDB\\"+m_pDoc->m_szWellName+"\\"+m_pDoc->m_szProjectName+"\\"+inProject;

			int p=0;
			pRS = ado.GetRS("SELECT * FROM Curve WHERE UpperID=" + cID);
			//执行SELECT语句
			while(pRS->adoEOF==0)
			{
				Name= (LPCTSTR)(_bstr_t)pRS->GetCollect("CurveName");
				Name.Trim();
				pGuage->SetPos(p);
				CGeoFIDIO* pFIDIO = new CGeoFIDIO(m_szPathName+"."+Name);
				if(pFIDIO->Open(m_pDoc->m_szSever))
					pFIDIO->WriteLocalFID(SaveName,Name);
				delete pFIDIO;pFIDIO=NULL;
				pRS->MoveNext();
				p++;
			}
			/*if(!RefurbishFID(FIDPath))
				AfxMessageBox("Can't Refurbish file "+SaveName);*/
			ado.Disconnect();delete pGuage;
			return TRUE;
		}
	}
	ado.Disconnect();
	return FALSE;
}

BOOL CLogPlotView::DownProduction(CString SaveName,int ProjectID)
{
	CGeoAdo	ado;
	if(!ado.InitData(m_pDoc->m_szSever)) return FALSE;

	CString cID;
	cID.Format("%lu",ProjectID);
	_RecordsetPtr pRS = ado.GetRS("SELECT * FROM Storage WHERE StorageID=" + cID);

	//返回各列的值
	if(pRS->adoEOF)
		return FALSE;
	else
	{
		CString StorageKind=(LPCTSTR)(_bstr_t)pRS->GetCollect("StorageKind");
		StorageKind.Trim();
		if(StorageKind=="测井项目")
		{
			pRS = ado.GetRS("SELECT * FROM Production WHERE UpperID=" + cID);
			
			CString Name,str;
			int FileNum=0;
			//得到项目中曲线条数，进度条使用
			while(pRS->adoEOF==0)
			{
				FileNum++;
				pRS->MoveNext();
			}
			//本地项目检索路径
			CString Path=SaveName.Left(SaveName.ReverseFind('\\'));
			CString FileName;
			//初始化进度指示器
			CProgressDlg *pGuage = new CProgressDlg;
			pGuage->Create();
			pGuage->SetPropMesg("下载成果文档到本地...");
			pGuage->SetRange(0,FileNum);
			pRS = ado.GetRS("SELECT * FROM Production WHERE UpperID=" + cID);
			//执行SELECT语句
			int p=0;
			while(pRS->adoEOF==0)
			{
				Name= (LPCTSTR)(_bstr_t)pRS->GetCollect("DocName");
				Name.Trim();
				pGuage->SetPos(p);
			
				//写成果文档到了本地
				FileName=Path+"\\"+Name;
				CFile fl;
				if(fl.Open(FileName,CFile::modeCreate|CFile::modeWrite))
				{
					//读取DataByte字段的实际大小
					DWORD Size = pRS->GetFields()->GetItem("DataByte1")->ActualSize;		
					BSTR HUGEP *Buffer=NULL;
					if(Size>0)
					{		
						//从图像字段中读取数据到varBLOB中
						_variant_t varBLOB = pRS->GetFields()->GetItem("DataByte1")->GetChunk(Size);		
						if(varBLOB.vt == (VT_ARRAY | VT_UI1))
						{
							SafeArrayAccessData(varBLOB.parray,(void HUGEP**)&Buffer); 
							fl.Write(Buffer,Size);
							SafeArrayUnaccessData(varBLOB.parray);
						}
					}
					fl.Close();
				}
				pRS->MoveNext();
				p++;
			}
			ado.Disconnect();delete pGuage;
			return TRUE;
		}
	}
	return FALSE;
}
void CLogPlotView::OnAddCurve()
{
	CPoint pt;
	GetCenterPoint(&pt);
	CRect rect(pt.x,pt.y,pt.x,pt.y);
	
	CString Path="@GeoDB\\"+m_pDoc->m_szWellName+"\\"+m_pDoc->m_szProjectName+"\\";
	CString szProject=m_pDoc->m_szProjectName;
	if(szProject.GetAt(0)=='#')
		szProject=szProject.Mid(1);
	Path=Path+szProject+".GR";
	CCurveObj* pObj = new CCurveObj(Path,rect,NULL,m_pDoc);
	double sdep,edep;
	m_pDoc->GetDepthRangle(sdep, edep);
	pObj->SetDepth(sdep,edep);
	if(pObj->Properties(this))
	{
		m_pDoc->AddObj(pObj);
		SelectObj(pObj);
	}                                                            
	else delete pObj;
}

void CLogPlotView::OnUpdateAddCurve(CCmdUI *pCmdUI)
{
	
}

void CLogPlotView::OnAddTrack()
{
	CPoint pt;
	GetCenterPoint(&pt);
	CRect rect(pt.x,pt.y,pt.x,pt.y);
	double sdep,edep;
	m_pDoc->GetDepthRangle(sdep, edep);
	CTrackObj* pObj = (CTrackObj*) new CTrackObj(rect,NULL,m_pDoc,sdep,edep);
	m_pDoc->AddObj(pObj);
	SelectObj(pObj);
}

void CLogPlotView::OnUpdateAddTrack(CCmdUI *pCmdUI)
{
	
}

int CLogPlotView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}
