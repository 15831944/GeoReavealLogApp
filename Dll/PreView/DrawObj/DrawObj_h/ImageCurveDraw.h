/******************************************************************
*   ImageDraw.h - declarations of Image Draw object class           *           
*******************************************************************/


#ifndef __IMAGECURVEDRAW_H__
#define __IMAGECURVEDRAW_H__

#include "FIDDraw.h"

class CImageCurveObj : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CImageCurveObj)
#else
	DECLARE_SERIAL(CImageCurveObj);
#endif
public:
	CImageCurveObj();
	CImageCurveObj(CString strName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument);
	~CImageCurveObj();

	int *m_NoNum;
//Attributes
public: 
	int m_HorOff;
	CString m_sCurveArray;
	COLORREF m_CurveColor,m_FillColor;
	LOGFONT m_ScaleFont;
	int m_LeftValue,m_RightValue;
	int m_ScaleType,m_GradNum;
	float m_HorOffVal;
	BOOL m_bGDIPlus;
//Operations
public:
	int GetDrawCurveNum();
	void SetClipRect(CDC* pDC, CRect rect);
	virtual int HitTest(CView* pView,const CPoint& point);
//Implementation
public:
	void DrawClientGDI(CDC *pDC);
	void DrawClientGDIPlus(CDC *pDC);
	virtual void Serialize(CArchive& ar);
	virtual void DrawHead(CDC *pDC);	
	virtual void DrawClient(CDC *pDC);	
	virtual BOOL Properties(CView* pView);
	virtual void OnScaleFont();
};                

#endif //__IMAGEDRAW_H__