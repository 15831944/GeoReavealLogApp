/******************************************************************
*   ImageManView.h - declarations of CImageManView class          * 
*																  *
*   author:zhaozhongmin	2019.3								      *
*                                                                 *
*******************************************************************/


#ifndef __ImageManView_H__
#define __ImageManView_H__

////////////////////////////////////////////////////////////////////////
// CImageManView view

#include "DrawObj.h"
#include "MeasureMultiLine.h"
#include "DibImage.h"

class CTrackObj;
class CDrawObj;
class CCurveObj;
enum ToolPro
{
	NoneTool,
	MoveDep,
	PlusValue,
	MouseEdit,
	CurveStat,
	RangValue,
	FilterPro,
	InsertPoint,
	CurveUnite,
	ReWrite,
	DisValue,
	DepRangeDraw,
	PointDep,
	CurveNib,
	CounterGlow
};
class CImage3D;
class CImageObj;
class CImageManView : public CScrollView
{
	DECLARE_DYNCREATE(CImageManView)
protected:
	CImageManView();			// protected constructor used by dynamic creation
// Attributes
public:
	
	//读取图像相关控制参数
	CString m_PickupFIDName;				//曲线全名
	CString m_PickupAliasName;				//曲线别名
	int 	m_PickupBit;					//钻头直径 mm
	int 	m_PickupHorScale;				//横向绘图比例
	CRect	m_PickupRect;					//新建时的图像大小 
	float	m_PickupStDep;					//新建时的起始深度
	float	m_PickupEnDep;					//新建时的结束深度
	float   m_PickupVerScale;				//纵向绘图比例
	float   m_CoverageRate;					//自动拾取覆盖率

	//溶融孔洞自动拾取参数
	int m_KPara;							//特征值聚类数
	float m_HoleMinAcr;						//仅拾取大于m_HoleMinAcr的孔洞
	BOOL  m_bHoleMinAcr;
	float m_HoleMaxAcr;						//仅拾取小于m_HoleMaxAcr的孔洞
	BOOL  m_bHoleMaxAcr;
	float m_HoleMinRound;					//仅拾取圆度大于m_HoleMinRound的孔洞
	BOOL m_bMinRound;
	float m_HoleMaxRound;					//仅拾取圆度大于m_HoleMaxRound的孔洞
	BOOL m_bMaxRound;

	//孔洞刻度关系参数 y=A*x+B
	float m_fA,m_fB;
	//裂缝刻度关系参数 y=C*x+D
	float m_fC,m_fD;

	//分析联通性参用
	float	m_ImageMax;						//成像数据最值 缺省1024，CIFLog处理的为256
	float	m_ImageMin;						//成像数据最值 缺省0
	
	int		m_iPickupSinWidth;				//拾取层理裂缝的最大宽度mm
	CString m_sPickupSinLabel;				//拾取层理裂缝类型标识
	//裂缝统计参数
	float m_SinStatSdep,m_SinStatEdep;		//裂缝统计井段
	float m_SinStatWin;						//滑动窗长
	float m_SinStatRlev;					//滑动步长
	//计算裂缝宽度参数
	float m_Rm;								//泥浆电阻率
	float m_ParC;							//水动力参数C
	float m_ParB;							//水动力参数B
	CString	m_sRxoWell;						//标准电阻率井名
	CString	m_sRxoName;						//浅侧向曲线
	
	//孔洞统计参数
	CString m_StatWellName;					//统计曲线保存位置
	float m_StatWin;						//滑动窗长
	float m_StatRlev;						//滑动步长
	float m_ContourMax;						//最大颗粒孔洞直径
	float m_StatSdep,m_StatEdep;			//统计井段
	//以颗粒孔洞直径进行分类统计
	float m_StatDiam1;						//小于m_StatDiam1的为小孔
	float m_StatDiam2;						//大于m_StatDiam1为中孔，小于m_StatDiam2
	float m_StatDiam3;						//大于m_StatDiam2为大孔，小于m_StatDiam3大于m_StatDiam3为溶洞
	//生成颗粒孔洞大小频谱图
	int	  m_nSpcNum;						//频谱曲线横向刻度区间数

	
	CString		m_PickupFileName;			//保存颗粒孔洞分析的文件名
	CImageObj	*m_pPickupObj;				//新建时的图像对像
	CDibImage	m_DibImage;					//旋转探测图像边界主要方法类	
	CObList		m_ContourList;				//存储已拾取的颗粒或孔洞
	
	//拾取颗粒孔洞的交互操作选项
	int		m_iHoleOperate;
	BOOL	m_bRectangleSel;
	CPoint	m_StSelPt,m_EnSelPt;
	
	int     m_iPickupNum;					//拾取颗粒或孔洞操作记录，用于退出时提醒是否保存
	BOOL	m_bConnection;					//显示孔洞联通性

	void DelSelContour(CPoint pt);
	void RectanglePickupContour();
	void EllipsePickupContour();
	
	BOOL SavePickupContour(BOOL bSave);
	void DrawPickupContour(CDC *pDC);
	//计算单条裂缝储层参数，赋给CSinPoint对象并生成Crack.List
	BOOL StatSinContour();
// Attributes
public:	
	float m_sindep0;
	//拾取层理裂缝的交互操作选项
	int m_SinFlag;
	CDrawObj* m_pHandDrawObj;
	CMeasureMultiLine* m_pMeasure;
	void StartHandDrawSin();
	//自动拾取选定区域层理并追加到HandWork.List文件后\n区域层理拾取
	void RectangleComputeDip();
	//删除选定区域层理并更新HandWork.List文件\n区域层理裂缝删除
	void RectangleDeleteDip();

	//结束曲线交互编辑或层理裂缝拾取
	afx_msg LRESULT OnEndClick(WPARAM wParam, LPARAM lParam);
	//中断曲线交互编辑或层理裂缝拾取
	afx_msg LRESULT OnCancelClick(WPARAM wParam, LPARAM lParam);
	//处理曲线交互编辑并写入文件
	void EndHandDrawCurve();
	
	BOOL			m_EddyPlay;
	CImage3D*		m_pImage3D;
	BOOL			m_bAlikePro;
	CStringArray	m_SelCurveNameArray;
	float			depArray[100][2];//多曲线拉线深度处理

	BOOL m_bActive,m_bFirstUpdate,m_bTrackTitleShow;
	ToolPro c_ToolPro;
	BOOL m_bProMutilDep;

	int m_UnDo,m_ReDo,m_InitOprationNum,m_PreOprationNum;
	CStringArray m_UnDoName,m_UnDoNameCopy;
	CString m_UnDoFilePath;
	long m_DepPoint;
	BOOL m_MouseTrace;

	BOOL m_bDisFIDValue;
	CString m_DisFIDName;
	int m_iDisFIDLeftX,m_DisFIDCalType,m_iDisFIDIntiX,m_iDisFIDPreX;
	float m_fDisFIDLeftValue,m_fDisFIDRatio;

	BOOL m_bCounterGlow;
	long m_iCounterIntiX,m_iCounterPreX;
	CString m_StatusText;int m_DisCharPos;

	//自动识取地质沉积层理（倾角计算相关参数）	
	//参加倾角计算的最小相关系数
	float	m_WinLength;         //相关对比窗长
	float	m_StepLength;        //计算倾角曲线的步长
	float	m_SearchLength;      //上下探索长度
	float	m_DangMax;           //最大倾角
	float	m_RMin;              //参加倾角计算的最小相关系数
	float   m_GradCons;
	float   m_GradMin;
protected:
	BOOL bObjectLink;
	CDrawObj* m_pSelectObj;
	
	CPen* pOldPen;
	CFont* pOldFont;   
	int m_LeftSpace,m_TopSpace;
	BOOL bToolTipActive;
	CString TipText;
	CToolTipCtrl m_tooltip;
	CImageManDoc* pDoc;
	
protected:
	// Printing support
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	
// Operations
protected:
	CImageManDoc* GetDocument()
		{ return (CImageManDoc*)m_pDocument; }
	CRect GetInitialPosition();
	void SetScrollSizes(int nMapMode, SIZE sizeTotal);
    void SelectObj(CDrawObj* pObj);   
	void UpdateActiveItem();
	void PasteNative(COleDataObject& dataObject);
	BOOL DoPasteItem(COleDataObject& dataObject);
	void GetCenterPoint(LPPOINT pt);
public:
	void ProMultisectdep(CString SelCurveName);
	void SetScroDep(float dep);
	void DelUnDoOpration();
	void ProMultiCurveVal(float dep1,float dep2);
	void DeSelectObj();
	BOOL Curve_Filter(CString CurveFullPath);
	virtual void DeleteContents();
// Implementation
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll);
	virtual ~CImageManView();
	virtual	void OnDraw(CDC* pDC);		// overridden to draw this view
	virtual	void OnInitialUpdate();		// first time after construct
	virtual void OnUpdate(CView* , LPARAM lHint, CObject* pHint);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActiveView,	CView* pDeactiveView);
	// Generated message map functions
	//{{AFX_MSG(CImageManView)
	afx_msg void OnAddCurve();
	afx_msg void OnAddAzimfreq();
	afx_msg void OnAddDevdaz();
	afx_msg void OnAddDangddir();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnObjectLink();
	afx_msg void OnUpdateObjectLink(CCmdUI* pCmdUI);
	afx_msg void OnObjectUnlink();
	afx_msg void OnUpdateObjectUnlink(CCmdUI* pCmdUI);
	afx_msg void OnViewTrackAlignClient();
	afx_msg void OnViewTrackAlignDepth();
	afx_msg void OnViewTrackArrangleLeft();
	afx_msg void OnViewTrackArrangleLeftright();
	afx_msg void OnViewTrackArrangleRight();
	afx_msg void OnViewTrackHoriArrangle();
	afx_msg void OnViewTrackHoriAutoarangle();
	afx_msg void OnViewTrackHoriSetautoarrangle();
	afx_msg void OnViewTrackVertAlign();
	afx_msg void OnViewTrackVertSetautoalign();
	afx_msg void OnUpdateViewTrackAlignClient(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTrackAlignDepth(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTrackArrangleLeft(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTrackArrangleLeftright(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTrackArrangleRight(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTrackHoriArrangle(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTrackHoriAutoarangle(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTrackVertAlign(CCmdUI* pCmdUI);
	afx_msg void OnViewTrackVertAutoalign();
	afx_msg void OnUpdateViewTrackVertAutoalign(CCmdUI* pCmdUI);
	afx_msg void OnViewTrackAlignTop();
	afx_msg void OnUpdateViewTrackAlignTop(CCmdUI* pCmdUI);
	afx_msg void OnAddTrack();
	afx_msg void OnUpdateAddTrack(CCmdUI* pCmdUI);
	afx_msg void OnDrawSelect();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnObjectPropterties();
	afx_msg void OnUpdateObjectPropterties(CCmdUI* pCmdUI);
	afx_msg void OnTrackHoriFIDAlignLeft();
	afx_msg void OnTrackHoriFIDAlignLeftright();
	afx_msg void OnTrackHoriFIDAlignRight();
	afx_msg void OnTrackHoriFIDArrangleBottom();
	afx_msg void OnTrackHoriFIDArrangleLeft();
	afx_msg void OnTrackHoriFIDArrangleLeftright();
	afx_msg void OnTrackHoriFIDArrangleRight();
	afx_msg void OnTrackHoriFIDArrangleTop();
	afx_msg void OnTrackHoriFIDArrangleTopbottom();
	afx_msg void OnTrackHoriFIDSetheight();
	afx_msg void OnTrackHoriFIDSetwidth();
	afx_msg void OnTrackHoriFIDAlignTop();
	afx_msg void OnTrackHoriFIDAlignBottom();
	afx_msg void OnTrackHoriFIDAlignTopbottom();
	afx_msg void OnUpdateTrackHoriFIDAlignBottom(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrackHoriFIDAlignLeft(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrackHoriFIDAlignLeftright(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrackHoriFIDAlignRight(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrackHoriFIDAlignTop(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrackHoriFIDAlignTopbottom(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrackHoriFIDArrangleBottom(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrackHoriFIDArrangleLeft(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrackHoriFIDArrangleLeftright(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrackHoriFIDArrangleRight(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrackHoriFIDArrangleTop(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrackHoriFIDArrangleTopbottom(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrackHoriFIDSetheight(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrackHoriFIDSetwidth(CCmdUI* pCmdUI);
	afx_msg void OnTrackAutoarrangle();
	afx_msg void OnTrackArrangle();
	afx_msg void OnUpdateTrackArrangle(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrackAutoarrangle(CCmdUI* pCmdUI);
	afx_msg void OnTrackVertFIDAlignTop();
	afx_msg void OnTrackVertFIDAlignTopbottom();
	afx_msg void OnTrackVertFIDAlignBottom();
	afx_msg void OnTrackVertFIDArrangleLeft();
	afx_msg void OnTrackVertFIDArrangleLeftright();
	afx_msg void OnTrackVertFIDArrangleRight();
	afx_msg void OnUpdateTrackVertFIDAlignTop(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrackVertFIDAlignTopbottom(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrackVertFIDAlignBottom(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrackVertFIDArrangleLeft(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrackVertFIDArrangleLeftright(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrackVertFIDArrangleRight(CCmdUI* pCmdUI);
	afx_msg void OnTrackVertFIDSetheight();
	afx_msg void OnUpdateTrackVertFIDSetheight(CCmdUI* pCmdUI);
	afx_msg void OnTrackVertFIDSetwidth();
	afx_msg void OnUpdateTrackVertFIDSetwidth(CCmdUI* pCmdUI);
	afx_msg void OnTrackTrackArrangleLeft();
	afx_msg void OnTrackTrackArrangleLeftright();
	afx_msg void OnTrackTrackArrangleRight();
	afx_msg void OnUpdateTrackTrackArrangleLeft(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrackTrackArrangleLeftright(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrackTrackArrangleRight(CCmdUI* pCmdUI);
	afx_msg void OnTrackHoriFIDHoriarrangleNone();
	afx_msg void OnUpdateTrackHoriFIDHoriarrangleNone(CCmdUI* pCmdUI);
	afx_msg void OnTrackHoriFIDHorialignNone();
	afx_msg void OnUpdateTrackHoriFIDHorialignNone(CCmdUI* pCmdUI);
	afx_msg void OnTrackHoriFIDVertalignNone();
	afx_msg void OnUpdateTrackHoriFIDVertalignNone(CCmdUI* pCmdUI);
	afx_msg void OnTrackHoriFIDVertarrangleNone();
	afx_msg void OnUpdateTrackHoriFIDVertarrangleNone(CCmdUI* pCmdUI);
	afx_msg void OnTrackVertFIDArrangleNone();
	afx_msg void OnUpdateTrackVertFIDArrangleNone(CCmdUI* pCmdUI);
	afx_msg void OnTrackVertFIDAlignNone();
	afx_msg void OnUpdateTrackVertFIDAlignNone(CCmdUI* pCmdUI);
	afx_msg void OnTrackTrackArrangleNone();
	afx_msg void OnUpdateTrackTrackArrangleNone(CCmdUI* pCmdUI);
	afx_msg void OnTrackSetautoarrangle();
	afx_msg void OnUpdateTrackSetautoarrangle(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnViewTrackSetwidth();
	afx_msg void OnUpdateViewTrackSetwidth(CCmdUI* pCmdUI);
	afx_msg void OnViewTrackTitleShow();
	afx_msg void OnUpdateViewTrackTitleShow(CCmdUI* pCmdUI);
	afx_msg void OnViewTrackHeadSetheightAuto();
	afx_msg void OnUpdateViewTrackHeadSetheightAuto(CCmdUI* pCmdUI);
	afx_msg void OnViewTrackHeadSetheightMax();
	afx_msg void OnUpdateViewTrackHeadSetheightMax(CCmdUI* pCmdUI);
	afx_msg void OnViewTrackTitleSetheight();
	afx_msg void OnUpdateViewTrackTitleSetheight(CCmdUI* pCmdUI);
	afx_msg void OnTrackTrackSetwidth();
	afx_msg void OnUpdateTrackTrackSetwidth(CCmdUI* pCmdUI);
	afx_msg void OnTrackTrackTitleSetheight();
	afx_msg void OnUpdateTrackTrackTitleSetheight(CCmdUI* pCmdUI);
	afx_msg void OnTrackTrackTitleShow();
	afx_msg void OnUpdateTrackTrackTitleShow(CCmdUI* pCmdUI);
	afx_msg void OnTrackTrcakTitleHide();
	afx_msg void OnUpdateTrackTrcakTitleHide(CCmdUI* pCmdUI);
	afx_msg void OnTrackTrackHeadSetheightAuto();
	afx_msg void OnUpdateTrackTrackHeadSetheightAuto(CCmdUI* pCmdUI);
	afx_msg void OnTrackTrackHeadSetheightMax();
	afx_msg void OnUpdateTrackTrackHeadSetheightMax(CCmdUI* pCmdUI);
	afx_msg void OnPreRangvalue();
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnInsertpoint();
	afx_msg void OnUpdateInsertpoint(CCmdUI* pCmdUI);
	afx_msg void OnCurveMouseedit();
	afx_msg void OnCurveDisvalue();
	afx_msg void OnCurveMovedep();
	afx_msg void OnUpdateCurveMovedep(CCmdUI* pCmdUI);
	afx_msg void OnCurvePlusmouse();
	afx_msg void OnUpdateCurvePlusmouse(CCmdUI* pCmdUI);
	afx_msg void OnModfyDepsegment();
	afx_msg void OnUpdatePreRangvalue(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCurveMouseedit(CCmdUI* pCmdUI);
	afx_msg void OnCurveCounterglow();
	afx_msg void OnUpdateCurveCounterglow(CCmdUI* pCmdUI);
	afx_msg void OnMultiSectDep();
	afx_msg void OnUpdateMultiSectDep(CCmdUI* pCmdUI);
	afx_msg void OnValueSendtoClipboard();
	afx_msg void OnUpdateValueSendtoClipboard(CCmdUI* pCmdUI);
	afx_msg void OnAddImage();
	afx_msg void OnAddImage3d();
	afx_msg void OnAddStratum();
	afx_msg void OnMarkStratum();
	afx_msg void OnUpdateMarkStratum(CCmdUI* pCmdUI);
	afx_msg void OnMarkCCL();
	afx_msg void OnUpdateMarkCCL(CCmdUI* pCmdUI);
	afx_msg void OnMarkConfracture();
	afx_msg void OnUpdateMarkConfracture(CCmdUI* pCmdUI);
	afx_msg void OnMarkResfracture();
	afx_msg void OnUpdateMarkResfracture(CCmdUI* pCmdUI);
	afx_msg void OnMarkAvalanche();
	afx_msg void OnUpdateMarkAvalanche(CCmdUI* pCmdUI);
	afx_msg void OnAddMarkvector();
	afx_msg void OnMarkResfracture2();
	afx_msg void OnUpdateMarkResfracture2(CCmdUI* pCmdUI);
	afx_msg void OnAddMarkfreq();
	afx_msg void OnAddImagecurve();
	afx_msg void OnEddyPlay();
	afx_msg void OnUpdateEddyPlay(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPlayStop();
	afx_msg void OnUpdatePlayStop(CCmdUI* pCmdUI);
	afx_msg void OnEddyRight();
	afx_msg void OnUpdateEddyRight(CCmdUI* pCmdUI);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnAddEcho();
	afx_msg void OnFrequency();
	afx_msg void OnUpdateFrequency(CCmdUI* pCmdUI);
	afx_msg void OnAddPole();
	afx_msg void OnUpdateCurveDisvalue(CCmdUI* pCmdUI);
	afx_msg void OnHoleAnalysis();
	afx_msg void OnUpdateHoleAnalysis(CCmdUI* pCmdUI);
	afx_msg void OnImageAnalysis();

	afx_msg void OnCurveDepico();
	afx_msg void OnUpdateCurveDepico(CCmdUI* pCmdUI);
	afx_msg void OnComputeDip();
	afx_msg void OnDipPara();
	afx_msg void OnDelHandwork();
	afx_msg void OnUpdateComputeDip(CCmdUI* pCmdUI);
	afx_msg void OnModifyHandwork();
	afx_msg void OnUpdateModifyHandwork(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDelHandwork(CCmdUI* pCmdUI);
	afx_msg void OnMarkLayerfracture();
	afx_msg void OnUpdateMarkLayerfracture(CCmdUI* pCmdUI);
	afx_msg void OnMarkHole();
	afx_msg void OnUpdateMarkHole(CCmdUI* pCmdUI);
	afx_msg void OnPartCopy();
	afx_msg void OnUpdatePartCopy(CCmdUI* pCmdUI);
	afx_msg void OnPartPaste();
	afx_msg void OnUpdatePartPaste(CCmdUI* pCmdUI);
	afx_msg void OnCurvePlus();
	afx_msg void OnUpdateCurvePlus(CCmdUI* pCmdUI);
	afx_msg void OnCurvePartFilter();
	afx_msg void OnUpdateCurvePartFilter(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBmpSenttoCilp();
	DECLARE_MESSAGE_MAP()
public:
	
	//显示缝洞联通性
	afx_msg void OnDrawConnection();
	afx_msg void OnUpdateDrawConnection(CCmdUI *pCmdUI);
	//新建图像特征数据
	afx_msg void OnCreateContour();
	afx_msg void OnUpdateCreateContour(CCmdUI *pCmdUI);
	//单个删除
	afx_msg void OnDelSelhole();
	afx_msg void OnUpdateDelSelhole(CCmdUI *pCmdUI);
	//删除面积小于...
	afx_msg void OnDelSmallSel();
	afx_msg void OnUpdateDelSmallSel(CCmdUI *pCmdUI);
	//删除面积大于...
	afx_msg void OnDelBigSel();
	afx_msg void OnUpdateDelBigSel(CCmdUI *pCmdUI);
	//设置自动拾取参数
	afx_msg void OnSetContourPara();
	//区域删除
	//删除选定的矩形区域图像特征
	afx_msg void OnDelRectangle();
	afx_msg void OnUpdateDelRectangle(CCmdUI *pCmdUI);
	//手工对选定的矩形区域图像特征拾取
	afx_msg void OnGetRectanglContour();
	afx_msg void OnUpdateGetRectanglContour(CCmdUI *pCmdUI);
	//手工标注椭圆图像特征
	afx_msg void OnGetEllipseContour();							
	afx_msg void OnUpdateGetEllipseContour(CCmdUI *pCmdUI);
	//加载图像特征数据
	afx_msg void OnOpenContour();
	afx_msg void OnUpdateOpenContour(CCmdUI *pCmdUI);
	//保存图像特征数据
	afx_msg void OnSaveContour();
	afx_msg void OnUpdateSaveContour(CCmdUI *pCmdUI);
	//孔洞特征数据另存为...
	afx_msg void OnSaveasContour();
	afx_msg void OnUpdateSaveasContour(CCmdUI *pCmdUI);
	//生成孔洞分类统计曲线
	//生成孔洞直径频谱曲线和不同孔径标准的面孔率、孔洞密度、面孔率累加曲线
	afx_msg void OnPickupStat();
	afx_msg void OnUpdatePickupStat(CCmdUI *pCmdUI);
	//生成储层孔洞分类统计表
	//在综合解释表给定的井段内对孔洞进行统计并生成统计表ReHole.List
	afx_msg void OnPickupList();
	afx_msg void OnUpdatePickupList(CCmdUI *pCmdUI);
	//裂缝异常带分析
	//新建图像特征存储空间并自动拾取裂缝的图像特征
	afx_msg void OnCreateSinContour();
	afx_msg void OnUpdateCreateSinContour(CCmdUI *pCmdUI);
	//生成裂缝描述曲线
	//裂缝长度FVTL
	//裂缝宽度FVA
	//裂缝密度FVDA(每米井段)
	//裂缝水动力宽度FVAH
	//裂缝孔隙度FVPA
	afx_msg void OnPickupSinStat();
	afx_msg void OnUpdatePickupSinStat(CCmdUI *pCmdUI);
	//生成储层裂缝统计表
	//在综合解释表给定的井段内对裂缝进行统计并生成统计表ReCrack.List
	afx_msg void OnReCrackList();
	afx_msg void OnUpdateReCrackList(CCmdUI *pCmdUI);
	//生成全井段裂缝统计表
	//根据层理裂缝标注对象生成全井段单条层理裂缝参数统计表Crack.List
	afx_msg void OnCrackList();
	afx_msg void OnUpdateCrackList(CCmdUI *pCmdUI);
	//设置孔洞统计参数
	afx_msg void OnSetholeStatPara();
	//设置裂缝统计参数
	afx_msg void OnSetCrackStatPara();
	
	//删除全井段的孔洞标注
	afx_msg void OnDelAllhole();
	afx_msg void OnUpdateDelAllhole(CCmdUI *pCmdUI);
	//删除非储层段的孔洞标注
	afx_msg void OnPersistResult();
	afx_msg void OnUpdatePersistresult(CCmdUI *pCmdUI);
	//全井壁图像恢复
	//基于纹理合成的全井壁图像修复技术
	//生成全井壁图像曲线名为选中图像曲线名增加PIC
	afx_msg void OnCreateFullImage();
	afx_msg void OnUpdateCreateFullImage(CCmdUI *pCmdUI);
	//井眼图像三维重建
	//生成2个方位的三维图像垂直切片
	afx_msg void OnCreateImage3d();
	afx_msg void OnUpdateCreateImage3d(CCmdUI *pCmdUI);
	//区间孔隙度谱分析
	//生成孔隙度谱和原生次生孔隙度曲线
	afx_msg void OnPorCompute();
	afx_msg void OnUpdatePorCompute(CCmdUI *pCmdUI);
	//用浅侧向对选中图像进行电阻率刻度
	afx_msg void OnRmfScale();
	afx_msg void OnUpdateRmfScale(CCmdUI *pCmdUI);
	//孔洞电导率直方图分析
	afx_msg void OnDrawFrequency();
	afx_msg void OnUpdateDrawFrequency(CCmdUI *pCmdUI);	
	//缝洞直方图交会图分析
	void DrawFrequency(float sdep,float edep);
	
	//孔洞图像值--电导率交会图分析
	afx_msg void OnDrawCross();
	afx_msg void OnUpdateDrawCross(CCmdUI *pCmdUI);
	void DrawCross(float dep1,float dep2);

	//快速生成单条面孔率曲线
	afx_msg void OnCreateHpor();
	afx_msg void OnUpdateCreateHpor(CCmdUI *pCmdUI);
	//孔洞直径直方图分析
	afx_msg void OnDrawDiameter();
	afx_msg void OnUpdateDrawDiameter(CCmdUI *pCmdUI);
	//孔洞面孔率直方图分析
	afx_msg void OnDrawPor();
	afx_msg void OnUpdateDrawPor(CCmdUI *pCmdUI);
	//根据输入的约束曲线下限值生成缝洞综合统计表
	afx_msg void OnCreateimagelist();
	afx_msg void OnUpdateCreateimagelist(CCmdUI *pCmdUI);
	//统计孔洞参数
	//返回 序号,起始深度,终止深度,段长,洞密度,大洞数,中洞数,小洞数
	CString StatHolePara(int No,float dep1,float dep2);
	//统计裂缝参数
	//返回 裂缝条数,缝密度
	CString StatCrackPara(float dep1,float dep2);
	//统计曲线最大值
	float StatCurvePara(CString PathName,float dep1,float dep2);
};

/////////////////////////////////////////////////////////////////////////////

#endif
