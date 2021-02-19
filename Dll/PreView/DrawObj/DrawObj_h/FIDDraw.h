/******************************************************************
*   FIDDrw.h - declarations of FID Draw object base class         *           
*******************************************************************/


#ifndef __FIDDRW_H__
#define __FIDDRW_H__

#include "DrawObj.h"
#include "GeoFIDio.h"

class CFIDDrawObj: public CDrawObj
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CFIDDrawObj)
#else
	DECLARE_SERIAL(CFIDDrawObj);
#endif
public:
	CFIDDrawObj();
	CFIDDrawObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument);

// Attributes
public:
	CRect m_HeadRect,m_ClientRect;
	BOOL m_DrawDirect;
	BOOL bFlush,bErrorFID;
	double m_SDep,m_EDep,m_Rlev,m_SSdep,m_EEdep;
	double m_PlotSdep,m_PlotEdep;
	float m_LeftVal,m_RightVal;
	CString m_strPathName,m_AliasName,m_UnitText,m_ScripFileName;
	LOGFONT m_NameFont;
	COLORREF m_NameColor;
	LOGBRUSH m_Brush;
	LOGPEN m_FramePen;
	CFIDDrawObj* m_pFocusObj;

// Operations
public:
	HCURSOR GetHandleCursor(int nHandle);
	void GetWellName(CString& m_WellName);
	void GetCurveName(CString& m_WellName);
	virtual void DrawHead(CDC *pDC);	
	virtual void DrawFrame(CDC *pDC);	
	virtual void DrawClient(CDC *pDC);	
	virtual CRect GetHeadRect(){return m_HeadRect;};
	virtual void SetHeadRect(const CRect& rect);
	virtual void SetDepth(double sdep,double edep);
	virtual double YCoorToDepth(int y);
	virtual int DepthToYCoor(double depth);
	virtual void OnSetDepth();
	virtual BOOL IsDepthUnitOK(CGeoFIDIO* pFIDIO);
	virtual int GetDrawDirect();
	virtual void SetDrawDirect(int direct);

//  Implementtation	
public:
	virtual void Serialize(CArchive& ar);
	virtual BOOL MatchCurve(CString strName);
	virtual BOOL MatchWell(CString strName);
	virtual void SetRect(const CRect& rect);
	virtual void OnChangeDepC(double scale, int flag);
	virtual int GetMinHeight();
	virtual int GetMinWidth();
	virtual void Move(const CPoint& point);          
	virtual void Draw(CView* pView,CDC *pDC);	

	virtual void DrawTracker(CView* pView,CDC* pDC, TrackerState state);
	virtual int GetHandleCount();
	virtual void MoveHandle(CView* pView,int nDragHandle, const CPoint& point);
	virtual void DrawFocus(CView* pView);
	virtual void DrawFocusObj(CView* pView);
	virtual void CreateFocusObj(CView* pView);
	virtual void DeleteFocusObj(CView* pView);
	virtual int HitTest(CView* pView,const CPoint& point);
	virtual void OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CView* pView, UINT nFlags, const CPoint& point);

	virtual void InvalObj(CView* pView);
	virtual BOOL Properties(CView* pView);
};

#endif // __FIDDRW_H__