/******************************************************************
*   TrackDrw.h - declarations of Track Draw object class          *           
*******************************************************************/


#ifndef __TRACKDRW_H__
#define __TRACKDRW_H__   

#define SCALE_HEIGHT 40
#define TITLE_HEIGHT 30
#define TRACK_WIDTH 150

#include "DrawObj.h"               
#include "FIDDraw.h"
#include "CurveDraw.h"
#include "WaveDraw.h"
#include "Grid.h"

class CLayerPoint : public CObject
{                  
public:
	CLayerPoint(CRect rect,CPoint pt, COLORREF ResultColor,LOGFONT ResultFont,
				CString NoStr);
	void Draw(CDC* pDC,CLogPlotDoc* pDoc,int PenW=1);
	CRect m_Rect;
	CPoint m_point;
	CString m_NoStr;
	COLORREF m_ResultColor;
	LOGFONT m_ResultFont;
	CRect GetRect(){return m_Rect;};
	void SetRect(const CRect& rect){m_Rect = rect;};
};

class CTrackObj : public CDrawObj
{                      
protected:
#ifdef _UNIX_
	DECLARE_SERIAL(CTrackObj)
#else
	DECLARE_SERIAL(CTrackObj);
#endif
public:
	CTrackObj();
	CTrackObj(const CRect& rect, CDrawObj* pParentObj,CLogPlotDoc* pDocument,double sdep, double edep);
	~CTrackObj();       
//Do Layer:
public:
	BOOL m_LayerFlush;
	BOOL m_bDrawRuler,m_bLayerHi;
	CString m_LayerFileName,m_DepUnitName;	
	CObList m_LayerPoint;
	virtual void CreatePoint(CDC* pDC);
	virtual void DrawLayer(CDC* pDC);
	virtual void SetLayerRect(const CRect& rect);
	void SubDeviseLayer(CRect rect,CString m_NoStr,CString Flag,CDC* pDC,float dep0,float dep);
	BOOL MatchWellName(CString strName, CString NewstrPathName);
//Attribute:
public:
	CString m_TrackName;        
	COLORREF m_TrackNameColor;
	LOGFONT m_TrackNameFont;
	CString m_DepthName;
	COLORREF m_DepthNameColor,m_DepUnitNameColor,m_LayerNameColor,m_DepthValueColor;
	LOGFONT m_DepthNameFont,m_DepUnitNameFont,m_LayerNameFont,m_DepthValueFont;
	BOOL bDrawDepth;	
	WORD m_DepthTrackPosition,m_DepthPosition,m_DepthLevel;

	double m_PlotSdep,m_PlotEdep;
	CRect m_ClientRect,m_HeadRect,m_TitleRect,m_DepthRect;	
	CRect m_VertFIDRect,m_HoriFIDRect,m_TrackRect;	
	LOGPEN m_TrackFramePen;
	LOGBRUSH m_TrackTitleBrush,m_TrackHeadBrush,m_TrackClientBrush,m_TrackDepthBrush;
	double m_Width;
	CGridObj* m_pGrid;
	BOOL bDrawTitle;
	BOOL bTrackResize;
	CTrackObj* m_pFocusObj;
	WORD m_VertFIDVertAlignMode,m_VertFIDHoriArrangleMode;
	WORD m_HoriFIDVertAlignMode,m_HoriFIDHoriAlignMode;
	WORD m_HoriFIDVertArrangleMode,m_HoriFIDHoriArrangleMode;
	WORD m_TrackHoriArrangleMode,m_TrackVertAlignMode;                     
	BOOL m_bAutoArrangle,m_bChildTrackHeadAlignMax,m_bChildTrackHeadHeightAuto,m_bHeadHeightAuto;
private:
	BOOL bSetHeadHeight;
// Operation
public:
	CRect GetHeadRect(){return m_HeadRect;};
	CRect GetTitleRect(){return m_TitleRect;};
	CRect GetClientRect(){return m_ClientRect;};
	CRect GetHoriFIDRect(){return m_HoriFIDRect;};
	CRect GetVertFIDRect(){return m_VertFIDRect;};
	CRect GetTrackRect(){return m_TrackRect;};
	void SetHeadRectHeight(int nHeight);
	void SetTitleRectHeight(int nHeight);
	void SetChildTrackHeadHeightAlignMax(BOOL bMax);
	void AlignChildTrackHeadHeight();
	void SetChildTrackHeadHeightAuto(BOOL bAuto);
	void AutoChildTrackHeadHeight();
	void SetHeadHeightAuto(BOOL bAuto);
	void AutoHeadHeight();
	void SetChildTrackWidth();
	void SetChildTrackTitleHeight();
	void DrawChildTrackTitle(BOOL bShow);
	void SetFIDHeight(int direct); 
	void SetFIDWidth(int direct); 
	void SetDepth(double sdep,double edep);
	virtual double YCoorToDepth(int y);
	virtual long DepthToYCoor(double depth);
	CFIDDrawObj* GetLeftFIDObj(int direct);
	CFIDDrawObj* GetRightFIDObj(int direct);
	CFIDDrawObj* GetTopFIDObj(int direct);
	CFIDDrawObj* GetBottomFIDObj(int direct);
	CFIDDrawObj* GetMaxLeftFIDObj(int direct);
	CFIDDrawObj* GetMinRightFIDObj(int direct);
	CFIDDrawObj* GetMaxTopFIDObj(int direct);
	CFIDDrawObj* GetMinBottomFIDObj(int direct);
	CFIDDrawObj* GetMaxWidthFIDObj(int direct);
	CFIDDrawObj* GetMinWidthFIDObj(int direct);
	CFIDDrawObj* GetMaxHeightFIDObj(int direct);
	CFIDDrawObj* GetMinHeightFIDObj(int direct);
	int GetFIDCount(int direct);
	CRect GetFIDUnionRect(int direct);
	int GetFIDTotalWidth(int direct);
	int GetFIDTotalHeight(int direct);
	CTrackObj* GetLeftTrackObj();
	CTrackObj* GetRightTrackObj();
	CTrackObj* GetMaxLeftTrackObj();
	CTrackObj* GetMinRightTrackObj();
	CTrackObj* GetMaxWidthTrackObj();
	CTrackObj* GetMinWidthTrackObj();
	CTrackObj* GetMaxHeightTrackObj();
	CTrackObj* GetMinHeightTrackObj();
	int GetTrackCount();
	CRect GetTrackUnionRect();
	int GetTrackTotalWidth();
	int GetTrackTotalHeight();
	int GetVertFIDMinWidth();	
	int GetVertFIDMinHeight();	
	int GetHoriFIDMinWidth();	
	int GetHoriFIDMinHeight();	
	int GetTrackMinWidth();
	int GetTrackMinHeight();
	CRect GetUnionRect();
	int GetMinHeadWidth();
	int GetMinWidth();
	int GetMinHeadHeight();
	int GetMinHeight();
	
	void DoVertArrangleFID(CRect rect, int nArrangleMode, int direct);
	void DoVertArrangleHoriFID(){DoVertArrangleFID(m_HoriFIDRect,m_HoriFIDVertArrangleMode,HORDIR);};

	void DoHoriArrangleFID(CRect rect, int nArrangleMode, int direct);
	void DoHoriArrangleHoriFID(){DoHoriArrangleFID(m_HoriFIDRect,m_HoriFIDHoriArrangleMode,HORDIR);};
	void DoHoriArrangleVertFID();

	void DoVertAlignFID(CRect rect, int nAlineMode, int direct);
	void DoVertAlignHoriFID(){DoVertAlignFID(m_HoriFIDRect,m_HoriFIDVertAlignMode,HORDIR);};
	void DoVertAlignVertFID(){DoVertAlignFID(m_VertFIDRect,m_VertFIDVertAlignMode,VERDIR);};

	void DoHoriAlignFID(CRect rect, int nAlineMode, int direct);
	void DoHoriAlignHoriFID(){DoHoriAlignFID(m_HoriFIDRect,m_HoriFIDHoriAlignMode,HORDIR);};

    void DoHoriAlignTrack(CRect rect, int nAlignMode);

    void DoHoriArrangleTrack(CRect rect, int nArrangleMode);
    void DoHoriArrangleTrack(){DoHoriArrangleTrack(m_TrackRect,m_TrackHoriArrangleMode);};

    void DoVertArrangle();
    void DoHoriArrangle();
    void Arrangle();
	void OrderObj();
	int AddFID(CFIDDrawObj* pObj);
	void SetClientRect(const CRect& rect);
	void SetHoriFIDSize(const CRect& rect);
	virtual int InsertObj(CDrawObj* pObj,CRect rect);
	virtual void DrawHead(CView* pView, CDC* pDC);
	virtual void DrawTitle(CView* pView, CDC* pDC);
	virtual void DrawClient(CView* pView, CDC* pDC);
	virtual void DrawDepth(CView* pView, CDC* pDC);
	virtual void DrawFrame(CView* pView, CDC* pDC);
	virtual void OnSetDepth();
	virtual void OnShowTitle(BOOL bShow);
	virtual void OnShowDepth(BOOL bShow);
	virtual void OnDepthPosition(int nPosition);
	virtual void OnDepthTrackPosition(int nPosition);

// Fill Operations
	BOOL IsFillCurve(CCurveObj* pObj);
	BOOL IsFillCurve(CString name);
	int RemoveFillCurve(CCurveObj* pObj);
	int RemoveFillCurve(CString name);
	CCurveObj* FindAliasCurve(CString name);
	CFIDDrawObj* FindCurve(CString name);
	CCurveObj* FindLastCurve(CCurveObj* pObj);


// Implementation
	virtual CDrawObj* GetLocateObject(CDrawObj* pObj);
	virtual void Serialize(CArchive& ar);
	virtual HCURSOR GetHandleCursor(int nHandle);
	virtual void SetRect(const CRect& rect);          
	virtual void OnChildWidthChange(CDrawObj* pObj);
	virtual void OnChildHeightChange(CDrawObj* pObj);
	virtual void OnChildPosChange(CDrawObj* pObj);
	virtual void OnChildRemove(CDrawObj* pObj,int nParentType);
	virtual void OnChildTrackHeadHeightChange(CDrawObj* pObj);
	virtual void OnChangeDepC(double scale, int flag);
	virtual void Draw(CView* pView, CDC* pDC);
	virtual void DrawTracker(CView* pView, CDC* pDC, TrackerState state);
	virtual BOOL Properties(CView* pView);
	virtual int AddObj(CDrawObj* pObj,WORD nParentObj = 0);
	virtual int RemoveObj(CDrawObj* pObj, int flag=0);
	virtual int HitTest(CView* pView,const CPoint& point);
	virtual void MoveHandle(CView* pView,int nDragHandle, const CPoint& point);
	virtual void DrawFocus(CView* pView);
	virtual void DrawFocusObj(CView* pView);
	virtual void CreateFocusObj(CView* pView);
	virtual void DeleteFocusObj(CView* pView);
	virtual void OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CView* pView, UINT nFlags, const CPoint& point);
};

#endif // __TRACKDRW_H__