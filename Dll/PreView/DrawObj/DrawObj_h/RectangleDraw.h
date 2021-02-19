/******************************************************************
*   RectDrw.h - declarations of Rectangle Draw object class       *           
*******************************************************************/


#ifndef __RECTDRW_H__
#define __RECTDRW_H__

#include "TextDraw.h"

class CRectangleObj : public CTextObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CRectangleObj)
#else
	DECLARE_SERIAL(CRectangleObj);
#endif
	CRectangleObj();
public:
	CRectangleObj(const CRect& rect,CDrawObj* pObj,CLogPlotDoc* pDocument);

// Attributes
protected:
	CRectangleObj* m_pFocusObj;

// Operations
protected:
	void CreateFocusObj(CView* pView);
	void DeleteFocusObj(CView* pView);

// Implementation
public:
	virtual void Serialize(CArchive& ar);
	virtual void Draw(CView* pView, CDC* pDC);
	virtual void DrawFocus(CView* pView);
	virtual void DrawFocusObj(CView* pView);
	virtual int HitTest(CView* pView,const CPoint& point);
	virtual void OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CView* pView, UINT nFlags, const CPoint& point);
};                

#endif // __RECTDRW_H__