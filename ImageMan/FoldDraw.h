/******************************************************************
*   Folddrw.h - declarations of Foldline Draw object class        *           
*******************************************************************/


#ifndef __FOLDDRW_H__
#define __FOLDDRW_H__

#include "RemDraw.h"

class CFoldObj : public CRemDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CFoldObj)
#else
	DECLARE_SERIAL(CFoldObj);
#endif
	CFoldObj();
	~CFoldObj();
public:
	CFoldObj(const CRect& rect,CDrawObj* pObj,CImageManDoc* pDocument);
	
	int m_nPoints;
	int m_nAllocPoints;
	CPoint* m_Points;
// Attribtues
protected:
	CFoldObj* m_pFocusObj;
	
	BOOL bChangePoint;

// Operations
public:
	void OnEditFold();
	void OnLineArc();
	void AddPartPoint(const CPoint& point);
	void DrawCopy(CView* pView,const CPoint& point);
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
	virtual void InitStatusBar();
	virtual void ShowStatus();
	virtual BOOL Properties(CView* pView);
};                

#endif // __FOLDDRW_H__