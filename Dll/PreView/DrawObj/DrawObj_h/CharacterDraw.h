/******************************************************************
*   CharacterDrw.h - declarations of Character Draw object class  *           
*******************************************************************/


#ifndef __CHARDRW_H__
#define __CHARDRW_H__

#include "TextDraw.h"

class CharacterObj : public CTextObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CharacterObj)
#else
	DECLARE_SERIAL(CharacterObj);
#endif
	CharacterObj();
public:
	CharacterObj(const CRect& rect,CDrawObj* pObj,CLogPlotDoc* pDocument);

// Attributes
protected:
	CharacterObj* m_pFocusObj;

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
	virtual BOOL Properties(CView* pView);
};                

#endif // __CHARDRW_H__