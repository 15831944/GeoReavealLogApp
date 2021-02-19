/******************************************************************
*   ArrayWaveDraw.h - declarations of Wave Draw object class           *           
*******************************************************************/


#ifndef __ARRAYWAVEDRAW_H__
#define __ARRAYWAVEDRAW_H__

#include "FIDDraw.h"

class CArrayWaveObj : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CArrayWaveObj)
#else
	DECLARE_SERIAL(CArrayWaveObj);
#endif
public:
	CArrayWaveObj();
	CArrayWaveObj(CString strName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument);
	~CArrayWaveObj();

//Attributes
public:  
	int PALSIZE,PAL12;
	CString m_DelayCurve,m_PlusCurve;
	int m_DelayCurveNo,m_PlusCurveNo;
	float m_StartTime;
	WORD m_WaveWidth;
	COLORREF m_WaveColor,m_WaveColor0,m_WaveColor1;
	LOGFONT m_ScaleFont;
	WORD m_Amp,m_WfMax;
	WORD m_LeftValue,m_RightValue;
	WORD m_Level;
	int m_WaveType;
	CPalette m_Palette;
//Operations
public:
	virtual int HitTest(CView* pView,const CPoint& point);
	void SetClipRect(CDC* pDC, CRect rect);
	void CreateWavePalette();
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

#endif //__WAVEDRAW_H__