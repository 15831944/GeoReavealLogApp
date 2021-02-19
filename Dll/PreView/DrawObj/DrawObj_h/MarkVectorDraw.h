/******************************************************************
*   MarkVectorObj.h - declarations of DandDdir Draw object class    *           
*******************************************************************/


#ifndef __MARKVECTORDRAW_H__
#define __MARKVECTORDRAW_H__

#include "FIDDraw.h"
#include "Grid.h"

class CVectorPoint : public CObject
{                  
public:
	CVectorPoint(CPoint Point,float Dep,float Dip,float Dir,CString Flag,COLORREF Color,BOOL Origin);
	void Draw(CDC* pDC,CLogPlotDoc* pDoc);
	CRect m_Rect;
	CPoint m_Point;
	CString m_Flag;
	float m_Dep,m_Dip,m_Dir;
	WORD m_TailLength,m_TailWidth,m_CircleRadius;
	COLORREF m_Color;
	BOOL m_PrintPara,m_Origin;
	CString m_ParaFormat;
};

class CMarkVectorObj : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CMarkVectorObj)
#else
	DECLARE_SERIAL(CMarkVectorObj);
#endif
public:
	CMarkVectorObj();
	CMarkVectorObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument);
	~CMarkVectorObj();
	float m_MinGrade;
	CString	m_PlotContent;
	CString m_DirName;
	WORD m_TailLength,m_CircleRadius,m_TailWidth,m_CircleWidth;
	COLORREF m_CircleColor,m_TailColor;
	int m_HeadStyle;
	CString m_ParaFormat;
	WORD m_DepLevel;

	BOOL m_bPrintOriginPara,m_bPrintTruePara;
	BOOL m_bPrintTrueDip,m_bPrintOriginDip;
	CObList m_VectorPoint;
	void DrawVector(CDC* pDC);
	void CreatePoint();
//Attributes
protected:    
	LOGFONT m_ScaleFont;
	CGridObj* m_pGrid;
	
//Implementation
public:
	virtual void Serialize(CArchive& ar);
	virtual void DrawHead(CDC *pDC);	
	virtual void DrawClient(CDC *pDC);	
	virtual BOOL Properties(CView* pView);
	virtual void SetRect(const CRect& rect);
	virtual void SetDepth(double sdep,double edep);
	virtual void OnChangeDepC(double depc, int flag);
//Operations	
protected:
	
public:
	unsigned int m_DrawLevel;
 	friend class CDipPropSheet;
};                

#endif 