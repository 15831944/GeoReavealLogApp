/******************************************************************
*   Coredrw.h - declarations of Core Draw object class            *           
*******************************************************************/


#ifndef __COREDRW_H__
#define __COREDRW_H__

#include "FIDDraw.h"

class CCorePoint : public CObject
{                  
public:
	CCorePoint(CPoint point, int nLthNo, int nOGLNo, int nDirect,CLogPlotDoc* pDocument);
	void Draw(CDC* pDC);
	CLogPlotDoc* pDoc;
	CPoint m_Point;
	WORD m_LthNo, m_OGLNo;                                     
	WORD m_Direct;
	CRect m_Rect;
	CPoint GetPoint(){return m_Point;};
	CRect GetRect(){return m_Rect;};
	void SetPoint(const CPoint& point);
};

class CCoreObj : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CCoreObj)
#else
	DECLARE_SERIAL(CCoreObj);
#endif
public:
	CCoreObj();
	CCoreObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument);
	~CCoreObj();

//Attributes
public:    
	CObList m_CorePoint;
	WORD m_CoreLR;

//operations
	virtual void CreatePoint();

//Implementation
public:
	virtual void Serialize(CArchive& ar);
	virtual void DrawHead(CDC *pDC);	
	virtual void DrawClient(CDC *pDC);	
	virtual void SetRect(const CRect& rect);
//	virtual int HitTest(CView* pView,const CPoint& point);
};                

#endif //__COREDRW_H__