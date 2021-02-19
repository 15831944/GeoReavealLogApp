/******************************************************************
*   EchoDraw.h - declarations of Echo Draw object class           *           
*******************************************************************/


#ifndef __ECHODRAW_H__
#define __ECHODRAW_H__

#include "FIDDraw.h"

class CEchoObj : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CEchoObj)
#else
	DECLARE_SERIAL(CEchoObj);
#endif
public:
	CEchoObj();
	CEchoObj(CString strName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument);
	~CEchoObj();

	WORD m_ColorGrade;
	CPalette m_Palette;
	CString m_SchemeFile;
	void CreateEchoPalette();
//Attributes
public:   
	float m_WfMin;
	WORD m_EchoWidth;
	COLORREF m_EchoColor,m_FillColor;
	LOGFONT m_ScaleFont;
	WORD m_Amp;
	float m_WfMax;
	float m_LeftValue,m_RightValue;
	WORD m_Level;
	int m_EchoType;

	WORD m_CtrlVer;
	BOOL m_FillScale;
	BOOL m_EchoFilter;
	BOOL m_bXlog;
	BOOL m_bYlog;
//Operations
public:
	virtual int HitTest(CView* pView,const CPoint& point);
	void SetClipRect(CDC* pDC, CRect rect);
//Implementation
public:
	virtual void Serialize(CArchive& ar);
	virtual void Draw(CView* pView,CDC* pDC);
	virtual void DrawHead(CDC *pDC);	
	virtual void DrawClient(CDC *pDC);	
	virtual BOOL Properties(CView* pView);
	virtual void OnScaleFont();
	virtual void OnLButtonDblClk(CView* pView, UINT nFlags, const CPoint& point);
};                

#endif //__EchoDRAW_H__