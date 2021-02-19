/*************************************************************************
*   ResultListDrw.h - declarations of ResultList Draw object class       *           
**************************************************************************/


#ifndef __RESULTLISTDRW_H__
#define __RESULTLISTDRW_H__

#include "TextDraw.h"

class CResultListObj : public CTextObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CResultListObj)
#else
	DECLARE_SERIAL(CResultListObj);
#endif
	CResultListObj();
public:
	CResultListObj(const CRect& rect,CLogPlotDoc* pDocument);
// Attributes
	COLORREF m_TitleFontColor,m_HeadFontColor,m_UnitFontColor,m_BodyFontColor,m_NoteFontColor;
	LOGFONT m_TitleFont,m_HeadFont,m_UnitFont,m_BodyFont,m_NoteFont;
	CString m_WellName;
protected:
	CString m_OkText;
	CString m_TitleText,m_NoteText;
	int m_TopSpace;
	int m_LeftSpace;
	int m_RemHight;
	int m_HeadCY;
	int m_BodyCY;
	int ResultSum;
	int m_TitleNum[200],m_BodyNum[200],m_RowNum;
		
// Operation
protected:
	BOOL OnBrowseWell();	
	int ComputerTextHeight();

// Implementation
public:
	void ReadFile();
	virtual BOOL Properties(CView* pView);
	virtual void Serialize(CArchive& ar);
	virtual void SetRect(const CRect& rect);
	virtual void DrawTracker(CView* pView, CDC* pDC, TrackerState state);
	virtual void Draw(CView* pView, CDC* pDC);
	virtual int HitTest(CView* pView,const CPoint& point);
	virtual BOOL OnSetCursor(CView* pView,const CPoint& point);
};                

#endif // __RESULLISTDRW_H__