/******************************************************************
*   AzimFreqObj.h - declarations of DandDdir Draw object class    *           
*******************************************************************/


#ifndef __AZIMFREQDRAW_H__
#define __AZIMFREQDRAW_H__

#include "FIDDraw.h"

class CAzimFreqObj : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CAzimFreqObj)
#else
	DECLARE_SERIAL(CAzimFreqObj);
#endif
public:
	CAzimFreqObj();
	CAzimFreqObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDocument);
	~CAzimFreqObj();

	CString m_AziName;
//Attributes
public:    
	float *ddir,*dang;
	WORD m_PointNum;
	WORD m_AzimNum, m_DangNum, m_DepLevel;
	WORD m_CircleWidth,m_PieWidth,m_DangBaseLineWidth,m_DangFreqLineWidth;
	COLORREF m_CircleColor,m_PieColor,m_DangBaseLineColor,m_DangFreqLineColor;
	WORD m_DangBaseLineLength,m_DangFreqLineLength,m_CircleRadius;
	LOGBRUSH m_PieBrush;
	int m_AzimFreq[50];
	int m_DangFreq[50];
	int m_MaxAzimFreq;
	int m_MaxDangFreq;

//Implementation
public:
	virtual int HitTest(CView* pView,const CPoint& point);
	virtual void Serialize(CArchive& ar);
    virtual void DrawClient(CDC* pDC);
	virtual void DrawHead(CDC *pDC);	
	virtual BOOL Properties(CView* pView);
	void SetClipRect(CDC* pDC, CRect rect);
//Operations	
protected:
	void CreatePoint();
};

#endif 