/******************************************************************
*   ImageDraw.h - declarations of Image Draw object class          *           
*******************************************************************/


#ifndef __IMAGEDRAW_H__
#define __IMAGEDRAW_H__

#include "FIDDraw.h"

class CImageObj : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CImageObj)
#else
	DECLARE_SERIAL(CImageObj);
#endif
public:
	CImageObj();
	CImageObj(CString strName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument);
	~CImageObj();

//Attributes
public: 
	int m_Bit;
	int m_HorScale;
	BOOL m_bColor;	
	COLORREF m_BkColor;
	LOGFONT m_ScaleFont;
	WORD m_ColorGrade,m_CrMode;
	short m_LeftValue,m_RightValue;
	CPalette m_Palette;
	WORD m_CtrlVer;
	CString m_SchemeFile;
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

#endif //__IMAGEDRAW_H__