/******************************************************************
*   Image3D.h - declarations of Image Draw object class           *           
*******************************************************************/


#ifndef __IMAGE3D_H__
#define __IMAGE3D_H__

#include "FIDDraw.h"
class CPlanePoint : public CObject
{                  
public:
	CPlanePoint(CRect rect,float Dep,float Dip,float Dir,CString Flag,WORD PenWidth,
		COLORREF Color,float LookAngle,double HVr);
	void Draw(CDC* pDC,CLogPlotDoc* pDoc,int StartPos);
	CRect m_Rect;
	CString m_Flag;
	float m_LookAngle;
	float m_Dep,m_Dip,m_Dir;
	COLORREF m_Color; 
	WORD m_PenWidth;
	double m_HVr;
	CRect GetRect(){return m_Rect;};
	void SetRect(const CRect& rect){m_Rect = rect;};
};
class CImage3D : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CImage3D)
#else
	DECLARE_SERIAL(CImage3D);
#endif
public:
	CImage3D();
	CImage3D(CString strName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument);
	~CImage3D();

//Attributes
public:   
	float m_LookAngle;
	int m_StartPos,m_Bit;
	BOOL m_bColor;
	COLORREF m_BkColor;
	LOGFONT m_ScaleFont;
	WORD m_ColorGrade,m_PenWidth;
	short m_LeftValue,m_RightValue;
	CPalette m_Palette;
	WORD m_CtrlVer;
	int m_HorScale;
	CString m_SchemeFile;
	CString m_PlaneName,m_PlotContent;

	CObList m_PlanePoint;
	void CreatePoint();
	void DrawPlane(CDC* pDC);
//Operations
public:
	void SetClipRect(CDC* pDC, CRect rect);
	void CreateImagePalette();
//Implementation
public:
	virtual int HitTest(CView* pView,const CPoint& point);
	virtual void Serialize(CArchive& ar);
	virtual void DrawHead(CDC *pDC);	
	virtual void DrawClient(CDC *pDC);	
	virtual BOOL Properties(CView* pView);
	virtual void OnScaleFont();
};                

#endif //__IMAGE3D_H__