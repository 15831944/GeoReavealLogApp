/******************************************************************
*   Resuldrw.h - declarations of Yield Draw object class         *           
*******************************************************************/


#ifndef __YIELDDRW_H__
#define __YIELDDRW_H__

#include "FIDDraw.h"

class CYieldPoint : public CObject
{                  
public:
	CYieldPoint(CRect rect, COLORREF VittaColor,COLORREF BkColor,COLORREF PenColor,CLogPlotDoc* pDocumnet);
	void Draw(CDC* pDC);
	CLogPlotDoc* pDoc;
	CRect m_Rect;
	CString m_YieldNo,m_NoStr;
	COLORREF m_VittaColor,m_BkColor,m_PenColor;
	CRect GetRect(){return m_Rect;};
	void SetRect(const CRect& rect){m_Rect = rect;};
};

class CYieldObj : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CYieldObj)
#else
	DECLARE_SERIAL(CYieldObj);
#endif
public:
	CYieldObj();
	CYieldObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument);
	~CYieldObj();
	float m_VittaDep;
//Attributes
public:    
	COLORREF m_VittaColor;
	CObList m_YieldPoint;
private:

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