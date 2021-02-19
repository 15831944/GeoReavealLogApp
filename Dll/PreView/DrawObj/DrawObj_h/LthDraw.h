/******************************************************************
*   Resuldrw.h - declarations of Curve Draw object class          *           
*******************************************************************/


#ifndef __LTHDRW_H__
#define __LTHDRW_H__

#include "FIDDraw.h"

class CLthPoint : public CObject
{                  
public:
	CLthPoint(CRect rect, int LithNo,unsigned long ColorNo,int LithWidth,CLogPlotDoc* pDocument );
	void Draw(CDC* pDC,COLORREF LithColor=0x0);
	CRect m_Rect;
	unsigned long m_ColorNo;
	WORD m_LithNo,m_LithWidth;                                     
	CLogPlotDoc* pDoc;
	CRect GetRect(){return m_Rect;};
	void SetRect(const CRect& rect){m_Rect = rect;};
};

class CLthObj : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CLthObj)
#else
	DECLARE_SERIAL(CLthObj);
#endif
public:
	CLthObj();
	CLthObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument);
	~CLthObj();

//Attributes
public:    
	CObList m_LthPoint;
	int m_LithWidth;
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