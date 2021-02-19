/******************************************************************
*   Resuldrw.h - declarations of MakResult Draw object class         *           
*******************************************************************/


#ifndef __MAKRESULDRW_H__
#define __MAKRESULDRW_H__

#include "FIDDraw.h"

class CMakResultPoint : public CObject
{                  
public:
	CMakResultPoint(CRect rect, COLORREF MakResultColor,LOGFONT MakResultFont,CString nMakResultNo,
		CString NoStr,int nDirect,CLogPlotDoc* pDocumnet);
	void Draw(CDC* pDC);
	CLogPlotDoc* pDoc;
	CRect m_Rect;
	CString m_MakResultNo,m_NoStr;
	WORD m_Direct;
	COLORREF m_MakResultColor;
	LOGFONT m_MakResultFont;
	CRect GetRect(){return m_Rect;};
	void SetRect(const CRect& rect){m_Rect = rect;};
};

class CMakResultObj : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CMakResultObj)
#else
	DECLARE_SERIAL(CMakResultObj);
#endif
public:
	CMakResultObj();
	CMakResultObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument);
	~CMakResultObj();

//Attributes
public:    
	COLORREF m_MakResultOrderColor;
	CObList m_MakResultPoint;
	WORD m_MakResultLR;
	LOGFONT m_MakResultOrderFont; 
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