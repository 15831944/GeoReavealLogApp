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
	
	//��ȡͼ����ؿ��Ʋ���
	CString m_PickupFIDName;				//����ȫ��
	CString m_PickupAliasName;				//���߱���
	int 	m_PickupBit;					//��ͷֱ�� mm
	int 	m_PickupHorScale;				//�����ͼ����
	CRect	m_PickupRect;					//�½�ʱ��ͼ���С 
	float	m_PickupStDep;					//�½�ʱ����ʼ���
	float	m_PickupEnDep;					//�½�ʱ�Ľ������
	float   m_PickupVerScale;				//�����ͼ����
	float   m_CoverageRate;					//�Զ�ʰȡ������

	//���ڿ׶��Զ�ʰȡ����
	int m_KPara;							//����ֵ������
	float m_HoleMinAcr;						//��ʰȡ����m_HoleMinAcr�Ŀ׶�
	BOOL  m_bHoleMinAcr;
	float m_HoleMaxAcr;						//��ʰȡС��m_HoleMaxAcr�Ŀ׶�
	BOOL  m_bHoleMaxAcr;
	float m_HoleMinRound;					//��ʰȡԲ�ȴ���m_HoleMinRound�Ŀ׶�
	BOOL m_bMinRound;
	float m_HoleMaxRound;					//��ʰȡԲ�ȴ���m_HoleMaxRound�Ŀ׶�
	BOOL m_bMaxRound;

	//�׶��̶ȹ�ϵ���� y=A*x+B
	float m_fA,m_fB;
	//�ѷ�̶ȹ�ϵ���� y=C*x+D
	float m_fC,m_fD;

	//������ͨ�Բ���
	float	m_ImageMax;						//����������ֵ ȱʡ1024��CIFLog�����Ϊ256
	float	m_ImageMin;						//����������ֵ ȱʡ0
	
	int		m_iPickupSinWidth;				//ʰȡ�����ѷ�������mm
	CString m_sPickupSinLabel;				//ʰȡ�����ѷ����ͱ�ʶ
	//�ѷ�ͳ�Ʋ���
	float m_SinStatSdep,m_SinStatEdep;		//�ѷ�ͳ�ƾ���
	float m_SinStatWin;						//��������
	float m_SinStatRlev;					//��������
	//�����ѷ��Ȳ���
	float m_Rm;								//�ཬ������
	float m_ParC;							//ˮ��������C
	float m_ParB;							//ˮ��������B
	CString	m_sRxoWell;						//��׼�����ʾ���
	CString	m_sRxoName;						//ǳ��������
	
	//�׶�ͳ�Ʋ���
	CString m_StatWellName;					//ͳ�����߱���λ��
	float m_StatWin;						//��������
	float m_StatRlev;						//��������
	float m_ContourMax;						//�������׶�ֱ��
	float m_StatSdep,m_StatEdep;			//ͳ�ƾ���
	//�Կ����׶�ֱ�����з���ͳ��
	float m_StatDiam1;						//С��m_StatDiam1��ΪС��
	float m_StatDiam2;						//����m_StatDiam1Ϊ�пף�С��m_StatDiam2
	float m_StatDiam3;						//����m_StatDiam2Ϊ��ף�С��m_StatDiam3����m_StatDiam3Ϊ�ܶ�
	//���ɿ����׶���СƵ��ͼ
	int	  m_nSpcNum;						//Ƶ�����ߺ���̶�������

	
	CString		m_PickupFileName;			//��������׶��������ļ���
	CImageObj	*m_pPickupObj;				//�½�ʱ��ͼ�����
	CDibImage	m_DibImage;					//��ת̽��ͼ��߽���Ҫ������	
	CObList		m_ContourList;				//�洢��ʰȡ�Ŀ�����׶�
	
	//ʰȡ�����׶��Ľ�������ѡ��
	int		m_iHoleOperate;
	BOOL	m_bRectangleSel;
	CPoint	m_StSelPt,m_EnSelPt;
	
	int     m_iPickupNum;					//ʰȡ������׶�������¼�������˳�ʱ�����Ƿ񱣴�
	BOOL	m_bConnection;					//��ʾ�׶���ͨ��

	void DelSelContour(CPoint pt);
	void RectanglePickupContour();
	void EllipsePickupContour();
	
	BOOL SavePickupContour(BOOL bSave);
	void DrawPickupContour(CDC *pDC);
	//���㵥���ѷ촢�����������CSinPoint��������Crack.List
	BOOL StatSinContour();
// Attributes
public:	
	float m_sindep0;
	//ʰȡ�����ѷ�Ľ�������ѡ��
	int m_SinFlag;
	CDrawObj* m_pHandDrawObj;
	CMeasureMultiLine* m_pMeasure;
	void StartHandDrawSin();
	//�Զ�ʰȡѡ���������׷�ӵ�HandWork.List�ļ���\n�������ʰȡ
	void RectangleComputeDip();
	//ɾ��ѡ�������������HandWork.List�ļ�\n��������ѷ�ɾ��
	void RectangleDeleteDip();

	//�������߽����༭������ѷ�ʰȡ
	afx_msg LRESULT OnEndClick(WPARAM wParam, LPARAM lParam);
	//�ж����߽����༭������ѷ�ʰȡ
	afx_msg LRESULT OnCancelClick(WPARAM wParam, LPARAM lParam);
	//�������߽����༭��д���ļ�
	void EndHandDrawCurve();
	
	BOOL			m_EddyPlay;
	CImage3D*		m_pImage3D;
	BOOL			m_bAlikePro;
	CStringArray	m_SelCurveNameArray;
	float			depArray[100][2];//������������ȴ���

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

	//�Զ�ʶȡ���ʳ���������Ǽ�����ز�����	
	//�μ���Ǽ������С���ϵ��
	float	m_WinLength;         //��ضԱȴ���
	float	m_StepLength;        //����������ߵĲ���
	float	m_SearchLength;      //����̽������
	float	m_DangMax;           //������
	float	m_RMin;              //�μ���Ǽ������С���ϵ��
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
	
	//��ʾ�춴��ͨ��
	afx_msg void OnDrawConnection();
	afx_msg void OnUpdateDrawConnection(CCmdUI *pCmdUI);
	//�½�ͼ����������
	afx_msg void OnCreateContour();
	afx_msg void OnUpdateCreateContour(CCmdUI *pCmdUI);
	//����ɾ��
	afx_msg void OnDelSelhole();
	afx_msg void OnUpdateDelSelhole(CCmdUI *pCmdUI);
	//ɾ�����С��...
	afx_msg void OnDelSmallSel();
	afx_msg void OnUpdateDelSmallSel(CCmdUI *pCmdUI);
	//ɾ���������...
	afx_msg void OnDelBigSel();
	afx_msg void OnUpdateDelBigSel(CCmdUI *pCmdUI);
	//�����Զ�ʰȡ����
	afx_msg void OnSetContourPara();
	//����ɾ��
	//ɾ��ѡ���ľ�������ͼ������
	afx_msg void OnDelRectangle();
	afx_msg void OnUpdateDelRectangle(CCmdUI *pCmdUI);
	//�ֹ���ѡ���ľ�������ͼ������ʰȡ
	afx_msg void OnGetRectanglContour();
	afx_msg void OnUpdateGetRectanglContour(CCmdUI *pCmdUI);
	//�ֹ���ע��Բͼ������
	afx_msg void OnGetEllipseContour();							
	afx_msg void OnUpdateGetEllipseContour(CCmdUI *pCmdUI);
	//����ͼ����������
	afx_msg void OnOpenContour();
	afx_msg void OnUpdateOpenContour(CCmdUI *pCmdUI);
	//����ͼ����������
	afx_msg void OnSaveContour();
	afx_msg void OnUpdateSaveContour(CCmdUI *pCmdUI);
	//�׶������������Ϊ...
	afx_msg void OnSaveasContour();
	afx_msg void OnUpdateSaveasContour(CCmdUI *pCmdUI);
	//���ɿ׶�����ͳ������
	//���ɿ׶�ֱ��Ƶ�����ߺͲ�ͬ�׾���׼������ʡ��׶��ܶȡ�������ۼ�����
	afx_msg void OnPickupStat();
	afx_msg void OnUpdatePickupStat(CCmdUI *pCmdUI);
	//���ɴ���׶�����ͳ�Ʊ�
	//���ۺϽ��ͱ�����ľ����ڶԿ׶�����ͳ�Ʋ�����ͳ�Ʊ�ReHole.List
	afx_msg void OnPickupList();
	afx_msg void OnUpdatePickupList(CCmdUI *pCmdUI);
	//�ѷ��쳣������
	//�½�ͼ�������洢�ռ䲢�Զ�ʰȡ�ѷ��ͼ������
	afx_msg void OnCreateSinContour();
	afx_msg void OnUpdateCreateSinContour(CCmdUI *pCmdUI);
	//�����ѷ���������
	//�ѷ쳤��FVTL
	//�ѷ���FVA
	//�ѷ��ܶ�FVDA(ÿ�׾���)
	//�ѷ�ˮ�������FVAH
	//�ѷ��϶��FVPA
	afx_msg void OnPickupSinStat();
	afx_msg void OnUpdatePickupSinStat(CCmdUI *pCmdUI);
	//���ɴ����ѷ�ͳ�Ʊ�
	//���ۺϽ��ͱ�����ľ����ڶ��ѷ����ͳ�Ʋ�����ͳ�Ʊ�ReCrack.List
	afx_msg void OnReCrackList();
	afx_msg void OnUpdateReCrackList(CCmdUI *pCmdUI);
	//����ȫ�����ѷ�ͳ�Ʊ�
	//���ݲ����ѷ��ע��������ȫ���ε��������ѷ����ͳ�Ʊ�Crack.List
	afx_msg void OnCrackList();
	afx_msg void OnUpdateCrackList(CCmdUI *pCmdUI);
	//���ÿ׶�ͳ�Ʋ���
	afx_msg void OnSetholeStatPara();
	//�����ѷ�ͳ�Ʋ���
	afx_msg void OnSetCrackStatPara();
	
	//ɾ��ȫ���εĿ׶���ע
	afx_msg void OnDelAllhole();
	afx_msg void OnUpdateDelAllhole(CCmdUI *pCmdUI);
	//ɾ���Ǵ���εĿ׶���ע
	afx_msg void OnPersistResult();
	afx_msg void OnUpdatePersistresult(CCmdUI *pCmdUI);
	//ȫ����ͼ��ָ�
	//��������ϳɵ�ȫ����ͼ���޸�����
	//����ȫ����ͼ��������Ϊѡ��ͼ������������PIC
	afx_msg void OnCreateFullImage();
	afx_msg void OnUpdateCreateFullImage(CCmdUI *pCmdUI);
	//����ͼ����ά�ؽ�
	//����2����λ����άͼ��ֱ��Ƭ
	afx_msg void OnCreateImage3d();
	afx_msg void OnUpdateCreateImage3d(CCmdUI *pCmdUI);
	//�����϶���׷���
	//���ɿ�϶���׺�ԭ��������϶������
	afx_msg void OnPorCompute();
	afx_msg void OnUpdatePorCompute(CCmdUI *pCmdUI);
	//��ǳ�����ѡ��ͼ����е����ʿ̶�
	afx_msg void OnRmfScale();
	afx_msg void OnUpdateRmfScale(CCmdUI *pCmdUI);
	//�׶��絼��ֱ��ͼ����
	afx_msg void OnDrawFrequency();
	afx_msg void OnUpdateDrawFrequency(CCmdUI *pCmdUI);	
	//�춴ֱ��ͼ����ͼ����
	void DrawFrequency(float sdep,float edep);
	
	//�׶�ͼ��ֵ--�絼�ʽ���ͼ����
	afx_msg void OnDrawCross();
	afx_msg void OnUpdateDrawCross(CCmdUI *pCmdUI);
	void DrawCross(float dep1,float dep2);

	//�������ɵ������������
	afx_msg void OnCreateHpor();
	afx_msg void OnUpdateCreateHpor(CCmdUI *pCmdUI);
	//�׶�ֱ��ֱ��ͼ����
	afx_msg void OnDrawDiameter();
	afx_msg void OnUpdateDrawDiameter(CCmdUI *pCmdUI);
	//�׶������ֱ��ͼ����
	afx_msg void OnDrawPor();
	afx_msg void OnUpdateDrawPor(CCmdUI *pCmdUI);
	//���������Լ����������ֵ���ɷ춴�ۺ�ͳ�Ʊ�
	afx_msg void OnCreateimagelist();
	afx_msg void OnUpdateCreateimagelist(CCmdUI *pCmdUI);
	//ͳ�ƿ׶�����
	//���� ���,��ʼ���,��ֹ���,�γ�,���ܶ�,����,�ж���,С����
	CString StatHolePara(int No,float dep1,float dep2);
	//ͳ���ѷ����
	//���� �ѷ�����,���ܶ�
	CString StatCrackPara(float dep1,float dep2);
	//ͳ���������ֵ
	float StatCurvePara(CString PathName,float dep1,float dep2);
};

/////////////////////////////////////////////////////////////////////////////

#endif
