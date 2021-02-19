/******************************************************************
*   RoundDrw.h - declarations of RoundRect Draw object class      *           
*******************************************************************/

#ifndef __ROUNDDRW_H__
#define __ROUNDDRW_H__

#include "TextDraw.h"

class CRoundRectObj : public CTextObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CRoundRectObj)
#else
	DECLARE_SERIAL(CRoundRectObj);
#endif
	CRoundRectObj();
public:
	CRoundRectObj(const CRect& rect,CDrawObj* pObj,CLogPlotDoc* pDocument);

// Attributes
protected:
	CRoundRectObj* m_pFocusObj;
	CPoint m_Roundness;

// Operations
protected:
	void CreateFocusObj(CView* pView);
	void DeleteFocusObj(CView* pView);

// Implementation
public:
	virtual void Serialize(CArchive& ar);
	virtual void SetRect(const CRect& rect);          
	virtual void Draw(CView* pView, CDC* pDC);
	virtual void DrawFocus(CView* pView);
	virtual void DrawFocusObj(CView* pView);
	virtual int GetHandleCount();
	virtual CPoint GetHandlePoint(int nHandle);
	virtual HCURSOR GetHandleCursor(int nHandle);
	virtual void MoveHandle(CView* pView,int nDragHandle, const CPoint& point);
	virtual int HitTest(CView* pView,const CPoint& point);
	virtual void OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CView* pView, UINT nFlags, const CPoint& point);
};                

#endif // __ROUNDDRW_H__