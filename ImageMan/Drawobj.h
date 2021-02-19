/******************************************************************
*   DrawObj.h - declarations of all Draw object base class        *           
*******************************************************************/


#ifndef __DRAWOBJ_H__
#define __DRAWOBJ_H__

#define AUTO_CASCADE 0
#define MENU_CASCADE 1

#define VERDIR 0
#define HORDIR 1  

#define IDM_NONE 3
#define IDM_LEFT 0
#define IDM_RIGHT 1
#define IDM_CENTER 2   
#define IDM_JUST 2
#define IDM_TOP 0
#define IDM_BOTTOM 1 

enum SelectMode
{
	none,
	net,
	move,
	size,
	editdata
};

enum ToolShape
{
	nonetool,
	selecttool,   
	rectangletool,
	roundrecttool,
	linetool,
	depsecttool,
	arrowtool,
	foldtool,
	polytool,
	BmpDrawtool,
	ellipsetool,
	rectarrowtool,
	ellipsearrowtool,
	oletool,
	texttool,
	tracktool,
	FIDtool,
	curvetool,
	coretool,
	lthtool,
	resulttool,
	dangddirtool,
	azimfreqtool,
	devdaztool,
	wavetool,
	imagetool,
	stratumtool,
	markvectortool,
	markfreqtool,
	imageCurvetool
};

enum DrawShape
{
	curveobj, 
	depthobj,
	resultobj,
	lthobj,
	coreobj,
	rectangleobj,
	roundrectobj,
	textobj,
	lineobj,
	depsectobj,
	arrowobj,
	foldobj,
	polyobj,
	ellipseobj,
	rectarrowobj,
	ellipsearrowobj,
	trackobj,
	dangddirobj,
	azimfreqobj,
	devdazobj,
	waveobj,
	echoobj,
	imageobj,
	image3D,
	stratumobj,
	curve3D,
	markvectorobj,
	Markfreqobj,
	imageCurveobj,
	lthnoteobj,
	CCLobj,
	Poleobj,
	sinobj
};

enum TrackerState { normal, selected, active };

#include "Style.h"

class CImageManDoc;

class CDrawObj : public CObject
{
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CDrawObj)
#else
	DECLARE_SERIAL(CDrawObj);
#endif
public:
	CDrawObj();    
	CDrawObj(const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDocument);
	~CDrawObj();    
                                              
// Attributes:
public:
	CRect m_Rect;   
	WORD m_ParentType;
	DrawShape m_DrawShape;
	BOOL m_bSelected,bEnableChildSizeMessage;
	CObList m_ObjList;
	CImageManDoc* pDoc;
	CStatusBar* m_pStatusBar;
	CStatusBar m_StatusBar;
	CString m_ShapeName;
protected:
	CDrawObj* m_pParentObj;

//Operations
public:	
	virtual CRect GetMaxRect();
	virtual CRect GetRect();
	virtual void FindCurve(CObList* pList,CString strName);
	virtual BOOL MatchCurve(CString strName){return FALSE;};
	virtual void FindWell(CObList* pList,CString strName);
	virtual BOOL MatchWell(CString strName){return FALSE;};
	virtual void SetRect(const CRect& rect);          
	virtual void OnChildWidthChange(CDrawObj* pObj){};
	virtual void OnChildHeightChange(CDrawObj* pObj){};
	virtual void OnChildPosChange(CDrawObj* pObj){};
	virtual void OnChildRemove(CDrawObj* pObj, int nParentType){};
	virtual void ZoomRect(const CRect& sRect, const CRect& zRect);
	virtual void AlignToGrid(const CPoint& point, const CSize& size);
	virtual void Move(const CPoint& point);          
	virtual void OnChangeDepC(double scale, int flag);
	virtual CDrawObj* GetParentObj();
	virtual WORD GetParentType(){return m_ParentType;};
	virtual void SetParentObj(CDrawObj* pObj,WORD nParentType = 0);
	virtual int AddObj(CDrawObj* pObj,WORD nParentObj = 0);
	virtual int RemoveObj(CDrawObj* pObj, int flag=0);
	virtual void Remove();
	virtual BOOL Properties(CView* pView);
	virtual void SetSelect(CView* pView,BOOL bf){m_bSelected = bf;};
	virtual HCURSOR GetHandleCursor(int nHandle);
	virtual int GetHandleCount();
	virtual CPoint GetHandlePoint(int nHandle);
	virtual BOOL OnSetCursor(CView* pView,const CPoint& point);
	virtual BOOL Intersects(const CRect& rect);
	virtual CRect GetHandleRect(CView* pView,int nHandleID);
	virtual CDrawObj* ObjectAt(CView* pView,CPoint point);
	virtual CDrawObj* GetLocateObject(CDrawObj* pObj);
	virtual int HitTest(CView* pView,const CPoint& point);
	virtual void MoveHandle(CView* pView,int nDragHandle, const CPoint& point);
	virtual void InvalObj(CView* pView);
	virtual void InvalidateRect(CRect rect);
	virtual void Draw(CView* pView, CDC* pDC);
	virtual void DrawFill(CView* pView, CDC* pDC);
	virtual void DrawFocus(CView* pView){};
	virtual void DrawFocusObj(CView* pView){};
	virtual void DoScroll(CScrollView* pView);
	virtual void DrawTracker(CView* pView, CDC* pDC, TrackerState state);
	virtual void Invalidate(CView* pView = NULL);
	virtual void OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDblClk(CView* pView, UINT nFlags, const CPoint& point);
	virtual void InitStatusBar();
	virtual void ShowStatus();
	virtual void ShowStatusExt(const CRect& rect,int nHandle);
	virtual void ReleaseStatusBar();
	virtual int MoveBack(CDrawObj* pObj);
	virtual int MoveForward(CDrawObj* pObj);
	virtual int MoveToHead(CDrawObj* pObj);
	virtual int MoveToTail(CDrawObj* pObj);
	void CDrawObj::LPtoHIMM(CRect& rect);
	void CDrawObj::LPtoHIMM(CPoint* point, int num);
	virtual BOOL OnToolTip(CView* pView,CRect& rect,CString& text);

//  Implementtation	
public:
	virtual void Serialize(CArchive& ar);
	
	static void dtos(double value, char* ss);
	static int nDragHandle;
	static int m_xMin,m_xMax,m_yMin,m_yMax;
	static CPoint c_down;
	static CPoint c_last;
	static SelectMode m_SelectMode;
	static ToolShape c_ToolShape;
};

#endif //__DrawObj_H__

