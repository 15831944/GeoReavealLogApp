/******************************************************************
*   MarkFreqObj.h - declarations of DandDdir Draw object class    *           
*******************************************************************/


#ifndef __MARKFREQDRAW_H__
#define __MARKFREQDRAW_H__

#include "FIDDraw.h"

class CMarkPoint : public CObject
{                  
public:
	CMarkPoint(float Dep,float Dip,float Dir,CString Flag);
	CString m_Flag;
	float m_Dep,m_Dip,m_Dir;
};

class CMarkFreqObj : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CMarkFreqObj)
#else
	DECLARE_SERIAL(CMarkFreqObj);
#endif
public:
	CMarkFreqObj();
	CMarkFreqObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDocument);
	~CMarkFreqObj();

	CString m_AziName;
	BOOL m_bDrawDipFreq;
//Attributes
public:    
	CObList m_MarkPoint;
	WORD m_PointNum;
	WORD m_AzimNum, m_DangNum, m_DepLevel;
	WORD m_CircleWidth,m_PieWidth,m_DangBaseLineWidth,m_DangFreqLineWidth;
	COLORREF m_CircleColor,m_PieColor,m_DangBaseLineColor,m_DangFreqLineColor;
	WORD m_DangBaseLineLength,m_DangFreqLineLength,m_CircleRadius;
	LOGBRUSH m_PieBrush;
	int m_AzimFreq[50];
	int m_DangFreq[50];
	int m_MaxAzimFreq;
	int m_MaxDangFreq;

//Implementation
public:
	void UpdatePoint();
	virtual void Serialize(CArchive& ar);
    virtual void DrawClient(CDC* pDC);
	virtual void DrawHead(CDC *pDC);	
	virtual BOOL Properties(CView* pView);
//Operations	
protected:
	void CreatePoint();
};

#endif 