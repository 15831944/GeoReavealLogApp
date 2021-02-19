/******************************************************************
*   Resuldrw.h - declarations of DSC Draw object class         *           
*******************************************************************/


#ifndef __DSCDRW_H__
#define __DSCDRW_H__

#include "FIDDraw.h"
class CDSCObj;
class CLogPlotDoc;
class CDSCPoint : public CObject
{                  
public:
	CDSCPoint(CRect rect,CDSCObj* pObj,CLogPlotDoc* pDocument);
	void Draw(CDC* pDC);
	CLogPlotDoc* pDoc;
	CRect m_Rect;
	CDSCObj* m_pObj;
	CRect m_GridRect,m_ListRect;
	void DrawGrid(CDC *pDC);
	void DrawLine(CDC *pDC);
	void DrawList(CDC *pDC);
	CString m_sCurveName;
	int m_FilterNum,m_DetectNo,m_Num,m_nps;
	float m_Dep;//≤‚æÆµ„…Ó∂»
	float m_Rlev,m_StDep,m_EnDep;
	CString m_sDBFWell,m_sDBFDate,m_sDBFFluid;
	float m_Sttime,m_Velocity,m_Flux;
	BOOL InitDrawObj();
	CRect GetRect(){return m_Rect;};
	void SetRect(const CRect& rect){m_Rect = rect;};
};

class CDSCObj : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CDSCObj)
#else
	DECLARE_SERIAL(CDSCObj);
#endif
public:
	CDSCObj();
	CDSCObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument);
	~CDSCObj();

//Attributes
public:    
	CObList m_DSCPoint;	
	CString m_XText,m_YText;
	LOGFONT m_TitleTextFont,m_AxiasTextFont,m_ScaleTextFont,m_LegendFont;
	COLORREF m_TitleTextColor,m_AxiasTextColor,m_ScaleTextColor,m_LegendColor;
	COLORREF m_ColorPen[5];
	COLORREF m_Color;
	float m_YMin,m_YMax,m_XMin,m_XMax;
	BOOL m_bD[5];
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