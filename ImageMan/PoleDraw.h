/******************************************************************
*   PoleDraw.h - declarations of Pole Draw object class       *           
*******************************************************************/


#ifndef __POLEDRAW_H__
#define __POLEDRAW_H__

#include "FIDDraw.h"
#include "Grid.h"

class CPolePoint : public CObject
{                  
public:
	CPolePoint(CRect rect,float Dep,float Dip,float Dir,CString Flag,
		int DrawType,WORD PenWidth,COLORREF Color);
	void Draw(CDC* pDC,CImageManDoc* pDoc);
	CRect m_Rect;
	CString m_Flag;
	int m_DrawType;
	CPoint m_Pt[4];
	float m_Dep,m_Dip,m_Dir;
	COLORREF m_Color; 
	WORD m_PenWidth;
	CRect GetRect(){return m_Rect;};
	void SetRect(const CRect& rect){m_Rect = rect;};
};

class CPoleObj : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CPoleObj)
#else
	DECLARE_SERIAL(CPoleObj);
#endif
public:
	CPoleObj();
	CPoleObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDocument);
	~CPoleObj();
	
	float m_LookAngle;
	int m_StartPos,m_DrawType;
	CObList m_PolePoint;
	void CreatePoint();
	void DrawPole(CDC* pDC);
	void SetSinRect(const CRect& rect);
//Attributes
protected:   
	WORD m_PenWidth;
	COLORREF m_PenColor;
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

#endif //__POLEDRAW_H__