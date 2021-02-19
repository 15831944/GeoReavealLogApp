/******************************************************************
*   Resuldrw.h - declarations of GrDenResult Draw object class    *           
*******************************************************************/


#ifndef __GRDENRESULDRW_H__
#define __GRDENRESULDRW_H__

#include "FIDDraw.h"

class CGrDenResultPoint : public CObject
{                  
public:
	CGrDenResultPoint(CRect rect, COLORREF Color1,COLORREF Color2,COLORREF Color31,COLORREF Color32,
		              LOGFONT GrDenResultFont,CString nGrDenResultNo,
		              CString NoStr,CLogPlotDoc *pDocument);
	void Draw(CDC* pDC);
	CLogPlotDoc *pDoc;
	CRect m_Rect;
	CString m_GrDenResultNo,m_NoStr;
	COLORREF m_Color1,m_Color2,m_Color31,m_Color32;
	LOGFONT m_GrDenResultFont;
	CRect GetRect(){return m_Rect;};
	void SetRect(const CRect& rect){m_Rect = rect;};
};

class CGrDenResultObj : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CGrDenResultObj)
#else
	DECLARE_SERIAL(CGrDenResultObj);
#endif
public:
	CGrDenResultObj();
	CGrDenResultObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument);
	~CGrDenResultObj();

//Attributes
public:    
	COLORREF m_GrDenResultOrderColor;
	CObList m_GrDenResultPoint;
	WORD m_GrDenResultLR;
	LOGFONT m_GrDenResultOrderFont; 
private:
	BOOL bRButtonDown;

//operations
	virtual void CreatePoint();

//Implementation
public:
	virtual int HitTest(CView* pView,const CPoint& point);
	virtual void Serialize(CArchive& ar);
	virtual void DrawHead(CDC *pDC);	
	virtual void DrawClient(CDC *pDC);	
	virtual BOOL Properties(CView* pView);
	virtual void SetRect(const CRect& rect);
};                

#endif //__RESULDRW_H__