/******************************************************************
*   RemDraw.cpp - implementation of the rem draw object class     *           
*******************************************************************/

#include "stdafx.h" 
#include "ImageMan.h"
#include "ImageManDoc.h"
#include "Style.h"
#include "StyleDialog.h"
#include "LineListPage.h"
#include "FillListPage.h"
#include "RemDraw.h"

IMPLEMENT_SERIAL(CRemDrawObj, CDrawObj, 0)
CRemDrawObj::CRemDrawObj()
{
	bRButtonDown = FALSE;
	m_LogBrush.lbStyle = BS_SOLID;
	m_LogBrush.lbColor = RGB(255, 255, 255);
	m_LogBrush.lbHatch = HS_HORIZONTAL;
	m_LogPen.lopnStyle = PS_INSIDEFRAME;
	m_LogPen.lopnWidth.x = 1;
	m_LogPen.lopnWidth.y = 1;
	m_LogPen.lopnColor = 0xff00ff;
}

CRemDrawObj::CRemDrawObj(const CRect& rect, CDrawObj* pParentObj,CImageManDoc* pDocument)
					 	: CDrawObj(rect,pParentObj,pDocument)
{
	bRButtonDown = FALSE;
	m_LogBrush.lbStyle = BS_SOLID;
	m_LogBrush.lbColor = RGB(255, 255, 255);
	m_LogBrush.lbHatch = HS_HORIZONTAL;
	m_LogPen.lopnStyle = PS_INSIDEFRAME;
	m_LogPen.lopnWidth.x = 1;
	m_LogPen.lopnWidth.y = 1;
	m_LogPen.lopnColor = 0xff00ff;
}

BOOL CRemDrawObj::Properties(CView* pView)
{
	CString szHT;
	szHT.LoadString(IDS_DRAWINGPROPERTIES);
	CPropertySheet m_Sheet(szHT);
 	CLineListPage m_LinePage;
	m_LinePage.AddPen(&m_LogPen,m_ShapeName);
 	CFillListPage m_FillPage;
	m_FillPage.AddFill(&m_LogBrush,m_ShapeName);
	m_Sheet.AddPage(&m_FillPage);
	m_Sheet.AddPage(&m_LinePage);
	if (m_Sheet.DoModal() == IDOK)
	{
		Invalidate(pView);
		return TRUE;
	}	
	return FALSE;
}

void CRemDrawObj::OnFill()
{
 	CPatternPage m_Page(&m_LogBrush);
	CString szHT;
	szHT.LoadString(IDS_FILLPROPERTIES);
	CPropertySheet m_Sheet(szHT);
	m_Sheet.AddPage(&m_Page);
	if (m_Sheet.DoModal() == IDOK)
		Invalidate();
}

void CRemDrawObj::OnLine()
{       
 	CLinePage m_Page(&m_LogPen);
	CPropertySheet m_Sheet("Ö±ÏßÊôÐÔ");
	m_Sheet.AddPage(&m_Page);
	if (m_Sheet.DoModal() == IDOK)
		Invalidate();
}

void CRemDrawObj::Serialize(CArchive& ar)
{
	CDrawObj::Serialize(ar);
	if(!ar.IsStoring())
	{
		pDoc = (CImageManDoc*)ar.m_pDocument;
		ASSERT_VALID(pDoc);
		ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CImageManDoc)));
		SerializeLogPen(ar,m_LogPen);
		SerializeLogBrush(ar,m_LogBrush);
	}
}

void CRemDrawObj::SetRect(const CRect& rect)
{
	CPoint point; 
	CRect rect1 = rect;
	rect1.NormalizeRect();
	if (m_SelectMode != size)
	{
		point.x = rect1.left - m_Rect.left;
		point.y = rect1.top - m_Rect.top;
		m_Rect = rect1;
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*) m_ObjList.GetNext(pos);
			if (pObj != NULL) pObj->SetRect(pObj->GetRect()+point);
		}
	}
	else m_Rect = rect1;
}

void CRemDrawObj::OnChangeDepC(double depc, int flag)
{
	CPoint pt(0,0);
	int y = (m_Rect.top + m_Rect.bottom) / 2;
	pt.y = (int) ((depc / pDoc->m_DepC -1) * y + 0.5);
	m_Rect += pt; 
	CDrawObj::OnChangeDepC(depc,flag);
}

void CRemDrawObj::AlignToGrid(const CPoint& point, const CSize& size)
{
	CRect rect0 = m_Rect;
	if (size.cx != 0)
	{
		m_Rect.left = point.x + ((int) ((m_Rect.left - point.x) * 1.0 / size.cx + 0.5)) * size.cx;           
		m_Rect.right = point.x + ((int) ((m_Rect.right - point.x) * 1.0 / size.cx + 0.5)) * size.cx;           
	}
	if (size.cy != 0)
	{
		m_Rect.top = point.y + ((int) ((m_Rect.top - point.y) * 1.0 / size.cy + 0.5)) * size.cy;           
		m_Rect.bottom = point.y + ((int) ((m_Rect.bottom - point.y) * 1.0 / size.cy + 0.5)) * size.cy;           
	}
	if ((m_Rect.Height() == 0) && (m_Rect.Width() == 0))
	{
		if (rect0.Height() > rect0.Width())
			m_Rect.bottom += size.cy;
		else
			m_Rect.right += size.cx;
	}
	CDrawObj::AlignToGrid(point,size);
}
