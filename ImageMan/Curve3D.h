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
	CCurve3D(CString strName, const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDocument);
	~CCurve3D();

//Attributes
public:   
	float m_LookAngle;
	int m_StartPos,m_HorScale,m_Bit;
	BOOL m_bColor;
	WORD m_CurveWidth;
	COLORREF m_CurveColor;
	LOGFONT m_ScaleFont;
	WORD m_ColorGrade,m_CrMode;
	short m_LeftValue,m_RightValue;
	BOOL m_Daz,m_PoleArray;
	int m_FilterType;
	CPalette m_Palette;
	int m_Palsize;
	int m_Space,m_PointNum;
	CStringArray m_CurveArray;
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