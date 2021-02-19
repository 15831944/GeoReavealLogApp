/******************************************************************
*   ArrowDrw.h - declarations of Arrow  Draw object class         *           
*******************************************************************/


#ifndef __ARROWDRW_H__
#define __ARROWDRW_H__

#include "TextDraw.h"

class CArrowObj : public CTextObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CArrowObj)
#else
	DECLARE_SERIAL(CArrowObj);
#endif
	CArrowObj();
public:
	CArrowObj(const CRect& rect,CDrawObj* pObj,CLogPlotDoc* pDocument);

// Attributes
protected:
	CArrowObj* m_pFocusObj;

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

#endif // __ARROWDRW_H__