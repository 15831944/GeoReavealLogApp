/******************************************************************
*   Curve3D.h - declarations of Curve Draw object class           *           
*******************************************************************/


#ifndef __CURVE3D_H__
#define __CURVE3D_H__

#include "FIDDraw.h"

class CCurve3D : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CCurve3D)
#else
	DECLARE_SERIAL(CCurve3D);
#endif
public:
	CCurve3D();
	CCurve3D(CString strName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument);
	~CCurve3D();

//Attributes
public:   
	float m_LookAngle,m_CurVal0,m_CurVal1;
	int m_StartPos;
	BOOL m_bColor;
	WORD m_CurveWidth;
	COLORREF m_Color;
	LOGFONT m_ScaleFont;
	WORD m_ColorGrade,m_CrMode;
	float m_LeftValue,m_RightValue;
	UINT m_PlotType;
	BOOL m_PoleArray;
	int m_FilterType;
	CPalette m_Palette;
	int m_Space,m_PointNum;
	CStringArray m_CurveArray;
	CString m_SchemeFile;
//Operations
public:
	void SetClipRect(CDC* pDC, CRect rect);
	void CreateCurvePalette();
//Implementation
public:
	virtual int HitTest(CView* pView,const CPoint& point);
	void CompuePlotPara();
	virtual void Serialize(CArchive& ar);
	virtual void Draw(CView* pView,CDC* pDC);
	virtual void DrawHead(CDC *pDC);	
	virtual void DrawClient(CDC *pDC);	
	virtual BOOL Properties(CView* pView);
	virtual void OnScaleFont();
	virtual void OnLButtonDblClk(CView* pView, UINT nFlags, const CPoint& point);
};                

#endif 