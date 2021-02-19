/******************************************************************
*   ElliDrw.h - declarations of Ellipse Draw object class         *           
*******************************************************************/


#ifndef __ELLIDRW_H__
#define __ELLIDRW_H__

#include "TextDraw.h"

class CEllipseObj : public CTextObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CEllipseObj)
#else
	DECLARE_SERIAL(CEllipseObj);
#endif
	CEllipseObj();
public:
	CEllipseObj(const CRect& rect,CDrawObj* pObj,CLogPlotDoc* pDocument);

// Attributes
protected:
	CEllipseObj* m_pFocusObj;

// Operations
protected:
	void CreateFocusObj(CView* pView);
	void DeleteFocusObj(CView* pView);

// Implementation
public:
	virtual void Serialize(CArchive& ar);
	virtual int HitTest(CView* pView,const CPoint& point);
	virtual void Draw(CView* pView, CDC* pDC);
	virtual void DrawFocus(CView* pView);
	virtual void DrawFocusObj(CView* pView);
	virtual void OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CView* pView, UINT nFlags, const CPoint& point);
};                

#endif //__ELLIDRW_H__