#pragma once

#define HINT_UPDATE_WINDOW      0
#define HINT_UPDATE_DRAWOBJ     1
#define HINT_UPDATE_OLE_ITEMS   2
#define HINT_UPDATE_SIZE        3
#define HINT_UPDATE_HEADOBJ     4

#define VERT_ALIGN_DEPTH 1
#define VERT_ALIGN_CLIENT 2
#define VERT_ALIGN_ALL 3

#define HEADMAX 0
#define HEADAUTO 1

#define LEFTSPACE 10
#define TOPSPACE 10

#define TEMPLATE_DOC 0
#define STAND_DOC 1

#define UNIT_M 0
#define UNIT_FEET 1

#include "DrawObj.h"
#include "ProgDlg.h"
class CTrackObj;                 
class CDrawObj;
class CImageObj;
class CLogPlotSrvrItem;
class CLogPlotView;

// CLogPlotDoc 文档
class AFX_EXT_CLASS CLogPlotDoc : public COleDocument
{
	DECLARE_DYNCREATE(CLogPlotDoc)

	CLogPlotView *m_pView;
public:
	CLogPlotDoc();
	virtual ~CLogPlotDoc();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	void Draw(CView* pView,CDC* pDC);

	//数据为链接字符串
	//如：10.132.204.16;sa;log;LogBase
	CString		m_szSever;
	CString		m_szRole;			//用户角色
	DWORD		m_ProjectID;		//用于新建绘图文档或匹配当前项目
	CString		m_szWellName;		//井名
	CString		m_szProjectName;	//项目名称
	int		m_ObjNum,m_DrawNo;
	BOOL	m_bRunFirst;
	CProgressDlg* m_pGuage;

	CImageObj* GetImageObject();
	void Print(CDC* pDC,CView* pView = NULL);
	BOOL SetupPage();
// Attributes
public:
	BOOL m_FillLithGround;  
	int m_PlotResultModel;
	float m_VersionNo;

	BOOL OpenWell();
	int	 GetObjCount();
	BOOL MatchAllObject();

	double m_DepthScale;
	double m_PlotSdep;
	double m_PlotEdep;     
	double m_DepC;
	BOOL   m_bPrint;	
	WORD m_DepthUnit;
	int m_Flag,m_LineType;
	int m_DrawFIDHeadMode; //上下对称显示格线道和曲线的刻度区
	WORD m_LeftSpace,m_TopSpace,m_RightSpace,m_BottomSpace,m_HeadTopSpace,m_HeadBottomSpace;
	WORD m_xGrid,m_yGrid;
    CRect m_Rect,m_TrackRect,m_HeadRect,m_ClipRect;
	CObList m_HeadList;
	CObList m_TrackList;
	CObList m_RemObjList;
	COLORREF m_paperColor;
	CPoint downPoint;
	BOOL bEnableChildSizeMessage;
	BOOL m_bChildTrackHeadHeightAuto,m_bChildTrackHeadAlignMax;
	WORD m_HoriArrangleMode;
    BOOL bHoriAutoArrangle,bVertAutoAlign,m_bVertAlignDepth,m_bVertAlignHead,m_bVertAlignTop;
	BOOL m_bReserveSpace,m_bPlotHead,m_bPlotTopRem,m_bPlotBottomRem,m_bPlotTrack,m_bPlotResultList,m_bMaxPageSize;
    WORD m_DocumentType;
	int m_PageWidth,m_PageHeight,m_NumCols,m_NumRows;
	CPoint m_WinOrg;
	CSize  m_WinPage;
	BOOL bNullTemplate,bNewDocument,bCopyFromTemplate;
	int m_ScaleNo;
	int m_ScaleFactor;

public:
	BOOL OpenPlotDoc(CString FileName);
// Template Operations
	int GetDocumentType(){return m_DocumentType;};
//  Coordinate Convert
public:
	void DPtoLP(CDC* pDC,LPPOINT lpPoints, int nCount = 1);
	void DPtoLP(CDC* pDC,LPRECT lpRect);
	void LPtoDP(CDC* pDC,LPPOINT lpPoints, int nCount = 1);
	void LPtoDP(CDC* pDC,LPRECT lpRect);
	void PPtoLP(LPPOINT lpPoints, int nCount = 1);
	void PPtoLP(LPRECT lpRect);
	void LPtoPP(LPPOINT lpPoints, int nCount = 1);
	void LPtoPP(LPRECT lpRect);
// Track Arrangle Operations
public:
	CTrackObj* GetLeftTrackObj();
	CTrackObj* GetRightTrackObj();
	CTrackObj* GetMaxLeftTrackObj();
	CTrackObj* GetMinRightTrackObj();
	CTrackObj* GetMaxWidthTrackObj();
	CTrackObj* GetMinWidthTrackObj();
	CTrackObj* GetMaxHeightTrackObj();
	CTrackObj* GetMinHeightTrackObj();
	CRect GetUnionRect();
	CRect GetUnionRectNoRem();
	int GetTrackCount();
	CRect GetTopRemObjUnionRect(int y);
	CRect GetBottomRemObjUnionRect(int y);
	CRect GetTrackUnionRect();
	int GetTrackTotalWidth();
	int GetTrackTotalHeight();//无执行代码

	void DoVertAlignDepth();
	void DoVertAlignHead();
	void DoVertAlignTop();
	void DoVertAlign();
	void DoHoriArrangle(int nMode);
	void DoHoriArrangle(){DoHoriArrangle(m_HoriArrangleMode);};

// Track Size Operations	
	void SetChildTrackWidth();
	void SetSelTrackWidth(CTrackObj* pObj);
	void SetChildTrackTitleHeight();
	void DrawChildTrackTitle(BOOL bShow);
	void SetChildTrackHeadHeightAlignMax(BOOL bMax);
	void AlignChildTrackHeadHeight();
	void SetChildTrackHeadHeightAuto(BOOL bAuto);
	void AutoChildTrackHeadHeight();

// Child Message
	void OnChildWidthChange(CDrawObj* pObj);
	void OnChildHeightChange(CDrawObj* pObj);
	void OnChildPosChange(CDrawObj* pObj);
	void OnChildRemove(CDrawObj* pObj,int nParentType);
	void OnChildTrackHeadHeightChange(CDrawObj* pObj);

// Size Operations	
public:
	void OnScaleChange(); /////////////////////////////////////////////////////////////////
	double YCoorToDepth(int y);
	int DepthToYCoor(double depth);
	void SetDepth(double sdep, double edep);
	void SetDepthNoRem(double sdep, double edep);
	void SetDepthScale(double scale);
	void SetDepC(double depc, int flag);
	
	void ComputePageSize();
	CSize ComputerPrintSize();
	void GetDepthRangle(double& minDep,double& maxDep);

	void SetSize();
	void SetSizeNoRem();
	CRect GetRect(){return m_Rect;};
	CRect GetHeadRect(){return m_HeadRect;};
	int GetLeftSpace(CDrawObj* pObj = NULL);
	int GetRightSpace(CDrawObj* pObj = NULL);
	int ComputerResultListHeight();
	CRect ComputerResultListRect(int y);
	void SetResultListRect();

// Draw Operations
public:
	
	//void Print(CDC* pDC,CView* pView = NULL);
	//void BmpDraw(CView* pView,CDC* pDC);
// Data Update Message
public:
	void OnWellDataUpdate(CString strWellName);
	void OnCurveDataUpdate(CString strCurveName);
	BOOL MatchWellName(CString strName,CString NewstrPathName);
	void MatchListFile(CString WellName);
	void MatchCurrentWorkSpace();
// Object Operations
public:
	void DocToTemplate();
	void OnLayerFlush();
	
	void OnReFresh();
	BOOL IsHeadObj(CDrawObj* pObj);
    void DeleteObList();

	void MoveObj(CPoint pt);
	void MoveHeadObj(CPoint pt);
	void AddObj(CDrawObj* pObj);
	void AddHeadObj(CDrawObj* pObj);
	void RemoveObj(CDrawObj* pObj);
	void RemoveHeadObj(CDrawObj* pObj);
	int MoveBack(CDrawObj* pObj);
	int MoveForward(CDrawObj* pObj);
	int MoveToHead(CDrawObj* pObj);
	int MoveToTail(CDrawObj* pObj);
	
	CDrawObj* ObjectAt(CView* pView,const CPoint& point);
	CDrawObj* GetLocateObject(CDrawObj* pObj);
	BOOL OnSetCursor(CView* pView,const CPoint& point);
	BOOL OnToolTip(CView* pView,CRect& rect,CString& text);

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
};
