/******************************************************************
*   Resuldrw.h - declarations of Curve Draw object class          *           
*******************************************************************/


#ifndef __LTHNOTEDRW_H__
#define __LTHNOTEDRW_H__

#include "FIDDraw.h"

class CLthNotePoint : public CObject
{                  
public:
	CLthNotePoint(CRect rect, COLORREF LthNoteColor,LOGFONT LthNoteFont,CString NoteStr,CLogPlotDoc* pDocumnet);
	void Draw(CDC* pDC);
	CLogPlotDoc* pDoc;
	CRect m_Rect;
	CString m_NoteStr;
	COLORREF m_LthNoteColor;
	LOGFONT m_LthNoteFont;
	CRect GetRect(){return m_Rect;};
	void SetRect(const CRect& rect){m_Rect = rect;};
};

class CLthNoteObj : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CLthNoteObj)
#else
	DECLARE_SERIAL(CLthNoteObj);
#endif
public:
	CLthNoteObj();
	CLthNoteObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument);
	~CLthNoteObj();

//Attributes
public:    
	CObList m_LthNotePoint;
	COLORREF m_LthNoteColor;
	LOGFONT m_LthNoteFont; 
//operations
	virtual void CreatePoint();
	virtual int HitTest(CView* pView,const CPoint& point);
//Implementation
public:
	virtual void Serialize(CArchive& ar);
	virtual void DrawHead(CDC *pDC);	
	virtual void DrawClient(CDC *pDC);	
	virtual void SetRect(const CRect& rect);
	virtual BOOL Properties(CView* pView);
};                

#endif //__LTHDRW_H__