/******************************************************************
*   StratumDraw.h - declarations of Stratum Draw object class       *           
*******************************************************************/


#ifndef __STRATUMDRAW_H__
#define __STRATUMDRAW_H__

#include "FIDDraw.h"
#include "Grid.h"

class CSinPoint : public CObject
{                  
public:
	CSinPoint(CRect rect,float Dep,float Dip,float Dir,CString Flag,WORD PenWidth,COLORREF Color);
	void Draw(CDC* pDC,CLogPlotDoc* pDoc);
	CRect m_Rect;
	CString m_Flag;
	float m_Dep,m_Dip,m_Dir;
	COLORREF m_Color; 
	WORD m_PenWidth;
	CRect GetRect(){return m_Rect;};
	void SetRect(const CRect& rect){m_Rect = rect;};
};

class CStratumObj : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CStratumObj)
#else
	DECLARE_SERIAL(CStratumObj);
#endif
public:
	CStratumObj();
	CStratumObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument);
	~CStratumObj();
	
	CString m_PlotContent;
	CObList m_SinPoint;
	void CreatePoint();
	void DrawSin(CDC* pDC);
	void SetSinRect(const CRect& rect);
//Attributes
protected:   
	WORD m_PenWidth;
	LOGFONT m_ScaleFont;
	COLORREF m_ScaleColor;

//Implementation
public:
	void SetRect(const CRect& rect);
	virtual void Serialize(CArchive& ar);
	virtual void DrawHead(CDC *pDC);	
	virtual void DrawClient(CDC *pDC);	
	virtual BOOL Properties(CView* pView);
};                

#endif //__STRATUMDRAW_H__