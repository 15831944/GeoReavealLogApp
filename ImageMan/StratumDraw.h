/******************************************************************
*   StratumDraw.h - declarations of Stratum Draw object class       *           
*******************************************************************/


#ifndef __STRATUMDRAW_H__
#define __STRATUMDRAW_H__

#include "FIDDraw.h"
#include "RemDraw.h"

class CSinPoint : public CRemDrawObj
{  	
	protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CSinPoint)
#else
	DECLARE_SERIAL(CSinPoint);
#endif
	CSinPoint();
public:
	CSinPoint(const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDoc);
	~CSinPoint();
	CString m_Flag;
	float m_Dep,m_Dip,m_Dir,m_Dip0,m_Dir0;
	COLORREF m_Color; 
	WORD m_PenWidth;
	BOOL m_bSave;
	
	//视裂缝长度            FVTL
	//视裂缝宽度            FVA
	//视裂缝水动力宽度      FVAH
	//视裂缝密度            FVDA
	//视裂缝密度            FVDC
	//视裂缝孔隙度          FVPA  
	float FVTL,FVA,FVAH,FVDA,FVPA,FVDC;
	int m_nPoints;
	int m_nAllocPoints;
	CPoint* m_Points;
// Attribtues
protected:
	CSinPoint* m_pFocusObj;

// Operations
public:
	void OnEditFold();
	void AddPartPoint(const CPoint& point);
protected:
	CPoint* NewPoints(int nPoints);
	BOOL RecalcBounds();
	void CreateFocusObj(CView* pView);
	void DeleteFocusObj(CView* pView);

// Implementation 
public:
	virtual void SetRect(const CRect& rect);          
	virtual void Draw(CView* pView, CDC* pDC);
	virtual void DrawFocus(CView* pView);
	virtual void DrawFocusObj(CView* pView);
	virtual HCURSOR GetHandleCursor(int nHandle);
	virtual void MoveHandle(CView* pView,int nDragHandle, const CPoint& point);
	virtual int HitTest(CView* pView,const CPoint& point);
	virtual void OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDblClk(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CView* pView, UINT nFlags, const CPoint& point);
	virtual void InitStatusBar();
	virtual void ShowStatus();
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
	CStratumObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDocument);
	~CStratumObj();
	
	CString m_PlotContent;
	void CreatePoint();
	virtual void Draw(CView* pView, CDC* pDC);
	void DrawSin(CView* pView,CDC* pDC);
	void AddPoint(float Dep,float Dip,float Dir,
		float Dip0,float Dir0,int Flag);
//Attributes
protected:   
	WORD m_PenWidth;
	LOGFONT m_ScaleFont;
	COLORREF m_ScaleColor;

//Implementation
public:
	virtual void SetRect(const CRect& rect);
	virtual void Serialize(CArchive& ar);
	virtual void DrawHead(CDC *pDC);	
	virtual void DrawClient(CDC *pDC);	
	virtual BOOL Properties(CView* pView);
	void UpdatePoint();
};                

#endif //__STRATUMDRAW_H__