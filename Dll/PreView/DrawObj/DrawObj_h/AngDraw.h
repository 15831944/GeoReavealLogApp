/******************************************************************
*   DangDdirObj.h - declarations of DandDdir Draw object class    *           
*******************************************************************/


#ifndef __DANGDDIRDRAW_H__
#define __DANGDDIRDRAW_H__

#include "FIDDraw.h"
#include "Grid.h"

class CDangDdirObj : public CFIDDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CDangDdirObj)
#else
	DECLARE_SERIAL(CDangDdirObj);
#endif
public:
	CDangDdirObj();
	CDangDdirObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument);
	~CDangDdirObj();
	float m_MinGrade;
	CString	m_GradName;
	CString m_DirName;
//Attributes
protected:    
	float *dip,*ddir;
	WORD m_TailLength,m_CircleRadius,m_TailWidth,m_CircleWidth;
	int m_HeadStyle;
	BOOL m_PrintPara;
	CString m_ParaFormat;
	COLORREF m_CircleColor,m_TailColor;
	LOGFONT m_ScaleFont;
	CGridObj* m_pGrid;
	long m_PointNum;
	WORD m_DepLevel;
	CPoint* point;
	
//Implementation
public:
	void  SetClipRect(CDC* pDC,CRect rect);
	virtual int HitTest(CView* pView,const CPoint& point);
	virtual void Serialize(CArchive& ar);
	virtual void DrawHead(CDC *pDC);	
	virtual void DrawClient(CDC *pDC);	
	virtual BOOL Properties(CView* pView);
	virtual void SetRect(const CRect& rect);
	virtual void SetDepth(double sdep,double edep);
	virtual void OnChangeDepC(double depc, int flag);
//Operations	
protected:
	void CreatePoint();
public:
	unsigned int m_DrawLevel;
    virtual void DrawDangDdir(CDC* pDC);
friend class CDipPage;
friend class CDipPropSheet;
};                

#endif 