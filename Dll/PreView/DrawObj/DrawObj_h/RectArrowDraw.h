/******************************************************************
*   RectArrowDraw.h - declarations of Rect-Arrow Draw object class*           
*******************************************************************/

#ifndef __RECTARROW_H__
#define __RECTARROW_H__

#include "TextDraw.h"

class CRectArrowObj : public CTextObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CRectArrowObj)
#else
	DECLARE_SERIAL(CRectArrowObj);
#endif
	CRectArrowObj();
public:
	CRectArrowObj(const CRect& rect,CDrawObj* pObj,CLogPlotDoc* pDocument);

// Attributes
protected:
	CRectArrowObj* m_pFocusObj;
	CPoint m_ArrowPoint1,m_ArrowPoint2,m_ArrowPoint3;
	CPoint m_RectPoint[6],m_BezierPoint[7];
	WORD m_nHandle1,m_nHandle2;

// Operations
protected:
	void CreateFocusObj(CView* pView);
	void DeleteFocusObj(CView* pView);
	void CreatePolyPoint();
	void LimitCoor(int nHandle, CPoint& point);

// Implementation
public:
	virtual void Serialize(CArchive& ar);
	virtual void SetRect(const CRect& rect);          
	virtual void Draw(CView* pView, CDC* pDC);
	virtual void DrawFocus(CView* pView);
	virtual void DrawFocusObj(CView* pView);
	virtual void InvalObj(CView* pView);
	virtual int GetHandleCount();
	virtual CPoint GetHandlePoint(int nHandle);
	virtual HCURSOR GetHandleCursor(int nHandle);
	virtual void MoveHandle(CView* pView,int nDragHandle, const CPoint& point);
	virtual int HitTest(CView* pView,const CPoint& point);
	virtual void OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CView* pView, UINT nFlags, const CPoint& point);
};                

#endif // __RECTARROW_H__