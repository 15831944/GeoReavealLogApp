// MeasureMultiLine.cpp: implementation of the CMeasureMultiLine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageMan.h"
#include "MeasureMultiLine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeasureMultiLine::CMeasureMultiLine()
{
	m_colorLine = RGB(0, 0, 0);
	m_colorTag = RGB(255,0, 0);
	m_colorFont = RGB(0, 0, 0);
}

CMeasureMultiLine::~CMeasureMultiLine()
{
	m_Pen.DeleteObject();
	m_CurPen.DeleteObject();
	m_dcScr.DeleteDC();
	m_dcMem.DeleteDC();
}

void CMeasureMultiLine::CreateMeasure(LPCTSTR lpTitle,CWnd *pWnd,CRect rect)
{
	m_Rect=rect;
	//创建透明窗口
	CreateEx(WS_EX_TOPMOST, AfxRegisterWndClass(CS_DBLCLKS, AfxGetApp()->LoadStandardCursor(IDC_ARROW)), 
		"Measure",WS_POPUP|WS_VISIBLE,rect,NULL,NULL,NULL);

	m_dcMem.CreateCompatibleDC(NULL);
	ShowWindow(SW_HIDE);
	//记录父窗口的指针,以便测量结束进行通知
	m_pWndParent = pWnd;
	//创建辅助线的画笔
	m_Pen.CreatePen(PS_DOT, 1, m_colorLine);
	m_CurPen.CreatePen(PS_SOLID, 1, m_colorTag);
	//取桌面屏幕设备 
	m_dcScr.CreateDC("DISPLAY",NULL,NULL,NULL);
}

BEGIN_MESSAGE_MAP(CMeasureMultiLine, CWnd)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

void CMeasureMultiLine::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDblClk(nFlags, point);
	ShowWindow(SW_HIDE);
	m_pWndParent->PostMessage(WM_ENDCLICK);
}

void CMeasureMultiLine::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);
	ClientToScreen(&point);
	m_dcScr.BitBlt(m_Rect.left,m_Rect.top,m_Rect.Width(),m_Rect.Height(),&m_dcMem,0,0,SRCCOPY);
	if(m_MousePt.x>0)
	{
		CPen * pOldPen= (CPen*)m_dcScr.SelectObject(&m_CurPen);
		m_dcScr.MoveTo(m_MousePt);
		m_dcScr.LineTo(point);
		m_dcScr.SelectObject(pOldPen);
	}
	m_MousePt=point;
	m_arrayPoints.Add(point);//当前点加入数组
	m_dcMem.BitBlt(0,0,m_Rect.Width(),m_Rect.Height(),&m_dcScr,m_Rect.left,m_Rect.top,SRCCOPY);
}

void CMeasureMultiLine::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);
	m_dcScr.BitBlt(m_Rect.left,m_Rect.top,m_Rect.Width(),m_Rect.Height(),&m_dcMem,0,0,SRCCOPY);
	ClientToScreen(&point);
	
	//画新的辅助线
	CPen *OldPen =(CPen*)m_dcScr.SelectObject(&m_Pen);
	m_dcScr.MoveTo(m_Rect.left,point.y);
	m_dcScr.LineTo(m_Rect.right,point.y);
	m_dcScr.MoveTo(point.x,m_Rect.top);
	m_dcScr.LineTo(point.x,m_Rect.bottom);
	/*if(m_MousePt.x>0)
	{
		m_dcScr.SelectObject(&m_CurPen);
		m_dcScr.MoveTo(m_MousePt);
		m_dcScr.LineTo(point);
	}*/
	m_dcScr.SelectObject(OldPen);
}

void CMeasureMultiLine::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonDown(nFlags, point);
	ShowWindow(SW_HIDE);
	m_pWndParent->PostMessage(WM_CANCELCLICK);
}
