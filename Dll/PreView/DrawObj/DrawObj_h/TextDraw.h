/******************************************************************
*   TextDrw.h - declarations of Text Draw object base class       *           
*******************************************************************/

#ifndef __TEXTDRW_H__
#define __TEXTDRW_H__

#include "RemDraw.h"

class CTextObj : public CRemDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CTextObj)
#else
	DECLARE_SERIAL(CTextObj);
#endif
	CTextObj();
	~CTextObj();
public:
	CTextObj(const CRect& rect,CDrawObj* pObj,CLogPlotDoc* pDocument);
	
//Attributes          
public:
	BOOL bInput;
	BOOL m_bClip,m_bRotate;
	WORD m_TextHoriAlignMode,m_TextVertAlignMode;
	CStringArray m_Text;
	COLORREF m_TextColor;
	LOGFONT m_TextFont;
protected:
//	CTextObj* m_pFocusObj;
	CBrush hatchBrush;
			
	BOOL bShowCaret,bCaretExist;
	CSize m_CharSize;
	CRect m_TextRect,m_TextRect0,m_Rect0;
	int m_Row,m_Col;
	CPoint m_CaretPos;
// Operations
public:
	BOOL OnKeyDown(CView* pViewm, UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL OnChar(CView* pView, UINT nChar, UINT nRepCnt, UINT nFlags);
	void KillFocus(CView* pView);
protected:
	BOOL KeyUp();
	BOOL KeyDown();
	BOOL KeyHome();
	BOOL KeyEnd();
	BOOL KeyLeft();
	BOOL KeyRight();
	BOOL KeyDelete();
	BOOL IsTextEmpty();         
	void SetInput(CView* pView,BOOL bf, CPoint pt);
	void InvalText(CView* pView);
	int SetCol(CDC* pDC,int x);
	int SetRow(CDC* pDC,int y);
	void GetCaretPos(CDC* pDC);
	void SetCaretPos(CView* pView,CPoint point);
	void ComputerTextRect(CDC* pDC);
	void DeleteChar(int row,int col);
	void CreateFocusObj(CView* pView);
	void DeleteFocusObj(CView* pView);
	
// Implementation
public:
	virtual void Serialize(CArchive& ar);
	virtual void SetSelect(CView* pView,BOOL bf);
	virtual void DrawTracker(CView* pView, CDC* pDC, TrackerState state);
	virtual void DrawFocus(CView* pView);
	virtual int HitTest(CView* pView,const CPoint& point);
	virtual CPoint GetHandlePoint(int nHandle);
	virtual void Draw(CView* pView, CDC* pDC);
	virtual void InvalObj(CView* pView);
	virtual void OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point);
	virtual BOOL OnSetCursor(CView* pView,const CPoint& point);
	virtual BOOL Properties(CView* pView);
};                

#endif // __TEXTDRW_H__