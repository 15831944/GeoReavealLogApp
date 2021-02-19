/******************************************************************
*   Polydrw.h - declarations of Polyline Draw object class        *           
*******************************************************************/


#ifndef __POLYDRW_H__
#define __DOLYDRW_H__

#include "RemDraw.h"

class CPolyObj : public CRemDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CPolyObj)
#else
	DECLARE_SERIAL(CPolyObj);
#endif
	CPolyObj();
	~CPolyObj();
public:
	CPolyObj(const CRect& rect,CDrawObj* pObj,CLogPlotDoc* pDocument);

// Attribtues
protected:
	CPolyObj* m_pFocusObj;
	int m_nPoints,m_nBezierPoints;
	int m_nAllocPoints;
	CPoint* m_Points,*m_BezierPoints;
	BOOL bChangePoint,m_bBezier;

// Operations
public:
	void OnEditPoly();
	void OnLineArc();
	void CreateBezierPoint();
protected:
	void AddPoint(const CPoint& point, CView* pView = NULL);
	CPoint* NewPoints(int nPoints);
	BOOL RecalcBounds();
	void CreateFocusObj(CView* pView);
	void DeleteFocusObj(CView* pView);

// Implementation 
public:
	virtual void Serialize(CArchive& ar);
	virtual void SetRect(const CRect& rect);          
	virtual void Draw(CView* pView, CDC* pDC);
	virtual void DrawFocus(CView* pView);
	virtual void DrawFocusObj(CView* pView);
	virtual void SetSelect(CView* pView,BOOL bf);
	virtual int GetHandleCount();
	virtual CPoint GetHandlePoint(int nHandle);
	virtual HCURSOR GetHandleCursor(int nHandle);
	virtual void MoveHandle(CView* pView,int nDragHandle, const CPoint& point);
	virtual int HitTest(CView* pView,const CPoint& point);
	virtual void OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDblClk(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CView* pView, UINT nFlags, const CPoint& point);
};                

#endif // __POLYDRW_H__