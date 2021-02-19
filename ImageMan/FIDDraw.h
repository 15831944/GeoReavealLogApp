/******************************************************************
*   FIDDrw.h - declarations of FID Draw object base class         *           
*******************************************************************/


#ifndef __FIDDRW_H__
#define __FIDDRW_H__

#include "DrawObj.h"
#include "FIDIO.h"
class CScriptPoint : public CObject
{                  
public:
	CScriptPoint(CPoint Point,float Dep,CString Content,COLORREF Color,LOGFONT ScripttFont,int Direction=TA_RIGHT);
	void Draw(CDC* pDC,CImageManDoc* pDoc);
	CPoint m_Point;
	CString m_Content;
	float m_Dep;
	int		m_Direction;
	COLORREF m_Color;
	LOGFONT m_ScriptFont;
};
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
	CFIDDrawObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDocument);

	float m_VersionNo;
	int		m_ScriptDepCol;
	int		m_ScriptCol;
// Attributes
public:
	CRect m_HeadRect,m_ClientRect;
	BOOL m_DrawDirect;
	BOOL m_bFlush,m_bErrorFID;
	double m_SDep,m_EDep,m_Rlev,m_SSdep,m_EEdep;
	double m_PlotSdep,m_PlotEdep;
	float m_LeftVal,m_RightVal;
	CString m_strPathName,m_AliasName,m_UnitText,m_ScriptFileName;
	LOGFONT m_NameFont,m_ScriptFont;
	COLORREF m_NameColor,m_ScriptColor;
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
	virtual BOOL IsDepthUnitOK(CFIDIO* pFIDIO);
	int GetDrawDirect(){return m_DrawDirect;};
	void SetDrawDirect(int direct){m_DrawDirect = direct;};

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
	virtual void InitStatusBar();
	virtual void ShowStatus();
	virtual void InvalObj(CView* pView);
	virtual BOOL Properties(CView* pView);
};

#endif // __FIDDRW_H__