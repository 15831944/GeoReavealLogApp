/******************************************************************
*   WaveDraw.h - declarations of Wave Draw object class           *           
*******************************************************************/


#ifndef __WAVEDRAW_H__
#define __WAVEDRAW_H__

#include "FIDDraw.h"

class CWaveObj : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CWaveObj)
#else
	DECLARE_SERIAL(CWaveObj);
#endif
public:
	CWaveObj();
	CWaveObj(CString strName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument);
	~CWaveObj();

//Attributes
public:   
	int PALSIZE;
	float m_StartTime;
	WORD m_WaveWidth;
	COLORREF m_WaveColor;
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
	void DrawDirectClient(CDC *pDC);
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