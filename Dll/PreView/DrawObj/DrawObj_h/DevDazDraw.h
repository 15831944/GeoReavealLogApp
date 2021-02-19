/******************************************************************
*   DevDazDraw.h - declarations of DevDaz Draw object class       *           
*******************************************************************/


#ifndef __DEVDAZDRAW_H__
#define __DEVDAZDRAW_H__

#include "FIDDraw.h"
#include "Grid.h"

class CDevDazObj : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CDevDazObj)
#else
	DECLARE_SERIAL(CDevDazObj);
#endif
public:
	CDevDazObj();
	CDevDazObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument);
	~CDevDazObj();

	CString m_DazName;
//Attributes
protected:    
	float *daz;
	WORD m_TailLength,m_CircleRadius,m_TailWidth,m_CircleWidth;
	COLORREF m_CircleColor,m_TailColor;
	long m_PointNum;
	float m_DepLevel;
	CPoint* point;
	LOGFONT m_ScaleFont;
	COLORREF m_ScaleColor;

//Implementation
public:
	virtual int HitTest(CView* pView,const CPoint& point);
	virtual void Serialize(CArchive& ar);
	virtual void DrawHead(CDC *pDC);	
	virtual void DrawClient(CDC *pDC);	
	virtual BOOL Properties(CView* pView);
//Operations	
protected:
	void CreatePoint();
public:
	friend class CDevDazPage;
};                

#endif //__DEVDAZDRAW_H__