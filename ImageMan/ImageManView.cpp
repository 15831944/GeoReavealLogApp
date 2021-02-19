/******************************************************************
*   ImageManView.cpp - implementation of the CImageManView class  *
*																  *
*    author:zhaozhongmin	2019.3								  *
*                                                                 *
*******************************************************************/

#include "stdafx.h"
#include <afxpriv.h>
#include <math.h> 
#include <io.h>
#include <process.h>

#include "FIDio.h"
#include "ImageMan.h"
#include "MainFrm.h"
#include "ImageManDoc.h"
#include "ImageManView.h"
#include "TrackDraw.h"
#include "FIDDraw.h"
#include "CurveDraw.h"
#include "AngDraw.h"
#include "AzimFreq.h"
#include "DevDazDraw.h"
#include "InputBox.h"
#include "LineDraw.h"
#include "mmsystem.h"
#include "DepthDialog.h"
#include "Guage.h"
#include "DepSect.h"
#include "SelectCurve.h"
#include "ImageDraw.h"
#include "Image3D.h"
#include "StratumDraw.h"
#include "MeasureMultiLine.h"
#include "DIPCalculate.h"
#include "MarkVectorDraw.h"
#include "LogProcess.h"
#include "MarkFreq.h"
#include "ImageCurveDraw.h"
#include "EchoDraw.h"
#include "PoleDraw.h"
#include "HoleAnalysis.h"
#include "ProMultiCurveDep.h"
#include "CurveDep.h"
#include "DIPCompute.h"
#include "DIPComputeDlg.h"
#include "LogProcess.h"
#include "DIPCalculate.h"
#include "DIPCompute.h"
#include "StratumDraw.h"
#include "ModifyHandWorkDlg.h"
#include "SetContourPara.h"
#include "ContourStatDlg.h"
#include "ResultDraw.h"
#include "SetContourSinPara.h"
#include "ContourSinStatDlg.h"
#include "Integrate.h"
#include "depthdialog.h"
#include "Insert3D.h"
#include "CreateSpectDlg.h"
#include "RxoScaleDlg.h"
#include "FrequencyDlg.h"
#include "CrossDlg.h"
#include "FoldDraw.h"
#include "CurvePlus.h"
#include "ArrayWaveDraw.h"
#include "ImageRepair.h"
#include "CreateImageListDlg.h"

#pragma comment(lib,"winmm.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CImageManView

IMPLEMENT_DYNCREATE(CImageManView, CScrollView)

CImageManView::CImageManView()
{
	m_pSelectObj = NULL;
	m_pHandDrawObj=NULL;
	bObjectLink = FALSE;
	m_LeftSpace = m_TopSpace = 20;
	m_bFirstUpdate = TRUE;
	m_bDisFIDValue=FALSE;
	m_bCounterGlow=FALSE;
	pDoc = NULL;
	m_UnDo=m_ReDo=m_InitOprationNum=m_PreOprationNum=0;
	m_UnDoFilePath=((CImageManApp*)AfxGetApp())->m_UnDoFilePath ;
	m_MouseTrace=FALSE;

	m_bProMutilDep=FALSE;
	m_pMeasure = NULL;
	m_EddyPlay=FALSE;
	m_pImage3D=NULL;
	m_bAlikePro=FALSE;
	m_SelCurveNameArray.RemoveAll();
	m_sindep0=0.f;
	//自动识取地质沉积层理（倾角计算相关参数）
	m_WinLength = 2.54f;
	m_StepLength = 0.127f;
	m_SearchLength = 0.508f;
	m_DangMax = 70.0f;
	m_RMin = 0.5f;
	m_GradCons=100000.f;
	m_GradMin=70.f;

	m_PickupFileName=_T("");
	m_pPickupObj=NULL;

	//拾取层理裂缝的交互操作选项
	m_SinFlag=-10;
	//删除孔洞操作选项
	m_iHoleOperate=-1;
	// 特征值聚类数
	m_KPara=110;
	//仅拾取大于m_HoleMinAcr的孔洞
	m_HoleMinAcr=10.f;
	m_bHoleMinAcr=FALSE;
	//仅拾取小于m_HoleMaxAcr的孔洞
	m_bHoleMaxAcr=FALSE;
	m_HoleMaxAcr=100.f;

	//仅拾取圆度大于m_HoleMinRound的孔洞
	m_bMinRound=FALSE;
	m_HoleMinRound=50.f;
	m_bMaxRound=FALSE;
	m_HoleMaxRound=20.f;

	m_bRectangleSel=FALSE;
	//拾取颗粒或孔洞操作记录，用于退出时提醒是否保存
	m_iPickupNum=0;
	m_CoverageRate=1.0;
	//交互解释统计参数
	m_StatWellName=_T("");//统计曲线保存位置
	m_StatWin=0.5f;//2英尺
	m_StatRlev=0.025f;//0.5英尺
	//颗粒孔洞直径频谱刻度区间数
	m_nSpcNum=40;
	//最大颗粒孔洞直径
	m_ContourMax=60.f;

	//以颗粒孔洞直径进行分类统计
	m_StatDiam1=10.f;//小于m_StatDiam1的为小孔
	m_StatDiam2=50.f;//大于m_StatDiam1为中孔，小于m_StatDiam2
	m_StatDiam3=100.f;//大于m_StatDiam2为大孔，小于m_StatDiam3大于m_StatDiam3为溶洞
	//孔洞储层参数统计井段
	m_StatSdep=m_StatEdep=-9999.f;

	//拾取层理裂缝的最大宽度mm
	m_iPickupSinWidth=40;			   
	//拾取层理裂缝类型标识
	m_sPickupSinLabel="张开缝充填缝压裂缝层间缝半充填缝";

	m_SinStatWin=0.6906f;//2英尺
	m_SinStatRlev=0.0762f;//1/4英尺
	m_ParC=0.004801f;
	m_ParB=0.863f;
	//裂缝储层参数统计井段
	m_SinStatSdep=m_SinStatEdep=-9999.f; 
	//标准电阻率井名
	m_sRxoWell=_T("");
	//浅侧向曲线				  
	m_sRxoName="RXO";
	//泥浆电阻率
	m_Rm=0.04f;	
	//显示孔洞联通性
	m_bConnection=FALSE;

	//孔洞刻度关系参数 y=A*x+B
	//裂缝刻度关系参数 y=C*x+D
	m_fA=0.38f;
	m_fB=16.53f;
	m_fC=1.f;
	m_fD=0.f;

	//分析联通性参用
	m_ImageMax=1024.f;		//成像数据最值 缺省1024，CIFLog处理的为256
	m_ImageMin=440.f;		//成像数据最值 缺省0
}

CImageManView::~CImageManView()
{   
	POSITION temp;
	POSITION pos=m_ContourList.GetHeadPosition();	 
	while(pos!=NULL)
	{
		temp=pos;
		m_ContourList.GetNext(pos);
		m_ContourList.RemoveAt(temp);
	}
	//拾取层理裂缝的交互操作选项
	if(m_pMeasure!=NULL)
		delete m_pMeasure;
}

BEGIN_MESSAGE_MAP(CImageManView, CScrollView)
	//{{AFX_MSG_MAP(CImageManView)
	ON_COMMAND(ID_ADD_CURVE, OnAddCurve)
	ON_COMMAND(ID_ADD_AZIMFREQ, OnAddAzimfreq)
	ON_COMMAND(ID_ADD_DEVDAZ, OnAddDevdaz)
	ON_COMMAND(ID_ADD_DANGDDIR, OnAddDangddir)
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_DROPFILES()
	ON_COMMAND(ID_OBJECT_LINK, OnObjectLink)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_LINK, OnUpdateObjectLink)
	ON_COMMAND(ID_OBJECT_UNLINK, OnObjectUnlink)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_UNLINK, OnUpdateObjectUnlink)
	ON_COMMAND(ID_VIEW_TRACK_ALIGN_CLIENT, OnViewTrackAlignClient)
	ON_COMMAND(ID_VIEW_TRACK_ALIGN_DEPTH, OnViewTrackAlignDepth)
	ON_COMMAND(ID_VIEW_TRACK_ARRANGLE_LEFT, OnViewTrackArrangleLeft)
	ON_COMMAND(ID_VIEW_TRACK_ARRANGLE_LEFTRIGHT, OnViewTrackArrangleLeftright)
	ON_COMMAND(ID_VIEW_TRACK_ARRANGLE_RIGHT, OnViewTrackArrangleRight)
	ON_COMMAND(ID_VIEW_TRACK_HORI_ARRANGLE, OnViewTrackHoriArrangle)
	ON_COMMAND(ID_VIEW_TRACK_HORI_AUTOARANGLE, OnViewTrackHoriAutoarangle)
	ON_COMMAND(ID_VIEW_TRACK_HORI_SETAUTOARRANGLE, OnViewTrackHoriSetautoarrangle)
	ON_COMMAND(ID_VIEW_TRACK_VERT_ALIGN, OnViewTrackVertAlign)
	ON_COMMAND(ID_VIEW_TRACK_VERT_SETAUTOALIGN, OnViewTrackVertSetautoalign)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRACK_ALIGN_CLIENT, OnUpdateViewTrackAlignClient)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRACK_ALIGN_DEPTH, OnUpdateViewTrackAlignDepth)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRACK_ARRANGLE_LEFT, OnUpdateViewTrackArrangleLeft)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRACK_ARRANGLE_LEFTRIGHT, OnUpdateViewTrackArrangleLeftright)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRACK_ARRANGLE_RIGHT, OnUpdateViewTrackArrangleRight)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRACK_HORI_ARRANGLE, OnUpdateViewTrackHoriArrangle)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRACK_HORI_AUTOARANGLE, OnUpdateViewTrackHoriAutoarangle)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRACK_VERT_ALIGN, OnUpdateViewTrackVertAlign)
	ON_COMMAND(ID_VIEW_TRACK_VERT_AUTOALIGN, OnViewTrackVertAutoalign)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRACK_VERT_AUTOALIGN, OnUpdateViewTrackVertAutoalign)
	ON_COMMAND(ID_VIEW_TRACK_ALIGN_TOP, OnViewTrackAlignTop)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRACK_ALIGN_TOP, OnUpdateViewTrackAlignTop)
	ON_COMMAND(ID_ADD_TRACK, OnAddTrack)
	ON_UPDATE_COMMAND_UI(ID_ADD_TRACK, OnUpdateAddTrack)
	ON_COMMAND(ID_DRAW_SELECT, OnDrawSelect)
	ON_WM_KILLFOCUS()
	ON_COMMAND(ID_OBJECT_PROPTERTIES, OnObjectPropterties)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_PROPTERTIES, OnUpdateObjectPropterties)
	ON_COMMAND(ID_TRACK_HORIFID_ALIGN_LEFT, OnTrackHoriFIDAlignLeft)
	ON_COMMAND(ID_TRACK_HORIFID_ALIGN_LEFTRIGHT, OnTrackHoriFIDAlignLeftright)
	ON_COMMAND(ID_TRACK_HORIFID_ALIGN_RIGHT, OnTrackHoriFIDAlignRight)
	ON_COMMAND(ID_TRACK_HORIFID_ARRANGLE_BOTTOM, OnTrackHoriFIDArrangleBottom)
	ON_COMMAND(ID_TRACK_HORIFID_ARRANGLE_LEFT, OnTrackHoriFIDArrangleLeft)
	ON_COMMAND(ID_TRACK_HORIFID_ARRANGLE_LEFTRIGHT, OnTrackHoriFIDArrangleLeftright)
	ON_COMMAND(ID_TRACK_HORIFID_ARRANGLE_RIGHT, OnTrackHoriFIDArrangleRight)
	ON_COMMAND(ID_TRACK_HORIFID_ARRANGLE_TOP, OnTrackHoriFIDArrangleTop)
	ON_COMMAND(ID_TRACK_HORIFID_ARRANGLE_TOPBOTTOM, OnTrackHoriFIDArrangleTopbottom)
	ON_COMMAND(ID_TRACK_HORIFID_SETHEIGHT, OnTrackHoriFIDSetheight)
	ON_COMMAND(ID_TRACK_HORIFID_SETWIDTH, OnTrackHoriFIDSetwidth)
	ON_COMMAND(ID_TRACK_HORIFID_ALIGN_TOP, OnTrackHoriFIDAlignTop)
	ON_COMMAND(ID_TRACK_HORIFID_ALIGN_BOTTOM, OnTrackHoriFIDAlignBottom)
	ON_COMMAND(ID_TRACK_HORIFID_ALIGN_TOPBOTTOM, OnTrackHoriFIDAlignTopbottom)
	ON_UPDATE_COMMAND_UI(ID_TRACK_HORIFID_ALIGN_BOTTOM, OnUpdateTrackHoriFIDAlignBottom)
	ON_UPDATE_COMMAND_UI(ID_TRACK_HORIFID_ALIGN_LEFT, OnUpdateTrackHoriFIDAlignLeft)
	ON_UPDATE_COMMAND_UI(ID_TRACK_HORIFID_ALIGN_LEFTRIGHT, OnUpdateTrackHoriFIDAlignLeftright)
	ON_UPDATE_COMMAND_UI(ID_TRACK_HORIFID_ALIGN_RIGHT, OnUpdateTrackHoriFIDAlignRight)
	ON_UPDATE_COMMAND_UI(ID_TRACK_HORIFID_ALIGN_TOP, OnUpdateTrackHoriFIDAlignTop)
	ON_UPDATE_COMMAND_UI(ID_TRACK_HORIFID_ALIGN_TOPBOTTOM, OnUpdateTrackHoriFIDAlignTopbottom)
	ON_UPDATE_COMMAND_UI(ID_TRACK_HORIFID_ARRANGLE_BOTTOM, OnUpdateTrackHoriFIDArrangleBottom)
	ON_UPDATE_COMMAND_UI(ID_TRACK_HORIFID_ARRANGLE_LEFT, OnUpdateTrackHoriFIDArrangleLeft)
	ON_UPDATE_COMMAND_UI(ID_TRACK_HORIFID_ARRANGLE_LEFTRIGHT, OnUpdateTrackHoriFIDArrangleLeftright)
	ON_UPDATE_COMMAND_UI(ID_TRACK_HORIFID_ARRANGLE_RIGHT, OnUpdateTrackHoriFIDArrangleRight)
	ON_UPDATE_COMMAND_UI(ID_TRACK_HORIFID_ARRANGLE_TOP, OnUpdateTrackHoriFIDArrangleTop)
	ON_UPDATE_COMMAND_UI(ID_TRACK_HORIFID_ARRANGLE_TOPBOTTOM, OnUpdateTrackHoriFIDArrangleTopbottom)
	ON_UPDATE_COMMAND_UI(ID_TRACK_HORIFID_SETHEIGHT, OnUpdateTrackHoriFIDSetheight)
	ON_UPDATE_COMMAND_UI(ID_TRACK_HORIFID_SETWIDTH, OnUpdateTrackHoriFIDSetwidth)
	ON_COMMAND(ID_TRACK_AUTOARRANGLE, OnTrackAutoarrangle)
	ON_COMMAND(ID_TRACK_ARRANGLE, OnTrackArrangle)
	ON_UPDATE_COMMAND_UI(ID_TRACK_ARRANGLE, OnUpdateTrackArrangle)
	ON_UPDATE_COMMAND_UI(ID_TRACK_AUTOARRANGLE, OnUpdateTrackAutoarrangle)
	ON_COMMAND(ID_TRACK_VERTFID_ALIGN_TOP, OnTrackVertFIDAlignTop)
	ON_COMMAND(ID_TRACK_VERTFID_ALIGN_TOPBOTTOM, OnTrackVertFIDAlignTopbottom)
	ON_COMMAND(ID_TRACK_VERTFID_ALIGN_BOTTOM, OnTrackVertFIDAlignBottom)
	ON_COMMAND(ID_TRACK_VERTFID_ARRANGLE_LEFT, OnTrackVertFIDArrangleLeft)
	ON_COMMAND(ID_TRACK_VERTFID_ARRANGLE_LEFTRIGHT, OnTrackVertFIDArrangleLeftright)
	ON_COMMAND(ID_TRACK_VERTFID_ARRANGLE_RIGHT, OnTrackVertFIDArrangleRight)
	ON_UPDATE_COMMAND_UI(ID_TRACK_VERTFID_ALIGN_TOP, OnUpdateTrackVertFIDAlignTop)
	ON_UPDATE_COMMAND_UI(ID_TRACK_VERTFID_ALIGN_TOPBOTTOM, OnUpdateTrackVertFIDAlignTopbottom)
	ON_UPDATE_COMMAND_UI(ID_TRACK_VERTFID_ALIGN_BOTTOM, OnUpdateTrackVertFIDAlignBottom)
	ON_UPDATE_COMMAND_UI(ID_TRACK_VERTFID_ARRANGLE_LEFT, OnUpdateTrackVertFIDArrangleLeft)
	ON_UPDATE_COMMAND_UI(ID_TRACK_VERTFID_ARRANGLE_LEFTRIGHT, OnUpdateTrackVertFIDArrangleLeftright)
	ON_UPDATE_COMMAND_UI(ID_TRACK_VERTFID_ARRANGLE_RIGHT, OnUpdateTrackVertFIDArrangleRight)
	ON_COMMAND(ID_TRACK_VERTFID_SETHEIGHT, OnTrackVertFIDSetheight)
	ON_UPDATE_COMMAND_UI(ID_TRACK_VERTFID_SETHEIGHT, OnUpdateTrackVertFIDSetheight)
	ON_COMMAND(ID_TRACK_VERTFID_SETWIDTH, OnTrackVertFIDSetwidth)
	ON_UPDATE_COMMAND_UI(ID_TRACK_VERTFID_SETWIDTH, OnUpdateTrackVertFIDSetwidth)
	ON_COMMAND(ID_TRACK_TRACK_ARRANGLE_LEFT, OnTrackTrackArrangleLeft)
	ON_COMMAND(ID_TRACK_TRACK_ARRANGLE_LEFTRIGHT, OnTrackTrackArrangleLeftright)
	ON_COMMAND(ID_TRACK_TRACK_ARRANGLE_RIGHT, OnTrackTrackArrangleRight)
	ON_UPDATE_COMMAND_UI(ID_TRACK_TRACK_ARRANGLE_LEFT, OnUpdateTrackTrackArrangleLeft)
	ON_UPDATE_COMMAND_UI(ID_TRACK_TRACK_ARRANGLE_LEFTRIGHT, OnUpdateTrackTrackArrangleLeftright)
	ON_UPDATE_COMMAND_UI(ID_TRACK_TRACK_ARRANGLE_RIGHT, OnUpdateTrackTrackArrangleRight)
	ON_COMMAND(ID_TRACK_HORIFID_HORIARRANGLE_NONE, OnTrackHoriFIDHoriarrangleNone)
	ON_UPDATE_COMMAND_UI(ID_TRACK_HORIFID_HORIARRANGLE_NONE, OnUpdateTrackHoriFIDHoriarrangleNone)
	ON_COMMAND(ID_TRACK_HORIFID_HORIALIGN_NONE, OnTrackHoriFIDHorialignNone)
	ON_UPDATE_COMMAND_UI(ID_TRACK_HORIFID_HORIALIGN_NONE, OnUpdateTrackHoriFIDHorialignNone)
	ON_COMMAND(ID_TRACK_HORIFID_VERTALIGN_NONE, OnTrackHoriFIDVertalignNone)
	ON_UPDATE_COMMAND_UI(ID_TRACK_HORIFID_VERTALIGN_NONE, OnUpdateTrackHoriFIDVertalignNone)
	ON_COMMAND(ID_TRACK_HORIFID_VERTARRANGLE_NONE, OnTrackHoriFIDVertarrangleNone)
	ON_UPDATE_COMMAND_UI(ID_TRACK_HORIFID_VERTARRANGLE_NONE, OnUpdateTrackHoriFIDVertarrangleNone)
	ON_COMMAND(ID_TRACK_VERTFID_ARRANGLE_NONE, OnTrackVertFIDArrangleNone)
	ON_UPDATE_COMMAND_UI(ID_TRACK_VERTFID_ARRANGLE_NONE, OnUpdateTrackVertFIDArrangleNone)
	ON_COMMAND(ID_TRACK_VERTFID_ALIGN_NONE, OnTrackVertFIDAlignNone)
	ON_UPDATE_COMMAND_UI(ID_TRACK_VERTFID_ALIGN_NONE, OnUpdateTrackVertFIDAlignNone)
	ON_COMMAND(ID_TRACK_TRACK_ARRANGLE_NONE, OnTrackTrackArrangleNone)
	ON_UPDATE_COMMAND_UI(ID_TRACK_TRACK_ARRANGLE_NONE, OnUpdateTrackTrackArrangleNone)
	ON_COMMAND(ID_TRACK_SETAUTOARRANGLE, OnTrackSetautoarrangle)
	ON_UPDATE_COMMAND_UI(ID_TRACK_SETAUTOARRANGLE, OnUpdateTrackSetautoarrangle)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_VIEW_TRACK_SETWIDTH, OnViewTrackSetwidth)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRACK_SETWIDTH, OnUpdateViewTrackSetwidth)
	ON_COMMAND(ID_VIEW_TRACK_TITLE_SHOW, OnViewTrackTitleShow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRACK_TITLE_SHOW, OnUpdateViewTrackTitleShow)
	ON_COMMAND(ID_VIEW_TRACK_HEAD_SETHEIGHT_AUTO, OnViewTrackHeadSetheightAuto)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRACK_HEAD_SETHEIGHT_AUTO, OnUpdateViewTrackHeadSetheightAuto)
	ON_COMMAND(ID_VIEW_TRACK_HEAD_SETHEIGHT_MAX, OnViewTrackHeadSetheightMax)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRACK_HEAD_SETHEIGHT_MAX, OnUpdateViewTrackHeadSetheightMax)
	ON_COMMAND(ID_VIEW_TRACK_TITLE_SETHEIGHT, OnViewTrackTitleSetheight)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TRACK_TITLE_SETHEIGHT, OnUpdateViewTrackTitleSetheight)
	ON_COMMAND(ID_TRACK_TRACK_SETWIDTH, OnTrackTrackSetwidth)
	ON_UPDATE_COMMAND_UI(ID_TRACK_TRACK_SETWIDTH, OnUpdateTrackTrackSetwidth)
	ON_COMMAND(ID_TRACK_TRACK_TITLE_SETHEIGHT, OnTrackTrackTitleSetheight)
	ON_UPDATE_COMMAND_UI(ID_TRACK_TRACK_TITLE_SETHEIGHT, OnUpdateTrackTrackTitleSetheight)
	ON_COMMAND(ID_TRACK_TRACK_TITLE_SHOW, OnTrackTrackTitleShow)
	ON_UPDATE_COMMAND_UI(ID_TRACK_TRACK_TITLE_SHOW, OnUpdateTrackTrackTitleShow)
	ON_COMMAND(ID_TRACK_TRCAK_TITLE_HIDE, OnTrackTrcakTitleHide)
	ON_UPDATE_COMMAND_UI(ID_TRACK_TRCAK_TITLE_HIDE, OnUpdateTrackTrcakTitleHide)
	ON_COMMAND(ID_TRACK_TRACK_HEAD_SETHEIGHT_AUTO, OnTrackTrackHeadSetheightAuto)
	ON_UPDATE_COMMAND_UI(ID_TRACK_TRACK_HEAD_SETHEIGHT_AUTO, OnUpdateTrackTrackHeadSetheightAuto)
	ON_COMMAND(ID_TRACK_TRACK_HEAD_SETHEIGHT_MAX, OnTrackTrackHeadSetheightMax)
	ON_UPDATE_COMMAND_UI(ID_TRACK_TRACK_HEAD_SETHEIGHT_MAX, OnUpdateTrackTrackHeadSetheightMax)
	ON_COMMAND(ID_PRE_RANGVALUE, OnPreRangvalue)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_INSERTPOINT, OnInsertpoint)
	ON_UPDATE_COMMAND_UI(ID_INSERTPOINT, OnUpdateInsertpoint)
	ON_COMMAND(ID_CURVE_MOUSEEDIT, OnCurveMouseedit)
	ON_COMMAND(ID_CURVE_DISVALUE, OnCurveDisvalue)
	ON_COMMAND(IDD_CURVE_MOVEDEP, OnCurveMovedep)
	ON_UPDATE_COMMAND_UI(IDD_CURVE_MOVEDEP, OnUpdateCurveMovedep)
	ON_COMMAND(IDD_CURVE_PLUSMOUSE, OnCurvePlusmouse)
	ON_UPDATE_COMMAND_UI(IDD_CURVE_PLUSMOUSE, OnUpdateCurvePlusmouse)
	ON_COMMAND(IDD_MODFY_DEPSEGMENT, OnModfyDepsegment)
	ON_UPDATE_COMMAND_UI(ID_PRE_RANGVALUE, OnUpdatePreRangvalue)
	ON_UPDATE_COMMAND_UI(ID_CURVE_MOUSEEDIT, OnUpdateCurveMouseedit)
	ON_COMMAND(ID_CURVE_COUNTERGLOW, OnCurveCounterglow)
	ON_UPDATE_COMMAND_UI(ID_CURVE_COUNTERGLOW, OnUpdateCurveCounterglow)
	ON_COMMAND(IDD_MultiSectDep, OnMultiSectDep)
	ON_UPDATE_COMMAND_UI(IDD_MultiSectDep, OnUpdateMultiSectDep)
	ON_COMMAND(ID_VALUE_SENDTO_CLIPBOARD, OnValueSendtoClipboard)
	ON_UPDATE_COMMAND_UI(ID_VALUE_SENDTO_CLIPBOARD, OnUpdateValueSendtoClipboard)
	ON_COMMAND(ID_ADD_IMAGE, OnAddImage)
	ON_COMMAND(ID_ADD_IMAGE3D, OnAddImage3d)
	ON_COMMAND(ID_ADD_STRATUM, OnAddStratum)
	ON_COMMAND(ID_MARK_STRATUM, OnMarkStratum)
	ON_COMMAND(ID_MARK_CCL, OnMarkCCL)
	ON_UPDATE_COMMAND_UI(ID_MARK_STRATUM, OnUpdateMarkStratum)
	ON_UPDATE_COMMAND_UI(ID_MARK_CCL, OnUpdateMarkCCL)
	ON_COMMAND(ID_MARK_CONFRACTURE, OnMarkConfracture)
	ON_UPDATE_COMMAND_UI(ID_MARK_CONFRACTURE, OnUpdateMarkConfracture)
	ON_COMMAND(ID_MARK_RESFRACTURE, OnMarkResfracture)
	ON_UPDATE_COMMAND_UI(ID_MARK_RESFRACTURE, OnUpdateMarkResfracture)
	ON_COMMAND(ID_MARK_AVALANCHE, OnMarkAvalanche)
	ON_UPDATE_COMMAND_UI(ID_MARK_AVALANCHE, OnUpdateMarkAvalanche)
	ON_COMMAND(ID_ADD_MARKVECTOR, OnAddMarkvector)
	ON_COMMAND(ID_MARK_RESFRACTURE2, OnMarkResfracture2)
	ON_UPDATE_COMMAND_UI(ID_MARK_RESFRACTURE2, OnUpdateMarkResfracture2)
	ON_COMMAND(ID_ADD_MARKFREQ, OnAddMarkfreq)
	ON_COMMAND(ID_ADD_IMAGECURVE, OnAddImagecurve)
	ON_COMMAND(ID_EDDY_PLAY, OnEddyPlay)
	ON_UPDATE_COMMAND_UI(ID_EDDY_PLAY, OnUpdateEddyPlay)
	ON_WM_TIMER()
	ON_COMMAND(ID_PLAY_STOP, OnPlayStop)
	ON_UPDATE_COMMAND_UI(ID_PLAY_STOP, OnUpdatePlayStop)
	ON_COMMAND(ID_EDDY_RIGHT, OnEddyRight)
	ON_UPDATE_COMMAND_UI(ID_EDDY_RIGHT, OnUpdateEddyRight)
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_ADD_ECHO, OnAddEcho)
	ON_COMMAND(ID_FREQUENCY, OnFrequency)
	ON_UPDATE_COMMAND_UI(ID_FREQUENCY, OnUpdateFrequency)
	ON_COMMAND(ID_ADD_POLE, OnAddPole)
	ON_UPDATE_COMMAND_UI(ID_CURVE_DISVALUE, OnUpdateCurveDisvalue)
	ON_COMMAND(ID_HOLE_ANALYSIS, OnHoleAnalysis)
	ON_UPDATE_COMMAND_UI(ID_HOLE_ANALYSIS, OnUpdateHoleAnalysis)
	ON_COMMAND(ID_IMAGE_ANALYSIS, OnImageAnalysis)

	ON_COMMAND(IDD_CURVE_DEPICO, OnCurveDepico)
	ON_UPDATE_COMMAND_UI(IDD_CURVE_DEPICO, OnUpdateCurveDepico)
	ON_COMMAND(ID_COMPUTE_DIP, OnComputeDip)
	ON_COMMAND(ID_DIP_PARA, OnDipPara)
	ON_COMMAND(ID_DEL_HANDWORK, OnDelHandwork)
	ON_UPDATE_COMMAND_UI(ID_COMPUTE_DIP, OnUpdateComputeDip)
	ON_COMMAND(ID_MODIFY_HANDWORK, OnModifyHandwork)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_HANDWORK, OnUpdateModifyHandwork)
	ON_UPDATE_COMMAND_UI(ID_DEL_HANDWORK, OnUpdateDelHandwork)
	ON_COMMAND(ID_MARK_LAYERFRACTURE, OnMarkLayerfracture)
	ON_UPDATE_COMMAND_UI(ID_MARK_LAYERFRACTURE, OnUpdateMarkLayerfracture)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_MARK_HOLE, OnMarkHole)
	ON_UPDATE_COMMAND_UI(ID_MARK_HOLE, OnUpdateMarkHole)
	ON_COMMAND(ID_PARTCOPY, OnPartCopy)
	ON_UPDATE_COMMAND_UI(ID_PARTCOPY, OnUpdatePartCopy)

	ON_COMMAND(ID_PART_PASTE, OnPartPaste)
	ON_UPDATE_COMMAND_UI(ID_PART_PASTE, OnUpdatePartPaste)

	ON_COMMAND(IDD_CURVE_PLUS, OnCurvePlus)
	ON_UPDATE_COMMAND_UI(IDD_CURVE_PLUS, OnUpdateCurvePlus)
	ON_COMMAND(IDD_CURVE_PART_FILTER, OnCurvePartFilter)
	ON_UPDATE_COMMAND_UI(IDD_CURVE_PART_FILTER, OnUpdateCurvePartFilter)
	//}}AFX_MSG_MAP
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
	ON_MESSAGE(WM_ENDCLICK, OnEndClick)
	ON_MESSAGE(WM_CANCELCLICK,OnCancelClick)
	ON_COMMAND(ID_BMP_SENTTOCILP, OnBmpSenttoCilp)

	ON_COMMAND(ID_DRAWCONNECTION,OnDrawConnection)
	ON_UPDATE_COMMAND_UI(ID_DRAWCONNECTION, OnUpdateDrawConnection)
	ON_COMMAND(ID_COUNT_CONTOUR,OnCreateContour)
	ON_UPDATE_COMMAND_UI(ID_COUNT_CONTOUR, OnUpdateCreateContour)
	ON_COMMAND(ID_DEL_SELHOLE, OnDelSelhole)
	ON_UPDATE_COMMAND_UI(ID_DEL_SELHOLE, OnUpdateDelSelhole)
	ON_COMMAND(ID_DEL_SMALL_SEL, OnDelSmallSel)
	ON_UPDATE_COMMAND_UI(ID_DEL_SMALL_SEL, OnUpdateDelSmallSel)
	ON_COMMAND(ID_DEL_BIG_SEL, OnDelBigSel)
	ON_UPDATE_COMMAND_UI(ID_DEL_BIG_SEL, OnUpdateDelBigSel)
	ON_COMMAND(ID_SET_CONTOUR_PARA, OnSetContourPara)

	ON_COMMAND(ID_DEL_RECTANGLE, OnDelRectangle)
	ON_UPDATE_COMMAND_UI(ID_DEL_RECTANGLE, OnUpdateDelRectangle)
	ON_COMMAND(ID_GET_RECTANGL_CONTOUR, OnGetRectanglContour)
	ON_UPDATE_COMMAND_UI(ID_GET_RECTANGL_CONTOUR, OnUpdateGetRectanglContour)
	ON_COMMAND(ID_GET_ELLIPSE_CONTOUR, OnGetEllipseContour)
	ON_UPDATE_COMMAND_UI(ID_GET_ELLIPSE_CONTOUR, OnUpdateGetEllipseContour)

	ON_COMMAND(ID_OPEN_CONTOUR, OnOpenContour)
	ON_UPDATE_COMMAND_UI(ID_OPEN_CONTOUR, OnUpdateOpenContour)
	ON_COMMAND(ID_SAVE_CONTOUR, OnSaveContour)
	ON_UPDATE_COMMAND_UI(ID_SAVE_CONTOUR, OnUpdateSaveContour)
	ON_COMMAND(ID_SAVEAS_CONTOUR, OnSaveasContour)
	ON_UPDATE_COMMAND_UI(ID_SAVEAS_CONTOUR, OnUpdateSaveasContour)
	ON_COMMAND(ID_PICKUP_STAT, OnPickupStat)
	ON_UPDATE_COMMAND_UI(ID_PICKUP_STAT, OnUpdatePickupStat)
	ON_COMMAND(ID_PICKUP_LIST, OnPickupList)
	ON_UPDATE_COMMAND_UI(ID_PICKUP_LIST, OnUpdatePickupList)
	ON_COMMAND(ID_SIN_CONTOUR, OnCreateSinContour)
	ON_UPDATE_COMMAND_UI(ID_SIN_CONTOUR, OnUpdateCreateSinContour)
	ON_COMMAND(ID_PICKUP_SINSTAT, OnPickupSinStat)
	ON_UPDATE_COMMAND_UI(ID_PICKUP_SINSTAT, OnUpdatePickupSinStat)
	ON_COMMAND(ID_PICKUP_SINLIST, OnReCrackList)
	ON_UPDATE_COMMAND_UI(ID_PICKUP_SINLIST, OnUpdateReCrackList)
	ON_COMMAND(ID_CREATE_SINLIST, OnCrackList)
	ON_UPDATE_COMMAND_UI(ID_CREATE_SINLIST, OnUpdateCrackList)
	ON_COMMAND(ID_SETHOLE_STATPARA, OnSetholeStatPara)
	ON_COMMAND(ID_SETCRACK_STATPARA, OnSetCrackStatPara)

	ON_COMMAND(ID_DEL_ALLHOLE, OnDelAllhole)
	ON_UPDATE_COMMAND_UI(ID_DEL_ALLHOLE, OnUpdateDelAllhole)
	ON_COMMAND(ID_PERSISTRESULT, OnPersistResult)
	ON_UPDATE_COMMAND_UI(ID_PERSISTRESULT, OnUpdatePersistresult)
	ON_COMMAND(ID_CREATE_FULLIMAGE, OnCreateFullImage)
	ON_UPDATE_COMMAND_UI(ID_CREATE_FULLIMAGE, OnUpdateCreateFullImage)
	ON_COMMAND(ID_CREATE_IMAGE3D, OnCreateImage3d)
	ON_UPDATE_COMMAND_UI(ID_CREATE_IMAGE3D, OnUpdateCreateImage3d)
	ON_COMMAND(ID_POR_COMPUTE, OnPorCompute)
	ON_UPDATE_COMMAND_UI(ID_POR_COMPUTE, OnUpdatePorCompute)
	ON_COMMAND(ID_RMF_SCALE, OnRmfScale)
	ON_UPDATE_COMMAND_UI(ID_RMF_SCALE, OnUpdateRmfScale)
	ON_COMMAND(ID_DRAW_FREQUENCY, OnDrawFrequency)
	ON_UPDATE_COMMAND_UI(ID_DRAW_FREQUENCY, OnUpdateDrawFrequency)
	ON_COMMAND(ID_DRAW_CROSS, OnDrawCross)
	ON_UPDATE_COMMAND_UI(ID_DRAW_CROSS, OnUpdateDrawCross)
	ON_COMMAND(ID_CREATE_HPOR, OnCreateHpor)
	ON_UPDATE_COMMAND_UI(ID_CREATE_HPOR, OnUpdateCreateHpor)
	ON_COMMAND(ID_DRAW_DIAMETER, OnDrawDiameter)
	ON_UPDATE_COMMAND_UI(ID_DRAW_DIAMETER, OnUpdateDrawDiameter)
	ON_COMMAND(ID_DRAW_POR, OnDrawPor)
	ON_UPDATE_COMMAND_UI(ID_DRAW_POR, OnUpdateDrawPor)
	ON_COMMAND(ID_CREATEIMAGELIST, OnCreateimagelist)
	ON_UPDATE_COMMAND_UI(ID_CREATEIMAGELIST, OnUpdateCreateimagelist)

	END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CImageManView drawing

BOOL CImageManView::PreTranslateMessage(MSG* pMsg)
{
	m_tooltip.RelayEvent(pMsg);
	return CView::PreTranslateMessage(pMsg);
}

void CImageManView::DeleteContents()
{
	m_pSelectObj = NULL;
	bObjectLink = FALSE;
	m_LeftSpace = m_TopSpace = 10;
}

void CImageManView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	c_ToolPro=NoneTool;
	CRect rect(100,100,150,150);
	m_tooltip.Create(this);
	m_tooltip.AddTool(this,"0.1234",rect,ID_CURVE_VALUE);
	m_tooltip.Activate(FALSE);
	CSize sizeTotal(100,100);
	pDoc = (CImageManDoc*)GetParentFrame()->GetActiveDocument();
	CClientDC dc(this);
	if(pDoc->m_ScaleNo != 0)
	{
		if (pDoc->m_ScaleFactor < 0)
		{
			sizeTotal.cx = MulDiv(pDoc->m_Rect.Width()+2*m_LeftSpace,dc.GetDeviceCaps(LOGPIXELSX),-pDoc->m_ScaleFactor);
			sizeTotal.cy = MulDiv(pDoc->m_Rect.Height()+2*m_TopSpace,dc.GetDeviceCaps(LOGPIXELSY),-pDoc->m_ScaleFactor);
		}
		else
		{
			sizeTotal.cx = MulDiv(pDoc->m_Rect.Width()+2*m_LeftSpace,dc.GetDeviceCaps(LOGPIXELSX)*pDoc->m_ScaleFactor,10000);
			sizeTotal.cy = MulDiv(pDoc->m_Rect.Height()+2*m_TopSpace,dc.GetDeviceCaps(LOGPIXELSY)*pDoc->m_ScaleFactor,10000);
		}
	}
	else
	{
		m_LeftSpace = 10;
		m_TopSpace = 10;
		sizeTotal.cx = rect.right;
		sizeTotal.cy = MulDiv(pDoc->m_Rect.Height()+2*m_TopSpace,rect.right,pDoc->m_Rect.Width()+2*m_LeftSpace);
	}
	SetScrollSizes(MM_TEXT,sizeTotal);
	if(pDoc->m_CmdTemplateName.Find("CIFImage")>0)
	{
		m_ImageMax=256.f;
		m_ImageMin=110.f;
	}
}

BOOL CImageManView::PreCreateWindow(CREATESTRUCT& cs)
{
	ASSERT(cs.style & WS_CHILD);
	if (cs.lpszClass == NULL)
		cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS);
	return TRUE;
}

void CImageManView::OnActivateView(BOOL bActivate, CView* pActiveView,
	CView* pDeactiveView)
{
	CView::OnActivateView(bActivate, pActiveView, pDeactiveView);

	// invalidate selections when active status changes
	if (m_bActive != bActivate)
	{
		if (bActivate)  // if becoming active update as if active
			m_bActive = bActivate;
		if ((m_pSelectObj != NULL) && (CDrawObj::c_ToolShape == foldtool))
		{
			CPoint point;
		    GetCursorPos(&point);
			ScreenToClient(&point);
			CClientDC dc(this);
			OnPrepareDC(&dc,NULL);
			dc.DPtoLP(&point);
			pDoc->PPtoLP(&point);
			m_pSelectObj->OnLButtonDblClk(this,0,point);
		}
		DeSelectObj();
		m_bActive = bActivate;
	}
}

void CImageManView::OnUpdate(CView* , LPARAM lHint, CObject* pHint)
{
	if(m_bFirstUpdate) 
		lHint = HINT_UPDATE_SIZE; 
	switch (lHint)
	{
	case HINT_UPDATE_WINDOW:    // redraw entire window
		Invalidate(FALSE);
		break;
	case HINT_UPDATE_SIZE:
        {
			if (pDoc == NULL) return;
			m_bFirstUpdate = FALSE;
			CSize sizeTotal;
			CClientDC dc(this);
			CRect rect;
			GetClientRect(rect);
			if (pDoc->m_ScaleNo != 0)
			{
				if (pDoc->m_ScaleFactor < 0)
				{
					int cx = MulDiv(rect.right,(-pDoc->m_ScaleFactor),dc.GetDeviceCaps(LOGPIXELSX));
					int nLeftSpace = (cx - pDoc->m_Rect.Width()) / 2;
					if (nLeftSpace < 10) nLeftSpace = 10;
					if (m_LeftSpace != nLeftSpace) m_LeftSpace = nLeftSpace;
					sizeTotal.cx = MulDiv(pDoc->m_Rect.Width()+2*m_LeftSpace,dc.GetDeviceCaps(LOGPIXELSX),-pDoc->m_ScaleFactor);
					sizeTotal.cy = MulDiv(pDoc->m_Rect.Height()+2*m_TopSpace,dc.GetDeviceCaps(LOGPIXELSY),-pDoc->m_ScaleFactor);
				}
				else
				{
					int cx = MulDiv(rect.right,100*100,dc.GetDeviceCaps(LOGPIXELSX)*pDoc->m_ScaleFactor);
					int nLeftSpace = (cx - pDoc->m_Rect.Width()) / 2;
					if (nLeftSpace < 10) nLeftSpace = 10;
					if (m_LeftSpace != nLeftSpace) m_LeftSpace = nLeftSpace;
					sizeTotal.cx = MulDiv(pDoc->m_Rect.Width()+2*m_LeftSpace,dc.GetDeviceCaps(LOGPIXELSX)*pDoc->m_ScaleFactor,10000);
					sizeTotal.cy = MulDiv(pDoc->m_Rect.Height()+2*m_TopSpace,dc.GetDeviceCaps(LOGPIXELSY)*pDoc->m_ScaleFactor,10000);
				}
			}
			else
			{
				m_LeftSpace = 10;
				m_TopSpace = 10;
				sizeTotal.cx = rect.right;
				sizeTotal.cy = MulDiv(pDoc->m_Rect.Height()+2*m_TopSpace,rect.right,pDoc->m_Rect.Width()+2*m_LeftSpace);
			}
			SetScrollSizes(MM_TEXT, sizeTotal);
			Invalidate(FALSE);
		}
        break;
	case HINT_UPDATE_DRAWOBJ:   // a single object has changed
		((CDrawObj*)pHint)->InvalObj(this);
		break;
	case HINT_UPDATE_HEADOBJ:   // a single object has changed
		break;
	default:
		ASSERT(FALSE);
		break;
	}	
}

void CImageManView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	CScrollView::OnPrepareDC(pDC, pInfo);
	CPoint point;
	pDC->SetMapMode(MM_ANISOTROPIC);         
	point.x = pDC->GetDeviceCaps(LOGPIXELSX);
	point.y = pDC->GetDeviceCaps(LOGPIXELSY);
	CPoint point1;
	if((pInfo==NULL)&&(pDoc->m_ScaleNo==7))
	{
		point1.x=96;
		point1.y=96;
	}
	else
	{
		point1.x=100;
		point1.y=100;
	}
	if((pInfo==NULL)&&(pDoc->m_ScaleNo != 0))
	{
		if (pDoc->m_ScaleFactor < 0) 
		{
			point1.x = -pDoc->m_ScaleFactor;
			point1.y = -pDoc->m_ScaleFactor;
		}
		else
		{
			point.x = point.x * pDoc->m_ScaleFactor / 100;
			point.y = point.y * pDoc->m_ScaleFactor / 100;
		}
	}
	else
	{
		CRect rect;
		GetClientRect(rect);
		point.x = point.y = rect.right;
		point1.x = point1.y = pDoc->m_Rect.Width() + 2*m_LeftSpace;
	}
	pDC->SetViewportExt(point.x,point.y),
	pDC->SetWindowExt(point1.x, point1.y);              
	// Windows限制设备坐标最大值为32768,使用逻辑坐标原点偏移.
	// 以50英寸(5000个逻辑单位)为页单位实现偏移;
	if(pInfo==NULL)
	{
			CSize m_Page;
			if (point.x > point1.x)
			{
				m_Page.cx = 32600 / point.x;
				m_Page.cy = 32600 / point.y;
			}
			else
			{
				m_Page.cx = 32600 / point1.x;
				m_Page.cy = 32600 / point1.y;
			}
			CSize m_WinPage(m_Page.cx * point.x, m_Page.cy * point.y);
			pDoc->m_WinPage.cx = m_Page.cx * point1.x;
			pDoc->m_WinPage.cy = m_Page.cy * point1.y;
			CPoint org;
			org.x = GetScrollPos(SB_HORZ);
			org.y = GetScrollPos(SB_VERT);
			int k = (int)(1.0 * org.x / m_WinPage.cx + 0.5);
			pDoc->m_WinOrg.x = pDoc->m_WinPage.cx * k;
			org.x = org.x - k * m_WinPage.cx;
			k = (int)(1.0 * org.y / m_WinPage.cy + 0.5); 
			pDoc->m_WinOrg.y = pDoc->m_WinPage.cy * k;
			org.y = org.y - k * m_WinPage.cy;
			pDC->SetViewportOrg(-org.x,-org.y);
 		    CSize size(m_LeftSpace,m_TopSpace);
		    pDC->LPtoDP(&size);
		    pDC->OffsetViewportOrg(size.cx,size.cy);
	}
	else
	{
			int col=pInfo->m_nCurPage/pDoc->m_NumRows+(pInfo->m_nCurPage%pDoc->m_NumRows>0);
			int row=(pInfo->m_nCurPage-1)%pDoc->m_NumRows+1;
			CSize m_Page;
			int offs;
			if(point.x>point1.x)
			{
				m_Page.cx=32600/point.x;
				m_Page.cy=32600/point.y;
				offs=3200;
			}
			else
			{
				m_Page.cx=32600/point1.x;
				m_Page.cy=32600/point1.y;
				offs=3200*point.x/point1.x;
			}
			CSize m_WinPage(m_Page.cx*point.x,m_Page.cy*point.y);
			pDoc->m_WinPage.cx=m_Page.cx*point1.x;
			pDoc->m_WinPage.cy=m_Page.cy*point1.y;

			CPoint org(pDoc->m_PageWidth*(col-1),pDoc->m_PageHeight*(row-1));
			pDoc->m_WinOrg.x=0;
			pDoc->m_WinOrg.y=(org.y+offs)*point1.x/point.x;
			pDC->SetViewportOrg(-org.x,-org.y);
	}
}

void CImageManView::OnDraw(CDC* pDC)
{     
	if(pDoc == NULL) 
		return;
	if(pDC->IsPrinting())
	{
		pDoc->Print(pDC,this);
		return;
	}
	CDC m_MemDC;
	CDC* pDrawDC = pDC;
	CBitmap bitmap;
	CBitmap* pOldBitmap;
	
	CRect rect,m_ClipRect;
    pDC->GetClipBox(&rect);        
	rect.InflateRect(5,5);
	m_ClipRect = rect;
    pDC->LPtoDP(&rect);

	// draw to offscreen bitmap for fast looking repaints
	if(m_MemDC.CreateCompatibleDC(pDC))
	{
		if(bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height()))
		{
			OnPrepareDC(&m_MemDC, NULL);
			pDrawDC = &m_MemDC;
            m_MemDC.OffsetViewportOrg(-rect.left, -rect.top);
			pOldBitmap = m_MemDC.SelectObject(&bitmap);
            m_MemDC.SetBrushOrg(rect.left % 8, rect.top % 8);
            m_MemDC.IntersectClipRect(m_ClipRect);
		}
	}
    CBrush brush;
	if (!brush.CreateSolidBrush(0xC0C0C0)) return;
	brush.UnrealizeObject();     
	pDrawDC->FillRect(m_ClipRect , &brush);	
    brush.DeleteObject();
	if (!brush.CreateSolidBrush(pDoc->m_paperColor)) return;
	brush.UnrealizeObject();     
	CRect m_Rect = pDoc->GetRect();
	pDoc->LPtoPP(m_Rect);
	pDrawDC->FillRect(m_ClipRect & m_Rect , &brush);

	CPen m_Pen;
    m_Pen.CreatePen(PS_SOLID,1,RGB(0,0,0));
   	CPen *pOldPen = pDrawDC->SelectObject(&m_Pen);
    CBrush* pOldBrush = (CBrush*)pDrawDC->SelectStockObject(NULL_BRUSH);
	pDrawDC->Rectangle(m_Rect);           
	pDrawDC->SelectObject(pOldPen);
	m_Pen.DeleteObject();                              
	m_Pen.CreatePen(PS_SOLID,2,RGB(0,0,0));
   	pDrawDC->SelectObject(&m_Pen);
   	pDrawDC->MoveTo(m_Rect.left+3,m_Rect.bottom+1);
   	pDrawDC->LineTo(m_Rect.right,m_Rect.bottom+1);
   	pDrawDC->MoveTo(m_Rect.right+1,m_Rect.top+3);
   	pDrawDC->LineTo(m_Rect.right+1,m_Rect.bottom+1);

	m_Pen.DeleteObject();                              
	m_Pen.CreatePen(PS_DOT,1,RGB(0,0,0));
   	pDrawDC->SelectObject(&m_Pen);
	CRect rect1 = pDoc->m_TrackRect;
	pDoc->LPtoPP(rect1);
	rect1.InflateRect(4,4);
	pDrawDC->Rectangle(rect1);           
	pDrawDC->SelectObject(pOldPen);
	pDrawDC->SelectObject(pOldBrush);

	m_ClipRect &= m_Rect;
	pDrawDC->IntersectClipRect(m_ClipRect);
	pDoc->Draw(this,pDrawDC);
	//绘拾取的图像边沿特征
	DrawPickupContour(pDrawDC);

	if (m_pSelectObj != NULL)
	{
		m_pSelectObj->Draw(this,pDrawDC);
		if ((CDrawObj::m_SelectMode != size) && (CDrawObj::m_SelectMode != move))
			m_pSelectObj->DrawTracker(this,pDrawDC,selected);
	}
	if(pDrawDC != pDC)
	{
		pDC->SetViewportOrg(0, 0);
		pDC->SetWindowOrg(0,0);
		pDC->SetMapMode(MM_TEXT);
		m_MemDC.SetViewportOrg(0, 0);
		m_MemDC.SetWindowOrg(0,0);
		m_MemDC.SetMapMode(MM_TEXT);
		pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),
			&m_MemDC, 0, 0, SRCCOPY);
		m_MemDC.SelectObject(pOldBitmap);
	}
	//绘纵向标尽读取曲线平均值
	if(m_bDisFIDValue)
	{
		OnPrepareDC(pDC,NULL);
		CRect cRect;
		GetClientRect(cRect);
		pDC->DPtoLP (&cRect);
		int DrawMode = pDC->SetROP2(R2_XORPEN);
		CPen AxialPen;
		AxialPen.CreatePen(PS_SOLID, 1,0xffff00);
		CPen *OldPen = (CPen *)pDC->SelectObject(&AxialPen);
		pDC->MoveTo (m_iDisFIDPreX,cRect.top );
		pDC->LineTo (m_iDisFIDPreX,cRect.bottom );
		pDC->SetROP2( DrawMode );
		pDC->SelectObject( OldPen );
	}
	//绘纵向标尽读取某深度点所有曲线数据
	if(m_bCounterGlow)
	{
		OnPrepareDC(pDC,NULL);
		CRect cRect;
		GetClientRect(cRect);
		pDC->DPtoLP (&cRect);
		int DrawMode = pDC->SetROP2(R2_XORPEN);
		CPen AxialPen;
		AxialPen.CreatePen(PS_SOLID, 1,0xffff00);
		CPen *OldPen = (CPen *)pDC->SelectObject(&AxialPen);
		CPoint pt1,pt2;
		pt1.x=cRect.left;
		pt1.y=m_iCounterPreX;
		pt2.x=cRect.right;
		pt2.y=m_iCounterPreX;
		pDoc->LPtoPP (&pt1);
		pDoc->LPtoPP (&pt2);
		pDC->MoveTo (pt1);
		pDC->LineTo (pt2);
		pDC->SetROP2( DrawMode );
		pDC->SelectObject( OldPen );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CImageManView message handlers


int CImageManView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	DragAcceptFiles();
 	return(0);
}                        

BOOL CImageManView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
{
	// do the scroll
	if (!CScrollView::OnScrollBy(sizeScroll, bDoScroll))
		return FALSE;
	// update the position of any in-place active item
	if (bDoScroll)
	{
		UpdateActiveItem();
		UpdateWindow();
	}
	return TRUE;
}

BOOL CImageManView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CImageManView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CSize size;
	size.cx = pDC->GetDeviceCaps(LOGPIXELSX);
	size.cy = pDC->GetDeviceCaps(LOGPIXELSY);
	CSize size1 = pDoc->ComputerPrintSize();
	int h = size1.cy*size.cy / 100;
	int w = size1.cx*size.cx / 100;
	pDoc->m_PageHeight = pDC->GetDeviceCaps(VERTRES);
	pDoc->m_PageWidth = pDC->GetDeviceCaps(HORZRES);
	pDoc->m_NumRows = h / pDoc->m_PageHeight + (h % pDoc->m_PageHeight > 0);
	pDoc->m_NumCols = w / pDoc->m_PageWidth + (w % pDoc->m_PageWidth > 0);
	pInfo->SetMinPage(1);
	pInfo->SetMaxPage(pDoc->m_NumRows * pDoc->m_NumCols);
	CScrollView::OnBeginPrinting(pDC,pInfo);
	pDC->SelectStockObject(SYSTEM_FONT);
    pDC->SelectStockObject(NULL_BRUSH);
    pDC->SelectStockObject(BLACK_PEN);
}

void CImageManView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
}

void CImageManView::OnLButtonDown(UINT nFlags, CPoint point)
{	
	CScrollView::OnLButtonDown(nFlags, point);
	if(!m_bActive) return;

	//m_SinFlag=90时选择区域进行层理自动拾取
	//m_SinFlag=100时删除选择区域的层理裂缝
	if(m_SinFlag>0&&m_SinFlag!=90&&m_SinFlag!=100)
	{
		StartHandDrawSin();
		m_pMeasure->OnLButtonDown(nFlags, point);
		return;
	}

	CClientDC dc(this);
	OnPrepareDC(&dc,NULL);
	dc.DPtoLP(&point);
	if(m_iHoleOperate>0||m_SinFlag==90||m_SinFlag==100)
	{
		if(m_iHoleOperate>3||m_SinFlag==90||m_SinFlag==100)
		{
			m_StSelPt=point;
			m_EnSelPt=point;
			m_bRectangleSel=TRUE;
		}
		else
			DelSelContour(point);
		return;
	}

	pDoc->PPtoLP(&point);
	if(m_bDisFIDValue)
	{
		if(m_iDisFIDPreX!=point.x)
		{
			m_iDisFIDPreX=point.x;
			CRect cRect;
			GetClientRect(cRect);
			dc.DPtoLP (&cRect);
			int DrawMode = dc.SetROP2(R2_XORPEN);
			CPen AxialPen;
			AxialPen.CreatePen(PS_SOLID, 1, 0xffff00);
			CPen *OldPen = (CPen *)dc.SelectObject(&AxialPen);
			dc.MoveTo (m_iDisFIDIntiX,cRect.top );
			dc.LineTo (m_iDisFIDIntiX,cRect.bottom );
			dc.MoveTo (m_iDisFIDPreX,cRect.top );
			dc.LineTo (m_iDisFIDPreX,cRect.bottom );
			dc.SetROP2( DrawMode );
			dc.SelectObject( OldPen );
			m_iDisFIDIntiX=m_iDisFIDPreX;
			CString str;
			float LineValue=m_fDisFIDLeftValue+(m_iDisFIDPreX-m_iDisFIDLeftX)*m_fDisFIDRatio;
			if(m_DisFIDCalType==1)
				LineValue=(float)pow(10.,LineValue*1.);
			str.Format ("  %.4f",LineValue);
			str=m_DisFIDName+str;
			CMainFrame* pFrame=(CMainFrame*)AfxGetMainWnd ();
			pFrame->m_wndStatusBar.SetWindowText (str);
		}
		return;
	}	
	if(m_bCounterGlow)
	{
		if(m_iCounterPreX!=point.y)
		{
			m_iCounterPreX=point.y;
			CRect cRect;
			GetClientRect(cRect);
			dc.DPtoLP (&cRect);
			int DrawMode = dc.SetROP2(R2_XORPEN);
			CPen AxialPen;
			AxialPen.CreatePen(PS_SOLID, 1, 0xffff00);
			CPen *OldPen = (CPen *)dc.SelectObject(&AxialPen);
			CPoint pt1,pt2;
			pt1.x=cRect.left;
			pt1.y=m_iCounterIntiX;
			pt2.x=cRect.right;
			pt2.y=m_iCounterIntiX;
			pDoc->LPtoPP (&pt1);
			pDoc->LPtoPP (&pt2);
			dc.MoveTo (pt1);
			dc.LineTo (pt2);
			
			
			pt1.x=cRect.left;
			pt1.y=m_iCounterPreX;
			pt2.x=cRect.right;
			pt2.y=m_iCounterPreX;
			pDoc->LPtoPP (&pt1);
			pDoc->LPtoPP (&pt2);
			dc.MoveTo (pt1);
			dc.LineTo (pt2);
			dc.SetROP2( DrawMode );
			dc.SelectObject( OldPen );
			m_iCounterIntiX=m_iCounterPreX;
			CString str=pDoc->GetCurveValue(m_iCounterPreX);
			CMainFrame* pFrame=(CMainFrame*)AfxGetMainWnd ();
   			pFrame->m_wndStatusBar.SetWindowText (str);
			m_StatusText=str;
			m_DisCharPos=0;
		}
		return;
	}		
	COleClientItem* pActiveItem = pDoc->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL)
	{
		pActiveItem->Close();
		ASSERT(pDoc->GetInPlaceActiveItem(this) == NULL);
	}
	if ((CDrawObj::m_SelectMode == size) && (CDrawObj::c_ToolShape == foldtool))
	{
		ASSERT(m_pSelectObj);
		m_pSelectObj->OnLButtonDown(this,nFlags,point); 
		return;
	}  
    if (CDrawObj::c_ToolShape != NULL)
	{
		bObjectLink = FALSE;
		CDrawObj* pObj = NULL;  
		CRect rect(point.x,point.y,point.x,point.y);
		if (CDrawObj::c_ToolShape==depsecttool)
			pObj = (CDepSect*) new CDepSect(rect,NULL,pDoc);
		else
			pObj = (CLineObj*) new CLineObj(rect,NULL,pDoc);
		if (pObj != NULL)
		{                   
			DeSelectObj();
			pDoc->AddObj(pObj);
			SelectObj(pObj);
			pObj->OnLButtonDown(this,nFlags,point);
			return;
		}
	}
	CDrawObj* pDrawObj = NULL;
	CDrawObj::m_SelectMode = none;  
	if(m_pSelectObj != NULL) 
		pDrawObj = m_pSelectObj->ObjectAt(this,point);
	if (pDrawObj == NULL) 
		pDrawObj = pDoc->ObjectAt(this,point);
	if (pDrawObj != NULL)
	{
		if (pDrawObj != m_pSelectObj)
		{
			if (bObjectLink)
			{                                       
				CDrawObj* pObj = pDrawObj;
				while ((pObj != NULL) && (pObj != m_pSelectObj)) 
					pObj = pObj->GetParentObj();
				if (pObj == NULL) 
				{
					pDoc->RemoveObj(m_pSelectObj);
					pDrawObj->AddObj(m_pSelectObj,2);
					bObjectLink = FALSE;
				}	
				return;
			}
			SelectObj(pDrawObj);
		}	
		pDrawObj->OnLButtonDown(this,nFlags,point);
		if(pDrawObj->IsKindOf(RUNTIME_CLASS(CSinPoint)))
		{
			CSinPoint* pSinPoint=(CSinPoint *)pDrawObj;
			if(pSinPoint!=NULL)
			{
				float dep=(float)pDoc->GetLeftTrackObj ()->YCoorToDepth(point.y);
				m_sindep0=pSinPoint->m_Dep-dep;
			}
		}
	}       
	else DeSelectObj();
	bObjectLink = FALSE;
}

void CImageManView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!m_bActive) return;
	c_ToolPro=NoneTool;
	CClientDC dc(this);
	OnPrepareDC(&dc,NULL);
	if(m_bDisFIDValue)
	{
		CRect cRect;
		GetClientRect(cRect);
		dc.DPtoLP (&cRect);
		int DrawMode = dc.SetROP2(R2_XORPEN);
		CPen AxialPen;
		AxialPen.CreatePen(PS_SOLID, 1,0xffff00);
		CPen *OldPen = (CPen *)dc.SelectObject(&AxialPen);
		dc.MoveTo (m_iDisFIDPreX,cRect.top );
		dc.LineTo (m_iDisFIDPreX,cRect.bottom );
		dc.SetROP2( DrawMode );
		dc.SelectObject( OldPen );
		m_bDisFIDValue=FALSE;
		CMainFrame* pFrame=(CMainFrame*)AfxGetMainWnd ();
   	    pFrame->m_wndStatusBar.SetWindowText ("准备");
		return;
	}
	if(m_bCounterGlow)
	{
		CRect cRect;
		GetClientRect(cRect);
		dc.DPtoLP (&cRect);
		int DrawMode = dc.SetROP2(R2_XORPEN);
		CPen AxialPen;
		AxialPen.CreatePen(PS_SOLID, 1,0xffff00);
		CPen *OldPen = (CPen *)dc.SelectObject(&AxialPen);
		CPoint pt1,pt2;
		pt1.x=cRect.left;
		pt1.y=m_iCounterPreX;
		pt2.x=cRect.right;
		pt2.y=m_iCounterPreX;
		pDoc->LPtoPP (&pt1);
		pDoc->LPtoPP (&pt2);
		dc.MoveTo (pt1);
		dc.LineTo (pt2);
		dc.SetROP2( DrawMode );
		dc.SelectObject( OldPen );
		m_bCounterGlow=FALSE;
		CMainFrame* pFrame=(CMainFrame*)AfxGetMainWnd ();
	    pFrame->m_wndStatusBar.SetWindowText ("准备");
		m_SelCurveNameArray.RemoveAll();
		m_bAlikePro=FALSE;
		return;
	}
	dc.DPtoLP(&point);
	pDoc->PPtoLP(&point);
	if(m_pSelectObj != NULL)
	{
		if(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CDepSect)))
			OnModfyDepsegment();
		else
			m_pSelectObj->OnLButtonDblClk(this,nFlags,point);
	}

	CImageManApp* pApp=(CImageManApp*)AfxGetApp();
	pApp->m_Nib =FALSE;
}

void CImageManView::OnLButtonUp(UINT nFlags, CPoint point)
{
	float sindep0=m_sindep0;
	m_sindep0=0.f;
	if (!m_bActive) return;
	CClientDC dc(this);
	OnPrepareDC(&dc,NULL);
	dc.DPtoLP(&point);	
	if(m_bRectangleSel)
	{
		DeSelectObj();
		m_bRectangleSel=FALSE;
		int DrawMode = dc.SetROP2(R2_XORPEN);
		CPen AxialPen;
		AxialPen.CreatePen(PS_DOT,1,RGB(0,0,0));//0xffff00);
		CPen *OldPen = (CPen *)dc.SelectObject(&AxialPen);
		CBrush *OldBrush=(CBrush *)dc.SelectStockObject(NULL_BRUSH);
		dc.Rectangle(m_StSelPt.x,m_StSelPt.y,m_EnSelPt.x,m_EnSelPt.y);	
		m_EnSelPt=point;
		dc.SetROP2(DrawMode);
		dc.SelectObject(OldPen);
		dc.SelectObject(OldBrush);
		if(m_StSelPt.x>m_EnSelPt.x)
		{
			int x=m_StSelPt.x;
			m_StSelPt.x=m_EnSelPt.x;
			m_EnSelPt.x=x;
		}
		if(m_StSelPt.y>m_EnSelPt.y)
		{
			int y=m_StSelPt.y;
			m_StSelPt.y=m_EnSelPt.y;
			m_EnSelPt.y=y;
		}
		//m_SinFlag=90时对选定的区域进行层理自动拾取
		if(m_SinFlag==90)
			RectangleComputeDip();
		else	if(m_SinFlag==100)
			RectangleDeleteDip();
		else	if(m_iHoleOperate==4)
			DelSelContour(m_EnSelPt);
		else	if(m_iHoleOperate==5)
			RectanglePickupContour();
		else	if(m_iHoleOperate==6)
			EllipsePickupContour();
		return;
	}

	pDoc->PPtoLP(&point);
	if((CDrawObj::m_SelectMode == none)
		&&(c_ToolPro!=CurveNib)) return;
	if(m_pSelectObj != NULL)
	{
		c_ToolPro = NoneTool;
		m_pSelectObj->OnLButtonUp(this,nFlags,point);
		if(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CSinPoint)))
		{
			if(GetKeyState(VK_CONTROL) & ~1)
			{
				CSinPoint* pSinPoint=(CSinPoint *)m_pSelectObj;
				if(pSinPoint!=NULL)
				{
					float dep=(float)pDoc->GetLeftTrackObj ()->YCoorToDepth(point.y);
					dep+=sindep0;
					CString Title[6]={"层理","充填缝","张开缝","压裂缝","半充填缝","层间缝"};
					int flag=0;
					while(flag<6)
					{
						if(Title[flag]==pSinPoint->m_Flag)
							break;
						flag++;
					}
					flag++;
					flag*=10;
					CStratumObj* pStObj=pDoc->GetStratumObject();
					if(pStObj!=NULL)
					{
						pStObj->AddPoint(dep,pSinPoint->m_Dip,pSinPoint->m_Dir,pSinPoint->m_Dip0,pSinPoint->m_Dir0,flag);
						pDoc->m_HWOprateNum++;
					}
					CMarkVectorObj* pVecObj=pDoc->GetMarkVectorObject();
					if(pVecObj!=NULL)
						pVecObj->AddPoint(dep,pSinPoint->m_Dip,pSinPoint->m_Dir,pSinPoint->m_Dip0,pSinPoint->m_Dir0,flag);
					return;
				}
			}
		}
		else if(m_pImage3D!=NULL&&m_pImage3D==m_pSelectObj);
			m_EddyPlay=!m_EddyPlay;
		if(CDrawObj::c_ToolShape != nonetool) return;
		CImageManApp* pApp=(CImageManApp*) AfxGetApp();
		//幅度校正
		if(pApp->m_PlusValue)
		{
			if(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CCurveObj)))
			{
				CCurveObj* pObj=(CCurveObj*) m_pSelectObj;
				if(pObj!=NULL)
				{
					CString WellName,CurveName;		
					CFIDIO::GetFullWellName(pObj->m_strPathName,WellName.GetBuffer(512));
					WellName.ReleaseBuffer();				
					CFIDIO::GetFIDName(pObj->m_strPathName,CurveName.GetBuffer(512));
					CurveName.ReleaseBuffer();					
					CFIDIO* pFIDtemp = NULL;
					pFIDtemp = new CFIDIO(WellName + "." + CurveName);
					if(!pFIDtemp->Open())
					{
						MessageBox("打开 "+WellName + "." +
							CurveName+" 文件失败!","错误提示...",MB_OK);
						delete pFIDtemp;return;
					}	
					BeginWaitCursor();
					int RectLeftX=pObj->m_Rect.left;
					float DataValue=(float)((pObj->m_RightVal1-pObj->m_LeftVal1)
											*pObj->m_MovePointX/pObj->m_Rect.Width());
					if(fabs(DataValue)>=0.0001)
					{
						char ss[20];						
						sprintf(ss,".#$#%d",m_InitOprationNum+1);
						if((pFIDtemp->Copy(m_UnDoFilePath+ss))!=-1)
						{
							m_InitOprationNum++;
							m_UnDoName.Add(WellName+"."+CurveName);
							m_UnDoNameCopy.Add(m_UnDoFilePath+ss);					
							delete pFIDtemp;

							CFIDIO* pFIDW = NULL;
							pFIDW = new CFIDIO(WellName+"."+CurveName);
							pFIDW->Open(); 
							CFIDIO* pFIDR = NULL;
							pFIDR = new CFIDIO(m_UnDoFilePath+ss);							
							pFIDR->Open();
							unsigned long AllRecord=(unsigned long)((pFIDR->GetEndDepth ()-
								pFIDR->GetStartDepth ())/pFIDR->GetDepthLevel ());
							unsigned long i=0;
							float fbuf;
							while(i<AllRecord)
							{
								pFIDR->Read(2,1,&fbuf);
								if(fbuf!=-9999.f)
								{
									fbuf+=DataValue;
									pFIDW->Write(2,1,&fbuf);
								}
								i++;pFIDR->Next();pFIDW->Next();
							}
							delete pFIDR;delete pFIDW;
						}
						else
							delete pFIDtemp;
						pDoc->OnReFresh ();
					}
					EndWaitCursor();
				}
			}
			pApp->m_PlusValue=FALSE;
			OnDrawSelect();
			return;
		}
		//深度校正
		if(pApp->m_MoveDep)
		{
			if(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CCurveObj)))
			{
				CCurveObj* pObj=(CCurveObj*) m_pSelectObj;
				if(pObj!=NULL)
				{
					CString WellName,CurveName;		
					CFIDIO::GetFullWellName(pObj->m_strPathName , WellName.GetBuffer(512));
					WellName.ReleaseBuffer();				
					CFIDIO::GetFIDName(pObj->m_strPathName , CurveName.GetBuffer(512));
					CurveName.ReleaseBuffer();
					CFIDIO* pFIDtemp = NULL;
					pFIDtemp = new CFIDIO(WellName + "." + CurveName);
					if(!pFIDtemp->Open())
					{
						MessageBox("打开 "+WellName + "." +
							CurveName+" 文件失败!","错误提示...",MB_OK);
						delete pFIDtemp;return;
					}
					BeginWaitCursor();
					float DataDep=(float)(pObj->YCoorToDepth (pObj->m_MovePointY)
						-pObj->YCoorToDepth (0));
					float Rlev=pFIDtemp->GetDepthLevel ();
					DataDep=((int)(DataDep/Rlev))*Rlev;
					if(fabs(DataDep)>=fabs(Rlev))
					{
						char ss[20];						
						sprintf(ss,".#$#%d",m_InitOprationNum+1);
						if((pFIDtemp->Copy(m_UnDoFilePath+ss))!=-1)
						{
							m_InitOprationNum++;
							m_UnDoName.Add(WellName+"."+CurveName);
							m_UnDoNameCopy.Add(m_UnDoFilePath+ss);					
							delete pFIDtemp;

							CFIDIO* pFIDIO = NULL;
							pFIDIO = new CFIDIO(WellName + "." + CurveName);
							pFIDIO->Open();
							pFIDIO->MoveDepth (DataDep);
							delete pFIDIO;
						}
						else
							delete pFIDtemp;
						pDoc->OnReFresh ();
					}
					EndWaitCursor();
				}
			}
			pApp->m_MoveDep=FALSE;
			OnDrawSelect();
			return;
		}
	}
}

BOOL CImageManView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	int iScrollBarPos = GetScrollPos( SB_VERT )-zDelta;
	SetScrollPos( SB_VERT, iScrollBarPos );
	Invalidate();
	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}

void CImageManView::OnMouseMove(UINT nFlags, CPoint point)
{
	if(!m_bActive) return;
	CClientDC dc(this);	
	OnPrepareDC(&dc,NULL); 
	dc.DPtoLP(&point);
	if(m_bRectangleSel)
	{
		int DrawMode = dc.SetROP2(R2_XORPEN);
		CPen AxialPen;
		AxialPen.CreatePen(PS_DOT,1,RGB(0,0,0));//0xffff00);
		CPen *OldPen = (CPen *)dc.SelectObject(&AxialPen);
		CBrush *OldBrush=(CBrush *)dc.SelectStockObject(NULL_BRUSH);
		dc.Rectangle(m_StSelPt.x,m_StSelPt.y,m_EnSelPt.x,m_EnSelPt.y);
		
		m_EnSelPt=point;
		
		dc.Rectangle(m_StSelPt.x,m_StSelPt.y,m_EnSelPt.x,m_EnSelPt.y);
		dc.SetROP2(DrawMode);
		dc.SelectObject(OldPen);
		dc.SelectObject(OldBrush);
		return;
	}
	CPoint DisPoint=point; 
	pDoc->PPtoLP(&point);
	if(GetCapture()!= this)
	{		
		if (!pDoc->OnSetCursor(this,point)) 
			SetCursor(LoadCursor(NULL,IDC_ARROW));
		CRect rect(point.x,point.y,point.x,point.y);
		if ((pDoc->OnToolTip(this,rect,TipText))||
			((m_bDisFIDValue)&&(m_iDisFIDPreX-2<DisPoint.x)&&
			(DisPoint.x<m_iDisFIDPreX+2)))
		{
			if (!bToolTipActive)
			{
				if(c_ToolPro==CurveNib)
					SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_BOX));
				else
				{
					SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_CROSS));
					m_tooltip.Activate(TRUE);
					pDoc->LPtoPP(rect);
					dc.LPtoDP(rect);
					rect.InflateRect(5,5);
					m_tooltip.SetToolRect(this,ID_CURVE_VALUE,rect);
					if ((m_bDisFIDValue)&&(m_iDisFIDPreX-3<DisPoint.x)&&
						(DisPoint.x<m_iDisFIDPreX+3))
					{
						CString str;
						float LineValue=m_fDisFIDLeftValue+(m_iDisFIDPreX-m_iDisFIDLeftX)*m_fDisFIDRatio;
						if(m_DisFIDCalType==1)
							LineValue=(float)pow(10.,LineValue*1.);
						str.Format ("  %.4f",LineValue);
						str=m_DisFIDName+str;
						m_tooltip.UpdateTipText((LPCTSTR)str,this,ID_CURVE_VALUE);
					}
					else
						m_tooltip.UpdateTipText((LPCTSTR)TipText,this,ID_CURVE_VALUE);
				}
				bToolTipActive = TRUE;
			    CPoint ps;
				GetCursorPos(&ps);
			    SetCursorPos(ps.x,ps.y);
			}
			
		}
		else
		{
			m_tooltip.Activate(FALSE);
			bToolTipActive = FALSE;
		}
		return;
	}	
	if (bToolTipActive)
	{
		m_tooltip.Activate(FALSE);
		bToolTipActive = FALSE;
	}
	if (m_pSelectObj != NULL)
		m_pSelectObj->OnMouseMove(this,nFlags,point); 
}

void CImageManView::SelectObj(CDrawObj* pObj)
{
	DeSelectObj();
	m_pSelectObj = pObj;
	pObj->SetSelect(this,TRUE);
	pObj->InvalObj(this);
}                 

void CImageManView::DeSelectObj()
{
	if (m_pSelectObj != NULL)
	{
		m_pSelectObj->SetSelect(this,FALSE);
		CDrawObj* pObj = m_pSelectObj;
		m_pSelectObj = NULL;
		pObj->InvalObj(this);
	}
}

void CImageManView::PasteNative(COleDataObject& dataObject)
{
	// get file refering to clipboard data
	CFile* pFile = dataObject.GetFileData(CImageManDoc::m_cfDraw);
	CObList m_ObjList;
	if (pFile == NULL)return;

	// connect the file to the archive
	CArchive ar(pFile, CArchive::load);
	TRY
	{
		ar.m_pDocument = pDoc; // set back-pointer in archive

		// read the selection
		m_ObjList.Serialize(ar);
	}
	CATCH_ALL(e)
	{
		ar.Close();
		delete pFile;
		THROW_LAST();
		return;
	}
	END_CATCH_ALL

	CObject * pObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	if (pos != NULL) 
	{
		pObj = m_ObjList.GetNext(pos);
		if (pObj->IsKindOf(RUNTIME_CLASS(CDrawObj)))	
			m_pSelectObj = (CDrawObj*) pObj;
	}		
	ar.Close();
	delete pFile;
}

void CImageManView::OnEditCopy()
{
	if(m_pSelectObj->IsKindOf (RUNTIME_CLASS(CCurveObj)))
	{
		ASSERT_VALID(this);
		ASSERT(CImageManDoc::m_cfDraw != NULL);
		// Create a shared file and associate a CArchive with it
		CSharedFile file;           
		CObList m_ObjList;
		CArchive ar(&file, CArchive::store);
		// Serialize selected objects to the archive
		m_ObjList.AddTail(m_pSelectObj);
		m_ObjList.Serialize(ar);
		ar.Close();
		COleDataSource* pDataSource = NULL;
		TRY
		{
			pDataSource = new COleDataSource;
			// put on local format instead of or in addation to
			pDataSource->CacheGlobalData(CImageManDoc::m_cfDraw, file.Detach());
			// if only one item and it is a COleClientItem then also
			// paste in that format
			pDataSource->SetClipboard();
		}
		CATCH_ALL(e)
		{
			delete pDataSource;
			THROW_LAST();
		}
		END_CATCH_ALL
	}
}

void CImageManView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pSelectObj!=NULL);
}

void CImageManView::OnEditCut()
{
	OnEditCopy();
	if (m_pSelectObj != NULL) 
	{
		CDrawObj* pObj = m_pSelectObj;
		CDrawObj* pParentObj = m_pSelectObj->GetParentObj();
		int nParentType = m_pSelectObj->GetParentType();
		pDoc->RemoveObj(m_pSelectObj);
		m_pSelectObj = NULL;
		if ((pParentObj != NULL) && (pParentObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (nParentType == 0))
		{
			((CTrackObj*)pParentObj)->Arrangle();
			pParentObj->Invalidate(this);
		}
		else if ((pParentObj == NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))))
		{
			if (pDoc->bHoriAutoArrangle) pDoc->DoHoriArrangle(pDoc->m_HoriArrangleMode);
			if (pDoc->bVertAutoAlign) pDoc->DoVertAlign();
			pDoc->UpdateAllViews(NULL,HINT_UPDATE_WINDOW);
		}
		else pObj->Invalidate(this);
	//	pObj->Remove();
	}	
}

void CImageManView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	CDrawObj* pObj = m_pSelectObj;
	pCmdUI->Enable(pObj != NULL);
}

BOOL CImageManView::DoPasteItem(COleDataObject& dataObject)
{
	if (dataObject.IsDataAvailable(CImageManDoc::m_cfDraw))
		PasteNative(dataObject);
		// now add all items in m_selection to document

	if (m_pSelectObj != NULL)
	{      
		pDoc->AddObj(m_pSelectObj);
		if(m_pSelectObj->IsKindOf (RUNTIME_CLASS(CCurveObj)))
		{
			CCurveObj* pObj=(CCurveObj*) m_pSelectObj;
			if(pObj!=NULL)
			{
				pObj->m_HeadRect .OffsetRect (100,50);
				pObj->m_Rect .OffsetRect (100,50);
				pObj->m_AliasName =pObj->m_AliasName+"+";
				pObj->Invalidate();
			}
		}
		SelectObj(m_pSelectObj);
		return TRUE;
	}
	return FALSE;
}

void CImageManView::OnEditPaste()
{
	COleDataObject dataObject;
	dataObject.AttachClipboard();
	DeSelectObj();
	DoPasteItem(dataObject);
}

void CImageManView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	// determine if private or standard OLE formats are on the clipboard
	COleDataObject dataObject;
	BOOL bEnable = dataObject.AttachClipboard() &&
		(dataObject.IsDataAvailable(CImageManDoc::m_cfDraw) ||
		 COleClientItem::CanCreateFromData(&dataObject));
	// enable command based on availability
	pCmdUI->Enable(bEnable);
}

void CImageManView::OnKillFocus(CWnd* pNewWnd) 
{
	CScrollView::OnKillFocus(pNewWnd);
}

void CImageManView::OnSetFocus(CWnd* pOldWnd)
{
	if (pDoc == NULL) return;
	COleClientItem* pActiveItem = pDoc->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL &&
		pActiveItem->GetItemState() == COleClientItem::activeUIState)
	{
		CWnd* pWnd = pActiveItem->GetInPlaceWindow();
		if (pWnd != NULL)
		{
			pWnd->SetFocus();
			return;                 
		}
	}
	CScrollView::OnSetFocus(pOldWnd);
}

CRect CImageManView::GetInitialPosition()
{
	CClientDC dc(this);
	OnPrepareDC(&dc,NULL);
	CRect rect(10, 10, 40, 40);
	dc.DPtoLP(rect);
	pDoc->PPtoLP(rect);
	return rect;
}

void CImageManView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);
	if (pDoc == NULL) return;
	UpdateActiveItem();                
	CSize size(cx,cy);
	if (pDoc->m_ScaleNo == 0)
	{
		OnUpdate(NULL,HINT_UPDATE_SIZE,NULL);
		return;
	}
	CClientDC dc(this);
	OnPrepareDC(&dc,NULL);
	dc.DPtoLP(&size);
	int nLeftSpace = (size.cx - pDoc->m_Rect.Width()) / 2;
	if (nLeftSpace < 10) nLeftSpace = 10;
	if (m_LeftSpace != nLeftSpace)
	{
		m_LeftSpace = nLeftSpace;
		OnUpdate(NULL,HINT_UPDATE_SIZE,NULL);
	}
}

void CImageManView::UpdateActiveItem()
{
	if (pDoc == NULL) return;
	COleClientItem* pActiveItem = pDoc->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL &&
		pActiveItem->GetItemState() == COleClientItem::activeUIState)
	    pActiveItem->SetItemRects();
}


void CImageManView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{	
	CClientDC dc(this);	
	CRect cRect;
	GetClientRect(cRect);
	CMainFrame* pFrame=(CMainFrame*)AfxGetMainWnd ();
	CString str;
	if((m_bDisFIDValue)&&((nChar==VK_LEFT)||(nChar==VK_RIGHT)))
	{
		CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
		OnPrepareDC(&dc,NULL);
		dc.DPtoLP (&cRect);
		int DrawMode = dc.SetROP2(R2_XORPEN);
		CPen AxialPen;
		AxialPen.CreatePen(PS_SOLID, 1, 0xffff00);
		CPen *OldPen = (CPen *)dc.SelectObject(&AxialPen);
		dc.MoveTo (m_iDisFIDIntiX,cRect.top );
		dc.LineTo (m_iDisFIDIntiX,cRect.bottom );
		if(nChar==VK_LEFT) 
		{
			if((GetKeyState(VK_CONTROL) & ~1))
				m_iDisFIDPreX -= 5;
			else
			{
				Beep(400,50);
				m_iDisFIDPreX -= 10;
			}
		}
		if(nChar==VK_RIGHT)
		{
			if((GetKeyState(VK_CONTROL) & ~1))
				m_iDisFIDPreX += 5;
			else
			{
				Beep(1000,50);
				m_iDisFIDPreX += 10;
			}
		}
		dc.MoveTo (m_iDisFIDPreX,cRect.top );
		dc.LineTo (m_iDisFIDPreX,cRect.bottom );
		dc.SetROP2( DrawMode );
		dc.SelectObject( OldPen );
		m_iDisFIDIntiX=m_iDisFIDPreX;
		float LineValue=m_fDisFIDLeftValue+(m_iDisFIDPreX-m_iDisFIDLeftX)*m_fDisFIDRatio;
		if(m_DisFIDCalType==1)
			LineValue=(float)pow(10.,LineValue*1.);
		str.Format ("  %.4f",LineValue);
		str=m_DisFIDName+str;
		pFrame->m_wndStatusBar.SetWindowText (str);
		return;
	}
	if((m_bCounterGlow)&&((nChar==VK_UP)||(nChar==VK_DOWN)))
	{
		CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
		OnPrepareDC(&dc,NULL);
		dc.DPtoLP (&cRect);
		int DrawMode = dc.SetROP2(R2_XORPEN);
		CPen AxialPen;
		AxialPen.CreatePen(PS_SOLID, 1, 0xffff00);
		CPen *OldPen = (CPen *)dc.SelectObject(&AxialPen);
		CPoint pt1,pt2;
		pt1.x=cRect.left;
		pt1.y=m_iCounterIntiX;
		pt2.x=cRect.right;
		pt2.y=m_iCounterIntiX;
		pDoc->LPtoPP (&pt1);
		pDoc->LPtoPP (&pt2);
		dc.MoveTo (pt1);
		dc.LineTo (pt2);
		float dep1=-9999.f;
		if((GetKeyState(VK_CONTROL) & ~1)&&(GetKeyState(VK_SHIFT) & ~1))
		{
			dep1=pDoc->GetCurrentDep(m_iCounterPreX);
			Beep(400,50);
			if(nChar==VK_UP) 
				m_iCounterPreX -= 5;
			if(nChar==VK_DOWN)
				m_iCounterPreX += 5;
		}
		else
		{
			if(nChar==VK_UP) 
			{
				if((GetKeyState(VK_CONTROL) & ~1))		
					m_iCounterPreX -= 5;
				else
				{
					Beep(400,50);
					m_iCounterPreX -= 10;
				}
			}
			if(nChar==VK_DOWN)
			{
				if((GetKeyState(VK_CONTROL) & ~1))
					m_iCounterPreX += 5;
				else
				{
					Beep(1000,50);
					m_iCounterPreX += 10;
				}
			}
		}
		pt1.x=cRect.left;
		pt1.y=m_iCounterPreX;
		pt2.x=cRect.right;
		pt2.y=m_iCounterPreX;
		pDoc->LPtoPP (&pt1);
		pDoc->LPtoPP (&pt2);
		dc.MoveTo (pt1);
		dc.LineTo (pt2);
		
		dc.SetROP2( DrawMode );
		dc.SelectObject( OldPen );
		m_iCounterIntiX=m_iCounterPreX;
		str=pDoc->GetCurveValue(m_iCounterPreX);
   		pFrame->m_wndStatusBar.SetWindowText (str);
		m_StatusText=str;
		m_DisCharPos=0;
		if(dep1>0.f)
		{
			float dep2=pDoc->GetCurrentDep(m_iCounterPreX);
			ProMultiCurveVal(dep1,dep2);
		}
		return;
	}
	CPoint ptScroll = GetDeviceScrollPosition();
	CPoint ptSave = ptScroll;
	switch (nChar)
	{
		case VK_LEFT: 
			if(m_bCounterGlow)
			{
				m_DisCharPos--;
				if(m_DisCharPos<0) m_DisCharPos=0;
				str=m_StatusText.Mid(m_DisCharPos);
				pFrame->m_wndStatusBar.SetWindowText (str);
				break;
			}
			ptScroll.x -= 20;	
			break;
		case VK_RIGHT:
			if(m_bCounterGlow)
			{
				m_DisCharPos++;
				if(m_DisCharPos>m_StatusText.GetLength()) 
					m_DisCharPos=m_StatusText.GetLength();
				str=m_StatusText.Mid(m_DisCharPos);
				pFrame->m_wndStatusBar.SetWindowText (str);
				break;
			}
			ptScroll.x += 20;
			break;
		case VK_UP: 
			ptScroll.y -= 20;
			break;
		case VK_DOWN: 
			ptScroll.y += 20;
			break;
		case VK_PRIOR:
			ptScroll.y -= cRect.Height();
			break;
		case VK_NEXT:
			ptScroll.y += cRect.Height();
			break;
	}
	if (ptScroll.y < 0) ptScroll.y = 0;
	else if (ptScroll.y+cRect.Height() > m_totalDev.cy) ptScroll.y = m_totalDev.cy - cRect.Height();
	if (ptScroll.x < 0) ptScroll.x = 0;
	else if (ptScroll.x+cRect.Width() > m_totalDev.cx) ptScroll.x = m_totalDev.cx - cRect.Width();
	if ((ptSave.x != ptScroll.x) && (cRect.Width() < m_totalDev.cx))
	{
		SetScrollPos(SB_HORZ, ptScroll.x);
		ScrollWindow(ptSave.x - ptScroll.x,0);
		UpdateWindow();
	}
	if ((ptSave.y != ptScroll.y) && (cRect.Height() < m_totalDev.cy))
	{
		SetScrollPos(SB_VERT, ptScroll.y);
		ScrollWindow(0,ptSave.y - ptScroll.y);
		UpdateWindow();
	}
	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CImageManView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CScrollView::OnChar(nChar, nRepCnt, nFlags);
}

void CImageManView::OnDropFiles(HDROP hDropInfo)
{
	char szFileName[512];
	CString szFIDName;
	CFIDDrawObj* pObj;
	CPoint pt;
    DragQueryPoint (hDropInfo, (LPPOINT) &pt);
    int gwFilesDropped = DragQueryFile (hDropInfo,(UINT)-1,NULL,0);
    for (int i=0; i < gwFilesDropped; i++)
    {
		DragQueryFile (hDropInfo,i,(LPSTR) szFileName,512);
		CFIDIO::GetFIDName(szFileName,szFIDName.GetBuffer(128));
		szFIDName.ReleaseBuffer();
		pObj = NULL;
		if (szFIDName != "")
		{
			szFIDName.MakeUpper();
			CRect m_Rect(pt.x,pt.y,pt.x,pt.y);
			double sdep,edep;
			pDoc->GetDepthRangle(sdep, edep);
			CFIDIO* pFIDIO = new CFIDIO(szFileName);
			if (pFIDIO->Open() && (pFIDIO->GetDimenSion() == 2))
			{
				pObj = new CCurveObj(szFileName,m_Rect,NULL,pDoc);
				pObj->SetDepth(sdep,edep);
				if (!pObj->Properties(this)) 
				{
					delete pObj;
					pObj = NULL;
				}
			}
			else
			{
		        CString  msg;
				AfxFormatString1(msg,IDS_CANNT_INPUT_FILE,szFileName);
				AfxMessageBox(msg,MB_OK | MB_ICONSTOP);
			}
			delete pFIDIO;
  			if (pObj != NULL)
			{
				if (!pObj->IsKindOf(RUNTIME_CLASS(CCurveObj)))
					pObj->SetDepth(sdep,edep);
				pDoc->AddObj(pObj);
				SelectObj(pObj);
			}
         }	
    }
    DragFinish (hDropInfo);	
}

void CImageManView::OnObjectLink()
{
	bObjectLink = TRUE;
}

void CImageManView::OnUpdateObjectLink(CCmdUI* pCmdUI)
{
 	pCmdUI->Enable((m_pSelectObj != NULL) && (m_pSelectObj->GetParentObj() == NULL));
}

void CImageManView::OnObjectUnlink()
{
	m_pSelectObj->GetParentObj()->RemoveObj(m_pSelectObj);
	m_pSelectObj->SetParentObj(NULL,2);
	pDoc->AddObj(m_pSelectObj);
}

void CImageManView::OnUpdateObjectUnlink(CCmdUI* pCmdUI)
{
 	pCmdUI->Enable((m_pSelectObj != NULL) && (m_pSelectObj->GetParentType() == 2));
}

void CImageManView::OnViewTrackAlignClient()
{
	pDoc->DoVertAlignHead();
	pDoc->SetSize();
	pDoc->UpdateAllViews(NULL,HINT_UPDATE_WINDOW);
}

void CImageManView::OnViewTrackAlignDepth()
{
	pDoc->DoVertAlignDepth();
	pDoc->SetSize();
	pDoc->UpdateAllViews(NULL,HINT_UPDATE_WINDOW);
}

void CImageManView::OnViewTrackAlignTop()
{
	pDoc->DoVertAlignTop();
	pDoc->SetSize();
	pDoc->UpdateAllViews(NULL,HINT_UPDATE_WINDOW);
}

void CImageManView::OnViewTrackVertAlign()
{
	pDoc->DoVertAlign();
	pDoc->UpdateAllViews(NULL,HINT_UPDATE_WINDOW);
}

void CImageManView::OnViewTrackVertAutoalign()
{
	pDoc->bVertAutoAlign = !pDoc->bVertAutoAlign;
	if (pDoc->bVertAutoAlign)
	{
		pDoc->DoVertAlign();
		pDoc->UpdateAllViews(NULL,HINT_UPDATE_WINDOW);
	}
}

void CImageManView::OnViewTrackVertSetautoalign()
{
	CDocVertAlignDlg m_Dialog(pDoc);
	if (m_Dialog.DoModal() == IDOK)
	{
		pDoc->DoVertAlign();
		pDoc->UpdateAllViews(NULL,HINT_UPDATE_WINDOW);
	}
}

void CImageManView::OnViewTrackArrangleLeft()
{
	pDoc->DoHoriArrangle(IDM_LEFT);
	pDoc->UpdateAllViews(NULL,HINT_UPDATE_WINDOW);
}

void CImageManView::OnViewTrackArrangleLeftright()
{
	pDoc->DoHoriArrangle(IDM_JUST);
	pDoc->UpdateAllViews(NULL,HINT_UPDATE_WINDOW);
}

void CImageManView::OnViewTrackArrangleRight()
{
	pDoc->DoHoriArrangle(IDM_RIGHT);
	pDoc->UpdateAllViews(NULL,HINT_UPDATE_WINDOW);
}

void CImageManView::OnViewTrackHoriArrangle()
{
	pDoc->DoHoriArrangle(pDoc->m_HoriArrangleMode);
	pDoc->UpdateAllViews(NULL,HINT_UPDATE_WINDOW);
}

void CImageManView::OnViewTrackHoriAutoarangle()
{
	pDoc->bHoriAutoArrangle = !pDoc->bHoriAutoArrangle;
	if (pDoc->bHoriAutoArrangle)
	pDoc->DoHoriArrangle(pDoc->m_HoriArrangleMode);
	pDoc->UpdateAllViews(NULL,HINT_UPDATE_WINDOW);
}

void CImageManView::OnViewTrackHoriSetautoarrangle()
{
	CDocHoriArrangleDlg m_Dialog(pDoc);
	if (m_Dialog.DoModal() == IDOK)
	{
		pDoc->DoHoriArrangle(pDoc->m_HoriArrangleMode);
		pDoc->UpdateAllViews(NULL,HINT_UPDATE_WINDOW);
	}
}

void CImageManView::OnUpdateViewTrackAlignClient(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pDoc->m_bVertAlignHead);
	pCmdUI->Enable(!pDoc->bVertAutoAlign);
}

void CImageManView::OnUpdateViewTrackAlignDepth(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pDoc->m_bVertAlignDepth);
	pCmdUI->Enable(!pDoc->bVertAutoAlign);
}

void CImageManView::OnUpdateViewTrackAlignTop(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pDoc->m_bVertAlignTop);
	pCmdUI->Enable(!pDoc->bVertAutoAlign);
}

void CImageManView::OnUpdateViewTrackVertAlign(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!pDoc->bVertAutoAlign);
}

void CImageManView::OnUpdateViewTrackVertAutoalign(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pDoc->bVertAutoAlign);
}

void CImageManView::OnUpdateViewTrackArrangleLeft(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!pDoc->bHoriAutoArrangle);
	pCmdUI->SetCheck(pDoc->m_HoriArrangleMode == IDM_LEFT);
}

void CImageManView::OnUpdateViewTrackArrangleLeftright(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pDoc->m_HoriArrangleMode == IDM_JUST);
	pCmdUI->Enable(!pDoc->bHoriAutoArrangle);
}

void CImageManView::OnUpdateViewTrackArrangleRight(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pDoc->m_HoriArrangleMode == IDM_RIGHT);
	pCmdUI->Enable(!pDoc->bHoriAutoArrangle);
}

void CImageManView::OnUpdateViewTrackHoriArrangle(CCmdUI* pCmdUI)
{   
	pCmdUI->Enable(!pDoc->bHoriAutoArrangle);
}

void CImageManView::OnUpdateViewTrackHoriAutoarangle(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pDoc->bHoriAutoArrangle);
}

void CImageManView::OnDrawSelect()
{
	c_ToolPro = NoneTool;
	//拾取层理裂缝的交互操作选项
	if(m_pMeasure!=NULL)
		delete m_pMeasure;
	m_pMeasure=NULL;
	m_pHandDrawObj=NULL;
	m_SinFlag=-10;
	//删除孔洞操作选项
	m_iHoleOperate=-1;

	CDrawObj::c_ToolShape = nonetool;
	CImageManApp* pApp=(CImageManApp*) AfxGetApp(); 
	pApp->m_MoveDep=FALSE;
	pApp->m_PlusValue =FALSE;
	DeSelectObj();
	pDoc->OnReFresh ();
	pDoc->m_RemObjList.RemoveAll ();
	
	CClientDC dc(this);
	OnPrepareDC(&dc,NULL);
	if(m_bDisFIDValue)
	{
		CRect cRect;
		GetClientRect(cRect);
		dc.DPtoLP (&cRect);
		int DrawMode = dc.SetROP2(R2_XORPEN);
		CPen AxialPen;
		AxialPen.CreatePen(PS_SOLID, 1,0xffff00);
		CPen *OldPen = (CPen *)dc.SelectObject(&AxialPen);
		dc.MoveTo (m_iDisFIDPreX,cRect.top );
		dc.LineTo (m_iDisFIDPreX,cRect.bottom );
		dc.SetROP2( DrawMode );
		dc.SelectObject( OldPen );
		m_bDisFIDValue=FALSE;
		CMainFrame* pFrame=(CMainFrame*)AfxGetMainWnd ();
   	    pFrame->m_wndStatusBar.SetWindowText ("准备");
		return;
	}
	if(m_bCounterGlow)
	{
		CRect cRect;
		GetClientRect(cRect);
		dc.DPtoLP (&cRect);
		int DrawMode = dc.SetROP2(R2_XORPEN);
		CPen AxialPen;
		AxialPen.CreatePen(PS_SOLID, 1,0xffff00);
		CPen *OldPen = (CPen *)dc.SelectObject(&AxialPen);
		dc.MoveTo (cRect.left,m_iCounterPreX);
		dc.LineTo (cRect.right, m_iCounterPreX);
		dc.SetROP2( DrawMode );
		dc.SelectObject( OldPen );
		m_bCounterGlow=FALSE;
		CMainFrame* pFrame=(CMainFrame*)AfxGetMainWnd ();
	    pFrame->m_wndStatusBar.SetWindowText ("准备");
		return;
	}
}


void CImageManView::GetCenterPoint(LPPOINT pt)
{
	CClientDC dc(this);
	OnPrepareDC(&dc,NULL);
	CRect m_Rect;
	GetClientRect(&m_Rect);
	pt->x = m_Rect.Width() / 2;
	pt->y = m_Rect.Height() / 2;
	dc.DPtoLP(pt);
	pDoc->PPtoLP(pt);
}

void CImageManView::OnAddTrack()
{
	CPoint pt;
	GetCenterPoint(&pt);
	CRect rect(pt.x,pt.y,pt.x,pt.y);
	double sdep,edep;
	pDoc->GetDepthRangle(sdep, edep);
	CTrackObj* pObj = (CTrackObj*) new CTrackObj(rect,NULL,pDoc,sdep,edep);
	pDoc->AddObj(pObj);
	SelectObj(pObj);
}

void CImageManView::OnUpdateAddTrack(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(CDrawObj::c_ToolShape == tracktool);
}

void CImageManView::OnAddDangddir() 
{
	CPoint pt;
	GetCenterPoint(&pt);
	CRect m_Rect(pt.x,pt.y,pt.x,pt.y);
	CString Path=pDoc->m_PathName;
	Path=Path.Left(Path.ReverseFind('.'));
	CDangDdirObj* pObj = new CDangDdirObj(Path,m_Rect,NULL,pDoc);
	double sdep,edep;
	pDoc->GetDepthRangle(sdep, edep);
	pObj->SetDepth(sdep,edep);
	pDoc->AddObj(pObj);
	SelectObj(pObj);
}

void CImageManView::OnAddDevdaz() 
{
	CPoint pt;
	GetCenterPoint(&pt);
	CRect m_Rect(pt.x,pt.y,pt.x,pt.y);
	CString Path=pDoc->m_PathName;
	Path=Path.Left(Path.ReverseFind('.'));
	CDevDazObj* pObj = new CDevDazObj(Path,m_Rect,NULL,pDoc);
	double sdep,edep;
	pDoc->GetDepthRangle(sdep, edep);
	pObj->SetDepth(sdep,edep);
	pDoc->AddObj(pObj);
	SelectObj(pObj);
}

void CImageManView::OnAddAzimfreq() 
{
	CPoint pt;
	GetCenterPoint(&pt);
	CRect m_Rect(pt.x,pt.y,pt.x,pt.y);
	CString Path=pDoc->m_PathName;
	Path=Path.Left(Path.ReverseFind('.'));
	CAzimFreqObj* pObj = new CAzimFreqObj(Path,m_Rect,NULL,pDoc);
	double sdep,edep;
	pDoc->GetDepthRangle(sdep, edep);
	pObj->SetDepth(sdep,edep);
	pDoc->AddObj(pObj);
	SelectObj(pObj);
}

void CImageManView::OnAddCurve()
{
	CPoint pt;
	GetCenterPoint(&pt);
	CRect m_Rect(pt.x,pt.y,pt.x,pt.y);
	CString Path=pDoc->m_PathName;
	Path=Path.Left(Path.ReverseFind('.'));
	CCurveObj* pObj = new CCurveObj(Path,m_Rect,NULL,pDoc);
	double sdep,edep;
	pDoc->GetDepthRangle(sdep, edep);
	pObj->SetDepth(sdep,edep);
	if (pObj->Properties(this))
	{
		pDoc->AddObj(pObj);
		SelectObj(pObj);
	}                                                            
	else delete pObj;
}

void CImageManView::OnObjectPropterties() 
{
	if (m_pSelectObj != NULL)
		m_pSelectObj->OnLButtonDblClk(this, 0, CPoint(0,0));
}

void CImageManView::OnUpdateObjectPropterties(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pSelectObj != NULL);	
}

void CImageManView::OnTrackHoriFIDHorialignNone() 
{
	((CTrackObj*)m_pSelectObj)->m_HoriFIDHoriAlignMode = IDM_NONE;
}

void CImageManView::OnTrackHoriFIDAlignLeft() 
{
	((CTrackObj*)m_pSelectObj)->m_HoriFIDHoriAlignMode = IDM_LEFT;
	((CTrackObj*)m_pSelectObj)->DoHoriAlignHoriFID();
	m_pSelectObj->Invalidate(this);
}

void CImageManView::OnTrackHoriFIDAlignLeftright() 
{
	((CTrackObj*)m_pSelectObj)->m_HoriFIDHoriAlignMode = IDM_JUST;
	((CTrackObj*)m_pSelectObj)->DoHoriAlignHoriFID();
	m_pSelectObj->Invalidate(this);
}

void CImageManView::OnTrackHoriFIDAlignRight() 
{
	((CTrackObj*)m_pSelectObj)->m_HoriFIDHoriAlignMode = IDM_RIGHT;
	((CTrackObj*)m_pSelectObj)->DoHoriAlignHoriFID();
	m_pSelectObj->Invalidate(this);
}

void CImageManView::OnTrackHoriFIDVertalignNone() 
{
	((CTrackObj*)m_pSelectObj)->m_HoriFIDVertAlignMode = IDM_NONE;
}

void CImageManView::OnTrackHoriFIDAlignTop() 
{
	((CTrackObj*)m_pSelectObj)->m_HoriFIDVertAlignMode = IDM_TOP;
	((CTrackObj*)m_pSelectObj)->DoVertAlignHoriFID();
	m_pSelectObj->Invalidate(this);
}

void CImageManView::OnTrackHoriFIDAlignTopbottom() 
{
	((CTrackObj*)m_pSelectObj)->m_HoriFIDVertAlignMode = IDM_JUST;
	((CTrackObj*)m_pSelectObj)->DoVertAlignHoriFID();
	m_pSelectObj->Invalidate(this);
}

void CImageManView::OnTrackHoriFIDAlignBottom() 
{
	((CTrackObj*)m_pSelectObj)->m_HoriFIDVertAlignMode = IDM_BOTTOM;
	((CTrackObj*)m_pSelectObj)->DoVertAlignHoriFID();
	m_pSelectObj->Invalidate(this);
}

void CImageManView::OnTrackHoriFIDHoriarrangleNone() 
{
	((CTrackObj*)m_pSelectObj)->m_HoriFIDHoriArrangleMode = IDM_NONE;
}

void CImageManView::OnTrackHoriFIDArrangleLeft() 
{
	((CTrackObj*)m_pSelectObj)->m_HoriFIDHoriArrangleMode = IDM_LEFT;
	((CTrackObj*)m_pSelectObj)->DoHoriArrangleHoriFID();
	m_pSelectObj->Invalidate(this);
}

void CImageManView::OnTrackHoriFIDArrangleLeftright() 
{
	((CTrackObj*)m_pSelectObj)->m_HoriFIDHoriArrangleMode = IDM_JUST;
	((CTrackObj*)m_pSelectObj)->DoHoriArrangleHoriFID();
	m_pSelectObj->Invalidate(this);
}

void CImageManView::OnTrackHoriFIDArrangleRight() 
{
	((CTrackObj*)m_pSelectObj)->m_HoriFIDHoriArrangleMode = IDM_RIGHT;
	((CTrackObj*)m_pSelectObj)->DoHoriArrangleHoriFID();
	m_pSelectObj->Invalidate(this);
}

void CImageManView::OnTrackHoriFIDVertarrangleNone() 
{
	((CTrackObj*)m_pSelectObj)->m_HoriFIDVertArrangleMode = IDM_NONE;
}

void CImageManView::OnTrackHoriFIDArrangleTop() 
{
	((CTrackObj*)m_pSelectObj)->m_HoriFIDVertArrangleMode = IDM_TOP;
	((CTrackObj*)m_pSelectObj)->DoVertArrangleHoriFID();
	m_pSelectObj->Invalidate(this);
}

void CImageManView::OnTrackHoriFIDArrangleTopbottom() 
{
	((CTrackObj*)m_pSelectObj)->m_HoriFIDVertArrangleMode = IDM_JUST;
	((CTrackObj*)m_pSelectObj)->DoVertArrangleHoriFID();
	m_pSelectObj->Invalidate(this);
}

void CImageManView::OnTrackHoriFIDArrangleBottom() 
{
	((CTrackObj*)m_pSelectObj)->m_HoriFIDVertArrangleMode = IDM_BOTTOM;
	((CTrackObj*)m_pSelectObj)->DoVertArrangleHoriFID();
	m_pSelectObj->Invalidate(this);
}


void CImageManView::OnTrackHoriFIDSetheight() 
{
	((CTrackObj*)m_pSelectObj)->SetFIDHeight(HORDIR);	
}

void CImageManView::OnTrackHoriFIDSetwidth() 
{
	((CTrackObj*)m_pSelectObj)->SetFIDWidth(HORDIR);	
}

void CImageManView::OnUpdateTrackHoriFIDHorialignNone(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	BOOL bHoriArrangle;
	if (bSelectTrack) bHoriArrangle = ((CTrackObj*)m_pSelectObj)->m_HoriFIDHoriArrangleMode != IDM_NONE;
	pCmdUI->Enable(bSelectTrack && (!bHoriArrangle) && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_HoriFIDHoriAlignMode == IDM_NONE));
}

void CImageManView::OnUpdateTrackHoriFIDAlignLeft(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	BOOL bHoriArrangle;
	if (bSelectTrack) bHoriArrangle = ((CTrackObj*)m_pSelectObj)->m_HoriFIDHoriArrangleMode != IDM_NONE;
	pCmdUI->Enable(bSelectTrack && (!bHoriArrangle) && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_HoriFIDHoriAlignMode == IDM_LEFT));
}

void CImageManView::OnUpdateTrackHoriFIDAlignLeftright(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	BOOL bHoriArrangle;
	if (bSelectTrack) bHoriArrangle = ((CTrackObj*)m_pSelectObj)->m_HoriFIDHoriArrangleMode != IDM_NONE;
	pCmdUI->Enable(bSelectTrack && (!bHoriArrangle) && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_HoriFIDHoriAlignMode == IDM_JUST));
}

void CImageManView::OnUpdateTrackHoriFIDAlignRight(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	BOOL bHoriArrangle;
	if (bSelectTrack) bHoriArrangle = ((CTrackObj*)m_pSelectObj)->m_HoriFIDHoriArrangleMode != IDM_NONE;
	pCmdUI->Enable(bSelectTrack && (!bHoriArrangle) && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_HoriFIDHoriAlignMode == IDM_RIGHT));
}

void CImageManView::OnUpdateTrackHoriFIDVertalignNone(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	BOOL bVertArrangle;
	if (bSelectTrack) bVertArrangle = ((CTrackObj*)m_pSelectObj)->m_HoriFIDVertArrangleMode != IDM_NONE;
	pCmdUI->Enable(bSelectTrack && (!bVertArrangle) && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_HoriFIDVertAlignMode == IDM_NONE));
}

void CImageManView::OnUpdateTrackHoriFIDAlignTop(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	BOOL bVertArrangle;
	if (bSelectTrack) bVertArrangle = ((CTrackObj*)m_pSelectObj)->m_HoriFIDVertArrangleMode != IDM_NONE;
	pCmdUI->Enable(bSelectTrack && (!bVertArrangle) && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_HoriFIDVertAlignMode == IDM_TOP));
}

void CImageManView::OnUpdateTrackHoriFIDAlignTopbottom(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	BOOL bVertArrangle;
	if (bSelectTrack) bVertArrangle = ((CTrackObj*)m_pSelectObj)->m_HoriFIDVertArrangleMode != IDM_NONE;
	pCmdUI->Enable(bSelectTrack && (!bVertArrangle) && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_HoriFIDVertAlignMode == IDM_JUST));
}

void CImageManView::OnUpdateTrackHoriFIDAlignBottom(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	BOOL bVertArrangle;
	if (bSelectTrack) bVertArrangle = ((CTrackObj*)m_pSelectObj)->m_HoriFIDVertArrangleMode != IDM_NONE;
	pCmdUI->Enable(bSelectTrack && (!bVertArrangle) && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_HoriFIDVertAlignMode == IDM_BOTTOM));
}

void CImageManView::OnUpdateTrackHoriFIDHoriarrangleNone(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_HoriFIDHoriArrangleMode == IDM_NONE));
}

void CImageManView::OnUpdateTrackHoriFIDArrangleLeft(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_HoriFIDHoriArrangleMode == IDM_LEFT));
}

void CImageManView::OnUpdateTrackHoriFIDArrangleLeftright(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_HoriFIDHoriArrangleMode == IDM_JUST));
}

void CImageManView::OnUpdateTrackHoriFIDArrangleRight(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_HoriFIDHoriArrangleMode == IDM_RIGHT));
}

void CImageManView::OnUpdateTrackHoriFIDVertarrangleNone(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_HoriFIDVertArrangleMode == IDM_NONE));
}

void CImageManView::OnUpdateTrackHoriFIDArrangleTop(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_HoriFIDVertArrangleMode == IDM_TOP));
}

void CImageManView::OnUpdateTrackHoriFIDArrangleTopbottom(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_HoriFIDVertArrangleMode == IDM_JUST));
}

void CImageManView::OnUpdateTrackHoriFIDArrangleBottom(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_HoriFIDVertArrangleMode == IDM_BOTTOM));
}

void CImageManView::OnUpdateTrackHoriFIDSetheight(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	CTrackObj* pObj = (CTrackObj*) m_pSelectObj;
	pCmdUI->Enable(bSelectTrack && ((pObj->m_HoriFIDVertAlignMode != IDM_JUST) 
		|| (pObj->m_HoriFIDVertArrangleMode != IDM_NONE) 
		|| (!pObj->m_bAutoArrangle)) && (pObj->GetFIDCount(HORDIR)>0));
}

void CImageManView::OnUpdateTrackHoriFIDSetwidth(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	CTrackObj* pObj = (CTrackObj*) m_pSelectObj;
	pCmdUI->Enable(bSelectTrack && ((pObj->m_HoriFIDHoriAlignMode != IDM_JUST) 
		|| (pObj->m_HoriFIDHoriArrangleMode != IDM_NONE) 
		|| (!pObj->m_bAutoArrangle)) && (pObj->GetFIDCount(HORDIR)>0));
}

void CImageManView::OnTrackAutoarrangle() 
{
	CTrackObj* pObj = (CTrackObj*) m_pSelectObj;
	pObj->m_bAutoArrangle = !pObj->m_bAutoArrangle ;
	if (pObj->m_bAutoArrangle)  pObj->Arrangle();
	m_pSelectObj->Invalidate(this);
}

void CImageManView::OnTrackArrangle() 
{
	CTrackObj* pObj = (CTrackObj*) m_pSelectObj;
	int nSave = pObj->m_bAutoArrangle;
	pObj->m_bAutoArrangle = TRUE;
	pObj->Arrangle();
	pObj->m_bAutoArrangle = nSave;
	m_pSelectObj->Invalidate(this);
}

void CImageManView::OnUpdateTrackArrangle(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
}

void CImageManView::OnUpdateTrackAutoarrangle(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack);
	if (bSelectTrack)
	pCmdUI->SetCheck(((CTrackObj*)m_pSelectObj)->m_bAutoArrangle);
}

void CImageManView::OnTrackVertFIDAlignNone() 
{
	((CTrackObj*)m_pSelectObj)->m_VertFIDVertAlignMode = IDM_NONE;
}

void CImageManView::OnTrackVertFIDAlignTop() 
{
	((CTrackObj*)m_pSelectObj)->m_VertFIDVertAlignMode = IDM_TOP;
	((CTrackObj*)m_pSelectObj)->DoVertAlignVertFID();
	m_pSelectObj->Invalidate(this);
}

void CImageManView::OnTrackVertFIDAlignTopbottom() 
{
	((CTrackObj*)m_pSelectObj)->m_VertFIDVertAlignMode = IDM_JUST;
	((CTrackObj*)m_pSelectObj)->DoVertAlignVertFID();
	m_pSelectObj->Invalidate(this);
}

void CImageManView::OnTrackVertFIDAlignBottom() 
{
	((CTrackObj*)m_pSelectObj)->m_VertFIDVertAlignMode = IDM_BOTTOM;
	((CTrackObj*)m_pSelectObj)->DoVertAlignVertFID();
	m_pSelectObj->Invalidate(this);
}

void CImageManView::OnTrackVertFIDArrangleNone() 
{
	((CTrackObj*)m_pSelectObj)->m_VertFIDHoriArrangleMode = IDM_NONE;
}

void CImageManView::OnTrackVertFIDArrangleLeft() 
{
	((CTrackObj*)m_pSelectObj)->m_VertFIDHoriArrangleMode = IDM_LEFT;
	((CTrackObj*)m_pSelectObj)->DoHoriArrangleVertFID();
	m_pSelectObj->Invalidate(this);
}

void CImageManView::OnTrackVertFIDArrangleLeftright() 
{
	((CTrackObj*)m_pSelectObj)->m_VertFIDHoriArrangleMode = IDM_JUST;
	((CTrackObj*)m_pSelectObj)->DoHoriArrangleVertFID();
	m_pSelectObj->Invalidate(this);
}

void CImageManView::OnTrackVertFIDArrangleRight() 
{
	((CTrackObj*)m_pSelectObj)->m_VertFIDHoriArrangleMode = IDM_RIGHT;
	((CTrackObj*)m_pSelectObj)->DoHoriArrangleVertFID();
	m_pSelectObj->Invalidate(this);
}

void CImageManView::OnUpdateTrackVertFIDAlignNone(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_VertFIDVertAlignMode == IDM_NONE));
}

void CImageManView::OnUpdateTrackVertFIDAlignTop(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_VertFIDVertAlignMode == IDM_TOP));
}

void CImageManView::OnUpdateTrackVertFIDAlignTopbottom(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_VertFIDVertAlignMode == IDM_JUST));
}

void CImageManView::OnUpdateTrackVertFIDAlignBottom(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_VertFIDVertAlignMode == IDM_BOTTOM));
}

void CImageManView::OnUpdateTrackVertFIDArrangleNone(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_VertFIDHoriArrangleMode == IDM_NONE));
}

void CImageManView::OnUpdateTrackVertFIDArrangleLeft(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_VertFIDHoriArrangleMode == IDM_LEFT));
}

void CImageManView::OnUpdateTrackVertFIDArrangleLeftright(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_VertFIDHoriArrangleMode == IDM_JUST));
}

void CImageManView::OnUpdateTrackVertFIDArrangleRight(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_VertFIDHoriArrangleMode == IDM_RIGHT));
}

void CImageManView::OnTrackVertFIDSetheight() 
{
	((CTrackObj*)m_pSelectObj)->SetFIDHeight(VERDIR);	
}

void CImageManView::OnUpdateTrackVertFIDSetheight(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	CTrackObj* pObj = (CTrackObj*) m_pSelectObj;
	pCmdUI->Enable(bSelectTrack && ((pObj->m_VertFIDVertAlignMode != IDM_JUST) 
		|| (!pObj->m_bAutoArrangle)) && (pObj->GetFIDCount(VERDIR)>0));
}

void CImageManView::OnTrackVertFIDSetwidth() 
{
	((CTrackObj*)m_pSelectObj)->SetFIDWidth(VERDIR);	
}

void CImageManView::OnUpdateTrackVertFIDSetwidth(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (((CTrackObj*)m_pSelectObj)->GetFIDCount(VERDIR)>0));
}

void CImageManView::OnTrackTrackArrangleNone() 
{
	((CTrackObj*)m_pSelectObj)->m_TrackHoriArrangleMode = IDM_NONE;
}

void CImageManView::OnTrackTrackArrangleLeft() 
{
	((CTrackObj*)m_pSelectObj)->m_TrackHoriArrangleMode = IDM_LEFT;
	((CTrackObj*)m_pSelectObj)->DoHoriArrangleTrack();
	m_pSelectObj->Invalidate(this);
}

void CImageManView::OnTrackTrackArrangleLeftright() 
{
	((CTrackObj*)m_pSelectObj)->m_TrackHoriArrangleMode = IDM_JUST;
	((CTrackObj*)m_pSelectObj)->DoHoriArrangleTrack();
	m_pSelectObj->Invalidate(this);
}

void CImageManView::OnTrackTrackArrangleRight() 
{
	((CTrackObj*)m_pSelectObj)->m_TrackHoriArrangleMode = IDM_RIGHT;
	((CTrackObj*)m_pSelectObj)->DoHoriArrangleTrack();
	m_pSelectObj->Invalidate(this);
}

void CImageManView::OnUpdateTrackTrackArrangleNone(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_TrackHoriArrangleMode == IDM_NONE));
}

void CImageManView::OnUpdateTrackTrackArrangleLeft(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_TrackHoriArrangleMode == IDM_LEFT));
}

void CImageManView::OnUpdateTrackTrackArrangleLeftright(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_TrackHoriArrangleMode == IDM_JUST));
}

void CImageManView::OnUpdateTrackTrackArrangleRight(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (!((CTrackObj*)m_pSelectObj)->m_bAutoArrangle));
	pCmdUI->SetRadio(bSelectTrack && (((CTrackObj*)m_pSelectObj)->m_TrackHoriArrangleMode == IDM_RIGHT));
}

void CImageManView::OnTrackSetautoarrangle() 
{
}

void CImageManView::OnUpdateTrackSetautoarrangle(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack);
}

void CImageManView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if(m_pMeasure!=NULL)
		return;
	CClientDC dc(this);
	OnPrepareDC(&dc,NULL);
	GetParentFrame()->ActivateFrame();
	CPoint local = point;
	ScreenToClient(&local);
	dc.DPtoLP(&local);
	pDoc->PPtoLP(&local);
	if(m_bCounterGlow)
	{
		if((abs(m_iCounterPreX-local.y))<8)
		{
			CMenu menu;
			if (menu.LoadMenu(IDR_HORLINE_MENU))
			{
				CMenu* pPopup = menu.GetSubMenu(0);
				pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN,
									   point.x, point.y,
									   AfxGetMainWnd()); 
			
			}
			return;
		}
	}
	CDrawObj* pDrawObj = NULL;
	CDrawObj::m_SelectMode = none;  
	if (m_pSelectObj != NULL) pDrawObj = m_pSelectObj->ObjectAt(this,local);
	if (pDrawObj == NULL) pDrawObj = pDoc->ObjectAt(this,local);
	if (pDrawObj != NULL)
	{
		if (pDrawObj != m_pSelectObj) SelectObj(pDrawObj);
		CMenu menu;
		if (menu.LoadMenu(IDR_POPUP_OTHER_MENU))
		{
			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);
			if (!pDrawObj->IsKindOf(RUNTIME_CLASS(CTrackObj)))
			{
				pPopup->AppendMenu (MF_BYPOSITION,MF_SEPARATOR);
				if(pDrawObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj)))
				{
					pPopup->AppendMenu (MF_BYPOSITION,IDD_CURVE_DEPICO,"伸长压缩(&Z)");
					pPopup->AppendMenu (MF_BYPOSITION,IDD_CURVE_MOVEDEP,"拖拉深度移动(&U)");
					pPopup->AppendMenu (MF_BYPOSITION,MF_SEPARATOR);
					pPopup->AppendMenu (MF_BYPOSITION,IDD_CURVE_PART_FILTER,"滤波处理(&F)");
					pPopup->AppendMenu (MF_BYPOSITION,IDD_CURVE_PLUS,"公式校正(&A)");
					pPopup->AppendMenu (MF_BYPOSITION,ID_CURVE_MOUSEEDIT,"鼠标编辑(&M)");
					pPopup->AppendMenu (MF_BYPOSITION,IDD_CURVE_PLUSMOUSE,"拖拉加减校正(&L)");
					pPopup->AppendMenu (MF_BYPOSITION,MF_SEPARATOR);
					pPopup->AppendMenu (MF_BYPOSITION,ID_PARTCOPY,"曲线段拷贝(&C)");
					pPopup->AppendMenu (MF_BYPOSITION,ID_PART_PASTE,"曲线段粘贴(&P)");
					pPopup->AppendMenu (MF_BYPOSITION,MF_SEPARATOR);
					pPopup->AppendMenu (MF_BYPOSITION,ID_FREQUENCY,"直方图分析(&F)");
				}
				else if(pDrawObj->IsKindOf(RUNTIME_CLASS(CDepSect)))
				{
					pPopup->AppendMenu (MF_BYPOSITION,MF_SEPARATOR);
					pPopup->AppendMenu (MF_BYPOSITION,IDD_MODFY_DEPSEGMENT,"修改深度");
					pPopup->AppendMenu (MF_BYPOSITION,ID_ADDTO_RESULT,"添加到解释表");
					pPopup->AppendMenu (MF_BYPOSITION,MF_SEPARATOR);
					pPopup->AppendMenu (MF_BYPOSITION,ID_DRAW_POR,"孔隙度直方图");
					pPopup->AppendMenu (MF_BYPOSITION,ID_DRAW_DIAMETER,"孔洞直径直方图");
					pPopup->AppendMenu (MF_BYPOSITION,ID_DRAW_FREQUENCY,"孔洞电导率直方图");
					pPopup->AppendMenu (MF_BYPOSITION,MF_SEPARATOR);
					pPopup->AppendMenu (MF_BYPOSITION,ID_DRAW_CROSS,"孔洞电导率交会图");
					pPopup->AppendMenu (MF_BYPOSITION,MF_SEPARATOR);
					
					pPopup->AppendMenu (MF_BYPOSITION,ID_BMP_SENTTOCILP,"发送图像到剪贴板(BMP)");
				}
				else if(pDrawObj->IsKindOf(RUNTIME_CLASS(CSinPoint)))
					pPopup->AppendMenu (MF_BYPOSITION,ID_MODIFY_HANDWORK,"修改标注类型(&M)");

			}
			pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN,
							   point.x, point.y,
							   AfxGetMainWnd()); 
		}
		m_DepPoint =local.y;	
	}
	else
	{
		CMenu menu;
		if (menu.LoadMenu(IDR_POPUP_PAGE_MENU))
		{
			CMenu* pPopup = menu.GetSubMenu(0);
			pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN,
								   point.x, point.y,
								   AfxGetMainWnd()); 
		
		}
	}
}
BOOL CImageManView::Curve_Filter(CString CurveFullPath)
{
	CString f_WellName,f_Name;
	float f_Sdep,f_Edep,f_Rlev;
	CFIDIO::GetFullWellName(CurveFullPath, f_WellName.GetBuffer(512));
	f_WellName.ReleaseBuffer();	
	CFIDIO::GetFIDName(CurveFullPath, f_Name.GetBuffer(512));
	f_Name.ReleaseBuffer();
	
	float fdat[500];
	CString msg;char s[255];
	strcpy(s,f_WellName);
	strcat(s,".");
	strcat(s,f_Name); 
	CFIDIO* pFIDtemp = NULL;
	pFIDtemp = new CFIDIO(s);
	pFIDtemp->Open();
	int dim=pFIDtemp->GetDimenSion();
	FIDCONTENT* Content = pFIDtemp->GetContent(dim);
	if((Content != NULL)&&(dim==2)&&(Content->type == REPR_FLOAT))
	{
		pFIDtemp->Copy(f_WellName+".curveFiltertemp");
		f_Sdep = pFIDtemp->GetStartDepth();
		f_Edep = pFIDtemp->GetEndDepth();
		f_Rlev=pFIDtemp->GetDepthLevel();
		delete pFIDtemp;
		if(pDoc->GetLineObject ()!=NULL)
		{
			int DaltaY=pDoc->GetLeftTrackObj ()->GetClientRect ().top -pDoc->m_TopSpace;
			float Dep=(float)pDoc->YCoorToDepth(pDoc->GetLineObject ()->m_Rect.top-DaltaY);
			Dep=((int)(Dep/f_Rlev))*f_Rlev;
			f_Sdep =Dep;
			Dep=(float)pDoc->YCoorToDepth(pDoc->GetLineObject ()->m_Rect.bottom -DaltaY);
			Dep=((int)(Dep/f_Rlev))*f_Rlev;
			f_Edep=Dep;
		}
  	    
		CFIDIO* pFIDW = NULL;
		pFIDW = new CFIDIO(s);
	    if(!pFIDW->Open())
		{
			AfxMessageBox("打开数据文件失败    ");
			delete pFIDW;return FALSE;
		}
		
		CFIDIO* pFIDR = NULL;
		strcpy(s,f_WellName);
		strcat(s,".curveFiltertemp"); 
		pFIDR = new CFIDIO(s);
	    if(!pFIDR->Open()) 
		{
			AfxMessageBox("打开缓冲区失败    ");
			delete pFIDW;delete pFIDR;return FALSE;
		}
		pFIDR->SeekToDepth (f_Sdep);
		pFIDW->SeekToDepth (f_Sdep);
		long AllRecord=(long)((f_Edep-f_Sdep)/f_Rlev)+1;
		pFIDR->Read(2,5,fdat);
		pFIDR->Next ();pFIDR->Next ();pFIDR->Next ();
		BeginWaitCursor();
		long i=5;float data=fdat[4];
		while(i<AllRecord)
		{		
			if((fdat[4]!=-9999.f)&&(fdat[3]!=-9999.f)&&	
				(fdat[2]!=-9999.f)&&(fdat[1]!=-9999.f)&&(fdat[0]!=-9999.f))	
			{
				//五点钟形滤波         0.11(Ti-2+Ti+2)+0.24(Ti-1+Ti+1)+0.3Ti
				data=0.11f*(fdat[0]+fdat[4])+0.24f*(fdat[1]+fdat[3])+0.3f*fdat[2];
				pFIDW->Write(2,1,&data);
			}
			pFIDW->Next();
			for(int j=0;j<4;j++) fdat[j]=fdat[j+1];
			pFIDR->Read(2,1,&data);fdat[4]=data;
			i++;pFIDR->Next();
		}		
		pFIDR->Delete();delete pFIDR;delete pFIDW;
		EndWaitCursor();
		return TRUE;
	}
	else 
	{
		Content = pFIDtemp->GetContent("t");
		if((Content != NULL)&&(dim==3))
		{
			BeginWaitCursor();
			pFIDtemp->Copy(f_WellName+".curveFiltertemp");
			f_Sdep = pFIDtemp->GetStartDepth();
			f_Edep = pFIDtemp->GetEndDepth();
			f_Rlev=pFIDtemp->GetDepthLevel();
			delete pFIDtemp;
			if(pDoc->GetLineObject ()!=NULL)
			{
				int DaltaY=pDoc->GetLeftTrackObj ()->GetClientRect ().top -pDoc->m_TopSpace;
				float Dep=(float)pDoc->YCoorToDepth(pDoc->GetLineObject ()->m_Rect.top-DaltaY);
				Dep=((int)(Dep/f_Rlev))*f_Rlev;
				f_Sdep =Dep;
				Dep=(float)pDoc->YCoorToDepth(pDoc->GetLineObject ()->m_Rect.bottom -DaltaY);
				Dep=((int)(Dep/f_Rlev))*f_Rlev;
				f_Edep=Dep;
			}
  			EndWaitCursor();
			CFIDIO* pFIDW = NULL;
			pFIDW = new CFIDIO(s);
			if(!pFIDW->Open())
			{
				AfxMessageBox("打开数据文件失败    ");
				delete pFIDW;return FALSE;
			}
			
			CFIDIO* pFIDR = NULL;
			strcpy(s,f_WellName);
			strcat(s,".curveFiltertemp"); 
			pFIDR = new CFIDIO(s);
			if(!pFIDR->Open()) 
			{
				AfxMessageBox("打开缓冲区失败    ");
				delete pFIDW;delete pFIDR;return FALSE;
			}
			pFIDR->SeekToDepth (f_Sdep);
			pFIDW->SeekToDepth (f_Sdep);
			int nps=Content->nps;
			if(nps>5)
			{
				//定义进度指示器
				CGuage* pGuage = new CGuage(f_Sdep,f_Edep);
				pGuage->Create(CWnd::GetActiveWindow());
				pGuage->ShowWindow(SW_SHOW);
				short* iwf=new short[nps];			
				int i;
				while(f_Sdep<f_Edep)
				{
					pFIDR->Read(dim,nps,iwf);	
					//五点钟形滤波         0.11(Ti-2+Ti+2)+0.24(Ti-1+Ti+1)+0.3Ti
					for(i=2;i<nps-10;i++)
						iwf[i]=(short)(0.11f*(iwf[i-2]+iwf[i+2])+0.24f*(iwf[i-1]+iwf[i+1])+0.3f*iwf[i]);	
					pFIDW->Write(dim,nps,iwf);
					pFIDW->Next();
					f_Sdep+=f_Rlev;
					pFIDR->Next();
					pGuage->SetDepth(f_Sdep);
					if (pGuage->CheckCancelButton()) 
						break;
				}
				if(iwf!=NULL) delete iwf;
				delete pGuage;
			}	
			pFIDR->Delete();delete pFIDR;delete pFIDW;					
			return TRUE;
		}
		else
		{
			delete pFIDtemp;return FALSE;
		}
	}
}
void CImageManView::OnCurvePartFilter() 
{
	// TODO: Add your command handler code here
	if(m_pSelectObj != NULL) 
	{
		if(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CArrayWaveObj)))
		{
			CArrayWaveObj* pObj=(CArrayWaveObj*)m_pSelectObj;
			Curve_Filter(pObj->m_strPathName);
			return;
		}
		CCurveObj* pObj=(CCurveObj*)m_pSelectObj;
		CString WellName,CurveName;		
		CFIDIO::GetFullWellName(pObj->m_strPathName, WellName.GetBuffer(512));
		WellName.ReleaseBuffer();
		CFIDIO::GetFIDName(pObj->m_strPathName, CurveName.GetBuffer(512));
		CurveName.ReleaseBuffer();
		BeginWaitCursor();
		CFIDIO* pFIDtemp = NULL;
		pFIDtemp = new CFIDIO(WellName + "." + CurveName);
		if(!pFIDtemp->Open())
		{
			MessageBox("  打开 "+WellName + "." +
				CurveName+" 文件失败!  ","错误提示...",MB_OK);
			delete pFIDtemp;EndWaitCursor();return;
		}
		char ss[20];	
		sprintf(ss,".#$#%d",m_InitOprationNum+1);
		pFIDtemp->Copy(m_UnDoFilePath+ss);	
		delete pFIDtemp;
		EndWaitCursor();
		if(Curve_Filter(pObj->m_strPathName))
		{
			m_InitOprationNum++;
			m_UnDoName.Add(WellName+"."+CurveName);
			m_UnDoNameCopy.Add(m_UnDoFilePath+ss);
			pDoc->OnReFresh ();
		}
	}
}

void CImageManView::OnUpdateCurvePartFilter(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	bool TLD=FALSE;
	if (m_pSelectObj != NULL)
		if((m_pSelectObj->IsKindOf(RUNTIME_CLASS(CCurveObj)))
			||(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CArrayWaveObj))))
			TLD=TRUE;
	pCmdUI->Enable(TLD);
}
void CImageManView::OnCurvePlus() 
{
	if(m_pSelectObj != NULL)
	{
		CCurveObj* pObj=(CCurveObj*)m_pSelectObj;
		CString WellName,CurveName;		
		CFIDIO::GetFullWellName(pObj->m_strPathName, WellName.GetBuffer(512));
		WellName.ReleaseBuffer();
		CFIDIO::GetFIDName(pObj->m_strPathName, CurveName.GetBuffer(512));
		CurveName.ReleaseBuffer();
		BeginWaitCursor();
		CFIDIO* pFIDtemp = NULL;
		pFIDtemp = new CFIDIO(WellName + "." + CurveName);
		if(!pFIDtemp->Open())
		{
			MessageBox("  打开 "+WellName + "." +
				CurveName+" 文件失败!  ","错误提示...",MB_OK);
			delete pFIDtemp;EndWaitCursor();return;
		}
		char ss[20];	
		sprintf(ss,".#$#%d",m_InitOprationNum+1);
		pFIDtemp->Copy(m_UnDoFilePath+ss);	
		delete pFIDtemp;
		EndWaitCursor();
		BOOL IniDep=FALSE;float SDepth=0.f,EDepth=0.f;
		if(pDoc->GetLineObject ()!=NULL)
		{
			IniDep=TRUE;
			int DaltaY=pDoc->GetLeftTrackObj ()->GetClientRect ().top -pDoc->m_TopSpace;
			SDepth=(float)pDoc->YCoorToDepth(pDoc->GetLineObject ()->m_Rect.top-DaltaY);
			EDepth=(float)pDoc->YCoorToDepth(pDoc->GetLineObject ()->m_Rect.bottom -DaltaY);
		}
		CCurvePlus Plusdlg(pObj);
		Plusdlg.m_IniDep=IniDep;
		Plusdlg.m_Edep0 =EDepth;
		Plusdlg.m_Sdep0 =SDepth;
		if(Plusdlg.DoModal()==IDOK)
		{
			m_InitOprationNum++;
			m_UnDoName.Add(WellName+"."+CurveName);
			m_UnDoNameCopy.Add(m_UnDoFilePath+ss);
			pDoc->OnReFresh ();
		}
	}
}
void CImageManView::OnUpdateCurvePlus(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	bool TLD=FALSE;
	if (m_pSelectObj != NULL)
		if(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CCurveObj)))
			TLD=TRUE;
	pCmdUI->Enable(TLD);
}
void CImageManView::OnPartCopy() 
{
	CCurveObj* pObj=(CCurveObj*)m_pSelectObj;
	double sdep=pDoc->GetLeftTrackObj ()->YCoorToDepth(pDoc->GetLineObject ()->m_Rect.top);
	double edep=pDoc->GetLeftTrackObj ()->YCoorToDepth(pDoc->GetLineObject ()->m_Rect.bottom);
	
	CPoint* m_PartPoints=NULL;
	int ptNum = pObj->GetPoint(sdep,edep,m_PartPoints);	
	if(m_PartPoints==NULL) return;

	CPoint point;
	point.x=m_PartPoints->x;
	point.y=m_PartPoints->y;
	CRect rect(point.x,point.y,point.x,point.y);

	CDrawObj::c_ToolShape = foldtool;
	CFoldObj* FoldObj = (CFoldObj*) new CFoldObj(rect,NULL,pDoc);
	if(FoldObj != NULL)
	{
		pDoc->AddObj(FoldObj);
		SelectObj(FoldObj);
		FoldObj->OnLButtonDown(this,1,point); 
		CPoint tmpPt;
		for(int i=1; i<ptNum; i++)
		{
			tmpPt.x=m_PartPoints[i].x;
			tmpPt.y=m_PartPoints[i].y;

			if(pDoc->GetRect().PtInRect(tmpPt))	
				FoldObj->AddPartPoint(tmpPt);
		}
	}
	FoldObj->DrawCopy(this,point);
	if(m_PartPoints!=NULL) 
		delete m_PartPoints;
	CDrawObj::c_ToolShape = nonetool;
}

void CImageManView::OnUpdatePartCopy(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	bool TLD=FALSE;
	if (m_pSelectObj != NULL)
		if((m_pSelectObj->IsKindOf(RUNTIME_CLASS(CCurveObj)))
			&&(pDoc->GetLineObject()!=NULL))
			TLD=TRUE;
	pCmdUI->Enable(TLD);
}

void CImageManView::OnPartPaste() 
{
	CFoldObj* FoldObj=pDoc->GetFoldObject();
	CCurveObj* CurveObj=(CCurveObj*)m_pSelectObj;
	if((FoldObj != NULL)&&(CurveObj!=NULL))
	{
		int PointNum=FoldObj->m_nPoints;
		if(PointNum >2)
		{
			CString WellName,CurveName;		
			CFIDIO::GetFullWellName(CurveObj->m_strPathName , WellName.GetBuffer(512));
			WellName.ReleaseBuffer();			
			CFIDIO::GetFIDName(CurveObj->m_strPathName , CurveName.GetBuffer(512));
			CurveName.ReleaseBuffer();

			BeginWaitCursor();
			CFIDIO* pFIDIO = NULL;
			pFIDIO = new CFIDIO(WellName + "." + CurveName);
			if(!pFIDIO->Open())
			{
				MessageBox("打开 "+WellName + "." +
					CurveName+" 文件失败!","错误提示...",MB_OK);
				delete pFIDIO;EndWaitCursor();return;
			}
			float Rlev=pFIDIO->GetDepthLevel();
			if(pFIDIO->GetDimenSion()>2)
			{
				delete pFIDIO;AfxMessageBox("  只能对常规曲线进行编辑  ");
				return;
			}
			char ss[20];
			float Sdep0=0,Edep0=0;
			sprintf(ss,".#$#%d",m_InitOprationNum+1);
			if((pFIDIO->Copy(m_UnDoFilePath+ss))!=-1)
			{
				m_InitOprationNum++;
				m_UnDoName.Add(WellName+"."+CurveName);
				m_UnDoNameCopy.Add(m_UnDoFilePath+ss);	
			
				float fbuf,Svalue,Evalue,Kvalue;int i,j,k;
				float Sdep,Edep,dep;				
				int RectLeftX=CurveObj->m_Rect.left;
				float LeftValue=(float)CurveObj->m_LeftVal1;
				float RightValue=(float)CurveObj->m_RightVal1;
				int RectWide=CurveObj->m_Rect.Width();
				unsigned short CalType=CurveObj->m_CalType;
	
				Sdep=(float)CurveObj->YCoorToDepth (FoldObj->m_Points [0].y);
				Svalue = LeftValue+( RightValue-LeftValue )*(FoldObj->m_Points [0].x - RectLeftX)/RectWide;
				if (CalType == 1) 
					 Svalue= (float) pow(10.,Svalue*1.); 
			
				/////////////
				pFIDIO->SeekToDepth (Sdep);
				pFIDIO->Write(2,1,&Svalue);
				Sdep0=Sdep;
				for(i=1;i<PointNum;i++)
				{
					Edep=(float)CurveObj->YCoorToDepth (FoldObj->m_Points [i].y);			
					Evalue =LeftValue+( RightValue-LeftValue )*(FoldObj->m_Points [i].x - RectLeftX)/RectWide;
					if (CalType == 1) 
						Evalue = (float) pow(10.,Evalue*1.); 
					k=(int)((Edep-Sdep)/Rlev);
					if(k>0)
					{
						Kvalue=(Evalue-Svalue)/k;
						for(j=0;j<=k;j++)
						{
							dep=Sdep+Rlev*j;
							pFIDIO->SeekToDepth (dep);
							fbuf=Svalue+j*Kvalue;
							pFIDIO->Write(2,1,&fbuf);
						}
					}
					if(k>=0)
					{
						pFIDIO->SeekToDepth (Edep);
						pFIDIO->Write(2,1,&Evalue);
						Sdep=Edep;Svalue=Evalue;
					}
					if(Sdep<Sdep0) Sdep0=Sdep;
					if(Edep>Edep0) Edep0=Edep;
				}
			}
			delete pFIDIO;				
			EndWaitCursor();OnDrawSelect();	
		}
		SelectObj(CurveObj);
	}
}

void CImageManView::OnUpdatePartPaste(CCmdUI* pCmdUI) 
{
	bool TLD=FALSE;
	if(m_pSelectObj != NULL)
		if(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CCurveObj))
			&&pDoc->GetFoldObject()!=NULL)
			TLD=TRUE;
	pCmdUI->Enable(TLD);
}

void CImageManView::OnViewTrackSetwidth() 
{
	pDoc->SetChildTrackWidth();
}

void CImageManView::OnUpdateViewTrackSetwidth(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(pDoc->GetTrackCount()>0);
}

void CImageManView::OnViewTrackTitleShow() 
{
	m_bTrackTitleShow=!m_bTrackTitleShow;
	pDoc->DrawChildTrackTitle(m_bTrackTitleShow);
}

void CImageManView::OnUpdateViewTrackTitleShow(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(pDoc->GetTrackCount()>0);
	pCmdUI->SetCheck (m_bTrackTitleShow);
}

void CImageManView::OnViewTrackHeadSetheightAuto() 
{
	BOOL bAuto = !pDoc->m_bChildTrackHeadHeightAuto;
	pDoc->SetChildTrackHeadHeightAuto(bAuto);
}

void CImageManView::OnUpdateViewTrackHeadSetheightAuto(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(pDoc->GetTrackCount()>0);
	pCmdUI->SetCheck(pDoc->m_bChildTrackHeadHeightAuto);
}

void CImageManView::OnViewTrackHeadSetheightMax() 
{
 	BOOL bMax = !pDoc->m_bChildTrackHeadAlignMax;
	pDoc->SetChildTrackHeadHeightAlignMax(bMax);
}

void CImageManView::OnUpdateViewTrackHeadSetheightMax(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(pDoc->GetTrackCount()>0);
	pCmdUI->SetCheck(pDoc->m_bChildTrackHeadAlignMax);
}

void CImageManView::OnViewTrackTitleSetheight() 
{
	pDoc->SetChildTrackTitleHeight();
}

void CImageManView::OnUpdateViewTrackTitleSetheight(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(pDoc->GetTrackCount()>0);
}

void CImageManView::OnTrackTrackSetwidth() 
{
	((CTrackObj*)m_pSelectObj)->SetChildTrackWidth();
}

void CImageManView::OnUpdateTrackTrackSetwidth(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (((CTrackObj*)m_pSelectObj)->GetTrackCount()>0));
}

void CImageManView::OnTrackTrackTitleSetheight() 
{
	((CTrackObj*)m_pSelectObj)->SetChildTrackTitleHeight();
}

void CImageManView::OnUpdateTrackTrackTitleSetheight(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (((CTrackObj*)m_pSelectObj)->GetTrackCount()>0));
}

void CImageManView::OnTrackTrackTitleShow() 
{
	((CTrackObj*)m_pSelectObj)->DrawChildTrackTitle(TRUE);
}

void CImageManView::OnUpdateTrackTrackTitleShow(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (((CTrackObj*)m_pSelectObj)->GetTrackCount()>0));
}

void CImageManView::OnTrackTrcakTitleHide() 
{
	((CTrackObj*)m_pSelectObj)->DrawChildTrackTitle(FALSE);
}

void CImageManView::OnUpdateTrackTrcakTitleHide(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (((CTrackObj*)m_pSelectObj)->GetTrackCount()>0));
}

void CImageManView::OnTrackTrackHeadSetheightAuto() 
{
	BOOL bAuto = !((CTrackObj*)m_pSelectObj)->m_bChildTrackHeadHeightAuto;
	((CTrackObj*)m_pSelectObj)->SetChildTrackHeadHeightAuto(bAuto);
}

void CImageManView::OnUpdateTrackTrackHeadSetheightAuto(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (((CTrackObj*)m_pSelectObj)->GetTrackCount()>0));
	if (bSelectTrack) pCmdUI->SetCheck(((CTrackObj*)m_pSelectObj)->m_bChildTrackHeadHeightAuto);
}

void CImageManView::OnTrackTrackHeadSetheightMax() 
{
 	BOOL bMax = !((CTrackObj*)m_pSelectObj)->m_bChildTrackHeadAlignMax;
	((CTrackObj*)m_pSelectObj)->SetChildTrackHeadHeightAlignMax(bMax);
}

void CImageManView::OnUpdateTrackTrackHeadSetheightMax(CCmdUI* pCmdUI) 
{
	BOOL bSelectTrack = (m_pSelectObj != NULL) && (m_pSelectObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
	pCmdUI->Enable(bSelectTrack && (((CTrackObj*)m_pSelectObj)->GetTrackCount()>0));
	if (bSelectTrack) pCmdUI->SetCheck(((CTrackObj*)m_pSelectObj)->m_bChildTrackHeadAlignMax);
}

void CImageManView::SetScrollSizes(int nMapMode, SIZE sizeTotal)
{
	int nOldMapMode = m_nMapMode;
	m_nMapMode = nMapMode;
	m_totalLog = sizeTotal;
	if(pDoc->m_ScaleNo==7)
	{
		m_totalLog.cx=m_totalLog.cx*100/96;
		m_totalLog.cy=m_totalLog.cy*100/96;
	}
	//BLOCK: convert logical coordinate space to device coordinates
	{
		CWindowDC dc(NULL);
		ASSERT(m_nMapMode > 0);
		dc.SetMapMode(m_nMapMode);
		// total size
		m_totalDev = m_totalLog;
		dc.LPtoDP(&m_totalDev);
	} // release DC here
	// now adjust device speFIDic sizes
	m_pageDev.cx = m_totalDev.cx / 30;
	m_pageDev.cy = m_totalDev.cy / 30;
	m_lineDev.cx = m_pageDev.cx / 30;
	m_lineDev.cy = m_pageDev.cy / 30;
	if(m_hWnd != NULL)
	{
		// window has been created, invalidate now
		UpdateBars();
		if (nOldMapMode != m_nMapMode)
			Invalidate(TRUE);
	}
	int min,max;
	GetScrollRange(SB_VERT,&min,&max); 
	return;
}

void CImageManView::OnPreRangvalue() 
{
	// TODO: Add your command handler code here
	c_ToolPro=DepRangeDraw;
	CDrawObj::c_ToolShape = depsecttool;
}

void CImageManView::OnEditUndo() 
{
	// TODO: Add your command handler code here	
	if(m_InitOprationNum!=m_PreOprationNum)
	{
		m_PreOprationNum=m_InitOprationNum;
		m_UnDo=m_InitOprationNum;m_ReDo=0;
	}
	CString WellName;		
	CFIDIO::GetFullWellName(m_UnDoName[m_UnDo-1], WellName.GetBuffer(512));
	WellName.ReleaseBuffer();
	CFIDIO* pFID = NULL;
	pFID = new CFIDIO(m_UnDoName[m_UnDo-1]);
	if(pFID->Open())
	{
		if(pFID->Rename (m_UnDoFilePath+".%%%%")!=-1)
		{
			delete pFID;
			CFIDIO* pFIDtemp = NULL;
			pFIDtemp = new CFIDIO(m_UnDoNameCopy[m_UnDo-1]);
			if(pFIDtemp->Open())
			{
				if(pFIDtemp->Rename (m_UnDoName[m_UnDo-1])==-1)
					if(pFIDtemp->Copy (m_UnDoName[m_UnDo-1])!=-1)
						pFIDtemp->Delete ();
				pDoc->OnReFresh ();
			}
			delete pFIDtemp;
		
			CFIDIO* pFIDio = NULL;
			pFIDio = new CFIDIO(m_UnDoFilePath+".%%%%");
			if(pFIDio->Open())
				if(pFIDio->Rename (m_UnDoNameCopy[m_UnDo-1])==-1)
					if(pFIDio->Copy (m_UnDoNameCopy[m_UnDo-1])!=-1)
						pFIDio->Delete ();
			delete pFIDio;	
			m_UnDo--;m_ReDo++;		
		}
		else	delete pFID;
	}
	else	delete pFID;
}

void CImageManView::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pDoc->m_UnDo=((m_UnDo>0)||(m_InitOprationNum!=m_PreOprationNum));
	pCmdUI->Enable (pDoc->m_UnDo);
}

void CImageManView::OnEditRedo() 
{
	// TODO: Add your command handler code here
	CString WellName;		
	CFIDIO::GetFullWellName(m_UnDoName[m_UnDo], WellName.GetBuffer(512));
	WellName.ReleaseBuffer();
	CFIDIO* pFID = NULL;
	pFID = new CFIDIO(m_UnDoNameCopy[m_UnDo]);
	if(pFID->Open())
	{
		if(pFID->Rename (WellName+".%%%%")!=-1)
		{
			delete pFID;
			CFIDIO* pFIDtemp = NULL;
			pFIDtemp = new CFIDIO(m_UnDoName[m_UnDo]);
			if(pFIDtemp->Open())
				if(pFIDtemp->Rename (m_UnDoNameCopy[m_UnDo])!=-1)
					pDoc->OnReFresh ();
			delete pFIDtemp;
			
			CFIDIO* pFIDio = NULL;
			pFIDio = new CFIDIO(WellName+".%%%%");
			if(pFIDio->Open())
				pFIDio->Rename (m_UnDoName[m_UnDo]);
			delete pFIDio;
			m_UnDo++;m_ReDo--;
		}
		else	delete pFID;
	}
	else	delete pFID;
}

void CImageManView::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable (m_ReDo>0);
}

void CImageManView::OnInsertpoint() 
{
	// TODO: Add your command handler code here
	CCurveObj* pObj=(CCurveObj*)m_pSelectObj;
	if (pObj != NULL)
	{	
		CString WellName,CurveName;		
		CFIDIO::GetFullWellName(pObj->m_strPathName, WellName.GetBuffer(512));
		WellName.ReleaseBuffer();	
		CFIDIO::GetFIDName(pObj->m_strPathName, CurveName.GetBuffer(512));
		CurveName.ReleaseBuffer();
		CFIDIO* pFID0 = NULL;
		CString FullCurveName=WellName + "." + CurveName;
		pFID0 = new CFIDIO(FullCurveName);
		if(!pFID0->Open())
		{
			MessageBox("       打开 "+ CurveName+
						" 曲线失败!        ","错误提示...",MB_OK);
			delete pFID0;return;
		}
		float Sdep = pFID0->GetStartDepth();
		float Edep = pFID0->GetEndDepth();
		float Rlev=pFID0->GetDepthLevel();
		delete pFID0;
	
		char DepRenge[512];
		sprintf(DepRenge,"井段：%.3f--%.3f",Sdep,Edep);
		CInputBox InBox;
		InBox.m_CurveName ="曲线："+CurveName;
		InBox.m_DepRange =DepRenge;
		InBox.m_Value=Rlev;
		InBox.m_Title ="采样步长";
		if(InBox.DoModal ()==IDOK)
		{
			float NewRlev=InBox.m_Value;
			if(fabs(NewRlev-Rlev)>0.000125)
			{
				BeginWaitCursor();
				int i;
				char ss[20];
				m_InitOprationNum++;
				sprintf(ss,".#$#%d",m_InitOprationNum);
				CFIDIO* pFIDtemp = NULL;
				pFIDtemp = new CFIDIO(FullCurveName);
				pFIDtemp->Open();
				int dim=pFIDtemp->GetDimenSion();
				pFIDtemp->Copy(m_UnDoFilePath+ss);
				m_UnDoName.Add(FullCurveName);
				m_UnDoNameCopy.Add(m_UnDoFilePath+ss);

				FIDCONTENT* Content;
				FIDCONTENT ContentW[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
											{"","none",REPR_FLOAT,4,1,1,0.f,100.f,0.f},
											{"","none",REPR_FLOAT,4,1,1,0.f,100.f,0.f}};
				ContentW[0].min = Sdep;
				ContentW[0].max = Edep;
				ContentW[0].rlev =NewRlev ;
				for(i=1;i<dim;i++)
				{
					Content = pFIDtemp->GetContent(i+1);
					strcpy(ContentW[i].name,Content->name);
					strcpy(ContentW[i].unit,Content->unit);
					ContentW[i].type=Content->type;
					ContentW[i].length=Content->length;
					ContentW[i].nps=Content->nps;
					ContentW[i].npw=Content->npw;
					ContentW[i].min=Content->min;
					ContentW[i].max=Content->max;
					ContentW[i].rlev=Content->rlev;
				}
				int nps = Content->nps;
				short* ibuf = NULL;
				float* fbuf = NULL;
				char* cbuf = NULL;
				if (Content != NULL)
				{
					if (Content->type == REPR_FLOAT) fbuf = new float[nps];
					else if (Content->type == REPR_CHAR) cbuf = new char[nps];
					else ibuf = new short[nps];
				}
				pFIDtemp->Delete ();
				delete pFIDtemp;

				FIDINDEX IndexW;
				IndexW.dimension = dim;
				ContentW[0].rlev =NewRlev ;
				IndexW.Contentlist = ContentW;
				char adi[] = "==END==";
  				IndexW.adi = adi;
				CFIDIO* pFIDW = NULL;
				pFIDW= new CFIDIO(FullCurveName,&IndexW);
				pFIDW->Open();
				CFIDIO* pFIDR = NULL;
				pFIDR = new CFIDIO(m_UnDoFilePath+ss);
				if(!pFIDR->Open())
				{
					MessageBox("打开 "+m_UnDoFilePath+ss+" 文件失败!","错误提示...",MB_OK);
					delete pFIDR;return;
				}
				float m_Dep=Sdep;
				Edep+=NewRlev;
				unsigned k0,k;
				k0=k=0;
				while(Sdep+k0*Rlev<=Edep)
				{
					if (Content->type == REPR_FLOAT)
							pFIDR->Read(dim,nps,fbuf);
					else if (Content->type == REPR_CHAR)
							pFIDR->Read(dim,nps,cbuf);
					else 
							pFIDR->Read(dim,nps,ibuf);
					pFIDR->Next();
					k0++;
					while(m_Dep+k*NewRlev<Sdep+k0*Rlev)
					{
						if (Content->type == REPR_FLOAT)
							pFIDW->Write(dim,nps,fbuf);
						else if (Content->type == REPR_CHAR)
							pFIDW->Write(dim,nps,cbuf);
						else 
							pFIDW->Write(dim,nps,ibuf);
						pFIDW->Next();
						k++;
					}
				}
				EndWaitCursor();
				delete pFIDR;delete pFIDW;
				if (cbuf != NULL)	delete cbuf;
				if (ibuf != NULL)	delete ibuf;
				if (fbuf != NULL)	delete fbuf;
				pDoc->OnReFresh ();
			}
		}
	}
}

void CImageManView::OnUpdateInsertpoint(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	bool TLD=FALSE;
	if (m_pSelectObj != NULL)
		if(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CCurveObj)))
			TLD=TRUE;
	pCmdUI->Enable(TLD&&(c_ToolPro == NoneTool));
	pCmdUI->SetCheck (c_ToolPro==InsertPoint);
}

void CImageManView::OnCurveDisvalue() 
{
	// TODO: Add your command handler code here
	CFIDDrawObj* pObj=(CFIDDrawObj*)m_pSelectObj;
	if(pObj!=NULL)
	{
		m_bDisFIDValue=TRUE;
		CRect rect = pObj->m_HeadRect;
		m_iDisFIDLeftX=rect.left ;
		m_fDisFIDLeftValue=pObj->m_LeftVal ;
		m_fDisFIDRatio=(pObj->m_RightVal -pObj->m_LeftVal)/rect.Width ();
		CClientDC dc(this);
		OnPrepareDC(&dc,NULL);
		CRect cRect;
		GetClientRect(cRect);
		dc.DPtoLP (&cRect);
		m_iDisFIDIntiX=m_iDisFIDPreX=m_iDisFIDLeftX+rect.Width ()/3;
		int DrawMode = dc.SetROP2(R2_XORPEN);
		CPen AxialPen;
		AxialPen.CreatePen(PS_SOLID, 1,0xffff00);
		CPen *OldPen = (CPen *)dc.SelectObject(&AxialPen);
		dc.MoveTo (m_iDisFIDPreX,cRect.top );
		dc.LineTo (m_iDisFIDPreX,cRect.bottom );
		dc.SetROP2( DrawMode );
		dc.SelectObject( OldPen );
		char ss[512];
		CFIDIO::GetFIDName(pObj ->m_strPathName ,ss);
		m_DisFIDName=pObj->m_AliasName+"("+ss+")";	
		if(pObj->IsKindOf (RUNTIME_CLASS(CCurveObj)))
		{
			CCurveObj* pCurveObj=(CCurveObj*) pObj;
			m_DisFIDCalType=pCurveObj->m_CalType ;
			m_fDisFIDLeftValue=(float)pCurveObj->m_LeftVal1 ;
			m_fDisFIDRatio=(float)((pCurveObj->m_RightVal1 -pCurveObj->m_LeftVal1)/rect.Width ());
		}
		else
			m_DisFIDCalType=0;
		c_ToolPro=DisValue;
		CString str;
		float LineValue=m_fDisFIDLeftValue+(m_iDisFIDPreX-m_iDisFIDLeftX)*m_fDisFIDRatio;
		if(m_DisFIDCalType==1)
			LineValue=(float)pow(10.,LineValue*1.);
		str.Format ("  %.4f",LineValue);
		str=m_DisFIDName+str;
		CMainFrame* pFrame=(CMainFrame*)AfxGetMainWnd ();
		pFrame->m_wndStatusBar.SetWindowText (str);
	}
}

void CImageManView::OnUpdateCurveDisvalue(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	bool TLD=FALSE;
	if (m_pSelectObj != NULL)
		if((m_pSelectObj->IsKindOf(RUNTIME_CLASS(CCurveObj)))||
			(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CDevDazObj))))
			TLD=TRUE;
	pCmdUI->Enable(TLD&&(c_ToolPro == NoneTool));
	pCmdUI->SetCheck (c_ToolPro==DisValue);
}

void CImageManView::OnCurveMovedep() 
{
	// TODO: Add your command handler code here
	CImageManApp* pApp=(CImageManApp*) AfxGetApp(); 
	pApp->m_MoveDep=TRUE;
	c_ToolPro=MoveDep;
}

void CImageManView::OnUpdateCurveMovedep(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	bool TLD=FALSE;
	if (m_pSelectObj != NULL)
		if(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CCurveObj)))
			TLD=TRUE;
	pCmdUI->Enable(TLD&&(c_ToolPro == NoneTool));
	pCmdUI->SetCheck (c_ToolPro==MoveDep);
}


void CImageManView::OnCurvePlusmouse() 
{
	// TODO: Add your command handler code here
		CImageManApp* pApp=(CImageManApp*) AfxGetApp(); 
		pApp->m_PlusValue=TRUE;
		c_ToolPro=PlusValue;
}

void CImageManView::OnUpdateCurvePlusmouse(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	bool TLD=FALSE;
	if (m_pSelectObj != NULL)
		if(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CCurveObj)))
		{
			CCurveObj* pObj=(CCurveObj*) m_pSelectObj;
			if(pObj->m_CalType != 1)
			TLD=TRUE;
		}
	pCmdUI->Enable(TLD&&(c_ToolPro == NoneTool));	
}

void CImageManView::OnModfyDepsegment() 
{
	// TODO: Add your command handler code here
	CPlotDepthDialog ModifyDepSegment;
//	ModifyDepSegment.m_DepPromp="处理井段深度";
	if(pDoc->GetLineObject ()!=NULL)
	{
		int DaltaY=pDoc->GetLeftTrackObj ()->GetClientRect ().top -pDoc->m_TopSpace;
		float Dep=(float)pDoc->YCoorToDepth(pDoc->GetLineObject ()->m_Rect.top-DaltaY);
		ModifyDepSegment.m_PlotSdep  =((int)(Dep*10+0.5))/10.f;
		Dep=(float)pDoc->YCoorToDepth(pDoc->GetLineObject ()->m_Rect.bottom -DaltaY);
		ModifyDepSegment.m_PlotEdep =((int)(Dep*10+0.5))/10.f;
	}
	if(ModifyDepSegment.DoModal ()==IDOK)
	{
		CPoint point1=((CDepSect*)m_pSelectObj)->m_Point1 ;
		CPoint point2=((CDepSect*)m_pSelectObj)->m_Point2 ;
		point1.y=pDoc->DepthToYCoor((double)ModifyDepSegment.m_PlotSdep )
			+pDoc->GetLeftTrackObj ()->GetClientRect ().top-pDoc->m_TopSpace;
		point2.y=pDoc->DepthToYCoor((double)ModifyDepSegment.m_PlotEdep )
			+pDoc->GetLeftTrackObj ()->GetClientRect ().top-pDoc->m_TopSpace;
		((CDepSect*)m_pSelectObj)->m_Point1 =point1;
		((CDepSect*)m_pSelectObj)->m_Point2 =point2;
		((CDepSect*)m_pSelectObj)->m_Rect .left =point1.x;
		((CDepSect*)m_pSelectObj)->m_Rect.top =point1.y;
		((CDepSect*)m_pSelectObj)->m_Rect.right =point2.x;
		((CDepSect*)m_pSelectObj)->m_Rect.bottom =point2.y;
		pDoc->OnReFresh ();
	}
}

void CImageManView::OnUpdatePreRangvalue(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable (c_ToolPro == NoneTool);
	pCmdUI->SetCheck (c_ToolPro==DepRangeDraw);
}

void CImageManView::OnCurveCounterglow() 
{
	// TODO: Add your command handler code here
	CClientDC dc(this);
	OnPrepareDC(&dc,NULL);
	CRect cRect;
	GetClientRect(cRect);
	dc.DPtoLP (&cRect);
	m_iCounterIntiX=m_iCounterPreX=4*cRect.Height ()/5;
	int DrawMode = dc.SetROP2(R2_XORPEN);
	CPen AxialPen;
	AxialPen.CreatePen(PS_SOLID, 1,0xffff00);
	CPen *OldPen = (CPen *)dc.SelectObject(&AxialPen);
	dc.MoveTo (cRect.left,m_iCounterPreX);
	dc.LineTo (cRect.right,m_iCounterPreX);
	dc.SetROP2( DrawMode );
	dc.SelectObject( OldPen );
	c_ToolPro=CounterGlow;
	m_bCounterGlow=TRUE;
	CString str=pDoc->GetCurveValue(m_iCounterPreX);
	CMainFrame* pFrame=(CMainFrame*)AfxGetMainWnd ();
   	pFrame->m_wndStatusBar.SetWindowText (str);
	m_StatusText=str;
	m_DisCharPos=0;
}

void CImageManView::OnUpdateCurveCounterglow(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable((c_ToolPro == NoneTool)&&(pDoc->m_LogName!=""));
	pCmdUI->SetCheck (c_ToolPro==CounterGlow);
}

void CImageManView::OnMultiSectDep() 
{
	// TODO: Add your command handler code here
//	c_ToolPro=DepRangeDraw;
	if(!m_bProMutilDep)
		pDoc->DeleteAllRemDraw();
	m_bProMutilDep=TRUE;
	CDrawObj::c_ToolShape = linetool;
}

void CImageManView::OnUpdateMultiSectDep(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
/*	bool TLD=FALSE;
	if (m_pSelectObj != NULL)
		if(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CCurveObj)))
			TLD=TRUE;
	pCmdUI->Enable(TLD&&(c_ToolPro == NoneTool));*/
	pCmdUI->Enable(c_ToolPro == NoneTool);
}

void CImageManView::ProMultiCurveVal(float dep1,float dep2)
{
	DeSelectObj();
	CString str;int i;
	int Num=m_SelCurveNameArray.GetSize();
	if(Num<1) m_bAlikePro=FALSE;
	if(!m_bAlikePro)
	{
		CSelectCurve SelectCurDlg(pDoc->m_LogName);
		if(SelectCurDlg.DoModal ()==IDOK)
		{
			m_bAlikePro=SelectCurDlg.m_bAlikePro;
			int SelCurveNum=SelectCurDlg.m_SelCurveNameArray.GetSize();
			m_SelCurveNameArray.RemoveAll();
			if(SelCurveNum<1) 
			{
				m_bAlikePro=FALSE;
				return;
			}
			for(i=0;i<SelCurveNum;i++)
				m_SelCurveNameArray.Add(SelectCurDlg.m_SelCurveNameArray[i]);
		}
	}
	char path[512];
	strcpy(path,pDoc->m_LogName);
	CString strPath=pDoc->m_LogName;
	int yyy=strPath.Find ("#");
	if(yyy<0)
	{
		MessageBox("打开井数据失败  ","错误提示",MB_OK);
		return;
	}
	else
	{
		CFIDIO::GetPath(path,strPath.GetBuffer(512));
		strPath.ReleaseBuffer();
		strPath=strPath.Left (strPath.ReverseFind ('\\'));
		strPath=strPath+"\\"+strPath.Mid (yyy+1)+".FID";
	}
	//得到包括路径的井名FullWellName
	CString FullWellName;
	CFIDIO::GetFullWellName(strPath,FullWellName.GetBuffer(512));
	FullWellName.ReleaseBuffer();
	
	BeginWaitCursor();
	//得到将要寻找的索引文件IWellName.*
	char szFind[512];
	CFIDIO::GetFileName(FullWellName,szFind,'I');
	CString CurveName,CopyName;
	
	Num=m_SelCurveNameArray.GetSize();
	for(i=0;i<Num;i++)
	{
		CurveName=m_SelCurveNameArray[i];
		CFIDIO* pFIDtemp = NULL;
		pFIDtemp = new CFIDIO( FullWellName + "."+CurveName);
		if(!pFIDtemp->Open())
		{
			MessageBox(CurveName+" 处理失败  ","错误提示...",MB_OK);
			delete pFIDtemp;
			continue;
		}			
		int dim=pFIDtemp->GetDimenSion ();
		//常规曲线处理撤销操作的备份数据
		if(dim<3) 
		{
			CopyName.Format (".#$#%d",m_InitOprationNum+1);
			CopyName=m_UnDoFilePath+CopyName;
			pFIDtemp->Copy(CopyName);
		}
		float Sdep=pFIDtemp->GetStartDepth ();
		float Edep=pFIDtemp->GetEndDepth ();
		float Rlev=pFIDtemp->GetDepthLevel ();
		delete pFIDtemp;
		if(dep1<Sdep+2*Rlev) dep1=Sdep+2*Rlev;
		if(dep2>Edep) dep2=Edep;
		CFIDIO* pFIDIO = NULL;
		pFIDIO= new CFIDIO(FullWellName + "."+CurveName);
		if(!pFIDIO->Open())
			MessageBox(CurveName+" 处理失败          ","错误提示...",MB_OK);
		else
		{
			FIDCONTENT* Content = pFIDIO->GetContent(dim);
			short* ibuf = NULL;
			float* fbuf = NULL;
			char* cbuf = NULL;
			int nps = Content->nps;
			if (Content != NULL)
			{
				if (Content->type == REPR_FLOAT) 
					fbuf = new float[nps];
				else if (Content->type == REPR_CHAR) 
					cbuf = new char[nps];
				else 
					ibuf = new short[nps];
			}
			pFIDIO->SeekToDepth(dep1-4*Rlev);
			if(Content->type == REPR_FLOAT)
				pFIDIO->Read(dim,nps,fbuf);
			else if(Content->type == REPR_CHAR)
				pFIDIO->Read(dim,nps,cbuf);
			else 
				pFIDIO->Read(dim,nps,ibuf);
			pFIDIO->SeekToDepth(dep1);
			float dep=dep1;
			while(dep<dep2)
			{
				if(Content->type == REPR_FLOAT)
					pFIDIO->Write(dim,nps,fbuf);
				else if(Content->type == REPR_CHAR)
					pFIDIO->Write(dim,nps,cbuf);
				else 
					pFIDIO->Write(dim,nps,ibuf);
				dep+=Rlev;
				pFIDIO->Next();
			}
			if (cbuf != NULL)	delete cbuf;
			if (ibuf != NULL)	delete ibuf;
			if (fbuf != NULL)	delete fbuf;	
		}
		delete pFIDIO;
		//常规曲线可撤销操作数加一次
		if(dim<3) 
		{
			m_InitOprationNum++;
			m_UnDoName.Add(FullWellName+"."+CurveName);
			m_UnDoNameCopy.Add(CopyName);
		}
	}
	EndWaitCursor();
	pDoc->OnReFresh ();	
}

void CImageManView::OnValueSendtoClipboard() 
{
	// TODO: Add your command handler code here
	if(OpenClipboard())
	{
		// Empty the Clipboard. This also has the effect
		// of allowing Windows to free the memory associated
		// with any data that is in the Clipboard
		if(EmptyClipboard())
		{
			// Ok. We have the Clipboard locked and it's empty. 
			// Now let's allocate the global memory for our data.

			// Here I'm simply using the GlobalAlloc function to 
			// allocate a block of data equal to the text in the
			// "to clipboard" edit control plus one character for the
			// terminating null character required when sending
			// ANSI text to the Clipboard.

			HGLOBAL hClipboardData;
			hClipboardData = GlobalAlloc(GMEM_DDESHARE, 
										m_StatusText.GetLength()+1);

			// Calling GlobalLock returns to me a pointer to the 
			// data associated with the handle returned from 
			// GlobalAlloc
			char * pchData;
			pchData = (char*)GlobalLock(hClipboardData);
				  
			// At this point, all I need to do is use the standard 
			// C/C++ strcpy function to copy the data from the local 
			// variable to the global memory.
			strcpy(pchData, LPCSTR(m_StatusText));
				  
			// Once done, I unlock the memory - remember you 
			// don't call GlobalFree because Windows will free the 
			// memory automatically when EmptyClipboard is next 
			// called. 
			GlobalUnlock(hClipboardData);
			if(SetClipboardData(CF_TEXT,hClipboardData)==NULL)
				AfxMessageBox("发送到剪贴板失败        ");
		}
		CloseClipboard();
	}	
}

void CImageManView::OnUpdateValueSendtoClipboard(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable (m_bCounterGlow);
}

void CImageManView::DelUnDoOpration()
{
	m_UnDo=m_ReDo=m_InitOprationNum=m_PreOprationNum=0;
}

void CImageManView::OnAddImage() 
{
	// TODO: Add your command handler code here
	CPoint pt;
	GetCenterPoint(&pt);
	CRect rect(pt.x,pt.y,pt.x,pt.y);
	CString Path=pDoc->m_PathName;
	Path=Path.Left(Path.ReverseFind('.'));
	CImageObj* pObj = new CImageObj(Path,rect,NULL,pDoc);
	double sdep,edep;
	pDoc->GetDepthRangle(sdep, edep);
	pObj->SetDepth(sdep,edep);
	pDoc->AddObj(pObj);
	SelectObj(pObj);
}

void CImageManView::OnAddImage3d() 
{
	// TODO: Add your command handler code here
	CPoint pt;
	GetCenterPoint(&pt);
	CRect rect(pt.x,pt.y,pt.x,pt.y);
	CString Path=pDoc->m_PathName;
	Path=Path.Left(Path.ReverseFind('.'));
	CImage3D* pObj = new CImage3D(Path,rect,NULL,pDoc);
	double sdep,edep;
	pDoc->GetDepthRangle(sdep, edep);
	pObj->SetDepth(sdep,edep);
	pDoc->AddObj(pObj);
	SelectObj(pObj);
}

void CImageManView::OnAddStratum() 
{
	// TODO: Add your command handler code here
	CPoint pt;
	GetCenterPoint(&pt);
	CRect rect(pt.x,pt.y,pt.x,pt.y);
	CString Path=pDoc->m_PathName;
	Path=Path.Left(Path.ReverseFind('\\')+1);
	Path=Path+"HandWork.List";
	CStratumObj* pObj = new CStratumObj(Path,rect,NULL,pDoc);
	double sdep,edep;
	pDoc->GetDepthRangle(sdep, edep);
	pObj->SetDepth(sdep,edep);
	pDoc->AddObj(pObj);
	SelectObj(pObj);
}

LRESULT CImageManView::OnEndClick(WPARAM wParam, LPARAM lParam)
{
	//手工编辑曲线
	if(m_SinFlag==80)
	{
		EndHandDrawCurve();
		return 0;
	}
	//成像层理裂缝拾取
	if(m_pMeasure)
	{
		int i;
		int size=m_pMeasure->m_arrayPoints.GetSize();
		CPoint *point=new CPoint[size];
		for(i=0; i<size; i++)
			point[i] = m_pMeasure->m_arrayPoints.GetAt(i);

		float dip,dir,dep,dip0,dir0; 
		//接箍标注
		if(size>1&&m_SinFlag==70)
		{
			CClientDC dc1(this);
			OnPrepareDC(&dc1,NULL);
			ScreenToClient(&point[0]);
			dc1.DPtoLP(&point[0]);
			pDoc->PPtoLP(&point[0]);
			int yyy=point[0].y;
			for(i=1;i<size; i++)
			{
				ScreenToClient(&point[i]);
				dc1.DPtoLP(&point[i]);
				pDoc->PPtoLP(&point[i]);
				yyy=(yyy+point[i].y)/2;
			}
			dep=(float)pDoc->GetLeftTrackObj()->YCoorToDepth(yyy);
			dip=dir=dip0=dir0=0.f;
			CStratumObj* pStObj=pDoc->GetStratumObject();
			if(pStObj!=NULL)
			{
				pStObj->AddPoint(dep,dip,dir,dip0,dir0,m_SinFlag);
				pDoc->m_HWOprateNum++;
			}
			delete point;
			return 0;
		}

		if(size>2)
		{
			double VHr=-999.25f;
			CTrackObj* pTrack=(CTrackObj*)m_pHandDrawObj->GetParentObj();
			if(pTrack!=NULL)
			{
				POSITION pos= pTrack->m_ObjList.GetHeadPosition();
				while(pos!=NULL)
				{
					CImageObj* pImage = (CImageObj*)pTrack->m_ObjList.GetNext(pos);
					if(pImage!= NULL) 
					{
						double HorScale=pImage->m_Bit*PI/(pImage->m_Rect.Width ()*0.254f);
						VHr=pDoc->m_DepthScale/HorScale;
						break;
					}
				}
			}
			if(VHr<0.f)
			{
				AfxMessageBox("在该格线道内未找到图像对象      ");
				return 0;
			}

			CRect rect=m_pHandDrawObj->m_Rect;
			pDoc->PPtoLP(&rect);
			CDIPCalculate dipObj;			
			CClientDC dc(this);
			OnPrepareDC(&dc,NULL);
			ScreenToClient(&point[0]);
			dc.DPtoLP(&point[0]);
			pDoc->PPtoLP(&point[0]);
			dipObj.vnn=size;
			dipObj.x[0]=(double)(point[0].x-rect.left);
			dipObj.y[0]=(double)point[0].y*VHr;
			for(i=1; i < size; i++)
			{
				this->ScreenToClient(&point[i]);
				dc.DPtoLP(&point[i]);
				pDoc->PPtoLP(&point[i]);
				dipObj.x[i]=(double)(point[i].x-rect.left);
				dipObj.y[i]=(double)(point[i].y)*VHr;
			}
			dipObj.ll=rect.Width();
			dipObj.Compute ();	
		
			dir=(float)dipObj.m_dir;
			if(dir>360.) 
				dir-=360.;
			dip=(float)dipObj.m_dip;
			dep=(float)pDoc->GetLeftTrackObj ()->YCoorToDepth((int)(dipObj.m_dep/VHr));
		
			CString WellName;
			if(pDoc->m_szBufPath!="")
				WellName=pDoc->m_szBufPath;
			else
			{
				WellName=pDoc->m_LogName.Left (pDoc->m_LogName.ReverseFind ('\\'));
				WellName=WellName+"\\"
					+WellName.Mid (WellName.ReverseFind ('#')+1);
			}
			BOOL bCorre=TRUE;
			CLogProcess *pLog;float *XX;
			pLog = new CLogProcess(WellName,NULL);
			INCURVE *PInCurve = new INCURVE[4];
			strcpy(PInCurve[0].name,pDoc->m_sDEVName);
			strcpy(PInCurve[1].name,pDoc->m_sDAZName);
			strcpy(PInCurve[2].name,pDoc->m_sAZ1Name);
			strcpy(PInCurve[3].name,pDoc->m_sRBName);
			XX = new float[4];
			pLog->InitIn(4,PInCurve,(char *)XX);
			pLog->InitBuffer();
			if (pLog->DepLevel <= 0.0001)
			{
				AfxMessageBox("读取井斜、井斜方位等曲线数据失败        ",
					MB_OK|MB_ICONERROR);
				bCorre=FALSE;
			}
			
			if(bCorre)//井斜校正
			{
				pLog->Dep = dep;	
				pLog->DatIn();
				if(XX[0]>0.5f)
				{
					if(XX[0]>85.f)
					{
						dip0=XX[0]-dip;
						dir0=dir;
					}
					else
					{
						dipObj.m_dip =dip;
						dipObj.m_dir =dir;
						dipObj.m_dip1 =XX[0];
						dipObj.m_dir1 =XX[1];
						dipObj.ComputeDIPs();
						dip0=(float)dipObj.m_dips;
						dir0=(float)dipObj.m_dirs;
					}
				}
				else
				{
					dip0=dip;
					dir0=dir; 
				}
			}
			else
			{
				dip0=dip;
				dir0=dir;
			}
			while(dir0>360.f) 
				dir0-=360.f;
			while(dir0<0.f) 
				dir0+=360.f;
			if(pDoc->m_fDip1>0.5f)//背景校正
			{
				dipObj.m_dip =dip0;
				dipObj.m_dir =dir0;
				dipObj.m_dip1 =pDoc->m_fDip1;
				dipObj.m_dir1 =pDoc->m_fDir1;
				dipObj.ComputeDIPs ();
				dip0=(float)dipObj.m_dips ;
				dir0=(float)dipObj.m_dirs ;
			}	
			delete []XX;
			delete PInCurve;
			delete pLog;
			//绘正弦线
			CStratumObj* pStObj=pDoc->GetStratumObject();
			if(pStObj!=NULL)
			{
				pStObj->AddPoint(dep,dip,dir,dip0,dir0,m_SinFlag);
				pDoc->m_HWOprateNum++;
			}
			//绘矢量点
			CMarkVectorObj* pVecObj=pDoc->GetMarkVectorObject();
			if(pVecObj!=NULL)
				pVecObj->AddPoint(dep,dip,dir,dip0,dir0,m_SinFlag);
		}
		delete point;
	}
	return 0;
}

LRESULT CImageManView::OnCancelClick(WPARAM wParam, LPARAM lParam)
{
	m_pHandDrawObj->InvalObj(this);
	return 0;
}

void CImageManView::EndHandDrawCurve()
{
	CCurveObj* CurveObj;
	CurveObj=(CCurveObj*)m_pHandDrawObj;
	if(m_pMeasure&&(CurveObj!=NULL))
	{
		int PointNum=m_pMeasure->m_arrayPoints.GetSize();
		if(PointNum>1)
		{
			CString WellName,CurveName;		
			CFIDIO::GetFullWellName(CurveObj->m_strPathName , WellName.GetBuffer(512));
			WellName.ReleaseBuffer();			
			CFIDIO::GetFIDName(CurveObj->m_strPathName , CurveName.GetBuffer(512));
			CurveName.ReleaseBuffer();		
			int i,j,k;
	
			CPoint *Points=new CPoint[PointNum];
			for(i=0; i < PointNum; i++)
				Points[i] = m_pMeasure->m_arrayPoints.GetAt(i);
	
			CClientDC dc(this);
			OnPrepareDC(&dc,NULL);
			for(i=0; i < PointNum; i++)
			{
				ScreenToClient(&Points[i]);
				dc.DPtoLP(&Points[i]);
				pDoc->PPtoLP(&Points[i]);
			}
			CFIDIO* pFIDIO = NULL;
			pFIDIO = new CFIDIO(WellName + "." + CurveName);
			if(!pFIDIO->Open())
			{
				delete pFIDIO;delete Points;
				MessageBox("打开 "+WellName + "." +
					CurveName+" 文件失败!","错误提示...",MB_OK);
				return;
			}
			float Rlev=pFIDIO->GetDepthLevel();
			if(pFIDIO->GetDimenSion()>2)
			{
				delete pFIDIO;delete Points;
				AfxMessageBox("只能对常规曲线进行编辑      ");
				return;
			}
			char ss[20];
			float Sdep0=0,Edep0=0;
			sprintf(ss,".#$#%d",m_InitOprationNum+1);
			if((pFIDIO->Copy(m_UnDoFilePath+ss))!=-1)
			{
				m_InitOprationNum++;
				m_UnDoName.Add(WellName+"."+CurveName);
				m_UnDoNameCopy.Add(m_UnDoFilePath+ss);	
			
				float fbuf,Svalue,Evalue,Kvalue;
				float Sdep,Edep,dep;				
				int RectLeftX=CurveObj->m_Rect.left;
				float LeftValue=(float)CurveObj->m_LeftVal1;
				float RightValue=(float)CurveObj->m_RightVal1;
				int RectWide=CurveObj->m_Rect.Width();
				unsigned short CalType=CurveObj->m_CalType;
	
				Sdep=(float)CurveObj->YCoorToDepth (Points[0].y);
				Svalue = LeftValue+( RightValue-LeftValue )*(Points[0].x - RectLeftX)/RectWide;
				if (CalType == 1) 
					 Svalue= (float) pow(10.,Svalue*1.); 
			
				/////////////
				pFIDIO->SeekToDepth (Sdep);
				pFIDIO->Write(2,1,&Svalue);
				Sdep0=Sdep;
				for(i=1;i<PointNum;i++)
				{
					Edep=(float)CurveObj->YCoorToDepth (Points[i].y);			
					Evalue =LeftValue+( RightValue-LeftValue )*(Points [i].x-RectLeftX)/RectWide;
					if (CalType == 1) 
						Evalue = (float) pow(10.,Evalue*1.); 
					k=(int)((Edep-Sdep)/Rlev);
					if(k>0)
					{
						Kvalue=(Evalue-Svalue)/k;
						for(j=0;j<=k;j++)
						{
							dep=Sdep+Rlev*j;
							pFIDIO->SeekToDepth (dep);
							fbuf=Svalue+j*Kvalue;
							pFIDIO->Write(2,1,&fbuf);
						}
					}
					if(k>=0)
					{
						pFIDIO->SeekToDepth (Edep);
						pFIDIO->Write(2,1,&Evalue);
						Sdep=Edep;Svalue=Evalue;
					}
					if(Sdep<Sdep0) Sdep0=Sdep;
					if(Edep>Edep0) Edep0=Edep;
				}
			}
			delete pFIDIO;delete Points;
			pDoc->OnReFresh ();
		}
	}
}

void CImageManView::StartHandDrawSin()
{
	if(m_SinFlag!=80)
		m_pHandDrawObj=pDoc->GetStratumObject();
	CRect rect;
	GetClientRect(&rect);
	ClientToScreen(&rect);
	if(m_pMeasure==NULL)
	{
		m_pMeasure = new CMeasureMultiLine;
		m_pMeasure->CreateMeasure("MeasureMultiLine",this,rect);
	}
	m_pMeasure->m_MousePt=CPoint(-999,-999);
	m_pMeasure->m_arrayPoints.RemoveAll();
	CDC dc;
	//取桌面屏幕设备 
	dc.CreateDC("DISPLAY",NULL,NULL,NULL);
	CBitmap Bitmap;
	Bitmap.CreateCompatibleBitmap(&dc,rect.Width(),rect.Height());
	m_pMeasure->m_dcMem.SelectObject(&Bitmap);
	m_pMeasure->m_dcMem.BitBlt(0,0,rect.Width(),rect.Height(),&dc,rect.left,rect.top,SRCCOPY);
	Bitmap.DeleteObject();
	dc.DeleteDC ();
	m_pMeasure->ShowWindow(SW_SHOW);
}

void CImageManView::OnMarkStratum() 
{
	// TODO: Add your command handler code here
	//绿色--层更理
	if(m_SinFlag<0)
		StartHandDrawSin();
	m_SinFlag=10;
}

void CImageManView::OnUpdateMarkStratum(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(pDoc->m_bHandDrawSin&&m_iHoleOperate<0&&m_SinFlag!=80);
	pCmdUI->SetCheck(m_SinFlag==10);
}

void CImageManView::OnMarkResfracture() 
{
	// TODO: Add your command handler code here
	//青蓝色--充填缝
	if(m_SinFlag<0)
		StartHandDrawSin();
	m_SinFlag=20;
}

void CImageManView::OnUpdateMarkResfracture(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(pDoc->m_bHandDrawSin&&m_iHoleOperate<0&&m_SinFlag!=80);
	pCmdUI->SetCheck(m_SinFlag==20);
}

void CImageManView::OnMarkConfracture() 
{
	// TODO: Add your command handler code here
	//蓝色--张开缝
	if(m_SinFlag<0)
		StartHandDrawSin();
	m_SinFlag=30;
}

void CImageManView::OnUpdateMarkConfracture(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(pDoc->m_bHandDrawSin&&m_iHoleOperate<0&&m_SinFlag!=80);
	pCmdUI->SetCheck(m_SinFlag==30);
}

void CImageManView::OnMarkAvalanche() 
{
	// TODO: Add your command handler code here
	//紫色--压裂缝
	if(m_SinFlag<0)
		StartHandDrawSin();
	m_SinFlag=40;
}

void CImageManView::OnUpdateMarkAvalanche(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(pDoc->m_bHandDrawSin&&m_iHoleOperate<0&&m_SinFlag!=80);
	pCmdUI->SetCheck(m_SinFlag==40);
}


void CImageManView::OnMarkResfracture2() 
{
	// TODO: Add your command handler code here
	//淡蓝色--半充填缝
	if(m_SinFlag<0)
		StartHandDrawSin();
	m_SinFlag=50;
}

void CImageManView::OnUpdateMarkResfracture2(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(pDoc->m_bHandDrawSin&&m_iHoleOperate<0&&m_SinFlag!=80);
	pCmdUI->SetCheck(m_SinFlag==50);
}

void CImageManView::OnMarkLayerfracture() 
{
	// TODO: Add your command handler code here
	//淡蓝色--层间缝
	if(m_SinFlag<0)
		StartHandDrawSin();
	m_SinFlag=60;
}

void CImageManView::OnUpdateMarkLayerfracture(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(pDoc->m_bHandDrawSin&&m_iHoleOperate<0&&m_SinFlag!=80);
	pCmdUI->SetCheck(m_SinFlag==60);
}

void CImageManView::OnMarkCCL() 
{
	// TODO: Add your command handler code here
	//绿色--层更理
	if(m_SinFlag<0)
		StartHandDrawSin();
	m_SinFlag=70;
}

void CImageManView::OnUpdateMarkCCL(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(pDoc->m_bHandDrawSin&&m_iHoleOperate<0&&m_SinFlag!=80);
	pCmdUI->SetCheck(m_SinFlag==70);
}

void CImageManView::OnCurveMouseedit() 
{
	// TODO: Add your command handler code here
	//常规曲线编辑
	if(m_SinFlag<0)
	{	
		m_SinFlag=80;
		m_pHandDrawObj=m_pSelectObj;
		StartHandDrawSin();
	}
	m_SinFlag=80;
}

void CImageManView::OnUpdateCurveMouseedit(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	bool TLD=FALSE;
	if (m_pSelectObj!=NULL)
		if(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CCurveObj)))
			TLD=TRUE;
	pCmdUI->Enable (TLD&&m_iHoleOperate<0||m_SinFlag==80);
	pCmdUI->SetCheck(m_SinFlag==80);
}


void CImageManView::OnComputeDip() 
{
	// TODO: Add your command handler code here
	//区域层理自动拾取
	m_SinFlag=90;
}

void CImageManView::OnUpdateComputeDip(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(pDoc->m_bHandDrawSin&&m_iHoleOperate<0&&m_SinFlag!=80);
	pCmdUI->SetCheck(m_SinFlag==90);
}

void CImageManView::OnDelHandwork() 
{
	// TODO: Add your command handler code here
	//区域层理裂缝
	m_SinFlag=100;
}

void CImageManView::OnUpdateDelHandwork(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(pDoc->m_bHandDrawSin&&m_iHoleOperate<0&&m_SinFlag!=80);
	pCmdUI->SetCheck(m_SinFlag==100);
}

void CImageManView::OnAddMarkvector() 
{
	// TODO: Add your command handler code here
	CPoint pt;
	GetCenterPoint(&pt);
	CRect rect(pt.x,pt.y,pt.x,pt.y);
	CString Path=pDoc->m_PathName;
	Path=Path.Left(Path.ReverseFind('\\')+1);
	Path=Path+"HandWork.List";
	CMarkVectorObj* pObj = new CMarkVectorObj(Path,rect,NULL,pDoc);
	double sdep,edep;
	pDoc->GetDepthRangle(sdep, edep);
	pObj->SetDepth(sdep,edep);
	pDoc->AddObj(pObj);
	SelectObj(pObj);
}


void CImageManView::OnAddMarkfreq() 
{
	// TODO: Add your command handler code here
	CPoint pt;
	GetCenterPoint(&pt);
	CRect m_Rect(pt.x,pt.y,pt.x,pt.y);
	CString Path=pDoc->m_PathName;
	Path=Path.Left(Path.ReverseFind('\\')+1);
	Path=Path+"HandWork.List";
	CMarkFreqObj* pObj = new CMarkFreqObj(Path,m_Rect,NULL,pDoc);
	double sdep,edep;
	pDoc->GetDepthRangle(sdep, edep);
	pObj->SetDepth(sdep,edep);
	pDoc->AddObj(pObj);
	SelectObj(pObj);
}

void CImageManView::OnAddImagecurve() 
{
	// TODO: Add your command handler code here
	CPoint pt;
	GetCenterPoint(&pt);
	CRect m_Rect(pt.x,pt.y,pt.x,pt.y);
	CString Path=pDoc->m_PathName;
	Path=Path.Left(Path.ReverseFind('.'));
	CImageCurveObj* pObj = new CImageCurveObj(Path,m_Rect,NULL,pDoc);
	double sdep,edep;
	pDoc->GetDepthRangle(sdep, edep);
	pObj->SetDepth(sdep,edep);
	pDoc->AddObj(pObj);
	SelectObj(pObj);
}

void CImageManView::OnEddyRight() 
{
	// TODO: Add your command handler code here
	CImage3D* pObj=(CImage3D*)m_pSelectObj;
	if(pObj!=NULL)
	{
		pObj->m_StartPos +=5;
		pObj->InvalObj(this);
	}
}

void CImageManView::OnUpdateEddyRight(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable((m_pSelectObj != NULL)&&(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CImage3D))));
}

void CImageManView::OnEddyPlay() 
{
	// TODO: Add your command handler code here
	m_pImage3D=(CImage3D*)m_pSelectObj;
	if(m_pImage3D!=NULL)
	{
		m_EddyPlay=TRUE;
		DeSelectObj();
		SetTimer(1, 50, NULL);
	}
}

void CImageManView::OnUpdateEddyPlay(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable((m_pSelectObj != NULL)&&
		(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CImage3D)))&&m_pImage3D==NULL);
}

void CImageManView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(m_EddyPlay&&(m_pImage3D!=NULL))
	{
		m_pImage3D->m_StartPos +=5;
		m_pImage3D->InvalObj(this);
	}
	CScrollView::OnTimer(nIDEvent);
}

void CImageManView::OnPlayStop() 
{
	// TODO: Add your command handler code here
	m_EddyPlay=FALSE;
	m_pImage3D=NULL;
	KillTimer(1);
}

void CImageManView::OnUpdatePlayStop(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_EddyPlay);
}

void CImageManView::SetScroDep(float dep)
{
	int Min,Max;
	GetScrollRange(SB_VERT,&Min,&Max);
	int iScrollBarPos=(int)((dep-pDoc->m_PlotSdep)/(pDoc->m_PlotEdep -pDoc->m_PlotSdep )
		*((double)(Max-Min)));
	SetScrollPos( SB_VERT, iScrollBarPos );
	Invalidate();
}

void CImageManView::OnAddEcho() 
{
	// TODO: Add your command handler code here
	CPoint pt;
	GetCenterPoint(&pt);
	CRect m_Rect(pt.x,pt.y,pt.x,pt.y);
	CString Path=pDoc->m_PathName;
	Path=Path.Left(Path.ReverseFind('.'));
	CEchoObj* pObj = new CEchoObj(Path,m_Rect,NULL,pDoc);
	double sdep,edep;
	pDoc->GetDepthRangle(sdep, edep);
	pObj->SetDepth(sdep,edep);
	pDoc->AddObj(pObj);
	SelectObj(pObj);
}

void CImageManView::OnFrequency() 
{
	// TODO: Add your command handler code here
	if (m_pSelectObj != NULL)
	{
		CCurveObj* pObj=(CCurveObj*)m_pSelectObj;
		CString WellName,CurveName;		
		CFIDIO::GetFullWellName(pObj->m_strPathName, WellName.GetBuffer(512));
		WellName.ReleaseBuffer();
		CFIDIO::GetFIDName(pObj->m_strPathName, CurveName.GetBuffer(512));
		CurveName.ReleaseBuffer();
		CFIDIO* pFIDtemp = NULL;
		pFIDtemp = new CFIDIO(WellName + "." + CurveName);
		if(!pFIDtemp->Open())
		{
			MessageBox("  打开 "+WellName + "." +
				CurveName+" 文件失败!  ","错误提示...",MB_OK);
			delete pFIDtemp;return;
		}
		float SDepth=pFIDtemp->GetStartDepth();
		float EDepth=pFIDtemp->GetEndDepth();
		float Rlev=pFIDtemp->GetDepthLevel();
		delete pFIDtemp;
		if(pDoc->GetLineObject ()!=NULL)
		{
			int DaltaY=pDoc->GetLeftTrackObj ()->GetClientRect ().top -pDoc->m_TopSpace;
			SDepth=(float)pDoc->YCoorToDepth(pDoc->GetLineObject ()->m_Rect.top-DaltaY);
			EDepth=(float)pDoc->YCoorToDepth(pDoc->GetLineObject ()->m_Rect.bottom -DaltaY);
		}
		CString ss;
		ss.Format ("+%.1f--%.1f+%d+%f+%f",SDepth,EDepth,pObj->m_CalType,
			pObj->m_LeftVal,pObj->m_RightVal);
		CString RunFileName=GetWorkPath("System")+"Frequency";
		CString CmdMessage=WellName+">"+CurveName+ss;
		char *args[3];char str[512];
		args[0]="Frequency";
		strcpy(str,CmdMessage);args[1]=str;
		args[2] = NULL;

		_spawnv( _P_NOWAIT, RunFileName, args );
	}
}

void CImageManView::OnUpdateFrequency(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	bool TLD=FALSE;
	if (m_pSelectObj != NULL)
		if(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CCurveObj))||m_pSelectObj->IsKindOf(RUNTIME_CLASS(CImageObj)))
			TLD=TRUE;
	pCmdUI->Enable(TLD&&(c_ToolPro == NoneTool));
}

void CImageManView::OnAddPole() 
{
	// TODO: Add your command handler code here
	CPoint pt;
	GetCenterPoint(&pt);
	CRect rect(pt.x,pt.y,pt.x,pt.y);
	CString Path=pDoc->m_PathName;
	Path=Path.Left(Path.ReverseFind('\\')+1);
	Path=Path+"HandWork.List";
	CPoleObj* pObj = new CPoleObj(Path,rect,NULL,pDoc);
	double sdep,edep;
	pDoc->GetDepthRangle(sdep, edep);
	pObj->SetDepth(sdep,edep);
	pDoc->AddObj(pObj);
	SelectObj(pObj);
}

void CImageManView::OnHoleAnalysis() 
{
	// TODO: Add your command handler code here
	if(pDoc->m_szBufPath!="")
	{
		CHoleAnalysis dlg;
		dlg.m_szAradFileName=pDoc->m_szBufPath+"."+pDoc->m_szRADSName;
		if(pDoc->m_sSeries.Find("MIT")>-1)
			dlg.m_szAradFileName=pDoc->m_szBufPath+".DATA00";
		dlg.m_Dep=pDoc->m_HorRulerDep;
		dlg.m_sAZ1Name=pDoc->m_szBufPath+"."+pDoc->m_sAZ1Name;
		dlg.m_sDEVName=pDoc->m_szBufPath+"."+pDoc->m_sDEVName;
		dlg.m_sDAZName=pDoc->m_szBufPath+"."+pDoc->m_sDAZName;
		dlg.m_bBoxAnisys=pDoc->m_bBoxAnisys;
		if(pDoc->m_sSeries=="MIT")
		{
			dlg.m_szUnit="mm";
			dlg.m_sAZ1Name=_T("");
		}
		else
			dlg.m_szUnit="in";
		dlg.DoModal ();
	}
}

void CImageManView::OnUpdateHoleAnalysis(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable (TRUE);/*(pDoc->m_sSeries=="CBIL")
		||(pDoc->m_sSeries=="CAST")
		||(pDoc->m_sSeries=="MIT"));*/
}

void CImageManView::OnImageAnalysis() 
{
	// TODO: Add your command handler code here
	if(pDoc->m_szBufPath!="")
	{
		CHoleAnalysis dlg;
		if(pDoc->m_sSeries.Find("MIT")>-1)
			dlg.m_szAradFileName=pDoc->m_LogName+".RIMG";
		else
			dlg.m_szAradFileName=pDoc->m_LogName+".STAMI";
		dlg.m_Dep=pDoc->m_HorRulerDep;
		dlg.m_sAZ1Name="";
		dlg.m_sDEVName=pDoc->m_szBufPath+"."+pDoc->m_sDEVName;
		dlg.m_sDAZName=pDoc->m_szBufPath+"."+pDoc->m_sDAZName;
		dlg.m_bBoxAnisys=pDoc->m_bBoxAnisys;
		/*if(pDoc->m_sSeries=="MIT")
		{
			dlg.m_szUnit="mm";
			dlg.m_sAZ1Name=_T("");
		}
		else*/
			dlg.m_szUnit="none";
		dlg.DoModal ();
	}
}

void CImageManView::OnCurveDepico() 
{
	// TODO: Add your command handler code here
	if (m_pSelectObj != NULL)
	{
		CFIDDrawObj* pObj=(CFIDDrawObj*)m_pSelectObj;
		CString WellName,CurveName;		
		CFIDIO::GetFullWellName(pObj->m_strPathName, WellName.GetBuffer(512));
		WellName.ReleaseBuffer();
		CFIDIO::GetFIDName(pObj->m_strPathName, CurveName.GetBuffer(512));
		CurveName.ReleaseBuffer();
		POSITION pos=pDoc->m_RemObjList .GetHeadPosition ();
		if(pos!=NULL)
		{
			CLineObj *pLineObj;
			pLineObj = (CLineObj*)pDoc->m_RemObjList.GetNext (pos);
			if (pLineObj != NULL)
				if(pLineObj->IsKindOf (RUNTIME_CLASS(CLineObj)))
				{
					ProMultisectdep(CurveName);
					return;
				}
		}

		BeginWaitCursor();
		CFIDIO* pFIDtemp = NULL;
		pFIDtemp = new CFIDIO(WellName + "." + CurveName);
		if(!pFIDtemp->Open())
		{
			MessageBox("  打开 "+WellName + "." +
				CurveName+" 文件失败!  ","错误提示...",MB_OK);
			delete pFIDtemp;EndWaitCursor();return;
		}
		float Sdep = pFIDtemp->GetStartDepth();
		float Edep = pFIDtemp->GetEndDepth();
		float Rlev=pFIDtemp->GetDepthLevel();
		char ss[20];
		sprintf(ss,".#$#%d",m_InitOprationNum+1);
		pFIDtemp->Copy(m_UnDoFilePath+ss);
		delete pFIDtemp;
		EndWaitCursor();
	
		CCurveDep ReviseDepdlg(pObj);
		if(pDoc->GetLineObject ()!=NULL)
		{
			int DaltaY=pDoc->GetLeftTrackObj ()->GetClientRect ().top -pDoc->m_TopSpace;
			float Dep=(float)pDoc->YCoorToDepth(pDoc->GetLineObject ()->m_Rect.top-DaltaY);
			Dep=((int)(Dep/Rlev))*Rlev;
			ReviseDepdlg.m_ExSdep =Dep;
			Dep=(float)pDoc->YCoorToDepth(pDoc->GetLineObject ()->m_Rect.bottom -DaltaY);
			Dep=((int)(Dep/Rlev))*Rlev;
			ReviseDepdlg.m_ExEdep=Dep;
		}
		else
		{
			ReviseDepdlg.m_ExSdep =Sdep;
			ReviseDepdlg.m_ExEdep=Edep;
		}
		if(ReviseDepdlg.DoModal()==IDOK)
		{
			m_InitOprationNum++;
			m_UnDoName.Add(WellName+"."+CurveName);
			m_UnDoNameCopy.Add(m_UnDoFilePath+ss);
			pDoc->OnReFresh ();
		}
	}
}

void CImageManView::OnUpdateCurveDepico(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	bool TLD=FALSE;
	if ((m_pSelectObj != NULL)&&
		(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CCurveObj))
		||m_pSelectObj->IsKindOf(RUNTIME_CLASS(CImageObj))))
			TLD=TRUE;
	pCmdUI->Enable(TLD&&(c_ToolPro == NoneTool));
}

void CImageManView::ProMultisectdep(CString SelCurveName)
{
	DeSelectObj();
	CString str,OptionStr="";
	float DaltaMove,DepPro;
	int SectNum=0;
	int DaltaY=pDoc->GetLeftTrackObj ()->GetClientRect ().top -pDoc->m_TopSpace;
	CLineObj *pLineObj;
	POSITION pos = pDoc->m_RemObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pLineObj = (CLineObj*)pDoc->m_RemObjList.GetNext (pos);
		if (pLineObj != NULL)
		{
			if(pLineObj->IsKindOf (RUNTIME_CLASS(CLineObj)))
			{				
				depArray[SectNum][0]=(float)pDoc->YCoorToDepth(pLineObj->m_Point1 .y-DaltaY);
				depArray[SectNum][1]=(float)pDoc->YCoorToDepth(pLineObj->m_Point2 .y-DaltaY);
				SectNum++;
			}
		}
	} 
	if(SectNum>0)
	{		
		DaltaMove=depArray[0][1]-depArray[0][0];
		if(DaltaMove>0)
			OptionStr.Format("曲线整体下移 %.1f 米",DaltaMove);
		else
			OptionStr.Format("曲线整体上移 %.1f 米",DaltaMove*-1);
	}
	CProMultiCurveDep SelectCurDlg(pDoc->m_LogName,this);
	SelectCurDlg.m_SectNum =SectNum-1;
	int i;
	if(SectNum>1)
	{
		for(i=0;i<SectNum-1;i++)
		{
			if(depArray[i][0]<depArray[i+1][0]&&depArray[i][1]>depArray[i+1][1])
			{
				AfxMessageBox("深度处理线有交叉的情况出现      ");
				return;
			}
			if(depArray[i][0]>depArray[i+1][0]&&depArray[i][1]<depArray[i+1][1])
			{
				AfxMessageBox("深度处理线有交叉的情况出现      ");
				return;
			}
			if(-0.2<depArray[i][0]-depArray[i+1][0]&&depArray[i][0]-depArray[i+1][0]<0.2)
			{
				AfxMessageBox("深度处理线起始有交叉的情况出现      ");
				return;
			}
		}
		//井段从小到大排序
		float data0,data1;int MinPos,j,k;
		for(i=0;i<SectNum;i++)
		{
			MinPos=i;
			data0=depArray[i][0];
			data1=depArray[i][1];
			for(j=i+1;j<SectNum;j++)
			{
				if(depArray[j][0]<data0)
				{
					MinPos=j;
					data0=depArray[j][0];
					data1=depArray[j][1];
				}
			}
			if(MinPos>i);
			{
				for(k=MinPos;k>i;k--)
				{
					depArray[k][0]=depArray[k-1][0];
					depArray[k][1]=depArray[k-1][1];
				}
				depArray[i][0]=data0;
				depArray[i][1]=data1;
			}
		}

		for(i=0;i<SectNum-1;i++)
		{
			str.Format ("%.1f--%.1f\r\n",depArray[i][0],depArray[i+1][0]);
			SelectCurDlg.m_DepArray .Add (str);
			DepPro=(depArray[i+1][1]-depArray[i][1])-(depArray[i+1][0]-depArray[i][0]);
			if(DepPro>0)
				str.Format ("伸长%.1f",DepPro);
			else
				str.Format ("压缩%.1f",DepPro*-1);
			SelectCurDlg.m_ReviseArray .Add (str);
		}
	}
	if(SectNum>0)
	{		
		DaltaMove=depArray[0][1]-depArray[0][0];
		if(DaltaMove>0)
			OptionStr.Format("曲线整体下移 %.1f 米",DaltaMove);
		else
			OptionStr.Format("曲线整体上移 %.1f 米",DaltaMove*-1);
	}
	SelectCurDlg.m_OptionStr =OptionStr;
	if(SelCurveName!="")
		SelectCurDlg.m_SelCurveNameArray .Add(SelCurveName);
	if(SelectCurDlg.DoModal ()==IDOK)
	{
		int SelCurveNum=SelectCurDlg.m_SelCurveNameArray .GetSize ();
		if(SelCurveNum<1) return;
		m_bProMutilDep=FALSE;
		POSITION pos = pDoc->m_RemObjList.GetHeadPosition();
		while (pos != NULL)
		{
			pLineObj = (CLineObj*)pDoc->m_RemObjList.GetNext (pos);
			if (pLineObj != NULL)
			{
				if(pLineObj->IsKindOf (RUNTIME_CLASS(CLineObj)))
					pLineObj->m_Point1.y=pLineObj->m_Point2.y;
			}
		} 
		pDoc->OnReFresh ();	
	}
}

void CImageManView::OnDipPara() 
{
	// TODO: Add your command handler code here
	CDIPComputeDlg DipParaSet;
	DipParaSet.m_WinLength=m_WinLength;                //相关对比窗长
	DipParaSet.m_StepLength=m_StepLength;              //计算倾角曲线的步长
	DipParaSet.m_SearchLength=m_SearchLength;          //上下探索长度
	DipParaSet.m_DangMax=m_DangMax;                    //最大倾角
	DipParaSet.m_RMin=m_RMin; 
	DipParaSet.m_GradCons=m_GradCons;
	DipParaSet.m_GradMin=m_GradMin;
	//参加倾角计算的最小相关系数
	if(DipParaSet.DoModal ()==IDOK)
	{
		m_WinLength=DipParaSet.m_WinLength;      
		m_StepLength=DipParaSet.m_StepLength;    
		m_SearchLength=DipParaSet.m_SearchLength;
		m_DangMax=DipParaSet.m_DangMax;          
		m_RMin=DipParaSet.m_RMin; 
		m_GradCons=DipParaSet.m_GradCons;
		m_GradMin=DipParaSet.m_GradMin;
	}
}

void CImageManView::OnModifyHandwork() 
{
	// TODO: Add your command handler code here
	float   StProDep,EnProDep;
	if(pDoc->GetLineObject ()!=NULL)
	{
		int DaltaY=pDoc->GetLeftTrackObj ()->GetClientRect ().top -pDoc->m_TopSpace;
		float Dep=(float)pDoc->YCoorToDepth(pDoc->GetLineObject ()->m_Rect.top-DaltaY);
		StProDep  =((int)(Dep*10+0.5))/10.f;
		Dep=(float)pDoc->YCoorToDepth(pDoc->GetLineObject ()->m_Rect.bottom -DaltaY);
		EnProDep =((int)(Dep*10+0.5))/10.f;
		pDoc->ModifyDepSectSin(StProDep,EnProDep);
	}
	else 
	{
		if (m_pSelectObj != NULL)
		{
			if(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CSinPoint)))
			{				
				CSinPoint* pObj=(CSinPoint*)m_pSelectObj;
				if(pObj!=NULL)
				{
					CString Title[6]={"层理","充填缝","张开缝","压裂缝","半充填缝","层间缝"};
					COLORREF Color[6]={0x009300,0x00FF00,0xFF0000,0x800080,0xFF8000,0xFFFF00};
					CModifyHandWorkDlg Dlg;
					for(int i=0;i<6;i++)
					{
						if(pObj->m_Flag==Title[i])
						{
							Dlg.m_TypeSin=i;
							break;
						}
					}
					if(Dlg.DoModal ()==IDOK)
					{
						pObj->m_Color=Color[Dlg.m_TypeSin];
						pObj->m_Flag=Title[Dlg.m_TypeSin];
						pObj->m_bSave=TRUE;
						pObj->Invalidate();
					}
				}
			}
		}
	}
}

void CImageManView::OnUpdateModifyHandwork(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_pSelectObj != NULL);
}

void CImageManView::OnMarkHole() 
{
	// TODO: Add your command handler code here
	float   StProDep,EnProDep;
	if(pDoc->GetLineObject ()!=NULL)
	{
		int DaltaY=pDoc->GetLeftTrackObj ()->GetClientRect ().top -pDoc->m_TopSpace;
		StProDep=(float)pDoc->YCoorToDepth(pDoc->GetLineObject ()->m_Rect.top-DaltaY);
		EnProDep=(float)pDoc->YCoorToDepth(pDoc->GetLineObject ()->m_Rect.bottom -DaltaY);
		if(!pDoc->m_szBufPath.IsEmpty ())
		{
			char str[512];
			char *args[3];	
			CString ExcFileName=GetWorkPath("System")+"MarkImage";
			strcpy(str,ExcFileName);
			args[0]=str;
			CString DataFileName;	
			DataFileName.Format ("%s>%f,%f^%s",pDoc->m_PathName,StProDep,EnProDep,pDoc->m_sSeries);	
			strcpy(str,DataFileName);
			args[1]=str;
			args[2] = NULL;
			_spawnv( _P_NOWAIT, ExcFileName, args );
		}
	}
}

void CImageManView::OnUpdateMarkHole(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_pSelectObj != NULL);
}

void CImageManView::OnBmpSenttoCilp() 
{
	// TODO: Add your command handler code here
	double sdep=pDoc->m_PlotSdep;
	double edep=pDoc->m_PlotEdep;
	BOOL DrawFIDMode=pDoc->m_DrawFIDHeadMode;
	BOOL bPlotHead=pDoc->m_bPlotHead;
	POINT point1,point2;
	double dep1,dep2;
	int Ycro=GetScrollPos(SB_VERT);
	if(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CDepSect)))
	{
		CBitmap m_Bitmap;
		CDC m_MemDC;
		CDC* pDrawDC=GetDC();
		if(!m_MemDC.CreateCompatibleDC(pDrawDC))
		{
			AfxMessageBox("初始化内存失败",MB_ICONINFORMATION);
			return;
		}
		int DaltaY=pDoc->GetLeftTrackObj ()->GetClientRect ().top-pDoc->m_TopSpace;
		point1=((CDepSect*)m_pSelectObj)->m_Point1 ;
		point2=((CDepSect*)m_pSelectObj)->m_Point2 ;
		dep1=pDoc->YCoorToDepth (point1.y-DaltaY);
		dep2=pDoc->YCoorToDepth (point2.y-DaltaY);

		dep1=((int)(dep1*10.f+0.5f))/10.f;
		dep2=((int)(dep2*10.f+0.5f))/10.f;
		
		pDoc->m_DrawFIDHeadMode=FALSE;
		pDoc->m_bPlotHead=FALSE;
		pDoc->SetDepthNoRem(dep1,dep2);

		CRect EmfRect=pDoc->GetUnionRect();
		EmfRect.left=pDoc->GetLeftTrackObj()->m_Rect.left-1;
		EmfRect.right=pDoc->GetRightTrackObj()->m_Rect.right+1;
		pDoc->LPtoPP(&EmfRect);
		if(m_Bitmap.CreateCompatibleBitmap(pDrawDC, EmfRect.Width(),EmfRect.Height()))
		{
			pDrawDC = &m_MemDC;	
			m_MemDC.OffsetViewportOrg(-EmfRect.left, -EmfRect.top);
			m_MemDC.SelectObject(&m_Bitmap);

			CBrush brush;
			brush.CreateSolidBrush(pDoc->m_paperColor);
			brush.UnrealizeObject();  
			pDrawDC->FillRect(EmfRect, &brush);
			pDoc->BmpDraw(this,pDrawDC);
			pDoc->m_DrawFIDHeadMode=DrawFIDMode;
			pDoc->m_bPlotHead=bPlotHead;
			pDoc->SetDepthNoRem(sdep,edep);
			if(!OpenClipboard() )
				AfxMessageBox("不能打开剪贴板    ");
			else
			{
				::EmptyClipboard();
				if(::SetClipboardData(CF_BITMAP,m_Bitmap.Detach())==NULL)
					AfxMessageBox("不能设置剪贴板的数据    ");
				else
				{
					CString str;
					str.Format("%.1f--%.1f井段的图片已发送到系统剪贴板中          ",dep1,dep2);
					AfxMessageBox(str);
				}
				::CloseClipboard();
			}
		}
		else	
			AfxMessageBox("初始化位图失败    ",MB_ICONINFORMATION);
	}
}
void CImageManView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO   ScrollInfo; 
	GetScrollInfo(SB_VERT, &ScrollInfo, SIF_TRACKPOS); 
	nPos = ScrollInfo.nTrackPos; 

	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CImageManView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO   ScrollInfo; 
	GetScrollInfo(SB_HORZ, &ScrollInfo, SIF_TRACKPOS); 
	nPos = ScrollInfo.nTrackPos; 

	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
}
/*
 *绘制自动识取的溶蚀孔洞边界
 */
void CImageManView::DrawPickupContour(CDC *pDC)
{
	if(m_ContourList.IsEmpty())
		return;
	if(m_pPickupObj==NULL)
		return;
	BOOL flag=TRUE;
	int DataNum=0;
	CFIDIO* inFID=new CFIDIO(m_PickupFIDName);
	if(inFID->Open())
	{
		FIDCONTENT *Content = inFID->GetContent(3);
		if(Content != NULL)
			DataNum=Content->npw;
	}
	if(DataNum<1) 
	{
		delete inFID;
		flag=FALSE;
	}

	CPoint pt,OffPt;
	OffPt.x=m_pPickupObj->m_Rect.left-m_PickupRect.left;
	OffPt.y=pDoc->DepthToYCoor(m_PickupStDep)-pDoc->DepthToYCoor(pDoc->m_PlotSdep);
	OffPt.y+=m_pPickupObj->m_Rect.top-m_PickupRect.top;
	
	//纵横向不同绘图比例校正系数
	double Her,Ver;
	Her=m_pPickupObj->m_Rect.Width()*1.0/m_PickupRect.Width();
	Ver=m_PickupVerScale/pDoc->m_DepthScale;

	CRect rect;
	int i,k,x,y,m;
	
	//分类填充孔洞需要的变量
	BOOL bRead;
	float dep;CRgn rgn;
	float *pData=new float[DataNum];
	/*COLORREF Color[6]={0xFFFF00,0x008000,0xFF0000,0x008CFF,0x00D7FF,0x0000FF};*/
	COLORREF Color[3]={0xFF0000,0x00D7FF,0x0000FF};
	//只绘边界需要的变量
	CArray<CPoint,CPoint&>ptArray;
	CRect rect0 =m_pPickupObj->m_Rect;
	CPen pen(PS_SOLID,1,0xD4FF7F);
	CPen* pOldPen=pDC->SelectObject(&pen);
	CBrush *pOldbr=(CBrush *)pDC->SelectStockObject(NULL_BRUSH);

	CContour* pContour=NULL;
	POSITION pos=m_ContourList.GetHeadPosition();			
	while(pos!=NULL)
	{	
		pContour=(CContour*)m_ContourList.GetNext(pos);
		rect = pContour->m_rect;
		rect.left=m_PickupRect.left+(int)((rect.left-m_PickupRect.left)*Her);
		rect.right=rect.left+(int)(rect.Width()*Her);
		rect.top=m_PickupRect.top+(int)((rect.top-m_PickupRect.top)*Ver);
		rect.bottom=rect.top+(int)(rect.Height()*Ver);
		rect.OffsetRect(OffPt);

		pDoc->LPtoPP(&rect);
		if(pDC->RectVisible(rect))
		{
			m=0;
			k=pContour->m_pointArray.GetSize();
			if(k>4)
			{
				if(m_bConnection&&flag)//分类填充孔洞
				{
					if(rgn.CreatePolygonRgn(&pContour->m_pointArray[0],k,ALTERNATE))
					{
						rgn.GetRgnBox(&rect);
						for(y=rect.top; y<=rect.bottom; y++)
						{
							bRead=TRUE;
							for(x=rect.left; x<=rect.right; x++)
							{
								pt.x=x;pt.y=y;
								if(rgn.PtInRegion(pt))
								{
									//进行纵横向不同绘图比例校正
									pt.x=m_PickupRect.left+(int)((pt.x-m_PickupRect.left)*Her);
									pt.y=m_PickupRect.top+(int)((pt.y-m_PickupRect.top)*Ver);
									//进行整体偏移
									pt.Offset(OffPt);
									if(pt.y>rect0.top&&pt.y<rect0.bottom)
									{
										if(bRead)
										{
											dep=(float)m_pPickupObj->YCoorToDepth(pt.y);
											inFID->SeekToDepth(dep);
											inFID->Read(3,DataNum,pData);
											bRead=FALSE;
										}
										m=(pt.x-m_pPickupObj->m_Rect.left)*DataNum/m_pPickupObj->m_Rect.Width();
										if(m>-1&&m<DataNum)
										{
											if(pData[m]>0.f)
											{
												k=(int)((pData[m]-m_ImageMin)*3.f/(m_ImageMax-m_ImageMin)-0.5f);
												if(k<0) k=0;
												if(k>2) k=2;
												pDoc->LPtoPP(&pt);
												pDC->FillSolidRect(pt.x,pt.y,2,2,Color[k]);
											}
										}
									}
								}
							}
						}
						rgn.DeleteObject();
					}
				}
				else //绘孔洞边界
				{
					ptArray.SetSize(k,5);
					for(i=0;i<k;i++)
					{
						pt=pContour->m_pointArray[i];
						//进行纵横向不同绘图比例校正
						pt.x=m_PickupRect.left+(int)((pt.x-m_PickupRect.left)*Her);
						pt.y=m_PickupRect.top+(int)((pt.y-m_PickupRect.top)*Ver);
						//进行整体偏移
						pt.Offset(OffPt);
						if(pt.y>rect0.top&&pt.y<rect0.bottom)
						{
							pDoc->LPtoPP(&pt);
							ptArray[m]=pt;
							m++;
						}
					}
					pDC->Polygon(&ptArray[0],m);
					ptArray.SetSize(0);
				}
			}
		}
	}
	delete pData;
	if(inFID) delete inFID;
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldbr);
}
void CImageManView::OnDrawConnection()
{
	m_bConnection=!m_bConnection;
	Invalidate();
}
void CImageManView::OnUpdateDrawConnection(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bConnection);
}
/*
 *
 *新建可粒孔洞存储空间并自动识取绘图井段内的可粒孔洞
 *
 */
void CImageManView::OnCreateContour()
{
	if(!m_pSelectObj->IsKindOf(RUNTIME_CLASS(CImageObj)))
		return;
	m_pPickupObj=(CImageObj*)m_pSelectObj;
	if(m_pPickupObj==NULL) return;

	CSetContourPara dlg;
	dlg.m_KPara=m_KPara;
	dlg.m_HoleMaxAcr=m_HoleMaxAcr;
	dlg.m_bHoleMaxAcr=m_bHoleMaxAcr;
	dlg.m_HoleMinAcr=m_HoleMinAcr;
	dlg.m_bHoleMinAcr=m_bHoleMinAcr;
	dlg.m_bMinRound=m_bMinRound;
	dlg.m_HoleMinRound=m_HoleMinRound;
	dlg.m_bMaxRound=m_bMaxRound;
	dlg.m_HoleMaxRound=m_HoleMaxRound;

	m_PickupStDep=(float)pDoc->m_PlotSdep;		//新建时的起始深度
	m_PickupEnDep=(float)pDoc->m_PlotEdep;		//新建时的结束深度
	m_PickupRect=m_pPickupObj->m_Rect;          //新建时的图像大小
	m_PickupFIDName=m_pPickupObj->m_strPathName;//曲线全名
	m_PickupAliasName=m_pPickupObj->m_AliasName;//曲线别名
	m_PickupBit=m_pPickupObj->m_Bit;            //钻头直径
	m_PickupHorScale=m_pPickupObj->m_HorScale;  //横向绘图比例
	m_PickupVerScale=(float)pDoc->m_DepthScale; //纵向绘图比例

	//计算自动拾取横向覆盖率
	//电成像极板数Content->min
	//电扣数/极板Content->max
	//一个深度采样写入文件的数据个数
	CFIDIO *pFIDIO = new CFIDIO(m_PickupFIDName);
	if(pFIDIO->Open()) 
	{
		FIDCONTENT* Content = pFIDIO->GetContent(3);
		float ButtonNum=Content->min*Content->max;
		//针对CIFLog处理的成像数据
		if(ButtonNum<10)
		{
			if(pDoc->m_CmdTemplateName.Find("CIFImage")>0)
				ButtonNum=196;
			else ButtonNum=150;
		}
		m_CoverageRate= ButtonNum*1.f/Content->npw;  //自动拾取覆盖率
	}
	delete pFIDIO;

	CString str=_T(""),ss=m_PickupFIDName;
	ss=ss.Left(ss.ReverseFind('\\'));
	ss=ss.Mid(ss.ReverseFind('\\')+2);
	str="项目名:"+ss;
	ss.Format("\n曲线名:%s",m_PickupFIDName.Mid(m_PickupFIDName.ReverseFind('.')+1));
	str+=ss;
	ss.Format("\n绘图名称:%s",m_PickupAliasName);
	str+=ss;
	ss.Format("\n钻头直径(mm):%d",m_PickupBit);
	str+=ss;
	ss.Format("\n横向绘图比例:%d",m_PickupHorScale);
	str+=ss;
	ss.Format("\n纵向绘图比例:%g",m_PickupVerScale);
	str+=ss;
	ss.Format("\n自动拾取横向覆盖率:%.3f",m_CoverageRate);
	str+=ss;
	dlg.m_DataMessage=str;
	if(dlg.DoModal()!=IDOK) 
		return;
	m_KPara=dlg.m_KPara;
	m_HoleMaxAcr=dlg.m_HoleMaxAcr;
	m_bHoleMaxAcr=dlg.m_bHoleMaxAcr;
	m_HoleMinAcr=dlg.m_HoleMinAcr;
	m_bHoleMinAcr=dlg.m_bHoleMinAcr;
	m_bMinRound=dlg.m_bMinRound;
	m_HoleMinRound=dlg.m_HoleMinRound;
	m_bMaxRound=dlg.m_bMaxRound;
	m_HoleMaxRound=dlg.m_HoleMaxRound;
	//计算孔洞最小电导率
	
	if(pDoc->m_CmdTemplateName.Find("CIFImage")>0)
		m_ImageMax=256.f;//成像数据最值 缺省1024，CIFLog处理的为256
	m_ImageMin=m_ImageMax*m_KPara/256.f;

	m_StatSdep=(float)pDoc->m_PlotSdep;
	m_StatEdep=(float)pDoc->m_PlotEdep;
	m_StatWellName=pDoc->m_PathName;
	if(m_StatWellName.ReverseFind('.')>0)
		m_StatWellName=m_StatWellName.Left(m_StatWellName.ReverseFind('.'));

	HBITMAP hBitmap;
	HDIB m_hDIB=NULL;
	CBitmap m_Bitmap;	
	CDC* pDrawDC=GetDC();
	BeginWaitCursor();
	POSITION temp;
	POSITION pos=m_ContourList.GetHeadPosition();	 
	while(pos!=NULL)
	{
		temp=pos;
		m_ContourList.GetNext(pos);
		m_ContourList.RemoveAt(temp);
	}

	CRect rect=m_PickupRect;
	//定义进度指示器
	CGuage* pGuage = new CGuage(0.f,100.f);
	pGuage->m_szGuage="纵坐标";
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->CenterWindow();
	pGuage->ShowWindow(SW_SHOW);
	pGuage->SetTitle("图像特征分析...");		
	pGuage->SetRange(rect.top,rect.bottom);

	float s;
	while(rect.top<=m_PickupRect.bottom)
	{
		rect.bottom=rect.top+1600;
		if(rect.bottom>m_PickupRect.bottom)
			rect.bottom=m_PickupRect.bottom;
		pGuage->SetPos(rect.bottom);
		if(pGuage->CheckCancelButton())
			break;
		//局部绘图获取BMP图像
		m_pPickupObj->DrawBmp(pDrawDC,&m_Bitmap,rect);
	
		hBitmap = (HBITMAP)m_Bitmap.GetSafeHandle();
		if(!hBitmap)
			continue;	
		m_hDIB = BitmapToDIB(hBitmap,NULL);
		if(m_hDIB==NULL)
			continue;

		if(m_DibImage.Attach(m_hDIB))
		{
			m_DibImage.m_KPara=m_KPara;
			//根据横向绘图比例将实际孔洞面积折算为绘图面积
			//将绘图面积mm2转换为像素
			//标准井线性刻度
			m_DibImage.m_HoleMaxAcr=m_HoleMaxAcr;
			m_DibImage.m_bHoleMaxAcr=m_bHoleMaxAcr;
			m_DibImage.m_HoleMinAcr=m_HoleMinAcr;
			m_DibImage.m_bHoleMinAcr=m_bHoleMinAcr;

			m_DibImage.m_bMinRound=m_bMinRound;
			m_DibImage.m_HoleMinRound=m_HoleMinRound;
			m_DibImage.m_bMaxRound=m_bMaxRound;
			m_DibImage.m_HoleMaxRound=m_HoleMaxRound;
			m_DibImage.m_Hor=m_PickupHorScale;
			m_DibImage.m_Ver=m_PickupVerScale;
			m_DibImage.m_fA=m_fA;
			m_DibImage.m_fB=m_fB;
			if(m_DibImage.GetContour(FALSE))
			{
				int i,k;
				CContour* pContour=NULL;
				pos=m_DibImage.m_ContourList.GetHeadPosition();
				while(pos!=NULL)
				{
					pContour=m_DibImage.m_ContourList.GetNext(pos);
					k=pContour->m_pointArray.GetCount();
					for(i=0;i<k;i++)
						pContour->m_pointArray[i]+=rect.TopLeft();
					pContour->m_rect.OffsetRect(rect.TopLeft());
					//计算颗粒的直径，单位mm
					s=(float)pContour->ComputeArea();
					s=s*m_PickupVerScale*0.254f;
					s=s*m_PickupHorScale*0.254f;
					//标准井线性刻度
					s=s*m_fA+m_fB;
					pContour->m_Radius=(float)sqrt(s/3.1415926)*2.f;
					m_ContourList.AddTail(pContour);
				}
			}	
		}
		m_Bitmap.DeleteObject ();
		rect.top+=1600;
	}
	delete pGuage;
	if(m_ContourList.GetCount()>0)
		m_iPickupNum++;
	pDrawDC->DeleteDC();
	OnDrawSelect();
	EndWaitCursor();
}

void CImageManView::OnUpdateCreateContour(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pSelectObj!=NULL);
}

void CImageManView::OnDelSelhole()
{
	m_iHoleOperate=1;//删除单个颗粒孔洞
}

void CImageManView::OnUpdateDelSelhole(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(!m_ContourList.IsEmpty()&&m_SinFlag<0);
	pCmdUI->SetCheck(m_iHoleOperate==1);
}

/*
 *
 *删除已存在的部份颗粒或孔洞
 *操作选项m_iHoleOperate
 *1=删除单个颗粒孔洞\n单个删除
 *2=删除面积大于所选择的颗粒孔洞\n删除面积大于...
 *3=删除面积小于所选择的颗粒孔洞\n删除面积小于...
 *4=删除所选定区域内的颗粒孔洞\n区域删除
 *
 */
void CImageManView::DelSelContour(CPoint pt)
{
	if(pDoc->m_DepthScale!=m_PickupVerScale
		||m_PickupHorScale!=m_pPickupObj->m_HorScale)  //横向绘图比例
	{
		CString str;
		str.Format("初始化时图像横向比例1:%d\n初始化时图像纵向比例1:%g\n\n",m_PickupHorScale,m_PickupVerScale);
		AfxMessageBox(str+"图像纵向或横向绘图比例与初始化时不一致          ");
		return;
	}

	CPoint OffPt;
	OffPt.x=m_pPickupObj->m_Rect.left-m_PickupRect.left;
	OffPt.y=pDoc->DepthToYCoor(m_PickupStDep)-pDoc->DepthToYCoor(pDoc->m_PlotSdep);
	OffPt.y+=m_pPickupObj->m_Rect.top-m_PickupRect.top;

	//纵横向不同绘图比例校正系数
	double Her,Ver;
	Her=m_pPickupObj->m_Rect.Width()*1.0/m_PickupRect.Width();
	Ver=m_PickupVerScale/pDoc->m_DepthScale;

	CRgn rgn;
	int i,k;
	CArray<CPoint,CPoint&>ptAr;
	
	POSITION pos=NULL;
	POSITION temp=NULL;
	CContour* pContour=NULL;
	if(m_iHoleOperate==1)		//删除单个颗粒孔洞
	{
		pDoc->PPtoLP(&pt);
		CPoint pt0;
		BeginWaitCursor();
		pos=m_ContourList.GetHeadPosition();
		while(pos!=NULL)
		{
			temp=pos;
			pContour=(CContour*)m_ContourList.GetNext(pos);
			if(pContour!=NULL)
			{
				k=pContour->m_pointArray.GetSize();
				ptAr.SetSize(k,5);
				//对边界点进行纵横向不同绘图比例校正
				for(i=0;i<k;i++)
				{
					pt0=pContour->m_pointArray[i];
					//进行纵横向不同绘图比例校正
					pt0.x=m_PickupRect.left+(int)((pt0.x-m_PickupRect.left)*Her);
					pt0.y=m_PickupRect.top+(int)((pt0.y-m_PickupRect.top)*Ver);
					//进行整体偏移
					pt0.Offset(OffPt);
					ptAr[i]=pt0; 
				}
				if(rgn.CreatePolygonRgn(&ptAr[0],k,ALTERNATE))
				{
					if(rgn.PtInRegion(pt))
					{
						m_ContourList.RemoveAt(temp);
						m_iPickupNum++;
						
						rgn.DeleteObject();
						ptAr.SetSize(0);
						EndWaitCursor();
						Invalidate();
						return;
					}
					rgn.DeleteObject();
				}
				ptAr.SetSize(0);
			}
		}
		EndWaitCursor();
	}
	else if(m_iHoleOperate==4)	//删除区域内颗粒孔洞
	{
		CRect rect(m_StSelPt.x,m_StSelPt.y,m_EnSelPt.x,m_EnSelPt.y);
		pDoc->PPtoLP(&rect);
		if(rect.Width()<3||rect.Height()<3)
			return;
		BOOL bflag;
		BeginWaitCursor();
		pos=m_ContourList.GetHeadPosition(); 
		while(pos!=NULL)
		{
			temp=pos;
			bflag=TRUE;
			pContour=(CContour*)m_ContourList.GetNext(pos);
			k=pContour->m_pointArray.GetCount();
			for(i=0;i<k;i++)
			{
				pt=pContour->m_pointArray[i];
				//进行纵横向不同绘图比例校正
				pt.x=m_PickupRect.left+(int)((pt.x-m_PickupRect.left)*Her);
				pt.y=m_PickupRect.top+(int)((pt.y-m_PickupRect.top)*Ver);
				//进行整体偏移
				pt.Offset(OffPt);

				if(!rect.PtInRect(pt))
				{
					bflag=FALSE;
					break;
				}
			}
			if(bflag)
			{
				m_ContourList.RemoveAt(temp);
				m_iPickupNum++;
			}
		}
		EndWaitCursor();
	}
	else //以孔洞面积判断删除
	{
		CInputBox InBox;
		InBox.m_Value=10.f;
		if(m_iHoleOperate==2)
			InBox.m_CurveName ="删除面积小于的孔洞";
		else
			InBox.m_CurveName ="删除面积大于的孔洞";
		InBox.m_Title ="孔洞面积";
		if(InBox.DoModal ()!=IDOK)
			return;
		long c=InBox.m_Value;
		long s;
		BeginWaitCursor();
		pos=m_ContourList.GetHeadPosition();	 
		while(pos!=NULL)
		{
			temp=pos;
			pContour=(CContour*)m_ContourList.GetNext(pos);
			s=pContour->ComputeArea();
			//判断,分类
			if(m_iHoleOperate==2&&s<=c)
			{
				m_ContourList.RemoveAt(temp);
				m_iPickupNum++;
			}
			else if(m_iHoleOperate==3&&s>=c)
			{
				m_ContourList.RemoveAt(temp);
				m_iPickupNum++;
			}
		}
		EndWaitCursor();
		m_iHoleOperate=-1;
	}
	Invalidate();
}

void CImageManView::OnDelSmallSel()
{
	m_iHoleOperate=2;//删除面积小于所选择的颗粒孔洞
	DelSelContour(CPoint(0,0));
}

void CImageManView::OnUpdateDelSmallSel(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_ContourList.IsEmpty()&&m_SinFlag<0);
	pCmdUI->SetCheck(m_iHoleOperate==2);
}

void CImageManView::OnDelBigSel()
{
	m_iHoleOperate=3;//删除面积大于所选择的颗粒孔洞
	DelSelContour(CPoint(0,0));
}

void CImageManView::OnUpdateDelBigSel(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_ContourList.IsEmpty()&&m_SinFlag<0);
	pCmdUI->SetCheck(m_iHoleOperate==3);
}

/*
 *设置自动拾取颗粒孔洞的控制参数\n自动拾取参数设置
 */
void CImageManView::OnSetContourPara()
{
	CSetContourPara dlg;
	dlg.m_KPara=m_KPara;
	dlg.m_HoleMaxAcr=m_HoleMaxAcr;
	dlg.m_bHoleMaxAcr=m_bHoleMaxAcr;
	dlg.m_HoleMinAcr=m_HoleMinAcr;
	dlg.m_bHoleMinAcr=m_bHoleMinAcr;
	dlg.m_bMinRound=m_bMinRound;
	dlg.m_HoleMinRound=m_HoleMinRound;
	dlg.m_bMaxRound=m_bMaxRound;
	dlg.m_HoleMaxRound=m_HoleMaxRound;
	CString str=_T(""),ss=m_PickupFIDName;
	ss=ss.Left(ss.ReverseFind('\\'));
	ss=ss.Mid(ss.ReverseFind('\\')+2);
	str="项目名:"+ss;
	ss.Format("\n曲线名:%s",m_PickupFIDName.Mid(m_PickupFIDName.ReverseFind('.')+1));
	str+=ss;
	ss.Format("\n绘图名称:%s",m_PickupAliasName);
	str+=ss;
	ss.Format("\n钻头直径(mm):%d",m_PickupBit);
	str+=ss;
	ss.Format("\n横向绘图比例:%d",m_PickupHorScale);
	str+=ss;
	ss.Format("\n纵向绘图比例:%g",m_PickupVerScale);
	str+=ss;
	ss.Format("\n自动拾取横向覆盖率:%.3f",m_CoverageRate);
	str+=ss;
	dlg.m_DataMessage=str;
	if(dlg.DoModal()==IDOK)
	{
		m_KPara=dlg.m_KPara;
		m_HoleMaxAcr=dlg.m_HoleMaxAcr;
		m_bHoleMaxAcr=dlg.m_bHoleMaxAcr;
		m_HoleMinAcr=dlg.m_HoleMinAcr;
		m_bHoleMinAcr=dlg.m_bHoleMinAcr;
		m_bMinRound=dlg.m_bMinRound;
		m_HoleMinRound=dlg.m_HoleMinRound;
		m_bMaxRound=dlg.m_bMaxRound;
		m_HoleMaxRound=dlg.m_HoleMaxRound;
		m_iPickupNum++;
	}
}

void CImageManView::OnDelRectangle()
{
	m_iHoleOperate=4;//删除所选定区域内的颗粒孔洞
}

void CImageManView::OnUpdateDelRectangle(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_ContourList.IsEmpty()&&m_SinFlag<0);
	pCmdUI->SetCheck(m_iHoleOperate==4);
}
//手工对选定的矩形区域图像特征拾取
void CImageManView::OnGetRectanglContour()
{
	m_iHoleOperate=5;//区域自动拾取
}
void CImageManView::OnUpdateGetRectanglContour(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pPickupObj!=NULL&&m_SinFlag<0);
	pCmdUI->SetCheck(m_iHoleOperate==5);
}

//手工标注椭圆图像特征
void CImageManView::OnGetEllipseContour()
{
	m_iHoleOperate=6;
}
void CImageManView::OnUpdateGetEllipseContour(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pPickupObj!=NULL&&m_SinFlag<0);
	pCmdUI->SetCheck(m_iHoleOperate==6);
}
/*
 *自动拾取选定区域层理并追加到HandWork.List文件后\n区域层理拾取
 */
void CImageManView::RectangleComputeDip()
{
	if(m_EnSelPt.y-m_StSelPt.y<3)
		return;
	int DaltaY=pDoc->GetLeftTrackObj()->GetClientRect().top-pDoc->m_TopSpace;
	float Dep=(float)pDoc->YCoorToDepth(m_StSelPt.y-DaltaY);
	float StProDep  =((int)(Dep*10+0.5))/10.f;
	Dep=(float)pDoc->YCoorToDepth(m_EnSelPt.y-DaltaY);
	float EnProDep =((int)(Dep*10+0.5))/10.f;
	if(!pDoc->m_szBufPath.IsEmpty ())
	{
		CDIPCompute DipCompute;
		DipCompute.m_szSeries=pDoc->m_sSeries;
		DipCompute.m_szWellName=pDoc->m_PathName;
		DipCompute.m_szBufPath=pDoc->m_szBufPath;
		DipCompute.m_PadNum=pDoc->m_PadNum;
		DipCompute.m_StProDep=StProDep;
		DipCompute.m_EnProDep=EnProDep;
		DipCompute.m_WinLength=m_WinLength;                //相关对比窗长
		DipCompute.m_StepLength=m_StepLength;              //计算倾角曲线的步长
		DipCompute.m_SearchLength=m_SearchLength;          //上下探索长度
		DipCompute.m_DangMax=m_DangMax;                    //最大倾角
		DipCompute.m_RMin=m_RMin;
		DipCompute.m_GradCons=m_GradCons;
		DipCompute.m_GradMin=m_GradMin;
		//参加倾角计算的最小相关系数
		if(DipCompute.Run())
			pDoc->UpdateVector();
	}
}
/*
 *删除选定区域层理并更新HandWork.List文件\n区域层理裂缝删除
 */
void CImageManView::RectangleDeleteDip()
{
	if(m_EnSelPt.y-m_StSelPt.y<3)
		return;
	int DaltaY=pDoc->GetLeftTrackObj()->GetClientRect().top-pDoc->m_TopSpace;
	float Dep=(float)pDoc->YCoorToDepth(m_StSelPt.y-DaltaY);
	float StProDep  =((int)(Dep*10+0.5))/10.f;
	Dep=(float)pDoc->YCoorToDepth(m_EnSelPt.y-DaltaY);
	float EnProDep =((int)(Dep*10+0.5))/10.f;
	pDoc->DelDepSectSin(StProDep,EnProDep);
	pDoc->UpdateVector();
}
/*
 *自动拾取选定区域颗粒孔洞并追加到已打开的颗粒孔洞分析文件后\n区域自动拾取
 */
void CImageManView::RectanglePickupContour()
{
	if(pDoc->m_DepthScale!=m_PickupVerScale
		||m_PickupHorScale!=m_pPickupObj->m_HorScale)  //横向绘图比例
	{
		CString str;
		str.Format("初始化时图像横向比例1:%d\n初始化时图像纵向比例1:%g\n\n",m_PickupHorScale,m_PickupVerScale);
		AfxMessageBox(str+"图像纵向或横向绘图比例与初始化时不一致          ");
		return;
	}
	HBITMAP hBitmap;
	HDIB m_hDIB=NULL;
	CBitmap m_Bitmap;

	CDC* pDrawDC=GetDC();
	pDoc->PPtoLP(&m_StSelPt);
	pDoc->PPtoLP(&m_EnSelPt);

	CRect rect(m_StSelPt.x,m_StSelPt.y,m_EnSelPt.x,m_EnSelPt.y);
	//局部绘图获取BMP图像
	m_pPickupObj->DrawBmp(pDrawDC,&m_Bitmap,rect);

	hBitmap = (HBITMAP)m_Bitmap.GetSafeHandle();
	if(!hBitmap) 
	{
		AfxMessageBox("error:GetSafeHandlereturn");
		return;
	}
	m_hDIB = BitmapToDIB(hBitmap,NULL);
	if(m_hDIB==NULL) 
	{
		AfxMessageBox("error:BitmapToDIB");
		return;
	}
	float s;
	BeginWaitCursor();
	if(m_DibImage.Attach(m_hDIB))
	{
		m_DibImage.m_KPara=m_KPara;
		m_DibImage.m_HoleMaxAcr=m_HoleMaxAcr;
		m_DibImage.m_bHoleMaxAcr=m_bHoleMaxAcr;
		m_DibImage.m_HoleMinAcr=m_HoleMinAcr;
		m_DibImage.m_bHoleMinAcr=m_bHoleMinAcr;
		m_DibImage.m_bMinRound=m_bMinRound;
		m_DibImage.m_HoleMinRound=m_HoleMinRound;
		m_DibImage.m_bMaxRound=m_bMaxRound;
		m_DibImage.m_HoleMaxRound=m_HoleMaxRound;
		m_DibImage.m_Hor=(float)m_PickupHorScale;
		m_DibImage.m_Ver=m_PickupVerScale;
		m_DibImage.m_fA=m_fA;
		m_DibImage.m_fB=m_fB;
		if(m_DibImage.GetContour(FALSE))
		{
			int i,k;
			CPoint OffPt;
			OffPt.x=m_PickupRect.left-m_pPickupObj->m_Rect.left;
			OffPt.y=pDoc->DepthToYCoor(pDoc->m_PlotSdep)-pDoc->DepthToYCoor(m_PickupStDep);
			OffPt+=rect.TopLeft();
			CContour* pContour=NULL;
			POSITION pos=m_DibImage.m_ContourList.GetHeadPosition();
			while(pos!=NULL)
			{
				pContour=m_DibImage.m_ContourList.GetNext(pos);
				k=pContour->m_pointArray.GetCount();
				for(i=0;i<k;i++)
					pContour->m_pointArray[i]+=OffPt;
				pContour->m_rect.OffsetRect(OffPt);
				//计算颗粒的直径，单位mm
				s=(float)pContour->ComputeArea();
				s=s*m_PickupVerScale*0.254f;
				s=s*m_PickupHorScale*0.254f;
				//标准井线性刻度
				s=s*m_fA+m_fB;
				pContour->m_Radius=(float)sqrt(s/3.1415926)*2.f;
				m_ContourList.AddTail(pContour);
			}
			if(m_DibImage.m_ContourList.GetCount()>0)
			{
				m_iPickupNum++;
				Invalidate(FALSE);
			}
		}
	}
	EndWaitCursor();
	m_Bitmap.DeleteObject ();
	pDrawDC->DeleteDC ();
}

/*
 *人工标注椭圆并追加到已打开的图像特征分析文件后\n标注椭圆图像特征
 */
void CImageManView::EllipsePickupContour()
{
	if(pDoc->m_DepthScale!=m_PickupVerScale
		||m_PickupHorScale!=m_pPickupObj->m_HorScale)  //横向绘图比例
	{
		CString str;
		str.Format("初始化时图像横向比例1:%d\n初始化时图像纵向比例1:%g\n\n",m_PickupHorScale,m_PickupVerScale);
		AfxMessageBox(str+"图像纵向或横向绘图比例与初始化时不一致          ");
		return;
	}
	CRect rect(m_StSelPt.x,m_StSelPt.y,m_EnSelPt.x,m_EnSelPt.y);
	pDoc->PPtoLP(&rect);

	CPoint OffPt;
	OffPt.x=m_PickupRect.left-m_pPickupObj->m_Rect.left;
	OffPt.y=pDoc->DepthToYCoor(pDoc->m_PlotSdep)-pDoc->DepthToYCoor(m_PickupStDep);
	OffPt.y+=m_PickupRect.top-m_pPickupObj->m_Rect.top;
	rect.OffsetRect(OffPt);

	CContour *pNew=new CContour(TRUE);	
	pNew->m_rect=rect;
	int i;CPoint Pt;
	for(i=1;i<18;i++)
	{
		Pt.x=rect.left+(rect.Width()*i/18);
		Pt.y=(rect.top+rect.bottom)/2-(rect.Height()/2.)*sin(PI*i/18);
		pNew->m_pointArray.Add(Pt);
	}
	for(i=17;i>0;i--)
	{
		Pt.x=rect.left+(rect.Width()*i/18);
		Pt.y=(rect.top+rect.bottom)/2+(rect.Height()/2.)*sin(PI*i/18);
		pNew->m_pointArray.Add(Pt);
	}
	//计算颗粒的直径，单位mm
	float s=(float)pNew->ComputeArea();
	s=s*m_PickupVerScale*0.254f;
	s=s*m_PickupHorScale*0.254f;
	//标准井线性刻度
	s=s*m_fA+m_fB;
	pNew->m_Radius=(float)sqrt(s/3.1415926)*2.f;
	m_ContourList.AddTail(pNew);
	m_iPickupNum++;
	Invalidate(FALSE);
}
/*
 *打开文件并加载颗粒孔洞分析
 */
void CImageManView::OnOpenContour()
{
	if(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CImageObj)))
	{
		m_pPickupObj=(CImageObj*)m_pSelectObj;
		static char BASED_CODE szFilter[] = 
		"颗粒孔洞分析(*.gha)|*.gha|所有文件(*.*)|*.*||";
		CFileDialog  Browse(TRUE, "颗粒孔洞分析(*.gha)", m_PickupFileName, 
							OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
		Browse.m_ofn.lpstrTitle = "打开文件";
		CString Path=pDoc->m_PathName;
		Path=Path.Left(Path.ReverseFind('\\'));
		Browse.m_ofn.lpstrInitialDir=Path;
		if(Browse.DoModal()==IDOK)
		{
			m_PickupFileName =Browse.GetPathName();
			SavePickupContour(FALSE);
			m_StatSdep=(float)pDoc->m_PlotSdep;
			m_StatEdep=(float)pDoc->m_PlotEdep;
			m_StatWellName=pDoc->m_PathName;
			if(m_StatWellName.ReverseFind('.')>0)
				m_StatWellName=m_StatWellName.Left(m_StatWellName.ReverseFind('.'));
			if(m_SinStatSdep<0.f)
			{
				m_SinStatSdep=(float)pDoc->m_PlotSdep;
				m_SinStatEdep=(float)pDoc->m_PlotEdep;
			}
			m_sRxoWell=pDoc->m_PathName;
		}
	}
}

void CImageManView::OnUpdateOpenContour(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pSelectObj!=NULL);
}

/*
 *保存已有的颗粒孔洞分析文件\n保存颗粒孔洞分析
 */
void CImageManView::OnSaveContour()
{
	// TODO: 在此添加命令处理程序代码
	if(m_PickupFileName==_T(""))
	{
		CString Path=pDoc->m_PathName;
		Path=Path.Left(Path.ReverseFind('\\'));
		CString name=Path+"\\HandWork.gha";
		static char BASED_CODE szFilter[] = 
		"颗粒孔洞分析(*.gha)|*.gha|所有文件(*.*)|*.*||";
		CFileDialog  Browse(FALSE, "颗粒孔洞分析(*.gha)", name, 
							OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
		Browse.m_ofn.lpstrTitle = "保存文件";
		Browse.m_ofn.lpstrInitialDir=Path;
		if(Browse.DoModal()==IDOK)
			m_PickupFileName =Browse.GetPathName();
		else return;
	}
	SavePickupContour(TRUE);
}

void CImageManView::OnUpdateSaveContour(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pPickupObj!=NULL&&!m_ContourList.IsEmpty());
}

/*
 *另存已有的颗粒孔洞分析文件\n另存颗粒孔洞分析
 */
void CImageManView::OnSaveasContour()
{
	static char BASED_CODE szFilter[] = 
	"颗粒孔洞分析(*.gha)|*.gha|所有文件(*.*)|*.*||";
	CFileDialog  Browse(FALSE, "颗粒孔洞分析(*.gha)", m_PickupFileName, 
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	Browse.m_ofn.lpstrTitle = "保存文件";
	CString Path=pDoc->m_PathName;
	Path=Path.Left(Path.ReverseFind('\\'));
	Browse.m_ofn.lpstrInitialDir=Path;
	if(Browse.DoModal()==IDOK)
	{
		m_PickupFileName =Browse.GetPathName();
		SavePickupContour(TRUE);
	}
}

void CImageManView::OnUpdateSaveasContour(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pPickupObj!=NULL&&!m_ContourList.IsEmpty());
}

BOOL CImageManView::SavePickupContour(BOOL bSave)
{
	char DataType[] = "IMAGEPRO-GHA    ";
	float fVer=1.5f;
	//处理备用参数据所用变量
	BOOL bNoise=FALSE;
	float fNoise=1.f;

	//创建文件
	CFile fp;
	if(bSave)
	{
		if(m_pPickupObj!=NULL)
			m_PickupFIDName=m_pPickupObj->m_strPathName;

		if(!fp.Open((const char*)m_PickupFileName,CFile::modeCreate|CFile::modeWrite))
		{
			AfxMessageBox("保存颗粒孔洞分析文件失败          ");
			return FALSE;
		}
		//创建一个档案对象
		CArchive ar(&fp,CArchive::store);
		//保存成像处理的颗粒孔洞分析数据格式标识IMAGEPRO-GHA
		ar.Write(DataType,16);
		//保存图像相关控制参数
		ar << fVer;                          //文件格式版本号
		ar << m_PickupFIDName;               //曲线全名
		ar << m_PickupAliasName;             //曲线别名
		ar << m_PickupBit;                   //钻头直径
		ar << m_PickupHorScale;              //横向绘图比例
		ar << m_PickupRect;                  //新建时的图像大小 
		ar << m_PickupStDep;                 //新建时的起始深度
		ar << m_PickupEnDep;                 //新建时的结束深度
		ar << m_PickupVerScale;              //纵向绘图比例
		ar << m_CoverageRate;                //自动拾取覆盖率
		//存储已拾取的颗粒或孔洞
		m_ContourList.Serialize(ar);

		//分析联通性参用
		ar << m_ImageMax;						//成像数据最值 缺省1024，CIFLog处理的为256
		ar << m_ImageMin;						//成像数据最值 缺省0

		//溶融孔洞自动拾取参数
		ar << m_KPara;							// 特征值聚类数
		ar << m_HoleMinAcr;						//仅拾取大于m_HoleMinAcr的孔洞
		ar << m_bHoleMinAcr;
		ar << m_HoleMaxAcr;						//仅拾取小于m_HoleMaxAcr的孔洞
		ar << m_bHoleMaxAcr;
		ar << fNoise;							//备用 float
		ar << bNoise;							//备用 BOOL
		ar << m_HoleMinRound;					//仅拾取圆度大于m_HoleMinRound的孔洞
		ar << m_bMinRound;
		ar << m_HoleMaxRound;					//仅拾取圆度大于m_HoleMaxRound的孔洞
		ar << m_bMaxRound;

		//孔洞刻度关系参数 y=A*x+B
		ar << m_fA;
		ar << m_fB;
		//裂缝刻度关系参数 y=C*x+D
		ar << m_fC;
		ar << m_fD;

		ar << m_iPickupSinWidth;				//拾取层理裂缝的最大宽度mm
		ar << m_sPickupSinLabel;				//拾取层理裂缝类型标识
		//裂缝统计参数
		ar << m_SinStatSdep,m_SinStatEdep;		//裂缝统计井段
		ar << m_SinStatWin;						//滑动窗长
		ar << m_SinStatRlev;					//滑动步长
		//计算裂缝宽度参数
		ar << m_Rm;								//泥浆电阻率
		ar << m_ParC;							//水动力参数C
		ar << m_ParB;							//水动力参数B
		ar << m_sRxoWell;						//标准电阻率井名
		ar << m_sRxoName;						//浅侧向曲线
		
		//孔洞统计参数
		ar << m_StatWellName;					//统计曲线保存位置
		ar << m_StatWin;						//滑动窗长
		ar << m_StatRlev;						//滑动步长
		ar << m_ContourMax;						//最大颗粒孔洞直径
		ar << m_StatSdep,m_StatEdep;			//统计井段
		//以颗粒孔洞直径进行分类统计
		ar << m_StatDiam1;						//小于m_StatDiam1的为小孔
		ar << m_StatDiam2;						//大于m_StatDiam1为中孔，小于m_StatDiam2
		ar << m_StatDiam3;						//大于m_StatDiam2为大孔，小于m_StatDiam3大于m_StatDiam3为溶洞
		//生成颗粒孔洞大小频谱图
		ar << m_nSpcNum;						//频谱曲线横向刻度区间数

		ar.Close();//关闭档案
		fp.Close();//关闭文件
		m_iPickupNum=0;
	}
	else
	{
		if(!fp.Open((const char*)m_PickupFileName,CFile::modeRead))
		{
			fp.Close();
			return FALSE;
		}
		//创建一个档案对象
		CArchive ar(&fp,CArchive::load);
		char type[16];
		ar.Read(type,16);
		type[16] = '\0';
		if(!strncmp(type,DataType,16))
		{
			//读取图像相关控制参数
			ar >> fVer;                          //文件格式版本号
			ar >> m_PickupFIDName;               //曲线全名
			ar >> m_PickupAliasName;             //曲线别名
			ar >> m_PickupBit;                   //钻头直径
			ar >> m_PickupHorScale;              //横向绘图比例
			ar >> m_PickupRect;                  //新建时的图像大小 
			ar >> m_PickupStDep;                 //新建时的起始深度
			ar >> m_PickupEnDep;                 //新建时的结束深度
			ar >> m_PickupVerScale;              //纵向绘图比例
			ar >> m_CoverageRate;                //自动拾取覆盖率
			//删除已有的颗粒或孔洞
			POSITION temp;
			POSITION pos=m_ContourList.GetHeadPosition();	 
			while(pos!=NULL)
			{
				temp=pos;
				m_ContourList.GetNext(pos);
				m_ContourList.RemoveAt(temp);
			}
			//读取文件中的颗粒或孔洞
			m_ContourList.Serialize(ar);
			if(m_pPickupObj!=NULL)
				m_PickupFIDName=m_pPickupObj->m_strPathName;
			if(fVer>1.2f)
			{
				//分析联通性参用
				ar >> m_ImageMax;						//成像数据最值 缺省1024，CIFLog处理的为256
				ar >> m_ImageMin;						//成像数据最值 缺省0

				//溶融孔洞自动拾取参数
				ar >> m_KPara;							// 特征值聚类数
				ar >> m_HoleMinAcr;						//仅拾取大于m_HoleMinAcr的孔洞
				ar >> m_bHoleMinAcr;
				ar >> m_HoleMaxAcr;						//仅拾取小于m_HoleMaxAcr的孔洞
				ar >> m_bHoleMaxAcr;
				ar >> fNoise;							//备用 float
				ar >> bNoise;							//备用 BOOL
				ar >> m_HoleMinRound;					//仅拾取圆度大于m_HoleMinRound的孔洞
				ar >> m_bMinRound;
				ar >> m_HoleMaxRound;					//仅拾取圆度大于m_HoleMaxRound的孔洞
				ar >> m_bMaxRound;

				//孔洞刻度关系参数 y=A*x+B
				ar >> m_fA;
				ar >> m_fB;
				//裂缝刻度关系参数 y=C*x+D
				ar >> m_fC;
				ar >> m_fD;

				ar >> m_iPickupSinWidth;				//拾取层理裂缝的最大宽度mm
				ar >> m_sPickupSinLabel;				//拾取层理裂缝类型标识
				//裂缝统计参数
				ar >> m_SinStatSdep,m_SinStatEdep;		//裂缝统计井段
				ar >> m_SinStatWin;						//滑动窗长
				ar >> m_SinStatRlev;					//滑动步长
				//计算裂缝宽度参数
				ar >> m_Rm;								//泥浆电阻率
				ar >> m_ParC;							//水动力参数C
				ar >> m_ParB;							//水动力参数B
				ar >> m_sRxoWell;						//标准电阻率井名
				ar >> m_sRxoName;						//浅侧向曲线
				
				//孔洞统计参数
				ar >> m_StatWellName;					//统计曲线保存位置
				ar >> m_StatWin;						//滑动窗长
				ar >> m_StatRlev;						//滑动步长
				ar >> m_ContourMax;						//最大颗粒孔洞直径
				ar >> m_StatSdep,m_StatEdep;			//统计井段
				//以颗粒孔洞直径进行分类统计
				ar >> m_StatDiam1;						//小于m_StatDiam1的为小孔
				ar >> m_StatDiam2;						//大于m_StatDiam1为中孔，小于m_StatDiam2
				ar >> m_StatDiam3;						//大于m_StatDiam2为大孔，小于m_StatDiam3大于m_StatDiam3为溶洞
				//生成颗粒孔洞大小频谱图
				ar >> m_nSpcNum;						//频谱曲线横向刻度区间数
			}
			Invalidate(FALSE);
		}
		else
			AfxMessageBox("不能识别的数据格式          ",MB_OK | MB_ICONEXCLAMATION);
		ar.Close();//关闭档案
		fp.Close();//关闭文件
	}
	return TRUE;
}
/*
 生成孔洞直径频谱曲线和不同孔径标准的面孔率、孔洞密度、面孔率累加曲线
 
 绘图映射模式:纵横向1英寸=100像素点 
 纵横向1个像素=0.254mm
 横向绘图比例:m_HorScale=(int)(m_Bit*PI/(m_Rect.Width()*0.254));
 m_Bit 单位 mm
 自动拾取覆盖率:m_CoverageRate= ButtonNum/Content->npw;
 拾取图像宽度:m_CoverageRate*m_Rect.Width();
 
 1、生成孔洞径频谱曲线
	相关参数
		//最大孔洞直径 160mm 对应刻度范围0--160mm
		//孔洞直径频谱刻度区间数 40
	输出曲线
		//孔洞直径频谱 HSPC 为阵列曲线 每个深度点所存数据个数为统计参数刻度区间数 40决定
 
 2、生成常规曲线 
	相关参数
		//以孔洞直径进行分类统计
			小孔洞直径<m_StatDiam1(10mm);
			中孔洞直径>m_StatDiam1(10mm)同时小于m_StatDiam2(50mm);
			大孔洞直径>m_StatDiam2(100mm);
		//以孔洞联通性进行分类统计
		    电导率最小值  m_ImageMin 缺省值为自动拾取时特征聚类数折算值 m_ImageMin=x*1024/256
			电导率最大值  m_ImageMax 缺省值为1024
 
	输出曲线
		最大孔洞直径      DMAX   单位mm
		最小孔洞直径      DMIN   单位mm
		平均孔洞直径      DAVE   单位mm
		面孔率            HPOR   单位%
		孔洞密度          SUMB   单位#/米2

		小孔洞面孔率      HPOR1  单位%
		小孔洞密度		  SUMB1  单位#/米2
		中孔洞面孔率      HPOR2  单位%
		中孔洞密度		  SUMB2  单位#/米2
		大孔洞面孔率      HPOR3  单位%
		大孔洞密度		  SUMB3  单位#/米2
		溶洞面孔率        HPOR4  单位%
		溶洞密度		  SUMB4  单位#/米2

		//分类面孔率累加曲线
	        小孔面孔率累加曲线 VPOR1  单位%
	        中孔面孔率累加曲线 VPOR2  单位%
	        大孔面孔率累加曲线 VPOR3  单位%
			溶洞面孔率累加曲线 VPOR4  单位%

        //分类孔洞密度累加曲线
			小孔密度累加曲线   VSUM1  #/米2
			中孔密度累加曲线   VSUM2  #/米2
			大孔密度累加曲线   VSUM3  #/米2
			溶洞密度累加曲线   VSUM4  #/米2

		//孔洞联通性分析
			总联通指数	 	 HPERT  单位none  HPERT=√(HPERH**2+HPERV**2)
			水平联通指数 	 HPERH  单位none
			垂直联通指数 	 HPERV  单位none 

			低导面孔率 		  HPORC1 单位%
			中导面孔率 		  HPORC2 单位%
			高导面孔率 	      HPORC3 单位%
			高导面孔率累加	  VPORC1 单位%
			中导面孔率累加 	  VPORC2 单位%
			低导面孔率累加 	  VPORC3 单位%

*/
void CImageManView::OnPickupStat()
{
	if(m_ContourList.IsEmpty()||m_pPickupObj==NULL)
		return;
	int DataNum=0;
	CFIDIO* inFID=new CFIDIO(m_PickupFIDName);
	if(inFID->Open())
	{
		FIDCONTENT *Content = inFID->GetContent(3);
		if(Content != NULL)
			DataNum=Content->npw;
	}
	if(DataNum<1) 
	{
		delete inFID;
		AfxMessageBox(m_PickupFIDName+"\n曲线打开失败!        ");
		return;
	}

	CString OutName="HSPC";
	float win12=m_StatWin/2.f;
	win12=((int)(win12/m_StatRlev))*m_StatRlev;
	//一个统计窗口的井壁面积mm2
	float WinS=(float)PI*m_pPickupObj->m_Bit*(2.f*win12*1000.f);
	//一个统计窗口的井壁面积m2
	float WinSm=(float)PI*(m_pPickupObj->m_Bit/1000.f)*(2.f*win12);

	//自动拾取覆盖率m_CoverageRate
	WinS=WinS*m_CoverageRate;
	WinSm=WinSm*m_CoverageRate;
	m_StatSdep=(float)pDoc->m_PlotSdep;
	m_StatEdep=(float)pDoc->m_PlotEdep;
	float StartDep=((int)(m_StatSdep/m_StatRlev))*m_StatRlev;
	float EndDep=((int)(m_StatEdep/m_StatRlev))*m_StatRlev;

	//定义进度指示器
	CGuage* pGuage = new CGuage(StartDep,EndDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->CenterWindow();
	pGuage->ShowWindow(SW_SHOW);
	BOOL bBreak=FALSE;
	pGuage->SetTitle(OutName+" 初始化孔洞直径频谱曲线...");

	float Scale=m_ContourMax/m_nSpcNum;
	//对滑动窗长和滑动步长重新计算
	FIDCONTENT ContentW3[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
							   {"S","mm2",REPR_FLOAT,4,1,0,0.f,100.f,0.5f},
   						       {"HSPC","#",REPR_FLOAT,4,1,1,0.f,100.f,0.5f}};
	ContentW3[0].min = StartDep;
	ContentW3[0].max = EndDep;
	ContentW3[0].rlev= m_StatRlev;
	ContentW3[1].nps=m_nSpcNum;
	ContentW3[1].min = 0.f;
	ContentW3[1].max = m_ContourMax;
	ContentW3[1].rlev=Scale;
	ContentW3[2].nps=m_nSpcNum;
	ContentW3[2].npw=m_nSpcNum;
	ContentW3[2].min = 0.f;
	ContentW3[2].max = 100.f;
	ContentW3[2].rlev=1.f;
	strcpy(ContentW3[2].name,OutName);

	FIDINDEX Index;
	Index.dimension = 3;
	Index.Contentlist = ContentW3;
	char adi[] = "==END==";
	Index.adi = adi;

	//初始化颗粒孔洞频谱曲线
	CFIDIO* pFID = new CFIDIO(m_StatWellName+"."+OutName,&Index);
	if(!pFID->Open())
	{
		delete pGuage;delete inFID;
		AfxMessageBox(m_StatWellName+"\n初始化孔洞直径频谱曲线失败");
		return;
	}
	pFID->Close();delete pFID;
 
	//输出曲线信息
	CLogProcess *pLog= new CLogProcess(m_StatWellName,NULL);
	pLog->StartDep=StartDep;
	pLog->EndDep=EndDep;
	pLog->DepLevel=m_StatRlev;
	//初始化输出
	OUTCURVE *pOutCurve= new OUTCURVE[32];
	strcpy(pOutCurve[0].name,"DMAX");    //最大孔洞直径       DMAX   单位mm
	strcpy(pOutCurve[1].name,"DMIN");	 //最小孔洞直径       DMIN   单位mm
	strcpy(pOutCurve[2].name,"DAVE");	 //平均孔洞直径       DAVE   单位mm
	strcpy(pOutCurve[3].name,"HPOR");	 //面孔率             HPOR   单位%
	strcpy(pOutCurve[4].name,"SUMB");	 //孔洞密度           SUMB   单位#/米2	
	strcpy(pOutCurve[5].name,"HPOR1");	 //小孔洞面孔率       HPOR1  单位%
	strcpy(pOutCurve[6].name,"SUMB1");	 //小孔洞密度		  SUMB1  单位#/米2
	strcpy(pOutCurve[7].name,"HPOR2");	 //中孔洞面孔率       HPOR2  单位%
	strcpy(pOutCurve[8].name,"SUMB2");	 //中孔洞密度		  SUMB2  单位#/米2
	strcpy(pOutCurve[9].name,"HPOR3");	 //大孔洞面孔率       HPOR3  单位%
	strcpy(pOutCurve[10].name,"SUMB3");	 //大孔洞密度		  SUMB3  单位#/米2
	strcpy(pOutCurve[11].name,"HPOR4");	 //溶洞面孔率         HPOR4  单位%
	strcpy(pOutCurve[12].name,"SUMB4");	 //溶洞密度		      SUMB4  单位#/米2
	//分类面孔率累加曲线
	strcpy(pOutCurve[13].name,"VPOR1");	 //小孔面孔率累加曲线 VPOR1  单位%
	strcpy(pOutCurve[14].name,"VPOR2");	 //中孔面孔率累加曲线 VPOR2  单位%
	strcpy(pOutCurve[15].name,"VPOR3");	 //大孔面孔率累加曲线 VPOR3  单位%
	strcpy(pOutCurve[16].name,"VPOR4");	 //溶洞面孔率累加曲线 VPOR4  单位%
	//分类孔洞密度累加曲线
	strcpy(pOutCurve[17].name,"VSUM1");	 //小孔密度累加曲线	  VSUM1  #/米2
	strcpy(pOutCurve[18].name,"VSUM2");	 //中孔密度累加曲线   VSUM2  #/米2
	strcpy(pOutCurve[19].name,"VSUM3");	 //大孔密度累加曲线   VSUM3  #/米2
	strcpy(pOutCurve[20].name,"VSUM4");	 //溶洞密度累加曲线   VSUM4  #/米2
	//孔洞联通性分析
	strcpy(pOutCurve[21].name,"HPERT");	 //总联通指数 		  HPERM  单位none
	strcpy(pOutCurve[22].name,"HPERH");	 //水平联通指数 	  HPERH  单位none
	strcpy(pOutCurve[23].name,"HPERV");	 //垂直联通指数 	  HPERV  单位none
	//孔洞联通性分类面孔率
	strcpy(pOutCurve[24].name,"HPORC1"); //低导面孔率 		  HPORC1 单位%
	strcpy(pOutCurve[25].name,"HPORC2"); //中导面孔率 		  HPORC2 单位%
	strcpy(pOutCurve[26].name,"HPORC3"); //高导面孔率 	      HPORC3 单位%
	strcpy(pOutCurve[27].name,"VPORC1"); //低导面孔率累加	  VPORC1 单位%
	strcpy(pOutCurve[28].name,"VPORC2"); //中导面孔率累加 	  VPORC2 单位%
	strcpy(pOutCurve[29].name,"VPORC3"); //高导面孔率累加 	  VPORC3 单位%
	strcpy(pOutCurve[30].name,"DFMA");   //最大频率位置孔径   DFMA   单位mm
	strcpy(pOutCurve[31].name,"DFWD");   //孔径谱宽度         DFWD   单位mm

	float *OutXX=new float[32];
	pLog->InitOut(32,pOutCurve,(char *)OutXX);	
	pLog->InitBuffer();

	CPoint pt,OffPt;
	OffPt.x=m_pPickupObj->m_Rect.left-m_PickupRect.left;
	OffPt.y=pDoc->DepthToYCoor(m_PickupStDep)-pDoc->DepthToYCoor(pDoc->m_PlotSdep);
	OffPt.y+=m_pPickupObj->m_Rect.top-m_PickupRect.top;
	
	//纵横向不同绘图比例校正系数
	double Her,Ver;
	Her=m_pPickupObj->m_Rect.Width()*1.0/m_PickupRect.Width();
	Ver=m_PickupVerScale/pDoc->m_DepthScale;

	CArray<CPoint,CPoint&>ptAr;
	
	CRgn rgn;
	int i,k,m,n,Num,x,y,ptNum;

	POSITION pos=NULL;
	CContour* pContour=NULL;
	//分类填充孔洞需要的变量
	BOOL bRead;
	float *pData=new float[DataNum];

	CFile Outfp;
	char s[256],fn[256];
	strcpy(s,m_StatWellName);
	strcat(s,".");strcat(s,OutName);
	CFIDIO::GetFileName(s,fn,'D');
	if(Outfp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		pGuage->SetTitle("生成孔洞分类统计曲线...");
		float *freq=new float[m_nSpcNum];
		//分区面孔率和孔洞密度 
		float Hpor1,Hpor2,Hpor3,Hpor4, Hporc[3];
		int Sumb,Sumb1,Sumb2,Sumb3,Sumb4;

		float s,area,hpor,d,dmax,dmin,dave;
		//孔洞水平垂直联通性
		float perh,perv,pert,Rx,Ry,tmp;
		int p0,p1,pm;
		

		Num=0;
		CRect SlideRect,rect;
		float dep=StartDep+win12;
		Outfp.Seek(((int)(win12/m_StatRlev))*m_nSpcNum*4,CFile::begin);
		while(dep<EndDep-win12)
		{
			SlideRect=m_pPickupObj->m_Rect;
			SlideRect.top=m_pPickupObj->DepthToYCoor(dep-win12);
			SlideRect.bottom=m_pPickupObj->DepthToYCoor(dep+win12);
			
			area=0.f;hpor=0.f;Sumb=0;
			dmax=0.f;dave=0.f;dmin=9999.25f;
			Hpor1=0.f;Sumb1=0;
			Hpor2=0.f;Sumb2=0;
			Hpor3=0.f;Sumb3=0;
			Hpor4=0.f;Sumb4=0;

			perh=0.f;perv=0.f,pert=0.f;
			for(i=0;i<3;i++)
				Hporc[i]=0.f;
			//计算窗口内各区间频率点数
			for(i=0;i<m_nSpcNum;i++)
				freq[i]=0.f;
			pos=m_ContourList.GetHeadPosition();
			while(pos!=NULL)
			{	
				pContour=(CContour*)m_ContourList.GetNext(pos);
				k=pContour->m_pointArray.GetSize();
				////计算单个图像特征面积ptNum
				ptNum=0;
				if(k>3)
				{
					n=0;
					ptAr.SetSize(k,5);
					//计算滑动窗口内的真实面积
					for(i=0;i<k;i++)
					{
						pt=pContour->m_pointArray[i];
						//进行纵横向不同绘图比例校正
						pt.x=m_PickupRect.left+(int)((pt.x-m_PickupRect.left)*Her);
						pt.y=m_PickupRect.top+(int)((pt.y-m_PickupRect.top)*Ver);
						//进行整体偏移
						pt.Offset(OffPt);
						if(SlideRect.PtInRect(pt))
						{
							ptAr[n]=pt; 
							n++;
						}
					}
					//计算区域内部的面积ptNum
					if(n>4)
					{
						if(rgn.CreatePolygonRgn(&ptAr[0],n,ALTERNATE))
						{
							rgn.GetRgnBox(&rect);
							//计算面积
							//计算联通指数
							n=0;tmp=0;
							for(y=rect.top; y<=rect.bottom; y++)
							{
								bRead=TRUE;
								for(x=rect.left; x<=rect.right; x++)
								{
									if(rgn.PtInRegion(CPoint(x,y)))
									{
										ptNum++;//计算面积
										//计算联通指数
										if(bRead)
										{
											inFID->SeekToDepth((float)m_pPickupObj->YCoorToDepth(y));
											inFID->Read(3,DataNum,pData);
											bRead=FALSE;
										}
										m=(x-m_pPickupObj->m_Rect.left)*DataNum/m_pPickupObj->m_Rect.Width();
										if(m>-1&&m<DataNum)
										{
											if(pData[m]>0.f)
											{
												tmp+=(pData[m]-m_ImageMin)/(m_ImageMax-m_ImageMin);
												k=(int)((pData[m]-m_ImageMin)*3.f/(m_ImageMax-m_ImageMin)-0.5f);
												if(k<0) k=0;
												if(k>2) k=2;
												Hporc[k]+=1.f;
												n++;
											}
										}
									}
								}
							}
							rgn.DeleteObject();

							Rx=rect.Width()*m_PickupHorScale*0.254f;
							Ry=rect.Height()*m_PickupVerScale*0.254f;
							if(n>0)
							{
								Rx=Rx*tmp/n;
								Ry=Ry*tmp/n;
							}
							perh+=Rx;
							perv+=Ry;

							s=(float)ptNum;
							s*=m_PickupVerScale*0.254f;
							s*=m_PickupHorScale*0.254f;
							//标准井线性刻度
							s=s*m_fA+m_fB;
							area+=s;
							
							//求孔洞直径
							tmp=pContour->ComputeArea();
							tmp*=m_PickupVerScale*0.254f;
							tmp*=m_PickupHorScale*0.254f;
							//标准井线性刻度
							tmp=tmp*m_fA+m_fB;

							d=(float)sqrt(tmp/3.1415926)*2.f;
							pContour->m_Radius=d;
							if(d>0.5f)//电成像测井最小分辨率
							{
								if(d>dmax) dmax=d;
								//统计大于最小分辨0.2英寸的孔洞
								if(d<dmin) dmin=d;
								dave+=d;
								//计算孔径频谱
								i=(int)(d*m_nSpcNum/m_ContourMax);
								if(i>m_nSpcNum-1) 
									i=m_nSpcNum-1;

								freq[i]+=1.f;
								//计算分类面孔率
								Sumb++;
								if(d<=m_StatDiam1)
								{
									Hpor1+=s;Sumb1++;
								}
								else if(d<=m_StatDiam2)
								{
									Hpor2+=s;Sumb2++;
								}
								else if(d<=m_StatDiam3)
								{
									Hpor3+=s;Sumb3++;
								}
								else
								{
									Hpor4+=s;Sumb4++;
								}
							}
						}
					}
					ptAr.SetSize(0);
				}			
			}
			freq[0]=freq[0]/Sumb*100.f;
			//三点钟形滤波         0.212Ti-1+0.576Ti+0.212Ti+1
			for(i=1;i<m_nSpcNum-1;i++)
			{
				freq[i]=freq[i]/Sumb*100.f;
				freq[i]=freq[i-1]*0.212f+freq[i]*0.576f+freq[i+1]*0.212f;
			}
			//保存孔径谱
			Outfp.Write (freq,m_nSpcNum*4L); 

			//统计最大频率位置的孔径
			pm=1;
			tmp=1.f;
			for(i=1;i<m_nSpcNum-1;i++)
			{
				if(freq[i]>tmp)
				{
					tmp=freq[i];
					pm=i;
				}
			}
			pm++;
			tmp/=5.f;
			//计算孔径谱宽度位置
			p0=0;
			for(i=pm;i>0;i--)
			{
				if(freq[i]<tmp)
				{
					p0=i;
					break;
				}
			}
			p1=0;
			for(i=pm;i<m_nSpcNum-1;i++)
			{
				if(freq[i]<tmp)
				{
					p1=i;
					break;
				}
			}
			//保存统计参数曲线
			for(i=0;i<31;i++)
				OutXX[i]=0.001f;
			if(Sumb>0)
			{
				OutXX[0]=dmax;									//最大孔洞直径		DMAX   单位mm
				OutXX[1]=dmin;									//最小孔洞直径		DMIN   单位mm
				OutXX[2]=dave/Sumb;								//平均孔洞直径		DAVE   单位mm
				OutXX[3]=area/WinS*100.f;						//面孔率			HPOR   单位%
				OutXX[4]=Sumb/WinSm;							//孔洞密度			SUMB   个#/米2
				OutXX[5]=Hpor1/WinS*100.f;						//小孔面孔率		HPOR1   单位%
				OutXX[6]=Sumb1/WinSm;							//小孔密度			SUMB1   个#/米2
				OutXX[7]=Hpor2/WinS*100.f;						//中孔面孔率		HPOR2   单位%
				OutXX[8]=Sumb2/WinSm;							//中孔密度			SUMB2   个#/米2
				OutXX[9]=Hpor3/WinS*100.f;						//大孔面孔率		HPOR3   单位%
				OutXX[10]=Sumb3/WinSm;							//大孔密度			SUMB3   个#/米2
				OutXX[11]=Hpor4/WinS*100.f;						//溶洞面孔率		HPOR4   单位%
				OutXX[12]=Sumb4/WinSm;							//溶洞密度			SUMB4   个#/米2
				OutXX[13]=OutXX[5];								//小孔累加曲线		VPOR1  单位%
				OutXX[14]=OutXX[13]+OutXX[7];					//中孔累加曲线		VPOR2  单位%
				OutXX[15]=OutXX[14]+OutXX[9];					//大孔累加曲线		VPOR3  单位%
				OutXX[16]=OutXX[15]+OutXX[11];					//大孔累加曲线		VPOR4  单位%

				OutXX[17]=OutXX[6];								//小孔密度累加曲线	VSUM1  #/米2
				OutXX[18]=OutXX[17]+OutXX[8];					//中孔密度累加曲线	VSUM2  #/米2
				OutXX[19]=OutXX[18]+OutXX[10];					//大孔密度累加曲线	VSUM3  #/米2
				OutXX[20]=OutXX[19]+OutXX[12];					//溶洞密度累加曲线	VSUM4  #/米2

				pert=(float)sqrt(perh*perh+perv*perv);		
				OutXX[21]=pert/WinS*area/Sumb;					//总联通指数 	 HPERT  单位none
				OutXX[22]=perh/WinS*area/Sumb;					//水平联通指数 	 HPERH  单位none
				OutXX[23]=perv/WinS*area/Sumb;					//垂直联通指数 	 HPERV  单位none
				//孔洞联通性分类面孔率
				//低导面孔率 		  HPORC1 单位%
				//中导面孔率 		  HPORC2 单位%
				for(i=0;i<2;i++)	
				{
					Hporc[i]*=m_PickupHorScale*0.254f;
					Hporc[i]*=m_PickupVerScale*0.254f;
					//标准井线性刻度
					Hporc[i]=Hporc[i]*m_fA+m_fB;
					OutXX[24+i]=Hporc[i]/WinS*100.f;
				}
				OutXX[26]=OutXX[3]-OutXX[24]-OutXX[25];			//高导面孔率 	      HPORC3 单位%											
				OutXX[27]=OutXX[24];							//低导面孔率累加	  VPORC1 单位%
				OutXX[28]=OutXX[24]+OutXX[25];					//中导面孔率累加 	  VPORC2 单位%
				OutXX[29]=OutXX[3];								//高导面孔率累加 	  VPORC3 单位%
				OutXX[30]=pm*Scale;								//最大频率位置孔径 	  DFMA   单位mm
				OutXX[31]=(p1-p0)*Scale;						//孔径谱宽度		  DFWD   单位mm
			}
			pLog->Dep=dep;
			pLog->DatOut();

			Num++;
			dep=StartDep+win12+m_StatRlev*Num;
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton())
			{
				bBreak=TRUE;
				break;
			}
		}

		delete freq;
		Outfp.Close();
		//释放批量常规曲线输出缓存
		delete OutXX;delete pOutCurve;delete pLog;
	}
	else bBreak=TRUE;
	delete pGuage;delete inFID;delete pData;
	if(!bBreak) 
		pDoc->WellRefresh();
}

void CImageManView::OnUpdatePickupStat(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pPickupObj!=NULL&&!m_ContourList.IsEmpty());
}
//生成储层孔洞分类统计表
//在综合解释表给定的井段内对孔洞进行统计并生成统计表ReHole.List
void CImageManView::OnPickupList()
{
	if(!m_pSelectObj->IsKindOf(RUNTIME_CLASS(CResultObj)))
	{
		AfxMessageBox("请先选中综合解释符号标注对象        ");
		return;
	}
	CResultObj *pResult=(CResultObj*)m_pSelectObj;
	if(pResult==NULL) return;
	
	if(m_ContourList.IsEmpty()||m_pPickupObj==NULL)
		return;

	CString HolesFile="ReHoles.List";
	static char BASED_CODE szFilter[] = 
	"颗粒孔洞统计表(*.List)|*.List|所有文件(*.*)|*.*||";
	CFileDialog  Browse(FALSE, "颗粒孔洞统计表(*.List)",HolesFile, 
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	Browse.m_ofn.lpstrTitle = "保存文件";
	CString Path=m_StatWellName;
	Path=Path.Left(Path.ReverseFind('\\'));
	Browse.m_ofn.lpstrInitialDir=Path;
	if(Browse.DoModal()!=IDOK) return;
	HolesFile =Browse.GetPathName();

	int DataNum=0;
	CFIDIO* inFID=new CFIDIO(m_pPickupObj->m_strPathName);
	if(inFID->Open())
	{
		FIDCONTENT *Content = inFID->GetContent(3);
		if(Content != NULL)
			DataNum=Content->npw;
	}
	if(DataNum<1) 
	{
		delete inFID;
		AfxMessageBox(m_pPickupObj->m_strPathName+"\n曲线打开失败!        ");
		return;
	}

	CStdioFile File;
	if(!File.Open(HolesFile,CFile::modeWrite | CFile::modeCreate| CFile::typeText)) 
	{
		AfxMessageBox(HolesFile+"\n文件打开失败!");
		delete inFID;return;
	}
	//列表题
	CString szStr;
	szStr="序号,深度,总孔洞数,面孔率,平均孔径,联通指数,小孔面孔率,小孔个数,中孔面孔率,中孔个数,大孔面孔率,大孔个数,溶洞面孔率,溶洞个数,最大孔径,最小孔径,水平联通指数,垂直联通指数\n";
	File.WriteString(szStr);
	//单位
	szStr.Format(",m,#,％,mm,none,％(<%gmm),#,％(%g-%gmm),#,％(%g-%gmm),#,％(>%gmm),#,mm,mm,none,none",m_StatDiam1,m_StatDiam1,m_StatDiam2,m_StatDiam2,m_StatDiam3,m_StatDiam3);
	File.WriteString(szStr);

	CPoint pt,OffPt;
	OffPt.x=m_pPickupObj->m_Rect.left-m_PickupRect.left;
	OffPt.y=pDoc->DepthToYCoor(m_PickupStDep)-pDoc->DepthToYCoor(pDoc->m_PlotSdep);
	OffPt.y+=m_pPickupObj->m_Rect.top-m_PickupRect.top;
	
	//纵横向不同绘图比例校正系数
	double Her,Ver;
	Her=m_pPickupObj->m_Rect.Width()*1.0/m_PickupRect.Width();
	Ver=m_PickupVerScale/pDoc->m_DepthScale;

	CArray<CPoint,CPoint&>ptAr;
	
	CRgn rgn;
	int x,y,ptNum;
	int i,k,m,n,Num;
	CRect SlideRect,rect;

	float WinS,s,area,hpor,sumb,d,dmax,dmin,dave;
	//分区面孔率和孔洞数
	float Hpor1;float Sumb1;
	float Hpor2;float Sumb2;
	float Hpor3;float Sumb3;
	float Hpor4;float Sumb4;
	//孔洞水平垂直联通性
	float perh,perv,pert,Rx,Ry,tmp;
	//定义进度指示器
	Num=pResult->m_ResultPoint.GetCount();
	CGuage* pGuage = new CGuage(0.f,Num*1.f);
	pGuage->m_szGuage="目标数";
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->CenterWindow();
	pGuage->ShowWindow(SW_SHOW);
	pGuage->SetRange(0,Num);
	pGuage->SetTitle("生成储层孔洞统计表...");

	Num=0;
	CContour* pContour=NULL;
	CResultPoint* pObj=NULL;
	POSITION pos=NULL;			//用于解释井段查询
	POSITION pos1=NULL;			//用于图像特征查询

	//分类填充孔洞需要的变量
	BOOL bRead;
	float *pData=new float[DataNum];
	pos=pResult->m_ResultPoint.GetHeadPosition();
	while(pos!=NULL)
	{
		pObj = (CResultPoint*) pResult->m_ResultPoint.GetNext(pos);
		SlideRect=m_pPickupObj->m_Rect;
		SlideRect.top=m_pPickupObj->DepthToYCoor(pObj->m_Sdep);
		SlideRect.bottom=m_pPickupObj->DepthToYCoor(pObj->m_Edep);

		//一个统计窗口的井壁面积mm2
		WinS=(float)(PI*m_pPickupObj->m_Bit*(pObj->m_Edep-pObj->m_Sdep)*1000.f);
		//自动拾取覆盖率m_CoverageRate
		WinS=WinS*m_CoverageRate;

		area=0.f;hpor=0.f;sumb=0.f;
		dmax=0.f;dmin=9999.25f;dave=0.f;
		Hpor1=0.f;Sumb1=0.f;
		Hpor2=0.f;Sumb2=0.f;
		Hpor3=0.f;Sumb3=0.f;
		Hpor4=0.f;Sumb4=0.f;
		perh=0.f;perv=0.f;pert=0.f;

		pos1=m_ContourList.GetHeadPosition();
		while(pos1!=NULL)
		{	
			pContour=(CContour*)m_ContourList.GetNext(pos1);
			k=pContour->m_pointArray.GetSize();
			//计算单个图像特征面积ptNum
			ptNum=0;
			if(k>4)
			{
				n=0;
				ptAr.SetSize(k,5);
				for(i=0;i<k;i++)
				{
					pt=pContour->m_pointArray[i];
					//进行纵横向不同绘图比例校正
					pt.x=m_PickupRect.left+(int)((pt.x-m_PickupRect.left)*Her);
					pt.y=m_PickupRect.top+(int)((pt.y-m_PickupRect.top)*Ver);
					//进行整体偏移
					pt.Offset(OffPt);
					if(SlideRect.PtInRect(pt))
					{
						ptAr[n]=pt; 
						n++;
					}
				}
				//计算区域内部的面积ptNum
				if(n>4)
				{
					if(rgn.CreatePolygonRgn(&ptAr[0],n,ALTERNATE))
					{
						rgn.GetRgnBox(&rect);
						//计算面积
						n=0;tmp=0;
						for(y=rect.top; y<=rect.bottom; y++)
						{
							bRead=TRUE;
							for(x=rect.left; x<=rect.right; x++)
							{
								if(rgn.PtInRegion(CPoint(x,y)))
								{
									ptNum++;//计算面积
									//计算联通指数
									if(bRead)
									{
										inFID->SeekToDepth((float)m_pPickupObj->YCoorToDepth(y));
										inFID->Read(3,DataNum,pData);
										bRead=FALSE;
									}
									m=(x-m_pPickupObj->m_Rect.left)*DataNum/m_pPickupObj->m_Rect.Width();
									if(m>-1&&m<DataNum)
									{
										if(pData[m]>0.f)
										{
											tmp+=(pData[m]-m_ImageMin)/(m_ImageMax-m_ImageMin);
											n++;
										}
									}
								}
							}
						}
						rgn.DeleteObject();
						//计算联通指数
						Rx=rect.Width()*m_PickupHorScale*0.254f;
						Ry=rect.Height()*m_PickupVerScale*0.254f;
						if(n>0)
						{
							Rx=Rx*tmp/n;
							Ry=Ry*tmp/n;
						}
						perh+=Rx;
						perv+=Ry;

						ptNum=(int)(ptNum*m_PickupVerScale*0.254f);
						ptNum=(int)(ptNum*m_PickupHorScale*0.254f);
						
						//标准井线性刻度
						ptNum=ptNum*m_fA+m_fB;
						area+=ptNum;

						//求最大颗粒孔洞直径
						s=(float)pContour->ComputeArea();
						s=s*m_PickupVerScale*0.254f;
						s=s*m_PickupHorScale*0.254f;

						d=(float)(2.f*sqrt(s/PI));
						if(d>dmax) dmax=d;
						//统计大于最小分辨0.2英寸的孔洞
						if(d<dmin&&d>0.5f) dmin=d;
						dave+=d;

						sumb+=1.f;
						if(d<=m_StatDiam1)
						{
							Hpor1+=ptNum;Sumb1+=1.f;
						}
						else if(d<=m_StatDiam2)
						{
							Hpor2+=ptNum;Sumb2+=1.f;
						}
						else if(d<=m_StatDiam3)
						{
							Hpor3+=ptNum;Sumb3+=1.f;
						}
						else
						{
							Hpor4+=ptNum;Sumb4+=1.f;
						}
					}
				}
				ptAr.SetSize(0);
			}
		}
		if(sumb>0.f)
		{
			pert=(float)sqrt(perh*perh+perv*perv);		
			pert=pert/WinS*area/sumb;				//总联通指数 	 HPERT  单位none
			perh=perh/WinS*area/sumb;				//水平联通指数 	 HPERH  单位none
			perv=perv/WinS*area/sumb;				//垂直联通指数 	 HPERV  单位none

			area=area/WinS*100.f;
			Hpor1=Hpor1/WinS*100.f;
			Hpor2=Hpor2/WinS*100.f;
			Hpor3=Hpor3/WinS*100.f;
			Hpor4=Hpor4/WinS*100.f;
			dave=dave/sumb;
			if(dmin>9999.f)							//最小孔洞直径   DMIN   单位mm
				szStr.Format("\n%s,%.1f--%.1f,%g,%.2f,%.2f,%f,%.3f,%g,%.3f,%g,%.3f,%g,%.3f,%g,%.2f,／,%f,%f",
					pObj->m_NoStr,pObj->m_Sdep,pObj->m_Edep,sumb,area,dave,pert,Hpor1,Sumb1,Hpor2,Sumb2,Hpor3,Sumb3,Hpor4,Sumb4,dmax,perh,perv);
			else
				szStr.Format("\n%s,%.1f--%.1f,%g,%.2f,%.2f,%f,%.3f,%g,%.3f,%g,%.3f,%g,%.3f,%g,%.2f,%f,%f,%f",
					pObj->m_NoStr,pObj->m_Sdep,pObj->m_Edep,sumb,area,dave,pert,Hpor1,Sumb1,Hpor2,Sumb2,Hpor3,Sumb3,Hpor4,Sumb4,dmax,dmin,perh,perv);
		}
		else
			szStr.Format("\n%s,%.1f--%.1f,%g,／,／,／,／,／,／,／,／,／,／,／,／,／,／,／",
					pObj->m_NoStr,pObj->m_Sdep,pObj->m_Edep,sumb);
		File.WriteString(szStr);
		Num++;
		pGuage->SetPos(Num);
		if(pGuage->CheckCancelButton())
			break;
	}
	File.Close();
	delete pGuage;delete inFID;delete pData;
}

void CImageManView::OnUpdatePickupList(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pSelectObj!=NULL&&!m_ContourList.IsEmpty());
}

//裂缝异常带分析
//新建图像特征存储空间并自动拾取裂缝的图像特征
void CImageManView::OnCreateSinContour()
{
	if(!m_pSelectObj->IsKindOf(RUNTIME_CLASS(CImageObj)))
		return;
	m_pPickupObj=(CImageObj*)m_pSelectObj;
	if(m_pPickupObj==NULL) return;

	if(m_ContourList.GetCount()>0&&m_iPickupNum>0)
	{
		if(AfxMessageBox("确认是否保存图像特征\n\n"+m_PickupFileName+"    ",
			MB_YESNO|MB_ICONQUESTION)==IDYES)
			OnSaveContour();
	}
	CSetContourSinPara dlg;
	//拾取层理裂缝的最大宽度mm	
	dlg.m_iPickupSinWidth=m_iPickupSinWidth;
	//拾取层理裂缝类型标识
	dlg.m_sPickupSinLabel=m_sPickupSinLabel;//层理张开缝充填缝压裂缝层间缝局部充填缝
	if(dlg.DoModal()!=IDOK)
		return;
	m_iPickupSinWidth=dlg.m_iPickupSinWidth;
	m_sPickupSinLabel=dlg.m_sPickupSinLabel;

	m_PickupStDep=(float)pDoc->m_PlotSdep;		//新建时的起始深度
	m_PickupEnDep=(float)pDoc->m_PlotEdep;		//新建时的结束深度
	m_PickupRect=m_pPickupObj->m_Rect;          //新建时的图像大小
	m_PickupFIDName=m_pPickupObj->m_strPathName;//曲线全名
	m_PickupAliasName=m_pPickupObj->m_AliasName;//曲线别名
	m_PickupBit=m_pPickupObj->m_Bit;            //钻头直径
	m_PickupHorScale=m_pPickupObj->m_HorScale;  //横向绘图比例
	m_PickupVerScale=(float)pDoc->m_DepthScale; //纵向绘图比例

	//计算自动拾取横向覆盖率
	//电成像极板数Content->min
	//电扣数/极板Content->max
	//一个深度采样写入文件的数据个数
	CFIDIO *pFIDIO = new CFIDIO(m_PickupFIDName);
	if(pFIDIO->Open()) 
	{
		FIDCONTENT* Content = pFIDIO->GetContent(3);
		float ButtonNum=Content->min*Content->max;
		//针对CIFLog处理的成像数据
		if(ButtonNum<10)
		{
			if(pDoc->m_CmdTemplateName.Find("CIFImage")>0)
				ButtonNum=196;
			else ButtonNum=150;
		}
		m_CoverageRate= ButtonNum*1.f/Content->npw;  //自动拾取覆盖率
	}
	delete pFIDIO;

	//计算孔洞最小电导率
	
	if(pDoc->m_CmdTemplateName.Find("CIFImage")>0)
		m_ImageMax=256.f;//成像数据最值 缺省1024，CIFLog处理的为256
	m_ImageMin=m_ImageMax*m_KPara/256.f;

	m_StatSdep=(float)pDoc->m_PlotSdep;
	m_StatEdep=(float)pDoc->m_PlotEdep;
	m_StatWellName=pDoc->m_PathName;
	if(m_StatWellName.ReverseFind('.')>0)
		m_StatWellName=m_StatWellName.Left(m_StatWellName.ReverseFind('.'));

	HBITMAP hBitmap;
	HDIB m_hDIB=NULL;
	CBitmap m_Bitmap;	
	CDC* pDrawDC=GetDC();

	BeginWaitCursor();
	POSITION temp=NULL;
	POSITION pos=m_ContourList.GetHeadPosition();	 
	while(pos!=NULL)
	{
		temp=pos;
		m_ContourList.GetNext(pos);
		m_ContourList.RemoveAt(temp);
	}

	CRect rect=m_PickupRect;
	//定义进度指示器
	CGuage* pGuage = new CGuage(0.f,100.f);
	pGuage->m_szGuage="纵坐标";
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->CenterWindow();
	pGuage->ShowWindow(SW_SHOW);
	pGuage->SetTitle("图像特征分析...");		
	pGuage->SetRange(rect.top,rect.bottom);

	float area;
	CContour* pContour=NULL;
	while(rect.top<=m_PickupRect.bottom)
	{
		rect.bottom=rect.top+1600;
		if(rect.bottom>m_PickupRect.bottom)
			rect.bottom=m_PickupRect.bottom;
		pGuage->SetPos(rect.bottom);
		if(pGuage->CheckCancelButton())
			break;
		//局部绘图获取BMP图像
		m_pPickupObj->DrawBmp(pDrawDC,&m_Bitmap,rect);
	
		hBitmap = (HBITMAP)m_Bitmap.GetSafeHandle();
		if(!hBitmap)
			continue;	
		m_hDIB = BitmapToDIB(hBitmap,NULL);
		if(m_hDIB==NULL)
			continue;

		if(m_DibImage.Attach(m_hDIB))
		{
			m_DibImage.m_KPara=m_KPara;
			//根据横向绘图比例将实际孔洞面积折算为绘图面积
			//将绘图面积mm2转换为像素
			//标准井线性刻度
			m_DibImage.m_HoleMaxAcr=m_HoleMaxAcr;
			m_DibImage.m_bHoleMaxAcr=m_bHoleMaxAcr;
			m_DibImage.m_HoleMinAcr=m_HoleMinAcr;
			m_DibImage.m_bHoleMinAcr=m_bHoleMinAcr;

			m_DibImage.m_bMinRound=m_bMinRound;
			m_DibImage.m_HoleMinRound=m_HoleMinRound;
			m_DibImage.m_bMaxRound=m_bMaxRound;
			m_DibImage.m_HoleMaxRound=m_HoleMaxRound;
			m_DibImage.m_Hor=m_PickupHorScale;
			m_DibImage.m_Ver=m_PickupVerScale;
			m_DibImage.m_fA=m_fA;
			m_DibImage.m_fB=m_fB;
			if(m_DibImage.GetContour(FALSE))
			{
				int i,k;
				pos=m_DibImage.m_ContourList.GetHeadPosition();
				while(pos!=NULL)
				{
					pContour=m_DibImage.m_ContourList.GetNext(pos);
					k=pContour->m_pointArray.GetCount();
					for(i=0;i<k;i++)
						pContour->m_pointArray[i]+=rect.TopLeft();
					pContour->m_rect.OffsetRect(rect.TopLeft());
					//计算颗粒的直径，单位mm
					area=(float)pContour->ComputeArea();
					area=area*m_PickupVerScale*0.254f;
					area=area*m_PickupHorScale*0.254f;
					//标准井线性刻度
					area=area*m_fA+m_fB;
					pContour->m_Radius=(float)sqrt(area/3.1415926)*2.f;
					m_ContourList.AddTail(pContour);
				}
			}	
		}
		m_Bitmap.DeleteObject ();
		rect.top+=1600;
	}
	pDrawDC->DeleteDC();
	int Num=m_ContourList.GetCount();
	if(Num<1) 
	{
		delete pGuage;
		return;
	}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//单条裂缝探索宽度(绘图坐标)
	int DeltaY=m_iPickupSinWidth;
	DeltaY=pDoc->DepthToYCoor(1000.+DeltaY/1000.)-pDoc->DepthToYCoor(1000.);
	DeltaY/=2;

	CPoint pt,OffPt;
	OffPt.x=m_pPickupObj->m_Rect.left-m_PickupRect.left;
	OffPt.y=pDoc->DepthToYCoor(m_PickupStDep)-pDoc->DepthToYCoor(pDoc->m_PlotSdep);
	OffPt.y+=m_pPickupObj->m_Rect.top-m_PickupRect.top;
	
	//纵横向不同绘图比例校正系数
	double Her,Ver;
	Her=m_pPickupObj->m_Rect.Width()*1.0/m_PickupRect.Width();
	Ver=m_PickupVerScale/pDoc->m_DepthScale;
	
	
	pGuage->SetTitle("裂缝异常特征拾取...");		
	pGuage->SetRange(0,Num);

	//所有层理裂缝区域表
	CObList SinContourList;
	SinContourList.RemoveAll();
	CContour *pNew=NULL;
	CSinPoint* pObj=NULL;
	
	int i,k,m,n;
	CRgn rgn;
	
	CArray<CPoint,CPoint&>ptArray;
	CArray<CPoint,CPoint&>ptAr;

	Num=0;
	pos=m_ContourList.GetHeadPosition();
	while(pos!=NULL)
	{	
		Num++;
		pGuage->SetPos(Num);
		if(pGuage->CheckCancelButton())
			break;
		pContour=(CContour*)m_ContourList.GetNext(pos);
		k=pContour->m_pointArray.GetSize();
		if(k>4)
		{
			temp= pDoc->m_SinObjList.GetHeadPosition();
			while(temp!= NULL)
			{
				pObj=(CSinPoint*) pDoc->m_SinObjList.GetNext(temp);
				if(m_sPickupSinLabel.Find(pObj->m_Flag)<0)
					continue;
				m=pObj->m_nPoints;
				ptArray.SetSize(m*2,5);
				for(i=0;i<m;i++)
				{
					ptArray[i].x=pObj->m_Points[i].x;
					ptArray[i].y=pObj->m_Points[i].y-DeltaY;
					ptArray[m*2-1-i].x=pObj->m_Points[i].x;
					ptArray[m*2-1-i].y=pObj->m_Points[i].y+DeltaY;
				}
				if(rgn.CreatePolygonRgn(&ptArray[0],m*2,ALTERNATE))
				{
					n=0;
					ptAr.SetSize(k,5);
					for(i=0;i<k;i++)
					{
						pt=pContour->m_pointArray[i];
						//进行纵横向不同绘图比例校正
						pt.x=m_PickupRect.left+(int)((pt.x-m_PickupRect.left)*Her);
						pt.y=m_PickupRect.top+(int)((pt.y-m_PickupRect.top)*Ver);
						//进行整体偏移
						pt.Offset(OffPt);
						if(rgn.PtInRegion(pt.x,pt.y))
						{
							ptAr[n]=pt; 
							n++;
						}
					}
					//计算区域内部的面积ptNum
					if(n>3)
					{
						pNew=new CContour(TRUE);
						for(i=0;i<n;i++)
						{
							pt=ptAr[i];
							pNew->m_pointArray.Add(pt); 
							if(i==0)
							{
								pNew->m_rect.left=pNew->m_rect.right=pt.x;
								pNew->m_rect.top=pNew->m_rect.bottom=pt.y;
							}
							else
							{
								if(pt.x<pNew->m_rect.left)
									pNew->m_rect.left=pt.x;
								if(pt.x>pNew->m_rect.right)
									pNew->m_rect.right=pt.x;
								if(pt.y<pNew->m_rect.top)
									pNew->m_rect.top=pt.y;
								if(pt.y>pNew->m_rect.bottom)
									pNew->m_rect.bottom=pt.y;
							}
						}
						if(n>4)
							SinContourList.AddTail(pNew);
						else
						{
							delete pNew;
							pNew=NULL;
						}
					}
					rgn.DeleteObject();
					ptAr.SetSize(0);
				}
				ptArray.SetSize(0);
			}
		}	
	}
	delete pGuage;

	//删除已有的颗粒或孔洞
	pos=m_ContourList.GetHeadPosition();	 
	while(pos!=NULL)
	{
		temp=pos;
		m_ContourList.GetNext(pos);
		m_ContourList.RemoveAt(temp);
	}
	
	pos=SinContourList.GetHeadPosition();	 
	while(pos!=NULL)
	{
		pContour=(CContour*)SinContourList.GetNext(pos);
		m_ContourList.AddTail(pContour);
	}

	//删除已有的裂缝
	pos=SinContourList.GetHeadPosition();	 
	while(pos!=NULL)
	{
		temp=pos;
		SinContourList.GetNext(pos);
		SinContourList.RemoveAt(temp);
	}
	m_PickupStDep=(float)pDoc->m_PlotSdep;		//新建时的起始深度
	m_PickupEnDep=(float)pDoc->m_PlotEdep;		//新建时的结束深度
	m_PickupRect=m_pPickupObj->m_Rect;          //新建时的图像大小
	m_PickupFIDName=m_pPickupObj->m_strPathName;//曲线全名
	m_PickupAliasName=m_pPickupObj->m_AliasName;//曲线别名
	m_PickupBit=m_pPickupObj->m_Bit;            //钻头直径
	m_PickupHorScale=m_pPickupObj->m_HorScale;  //横向绘图比例
	m_PickupVerScale=(float)pDoc->m_DepthScale; //纵向绘图比例
	m_StatSdep=(float)pDoc->m_PlotSdep;
	m_StatEdep=(float)pDoc->m_PlotEdep;
	m_StatWellName=pDoc->m_PathName;
		
	EndWaitCursor();
	m_iPickupNum++;
	Invalidate();
}
void CImageManView::OnUpdateCreateSinContour(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pSelectObj!=NULL&&!pDoc->m_SinObjList.IsEmpty());
}

/*
 *裂缝储层参数分类统计分析
 *
 *绘图映射模式:纵横向1英寸=100像素点 
 *纵横向1个像素=0.254mm
 *横向绘图比例:m_HorScale=(int)(m_Bit*PI/(m_Rect.Width()*0.254));
 *m_Bit 单位 mm
 *自动拾取覆盖率:m_CoverageRate= ButtonNum/Content->npw;
 *拾取图像宽度:m_CoverageRate*m_Rect.Width();
*/
/*
 *计算全井段裂缝储层参数统计，生成曲线
 *
 *裂缝长度  该裂缝在窗口内的长度和进入窗口的高度成线性关系
 *            FVTL=l*h1/h
 *                 l:单条裂缝长度
 *                 h1:该裂缝进入统计窗口的高度
 *                 h:该裂缝的高度
 *裂缝宽度            
 *            FVA=所有裂缝宽度的3次方均值
 *裂缝密度            
 *            FVDA=每米井段裂缝条数
 *裂缝水动力宽度      FVAH
 *裂缝孔隙度          FVPA 
*/
void CImageManView::OnPickupSinStat()
{
	//计算单条裂缝储层参数，赋给CSinPoint对象
	if(StatSinContour())
	{
		if(m_SinStatSdep<0.f)
		{
			m_SinStatSdep=(float)pDoc->m_PlotSdep;
			m_SinStatSdep=(float)pDoc->m_PlotEdep;
			m_StatWellName=pDoc->m_PathName;
		}

		if(m_StatWellName.ReverseFind('.')>-1)
			m_StatWellName=m_StatWellName.Left(m_StatWellName.ReverseFind('.'));
		float win12=0.5f;//m_SinStatWin/2.f;

		//一个统计窗口的井壁面积mm2
		float WinS=(float)PI*m_pPickupObj->m_Bit*(2.f*win12*1000.f);
		//一个统计窗口的井壁面积m2
		float WinSm=(float)PI*(m_pPickupObj->m_Bit/1000.f)*(2.f*win12);
		
		//自动拾取覆盖率m_CoverageRate
		WinS=WinS*m_CoverageRate;
		WinSm=WinSm*m_CoverageRate;

		float StartDep=((int)(m_SinStatSdep/m_SinStatRlev))*m_SinStatRlev;
		float EndDep=((int)(m_SinStatEdep/m_SinStatRlev))*m_SinStatRlev;

		//初始化输出曲线
		CLogProcess *pLog= new CLogProcess(m_StatWellName,NULL);
		pLog->StartDep=StartDep;
		pLog->EndDep=EndDep;
		pLog->DepLevel=m_SinStatRlev;
		//初始化输出
		OUTCURVE *pOutCurve = new OUTCURVE[5];
		strcpy(pOutCurve[0].name,"FVTL");
		strcpy(pOutCurve[1].name,"FVA");
		strcpy(pOutCurve[2].name,"FVAH");
		strcpy(pOutCurve[3].name,"FVDA");
		strcpy(pOutCurve[4].name,"FVPA");
		float *OutXX=NULL;
		OutXX = new float[5];
		pLog->InitOut(5,pOutCurve,(char *)OutXX);
		pLog->InitBuffer();

		CRect rect,rect1,rect2;
		float FVTL,FVA,FVAH,FVDA,FVPA;

		CGuage* pGuage = new CGuage(pLog->StartDep,pLog->EndDep);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->CenterWindow();
		pGuage->ShowWindow(SW_SHOW);
		pGuage->SetTitle("生成全井段裂缝描述曲线...");

		CPoint pt,OffPt;
		OffPt.x=m_pPickupObj->m_Rect.left-m_PickupRect.left;
		OffPt.y=pDoc->DepthToYCoor(m_PickupStDep)-pDoc->DepthToYCoor(pDoc->m_PlotSdep);
		OffPt.y+=m_pPickupObj->m_Rect.top-m_PickupRect.top;
		
		//纵横向不同绘图比例校正系数
		double Her,Ver;
		Her=m_pPickupObj->m_Rect.Width()*1.0/m_PickupRect.Width();
		Ver=m_PickupVerScale/pDoc->m_DepthScale;

		CArray<CPoint,CPoint&>ptAr;
		int x,y,ptNum;
		float SinH,tl,area,va;

		
		CRgn rgn;
		int i,k,n,DPN=0;
		float dep=StartDep;

		POSITION pos=NULL;
		CContour* pContour=NULL;
		while(dep<EndDep)
		{
	///////////////////////////////////////////////////////////////////////
			//统计一个窗口内的裂缝储层参数
			rect=m_pPickupObj->m_Rect;
			rect.top=m_pPickupObj->DepthToYCoor(dep-win12);
			rect.bottom=m_pPickupObj->DepthToYCoor(dep+win12);

			FVTL=FVA=FVAH=FVDA=FVPA=0.f;
			pos= pDoc->m_SinObjList.GetHeadPosition();
			while(pos!= NULL) 
			{
 				CSinPoint *pSin=(CSinPoint *)pDoc->m_SinObjList.GetNext(pos);
				if(pSin==NULL) continue;
				//是否符合统计条件
				if(m_sPickupSinLabel.Find(pSin->m_Flag)<0)
					continue;
				rect1=m_pPickupObj->m_Rect;//pSin->m_Rect;
				SinH=m_pPickupObj->m_Bit/2000.f*(float)(tan(pSin->m_Dip0*PI/180.));
				rect1.top=m_pPickupObj->DepthToYCoor(pSin->m_Dep-SinH);
				rect1.bottom=m_pPickupObj->DepthToYCoor(pSin->m_Dep+SinH);
				rect2=rect&rect1;
				if(rect2.Height()>2)
				{
					FVDA+=1.f;
					//该裂缝在窗口内的长度和进入窗口的高度成线性关系
					tl=pSin->FVTL*rect2.Height()/rect1.Height();
					FVTL+=tl;
					//裂缝宽度=所有裂缝宽度的3次方均值
					va=(float)pow(pSin->FVA,3.f);
					FVA+=va;
					//水动力宽度
					va=(float)pow(pSin->FVAH,3.f);
					FVAH+=va;
				}	
			}
			//FVTL长度：每平米面积内的长度
			OutXX[0]=FVTL/WinSm;
			//计算裂缝平均宽度
			FVA/=3.f;
			FVA=(float)pow(FVA,1.f/3.f);
			//标准井线性刻度
			OutXX[1]=FVA*m_fC+m_fD;
			
			//计算水动力宽度
			FVAH/=3.f;
			FVAH=(float)pow(FVAH,1.f/3.f);
			//标准井线性刻度
			OutXX[2]=FVAH*m_fC+m_fD;
			//FVDA密度：每米井段裂缝条数
			OutXX[3]=FVDA/(win12*2.f);
	///////////////////////////////////////////////////////////////////////
			rect=m_pPickupObj->m_Rect;
			rect.top=m_pPickupObj->DepthToYCoor(dep-win12);
			rect.bottom=m_pPickupObj->DepthToYCoor(dep+win12);
			
			//计算该裂缝区域图像特惩总面积area
			area=0.f;
			pos=m_ContourList.GetHeadPosition();
			while(pos!=NULL)
			{	
				pContour=(CContour*)m_ContourList.GetNext(pos);
				k=pContour->m_pointArray.GetSize();
				//计算单个图像特征面积ptNum
				if(k>4)
				{
					n=0;
					ptAr.SetSize(k,5);
					for(i=0;i<k;i++)
					{
						pt=pContour->m_pointArray[i];
						//进行纵横向不同绘图比例校正
						pt.x=m_PickupRect.left+(int)((pt.x-m_PickupRect.left)*Her);
						pt.y=m_PickupRect.top+(int)((pt.y-m_PickupRect.top)*Ver);
						//进行整体偏移
						pt.Offset(OffPt);
						if(rect.PtInRect(pt))
						{
							ptAr[n]=pt; 
							n++;
						}
					}
					//计算区域内部的面积ptNum
					if(n>4)
					{
						ptNum=0;
						if(rgn.CreatePolygonRgn(&ptAr[0],n,ALTERNATE))
						{
							rgn.GetRgnBox(&rect2);
							for(y=rect2.top; y<=rect2.bottom; y++)
								for(x=rect2.left; x<=rect2.right; x++)
									if(rgn.PtInRegion(CPoint(x,y)))
										ptNum++;
							area+=ptNum;
							rgn.DeleteObject();
						}
					}
					ptAr.SetSize(0);
				}	
			}
			
			area=area*m_PickupVerScale*0.254f;
			area=area*m_PickupHorScale*0.254f;
			//标准井线性刻度
			area=area*m_fA+m_fB;	
			//计算窗口内裂缝孔隙度FVPA	
			FVPA=area/WinS*100.f;
			OutXX[4]=FVPA;

			pLog->Dep=dep;
			pLog->DatOut();

			DPN++;
			dep=StartDep+DPN*m_SinStatRlev;
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton())
				break;
		}
		delete OutXX;
		delete pOutCurve;
		delete pLog;

		delete pGuage;
		pDoc->WellRefresh();
	}
}

void CImageManView::OnUpdatePickupSinStat(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pPickupObj!=NULL&&!pDoc->m_SinObjList.IsEmpty()&&!m_ContourList.IsEmpty());
}

/*
 *计算单条裂缝储层参数统计，赋给CSinPoint对象
 *裂缝储层参数单位 长度m 宽度mm 水动力宽度mm
 *
 *计算裂缝长度 FVTL=椭圆周长
 *计算椭圆周长 L=2PI*b+4(a-b)
 *             a:椭圆长半轴长 b:椭圆短半轴长
 *计算裂缝宽度 FVA=S/L
 *             S:裂缝面积 L:裂缝长度
 *             自动拾取覆盖率FVA=FVA/CoverageRate
 *             
 *计算裂缝面积 FVAR=∑Scontour
 *             裂缝带中所有图像特征面积和
*/
BOOL CImageManView::StatSinContour()
{
	//图中绘制的总裂缝图
	int Num=pDoc->m_SinObjList.GetCount();
	if(Num<1)  return FALSE;

	int DeltaY=m_iPickupSinWidth;
	DeltaY=pDoc->DepthToYCoor(1000.+DeltaY/1000.)-pDoc->DepthToYCoor(1000.);
	DeltaY/=2;

	CPoint pt,OffPt;
	OffPt.x=m_pPickupObj->m_Rect.left-m_PickupRect.left;
	OffPt.y=pDoc->DepthToYCoor(m_PickupStDep)-pDoc->DepthToYCoor(pDoc->m_PlotSdep);
	OffPt.y+=m_pPickupObj->m_Rect.top-m_PickupRect.top;
	
	//纵横向不同绘图比例校正系数
	double Her,Ver;
	Her=m_pPickupObj->m_Rect.Width()*1.0/m_PickupRect.Width();
	Ver=m_PickupVerScale/pDoc->m_DepthScale;

	CArray<CPoint,CPoint&>ptArray;
	CArray<CPoint,CPoint&>ptAr;
	
	//打开浅侧向电阻率曲线
	BOOL bOpen=FALSE;
	CFIDIO* pFID = NULL;
	float dep0,rlev0,fRxo;
	CString szStr=m_sRxoWell;
	int cx=szStr.Find(".");
	if(cx>0) 
		szStr=szStr.Left(cx+1);
	else
		szStr+=".";
	pFID = new CFIDIO(szStr+m_sRxoName);
	if(pFID->Open()) 
	{
		bOpen=TRUE;
		rlev0=pFID->GetDepthLevel();
	}
	else AfxMessageBox(szStr+m_sRxoName+"\n曲线打开失败!      ");

	//计算单条裂缝面积
	int x,y,ptNum;
	float area;

	float SinH;//裂缝在垂向上的跨越井段
	float dia=(float)m_pPickupObj->m_Bit;//井眼直径mm  用钻头直径替代
	float lAxis,sAxis;//椭圆长、短半轴长

	CRect rect; 
	CRgn rgn,rgn1;
	int i,k,m,n;
	CContour* pContour=NULL;
	CSinPoint* pObj=NULL;
	POSITION pos,temp;

	Num=0;
	temp= pDoc->m_SinObjList.GetHeadPosition();
	while(temp!= NULL)
	{
		Num++;
		pObj=(CSinPoint*) pDoc->m_SinObjList.GetNext(temp);
		if(m_sPickupSinLabel.Find(pObj->m_Flag)<0)
			continue;
		m=pObj->m_nPoints;
		ptArray.SetSize(m*2,5);
		//单条裂缝区域
		for(i=0;i<m;i++)
		{
			ptArray[i].x=pObj->m_Points[i].x;
			ptArray[i].y=pObj->m_Points[i].y-DeltaY;
			ptArray[m*2-1-i].x=pObj->m_Points[i].x;
			ptArray[m*2-1-i].y=pObj->m_Points[i].y+DeltaY;
		}
		//计算单条裂缝区域图像特征总面积
		area=0;
		if(rgn.CreatePolygonRgn(&ptArray[0],m*2,ALTERNATE))
		{
			pos=m_ContourList.GetHeadPosition();
			//计算该裂缝区域图像特惩总面积area
			while(pos!=NULL)
			{	
				pContour=(CContour*)m_ContourList.GetNext(pos);
				k=pContour->m_pointArray.GetSize();
				//计算单个图像特征面积ptNum
				if(k>4)
				{
					n=0;
					ptAr.SetSize(k,5);
					for(i=0;i<k;i++)
					{
						pt=pContour->m_pointArray[i];
						pt.x=m_PickupRect.left+(int)((pt.x-m_PickupRect.left)*Her);
						pt.y=m_PickupRect.top+(int)((pt.y-m_PickupRect.top)*Ver);
						pt.Offset(OffPt);
						if(rgn.PtInRegion(pt.x,pt.y))
						{
							ptAr[n]=pt; 
							n++;
						}
					}
					//计算区域内部的面积ptNum
					if(n>4)
					{
						ptNum=0;
						if(rgn1.CreatePolygonRgn(&ptAr[0],n,ALTERNATE))
						{
							rgn1.GetRgnBox(&rect);
							for(y=rect.top; y<=rect.bottom; y++)
								for(x=rect.left; x<=rect.right; x++)
									if(rgn1.PtInRegion(CPoint(x,y)))
										ptNum++;
							area+=ptNum;
							rgn1.DeleteObject();
						}
					}
					ptAr.SetSize(0);
				}	
			}
			rgn.DeleteObject();
		}
		ptArray.SetSize(0);

		//计算单条裂缝储层参数
		//计算裂缝长度m
		SinH=dia*(float)(tan(pObj->m_Dip0*PI/180.));//裂缝在垂向上的跨越井段
		lAxis=sqrt(SinH*SinH+dia*dia);//椭圆长半轴长
		lAxis/=2.f;
		sAxis=dia/2.f;//椭圆短半轴长
		pObj->FVTL=(float)(2.f*PI*lAxis-4.f*(lAxis-sAxis));
		//计算裂缝宽度mm
		area*=m_PickupVerScale*0.254f;
		area*=m_PickupHorScale*0.254f;
		
		area=area*m_fA+m_fB;
		pObj->FVA=area/pObj->FVTL;
		//自动拾取覆盖率m_CoverageRate
		pObj->FVA/=m_CoverageRate;
		
		//裂缝宽度刻度井指数函数回归
		/*pObj->FVA=0.27f*exp(0.15f*pObj->FVA);*/
		pObj->FVA/=m_iPickupSinWidth;
		//计算裂缝水动力宽度
		if(bOpen)
		{
			dep0=((int)(pObj->m_Dep/rlev0))*rlev0;
			pFID->SeekToDepth(dep0);
			pFID->Read(2,1,&fRxo);
			pObj->FVAH=m_ParC*area*pow(m_Rm,m_ParB)*pow(fRxo,1.f-m_ParB);
			//裂缝宽度刻度井指数函数回归
			/*pObj->FVAH=0.27f*exp(0.15f*pObj->FVAH);*/
			pObj->FVAH/=m_iPickupSinWidth;
		}
		else
			pObj->FVAH=0.f;

		pObj->FVTL/=1000.f;//折算长度单位m		
		//裂缝宽度刻度
		pObj->FVA=pObj->FVA*m_fC+m_fD;
		pObj->FVAH=pObj->FVAH*m_fC+m_fD;
	}
	delete pFID;
	return TRUE;
}

/*在综合解释表给定的井段内对层理裂缝进行统计并生成统计表
 *
 *按解释结论对象生成层理裂缝统计表ReCrack.List
 *
 *先计算单条裂缝储层参数，赋给CSinPoint对象
 *后根据综合解释标注CResultPoint对象的井段统计层理裂缝图像特征面积
*/
void CImageManView::OnReCrackList()
{
	if(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CResultObj)))
	{
		CResultObj *pResult=(CResultObj*)m_pSelectObj;
		if(pResult==NULL) return;
		CString HolesFile="ReCrack.List";
		static char BASED_CODE szFilter[] = 
		"裂缝统计表(*.List)|*.List|所有文件(*.*)|*.*||";
		CFileDialog  Browse(FALSE, "裂缝统计表(*.List)",HolesFile, 
							OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
		Browse.m_ofn.lpstrTitle = "保存文件";
		CString Path=m_StatWellName;
		Path=Path.Left(Path.ReverseFind('\\'));
		Browse.m_ofn.lpstrInitialDir=Path;
		if(Browse.DoModal()!=IDOK) return;
			
		HolesFile =Browse.GetPathName();
		CString str;
		CStdioFile File;
		if(File.Open(HolesFile,CFile::modeWrite | CFile::modeCreate| CFile::typeText)) 
		{
			//计算单条裂缝储层参数，赋给CSinPoint对象并生成Crack.List
			if(!StatSinContour())
			{
				File.Close();
				return;
			}
			//列表题
			str="序号,深度,裂缝总长度,裂缝总条数,裂缝总宽度,裂缝平均宽度,总水动力宽度,平均水动力宽度,裂缝孔隙度\n";
			File.WriteString(str);
			//单位
			str=",m,m,#,mm,mm,mm,mm,％";
			File.WriteString(str);

			CPoint pt,OffPt;
			OffPt.x=m_pPickupObj->m_Rect.left-m_PickupRect.left;
			OffPt.y=pDoc->DepthToYCoor(m_PickupStDep)-pDoc->DepthToYCoor(pDoc->m_PlotSdep);
			OffPt.y+=m_pPickupObj->m_Rect.top-m_PickupRect.top;
			
			//纵横向不同绘图比例校正系数
			double Her=m_pPickupObj->m_Rect.Width()*1.0/m_PickupRect.Width();
			double Ver=m_PickupVerScale/pDoc->m_DepthScale;

			CArray<CPoint,CPoint&>ptAr;
			
			CRgn rgn;
			int x,y,ptNum;
			int i,k,n,Num;
			CRect SlideRect,rect,rect1,rect2;

			float FVTL,FVA,FVAH,FVDA,FVPA,FVA0,FVAH0;
			float WinS,SinH,area,tl,va;
			//定义进度指示器
			Num=pResult->m_ResultPoint.GetCount();
			CGuage* pGuage = new CGuage(0.f,Num*1.f);
			pGuage->m_szGuage="目标数";
			pGuage->Create(CWnd::GetActiveWindow());
			pGuage->CenterWindow();
			pGuage->ShowWindow(SW_SHOW);
			pGuage->SetRange(0,Num);
			pGuage->SetTitle("生成储层裂缝参数统计表...");

			Num=0;
			CContour* pContour=NULL;
			CResultPoint* pObj=NULL;
			CSinPoint *pSin=NULL;
			POSITION pos = pResult->m_ResultPoint.GetHeadPosition();
			while(pos!=NULL)
			{
				pObj = (CResultPoint*) pResult->m_ResultPoint.GetNext(pos);
				SlideRect=m_pPickupObj->m_Rect;
				SlideRect.top=m_pPickupObj->DepthToYCoor(pObj->m_Sdep);
				SlideRect.bottom=m_pPickupObj->DepthToYCoor(pObj->m_Edep);

				//一个统计窗口的井壁面积mm2
				WinS=(float)(PI*m_pPickupObj->m_Bit*(pObj->m_Edep-pObj->m_Sdep)*1000.f);
				//自动拾取覆盖率m_CoverageRate
				WinS=WinS*m_CoverageRate;

				//统计一个窗口内的裂缝储层参数
				FVTL=FVA=FVAH=FVDA=FVPA=FVA0=FVAH0=0.f;				
				POSITION pos1= pDoc->m_SinObjList.GetHeadPosition();
				while(pos1!= NULL) 
				{
 					pSin=(CSinPoint *)pDoc->m_SinObjList.GetNext(pos1);
					if(pSin==NULL)
						continue;
					//是否符合统计条件
					if(m_sPickupSinLabel.Find(pSin->m_Flag)<0)
						continue;
					rect1=pSin->m_Rect;
					SinH=m_pPickupObj->m_Bit/2000.f*(float)(tan(pSin->m_Dip0*PI/180.));
					rect1.top=m_pPickupObj->DepthToYCoor(pSin->m_Dep-SinH);
					rect1.bottom=m_pPickupObj->DepthToYCoor(pSin->m_Dep+SinH);

					rect2=SlideRect&rect1;
					if(rect2.Height()>1)
					{
						FVDA++;
						//该裂缝在窗口内的长度和进入窗口的高度成线性关系
						tl=pSin->FVTL*rect2.Height()/rect1.Height();
						FVTL+=tl;
						//裂缝总宽度
						FVA+=pSin->FVA;
						//平均宽度=所有裂缝宽度的3次方均值
						va=(float)pow(pSin->FVA,3.f);
						FVA0+=va;
						//水动力宽度
						FVAH+=pSin->FVAH;
						va=(float)pow(pSin->FVAH,3.f);
						FVAH0+=va;
					}		
				}
				//FVTL长度：每平米面积内的长度
				//计算裂缝平均宽度
				FVA0/=3.f;
				FVA0=(float)pow(FVA0,1.f/3.f);
				//计算水动力宽度
				FVAH0/=3.f;
				FVAH0=(float)pow(FVAH0,1.f/3.f);
				//FVDA条数：统计井段内裂缝条数
				//计算裂缝区内图像特征面积area
				area=0.f;
				POSITION pos2=m_ContourList.GetHeadPosition();
				while(pos2!=NULL)
				{	
					pContour=(CContour*)m_ContourList.GetNext(pos2);
					k=pContour->m_pointArray.GetSize();
					//计算单个图像特征面积ptNum
					ptNum=0;
					if(k>4)
					{
						n=0;
						ptAr.SetSize(k,5);
						for(i=0;i<k;i++)
						{
							pt=pContour->m_pointArray[i];
							//进行纵横向不同绘图比例校正
							pt.x=m_PickupRect.left+(int)((pt.x-m_PickupRect.left)*Her);
							pt.y=m_PickupRect.top+(int)((pt.y-m_PickupRect.top)*Ver);
							//进行整体偏移
							pt.Offset(OffPt);
							if(SlideRect.PtInRect(pt))
							{
								ptAr[n]=pt; 
								n++;
							}
						}
						//计算区域内部的面积ptNum
						if(n>2)
						{
							if(rgn.CreatePolygonRgn(&ptAr[0],n,ALTERNATE))
							{
								rgn.GetRgnBox(&rect);
								for(y=rect.top; y<=rect.bottom; y++)
									for(x=rect.left; x<=rect.right; x++)
										if(rgn.PtInRegion(CPoint(x,y)))
											ptNum++;
								rgn.DeleteObject();
								area+=ptNum;
							}
						}
						ptAr.SetSize(0);
					}
				}			
				area*=m_PickupVerScale*0.254f;
				area*=m_PickupHorScale*0.254f;
				//标准井线性刻度
				area=area*m_fA+m_fB;
				//FVPA裂缝孔隙度
				FVPA=area/WinS*100.f;
			
				//序号,深度,裂缝长度,裂缝总条数,裂缝总宽度,裂缝平均宽度,总水动力宽度,平均水动力宽度,裂缝孔隙度
				if(FVDA>0)
					str.Format("\n%s,%.1f--%.1f,%f,%.2f,%f,%f,%f,%f,%.3f",
									pObj->m_NoStr,pObj->m_Sdep,pObj->m_Edep,FVTL,FVDA,FVA,FVA0,FVAH,FVAH0,FVPA);
				else
					str.Format("\n%s,%.1f--%.1f,／,%.2f,／,／,／,／,／",
									pObj->m_NoStr,pObj->m_Sdep,pObj->m_Edep,FVDA);
				File.WriteString(str);
				Num++;
				pGuage->SetPos(Num);
				if(pGuage->CheckCancelButton())
					break;
			}
			File.Close();
			delete pGuage;
		}
	}
	else
		AfxMessageBox("请先选中用于井段参考的综合解释符号标注对象        ");
}

void CImageManView::OnUpdateReCrackList(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pSelectObj!=NULL&&!m_ContourList.IsEmpty());
}

/*根据层理裂缝标注对象生成全井段单条层理裂缝参数统计表
 *
 *计算单条裂缝储层参数统计，赋给CSinPoint对象并生成Crack.List
 *裂缝储层参数单位 长度m 宽度mm 水动力宽度mm
 *
 *计算裂缝长度 FVTL=椭圆周长
 *计算椭圆周长 L=2PI*b+4(a-b)
 *             a:椭圆长半轴长 b:椭圆短半轴长
 *计算裂缝宽度 FVA=S/L
 *             S:裂缝面积 L:裂缝长度
 *             自动拾取覆盖率FVA=FVA/CoverageRate
 *             
 *计算裂缝面积 FVAR=∑Scontour
 *             裂缝带中所有图像特征面积和
*/
void CImageManView::OnCrackList()
{
	//图中绘制的总裂缝图
	int Num=pDoc->m_SinObjList.GetCount();
	if(Num<1)  return;

	CString CrackFile="Crack.List";
	static char BASED_CODE szFilter[] = 
	"单条裂缝参数统计表(*.List)|*.List|所有文件(*.*)|*.*||";
	CFileDialog  Browse(FALSE, "单条裂缝参数统计表(*.List)",CrackFile, 
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	Browse.m_ofn.lpstrTitle = "保存文件";
	CString Path=m_StatWellName;
	Path=Path.Left(Path.ReverseFind('\\')+1);
	Browse.m_ofn.lpstrInitialDir=Path;
	if(Browse.DoModal()!=IDOK) return;
		
	CrackFile==Browse.GetPathName();
	int DeltaY=m_iPickupSinWidth;
	DeltaY=pDoc->DepthToYCoor(1000.+DeltaY/1000.)-pDoc->DepthToYCoor(1000.);
	DeltaY/=2;

	CPoint pt,OffPt;
	OffPt.x=m_pPickupObj->m_Rect.left-m_PickupRect.left;
	OffPt.y=pDoc->DepthToYCoor(m_PickupStDep)-pDoc->DepthToYCoor(pDoc->m_PlotSdep);
	OffPt.y+=m_pPickupObj->m_Rect.top-m_PickupRect.top;
	
	//纵横向不同绘图比例校正系数
	double Her,Ver;
	Her=m_pPickupObj->m_Rect.Width()*1.0/m_PickupRect.Width();
	Ver=m_PickupVerScale/pDoc->m_DepthScale;

	CArray<CPoint,CPoint&>ptArray;
	CArray<CPoint,CPoint&>ptAr;
	//定义进度指示器
	CGuage* pGuage = new CGuage(0.f,Num*1.f);
	pGuage->m_szGuage="目标数";
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->CenterWindow();
	pGuage->ShowWindow(SW_SHOW);
	pGuage->SetTitle("生成全井段单条裂缝参数统计表...");		
	pGuage->SetRange(0,Num);

	//打开浅侧向电阻率曲线
	BOOL bOpen=FALSE;
	CFIDIO* pFID = NULL;
	float dep0,rlev0,fRxo;
	CString szStr=m_sRxoWell;
	int cx=szStr.Find(".");
	if(cx>0) 
		szStr=szStr.Left(cx+1);
	else
		szStr+=".";
	pFID = new CFIDIO(szStr+m_sRxoName);
	if(pFID->Open()) 
	{
		bOpen=TRUE;
		rlev0=pFID->GetDepthLevel();
	}
	else AfxMessageBox(szStr+m_sRxoName+"\n曲线打开失败!      ");

	//计算单条裂缝面积
	int x,y,ptNum;
	float area;

	float SinH;//裂缝在垂向上的跨越井段
	float dia=(float)m_pPickupObj->m_Bit;//井眼直径mm  用钻头直径替代
	float lAxis,sAxis;//椭圆长、短半轴长

	CRect rect; 
	CRgn rgn,rgn1;
	int i,k,m,n;
	CContour* pContour=NULL;
	CSinPoint* pObj=NULL;
	POSITION pos,temp;

	CStdioFile File;
	if(File.Open(CrackFile, CFile::modeWrite | CFile::modeCreate| CFile::typeText)) 
	{
		int nb=1;
		szStr="序号,深度,裂缝角度,裂缝方位,裂缝类型,裂缝长度,裂缝宽度,水动力宽度\n";
		File.WriteString(szStr);
		szStr="单位,米,度,度,,米,毫米,毫米";
		File.WriteString(szStr);
		Num=0;
		temp= pDoc->m_SinObjList.GetHeadPosition();
		while(temp!= NULL)
		{
			Num++;
			pGuage->SetPos(Num);
			if(pGuage->CheckCancelButton())
				break;
			pObj=(CSinPoint*) pDoc->m_SinObjList.GetNext(temp);
			if(m_sPickupSinLabel.Find(pObj->m_Flag)<0)
				continue;
			m=pObj->m_nPoints;
			ptArray.SetSize(m*2,5);
			for(i=0;i<m;i++)
			{
				ptArray[i].x=pObj->m_Points[i].x;
				ptArray[i].y=pObj->m_Points[i].y-DeltaY;
				ptArray[m*2-1-i].x=pObj->m_Points[i].x;
				ptArray[m*2-1-i].y=pObj->m_Points[i].y+DeltaY;
			}

			//计算单条裂缝区域图像特征总面积
			area=0;
			if(rgn.CreatePolygonRgn(&ptArray[0],m*2,ALTERNATE))
			{
				pos=m_ContourList.GetHeadPosition();
				//计算该裂缝区域图像特惩总面积area
				while(pos!=NULL)
				{	
					pContour=(CContour*)m_ContourList.GetNext(pos);
					m=0;
					k=pContour->m_pointArray.GetSize();
					//计算单个图像特征面积ptNum
					if(k>4)
					{
						n=0;
						ptAr.SetSize(k,5);
						for(i=0;i<k;i++)
						{
							pt=pContour->m_pointArray[i];
							pt.x=m_PickupRect.left+(int)((pt.x-m_PickupRect.left)*Her);
							pt.y=m_PickupRect.top+(int)((pt.y-m_PickupRect.top)*Ver);
							pt.Offset(OffPt);
							if(rgn.PtInRegion(pt.x,pt.y))
							{
								ptAr[n]=pt; 
								n++;
							}
						}
						//计算区域内部的面积ptNum
						if(n>4)
						{
							ptNum=0;
							if(rgn1.CreatePolygonRgn(&ptAr[0],n,ALTERNATE))
							{
								rgn1.GetRgnBox(&rect);
								for(y=rect.top; y<=rect.bottom; y++)
									for(x=rect.left; x<=rect.right; x++)
										if(rgn1.PtInRegion(CPoint(x,y)))
											ptNum++;
								area+=ptNum;
								rgn1.DeleteObject();
							}
						}
						ptAr.SetSize(0);
					}	
				}
				rgn.DeleteObject();
			}
			ptArray.SetSize(0);

			//计算单条裂缝储层参数
			//计算裂缝长度m
			SinH=dia*(float)(tan(pObj->m_Dip0*PI/180.));//裂缝在垂向上的跨越井段
			lAxis=sqrt(SinH*SinH+dia*dia);//椭圆长半轴长
			lAxis/=2.f;
			sAxis=dia/2.f;//椭圆短半轴长
			pObj->FVTL=(float)(2.f*PI*lAxis-4.f*(lAxis-sAxis));
			//计算裂缝宽度mm
			area*=m_PickupVerScale*0.254f;
			area*=m_PickupHorScale*0.254f;
			//标准井线性刻度
			area=area*m_fA+m_fB;
			pObj->FVA=area/pObj->FVTL;
			//自动拾取覆盖率m_CoverageRate
			pObj->FVA/=m_CoverageRate;
			//标准井线性刻度
			pObj->FVA=pObj->FVA*m_fC+m_fD;
			//裂缝宽度刻度井指数函数回归
			/*pObj->FVA=0.27f*exp(0.15f*pObj->FVA);*/
			pObj->FVA/=m_iPickupSinWidth;
			//计算裂缝水动力宽度
			if(bOpen)
			{
				dep0=((int)(pObj->m_Dep/rlev0))*rlev0;
				pFID->SeekToDepth(dep0);
				pFID->Read(2,1,&fRxo);
				pObj->FVAH=m_ParC*area*pow(m_Rm,m_ParB)*pow(fRxo,(1.f-m_ParB));
				//标准井线性刻度
				pObj->FVAH=pObj->FVAH*m_fC+m_fD;
				//裂缝宽度刻度井指数函数回归
				/*pObj->FVAH=0.27f*exp(0.15f*pObj->FVAH);*/
				pObj->FVAH/=m_iPickupSinWidth;
			}
			else
				pObj->FVAH=0.f;
			pObj->FVTL/=1000.f;//折算长度单位m

			szStr.Format("\n%d,%g,%g,%g,%s,%g,%g,%g",
					nb,pObj->m_Dep,pObj->m_Dip0,pObj->m_Dir0,pObj->m_Flag,pObj->FVTL,pObj->FVA,pObj->FVAH);
			File.WriteString(szStr);
			nb++;
		}
		File.Close();
	}
	delete pFID;
	delete pGuage;
}

void CImageManView::OnUpdateCrackList(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pPickupObj!=NULL&&!pDoc->m_SinObjList.IsEmpty()&&!m_ContourList.IsEmpty());
}
//设置孔洞统计参数
void CImageManView::OnSetholeStatPara()
{
	//if(m_StatSdep<0.f||m_StatSdep<0.f)
	{
		m_StatSdep=(float)pDoc->m_PlotSdep;
		m_StatEdep=(float)pDoc->m_PlotEdep;
		m_StatWellName=pDoc->m_PathName;
		if(m_StatWellName.ReverseFind('.')>-1)
			m_StatWellName=m_StatWellName.Left(m_StatWellName.ReverseFind('.'));
	}
	
	CContourStatDlg dlg;
	dlg.m_WellName=m_StatWellName;
	dlg.m_StatWin=m_StatWin;
	dlg.m_StatRlev=m_StatRlev;
	dlg.m_nSpcNum=m_nSpcNum;
	dlg.m_ContourMax=m_ContourMax;
	dlg.m_StatDiam1=m_StatDiam1;
	dlg.m_StatDiam2=m_StatDiam2;
	dlg.m_StatDiam3=m_StatDiam3;

	dlg.m_fA=m_fA;
	dlg.m_fB=m_fB;
	dlg.m_fC=m_fC;
	dlg.m_fD=m_fD;

	dlg.m_ImageMax=m_ImageMax;
	dlg.m_ImageMin=m_ImageMin;
	dlg.m_StatSdep=m_StatSdep;
	dlg.m_StatEdep=m_StatEdep;
	if(dlg.DoModal()!=IDOK)
		return;
	m_StatWellName=dlg.m_WellName;
	m_StatWin=dlg.m_StatWin;
	m_StatRlev=dlg.m_StatRlev;
	m_nSpcNum=dlg.m_nSpcNum;
	m_ContourMax=dlg.m_ContourMax;
	m_StatDiam1=dlg.m_StatDiam1;
	m_StatDiam2=dlg.m_StatDiam2;
	m_StatDiam3=dlg.m_StatDiam3;
	m_StatSdep=dlg.m_StatSdep;
	m_StatEdep=dlg.m_StatEdep;
	m_fA=dlg.m_fA;
	m_fB=dlg.m_fB;
	m_fC=dlg.m_fC;
	m_fD=dlg.m_fD;
	m_ImageMax=dlg.m_ImageMax;
	m_ImageMin=dlg.m_ImageMin;
	m_iPickupNum++;
	Invalidate();
}
//设置裂缝统计参数
void CImageManView::OnSetCrackStatPara()
{
	if(m_SinStatSdep<0.f||m_SinStatEdep<0.f)
	{
		m_SinStatSdep=(float)pDoc->m_PlotSdep;
		m_SinStatEdep=(float)pDoc->m_PlotEdep;
		m_sRxoWell=pDoc->m_PathName;
		if(m_sRxoWell.ReverseFind('.')>-1)
			m_sRxoWell=m_sRxoWell.Left(m_sRxoWell.ReverseFind('.'));
	}
	CContourSinStatDlg dlg;	
	dlg.m_sRxoWell=m_sRxoWell;//标准电阻率井名
	dlg.m_sRxoName=m_sRxoName;//浅侧向曲线
	dlg.m_SinStatWin=m_SinStatWin;
	dlg.m_SinStatRlev=m_SinStatRlev;
	dlg.m_ParC=m_ParC;
	dlg.m_ParB=m_ParB;
	dlg.m_Rm=m_Rm;
	dlg.m_SinStatSdep=m_SinStatSdep;
	dlg.m_SinStatEdep=m_SinStatEdep;
	if(dlg.DoModal()!=IDOK)
		return;
	m_sRxoWell=dlg.m_sRxoWell;
	m_sRxoName=dlg.m_sRxoName;
	m_SinStatWin=dlg.m_SinStatWin;
	m_SinStatRlev=dlg.m_SinStatRlev;
	m_ParC=dlg.m_ParC;
	m_ParB=dlg.m_ParB;
	m_Rm=dlg.m_Rm;
	m_SinStatSdep=dlg.m_SinStatSdep;
	m_SinStatEdep=dlg.m_SinStatEdep;
	CString Path=m_sRxoWell;
	int x=Path.Find(".");
	if(x>0) 
		Path=Path.Left(x+1);
	else
		Path+=".";
	CFIDIO* pFID= new CFIDIO(Path+m_sRxoName);
	if(!pFID->Open()) 
		AfxMessageBox(Path+m_sRxoName+"\n打开曲线失败!      ");
	delete pFID;
	m_iPickupNum++;
}

//删除全井段的孔洞标注
void CImageManView::OnDelAllhole()
{
	if(AfxMessageBox("是否确认删除所有缝洞标注?        ",MB_YESNO|MB_ICONQUESTION)==IDNO)
		return;
	POSITION temp;
	POSITION pos=m_ContourList.GetHeadPosition();	 
	while(pos!=NULL)
	{
		temp=pos;
		m_ContourList.GetNext(pos);
		m_ContourList.RemoveAt(temp);
	}
	OnDrawSelect();
}

void CImageManView::OnUpdateDelAllhole(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_ContourList.IsEmpty());
}
//删除非储层段的孔洞标注
void CImageManView::OnPersistResult()
{
	if(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CResultObj)))
	{
		CResultObj *pResult=(CResultObj*)m_pSelectObj;
		if(pResult==NULL) return;
		int Num=pResult->m_ResultPoint.GetCount();
		if(Num<1) 
		{
			AfxMessageBox("综合解释表中无储层段信息        ");
			return;
		}


		CPoint pt,OffPt;
		OffPt.x=m_pPickupObj->m_Rect.left-m_PickupRect.left;
		OffPt.y=pDoc->DepthToYCoor(m_PickupStDep)-pDoc->DepthToYCoor(pDoc->m_PlotSdep);
		OffPt.y+=m_pPickupObj->m_Rect.top-m_PickupRect.top;
		
		//纵横向不同绘图比例校正系数
		double Her,Ver;
		Her=m_pPickupObj->m_Rect.Width()*1.0/m_PickupRect.Width();
		Ver=m_PickupVerScale/pDoc->m_DepthScale;

		CArray<CPoint,CPoint&>ptArray;

		int i,k,m,ptNum;
		CRect SlideRect,rect;

		//定义进度指示器
		CGuage* pGuage = new CGuage(0.f,Num*1.f);
		pGuage->m_szGuage="储层段";
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->CenterWindow();
		pGuage->ShowWindow(SW_SHOW);
		pGuage->SetRange(0,Num);
		pGuage->SetTitle("删除非储层段标注...");

		Num=0;
		CContour* pContour=NULL;
		CResultPoint* pObj=NULL;

		CObList tmpList;
		POSITION temp=NULL;
		POSITION pos = pResult->m_ResultPoint.GetHeadPosition();
		while(pos!=NULL)
		{
			pObj = (CResultPoint*) pResult->m_ResultPoint.GetNext(pos);
			SlideRect=m_pPickupObj->m_Rect;
			SlideRect.top=m_pPickupObj->DepthToYCoor(pObj->m_Sdep);
			SlideRect.bottom=m_pPickupObj->DepthToYCoor(pObj->m_Edep);

			temp=m_ContourList.GetHeadPosition();
			while(temp!=NULL)
			{	
				pContour=(CContour*)m_ContourList.GetNext(temp);
				k=pContour->m_pointArray.GetSize();
				//计算单个图像特征面积ptNum
				ptNum=0;
				if(k>3)
				{
					ptArray.SetSize(k,5);
					m=0;
					for(i=0;i<k;i++)
					{
						pt=pContour->m_pointArray[i];
						//进行纵横向不同绘图比例校正
						pt.x=m_PickupRect.left+(int)((pt.x-m_PickupRect.left)*Her);
						pt.y=m_PickupRect.top+(int)((pt.y-m_PickupRect.top)*Ver);
						//进行整体偏移
						pt.Offset(OffPt);
						if(pt.y>m_PickupRect.top
							&&pt.y<m_PickupRect.bottom)
						{
							pDoc->LPtoPP(&pt);
							ptArray[m]=pt;
							m++;
						}
					}
					if(m>2)
					{
						rect = pContour->m_rect;
						rect.right=rect.left+(int)(rect.Width()*Her);
						rect.bottom=rect.top+(int)(rect.Height()*Ver);
						rect.OffsetRect(OffPt);

						rect&=SlideRect;						
						if(rect.Height()>2)
							tmpList.AddTail(pContour);
					}
					ptArray.SetSize(0);
				}
			}
			Num++;
			pGuage->SetPos(Num);
			if(pGuage->CheckCancelButton())
				break;
		}
		delete pGuage;

		pos=m_ContourList.GetHeadPosition();	 
		while(pos!=NULL)
		{
			temp=pos;
			m_ContourList.GetNext(pos);
			m_ContourList.RemoveAt(temp);
		}
		pos=tmpList.GetHeadPosition();	 
		while(pos!=NULL)
		{
			pContour=(CContour*)tmpList.GetNext(pos);
			m_ContourList.AddTail(pContour);
		}
		if(m_ContourList.GetCount()>0)
			m_iPickupNum++;
		Invalidate();
	}
	else
		AfxMessageBox("请先选中用于井段参考的综合解释符号标注对象        ");
}

void CImageManView::OnUpdatePersistresult(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pSelectObj!=NULL&&!m_ContourList.IsEmpty());
}
//全井壁图像恢复
//基于纹理合成的全井壁图像修复技术
//生成全井壁图像曲线名为选中图像曲线名增加PIC
void CImageManView::OnCreateFullImage()
{
	if(m_pSelectObj==NULL)
		return;
	if(!m_pSelectObj->IsKindOf(RUNTIME_CLASS(CImageObj)))
		return;
	CImageObj *pObj=(CImageObj *)m_pSelectObj;
	CString szStr=pObj->m_strPathName;
	CString szWellName=szStr.Left(szStr.ReverseFind('.'));
	CString InName=szStr.Mid(szStr.ReverseFind('.')+1);
	CString OutName=InName+"PIC";

	float StProDep,EnProDep,Rlev;
	float sdep;
	int type=4;
	int DataNum=0;
	CFIDIO* pFID=new CFIDIO(szStr);
	if(pFID->Open())
	{
		StProDep=pFID->GetStartDepth();
		EnProDep=pFID->GetEndDepth();
		Rlev=pFID->GetDepthLevel();

		FIDCONTENT *Content = pFID->GetContent(3);
		if(Content != NULL)
		{
			DataNum=Content->nps;
			type=Content->length;
		}
		sdep=StProDep;
	}
	delete pFID;pFID=NULL;
	if(StProDep<pDoc->m_PlotSdep) 
		StProDep=pDoc->m_PlotSdep;
	if(EnProDep>pDoc->m_PlotEdep)
		EnProDep=pDoc->m_PlotEdep;

	if(DataNum<1) 
	{
		AfxMessageBox(szStr+"\n打开曲线失败!        ");
		return;
	}
	
	//定义进度指示器
	CGuage* pGuage = new CGuage(StProDep,EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetTitle("全井壁图像恢复"+OutName+"...");
	pGuage->ShowWindow(SW_SHOW);
     
	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"PAD","none",REPR_FLOAT,4,16,0,0.f,0.f,0.f},
							   {"DATAST","ohmm",REPR_FLOAT,4,16,16,0.f,0.f,0.f}};
	char adi[] = "DATID 0";	
	m_Content[0].min = StProDep;
	m_Content[0].max = EnProDep;
	m_Content[0].rlev = Rlev;
	m_Content[1].nps=DataNum;                      
	m_Content[1].min=2.54f;                                        
	m_Content[1].max=pObj->m_Bit*3.1416f;                   
	m_Content[1].rlev=pObj->m_Bit;                        

	m_Content[2].nps=DataNum;
	m_Content[2].npw=DataNum; 
	m_Content[2].min=0.f;      
	m_Content[2].max=256.f; 
	m_Content[2].rlev = 0.f;                           
	strcpy(m_Content[2].name,OutName);

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;
	//初始化全井壁恢复图像
	pFID = new CFIDIO(szWellName+"."+OutName,&m_Index);
	if(!pFID->Open())
	{	
		AfxMessageBox(OutName+" 初始化全井壁恢复图像失败!        ");
		delete pFID;
		delete pGuage;
		return;
	}

	char s[256],fn[256];
	strcpy(s,szWellName);
	strcat(s,".");strcat(s,InName);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(!fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		AfxMessageBox(InName+" 打开曲线失败!        ");
		delete pFID;delete pGuage;
		return;
	}

	int RecordNum=(int)((EnProDep-StProDep)/Rlev);
	float *Value=new float[DataNum*RecordNum];			//图像原始数据
	//读取曲线所有数据
	int i,j,k=0;
	if(sdep<StProDep)
		k=((int)((StProDep-sdep)/Rlev))*DataNum*4;
	fp.Seek (k,CFile::begin);
	if(type<4)
	{
		short *pTmp=new short[DataNum*RecordNum];
		fp.Read (pTmp,DataNum*RecordNum*2);
		for(i=0;i<RecordNum*DataNum;i++)
			Value[i]=pTmp[i];
		delete []pTmp;pTmp=NULL;
	}
	else fp.Read (Value,DataNum*RecordNum*4);
	fp.Close();	

	//反间距三维插值
	//XY方向为井周平面XY值
	//Z方向取ndy*2个深度采样，深度点位上取ndy个采样，下取ndy个采样
	int ndy=80;
	int nHeight=90;
	CImageRepair Inpaint(DataNum,nHeight);
	float dep=StProDep+ndy*Rlev;
	bool bBreak=false;
	for(k=0;k<RecordNum-nHeight;k+=ndy)
	{
		for(j=0;j<nHeight;j++)
			for(i=0;i<DataNum;i++)
				Inpaint.m_pData[j*DataNum+i]=Value[DataNum*(k+j)+i];
		Inpaint.Process();
		dep=StProDep+k*Rlev;
		pFID->SeekToDepth(dep);
		pFID->Write(3,DataNum*ndy,Inpaint.m_pData);
		pGuage->SetDepth(dep);
		if(pGuage->CheckCancelButton())
		{
			bBreak=true;
			break;
		}
	}
	if(!bBreak)
	{
		//完整保最后一次的数据
		k-=ndy;
		dep=StProDep+k*Rlev;
		pFID->SeekToDepth(dep);
		pFID->Write(3,DataNum*nHeight,Inpaint.m_pData);
		//处理最后不够一个窗口高度的数据
		k=RecordNum-nHeight-1;
		for(j=0;j<nHeight;j++)
			for(i=0;i<DataNum;i++)
				Inpaint.m_pData[j*DataNum+i]=Value[DataNum*(k+j)+i];
		Inpaint.Process();
		dep=StProDep+k*Rlev;
		pFID->SeekToDepth(dep);
		pFID->Write(3,DataNum*nHeight,Inpaint.m_pData);
	}
	Inpaint.CleanBuf();
	//保存全井壁图像恢复数据
	
	delete pFID;pFID=NULL;
	delete []Value;Value=NULL;
	delete pGuage;

	pDoc->WellRefresh();
	pObj->m_strPathName=szWellName+"."+OutName;
	pObj->Invalidate();
}

void CImageManView::OnUpdateCreateFullImage(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pSelectObj!=NULL&&m_pSelectObj->IsKindOf(RUNTIME_CLASS(CImageObj)));	
}
//井眼图像三维重建
//生成2个方位的三维图像垂直切片
void CImageManView::OnCreateImage3d()
{
	if(m_pSelectObj==NULL)
		return;
	if(!m_pSelectObj->IsKindOf(RUNTIME_CLASS(CImageObj)))
		return;
	CImageObj *pObj=(CImageObj *)m_pSelectObj;
	CString szStr=pObj->m_strPathName;
	CString szWellName=szStr.Left(szStr.ReverseFind('.'));
	CString InName=szStr.Mid(szStr.ReverseFind('.')+1);
	CString OutName=InName+"3D";
	CString SectionName=InName;

	float StProDep,EnProDep,Rlev;

	int type=4;
	int DataNum=0;
	CFIDIO* pFID=new CFIDIO(szStr);
	if(pFID->Open())
	{
		StProDep=pFID->GetStartDepth();
		EnProDep=pFID->GetEndDepth();
		Rlev=pFID->GetDepthLevel();

		FIDCONTENT *Content = pFID->GetContent(3);
		if(Content != NULL)
		{
			DataNum=Content->nps;
			type=Content->length;
		}
	}
	delete pFID;pFID=NULL;

	if(DataNum<1) 
	{
		AfxMessageBox(szStr+"\n打开曲线失败!        ");
		return;
	}
	if(StProDep<pDoc->m_PlotSdep) 
		StProDep=pDoc->m_PlotSdep;
	if(EnProDep>pDoc->m_PlotEdep)
		EnProDep=pDoc->m_PlotEdep;
	//定义进度指示器
	CGuage* pGuage = new CGuage(StProDep,EnProDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->SetTitle(InName+" 井眼图像三维重建...");
	pGuage->ShowWindow(SW_SHOW);
     
	FIDCONTENT m_Content[3] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
    		                   {"PAD","none",REPR_SHORT,4,16,0,0.f,0.f,0.f},
							   {"DATAST","ohmm",REPR_FLOAT,4,16,16,0.f,0.f,0.f}};
	char adi[] = "DATID 0";	
	m_Content[0].min = StProDep;
	m_Content[0].max = EnProDep;
	m_Content[0].rlev = Rlev;
	m_Content[1].nps=DataNum;                      
	m_Content[1].min=2.54f;                            
	m_Content[1].max=pObj->m_Bit*3.1416f;                   
	m_Content[1].rlev = pObj->m_Bit;                        

	m_Content[2].nps=DataNum;
	m_Content[2].npw=DataNum;                        
	m_Content[2].min=0.f;                  
	m_Content[2].max=256.f;               
	m_Content[2].rlev =0.f;                            

	FIDINDEX m_Index;	
	m_Index.Contentlist = m_Content;
	m_Index.dimension = 3;
	m_Index.adi = adi;
	//初始化4条垂直切片图像
	double Radius=DataNum/2/PI;
	int Num=(int)(Radius*2);
	
	m_Content[1].nps=Num; 
	m_Content[2].nps=Num;
	m_Content[2].npw=Num; 
	int i,j,k=0;

	for(i=0;i<4;i++)
	{
		strcpy(m_Content[2].name,SectionName);
		szStr.Format("%d",i*45);
		strcat(m_Content[2].name,szStr);
		szStr=szWellName+"."+SectionName+szStr;
		pFID = new CFIDIO(szStr,&m_Index);
		if(!pFID->Open())
		{	
			AfxMessageBox(szStr+" 初始化垂直切片图像失败!        ");
			delete pFID;
			delete pGuage;
			return;
		}
		pFID->Close();
		delete pFID;pFID=NULL;
	}
	char s[256],fn[256];
	strcpy(s,szWellName);
	strcat(s,".");strcat(s,InName);
	CFIDIO::GetFileName(s,fn,'D');
	CFile fp;
	if(!fp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		AfxMessageBox(InName+" 打开曲线失败!        ");
		delete pGuage;
		return;
	}

	int RecordNum=(int)((EnProDep-StProDep)/Rlev);
	float *Value=new float[DataNum*RecordNum];			//图像原始数据
	//读取曲线所有数据
	fp.Seek (0L,CFile::begin);
	if(type<4)
	{
		short *pTmp=new short[DataNum*RecordNum];
		fp.Read (pTmp,DataNum*RecordNum*2);
		for(i=0;i<RecordNum*DataNum;i++)
			Value[i]=pTmp[i];
		delete []pTmp;pTmp=NULL;
	}
	else fp.Read (Value,DataNum*RecordNum*4);
	fp.Close ();	

	CFile *pFile=new CFile[4];
	for(i=0;i<4;i++)
	{
		szStr.Format("%d",i*45);
		szStr=szWellName+"."+SectionName+szStr;
		strcpy(s,szStr);
		CFIDIO::GetFileName(s,fn,'D');
		if(!pFile[i].Open(fn,CFile::modeReadWrite|CFile::typeBinary))
		{
			delete pFile;
			delete Value;
			delete pGuage;
			return;
		}
	}
	//反间距三维插值
	//XY方向为井周平面XY值
	//Z方向取ndy*2个深度采样，深度点位上取ndy个采样，下取ndy个采样
	int ndy=5;
	double x,y,z;
	vector<Point3D> input;
	Insert3D* pInterpolater = new Insert3D(DataNum,6);
	float **pData=new float* [Num];//垂直切片数据
	for(i=0;i<4;i++)
		pData[i]=new float[Num];
	
	for(j=ndy;j<RecordNum-ndy;j++)
	{
		input.clear();
		for(k=0;k<ndy*2;k++)
		{
			for(i=0;i<DataNum;i++)
			{
				if(Value[DataNum*(j+k-ndy)+i]!=-9999.f)
				{
					x=Radius*sin((i*2*PI)/DataNum);
					y=Radius*cos((i*2*PI)/DataNum);
					z=k-ndy;
					input.push_back(Point3D(x,y,z,Value[DataNum*(j+k-ndy)+i]));
				}
			}
		}
		z=0;
		for(k=0;k<4;k++)
		{
			for(i=0;i<Num;i++)
			{
				x=(Num/2-i)*sin(k*PI/4);
				y=(Num/2-i)*cos(k*PI/4);
				pData[k][i]=pInterpolater->GetInsertV(x,y,z,(Point3D*)&(*input.begin()),(Point3D*)&(*input.end()));
			}
			pFile[k].Write(pData[k],Num*4);
		}
		pGuage->SetDepth(StProDep+j*Rlev);
		if(pGuage->CheckCancelButton())
			break;
	}
	
	delete pInterpolater;
	for(i=0;i<4;i++)
		delete pData[i];
	delete pData;

	for(i=0;i<4;i++)
		pFile[i].Close ();
	delete pFile;delete Value;
	delete pGuage;
	pDoc->WellRefresh();
}

void CImageManView::OnUpdateCreateImage3d(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pSelectObj!=NULL&&m_pSelectObj->IsKindOf(RUNTIME_CLASS(CImageObj)));
}
//区间孔隙度谱分析
//生成孔隙度谱和原生次生孔隙度曲线
void CImageManView::OnPorCompute()
{
	if(m_pSelectObj==NULL)
		return;
	if(!m_pSelectObj->IsKindOf(RUNTIME_CLASS(CImageObj)))
		return;
	CImageObj *pObj=(CImageObj *)m_pSelectObj;
	CString szStr=pObj->m_strPathName;
	CString szWellName=szStr.Left(szStr.ReverseFind('\\'));
	szWellName=szWellName+"\\"+szWellName.Mid(szStr.ReverseFind('#')+1)+".FID";
	
	CString InName=szStr.Mid(szStr.ReverseFind('.')+1);
	float StProDep,EnProDep,Rlev=-999.25f;
	int DataNum=-1;
	int type=4;
			
	CFIDIO* pFID=new CFIDIO(szStr);
	if(pFID->Open())
	{
		StProDep=pFID->GetStartDepth();
		EnProDep=pFID->GetEndDepth();
		Rlev=pFID->GetDepthLevel();
		FIDCONTENT *Content = pFID->GetContent(3);
		if(Content != NULL)
		{
			DataNum=Content->nps;
			type=Content->length;
		}
	}
	delete pFID;pFID=NULL;
	if(DataNum<1||Rlev<0.0000025f) 
	{
		AfxMessageBox(szStr+"\n打开曲线失败!        ");
		return;
	}

	if(StProDep<pDoc->m_PlotSdep) 
		StProDep=pDoc->m_PlotSdep;
	if(EnProDep>pDoc->m_PlotEdep)
		EnProDep=pDoc->m_PlotEdep;

	CCreateSpectDlg dlg;
	dlg.m_szExtPorWell=szWellName;
	dlg.m_szExtRxoWell=szWellName;

	dlg.m_szWell=szWellName;
	dlg.m_szName=InName;
	dlg.m_StProDep=StProDep;
	dlg.m_EnProDep=EnProDep;
	dlg.m_Rlev=Rlev;
	dlg.m_DataNum=DataNum;
	dlg.m_DataType=type;
	dlg.DoModal();
}

void CImageManView::OnUpdatePorCompute(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pSelectObj!=NULL&&m_pSelectObj->IsKindOf(RUNTIME_CLASS(CImageObj)));
}
//用浅侧向对选中图像进行电阻率刻度
void CImageManView::OnRmfScale()
{
	if(m_pSelectObj==NULL)
		return;
	if(!m_pSelectObj->IsKindOf(RUNTIME_CLASS(CImageObj)))
		return;
	CImageObj *pObj=(CImageObj *)m_pSelectObj;
	CString szStr=pObj->m_strPathName;
	CString szWellName=szStr.Left(szStr.ReverseFind('.')+1)+"FID";
	CString InName=szStr.Mid(szStr.ReverseFind('.')+1);
	float StProDep,EnProDep,Rlev=-999.25f;
	int DataNum=-1;
	int type=4;
			
	CFIDIO* pFID=new CFIDIO(szStr);
	if(pFID->Open())
	{
		StProDep=pFID->GetStartDepth();
		EnProDep=pFID->GetEndDepth();
		Rlev=pFID->GetDepthLevel();
		FIDCONTENT *Content = pFID->GetContent(3);
		if(Content != NULL)
		{
			DataNum=Content->nps;
			type=Content->length;
		}
	}
	delete pFID;pFID=NULL;
	if(DataNum<1||Rlev<0.0000025f) 
	{
		AfxMessageBox(szStr+"\n打开曲线失败!        ");
		return;
	}

	if(StProDep<pDoc->m_PlotSdep) 
		StProDep=pDoc->m_PlotSdep;
	if(EnProDep>pDoc->m_PlotEdep)
		EnProDep=pDoc->m_PlotEdep;

	CRxoScaleDlg dlg;
	dlg.m_szWell=szWellName;
	dlg.m_szName=InName;
	dlg.m_szRMFWell=szWellName;
	dlg.m_szRMFName="RXO";
	dlg.m_StProDep=StProDep;
	dlg.m_EnProDep=EnProDep;
	dlg.m_Rlev=Rlev;
	dlg.m_DataNum=DataNum;
	dlg.m_DataType=type;
	dlg.m_LeftScal=400.f;
	dlg.m_RightScal=7000.f;
	dlg.m_bResistance = TRUE;
	dlg.m_fRlev=0.0254;
	dlg.m_fWin=2.54f; 
	if(dlg.DoModal()==IDOK)
	{
		pObj->m_strPathName=szStr.Left(szStr.ReverseFind('.')+1)+InName+"RXO";
		pObj->m_bColor=FALSE;
		pObj->CreateImagePalette();
		pObj->Invalidate();
	}
}

void CImageManView::OnUpdateRmfScale(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pSelectObj!=NULL&&m_pSelectObj->IsKindOf(RUNTIME_CLASS(CImageObj)));
}

void CImageManView::OnDrawFrequency()
{
	float dep1=pDoc->m_PlotSdep;
	float dep2=pDoc->m_PlotEdep;
	if(m_pSelectObj!=NULL)
	{
		if(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CDepSect)))
		{
			int DaltaY=pDoc->GetLeftTrackObj ()->GetClientRect ().top-pDoc->m_TopSpace;
			CPoint pt1=((CDepSect*)m_pSelectObj)->m_Point1 ;
			CPoint pt2=((CDepSect*)m_pSelectObj)->m_Point2 ;
			dep1=pDoc->YCoorToDepth (pt1.y-DaltaY);
			dep2=pDoc->YCoorToDepth (pt2.y-DaltaY);
		}
	}
	DrawFrequency(dep1,dep2);
}


void CImageManView::DrawFrequency(float sdep,float edep)
{
	if(!m_ContourList.IsEmpty())
	{
		if(m_pPickupObj!=NULL)
		{
			int nps=0;
			CFIDIO* inFID=new CFIDIO(m_pPickupObj->m_strPathName);
			if(inFID->Open())
			{
				FIDCONTENT *Content = inFID->GetContent(3);
				if(Content != NULL)
					nps=Content->npw;
			}
			if(nps<1) 
			{
				delete inFID;
				AfxMessageBox(m_pPickupObj->m_strPathName+"\n曲线打开失败!        ");
				return;
			}

			CPoint OffPt;
			OffPt.x=m_pPickupObj->m_Rect.left-m_PickupRect.left;
			OffPt.y=pDoc->DepthToYCoor(m_PickupStDep)-pDoc->DepthToYCoor(pDoc->m_PlotSdep);
			OffPt.y+=m_pPickupObj->m_Rect.top-m_PickupRect.top;
			//纵横向不同绘图比例校正系数
			double Her,Ver;
			Her=m_pPickupObj->m_Rect.Width()*1.0/m_PickupRect.Width();
			Ver=m_PickupVerScale/pDoc->m_DepthScale;
			
			//定义进度指示器
			int objNum=m_ContourList.GetCount();
			CGuage* pGuage = new CGuage(0.f,objNum*1.f);
			pGuage->m_szGuage="目标数";
			pGuage->Create(CWnd::GetActiveWindow());
			pGuage->CenterWindow();
			pGuage->ShowWindow(SW_SHOW);
			pGuage->SetRange(0,objNum);
			pGuage->SetTitle("统计孔洞内电导率值...");

			CRgn rgn;
			int x,y,k,m,n;
			CPoint pt;float dep;
			//定义直方图显示对话框
			CFrequencyDlg dlg;
			dlg.m_MinValue=0.f;
			dlg.m_MaxValue=1500.f;
			dlg.m_ImageMin=m_ImageMin;
			dlg.m_ImageMax=m_ImageMax;
			dlg.m_Point=0;
			for(m=0;m<dlg.m_NumScale;m++)
				dlg.m_freq[m]=0;
			
			CRect SlideRect=m_pPickupObj->m_Rect;
			SlideRect.top=m_pPickupObj->DepthToYCoor(sdep);
			SlideRect.bottom=m_pPickupObj->DepthToYCoor(edep);

			float *pData=new float[nps];
			CContour* pContour=NULL;
			POSITION pos=m_ContourList.GetHeadPosition();
			objNum=0;
			CRect rect;
			BOOL bRead,bOK=TRUE;		
			while(pos!=NULL)
			{	
				pContour=(CContour*)m_ContourList.GetNext(pos);
				k=pContour->m_pointArray.GetSize();
				if(k>4)
				{
					if(rgn.CreatePolygonRgn(&pContour->m_pointArray[0],k,ALTERNATE))
					{
						rgn.GetRgnBox(&rect);
						for(y=rect.top; y<=rect.bottom; y++)
						{
							bRead=TRUE;
							for(x=rect.left; x<=rect.right; x++)
							{
								pt.x=x;pt.y=y;
								if(rgn.PtInRegion(pt))
								{
									//进行纵横向不同绘图比例校正
									pt.x=m_PickupRect.left+(int)((pt.x-m_PickupRect.left)*Her);
									pt.y=m_PickupRect.top+(int)((pt.y-m_PickupRect.top)*Ver);
									//进行整体偏移
									pt.Offset(OffPt);
									if(pt.y>SlideRect.top&&pt.y<SlideRect.bottom)
									{
										if(bRead)
										{
											dep=(float)m_pPickupObj->YCoorToDepth(pt.y);
											inFID->SeekToDepth(dep);
											inFID->Read(3,nps,pData);
											bRead=FALSE;
										}
										m=(pt.x-SlideRect.left)*nps/SlideRect.Width();
										if(m>-1&&m<nps)
										{
											if(pData[m]>0.f)
											{
												n=(int)((pData[m]-m_ImageMin)/(m_ImageMax-m_ImageMin)*dlg.m_NumScale);
												n=(int)(pData[m]/1500.f*dlg.m_NumScale);
												if((n>-1)&&(n<dlg.m_NumScale))
												{
													dlg.m_freq[n]++;
													dlg.m_Point++;
												}
											}
										}
									}
								}
							}
						}
						rgn.DeleteObject();
					}
				}
				objNum++;
				pGuage->SetPos(objNum);
				if(pGuage->CheckCancelButton())
				{
					bOK=FALSE;
					break;
				}
			}
			delete pData;delete inFID;
			delete pGuage;
			if(bOK)
			{
				if(dlg.DoModal()==IDOK)
				{
					if(m_ImageMin!=dlg.m_ImageMin||m_ImageMax!=dlg.m_ImageMax)
						m_iPickupNum++;
					m_ImageMin=dlg.m_ImageMin;
					m_ImageMax=dlg.m_ImageMax;
					Invalidate();
				}
			}
		}
	}
}
void CImageManView::OnUpdateDrawFrequency(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_ContourList.IsEmpty()&&m_pPickupObj!=NULL);
}
//孔洞图像值--电导率交会图分析
void CImageManView::OnDrawCross()
{
	float dep1=pDoc->m_PlotSdep;
	float dep2=pDoc->m_PlotEdep;
	if(m_pSelectObj!=NULL)
	{
		if(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CDepSect)))
		{
			int DaltaY=pDoc->GetLeftTrackObj ()->GetClientRect ().top-pDoc->m_TopSpace;
			CPoint pt1=((CDepSect*)m_pSelectObj)->m_Point1 ;
			CPoint pt2=((CDepSect*)m_pSelectObj)->m_Point2 ;
			dep1=pDoc->YCoorToDepth (pt1.y-DaltaY);
			dep2=pDoc->YCoorToDepth (pt2.y-DaltaY);
		}
	}
	DrawCross(dep1,dep2);
}

void CImageManView::OnUpdateDrawCross(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_ContourList.IsEmpty()&&m_pPickupObj!=NULL);
}
//
//X轴为浅侧向RXO的倒数--电导率
//
void CImageManView::DrawCross(float sdep,float edep)
{
	if(!m_ContourList.IsEmpty())
	{
		if(m_pPickupObj!=NULL)
		{
			int nps=0;
			CFIDIO* inFID=new CFIDIO(m_pPickupObj->m_strPathName);
			if(inFID->Open())
			{
				FIDCONTENT *Content = inFID->GetContent(3);
				if(Content != NULL)
					nps=Content->npw;
			}
			if(nps<1) 
			{
				delete inFID;
				AfxMessageBox(m_pPickupObj->m_strPathName+"\n曲线打开失败!        ");
				return;
			}
			CString Path=m_sRxoWell;
			int i=Path.ReverseFind('.');
			if(i>0) 
				Path=Path.Left(i+1);
			else
				Path+=".";
			CFIDIO* pFID= new CFIDIO(Path+m_sRxoName);
			if(!pFID->Open()) 
			{
				delete pFID;delete inFID;
				AfxMessageBox(Path+m_sRxoName+"\n打开曲线失败!      ");
				return;
			}

			CPoint OffPt;
			OffPt.x=m_pPickupObj->m_Rect.left-m_PickupRect.left;
			OffPt.y=pDoc->DepthToYCoor(m_PickupStDep)-pDoc->DepthToYCoor(pDoc->m_PlotSdep);
			OffPt.y+=m_pPickupObj->m_Rect.top-m_PickupRect.top;
			//纵横向不同绘图比例校正系数
			double Her,Ver;
			Her=m_pPickupObj->m_Rect.Width()*1.0/m_PickupRect.Width();
			Ver=m_PickupVerScale/pDoc->m_DepthScale;
			
			//定义进度指示器
			int objNum=m_ContourList.GetCount();
			CGuage* pGuage = new CGuage(0.f,objNum*1.f);
			pGuage->m_szGuage="目标数";
			pGuage->Create(CWnd::GetActiveWindow());
			pGuage->CenterWindow();
			pGuage->ShowWindow(SW_SHOW);
			pGuage->SetRange(0,objNum);
			pGuage->SetTitle("统计孔洞内电导率值...");

			CRgn rgn;
			int x,y,k,m,n;
			CPoint pt;float dep;

			//定义交会图显示对话框
			CCrossDlg dlg;

			memset(&dlg.m_CrossData[0], 0, sizeof(dlg.m_CrossData));
			memset(&dlg.m_CrossX[0], 0, sizeof(dlg.m_CrossX));
			memset(&dlg.m_CrossY[0], 0, sizeof(dlg.m_CrossY));
			
			float	m_StartX= 0.00001f;		//横坐标初值	
			float	m_ScaleIncX=1.f;		//横坐标增量	
			WORD	m_ScaleNumX=4.f;		//横坐标刻度数	
			WORD	m_ScaleTypeX=1;			//横坐标刻度类型
		
			float	m_StartY=30.f;			//纵坐标初值	
			float	m_ScaleIncY=1.f;		//纵坐标增量	
			WORD	m_ScaleNumY=2.f;		//纵坐标刻度数	
			WORD	m_ScaleTypeY=1;			//纵坐标刻度类型

			dlg.m_StartX	=m_StartX;	
			dlg.m_ScaleIncX	=m_ScaleIncX;		
			dlg.m_ScaleNumX	=m_ScaleNumX;		
			dlg.m_ScaleTypeX=m_ScaleTypeX;	
						 
			dlg.m_StartY	=m_StartY;			
			dlg.m_ScaleIncY	=m_ScaleIncY;		
			dlg.m_ScaleNumY	=m_ScaleNumY;	
			dlg.m_ScaleTypeY=m_ScaleTypeY;	
			dlg.m_ImageMin=m_ImageMin;
			dlg.m_ImageMax=m_ImageMax;

			double dx=m_ScaleIncX*m_ScaleNumX/100.0;
			double dy=m_ScaleIncY*m_ScaleNumY/100.0;
			int nx,ny,npt;
			float imgVal,Rxo;
			float m_RHVal1=0.0004f;
			float m_RHVal2=0.002f;

			float ImageMin=m_ImageMin;
			float ImageMax=m_ImageMax;
			if(m_ScaleTypeY>0)
			{
				ImageMin=log10(ImageMin);
				ImageMax=log10(ImageMax);
			}

			CRect SlideRect=m_pPickupObj->m_Rect;
			SlideRect.top=m_pPickupObj->DepthToYCoor(sdep);
			SlideRect.bottom=m_pPickupObj->DepthToYCoor(edep);

			float *pData=new float[nps];
			CContour* pContour=NULL;
			POSITION pos=m_ContourList.GetHeadPosition();
			objNum=0;
			CRect rect;
			BOOL bRead,bOK=TRUE;		
			while(pos!=NULL)
			{	
				pContour=(CContour*)m_ContourList.GetNext(pos);
				k=pContour->m_pointArray.GetSize();
				//对每个孔洞按平均值作交会图
				/*if(k>4)*/
				//{
				//	imgVal=0.f;npt=0;
				//	if(rgn.CreatePolygonRgn(&pContour->m_pointArray[0],k,ALTERNATE))
				//	{
				//		rgn.GetRgnBox(&rect);
				//		for(y=rect.top; y<=rect.bottom; y++)
				//		{
				//			bRead=TRUE;
				//			for(x=rect.left; x<=rect.right; x++)
				//			{
				//				pt.x=x;pt.y=y;
				//				if(rgn.PtInRegion(pt))
				//				{
				//					//进行纵横向不同绘图比例校正
				//					pt.x=m_PickupRect.left+(int)((pt.x-m_PickupRect.left)*Her);
				//					pt.y=m_PickupRect.top+(int)((pt.y-m_PickupRect.top)*Ver);
				//					//进行整体偏移
				//					pt.Offset(OffPt);
				//					if(pt.y>SlideRect.top&&pt.y<SlideRect.bottom)
				//					{
				//						if(bRead)
				//						{
				//							dep=(float)m_pPickupObj->YCoorToDepth(pt.y);
				//							inFID->SeekToDepth(dep);
				//							inFID->Read(3,nps,pData);
				//							bRead=FALSE;
				//						}
				//						m=(pt.x-SlideRect.left)*nps/SlideRect.Width();
				//						if(m>-1&&m<nps)
				//						{
				//							if(pData[m]>0.f)
				//							{
				//								imgVal+=pData[m];
				//								nptY+=pt.y;
				//								npt++;
				//							}
				//						}
				//					}
				//				}
				//			}
				//		}
				//		if(npt>0)
				//		{
				//			nptY/=npt;
				//			dep=(float)m_pPickupObj->YCoorToDepth(nptY);
				//			pFID->SeekToDepth(dep);
				//			pFID->Read(2,1,&Rxo);
				//			if(Rxo<0.0001f) 
				//				Rxo=0.0001f;
				//			Rxo=1.f/Rxo;
				//			if(m_ScaleTypeX==1) 
				//				nx = (int)((log10(Rxo/m_StartX))/dx);					   
				//			else 
				//				nx = (int)((Rxo-m_StartX)/dx);
				//			if(nx>-1&&nx<100)
				//			{
				//				imgVal/=npt;
				//				if(imgVal<0.00001f) 
				//					imgVal=0.00001f;
				//				if(m_ScaleTypeY==1) 
				//					ny =(int)((log10(imgVal/m_StartY))/dy);
				//				else 
				//					ny =(int)((imgVal-m_StartY)/dy);
				//				if(ny>-1&&ny<100)
				//				{
				//					n=(int)(log10(Rxo/m_RHVal1)*3.0/log10(m_RHVal2/m_RHVal1)-0.5);
				//					if(n<0) n=0;
				//					if(n>2) n=2;
				//					dlg.m_CrossData[n][nx][ny]++;
				//				}
				//			}
				//		}
				//		rgn.DeleteObject();
				//	}
				//}
				//对每个点作交会图
				//if(k>4)
				{
					imgVal=0.f;npt=0;
					if(rgn.CreatePolygonRgn(&pContour->m_pointArray[0],k,ALTERNATE))
					{
						rgn.GetRgnBox(&rect);
						for(y=rect.top; y<=rect.bottom; y++)
						{
							bRead=TRUE;
							for(x=rect.left; x<=rect.right; x++)
							{
								pt.x=x;pt.y=y;
								if(rgn.PtInRegion(pt))
								{
									//进行纵横向不同绘图比例校正
									pt.x=m_PickupRect.left+(int)((pt.x-m_PickupRect.left)*Her);
									pt.y=m_PickupRect.top+(int)((pt.y-m_PickupRect.top)*Ver);
									//进行整体偏移
									pt.Offset(OffPt);
									if(pt.y>SlideRect.top&&pt.y<SlideRect.bottom)
									{
										if(bRead)
										{
											dep=(float)m_pPickupObj->YCoorToDepth(pt.y);
											inFID->SeekToDepth(dep);
											inFID->Read(3,nps,pData);
											bRead=FALSE;

											pFID->SeekToDepth(dep);
											pFID->Read(2,1,&Rxo);
											if(Rxo<0.00001f) 
												Rxo=0.00001f;
											Rxo=1.f/Rxo;
											if(m_ScaleTypeX==1) 
												nx = (int)((log10(Rxo/m_StartX))/dx);					   
											else 
												nx = (int)((Rxo-m_StartX)/dx);
										}
										m=(pt.x-SlideRect.left)*nps/SlideRect.Width();
										if(m>-1&&m<nps)
										{
											if(pData[m]>0.f)
											{
												imgVal+=pData[m];
											/*	nptY+=pt.y;*/
												npt++;
												//对每个点作交会图
												if(nx>-1&&nx<100)
												{
													imgVal=pData[m];
													if(m_ScaleTypeY==1) 
														ny =(int)((log10(imgVal/m_StartY))/dy);
													else 
														ny = (int)((imgVal-m_StartY)/dy);
													if(ny>-1&&ny<100)
													{
														n=(int)((log10(imgVal)-ImageMin)*3.f/(ImageMax-ImageMin));
														if(n<0) n=0;
														if(n>2) n=2;
														if(Rxo>m_RHVal1)
														{
															if(Rxo<m_RHVal2&&n>0)
																n=1;
															dlg.m_CrossData[n][nx][ny]++;
														}
														else
															dlg.m_CrossData[0][nx][ny]++;
													}
												}
											}
										}
									}
								}
							}
						}
						rgn.DeleteObject();
					}
				}
				objNum++;
				pGuage->SetPos(objNum);
				if(pGuage->CheckCancelButton())
				{
					bOK=FALSE;
					break;
				}
			}
			delete pData;delete inFID;delete pFID;
			delete pGuage;
			if(bOK)
			{
				//计算XY轴直方图
				for(x = 0; x<100; x++) 
					for(y = 0; y<100; y++) 
						for(n=0;n<3;n++)
							dlg.m_CrossX[x]+=dlg.m_CrossData[n][x][y];		
				for(y = 0; y<100; y++) 
					for (x = 0; x<100; x++) 
						for(n=0;n<3;n++)
							dlg.m_CrossY[y]+=dlg.m_CrossData[n][x][y];	
				dlg.DoModal();
			}
		}
	}
}
//快速生成单条面孔率曲线
void CImageManView::OnCreateHpor()
{
	if(m_ContourList.IsEmpty()||m_pPickupObj==NULL)
		return;

	CString OutName="HPOR";
	float Rlev=0.125f;
	float StartDep=(float)pDoc->m_PlotSdep;
	float EndDep=(float)pDoc->m_PlotEdep;
	float win12=m_StatWin/2.f;
	//一个统计窗口的井壁面积mm2
	float WinS=(float)PI*m_pPickupObj->m_Bit*(2.f*win12*1000.f);
	//一个统计窗口的井壁面积m2
	float WinSm=(float)PI*(m_pPickupObj->m_Bit/1000.f)*(2.f*win12);

	//自动拾取覆盖率m_CoverageRate
	WinS=WinS*m_CoverageRate;
	WinSm=WinSm*m_CoverageRate;

	//对滑动窗长和滑动步长重新计算
	FIDCONTENT ContentW[2] = {{"DEP","m",REPR_FLOAT,4,1,0,0.f,10000.f,0.f},
   						      {"HPOR","%",REPR_FLOAT,4,1,1,0.f,100.f,0.f}};
	ContentW[0].min = StartDep;
	ContentW[0].max = EndDep;
	ContentW[0].rlev= Rlev;

	FIDINDEX Index;
	Index.dimension = 2;
	Index.Contentlist = ContentW;
	char adi[] = "==END==";
	Index.adi = adi;

	//初始化颗粒孔洞频谱曲线
	CFIDIO* pFID = new CFIDIO(m_StatWellName+"."+OutName,&Index);
	if(!pFID->Open())
	{
		delete pFID;
		AfxMessageBox(m_StatWellName+"\n初始化面孔洞率失败");
		return;
	}
	pFID->Close();delete pFID;

	CPoint pt,OffPt;
	OffPt.x=m_pPickupObj->m_Rect.left-m_PickupRect.left;
	OffPt.y=pDoc->DepthToYCoor(m_PickupStDep)-pDoc->DepthToYCoor(pDoc->m_PlotSdep);
	OffPt.y+=m_pPickupObj->m_Rect.top-m_PickupRect.top;
	
	//纵横向不同绘图比例校正系数
	double Her,Ver;
	Her=m_pPickupObj->m_Rect.Width()*1.0/m_PickupRect.Width();
	Ver=m_PickupVerScale/pDoc->m_DepthScale;

	CArray<CPoint,CPoint&>ptAr;
	CRgn rgn;
	int i,k,n,Num,x,y,ptNum;

	POSITION pos=NULL;
	CContour* pContour=NULL;
	
	//定义进度指示器
	CGuage* pGuage = new CGuage(StartDep,EndDep);
	pGuage->Create(CWnd::GetActiveWindow());
	pGuage->CenterWindow();
	pGuage->ShowWindow(SW_SHOW);
	pGuage->SetTitle(OutName+" 快速生成面孔洞率曲线...");
	BOOL bBreak=FALSE;

	CFile Outfp;
	char s[256],fn[256];
	strcpy(s,m_StatWellName);
	strcat(s,".");strcat(s,OutName);
	CFIDIO::GetFileName(s,fn,'D');
	if(Outfp.Open (fn,CFile::modeReadWrite|CFile::typeBinary))
	{
		float s,area;
		Num=0;
		CRect SlideRect,rect;
		float dep=StartDep+win12;
		Outfp.Seek(((int)(win12/Rlev))*4,CFile::begin);
		while(dep<EndDep-win12)
		{
			SlideRect=m_pPickupObj->m_Rect;
			SlideRect.top=m_pPickupObj->DepthToYCoor(dep-win12);
			SlideRect.bottom=m_pPickupObj->DepthToYCoor(dep+win12);
			pos=m_ContourList.GetHeadPosition();
			while(pos!=NULL)
			{	
				ptNum=0;
				pContour=(CContour*)m_ContourList.GetNext(pos);
				k=pContour->m_pointArray.GetSize();
				//计算单个图像特征面积ptNum
				n=0;
				ptAr.SetSize(k,5);
				//计算滑动窗口内的真实面积
				for(i=0;i<k;i++)
				{
					pt=pContour->m_pointArray[i];
					//进行纵横向不同绘图比例校正
					pt.x=m_PickupRect.left+(int)((pt.x-m_PickupRect.left)*Her);
					pt.y=m_PickupRect.top+(int)((pt.y-m_PickupRect.top)*Ver);
					//进行整体偏移
					pt.Offset(OffPt);
					if(SlideRect.PtInRect(pt))
					{
						ptAr[n]=pt; 
						n++;
					}
				}
				if(n>3)
				{
					//计算区域内部的面积ptNum
					if(rgn.CreatePolygonRgn(&ptAr[0],n,ALTERNATE))
					{
						rgn.GetRgnBox(&rect);
						//计算面积
						for(y=rect.top; y<=rect.bottom; y++)
							for(x=rect.left; x<=rect.right; x++)
							{
								if(rgn.PtInRegion(CPoint(x,y)))
									ptNum++;
							}
						rgn.DeleteObject();
					}
					area+=ptNum;
				}
				ptAr.SetSize(0);
				/*s=ptNum*m_PickupVerScale*0.254f;
				s*=m_PickupHorScale*0.254f;
				area+=s;*/
			}
			area*=m_PickupVerScale*0.254f;
			area*=m_PickupHorScale*0.254f;
			//标准井线性刻度
			area=area*m_fA+m_fB;
			area=area/WinS*100.f;						//面孔率			HPOR   单位%
			Outfp.Write(&area,4L); 
			
			Num++;
			dep=StartDep+win12+Rlev*Num;
			pGuage->SetDepth(dep);
			if(pGuage->CheckCancelButton())
			{
				bBreak=TRUE;
				break;
			}
		}
		Outfp.Close();
	}
	else bBreak=TRUE;
	delete pGuage;
	if(!bBreak) 
		pDoc->WellRefresh();
}

void CImageManView::OnUpdateCreateHpor(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pPickupObj!=NULL&&!m_ContourList.IsEmpty());
}
//孔洞直径直方图分析
void CImageManView::OnDrawDiameter()
{
	if(!m_ContourList.IsEmpty())
	{
		float sdep=pDoc->m_PlotSdep;
		float edep=pDoc->m_PlotEdep;
		if(m_pSelectObj!=NULL)
		{
			if(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CDepSect)))
			{
				int DaltaY=pDoc->GetLeftTrackObj ()->GetClientRect ().top-pDoc->m_TopSpace;
				CPoint pt1=((CDepSect*)m_pSelectObj)->m_Point1 ;
				CPoint pt2=((CDepSect*)m_pSelectObj)->m_Point2 ;
				sdep=pDoc->YCoorToDepth (pt1.y-DaltaY);
				edep=pDoc->YCoorToDepth (pt2.y-DaltaY);
			}
		}
	
		if(m_pPickupObj!=NULL)
		{
			CPoint OffPt;
			OffPt.x=m_pPickupObj->m_Rect.left-m_PickupRect.left;
			OffPt.y=pDoc->DepthToYCoor(m_PickupStDep)-pDoc->DepthToYCoor(pDoc->m_PlotSdep);
			OffPt.y+=m_pPickupObj->m_Rect.top-m_PickupRect.top;
			//纵横向不同绘图比例校正系数
			double Her,Ver;
			Her=m_pPickupObj->m_Rect.Width()*1.0/m_PickupRect.Width();
			Ver=m_PickupVerScale/pDoc->m_DepthScale;
			
			//定义进度指示器
			int objNum=m_ContourList.GetCount();
			CGuage* pGuage = new CGuage(0.f,objNum*1.f);
			pGuage->m_szGuage="目标数";
			pGuage->Create(CWnd::GetActiveWindow());
			pGuage->CenterWindow();
			pGuage->ShowWindow(SW_SHOW);
			pGuage->SetRange(0,objNum);
			pGuage->SetTitle("统计孔洞内电导率值...");

			int i,k,m;
			CPoint pt;
			float tmp;
			//定义直方图显示对话框
			CFrequencyDlg dlg;
			dlg.m_MinValue=0.f;
			dlg.m_MaxValue=m_ContourMax;

			//m_StatDiam1=10.f;//小于m_StatDiam1的为小孔
			//m_StatDiam2=50.f;//大于m_StatDiam1为中孔，小于m_StatDiam2
			//m_StatDiam3=100.f;//大于m_StatDiam2为大孔，小于m_StatDiam3大于m_StatDiam3为溶洞
			/*dlg.m_ImageMin=m_StatDiam1;
			dlg.m_ImageMax=m_StatDiam2;*/
			dlg.m_Point=0;
			for(i=0;i<dlg.m_NumScale;i++)
				dlg.m_freq[i]=0;
			
			CRect SlideRect=m_pPickupObj->m_Rect;
			SlideRect.top=m_pPickupObj->DepthToYCoor(sdep);
			SlideRect.bottom=m_pPickupObj->DepthToYCoor(edep);

			CContour* pContour=NULL;
			POSITION pos=m_ContourList.GetHeadPosition();
			objNum=0;
			BOOL bOK=TRUE;		
			while(pos!=NULL)
			{	
				pContour=(CContour*)m_ContourList.GetNext(pos);
				k=pContour->m_pointArray.GetSize();
				/*if(k>4)*/
				{
					m=0;
					//计算滑动窗口内的真实面积
					for(i=0;i<k;i++)
					{
						pt=pContour->m_pointArray[i];
						//进行纵横向不同绘图比例校正
						pt.x=m_PickupRect.left+(int)((pt.x-m_PickupRect.left)*Her);
						pt.y=m_PickupRect.top+(int)((pt.y-m_PickupRect.top)*Ver);
						//进行整体偏移
						pt.Offset(OffPt);
						if(SlideRect.PtInRect(pt))
						{
							m++;
							break;
						}
					}
					if(m>0)
					{
						//求孔洞直径
						tmp=pContour->ComputeArea();
						tmp*=m_PickupVerScale*0.254f;
						tmp*=m_PickupHorScale*0.254f;
						//标准井线性刻度
						tmp=tmp*m_fA+m_fB;
						tmp=(float)sqrt(tmp/3.1415926)*2.f;
						//计算孔径频谱
						k=(int)(tmp*dlg.m_NumScale/m_ContourMax);
						if((k>-1)&&(k<dlg.m_NumScale))
						{
							dlg.m_freq[k]++;
							dlg.m_Point++;
						}
					}
				}
				objNum++;
				pGuage->SetPos(objNum);
				if(pGuage->CheckCancelButton())
				{
					bOK=FALSE;
					break;
				}
			}
			delete pGuage;
			if(bOK) dlg.DoModal();
		}
	}
}

void CImageManView::OnUpdateDrawDiameter(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_ContourList.IsEmpty()&&m_pPickupObj!=NULL);
}
//孔洞面孔率直方图分析
void CImageManView::OnDrawPor()
{
	float sdep=pDoc->m_PlotSdep;
	float edep=pDoc->m_PlotEdep;
	if(m_pSelectObj!=NULL)
	{
		if(m_pSelectObj->IsKindOf(RUNTIME_CLASS(CDepSect)))
		{
			int DaltaY=pDoc->GetLeftTrackObj ()->GetClientRect ().top-pDoc->m_TopSpace;
			CPoint pt1=((CDepSect*)m_pSelectObj)->m_Point1 ;
			CPoint pt2=((CDepSect*)m_pSelectObj)->m_Point2 ;
			sdep=pDoc->YCoorToDepth (pt1.y-DaltaY);
			edep=pDoc->YCoorToDepth (pt2.y-DaltaY);
		}
	}
	
	CString szStr=m_pPickupObj->m_strPathName;
	CString szWellName=szStr.Left(szStr.ReverseFind('\\'));
	szWellName=szWellName+"\\"+szWellName.Mid(szStr.ReverseFind('#')+1);

	float stDep,enDep;
	float Rlev=-999.25f;
	int nps=0;
	CFIDIO* inFID=new CFIDIO(m_pPickupObj->m_strPathName);
	if(inFID->Open())
	{
		FIDCONTENT *Content = inFID->GetContent(3);
		if(Content != NULL)
			nps=Content->npw;
		stDep=inFID->GetStartDepth();
		enDep=inFID->GetEndDepth();
		Rlev=inFID->GetDepthLevel();
	}
	if(nps<1||Rlev<0.f) 
	{
		delete inFID;
		AfxMessageBox(m_pPickupObj->m_strPathName+"\n曲线打开失败!        ");
		return;
	}	
	
	float por0;
	CFIDIO* pPOR= new CFIDIO(szWellName+".POR");
	if(!pPOR->Open()) 
	{
		delete inFID;delete pPOR;
		AfxMessageBox(szWellName+".POR"+"\n外部孔隙度曲线打开失败!        ");
		return;
	}	
	
	float fLLs;	
	CFIDIO* pLLs = new CFIDIO(szWellName+".RXO");
	if(!pLLs->Open()) 
	{
		delete inFID;delete pPOR;delete pLLs;
		AfxMessageBox(szWellName+".RXO"+"\n浅侧向曲线打开失败!        ");
		return;
	}

	BeginWaitCursor();
	float *Value=new float[nps];
	int i,k,p,m;
	
	float dep,tmp,ave;
	float SPOR,fXM=1.f/2.f;
	float PORMax=20.f;
	//定义直方图显示对话框
	CFrequencyDlg dlg;
	dlg.m_MinValue=0.f;
	dlg.m_MaxValue=PORMax;
	dlg.m_Point=0;
	for(i=0;i<dlg.m_NumScale;i++)
		dlg.m_freq[i]=0;

	k=0;dep=sdep;
	while(dep<edep)
	{
		//读取阵列电阻率数据
		inFID->SeekToDepth(dep);
		inFID->Read(3,nps,Value);
	
		//读取滑动窗口中部深度位置的外部孔隙度
		por0=PORMax;	
		if(pPOR->SeekToDepth(dep)>-1)
			pPOR->Read(2,1,&por0);
		//读取浅侧向值
		fLLs=m_Rm;
		if(pLLs->SeekToDepth(dep)>-1)
			pLLs->Read(2,1,&fLLs);
		//计算平均电导率
		ave=0.f;m=0;
		for(i=0;i<nps;i++)
		{		
			if(Value[i]>0.f)
			{
				ave+=Value[i];
				m++;
			}
		}
		if(m>0) ave/=m;
		if(ave>0.f)
		{
			SPOR=por0;
			tmp=fLLs/ave;
			SPOR/=pow(tmp,fXM);
			for(i=0;i<nps;i++)
			{	
				tmp=fLLs/Value[i];
				if(tmp>0.f)
				{		
					p=(int)(pow(tmp,fXM)*SPOR*dlg.m_NumScale/PORMax);
					if(p>-1&&p<dlg.m_NumScale)
					{
						dlg.m_freq[p]++;
						dlg.m_Point++;
					}
				}
			}
		}
		dep=sdep+Rlev*k;
		k++;
	}
	delete Value;delete inFID;
	delete pPOR;delete pLLs;
	EndWaitCursor();
	dlg.DoModal();
}

void CImageManView::OnUpdateDrawPor(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pPickupObj!=NULL);
}
//根据输入的约束曲线下限值生成缝洞综合统计表
void CImageManView::OnCreateimagelist()
{
	if(m_ContourList.IsEmpty()||m_pPickupObj==NULL)
		return;
	CString szStr=pDoc->m_PathName;
	if(szStr.ReverseFind('.')>0)
		szStr=szStr.Left(szStr.ReverseFind('.'));
	CCreateImageListDlg dlg;
	dlg.m_szProjectName=szStr;
	if(dlg.DoModal()==IDOK)
	{
		float minValue=dlg.m_MinValue;
		float minThick=dlg.m_MinThick;
		float sdep,edep,rlev;
		//裂缝密度 孔隙度曲线名 FVDC\FVPA
		CString szFVPA=szStr+"."+dlg.m_szFVPA;
		CString szFVDC=szStr+"."+dlg.m_szFVDC;
		CFIDIO* inFID=new CFIDIO(szStr+"."+dlg.m_szPorName);
		if(inFID->Open())
		{
			sdep=inFID->GetStartDepth();
			edep=inFID->GetEndDepth();
			rlev=inFID->GetDepthLevel();
		}
		else
		{
			delete inFID;
			AfxMessageBox(szStr+"."+dlg.m_szPorName+"\n曲线打开失败!        ");
			return;
		}
		if(sdep<pDoc->m_PlotSdep)
			sdep=pDoc->m_PlotSdep;
		if(edep>pDoc->m_PlotEdep)
			edep=pDoc->m_PlotEdep;

		szStr=szStr.Left(szStr.ReverseFind('\\')+1)+"ImageResult.List";
		CStdioFile File;
		if(!File.Open(szStr,CFile::modeWrite | CFile::modeCreate| CFile::typeText)) 
		{
			AfxMessageBox(szStr+"\n文件创建失败!");
			delete inFID;return;
		}
		//列表题
		szStr="序号,起始深度,结束深度,厚度,洞密度(个/平米井壁),大洞数(个),中洞数(个),小洞数(个),裂缝条数(条),缝密度(条/米井段),裂缝视孔隙度\n";
		File.WriteString(szStr);

		//定义进度指示器
		CGuage* pGuage = new CGuage(sdep,edep);
		pGuage->Create(CWnd::GetActiveWindow());
		pGuage->CenterWindow();
		pGuage->ShowWindow(SW_SHOW);
		pGuage->SetTitle("生成缝洞综合统计表...");
	
		float value,dep1,dep2;
		dep1=sdep;dep2=sdep;
		int dpNum=(int)((edep-sdep)/rlev);
		
		CString szTmp;
		bool bStart=false;
		bool bOk=false;
		int No=0,k=0;
		inFID->SeekToDepth(sdep);
		while(k<dpNum)
		{
			inFID->Read(2,1,&value);
			inFID->Next();
			if(value>minValue)
			{
				if(!bStart)
				{
					bStart=true;
					dep1=sdep+k*rlev;
					bOk=false;
				}
			}
			else if(bStart&&!bOk)
			{
				bStart=false;
				dep2=sdep+k*rlev;
				bOk=true;
			}
			if(bOk&&(dep2-dep1>minThick))
			{
				No++;
				szStr=StatHolePara(No,dep1,dep2);
				//裂缝条数,裂缝密度
				if(pDoc->m_SinObjList.GetCount()>0)
				{
					szTmp=StatCrackPara(dep1,dep2);
					if(szTmp==",/,/")
						szTmp+=",/\n";
					else
					{
						szStr+=szTmp;
						//裂缝孔隙度
						value=StatCurvePara(szFVPA,dep1,dep2);
						szTmp.Format(",%.10f\n",value);
					}
					szStr+=szTmp;
				}
				else//若未加载裂缝
				{
					//裂缝密度
					value=StatCurvePara(szFVDC,dep1,dep2);
					szTmp.Format(",/,%f",value);
					szStr+=szTmp;
					//裂缝孔隙度
					value=StatCurvePara(szFVPA,dep1,dep2);
					szTmp.Format(",%.10f\n",value);
					szStr+=szTmp;
				}
				File.WriteString(szStr);
				bOk=false;
			}
			k++;
			pGuage->SetDepth(sdep+k*rlev);
			if(pGuage->CheckCancelButton())
				break;
		}
		File.Close();delete inFID;delete pGuage;
	}
}
//统计曲线最大值
float CImageManView::StatCurvePara(CString PathName,float dep1,float dep2)
{
	int dpNum=0;
	float value,maxVal=0.f;
	CString szStr="";
	CFIDIO* inFID=new CFIDIO(PathName);
	if(inFID->Open())
	{
		float sdep=inFID->GetStartDepth();
		float edep=inFID->GetEndDepth();
		float rlev=inFID->GetDepthLevel();
		if(dep1<sdep) dep1=sdep;
		if(dep2>edep) dep2=edep;
		dpNum=(int)((dep2-dep1)/rlev);
	}
	if(dpNum<1) 
	{
		delete inFID;
		return maxVal;
	}
	
	inFID->SeekToDepth(dep1);
	int j=0;
	while(j<dpNum)
	{
		inFID->Read(2,1,&value);
		inFID->Next();
		if(maxVal<value)
			maxVal=value;
		j++;
	}
	delete inFID;
	return maxVal;
}
//统计孔洞参数
//返回 序号,起始深度,终止深度,段长,洞密度,大洞数,中洞数,小洞数
CString CImageManView::StatHolePara(int No,float dep1,float dep2)
{
	CString szStr;
	szStr.Format("%d,%.3f,%.3f,%.3f,/,/,/,/",No,dep1,dep2,dep2-dep1);
	int DataNum=0;
	CFIDIO* inFID=new CFIDIO(m_PickupFIDName);
	if(inFID->Open())
	{
		FIDCONTENT *Content = inFID->GetContent(3);
		if(Content != NULL)
			DataNum=Content->npw;
	}
	if(DataNum<1) 
	{
		delete inFID;
		return szStr;
	}
	CPoint pt,OffPt;
	OffPt.x=m_pPickupObj->m_Rect.left-m_PickupRect.left;
	OffPt.y=pDoc->DepthToYCoor(m_PickupStDep)-pDoc->DepthToYCoor(pDoc->m_PlotSdep);
	OffPt.y+=m_pPickupObj->m_Rect.top-m_PickupRect.top;
	
	//纵横向不同绘图比例校正系数
	double Her,Ver;
	Her=m_pPickupObj->m_Rect.Width()*1.0/m_PickupRect.Width();
	Ver=m_PickupVerScale/pDoc->m_DepthScale;

	CArray<CPoint,CPoint&>ptAr;
	
	CRgn rgn;
	int x,y,ptNum;
	int i,k,m,n,Num;
	CRect SlideRect,rect;

	float WinS,s,area,hpor,sumb,d,dmax,dmin,dave;
	//分区面孔率和孔洞数
	float Hpor1;float Sumb1;
	float Hpor2;float Sumb2;
	float Hpor3;float Sumb3;
	float Hpor4;float Sumb4;
	//孔洞水平垂直联通性
	float perh,perv,pert,Rx,Ry,tmp;
	Num=0;
	CContour* pContour=NULL;

	SlideRect=m_pPickupObj->m_Rect;
	SlideRect.top=m_pPickupObj->DepthToYCoor(dep1);
	SlideRect.bottom=m_pPickupObj->DepthToYCoor(dep2);

	////一个统计窗口的井壁面积mm2
	//WinS=(float)(PI*m_pPickupObj->m_Bit*(dep2-dep1)*1000.f);
	////自动拾取覆盖率m_CoverageRate
	//WinS=WinS*m_CoverageRate;

	//一个统计窗口的井壁面积m2
	WinS=(float)(PI*(m_pPickupObj->m_Bit/1000.f)*(dep2-dep1));

	area=0.f;hpor=0.f;sumb=0.f;
	dmax=0.f;dmin=9999.25f;dave=0.f;
	Hpor1=0.f;Sumb1=0.f;
	Hpor2=0.f;Sumb2=0.f;
	Hpor3=0.f;Sumb3=0.f;
	Hpor4=0.f;Sumb4=0.f;
	perh=0.f;perv=0.f;pert=0.f;

	BOOL bRead;
	float *pData=new float[DataNum];
	POSITION pos=m_ContourList.GetHeadPosition();
	while(pos!=NULL)
	{	
		pContour=(CContour*)m_ContourList.GetNext(pos);
		k=pContour->m_pointArray.GetSize();
		//计算单个图像特征面积ptNum
		ptNum=0;
		if(k>4)
		{
			n=0;
			ptAr.SetSize(k,5);
			for(i=0;i<k;i++)
			{
				pt=pContour->m_pointArray[i];
				//进行纵横向不同绘图比例校正
				pt.x=m_PickupRect.left+(int)((pt.x-m_PickupRect.left)*Her);
				pt.y=m_PickupRect.top+(int)((pt.y-m_PickupRect.top)*Ver);
				//进行整体偏移
				pt.Offset(OffPt);
				if(SlideRect.PtInRect(pt))
				{
					ptAr[n]=pt; 
					n++;
				}
			}
			//计算区域内部的面积ptNum
			if(n>4)
			{
				if(rgn.CreatePolygonRgn(&ptAr[0],n,ALTERNATE))
				{
					rgn.GetRgnBox(&rect);
					//计算面积
					n=0;tmp=0;
					for(y=rect.top; y<=rect.bottom; y++)
					{
						bRead=TRUE;
						for(x=rect.left; x<=rect.right; x++)
						{
							if(rgn.PtInRegion(CPoint(x,y)))
							{
								ptNum++;//计算面积
								//计算联通指数
								if(bRead)
								{
									inFID->SeekToDepth((float)m_pPickupObj->YCoorToDepth(y));
									inFID->Read(3,DataNum,pData);
									bRead=FALSE;
								}
								m=(x-m_pPickupObj->m_Rect.left)*DataNum/m_pPickupObj->m_Rect.Width();
								if(m>-1&&m<DataNum)
								{
									if(pData[m]>0.f)
									{
										tmp+=(pData[m]-m_ImageMin)/(m_ImageMax-m_ImageMin);
										n++;
									}
								}
							}
						}
					}
					rgn.DeleteObject();
					//计算联通指数
					Rx=rect.Width()*m_PickupHorScale*0.254f;
					Ry=rect.Height()*m_PickupVerScale*0.254f;
					if(n>0)
					{
						Rx=Rx*tmp/n;
						Ry=Ry*tmp/n;
					}
					perh+=Rx;
					perv+=Ry;

					ptNum=(int)(ptNum*m_PickupVerScale*0.254f);
					ptNum=(int)(ptNum*m_PickupHorScale*0.254f);
					
					//标准井线性刻度
					ptNum=ptNum*m_fA+m_fB;
					area+=ptNum;

					//求最大颗粒孔洞直径
					s=(float)pContour->ComputeArea();
					s=s*m_PickupVerScale*0.254f;
					s=s*m_PickupHorScale*0.254f;

					d=(float)(2.f*sqrt(s/PI));
					if(d>dmax) dmax=d;
					//统计大于最小分辨0.2英寸的孔洞
					if(d<dmin&&d>0.5f) dmin=d;
					dave+=d;

					sumb+=1.f;
					if(d<=m_StatDiam1)
					{
						Hpor1+=ptNum;Sumb1+=1.f;
					}
					else if(d<=m_StatDiam2)
					{
						Hpor2+=ptNum;Sumb2+=1.f;
					}
					else/* if(d<=m_StatDiam3)*/
					{
						Hpor3+=ptNum;Sumb3+=1.f;
					}
					/*else
					{
						Hpor4+=ptNum;Sumb4+=1.f;
					}*/
				}
			}
			ptAr.SetSize(0);
		}
	}
	if(sumb>0.f)
	{
		//pert=(float)sqrt(perh*perh+perv*perv);		
		//pert=pert/WinS*area/sumb;				//总联通指数 	 HPERT  单位none
		//perh=perh/WinS*area/sumb;				//水平联通指数 	 HPERH  单位none
		//perv=perv/WinS*area/sumb;				//垂直联通指数 	 HPERV  单位none

		//area=area/WinS*100.f;
		//Hpor1=Hpor1/WinS*100.f;
		//Hpor2=Hpor2/WinS*100.f;
		//Hpor3=Hpor3/WinS*100.f;
		//Hpor4=Hpor4/WinS*100.f;
		//dave=dave/sumb;
		////序号,深度,总孔洞数,面孔率,平均孔径,联通指数,小孔面孔率,小孔个数,中孔面孔率,中孔个数,大孔面孔率,大孔个数,溶洞面孔率,溶洞个数,最大孔径,最小孔径,水平联通指数,垂直联通指数
		//if(dmin>9999.f)							//最小孔洞直径   DMIN   单位mm
		//	szStr.Format("\n%s,%.1f--%.1f,%g,%.2f,%.2f,%f,%.3f,%g,%.3f,%g,%.3f,%g,%.3f,%g,%.2f,／,%f,%f",
		//		pObj->m_NoStr,pObj->m_Sdep,pObj->m_Edep,sumb,area,dave,pert,Hpor1,Sumb1,Hpor2,Sumb2,Hpor3,Sumb3,Hpor4,Sumb4,dmax,perh,perv);
		//else
		//	szStr.Format("\n%s,%.1f--%.1f,%g,%.2f,%.2f,%f,%.3f,%g,%.3f,%g,%.3f,%g,%.3f,%g,%.2f,%f,%f,%f",
		//		pObj->m_NoStr,pObj->m_Sdep,pObj->m_Edep,sumb,area,dave,pert,Hpor1,Sumb1,Hpor2,Sumb2,Hpor3,Sumb3,Hpor4,Sumb4,dmax,dmin,perh,perv);
		area=sumb/WinS;
		szStr.Format("%d,%.3f,%.3f,%.3f,%.1f,%d,%d,%d",No,dep1,dep2,dep2-dep1,area,(int)Sumb3,(int)Sumb2,(int)Sumb1);
	}
	delete inFID;delete pData;
	return szStr;
}
//统计裂缝参数
//返回 裂缝条数,缝密度
CString CImageManView::StatCrackPara(float dep1,float dep2)
{
	CString szReturn=_T(",/,/");
	//图中绘制的总裂缝图
	int Num=pDoc->m_SinObjList.GetCount();
	if(Num<1)  return szReturn;

	CRect SlideRect,rect,rect1,rect2;

	int sumb=0;		//裂缝条数
	float fvdc=0.f;	//裂缝密度
	float SinH;

	CSinPoint *pSin=NULL;
	
	SlideRect=pDoc->m_Rect;
	SlideRect.top=m_pPickupObj->DepthToYCoor(dep1);
	SlideRect.bottom=m_pPickupObj->DepthToYCoor(dep2);

	//统计一个窗口内的裂缝储层参数
	POSITION pos= pDoc->m_SinObjList.GetHeadPosition();
	while(pos!= NULL) 
	{
 		pSin=(CSinPoint *)pDoc->m_SinObjList.GetNext(pos);
		if(pSin==NULL)
			continue;
		//是否符合统计条件
		/*if(m_sPickupSinLabel.Find(pSin->m_Flag)<0)
			continue;*/
		rect1=pSin->m_Rect;
		SinH=m_pPickupObj->m_Bit/2000.f*(float)(tan(pSin->m_Dip0*PI/180.));
		rect1.top=m_pPickupObj->DepthToYCoor(pSin->m_Dep-SinH);
		rect1.bottom=m_pPickupObj->DepthToYCoor(pSin->m_Dep+SinH);

		rect2=SlideRect&rect1;
		if(rect2.Height()>1)
			sumb++;
	}
	if(sumb>0)
	{
		fvdc=sumb/(dep2-dep1);
		//裂缝条数,裂缝密度
		szReturn.Format(",%d,%.3f",sumb,fvdc);	
	}
	return szReturn;
}

void CImageManView::OnUpdateCreateimagelist(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pPickupObj!=NULL/*&&!pDoc->m_SinObjList.IsEmpty()*/&&!m_ContourList.IsEmpty());
}
