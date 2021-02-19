/******************************************************************
*   OleDrw.h - declarations of the Ole Draw object class          *           
*******************************************************************/


#ifndef __OLEDRW_H__
#define __OLEDRW_H__

#include "RemDraw.h"

class CDrawItem;    // COleClientItem derived class

class COleObj : public CRemDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(COleObj)
#else
	DECLARE_SERIAL(COleObj);
#endif
	COleObj();
	~COleObj();

public:
	COleObj(const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocment);
	
protected:	
	COleObj* m_pFocusObj;

// Implementation
public:
	virtual void Remove();
	virtual void Serialize(CArchive& ar);
	virtual void Draw(CView* pView, CDC* pDC);
	virtual BOOL Properties(CView* pView);
	virtual void CreateFocusObj(CView* pView);
	virtual void DeleteFocusObj(CView* pView);
	virtual void DrawFocus(CView* pView);
	virtual void DrawFocusObj(CView* pView);
	virtual CDrawObj* GetFocusObj();
	virtual void OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CView* pView, UINT nFlags, const CPoint& point);

	static BOOL c_bShowItems;

	CDrawItem* m_pClientItem;
	CSize m_extent; // current extent is tracked separate from scaled position
};

#endif // __OLEDRW_H__