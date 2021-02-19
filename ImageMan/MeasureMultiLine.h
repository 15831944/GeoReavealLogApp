// MeasureMultiLine.h: interface for the CMeasureMultiLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEASUREMULTILINE_H__B07B34C0_EC78_492D_A942_D75D571726A9__INCLUDED_)
#define AFX_MEASUREMULTILINE_H__B07B34C0_EC78_492D_A942_D75D571726A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_ENDCLICK (WM_USER+100)
#define WM_CANCELCLICK (WM_USER+101)

class CMeasureMultiLine : public CWnd 
{
public:
	void CreateMeasure(LPCTSTR lpTitle,CWnd* pWnd,CRect rect);
	CMeasureMultiLine();
	~CMeasureMultiLine();
	CArray<CPoint, CPoint> m_arrayPoints;

	CPoint m_MousePt;
	CDC m_dcMem;
	CDC m_dcScr;
	COLORREF m_colorTag;
//	CDC m_dcMem;
protected:	
	CRect m_Rect;
	COLORREF m_colorFont;
	COLORREF m_colorLine;
	CWnd* m_pWndParent;

	CPen m_Pen,m_CurPen;
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_MEASUREMULTILINE_H__B07B34C0_EC78_492D_A942_D75D571726A9__INCLUDED_)
