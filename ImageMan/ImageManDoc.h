/******************************************************************
*   ImageManDoc.h - declarations of document object class         *           
*                                                                 *
*    author:zhaozhongmin	2019.3								  *
*                                                                 *
*******************************************************************/

#ifndef __ImageManDoc_H__
#define __ImageManDoc_H__

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
class CTrackObj;                 
class CDrawObj;
class CLineObj;
class CDepSect;
class CFoldObj;
class CLogPlotSrvrItem;
class CStratumObj;
class CMarkVectorObj;
class CMarkFreqObj;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CImageManDoc: public COleDocument
{
protected: // create from serialization only
	CImageManDoc();
	DECLARE_DYNCREATE(CImageManDoc)
public:
	CString m_szBufPath;         //流程链处理中数据缓冲区位置
	
	CString m_ParaFileName;      //成像数据预处理参数文件名
	CString	m_sSeries;           //仪器类型
	CString	m_sAZ1Name,	m_sRBName,m_sDEVName,m_sDAZName;
	CString m_szRADSName;         //阵列井径
	float	m_fDip1,m_fDir1;	
	int m_PadNum;                 //极板数

	BOOL m_bHandDrawSin,m_bBoxAnisys;
	CString m_ImageHandWorkFile;
	void DelSinObjList();
	void DelDepSectSin(float sdep,float edep);
	void ModifyDepSectSin(float sdep,float edep);
	void HandMarkToWell();
	void SaveHandMark();
	CStratumObj* GetStratumObject();
	CMarkVectorObj* GetMarkVectorObject();
	CMarkFreqObj* GetMarkFreqObject();
	void UpdateVector();
	float m_ProSdep,m_ProEdep,m_HorRulerDep;

	CRect GetUnionRectNoRem();
	void BmpDraw(CView* pView,CDC* pDC);
	void SetDepthNoRem(double sdep, double edep);
	void SetSizeNoRem();
	CRect GetTopRemObjUnionRect(int y);
	CRect GetBottomRemObjUnionRect(int y);

// Attributes
public:  
	int m_HWOprateNum;
	BOOL m_UnDo;
	CString m_OldWellName,m_DataBuf,m_WellName;
	BOOL m_MouseTip,m_FillLithGround;
	float m_VersionNo;
	static CLIPFORMAT m_cfDraw; // custom clipboard format
	CString m_LogName,m_PathName;
	double m_DepthScale;
	double m_PlotSdep;
	double m_PlotEdep;     
	double m_DepC;
	WORD m_DepthUnit;
	int m_Flag;
	int m_DrawFIDHeadMode;
	WORD m_LeftSpace,m_TopSpace,m_RightSpace,m_BottomSpace,m_HeadTopSpace,m_HeadBottomSpace;
	WORD m_xGrid,m_yGrid;
    CRect m_Rect,m_TrackRect,m_HeadRect,m_ClipRect;

	CObList m_HeadList;
	CObList m_TrackList;
	CObList m_RemObjList;
	CObList m_SinObjList;

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

// Template Operations
public:
	int GetDocumentType(){return m_DocumentType;};
protected:
	BOOL CImageManDoc::LoadTemplate(int flag=0);
	void CImageManDoc::InitDepth();
    BOOL CopyFromTemplates(const char *pszPathName);

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
	int GetTrackCount();
	CRect GetTrackUnionRect();
	int GetTrackTotalWidth();
	int GetTrackTotalHeight();

public:
	void DoVertAlignDepth();
	void DoVertAlignHead();
	void DoVertAlignTop();
	void DoVertAlign();
	void DoHoriArrangle(int nMode);
	void DoHoriArrangle(){DoHoriArrangle(m_HoriArrangleMode);};

// Track Size Operations	
	void SetChildTrackWidth();
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
	void OnScaleChange(); 
	double YCoorToDepth(int y);
	int DepthToYCoor(double depth);
	void SetDepth(double m_PlotSdep, double m_PlotEdep);
	void SetDepthScale(double scale);
	void SetDepC(double depc, int flag);
	void GetDepthRangle(double& minDep,double& maxDep);
	void ComputePageSize();
	CSize ComputerPrintSize();
	void SetSize();
	CRect GetRect(){return m_Rect;};
	int GetLeftSpace(CDrawObj* pObj = NULL);
	int GetRightSpace(CDrawObj* pObj = NULL);

// Draw Operations
public:
	void Draw(CView* pView,CDC* pDC);
	void Print(CDC* pDC,CView* pView = NULL);

// DDE Update Message
public:
	void OnWellDataUpdate(CString strWellName);
	void OnCurveDataUpdate(CString strCurveName);
	BOOL MatchWellName(CString strName,CString NewstrPathName);
	void MatchListFile(CString WellName);
	void OnLayerFlush();
// Object Operations
public:	
	void BufDataToWell();
	BOOL CopyAllFile(CString OldDirName, CString NewDirName);
	void MatchAllObject(CString OpenPath);
	void DeleteAllRemDraw();
	float GetCurrentDep(int DepY);
	CString GetCurveValue(int DepY);
	CString m_CmdTemplateName;
	void OnReFresh();
	void OpenWell(CString OpenPath);
    void DeleteObList();
	void MoveObj(CPoint pt);
	void AddObj(CDrawObj* pObj);
	void RemoveObj(CDrawObj* pObj);
	int MoveBack(CDrawObj* pObj);
	int MoveForward(CDrawObj* pObj);
	int MoveToHead(CDrawObj* pObj);
	int MoveToTail(CDrawObj* pObj);
	CDrawObj* ObjectAt(CView* pView,const CPoint& point);
	CDrawObj* GetLocateObject(CDrawObj* pObj);
	BOOL OnSetCursor(CView* pView,const CPoint& point);
	void DropFiles(HDROP hDropInfo, CView* pView);
	BOOL OnToolTip(CView* pView,CRect& rect,CString& text);
	CFoldObj* GetFoldObject();
	CDepSect* GetLineObject();
	void OnSaveImagemark();
	void WellRefresh();
//Implementation
	virtual ~CImageManDoc();
	virtual void Serialize(CArchive& ar);  
	virtual void DeleteContents();
	BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,AFX_CMDHANDLERINFO* pHandlerInfo) ;
//	virtual COleServerItem* OnGetEmbeddedItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL OnNewDocument();
	virtual BOOL OnSaveDocument(const char* pszPathName);
    virtual BOOL OnOpenDocument(const char* pszPathName);

// Generated message map functions
protected:
	//{{AFX_MSG(CImageManDoc)
	afx_msg void OnUpdateWell();
	afx_msg void OnDocumentUpdate();
	afx_msg void OnViewPapercolor();
	afx_msg void OnSetDepth();
	afx_msg void OnUpdateSetDepth(CCmdUI* pCmdUI);
	afx_msg void OnSetDepthscale();
	afx_msg void OnSetupPage();
	afx_msg void OnDepthUnitFeet();
	afx_msg void OnUpdateDepthUnitFeet(CCmdUI* pCmdUI);
	afx_msg void OnDepthUnitM();
	afx_msg void OnUpdateDepthUnitM(CCmdUI* pCmdUI);
	afx_msg void OnPlotFIDhead();
	afx_msg void OnUpdatePlotFIDhead(CCmdUI* pCmdUI);
	afx_msg void OnFilePrintSetup();
	afx_msg void OnMouseTip();
	afx_msg void OnUpdateMouseTip(CCmdUI* pCmdUI);
	afx_msg void OnFillLithground();
	afx_msg void OnUpdateFillLithground(CCmdUI* pCmdUI);
	afx_msg void OnSaveData();
	afx_msg void OnUpdateSaveData(CCmdUI* pCmdUI);
	afx_msg void OnSaveasImagemark();
	afx_msg void OnUpdateSaveasImagemark(CCmdUI* pCmdUI);
	afx_msg void OnSetMarkcorrepara();
	afx_msg void OnUpdateSetMarkcorrepara(CCmdUI* pCmdUI);
	afx_msg void OnBoxAnisys();
    afx_msg void OnUpdateBoxAnisys(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};                    


/////////////////////////////////////////////////////////////////////////////
// CViewGridDialog dialog
class CViewGridDialog : public CDialog
{
// Construction
public:
	CViewGridDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CViewGridDialog)
	enum { IDD = IDD_VIEW_GRID };
	UINT	m_xGrid;
	UINT	m_yGrid;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewGridDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewGridDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDocHoriArrangleDlg dialog

class CDocHoriArrangleDlg : public CDialog
{
// Construction
public:
	CDocHoriArrangleDlg(CImageManDoc* pDocument,CWnd* pParent = NULL);	// standard constructor
	CImageManDoc* pDoc;

// Dialog Data
	//{{AFX_DATA(CDocHoriArrangleDlg)
	enum { IDD = IDD_DOC_HORI_ARRANGLE };
	int		m_HoriAlignMode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocHoriArrangleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDocHoriArrangleDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CDocVertAlignDlg dialog

class CDocVertAlignDlg : public CDialog
{
// Construction
public:
	void OnDocmentUpdate();
	CDocVertAlignDlg(CImageManDoc* pDocument,CWnd* pParent = NULL);	// standard constructor
	CImageManDoc* pDoc;

// Dialog Data
	//{{AFX_DATA(CDocVertAlignDlg)
	enum { IDD = IDD_DOC_VERT_ALIGN };
	BOOL	m_AlignTop;
	BOOL	m_AlignDepth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocVertAlignDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDocVertAlignDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif //__LogPlotDoc_H__
