/******************************************************************
*   DepSect.h - declarations of Line Draw object                  *           
*******************************************************************/

#ifndef __LINEDRW_H__
#define __LINEDAW_H__

#include "RemDraw.h"

class CDepSect : public CRemDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CDepSect)
#else
	DECLARE_SERIAL(CDepSect);
#endif
	CDepSect();
public:
	CDepSect(const CRect& rect,CDrawObj* pObj,CImageManDoc* pDocument);

// Attribtues
protected:
	CDepSect* m_pFocusObj;
public:
	CPoint m_Point1,m_Point2;
	BOOL m_bMove;
// Operations
protected:
	void CreateFocusObj(CView* pView);
	void DeleteFocusObj(CView* pView);

// Implementation 
public:
	virtual void Serialize(CArchive& ar);
	virtual void Normallize();
	virtual void SetRect(const CRect& rect);          
	virtual void ZoomRect(const CRect& sRect, const CRect& zRect);
	virtual void AlignToGrid(const CPoint& point, const CSize& size);
	virtual void Draw(CView* pView, CDC* pDC);
	virtual void DrawFocus(CView* pView);
	virtual void DrawFocusObj(CView* pView);
	virtual int HitTest(CView* pView,const CPoint& point);
	virtual int GetHandleCount();
	virtual CPoint GetHandlePoint(int nHandle);
	virtual HCURSOR GetHandleCursor(int nHandle);
	virtual void MoveHandle(CView* pView,int nDragHandle, const CPoint& point);
	virtual void OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CView* pView, UINT nFlags, const CPoint& point);
	void InitStatusBar();
	void ShowStatus();
	virtual BOOL Properties(CView* pView);
};                

#endif // __LINEDRW_H__