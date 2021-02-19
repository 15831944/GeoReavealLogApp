/******************************************************************
*   ImageManDoc.cpp - implementation of the CImageManDocclass     *
*																  *
*    author:zhaozhongmin	2019.3								  *
*                                                                 *
*******************************************************************/

#include "stdafx.h"
#include "afxpriv.h"
#include "ImageMan.h"
#include "TrackDraw.h"
#include "LineDraw.h"
#include <io.h>

#include "ImageManDoc.h" 
#include "MainFrm.h" 
#include "depthdialog.h"
#include "ImageManView.h"
#include "PageSetup.h"
#include "FIDIO.h"
#include "WidthDialog.h"
#include "HeightDialog.h"
#include "PrintDlg.h"
#include "ScaleFactor.h"
#include "DepSect.h"
#include "StratumDraw.h"
#include "DIPCalculate.h"
#include "MarkVectorDraw.h"
#include "MarkDipCorre.h"
#include "LogProcess.h"
#include "MarkFreq.h"
#include "ACImagePre.h"
#include "ImageDraw.h"
#include "ModifyHandWorkDlg.h"
#include "ResultDraw.h"
#include "FoldDraw.h"

#ifdef AFX_OLE_SEG
#pragma code_seg(AFX_OLE_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// private clipboard format (list of Draw objects)
CLIPFORMAT CImageManDoc::m_cfDraw =
#ifdef _UNIX_
	NULL;
#else
	(CLIPFORMAT)::RegisterClipboardFormat("LOG EXPRESS TOP LogPlot ");
#endif

////////////////////////////////////////////////////////////////////////////
// CImageManDoc

IMPLEMENT_DYNCREATE(CImageManDoc, COleDocument)

BEGIN_MESSAGE_MAP(CImageManDoc, COleDocument)
	//{{AFX_MSG_MAP(CImageManDoc)
	ON_COMMAND(ID_WELLDATAUPDATE,OnUpdateWell)
	ON_COMMAND(ID_VIEW_PAPER_COLOR, OnViewPapercolor)
	ON_COMMAND(ID_SET_DEPTH, OnSetDepth)
	ON_UPDATE_COMMAND_UI(ID_SET_DEPTH, OnUpdateSetDepth)
	ON_COMMAND(ID_SET_DEPTHSCALE, OnSetDepthscale)
	ON_COMMAND(ID_SETUP_PAGE, OnSetupPage)
	ON_COMMAND(ID_DEPTH_UNIT_FEET, OnDepthUnitFeet)
	ON_UPDATE_COMMAND_UI(ID_DEPTH_UNIT_FEET, OnUpdateDepthUnitFeet)
	ON_COMMAND(ID_DEPTH_UNIT_M, OnDepthUnitM)
	ON_UPDATE_COMMAND_UI(ID_DEPTH_UNIT_M, OnUpdateDepthUnitM)
	ON_COMMAND(ID_PLOTFIDHEAD, OnPlotFIDhead)
	ON_UPDATE_COMMAND_UI(ID_PLOTFIDHEAD, OnUpdatePlotFIDhead)
	ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
	ON_COMMAND(ID_MOUSE_TIP, OnMouseTip)
	ON_UPDATE_COMMAND_UI(ID_MOUSE_TIP, OnUpdateMouseTip)
	ON_COMMAND(ID_FILL_LITHGROUND, OnFillLithground)
	ON_UPDATE_COMMAND_UI(ID_FILL_LITHGROUND, OnUpdateFillLithground)
	ON_COMMAND(ID_SAVE_DATA, OnSaveData)
	ON_UPDATE_COMMAND_UI(ID_SAVE_DATA, OnUpdateSaveData)
	ON_COMMAND(ID_SAVEAS_IMAGEMARK, OnSaveasImagemark)
	ON_UPDATE_COMMAND_UI(ID_SAVEAS_IMAGEMARK, OnUpdateSaveasImagemark)
	ON_COMMAND(ID_SET_MARKCORREPARA, OnSetMarkcorrepara)
	ON_UPDATE_COMMAND_UI(ID_SET_MARKCORREPARA, OnUpdateSetMarkcorrepara)
	ON_COMMAND(ID_DOCDATA_UPDATE,OnReFresh)
	ON_COMMAND(ID_BOX_ANISYS,OnBoxAnisys)
	ON_UPDATE_COMMAND_UI(ID_BOX_ANISYS,OnUpdateBoxAnisys)
	//}}AFX_MSG_MAP
	// Enable default OLE container implementation
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageManDocconstruction/destruction

CImageManDoc::CImageManDoc()
{                               
	EnableCompoundFile();
	bNewDocument = FALSE;
	bEnableChildSizeMessage = TRUE;
	m_LeftSpace = m_RightSpace = m_TopSpace = m_BottomSpace =40;
	m_HeadTopSpace = m_HeadBottomSpace = 40;
	m_Rect.top = m_Rect.left = 0;
	m_Rect.right   = 1080; m_Rect.bottom = 1080;
	m_TrackRect.left = m_LeftSpace; m_TrackRect.right = m_Rect.right - m_RightSpace;
	m_TrackRect.top = m_TopSpace; m_TrackRect.bottom = m_Rect.bottom - m_BottomSpace;
	m_DepthScale = 200;
	m_PlotSdep = 1000;
	m_PlotEdep = 2200;
	m_bChildTrackHeadAlignMax = TRUE;
	m_bChildTrackHeadHeightAuto = TRUE;
	m_HoriArrangleMode = IDM_LEFT;
	m_bVertAlignTop = TRUE;
	m_bVertAlignHead = FALSE;
	m_bVertAlignDepth = TRUE;
	bVertAutoAlign = TRUE;
	bHoriAutoArrangle = TRUE;
	m_paperColor = RGB(255, 255, 0);
	m_xGrid = m_yGrid = 8;
	bCopyFromTemplate = FALSE;
	bNullTemplate = FALSE;
	m_DocumentType = STAND_DOC;
	m_Flag = 1;
	m_DepthUnit = UNIT_M;
	m_DepC = m_DepthScale * 2.54 / 10000.;
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	if(pMain != NULL) 
	{
		m_ScaleNo = pMain->m_ScaleNo;
		m_ScaleFactor = pMain->GetScaleFactor(m_ScaleNo);
	}
	else
	{
		m_ScaleNo = 7;
		m_ScaleFactor = 100;
	}
	m_DrawFIDHeadMode = 0;
	m_bPlotTopRem =m_bPlotBottomRem=m_bReserveSpace  =FALSE;
	m_bPlotResultList=m_bPlotHead =m_bMaxPageSize=FALSE;
	m_bPlotTrack=TRUE;
	m_VersionNo=10.0f;
	m_MouseTip=FALSE;
	m_FillLithGround=TRUE;
	m_OldWellName=_T("");
	m_UnDo=FALSE;
	m_bHandDrawSin=FALSE;
	m_DataBuf=((CImageManApp*)AfxGetApp())->m_DataBuf;
	m_HWOprateNum=0;
	m_ImageHandWorkFile=_T("");

	//流程链处理中数据缓冲区位置
	m_szBufPath="";
	m_bBoxAnisys=TRUE;
}

CImageManDoc::~CImageManDoc()
{
	DeleteObList();
	if (bCopyFromTemplate)
	{
		// be sure to delete template file
		TRY
		{
			CFile::Remove(m_LogName+".LPD");
		}
		CATCH_ALL(e)
		{
			TRACE0("Warning: failed to delete file after failed SaveAs.\n");
			delete e;
		}
		END_CATCH_ALL
		bCopyFromTemplate = FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CImageManDocserialization

void CImageManDoc::Serialize(CArchive& ar)
{
	char m_AppName[] = "LOGPLOT   ";//SICHUAN LOG
	char Version[10]="10.0     ";
	CString m_MethodName;
	m_Flag = 1;
	char tmp[100]; 
	strcpy(tmp,"Copyright (C) 2000-2005 SICHUAN LOG Company All rights reserved.");
	if(!ar.IsStoring())               
	{
		ar.Read(m_AppName,10);
		m_AppName[10] = '\0';
		if (!strncmp(m_AppName,"LOGPLOT   ",10)) 	 m_Flag = 1;
		else if (!strncmp(m_AppName,"LOGPROCESS",10))m_Flag = 2;
		else
		{
			m_Flag = 0;
			AfxMessageBox(IDS_UNKNOWN_FILEFORMAT,MB_OK | MB_ICONEXCLAMATION);
			return;
		}
		ar.Read(Version,6);
		m_VersionNo=(float)atof(Version);
		ar >> m_DepthScale;
		ar >> m_DepthUnit;
		ar >> m_Rect;     
		ar >> m_TrackRect;
		ar >> m_paperColor;
		ar >> m_PlotSdep;
		ar >> m_PlotEdep;
		ar >> m_LeftSpace;
		ar >> m_RightSpace;
		ar >> m_TopSpace;
		ar >> m_BottomSpace;
		ar >> m_HeadRect;     
		ar >> m_HeadTopSpace;
		ar >> m_HeadBottomSpace;
		ar >> m_HoriArrangleMode;
		ar >> m_bVertAlignDepth;
		ar >> m_bVertAlignHead;
		ar >> m_bVertAlignTop;
		ar >> bHoriAutoArrangle;
		ar >> bVertAutoAlign;
		ar >> m_xGrid;
		ar >> m_yGrid;
		ar >> m_bChildTrackHeadAlignMax;
		ar >> m_bChildTrackHeadHeightAuto;
		ar.Read(tmp,100);
		if(tmp[98]=='1')
			m_FillLithGround=FALSE;
		else	m_FillLithGround=TRUE;
	} 
	if (m_DepthUnit == UNIT_M) m_DepC = m_DepthScale * 2.54 / 10000.;
	else m_DepC = m_DepthScale  / 1200.;
	COleDocument::Serialize(ar);
	m_TrackList.Serialize(ar);
}

BOOL CImageManDoc::OnNewDocument()
{
	m_szBufPath="";
	if (!COleDocument::OnNewDocument())
		return FALSE;
	CString CmdMessage = ((CImageManApp*)AfxGetApp())->m_PlotFileName;
	((CImageManApp*)AfxGetApp())->m_PlotFileName="";
	int xxx;
	m_ProSdep=-9999.f;
	m_ProEdep=-9999.f;
	if(!CmdMessage.IsEmpty ())
	{
		xxx=CmdMessage.Find(">");
		if(xxx>-1)
		{
			m_PathName=CmdMessage.Left(xxx);
			CString sxx=CmdMessage.Mid(xxx+1);
			xxx=sxx.Find ("<");
			if(xxx==-1)
				m_CmdTemplateName=sxx;
			else
			{
				m_CmdTemplateName=sxx.Left (xxx);
				m_sSeries=m_CmdTemplateName.Left(m_CmdTemplateName.Find("_"));
				m_sSeries=m_sSeries.Mid(m_sSeries.ReverseFind('\\')+1);
				sxx=sxx.Mid (xxx+1);
				xxx=sxx.Find ("^");
				if(xxx>0)
				{
					m_szBufPath=sxx.Mid (xxx+1);
					sxx=sxx.Left (xxx);
				}
				xxx=sxx.Find (",");
				m_ProSdep=(float)atof(sxx.Left (xxx));
				m_ProEdep=(float)atof(sxx.Mid (xxx+1));
			}
		}
		else
		{
			if(CmdMessage.Find(".LPD")>-1||CmdMessage.Find(".LPT")>-1)
			{				
				((CImageManApp*)AfxGetApp())->OpenDocumentFile(CmdMessage);
				return FALSE;
			}
			m_PathName=CmdMessage;
		}
	}
	if(m_CmdTemplateName.IsEmpty())
	{
		if(!LoadTemplate()) 
			bNullTemplate=TRUE;
		bNewDocument = TRUE;
	}
	else
	{
		if(!LoadTemplate(10)) 
		{
			if(!m_PathName.IsEmpty())
			{
				if(!LoadTemplate())
					bNullTemplate=TRUE;
			}
			else bNullTemplate=TRUE;
		}
	}
	if(bNullTemplate) 
	{
		ComputePageSize();
		bNewDocument = TRUE;
	}
	else if(m_PathName!="") 
	{
		xxx=m_PathName.Find (".");
		if(xxx>-1)
			m_LogName=m_PathName.Left(xxx);
		else
			m_LogName=m_PathName;
		
		if(m_szBufPath!="")
		{
			if(m_sSeries.Find("FMI")>-1)
				m_PadNum=4;
			else
				m_PadNum=6;
			//交互解释中设置相关曲线名
			m_sAZ1Name="P1AZGPT";                     //I号极板方位曲线
			m_sRBName="RBGPT";                        //相对方位曲线
			m_sDEVName="DEVGPT";                      //井斜角曲线
			m_sDAZName="DAZGPT";                      //井斜方位曲线
			m_szRADSName="RADSGPT";                   //阵列井径
			m_fDip1=m_fDir1=0.f;                      //背景校正的构造倾角、方位
		}
		else if(m_CmdTemplateName.Find("CIFImage")>0)
		{
			m_szBufPath=m_PathName.Left(m_PathName.ReverseFind('.'));
			m_fDip1=m_fDir1=0.f;                      //背景校正的构造倾角、方位
			CString Path=m_PathName.Left(m_PathName.ReverseFind('\\'));
			if(DetectFileExist(Path+"\\*.FCA1"))
			{
				m_sSeries="FMI_8PAD";
				m_PadNum=8;
				//交互解释中设置相关曲线名
				m_sAZ1Name="P1AZ_S";                      //I号极板方位曲线
				m_sRBName="RB_S";                         //相对方位曲线
				m_sDEVName="DEVI_S";                      //井斜角曲线
				m_sDAZName="HAZI_S";                      //井斜方位曲线
				m_szRADSName="RADSGPT";                   //阵列井径
			}
			else if(DetectFileExist(Path+"\\*.PAD1"))
			{
				m_PadNum=6;
				m_sSeries="EMI";
				//交互解释中设置相关曲线名
				m_sAZ1Name="AZI1_S";                      //I号极板方位曲线
				m_sRBName="RB_S";                         //相对方位曲线
				m_sDEVName="DEVI_S";                      //井斜角曲线
				m_sDAZName="HAZI_S";                      //井斜方位曲线
				m_szRADSName="RADSGPT";                   //阵列井径	
			}
		}
		if(!bNullTemplate)
		{
			OpenWell(m_PathName);
			if(GetStratumObject()!=NULL)
				m_bHandDrawSin=TRUE;
		}
	}
	return TRUE;
}

BOOL CImageManDoc::OnSaveDocument(const char* pszPathName)
{
	CString m_SaveLogName = m_LogName;
	CFIDIO::GetFullWellName(pszPathName,m_LogName.GetBuffer(512));
	m_LogName.ReleaseBuffer();
	CString m_savePath = m_strPathName; 
	if (bCopyFromTemplate) m_strPathName = m_LogName + "LPD";
	if (!COleDocument::OnSaveDocument(pszPathName))
	{
		m_LogName = m_SaveLogName;
		m_strPathName = m_savePath;
		return FALSE;
	}
	if (bCopyFromTemplate && (m_LogName+".LPD" != pszPathName))
	{
		// be sure to delete template file
		TRY
		{
			CFile::Remove(m_LogName+".LPD");
		}
		CATCH_ALL(e)
		{
			TRACE0("Warning: failed to delete file after failed SaveAs.\n");
			delete e;
		}
		END_CATCH_ALL
		bCopyFromTemplate = FALSE;
	}
	return TRUE;
}

BOOL CImageManDoc::OnOpenDocument(const char* pszPathName)
{               
	m_PathName = pszPathName;
	CFIDIO::GetFullWellName(m_PathName,m_LogName.GetBuffer(512));
	m_LogName.ReleaseBuffer();
	if (!COleDocument::OnOpenDocument(pszPathName)) 
		return FALSE;
	if (m_Flag == 0) 
		return FALSE;
	SetSize();
	MatchAllObject(m_PathName);
	if(GetStratumObject()!=NULL)
		m_bHandDrawSin=TRUE;
	return TRUE;	
}

void CImageManDoc::DeleteContents()
{
	COleDocument::DeleteContents();
	DeleteObList();
	m_LeftSpace = m_RightSpace = m_TopSpace = m_BottomSpace = m_HeadTopSpace = m_HeadBottomSpace = 40;
	m_Rect.top = m_Rect.left = 0;
	m_Rect.right = 1080; m_Rect.bottom = 1080;
	m_TrackRect.left = m_LeftSpace; m_TrackRect.right = m_Rect.right - m_RightSpace;
	m_TrackRect.top = m_TopSpace; m_TrackRect.bottom = m_Rect.bottom - m_BottomSpace;
	m_DepthScale = 200;
	m_PlotSdep = 2000;
	m_PlotEdep = 2200;
	m_HoriArrangleMode = IDM_LEFT;
	m_bVertAlignTop = TRUE;
	m_bVertAlignHead = FALSE;
	m_bVertAlignDepth = TRUE;
	bVertAutoAlign = TRUE;
	bHoriAutoArrangle = TRUE;
	m_paperColor = RGB(255, 255, 255);
	m_xGrid = m_yGrid = 8;
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CImageManView)))
			((CImageManView*)pView)->DeleteContents();
	}   
}

BOOL CImageManDoc::LoadTemplate(int flag)
{
	CString TemplateName;
	if(flag==10)	
		TemplateName=m_CmdTemplateName;
	else
	{
		static char BASED_CODE szFilter[] = 
		"测井绘图模板(*.LPT)|*.LPT|所有文件(*.*)|*.*||";
		CFileDialog  Browse(TRUE, "测井绘图模板(*.LPT)", "*.LPT",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
		Browse.m_ofn.lpstrTitle = "加载绘图模板...";
		Browse.m_ofn.lpstrInitialDir=GetWorkPath("Template");
		if (Browse.DoModal() != IDOK) return FALSE;
		TemplateName=Browse.GetPathName();
		if (!SaveModified()) return FALSE;
		m_CmdTemplateName=TemplateName;
	} 

	FILE *fp1=NULL;
	FILE *fp2=NULL;
	CString path;
	if((fp1 = fopen(TemplateName,"rb")) != NULL)
	{
        DeleteContents();
		if (m_lpRootStg != NULL)
		{
			m_lpRootStg->Release();
			m_lpRootStg = NULL;
		}
		CString str;
		CString str1=GetWorkPath();
		for(int xx=1;xx<50;xx++)
		{
			str.Format ("-Pre%d",xx);//m_LogName
			path =  str1+str+ ".temp";
			if ((fp2 = fopen(path,"wb")) != NULL)
			{
				char buffer[1000];
				while (!feof(fp1))
				{
					int k = fread(buffer,1,1000,fp1);
					if (k>0) fwrite(buffer,k,1,fp2);
				}
				fclose(fp1);
				fclose(fp2);
				CString m_oldTitle = m_strTitle;
				CString m_oldPathName = m_strPathName;
				if(COleDocument::OnOpenDocument(path))//
				{
					SetTitle(m_oldTitle);			
					m_strPathName = m_oldPathName;
					return TRUE;
				}
				return FALSE;
			}
		}
		fclose(fp1);
	}
	AfxMessageBox(IDS_OPEN_TEMPLATE_FAILURE,MB_OK|MB_ICONERROR);
	return FALSE;
}

void CImageManDoc::InitDepth() 
{
	BOOL bExist = FALSE;
	char szFind[512];
	CFIDIO::GetFileName(m_LogName,szFind,'I');
	strcat(szFind,".*");
	CString szDir;
	CFIDIO::GetPath(m_LogName,szDir.GetBuffer(512));
	szDir.ReleaseBuffer();
	struct _finddata_t c_file;
	long hFile;
	BOOL bFirstCurve = TRUE;
	if ((hFile = _findfirst(szFind, &c_file )) != -1L)
	{
		CFIDIO* pFIDIO = new CFIDIO(szDir + c_file.name);
		if (pFIDIO->Open() && (pFIDIO->GetDimenSion() == 2) 
			&& (pFIDIO->GetDepthLevel() > 0.001))
		{
			m_PlotSdep = pFIDIO->GetStartDepth();
			m_PlotEdep = pFIDIO->GetEndDepth();
			bExist = TRUE;
		}
		delete pFIDIO;
		while ((!bExist) && (_findnext(hFile,&c_file) == 0)) 
		{
			CFIDIO* pFIDIO = new CFIDIO(szDir + c_file.name);
			if (pFIDIO->Open() && (pFIDIO->GetDimenSion() == 2) 
				&& (pFIDIO->GetDepthLevel() > 0.001))
			{
				m_PlotSdep = pFIDIO->GetStartDepth();
				m_PlotEdep = pFIDIO->GetEndDepth();
				bExist = TRUE;
			}
			delete pFIDIO;
		}	        
		_findclose( hFile );
	}
	if (bExist)	SetDepth(m_PlotSdep,m_PlotEdep);
}

/////////////////////////////////////////////////////////////////////////////
// CImageManDocserver implementation
//
BOOL CImageManDoc::OnCmdMsg(UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (CCmdTarget::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	if (m_pDocTemplate != NULL && m_pDocTemplate->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	return FALSE;
}

CSize CImageManDoc::ComputerPrintSize()
{
	int h = 0;
	if (m_bPlotTopRem || m_bPlotBottomRem || m_bPlotTrack )
	{
		if (m_bReserveSpace)
			h += m_TopSpace;
		h += m_BottomSpace;
	}
	if (m_bPlotTopRem) h += m_TrackRect.top - m_TopSpace;
	if (m_bPlotTrack) h += m_TrackRect.Height();
	if (m_bPlotBottomRem) h += m_Rect.bottom - m_BottomSpace - m_TrackRect.bottom;
	CSize size(m_Rect.Width(),h);
	if (m_ScaleNo != 0)
	{
		if (m_ScaleFactor > 0)
		{
			size.cx = (int)(size.cx * m_ScaleFactor / 100);
			size.cy = (int)(size.cy * m_ScaleFactor / 100);
		}
		else
		{
			size.cx = (int)(size.cx * 100 / -m_ScaleFactor);
			size.cy = (int)(size.cy * 100 / -m_ScaleFactor);
		}
	}
	return size;
}

void CImageManDoc::ComputePageSize()
{
	CPrintDialog dlg(FALSE);
	if (!AfxGetApp()->GetPrinterDeviceDefaults(&dlg.m_pd)) return;

	// GetPrinterDC returns a HDC so attach it
	CDC dc;
	HDC hDC= dlg.CreatePrinterDC();
	if (hDC==NULL) return;
	dc.Attach(hDC);

	// Get the size of the page in loenglish
	CPoint point;
	point.x = dc.GetDeviceCaps(LOGPIXELSX);
	point.y = dc.GetDeviceCaps(LOGPIXELSY);
	CSize size(MulDiv(dc.GetDeviceCaps(HORZSIZE),1000,254),
		MulDiv(dc.GetDeviceCaps(VERTSIZE),1000,254));

	CSize m_PageSize = size;
	if (m_LeftSpace + m_RightSpace > size.cx)
		size.cx = m_LeftSpace + m_RightSpace;
	m_Rect.right =size.cx -1;
	m_TrackRect.left = m_Rect.left + m_LeftSpace;
	m_TrackRect.right = m_Rect.right - m_RightSpace;
}

double CImageManDoc::YCoorToDepth(int y)
{
	return (double)((y - m_TrackRect.top) * 2.54 * m_DepthScale /10000 + m_PlotSdep);
}

int CImageManDoc::DepthToYCoor(double depth)
{
	return (int) ((depth - m_PlotSdep) * 100 * 100 / 2.54 / m_DepthScale + m_TrackRect.top);
}

void CImageManDoc::GetDepthRangle(double& minDepth,double& maxDepth)
{
	int k = 0;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))))
		{
			if (k == 0)
			{
				minDepth =  ((CTrackObj*) pObj)->m_PlotSdep;
				maxDepth =  ((CTrackObj*) pObj)->m_PlotEdep;
				k = 1;
			}
			if (minDepth > ((CTrackObj*)pObj)->m_PlotSdep) 
				minDepth = ((CTrackObj*)pObj)->m_PlotSdep;
			if (maxDepth < ((CTrackObj*)pObj)->m_PlotEdep) 
				maxDepth = ((CTrackObj*)pObj)->m_PlotEdep;
		}
	}             
	if (k == 0)
	{
		minDepth = m_PlotSdep = 1000;
		maxDepth = YCoorToDepth(m_TrackRect.bottom);
	}
}

void CImageManDoc::DoVertAlign()
{
	if (m_bVertAlignDepth) DoVertAlignDepth();
	if (m_bVertAlignTop) DoVertAlignTop();
	SetSize();
}

void CImageManDoc::DoVertAlignHead()
{
	int y = 20000;
	CPoint point(0,0);
	CTrackObj *pObj1; 
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))))
		{
			int y1 = ((CTrackObj*)pObj)->GetHeadRect().bottom;
			if (y > y1) 
			{
				y = y1;
				pObj1 = (CTrackObj*) pObj;
			}	
		}
	}	
	pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))))
		{
			point.y = y - ((CTrackObj*)pObj)->GetHeadRect().bottom;
			pObj->SetRect(pObj->GetRect()+point);
		}	
	}
}

void CImageManDoc::DoVertAlignDepth()
{
	DoVertAlignHead();
	CPoint point(0,0);
	CTrackObj *pObj1, *pObj2; 
	double minDepth,maxDepth;
	pObj1 = NULL; pObj2 = NULL;    
	minDepth = 10000; maxDepth = 0;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))))
		{
			if (minDepth > ((CTrackObj*)pObj)->m_PlotSdep) 
			{
				pObj1 = (CTrackObj*) pObj;
				minDepth = pObj1->m_PlotSdep;
			}
			if (maxDepth < ((CTrackObj*)pObj)->m_PlotEdep) 
			{
				pObj2 = (CTrackObj*) pObj;
				maxDepth = pObj2->m_PlotEdep;
			}	
		} 
	}
	pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))))
		{
			((CTrackObj*)pObj)->SetDepth(pObj1->m_PlotSdep,pObj2->m_PlotEdep);
		}	
	}
	m_PlotSdep = minDepth;
	m_PlotEdep = maxDepth;
}

void CImageManDoc::DoVertAlignTop()
{
	BOOL bChange = FALSE;
	BOOL bChangeSize = FALSE;
	CPoint point;
	CRect rect = GetTrackUnionRect(); 
	POSITION pos;
	if (m_TrackRect.top != rect.top)
	{
		point.y = m_TrackRect.top - rect.top;
		point.x = 0;
		pos = m_TrackList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*) m_TrackList.GetNext(pos);
			if (pObj != NULL) pObj->SetRect(pObj->GetRect()+ point);
		}
	}                                    
}

void CImageManDoc::OnChildTrackHeadHeightChange(CDrawObj* pObj)
{
	ASSERT(pObj != NULL);
	if (!bEnableChildSizeMessage) return;   
	if (pObj->GetParentType() == 0) 
	{
		AutoChildTrackHeadHeight();
		AlignChildTrackHeadHeight();
	}
}

void CImageManDoc::OnChildHeightChange(CDrawObj* pObj)
{
	ASSERT(pObj != NULL);
	if (!bEnableChildSizeMessage) return;  
	else if (pObj->GetParentType() == 0) 
	{
		if (m_bVertAlignTop) DoVertAlignTop();
		SetSize();
	}
}

void CImageManDoc::OnChildWidthChange(CDrawObj* pObj)
{
	ASSERT(pObj != NULL);
	if (!bEnableChildSizeMessage) return;   
	if ((bHoriAutoArrangle) && (pObj->GetParentType() == 0)) DoHoriArrangle(m_HoriArrangleMode);
}

void CImageManDoc::OnChildPosChange(CDrawObj* pObj)
{
	ASSERT(pObj != NULL);
	if (!bEnableChildSizeMessage) return;  
	else if (pObj->GetParentType() == 0)
	{
		if (bHoriAutoArrangle) DoHoriArrangle(m_HoriArrangleMode);
		DoVertAlign();
	}
}

void CImageManDoc::OnChildRemove(CDrawObj* pObj, int nParentType)
{
	ASSERT(pObj != NULL);
	if (!bEnableChildSizeMessage) return;   
	else if (nParentType == 0)
	{
		if (bHoriAutoArrangle) DoHoriArrangle(m_HoriArrangleMode);
		if (m_bChildTrackHeadAlignMax) AlignChildTrackHeadHeight();
		else 
		{
			if (m_bVertAlignTop) DoVertAlignTop();
			SetSize();
		}
	}
}

CTrackObj* CImageManDoc::GetLeftTrackObj()
{                       
	int xl = 20000;  
	CTrackObj* pObj1 = NULL;
	CDrawObj* pObj;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetRect().left < xl) && (pObj->GetParentType() == 0))
		{
			pObj1 = (CTrackObj*) pObj; 
			xl = pObj->GetRect().left;
		};
	}
	return pObj1;
}

CTrackObj* CImageManDoc::GetRightTrackObj()
{                       
	int xr = 0;  
	CTrackObj* pObj1 = NULL;
	CDrawObj* pObj;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetRect().right > xr) && (pObj->GetParentType() == 0))
		{
			pObj1 = (CTrackObj*)pObj; 
			xr = pObj->GetRect().right;
		};
	}
	return pObj1;
}

CTrackObj* CImageManDoc::GetMaxLeftTrackObj()
{                       
	int xl = 0;  
	CTrackObj* pObj1 = NULL;
	CDrawObj* pObj;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetRect().left > xl) && (pObj->GetParentType() == 0))
		{
			pObj1 = (CTrackObj*) pObj; 
			xl = pObj->GetRect().left;
		};
	}
	return pObj1;
}

CTrackObj* CImageManDoc::GetMinRightTrackObj()
{                       
	int xr = 20000;  
	CTrackObj* pObj1 = NULL;
	CDrawObj* pObj;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetRect().right < xr) && (pObj->GetParentType() == 0))
		{
			pObj1 = (CTrackObj*) pObj; 
			xr = pObj->GetRect().right;
		};
	}
	return pObj1;
}

CTrackObj* CImageManDoc::GetMaxWidthTrackObj()
{                       
	int w = 0;  
	CTrackObj* pObj1 = NULL;
	CDrawObj* pObj;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetRect().Width() > w) && (pObj->GetParentType() == 0))
		{
			pObj1 = (CTrackObj*) pObj; 
			w = pObj->GetRect().Width();
		};
	}
	return pObj1;
}

CTrackObj* CImageManDoc::GetMinWidthTrackObj()
{                       
	int w = 20000;  
	CTrackObj* pObj1 = NULL;
	CDrawObj* pObj;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetRect().Width() < w) && (pObj->GetParentType() == 0))
		{
			pObj1 = (CTrackObj*) pObj; 
			w = pObj->GetRect().Width();
		};
	}
	return pObj1;
}

CTrackObj* CImageManDoc::GetMaxHeightTrackObj()
{                       
	int h = 0;  
	CTrackObj* pObj1 = NULL;
	CDrawObj* pObj;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetParentType() == 0))
		{
			int h1 = ((CTrackObj*)pObj)->GetHeadRect().bottom - pObj->GetRect().top;
			if (h1 > h)
			{
				pObj1 = (CTrackObj*)pObj; 
				h = h1;
			}	
		}
	}
	return pObj1;
}

CTrackObj* CImageManDoc::GetMinHeightTrackObj()
{                       
	int h = 20000;  
	CTrackObj* pObj1 = NULL;
	CDrawObj* pObj;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetParentType() == 0))
		{
			int h1 = ((CTrackObj*)pObj)->GetHeadRect().bottom - pObj->GetRect().top;
			if (h1 < h)
			{
				pObj1 = (CTrackObj*) pObj; 
				h = h1;
			}	
		}
	}
	return pObj1;
}

int CImageManDoc::GetTrackTotalWidth()
{                       
	int w = 0;  
	CDrawObj* pObj;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetParentType() == 0))
			w += pObj->GetRect().Width();
	}
	return w;
}           

int CImageManDoc::GetTrackCount()
{
	int i=0;
	CDrawObj* pObj;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetParentType() == 0)) i++;
	}                                                             
	return i;
}           

void CImageManDoc::DoHoriArrangle(int nArrangleMode)
{
	CRect rect = m_TrackRect;
	if (nArrangleMode == IDM_NONE) return;
	int k = GetTrackCount();
	if (k == 0) return;
	CObList m_List;
	CTrackObj* pObj;       
	int w = GetTrackTotalWidth();
	if (nArrangleMode == IDM_LEFT)
	{
		CRect rect1 = rect;
  	    while((pObj = GetLeftTrackObj()) != NULL) 
		{ 
			m_List.AddTail(pObj);
			RemoveObj(pObj); 
			CRect rect = pObj->GetRect();
			rect1.top = rect.top; rect1.bottom = rect.bottom;         
			rect1.right = rect1.left + rect.Width();
			pObj->SetRect(rect1);
			rect1.left = rect1.right;
		}	
		m_TrackList.AddHead(&m_List);
	}
	else if (nArrangleMode == IDM_RIGHT)
	{
		CRect rect1 = rect;
		while  ((pObj = GetRightTrackObj()) != NULL) 
		{ 
			m_List.AddTail(pObj);
			RemoveObj(pObj);         
			rect1.left = rect1.right - pObj->GetRect().Width();
			rect1.top = pObj->GetRect().top; 
			rect1.bottom = pObj->GetRect().bottom;
			pObj->SetRect(rect1);
			rect1.right = rect1.left;
		}	
		m_TrackList.AddHead(&m_List);
	}   
	else if (nArrangleMode == IDM_JUST)
	{
		CRect rect1 = rect;
		CRect rect2 = rect;
		if ((w > rect.Width()) && (k > 1))
		{
			pObj = GetMaxLeftTrackObj();
			if (pObj!= NULL)
			{                              
				int d =  rect.Width() - pObj->GetRect().Width();
				ASSERT(d>=0);
				int k1,k2;
				if (k == 1)	k1 = k2 = 0; 
				else
				{
					k1 = d / (k-1);
					k2 = d - k1 * (k-1);         
				}
				d = 0;
				while  ((pObj = GetLeftTrackObj()) != NULL) 
				{ 
					m_List.AddTail(pObj);
					RemoveObj(pObj);         
					rect1.right = rect1.left + pObj->GetRect().Width();
					rect1.top = pObj->GetRect().top; 
					rect1.bottom = pObj->GetRect().bottom;
					pObj->SetRect(rect1);
					rect1.left += k1;
					if (k2 > 0)
					{
						rect1.left++;
						k2 --;
					}            
					if (rect1.left > rect1.right)
					{
						k2 += rect1.left - rect1.right;
						rect1.left = rect1.right;
					}
				}	
				m_TrackList.AddHead(&m_List);
			}
			return;
		}		
		int d =  rect.Width() - w;
		int k1,k2;
		if (k == 1)	k1 = k2 = 0; 
		else
		{
			k1 = d / (k-1);
			k2 = d - k1 * (k-1);         
		}
		while  ((pObj = GetLeftTrackObj()) != NULL) 
		{ 
			m_List.AddTail(pObj);
			RemoveObj(pObj);         
			rect1.right = rect1.left + pObj->GetRect().Width();
			rect1.top = pObj->GetRect().top; 
			rect1.bottom = pObj->GetRect().bottom;
			if (rect1.right > rect.right)
			{
			    CRect rect3 = rect1;
			    rect3.left = rect.right - pObj->GetRect().Width();
				rect3.right = rect.right;
				rect1.right = rect.right;
				pObj->SetRect(rect3);
			}
			else pObj->SetRect(rect1);
			rect1.left = rect1.right + k1;
			if (k2 > 0)
			{
				rect1.left++;
				k2 --;
			}            
			if ((pObj = GetRightTrackObj()) != NULL) 
			{ 
				m_List.AddTail(pObj);
				RemoveObj(pObj);         
				rect2.left = rect2.right - pObj->GetRect().Width();
				rect2.top = pObj->GetRect().top; 
				rect2.bottom = pObj->GetRect().bottom;
				if (rect2.left < rect.left)
				{
				    CRect rect3 = rect2;
				    rect3.right = rect.left + pObj->GetRect().Width();
					rect3.left = rect.left;
					rect2.left = rect.left;
					pObj->SetRect(rect3);
				}
				else pObj->SetRect(rect2);
				rect2.right = rect2.left - k1;
				if (k2 > 0)
				{
					rect2.right--;
					k2 --;
				}            
			}	
		}	
		m_TrackList.AddHead(&m_List);
	}
}

CRect CImageManDoc::GetUnionRect()
{
	CRect rect(m_TrackRect.left,m_TrackRect.top,m_TrackRect.right,m_TrackRect.bottom);
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		if (pObj != NULL) 
		{
			if (rect.IsRectEmpty()) rect = pObj->GetMaxRect();
			else rect |= pObj->GetMaxRect();  
		}	
	}
	return rect;
}               

CRect CImageManDoc::GetTrackUnionRect()
{
	CRect rect1;
	CRect rect(m_TrackRect.left,m_TrackRect.top,m_TrackRect.left,m_TrackRect.top);
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))))
		{
			rect1 = pObj->GetRect();
			if (m_DrawFIDHeadMode == 1)
				rect1.bottom = rect1.bottom + ((CTrackObj*)pObj)->GetHeadRect().bottom - rect1.top;
			if (rect.IsRectEmpty()) rect = rect1;
			else rect |= rect1;  
		}	
	}                
	return rect;
}               

int CImageManDoc::GetLeftSpace(CDrawObj* pObj1)
{
	CRect rect = m_Rect;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		if ((pObj != NULL) && (pObj != pObj1))  rect |= pObj->GetMaxRect();
	}
	return rect.left - m_Rect.left;
}

int CImageManDoc::GetRightSpace(CDrawObj* pObj1)
{
	CRect rect = m_Rect;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		if ((pObj != NULL) && (pObj != pObj1))  rect |= pObj->GetMaxRect();
	}
	return m_Rect.right - rect.right;
}

void CImageManDoc::SetChildTrackWidth()
{
	CTrackObj* pObj;
	CRect rect;
	UINT m_MinWidth,m_MaxWidth;
	m_MinWidth = 0;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CTrackObj*)m_TrackList.GetNext(pos);
		UINT w = pObj->GetMinWidth();
		if (m_MinWidth < w) m_MinWidth = w;
	}
	m_MaxWidth = m_TrackRect.Width();
	if (m_MinWidth > m_MaxWidth) m_MinWidth = m_MaxWidth;
	CWidthDialog m_Dialog;
	m_Dialog.m_MinWidth = (int)(m_MinWidth*0.254);
	m_Dialog.m_MaxWidth = (int)(m_MaxWidth*0.254);
	if (m_Dialog.DoModal() == IDOK)
	{
		int w = (int)(m_Dialog.m_Width / 0.254);
		bEnableChildSizeMessage = FALSE;		// 禁止子道消息
		POSITION pos = m_TrackList.GetHeadPosition();
		while (pos != NULL)
		{
			pObj = (CTrackObj*)m_TrackList.GetNext(pos);
			rect = pObj->GetRect();
			rect.right = rect.left + w;
			pObj->SetRect(rect);
		}
		if (bHoriAutoArrangle) DoHoriArrangle();
		pos = GetFirstViewPosition();
		CView* pView = GetNextView(pos);
		UpdateAllViews(pView,HINT_UPDATE_WINDOW);
		bEnableChildSizeMessage = TRUE;			// 允许子道消息	
	}
}

void CImageManDoc::SetChildTrackTitleHeight()
{
	CTrackObj* pObj;
	CHeightDialog m_Dialog;
	m_Dialog.m_MinHeight = 0;
	m_Dialog.m_MaxHeight = 25;
	if (m_Dialog.DoModal() == IDOK)
	{
		int h = (int)(m_Dialog.m_Height / 0.254);
		bEnableChildSizeMessage = FALSE;		// 禁止子道消息
		POSITION pos = m_TrackList.GetHeadPosition();
		while (pos != NULL)
		{
			pObj = (CTrackObj*)m_TrackList.GetNext(pos);
			pObj->SetTitleRectHeight(h);
		}
		if (m_bVertAlignTop) DoVertAlignTop();
		SetSize();
		bEnableChildSizeMessage = TRUE;			// 允许子道消息	
		pos = GetFirstViewPosition();
		CView* pView = GetNextView(pos);
		UpdateAllViews(pView,HINT_UPDATE_WINDOW);
	}
}

void CImageManDoc::DrawChildTrackTitle(BOOL bShow)
{
	bEnableChildSizeMessage = FALSE;		// 禁止子道消息
	CTrackObj* pObj;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CTrackObj*)m_TrackList.GetNext(pos);
		pObj->OnShowTitle(bShow);
	}
	if (m_bVertAlignTop) DoVertAlignTop();
	SetSize();
	bEnableChildSizeMessage = TRUE;			// 允许子道消息	
	pos = GetFirstViewPosition();
	CView* pView = GetNextView(pos);
	UpdateAllViews(pView,HINT_UPDATE_WINDOW);
}

void CImageManDoc::SetChildTrackHeadHeightAlignMax(BOOL bMax)
{
	if (m_bChildTrackHeadAlignMax != bMax)  
	{
		m_bChildTrackHeadAlignMax = bMax;
		if (bMax) 
		{
			AlignChildTrackHeadHeight();
			POSITION pos = GetFirstViewPosition();
			CView* pView = GetNextView(pos);
			UpdateAllViews(pView,HINT_UPDATE_WINDOW);
		}
	}
}

void CImageManDoc::SetChildTrackHeadHeightAuto(BOOL bAuto)
{
	if (m_bChildTrackHeadHeightAuto == bAuto) return;
	m_bChildTrackHeadHeightAuto = bAuto;
	bEnableChildSizeMessage = FALSE;		// 禁止子道消息
	CTrackObj* pObj;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CTrackObj*)m_TrackList.GetNext(pos);
		pObj->SetHeadHeightAuto(bAuto);
	}
	if (bAuto)
	{
		if (m_bChildTrackHeadAlignMax) AlignChildTrackHeadHeight();
		else 
		{
			if (m_bVertAlignTop && bVertAutoAlign) DoVertAlignTop();
			SetSize();
		}
		POSITION pos = GetFirstViewPosition();
		CView* pView = GetNextView(pos);
		UpdateAllViews(pView,HINT_UPDATE_WINDOW);
	}
	bEnableChildSizeMessage = TRUE;	// 允许子道消息	
}

void CImageManDoc::AutoChildTrackHeadHeight()
{
	if (!m_bChildTrackHeadHeightAuto) return;
	bEnableChildSizeMessage = FALSE;		// 禁止子道消息
	CTrackObj* pObj;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CTrackObj*) m_TrackList.GetNext(pos);
		pObj->AutoHeadHeight();
	}
	bEnableChildSizeMessage = TRUE;	// 允许子道消息	
}

void CImageManDoc::AlignChildTrackHeadHeight()
{
	if (!m_bChildTrackHeadAlignMax) return;
	CTrackObj* pObj;
	int nSave = -1;
	POSITION pos = m_TrackList.GetHeadPosition();
	if (pos != NULL)
	{
		pObj = (CTrackObj*) m_TrackList.GetNext(pos);
		nSave = pObj->m_bHeadHeightAuto;
	}
	if (nSave == -1) return;
	else if (nSave)
	{
		// 禁止子道自动调整头高度
		POSITION pos = m_TrackList.GetHeadPosition();
		while (pos != NULL)
		{
			pObj = (CTrackObj*) m_TrackList.GetNext(pos);
			pObj->m_bHeadHeightAuto = FALSE;
		}
	}

	bEnableChildSizeMessage = FALSE;		// 禁止子道消息
	UINT m_MaxHeight = 0;
	pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CTrackObj*)m_TrackList.GetNext(pos);
		UINT h = pObj->GetHeadRect().Height();
		if (m_MaxHeight < h) m_MaxHeight = h;
	}
	pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CTrackObj*)m_TrackList.GetNext(pos);
		pObj->SetHeadRectHeight(m_MaxHeight);
	}
	bEnableChildSizeMessage = TRUE;	// 允许子道消息	
	if (nSave)
	{
		// 允许子道自动调整头高度
		POSITION pos = m_TrackList.GetHeadPosition();
		while (pos != NULL)
		{
			pObj = (CTrackObj*) m_TrackList.GetNext(pos);
			pObj->m_bHeadHeightAuto = TRUE;
		}
	}
	if (m_bVertAlignTop && bVertAutoAlign) DoVertAlignTop();
	SetSize();
}

void CImageManDoc::MoveObj(CPoint pt)
{
	if ((pt.x == 0) && (pt.y == 0)) return;
	POSITION	pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*) m_TrackList.GetNext(pos);
		if (pObj != NULL) pObj->SetRect(pObj->GetRect()+ pt);
	}
}

void CImageManDoc::SetSize()
{
	BOOL bChange = FALSE;
	POSITION pos;
	CPoint point(0,0);
	SelectMode nSelectMode = CDrawObj::m_SelectMode;
	CDrawObj::m_SelectMode = move;
	CRect rect = GetTrackUnionRect(); 
	m_TrackRect.bottom =rect.bottom;
	if (m_TrackRect.top > rect.top)
	{
		point.y = m_TrackRect.top - rect.top;
		point.x = 0;
		pos = m_TrackList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*) m_TrackList.GetNext(pos);
			if (pObj != NULL) pObj->SetRect(pObj->GetRect()+ point);
		}
		MoveObj(point);
		rect += point;
		bChange = TRUE;
		m_TrackRect.bottom += point.x;
	} 
    CDrawObj::m_SelectMode = nSelectMode;
	m_Rect.bottom=rect.bottom+m_BottomSpace;
	UpdateAllViews(NULL,HINT_UPDATE_SIZE);
}

void CImageManDoc::Print(CDC* pDC,CView* pView)
{
	pDC->GetClipBox(m_ClipRect);
	CDrawObj* pObj;
	POSITION pos;
	CRect rect1 = m_ClipRect;
	m_WinOrg.y = 0;
	if (!m_bPlotTopRem) m_WinOrg.y += (m_TrackRect.top - m_TopSpace);
	pDC->OffsetWindowOrg(0,m_WinOrg.y);

//  Print Curve
	pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		pObj->Draw(pView,pDC);
	} 
	pos = m_SinObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_SinObjList.GetNext(pos);
		pObj->Draw(pView,pDC);
	} 
}

void CImageManDoc::Draw(CView* pView,CDC* pDC)
{
	pDC->GetClipBox(m_ClipRect);
	if (pView->IsKindOf(RUNTIME_CLASS(CImageManView)))
	{
		POSITION pos = m_TrackList.GetHeadPosition();
		while (pos != NULL)
		{
			CTrackObj* pObj = (CTrackObj*)m_TrackList.GetNext(pos);
			if (pObj != NULL) 
				pObj->Draw(pView,pDC);
		}  
		pos = m_RemObjList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*)m_RemObjList.GetNext(pos);
			if (pObj != NULL) pObj->Draw(pView,pDC);
		} 
		pos = m_SinObjList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*)m_SinObjList.GetNext(pos);
			if (pObj != NULL) pObj->Draw(pView,pDC);
		}  
	}
}

void CImageManDoc::DeleteObList()
{
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
		delete m_TrackList.GetNext(pos);
	m_TrackList.RemoveAll();
	pos = m_RemObjList.GetHeadPosition();
	while (pos != NULL)
		delete m_RemObjList.GetNext(pos);
	m_RemObjList.RemoveAll();
	pos = m_SinObjList.GetHeadPosition();
	while (pos != NULL)
		delete m_SinObjList.GetNext(pos);
	m_SinObjList.RemoveAll();
}

void CImageManDoc::RemoveObj(CDrawObj* pObj)
{
	ASSERT(pObj != NULL);
	if(pObj->IsKindOf(RUNTIME_CLASS(CStratumObj)))
		m_bHandDrawSin=FALSE;
	if (pObj->GetParentObj() != NULL)  
		pObj->GetParentObj()->RemoveObj(pObj);
	else if (pObj->IsKindOf(RUNTIME_CLASS(CTrackObj)))
	{
		POSITION pos = m_TrackList.Find(pObj,NULL);
		if (pos != NULL) m_TrackList.RemoveAt(pos);
	}
	else if ((pObj->IsKindOf(RUNTIME_CLASS(CLineObj)))
		||(pObj->IsKindOf(RUNTIME_CLASS(CDepSect)))
		||(pObj->IsKindOf(RUNTIME_CLASS(CFoldObj))))
	{
		POSITION pos = m_RemObjList.Find(pObj,NULL);
		if (pos != NULL) m_RemObjList.RemoveAt(pos);
	}
	else 
	{
		POSITION pos = m_SinObjList.Find(pObj,NULL);
		if (pos != NULL) 
		{
			m_SinObjList.RemoveAt(pos);
			m_HWOprateNum++;
		}
	}
}

void CImageManDoc::AddObj(CDrawObj* pObj)
{
	ASSERT(pObj != NULL);
	if(pObj->IsKindOf(RUNTIME_CLASS(CStratumObj)))
		m_bHandDrawSin=TRUE;
	if (pObj->IsKindOf(RUNTIME_CLASS(CTrackObj)))
	{
		m_TrackList.AddTail(pObj);
		pObj->SetParentObj(NULL,0);
		if (CDrawObj::c_ToolShape == none)
		{
			bEnableChildSizeMessage = FALSE;
			((CTrackObj*)pObj)->SetHeadHeightAuto(FALSE);
			((CTrackObj*)pObj)->SetHeadHeightAuto(m_bChildTrackHeadHeightAuto);
			AlignChildTrackHeadHeight();
			bEnableChildSizeMessage = TRUE;
			if (bHoriAutoArrangle) DoHoriArrangle(m_HoriArrangleMode);
			if (bVertAutoAlign) DoVertAlign();
			else SetSize();
		}	
	}
	else if (pObj->IsKindOf(RUNTIME_CLASS(CLineObj)))
	{
		m_RemObjList.AddTail(pObj);
		pObj->SetParentObj(NULL,1);
		SetSize();
	}
	else if (pObj->IsKindOf(RUNTIME_CLASS(CFoldObj)))
	{
		CFoldObj* pFoldObj=GetFoldObject();
		if(pFoldObj!=NULL) RemoveObj(pFoldObj);
		m_RemObjList.AddTail(pObj);
		pObj->SetParentObj(NULL,1);
		SetSize();
	}
	else if (pObj->IsKindOf(RUNTIME_CLASS(CDepSect)))
	{
		CDepSect* pDepSect=GetLineObject();
		if(pDepSect!=NULL) RemoveObj(pDepSect);
		m_RemObjList.AddTail(pObj);
		pObj->SetParentObj(NULL,1);
		SetSize();
	}
	else 
	{
		m_SinObjList.AddTail(pObj);
		pObj->SetParentObj(NULL,1);
		SetSize();
	}
}

CDrawObj* CImageManDoc::GetLocateObject(CDrawObj* pObj2)
{
	ASSERT(pObj2 != NULL);
	CDrawObj *pObj1,*pObj;
	POSITION pos = m_TrackList.GetTailPosition();
	while (pos != NULL)
	{
		pObj = (CTrackObj*)m_TrackList.GetPrev(pos);
		if ((pObj1 = pObj->GetLocateObject(pObj2)) != NULL) 
			return pObj1;
	}    
	pos = m_RemObjList.GetTailPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_RemObjList.GetPrev(pos);
		if ((pObj1 = pObj->GetLocateObject(pObj2)) != NULL) 
			return pObj1;
	}
	pos = m_SinObjList.GetTailPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_SinObjList.GetPrev(pos);
		if ((pObj1 = pObj->GetLocateObject(pObj2)) != NULL) 
			return pObj1;
	} 
	return NULL;
}

BOOL CImageManDoc::OnSetCursor(CView* pView,const CPoint& point)
{
	if (pView->IsKindOf(RUNTIME_CLASS(CImageManView)))
	{
		POSITION pos = m_TrackList.GetTailPosition();
		while (pos != NULL)
		{
			CTrackObj* pTrackObj = (CTrackObj*)m_TrackList.GetPrev(pos);
			if (pTrackObj->OnSetCursor(pView,point)) return TRUE;
		}  
		pos = m_RemObjList.GetTailPosition();
		while (pos != NULL)
		{
			CDrawObj* pDrawObj = (CDrawObj*)m_RemObjList.GetPrev(pos);
			if (pDrawObj->OnSetCursor(pView,point)) return TRUE;
		} 	
		pos = m_SinObjList.GetTailPosition();
		while (pos != NULL)
		{
			CDrawObj* pDrawObj = (CDrawObj*)m_SinObjList.GetPrev(pos);
			if (pDrawObj->OnSetCursor(pView,point)) return TRUE;
		} 
	}
	return FALSE;
}

CDrawObj* CImageManDoc::ObjectAt(CView* pView,const CPoint& point)
{
	ASSERT(pView != NULL);
	CDrawObj* pObj;
	if (pView->IsKindOf(RUNTIME_CLASS(CImageManView)))
	{
		POSITION pos = m_TrackList.GetHeadPosition();
		while (pos != NULL)
		{
			CTrackObj* pTrackObj = (CTrackObj*)m_TrackList.GetNext(pos);
			if ((pObj = pTrackObj->ObjectAt(pView,point)) != NULL) return pObj;
		} 
		pos = m_RemObjList.GetTailPosition();
		while (pos != NULL)
		{
			CDrawObj* pDrawObj = (CDrawObj*)m_RemObjList.GetPrev(pos);
			if ((pObj = pDrawObj->ObjectAt(pView,point)) != NULL) return pObj;
		}
		pos = m_SinObjList.GetTailPosition();
		while (pos != NULL)
		{
			CDrawObj* pDrawObj = (CDrawObj*)m_SinObjList.GetPrev(pos);
			if ((pObj = pDrawObj->ObjectAt(pView,point)) != NULL) return pObj;
		} 
	}
	return(NULL);
}

void CImageManDoc::SetDepth(double sdep, double edep)
{
	m_PlotSdep = sdep; m_PlotEdep = edep;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		CTrackObj* pObj = (CTrackObj*)m_TrackList.GetNext(pos);
		pObj->SetDepth(m_PlotSdep,m_PlotEdep);
	} 
	if (bVertAutoAlign) DoVertAlign();
	else 				SetSize();
	m_ProSdep=m_PlotSdep;
	m_ProEdep=m_PlotEdep;
}   

void CImageManDoc::SetDepC(double depc, int flag)
{
	m_DepC = depc;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		CTrackObj* pTrackObj = (CTrackObj*)m_TrackList.GetNext(pos);
		pTrackObj->OnChangeDepC(depc, flag);
	}         
	if ((m_bVertAlignDepth) || (m_bVertAlignTop)) 
	{
		CPoint point;
		CRect rect = GetTrackUnionRect(); 
		if (m_TrackRect.top != rect.top)
		{
			point.y = m_TrackRect.top - rect.top;
			point.x = 0;
			pos = m_TrackList.GetHeadPosition();
			while (pos != NULL)
			{
				CDrawObj* pObj = (CDrawObj*) m_TrackList.GetNext(pos);
				if (pObj != NULL) pObj->SetRect(pObj->GetRect()+ point);
			}
		}
	}                                    
	SetSize();
	UpdateAllViews(NULL,HINT_UPDATE_SIZE);
}

void CImageManDoc::SetDepthScale(double scale) 
{
	double depc;
	m_DepthScale = scale;
	if (m_DepthUnit == UNIT_M)
		depc = scale * 2.54 / 10000.;
	else
		depc = scale / 1200.;
	SetDepC(depc,0);
}

void CImageManDoc::OnSetDepth() 
{
	CPlotDepthDialog m_dialog(NULL);
	double sdep,edep;
	GetDepthRangle(sdep,edep);
	m_dialog.SetDepth(sdep,edep);
	if(m_dialog.DoModal() == IDOK)
	{
		m_dialog.GetDepth(sdep,edep);
		SaveHandMark();
		DelSinObjList();
		SetDepth(sdep,edep);
		CTrackObj* pObj;
		POSITION pos = m_TrackList.GetHeadPosition();
		while (pos != NULL)
		{
			pObj = (CTrackObj*)m_TrackList.GetNext(pos);
			if(pObj!=NULL)
			{
				if(pObj->bDrawDepth &&(pObj->m_LayerFileName !=""))
					pObj->m_LayerFlush =TRUE;
			}
		}
		CStratumObj* pStObj=GetStratumObject();
		if(pStObj!=NULL)
			pStObj->CreatePoint ();
	 		
		pos = GetFirstViewPosition();
		CView* pView = GetNextView(pos);
		UpdateAllViews(pView,HINT_UPDATE_WINDOW);
	}
}

void CImageManDoc::OnUpdateSetDepth(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( m_bVertAlignDepth && 
		(GetTrackCount()>0) && (m_DocumentType != TEMPLATE_DOC));//&& bVertAutoAlign
}

void CImageManDoc::OnSetDepthscale() 
{
	CDepthScaleDialog m_Dialog;
	m_Dialog.m_DepthScale = m_DepthScale;
	if(m_Dialog.DoModal() == IDOK)
	{
		SetDepthScale(m_Dialog.m_DepthScale);
		OnLayerFlush();
	}
}

void CImageManDoc::OnViewPapercolor()
{
	// TODO: Add your command handler code here
	CColorDialog dlg;
	if (dlg.DoModal() != IDOK)
		return;
	m_paperColor = dlg.GetColor();
	UpdateAllViews(NULL);
}

int CImageManDoc::MoveBack(CDrawObj* pObj)
{
	ASSERT(pObj != NULL);
	POSITION pos1; 
	POSITION pos = m_TrackList.Find(pObj,NULL);
	if (pos != NULL) 
	{
		pos1 = pos;
		m_TrackList.GetNext(pos1);
		if (pos1 != NULL)
		{
			m_TrackList.InsertAfter(pos1,pObj);
			m_TrackList.RemoveAt(pos);
			return 0;
		}
		return 1;
	}
	pos = m_RemObjList.Find(pObj,NULL);
	if (pos != NULL) 
	{
		pos1 = pos;
		m_RemObjList.GetNext(pos1);
		if (pos1 != NULL)
		{
			m_RemObjList.InsertAfter(pos1,pObj);
			m_RemObjList.RemoveAt(pos);
			return 0;
		}
		return 1;
	}
	pos = m_SinObjList.Find(pObj,NULL);
	if (pos != NULL) 
	{
		pos1 = pos;
		m_SinObjList.GetNext(pos1);
		if (pos1 != NULL)
		{
			m_SinObjList.InsertAfter(pos1,pObj);
			m_SinObjList.RemoveAt(pos);
			return 0;
		}
		return 1;
	}
	return 1;
}

int CImageManDoc::MoveForward(CDrawObj* pObj)
{
	ASSERT(pObj != NULL);
	POSITION pos1; 
	POSITION pos = m_TrackList.Find(pObj,NULL);
	if (pos != NULL) 
	{
		pos1 = pos;
		m_TrackList.GetPrev(pos1);
		if (pos1 != NULL)
		{
			m_TrackList.InsertBefore(pos1,pObj);
			m_TrackList.RemoveAt(pos);
			return 0;
		}
		return 1;
	}
	pos = m_RemObjList.Find(pObj,NULL);
	if (pos != NULL) 
	{
		pos1 = pos;
		m_RemObjList.GetPrev(pos1);
		if (pos1 != NULL)
		{
			m_RemObjList.InsertBefore(pos1,pObj);
			m_RemObjList.RemoveAt(pos);
			return 0;
		}
		return 1;
	}
	pos = m_SinObjList.Find(pObj,NULL);
	if (pos != NULL) 
	{
		pos1 = pos;
		m_SinObjList.GetPrev(pos1);
		if (pos1 != NULL)
		{
			m_SinObjList.InsertBefore(pos1,pObj);
			m_SinObjList.RemoveAt(pos);
			return 0;
		}
		return 1;
	}
	return 1;
}

int CImageManDoc::MoveToTail(CDrawObj* pObj)
{
	ASSERT(pObj != NULL);
	POSITION pos1; 
	POSITION pos = m_TrackList.Find(pObj,NULL);
	if (pos != NULL) 
	{
		pos1 = pos;
		m_TrackList.GetNext(pos1);
		if (pos1 != NULL)
		{
			m_TrackList.RemoveAt(pos);
			return 0;
		}
		return 1;
	}
	pos = m_RemObjList.Find(pObj,NULL);
	if (pos != NULL) 
	{
		pos1 = pos;
		m_RemObjList.GetNext(pos1);
		if (pos1 != NULL)
		{
			m_RemObjList.RemoveAt(pos);
			return 0;
		}
		return 1;
	}
	pos = m_SinObjList.Find(pObj,NULL);
	if (pos != NULL) 
	{
		pos1 = pos;
		m_SinObjList.GetNext(pos1);
		if (pos1 != NULL)
		{
			m_SinObjList.RemoveAt(pos);
			return 0;
		}
		return 1;
	}
	return 1;
}

int CImageManDoc::MoveToHead(CDrawObj* pObj)
{
	POSITION pos1; 
	POSITION pos = m_TrackList.Find(pObj,NULL);
	if (pos != NULL) 
	{
		pos1 = pos;
		m_TrackList.GetPrev(pos1);
		if (pos1 != NULL)
		{
			m_TrackList.RemoveAt(pos);
			m_TrackList.AddHead(pObj);
			return 0;
		}
		return 1;
	}
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
// CImageManDocdiagnostics

#ifdef _DEBUG
void CImageManDoc::AssertValid() const
{
	COleDocument::AssertValid();
}

void CImageManDoc::Dump(CDumpContext& dc) const
{
	COleDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CImageManDoccommands

void CImageManDoc::DropFiles(HDROP hDropInfo, CView* pView)
{
}

void CImageManDoc::DPtoLP(CDC* pDC,LPPOINT lpPoints, int nCount)
{
	pDC->DPtoLP(lpPoints,nCount);
	PPtoLP(lpPoints,nCount);
}

void CImageManDoc::DPtoLP(CDC* pDC,LPRECT lpRect)
{
	pDC->DPtoLP(lpRect);
	PPtoLP(lpRect);
}

void CImageManDoc::LPtoDP(CDC* pDC,LPPOINT lpPoints, int nCount)
{
	LPtoPP(lpPoints,nCount);
	pDC->LPtoDP(lpPoints,nCount);
}

void CImageManDoc::LPtoDP(CDC* pDC,LPRECT lpRect)
{
	LPtoPP(lpRect);
	pDC->LPtoDP(lpRect);
}

void CImageManDoc::PPtoLP(LPPOINT lpPoints, int nCount)
{
	if (m_WinOrg == CPoint(0,0)) return;
	for (int i=0; i<nCount; i++)
		lpPoints[i].y += m_WinOrg.y;
}

void CImageManDoc::PPtoLP(LPRECT lpRect)
{
	if (m_WinOrg == CPoint(0,0)) return;
	lpRect->top += m_WinOrg.y;
	lpRect->bottom += m_WinOrg.y;
}

void CImageManDoc::LPtoPP(LPPOINT lpPoints, int nCount)
{
	if (m_WinOrg != CPoint(0,0))
	{
		for (int i=0; i<nCount; i++)
			lpPoints[i].y -= m_WinOrg.y;
	}
}

void CImageManDoc::LPtoPP(LPRECT lpRect)
{
	BOOL bEQU = (lpRect->top == lpRect->bottom);
	if (m_WinOrg != CPoint(0,0)) 
	{
		lpRect->top -= m_WinOrg.y;
		lpRect->bottom -= m_WinOrg.y;
	}
	if ((lpRect->top == lpRect->bottom) && (!bEQU)) lpRect->bottom += 5;
}

/////////////////////////////////////////////////////////////////////////////
// CViewGridDialog dialog


CViewGridDialog::CViewGridDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CViewGridDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViewGridDialog)
	m_xGrid = 0;
	m_yGrid = 0;
	//}}AFX_DATA_INIT
}


void CViewGridDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewGridDialog)
	DDX_Text(pDX, IDC_XGRID, m_xGrid);
	DDV_MinMaxUInt(pDX, m_xGrid, 2, 100);
	DDX_Text(pDX, IDC_YGRID, m_yGrid);
	DDV_MinMaxUInt(pDX, m_yGrid, 2, 100);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CViewGridDialog, CDialog)
	//{{AFX_MSG_MAP(CViewGridDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewGridDialog message handlers

void CImageManDoc::OnSetupPage() 
{
	CPageSetup m_Page(this);
	if (m_Page.DoModal() == IDOK)
	{
		CPoint pt;
		pt.x = m_Page.m_LeftSpace - m_LeftSpace; 
		pt.y = m_Page.m_TopSpace - m_TopSpace; 
		SelectMode nSelectMode = CDrawObj::m_SelectMode;
		CDrawObj::m_SelectMode = move;
		
		MoveObj(pt);
		m_TrackRect += pt;
		m_Rect.right = m_Page.m_Width + m_Rect.left;
		m_Rect.bottom += m_Page.m_TopSpace - m_TopSpace + m_Page.m_BottomSpace - m_BottomSpace;

		m_LeftSpace = m_Page.m_LeftSpace;
		m_RightSpace = m_Page.m_RightSpace;
		m_TopSpace = m_Page.m_TopSpace;
		m_BottomSpace = m_Page.m_BottomSpace;

		m_TrackRect.left = m_Rect.left + m_LeftSpace;
		m_TrackRect.right = m_Rect.right - m_RightSpace;
		CDrawObj::m_SelectMode = nSelectMode;
		UpdateAllViews(NULL,HINT_UPDATE_SIZE);
	}
}

BOOL CImageManDoc::OnToolTip(CView* pView,CRect& rect,CString& text)
{
	POSITION pos = m_TrackList.GetTailPosition();
	while (pos != NULL)
	{
		CTrackObj* pTrackObj = (CTrackObj*)m_TrackList.GetPrev(pos);
		if (pTrackObj->OnToolTip(pView,rect,text)) return TRUE;
	}      
	pos = m_RemObjList.GetTailPosition();
	while (pos != NULL)
	{
		CDrawObj* pDrawObj = (CDrawObj*)m_RemObjList.GetPrev(pos);
		if (pDrawObj->OnToolTip(pView,rect,text)) return TRUE;
	}  
	pos = m_SinObjList.GetTailPosition();
	while (pos != NULL)
	{
		CDrawObj* pDrawObj = (CDrawObj*)m_SinObjList.GetPrev(pos);
		if (pDrawObj->OnToolTip(pView,rect,text)) return TRUE;
	}   
	return FALSE;
}

void CImageManDoc::OnWellDataUpdate(CString strName)
{
	CObList m_List;
	CDrawObj* pObj;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		pObj->FindWell(&m_List,strName);
	}      
	pos = m_RemObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_RemObjList.GetNext(pos);
		pObj->FindWell(&m_List,strName);
	} 
	pos = m_SinObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_SinObjList.GetNext(pos);
		pObj->FindWell(&m_List,strName);
	} 
	if (m_List.GetCount()>0)
	{
		pos = m_List.GetHeadPosition();
		while (pos != NULL)
		{
			pObj = (CDrawObj*)m_List.GetNext(pos);
			((CFIDDrawObj*)pObj)->m_bFlush = TRUE;
			((CFIDDrawObj*)pObj)->m_bErrorFID = FALSE;
		}                   
		pos = GetFirstViewPosition();
		CView* pView = GetNextView(pos);
		UpdateAllViews(pView,HINT_UPDATE_WINDOW);
	}
	m_List.RemoveAll();
}

void CImageManDoc::OnCurveDataUpdate(CString strName)
{
	if (m_DocumentType == TEMPLATE_DOC) return;
	CObList m_List;
	CDrawObj* pObj;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		pObj->FindCurve(&m_List,strName);
	}   
	pos = m_RemObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_RemObjList.GetNext(pos);
		pObj->FindCurve(&m_List,strName);
	} 
	pos = m_SinObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_SinObjList.GetNext(pos);
		pObj->FindCurve(&m_List,strName);
	} 
	if (m_List.GetCount()>0)
	{
		pos = GetFirstViewPosition();
		GetNextView(pos);
		if (pos == NULL) return;
		CView* pView = GetNextView(pos);
		pos = m_List.GetHeadPosition();
		while (pos != NULL)
		{
			pObj = (CDrawObj*)m_List.GetNext(pos);
			((CFIDDrawObj*)pObj)->m_bFlush = TRUE;
			((CFIDDrawObj*)pObj)->m_bErrorFID = FALSE;
			pObj->Invalidate(pView);
		}                   
	}
	m_List.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
// CDocHoriArrangleDlg dialog


CDocHoriArrangleDlg::CDocHoriArrangleDlg(CImageManDoc* pDocument,CWnd* pParent)
	: CDialog(CDocHoriArrangleDlg::IDD, pParent)
{
	pDoc = pDocument;
	//{{AFX_DATA_INIT(CDocHoriArrangleDlg)
	m_HoriAlignMode = -1;
	//}}AFX_DATA_INIT
}

void CDocHoriArrangleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDocHoriArrangleDlg)
		DDX_Radio(pDX, IDC_LEFT, m_HoriAlignMode);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDocHoriArrangleDlg, CDialog)
	//{{AFX_MSG_MAP(CDocHoriArrangleDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocHoriArrangleDlg message handlers

BOOL CDocHoriArrangleDlg::OnInitDialog() 
{
	if (pDoc->m_HoriArrangleMode == IDM_LEFT) m_HoriAlignMode = 0;
	else if (pDoc->m_HoriArrangleMode == IDM_RIGHT) m_HoriAlignMode = 1;
	else m_HoriAlignMode = 2;
	return CDialog::OnInitDialog();
}

void CDocHoriArrangleDlg::OnOK() 
{
	CDialog::OnOK();
	if (m_HoriAlignMode == 0) pDoc->m_HoriArrangleMode = IDM_LEFT;
	else if (m_HoriAlignMode == 1) pDoc->m_HoriArrangleMode = IDM_RIGHT;
	else pDoc->m_HoriArrangleMode = IDM_JUST;
}

/////////////////////////////////////////////////////////////////////////////
// CDocVertAlignDlg dialog

CDocVertAlignDlg::CDocVertAlignDlg(CImageManDoc* pDocument,CWnd* pParent)
	: CDialog(CDocVertAlignDlg::IDD, pParent)
{
	pDoc = pDocument;
	//{{AFX_DATA_INIT(CDocVertAlignDlg)
	m_AlignTop = FALSE;
	m_AlignDepth = FALSE;
	//}}AFX_DATA_INIT
}

void CDocVertAlignDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDocVertAlignDlg)
	DDX_Check(pDX, IDC_ALIGN_TOP, m_AlignTop);
	DDX_Check(pDX, IDD_ALIGN_DEPTH, m_AlignDepth);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDocVertAlignDlg, CDialog)
	//{{AFX_MSG_MAP(CDocVertAlignDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocVertAlignDlg message handlers

void CDocVertAlignDlg::OnOK() 
{
	CDialog::OnOK();
	pDoc->m_bVertAlignDepth = m_AlignDepth;
	pDoc->m_bVertAlignTop = m_AlignTop;
}

BOOL CDocVertAlignDlg::OnInitDialog() 
{
	m_AlignDepth = pDoc->m_bVertAlignDepth;
	m_AlignTop = pDoc->m_bVertAlignTop;
	return CDialog::OnInitDialog();
}

void CImageManDoc::OnDepthUnitFeet() 
{
	if (m_DepthUnit == UNIT_FEET) return;
	m_DepthUnit = UNIT_FEET;
	double depc = m_DepthScale / 1200.;
	SetDepC(depc,1);
}

void CImageManDoc::OnUpdateDepthUnitFeet(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(m_DepthUnit == UNIT_FEET);
}

void CImageManDoc::OnDepthUnitM() 
{
	if (m_DepthUnit == UNIT_M) return;
	m_DepthUnit = UNIT_M;
	double depc = m_DepthScale * 2.54 / 10000.;
	SetDepC(depc,1);
}

void CImageManDoc::OnUpdateDepthUnitM(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(m_DepthUnit == UNIT_M);
}

void CImageManDoc::OnScaleChange() 
{
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	if(pMain != NULL) 
	{
		m_ScaleNo = pMain->m_ScaleNo;
		m_ScaleFactor = pMain->GetScaleFactor(m_ScaleNo);
	}
	else
	{
		m_ScaleNo = 7;
		m_ScaleFactor = 100;
	}
	UpdateAllViews(NULL,HINT_UPDATE_SIZE);
}

void CImageManDoc::OnPlotFIDhead() 
{
	if (m_DrawFIDHeadMode == 0) 
		m_DrawFIDHeadMode = 1;
	else
		m_DrawFIDHeadMode = 0;
	SetSize();
}

void CImageManDoc::OnUpdatePlotFIDhead(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_DrawFIDHeadMode == 1);
}

void CImageManDoc::OnFilePrintSetup() 
{
//	CMyPrintDlg dlg(this);
//	dlg.DoModal();
}

void CImageManDoc::OnUpdateWell()
{
	char path[512];
	strcpy(path,".FID");

	CFileDialog dlgFile(TRUE);
	dlgFile.m_ofn.Flags |= OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	CString strFilter = "Flags File(*.FID)";
	strFilter += (TCHAR)'\0';   
	strFilter += _T("*.FID");
	strFilter += (TCHAR)'\0';   
	dlgFile.m_ofn.nMaxCustFilter++;
	strFilter += "Flags File(*.FID)";
	strFilter += (TCHAR)'\0';   
	strFilter += _T("*.FID");
	strFilter += (TCHAR)'\0';   
	dlgFile.m_ofn.nMaxCustFilter++;
	strFilter += _T("All File(*.*)");
	strFilter += (TCHAR)'\0';   
	strFilter += _T("*.*");
	strFilter += (TCHAR)'\0';   
	dlgFile.m_ofn.nMaxCustFilter++;

	dlgFile.m_ofn.lpstrFilter = strFilter;
	dlgFile.m_ofn.lpstrTitle = "选择井名";
	dlgFile.m_ofn.lpstrFile = path;
	
	dlgFile.m_ofn.lpstrInitialDir=GetCurrentUserPath();

	if(dlgFile.DoModal() == IDOK)
		OpenWell(path);
}

BOOL CImageManDoc::MatchWellName(CString strName, CString NewstrPathName)
{
	BOOL FindMatch=FALSE;
	CTrackObj* pObj;
	POSITION pos=GetFirstViewPosition();
	CView* pView= GetNextView(pos); 
	pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CTrackObj*)m_TrackList.GetNext(pos);
		if(pObj->IsKindOf(RUNTIME_CLASS(CTrackObj)))
		{
			POSITION pos0;
			pos0 = pObj->m_ObjList.GetHeadPosition();
			CTrackObj* pObj0;
			while (pos0 != NULL)
			{
				pObj0 = (CTrackObj*)pObj->m_ObjList.GetNext(pos0);				
				if(pObj0->IsKindOf(RUNTIME_CLASS(CTrackObj)))
				{
					POSITION pos1;
					pos1 = pObj0->m_ObjList.GetHeadPosition();
					CTrackObj* pObj1;
					while (pos1 != NULL)
					{
						pObj1 = (CTrackObj*)pObj0->m_ObjList.GetNext(pos1);
						if(pObj1->IsKindOf(RUNTIME_CLASS(CTrackObj)))
						{
							POSITION pos2;
							pos2 = pObj1->m_ObjList.GetHeadPosition();
							CTrackObj* pObj2;
							while (pos2 != NULL)
							{
								pObj2 = (CTrackObj*)pObj1->m_ObjList.GetNext(pos2);
								if(pObj2->IsKindOf(RUNTIME_CLASS(CTrackObj)))
								{
									POSITION pos3;
									pos3 = pObj2->m_ObjList.GetHeadPosition();
									CTrackObj* pObj3;
									while (pos3 != NULL)
									{	
										pObj3 = (CTrackObj*)pObj2->m_ObjList.GetNext(pos3);
										if(pObj3->MatchWellName(strName,NewstrPathName))
											FindMatch=TRUE;
									} 
								}
								if(pObj2->MatchWellName(strName,NewstrPathName))
									FindMatch=TRUE;
							}
						}
						if(pObj1->MatchWellName(strName,NewstrPathName))
							FindMatch=TRUE;
					}
				}
				if(pObj0->MatchWellName(strName,NewstrPathName))
					FindMatch=TRUE;
			}
		}
		if(pObj->MatchWellName(strName,NewstrPathName))
			FindMatch=TRUE;
	}
	return FindMatch;
}

void CImageManDoc::MatchListFile(CString WellName)
{
	//得到该口井的路径
	CString szDir;
	CFIDIO::GetPath(WellName,szDir.GetBuffer(512));
	szDir.ReleaseBuffer();

	CTrackObj* pObj;
	CFIDDrawObj* pCurve;
	POSITION pos;
	CString Name;int FilePos;
	pos = GetFirstViewPosition();
	CView* pView= GetNextView(pos); 
	pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CTrackObj*)m_TrackList.GetNext(pos);
		if(pObj!=NULL)
		{
			if(pObj->IsKindOf(RUNTIME_CLASS(CTrackObj)))
			{
				POSITION pos0;
				pos0 = pObj->m_ObjList.GetHeadPosition();
				CTrackObj* pObj0;
				while (pos0 != NULL)
				{
					pObj0 = (CTrackObj*)pObj->m_ObjList.GetNext(pos0);
					if(pObj0!=NULL)
					{
						POSITION pos1;
						pos1 = pObj0->m_ObjList.GetHeadPosition();
						while(pos1!=NULL)
						{
							pCurve=(CFIDDrawObj*)pObj0->m_ObjList.GetNext (pos1);
							if(pCurve!=NULL)
							{
								if((pCurve->IsKindOf (RUNTIME_CLASS(CStratumObj)))
									||(pCurve->IsKindOf (RUNTIME_CLASS(CMarkVectorObj)))
									||(pCurve->IsKindOf (RUNTIME_CLASS(CResultObj)))
									||(pCurve->IsKindOf (RUNTIME_CLASS(CMarkFreqObj))))
								{
									Name=pCurve->m_strPathName;
									FilePos=Name.ReverseFind ('\\');
									if(FilePos!=-1)
										Name=szDir+Name.Mid (FilePos+1);
									else
										Name=szDir+Name;
									pCurve->m_strPathName=Name;
									pCurve->m_bFlush = TRUE;
									pCurve->m_bErrorFID = FALSE;
								}
							}
						}
					}
				}
			}
			POSITION pos2;
			pos2 = pObj->m_ObjList.GetHeadPosition();
			while(pos2!=NULL)
			{
				pCurve=(CFIDDrawObj*)pObj->m_ObjList.GetNext (pos2);
				if(pCurve!=NULL)
				{
					if((pCurve->IsKindOf (RUNTIME_CLASS(CStratumObj)))
						||(pCurve->IsKindOf (RUNTIME_CLASS(CMarkVectorObj)))
						||(pCurve->IsKindOf (RUNTIME_CLASS(CResultObj)))
						||(pCurve->IsKindOf (RUNTIME_CLASS(CMarkFreqObj))))
					{
						Name=pCurve->m_strPathName;
						FilePos=Name.ReverseFind ('\\');
						if(FilePos!=-1)
							Name=szDir+Name.Mid (FilePos+1);
						else
							Name=szDir+Name;
						pCurve->m_strPathName=Name;
						pCurve->m_bFlush = TRUE;
						pCurve->m_bErrorFID = FALSE;
					}
				}
			}
			Name=pObj->m_LayerFileName ;
			if(Name!="")
			{
				FilePos=Name.ReverseFind ('\\');
				if(FilePos!=-1)
					Name=szDir+Name.Mid (FilePos);
				else
					Name=szDir+Name;
				pObj->m_LayerFileName =Name;
				pObj->m_LayerFlush =TRUE;
			}
			if(pObj->m_LayerFileName !="")
			{
				Name=pObj->m_LayerFileName;
				FilePos=Name.ReverseFind ('\\');
				if(FilePos!=-1)
					Name=szDir+Name.Mid (FilePos+1);
				else
					Name=szDir+Name;
				pObj->m_LayerFileName=Name;
				pObj->m_LayerFlush =TRUE;
			}
		}
	}
}

void CImageManDoc::OnDocumentUpdate()
{
	CDrawObj* pObj;
	POSITION pos;
	pos = GetFirstViewPosition();
	CView* pView = GetNextView(pos);
	pos = m_TrackList.GetHeadPosition();
	CClientDC dc(pView);
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		if(pObj!=NULL)
			pObj->Invalidate (pView);
	}  
	pos = m_RemObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_RemObjList.GetNext(pos);
		if(pObj!=NULL)
			pObj->Invalidate (pView);
	} 
	pos = m_SinObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_SinObjList.GetNext(pos);
		if(pObj!=NULL)
			pObj->Invalidate (pView);
	}
	UpdateAllViews(pView,HINT_UPDATE_WINDOW);
}

void CImageManDoc::OpenWell(CString OpenPath)
{
	char path[512];
	strcpy(path,OpenPath);
	CString m_SourceFileName;
	CFIDIO::GetFullWellName(path,m_SourceFileName.GetBuffer(512));
	m_SourceFileName.ReleaseBuffer();
	m_WellName=OpenPath.Left (OpenPath.ReverseFind ('\\'));
	m_WellName=m_WellName.Mid(m_WellName.ReverseFind ('#')+1);
	if(m_WellName.Find(".")>-1)
		m_WellName=m_WellName.Left (m_WellName.Find("."));

	//m_SourceFileName 文档类数据成员,保存通用文件对话选择的文件
	//得到包括路径的井名FullWellName
	CString FullWellName;
	CFIDIO::GetFullWellName(m_SourceFileName,FullWellName.GetBuffer(512));
	FullWellName.ReleaseBuffer();

	//得到该口井的路径
	CString szDir;
	CFIDIO::GetPath(FullWellName,szDir.GetBuffer(512));
	szDir.ReleaseBuffer();
	if(m_UnDo&&m_OldWellName!=_T(""))
	{	
		Beep(400,50);m_UnDo=FALSE;
		if(AfxMessageBox("保存数据源的数据\n"+m_OldWellName+"    ",MB_YESNO|MB_ICONQUESTION)==IDNO)
			CopyAllFile(m_DataBuf,m_OldWellName);
	}	
	if(!CopyAllFile(szDir,m_DataBuf))
	{
		AfxMessageBox("数据备份失败        ");
		return;
	}
	m_OldWellName=szDir;

	//得到将要寻找的索引文件IWellName.*
	char szFind[512];
	CFIDIO::GetFileName(FullWellName,szFind,'I');
	strcat(szFind,".*");
	char szFIDName[512];
	struct _finddata_t c_file;    //Microsoft C++ 的文件结构
	long hFile;
	BeginWaitCursor();
	MatchListFile(FullWellName);

	float dep1,dep2,Sdep=0.f,Edep=9999.f;
	if((hFile = _findfirst(szFind, &c_file )) != -1L)
	{ 
		CFIDIO::GetFIDName(c_file.name,szFIDName);	
		MatchWellName(szFIDName, szDir + c_file.name);

		CFIDIO* pFIDIO = new CFIDIO(szDir + c_file.name); 		
		if (pFIDIO->Open())
		{	
			Sdep = pFIDIO->GetStartDepth();
			Edep = pFIDIO->GetEndDepth();
		}
		delete pFIDIO;
		while(_findnext(hFile,&c_file) == 0 ) 
		{
			CFIDIO::GetFIDName(c_file.name,szFIDName);			
			if(MatchWellName(szFIDName, szDir + c_file.name))
			{
				CFIDIO* pFIDIO = new CFIDIO(szDir + c_file.name);         
				if (pFIDIO->Open())
				{	
					dep1 = pFIDIO->GetStartDepth();
					dep2 = pFIDIO->GetEndDepth();
					if(dep1>Sdep)	Sdep=dep1;
					if(dep2<Edep)	Edep=dep2;
				}
				delete pFIDIO;
			}
		}
		_findclose( hFile );
		EndWaitCursor();
		
		if(m_ProSdep<m_ProEdep&&m_ProEdep>0.f)
			SetDepth(m_ProSdep,m_ProEdep);
		else if(Sdep<Edep&&Edep<9999.f)
			SetDepth(Sdep,Edep);
		else
			OnSetDepth();
	}

	POSITION pos = GetFirstViewPosition();
	CImageManView* pView = (CImageManView*)GetNextView(pos);
	if(pView!=NULL)
	{
		pView->DelUnDoOpration();
		UpdateAllViews(pView,HINT_UPDATE_WINDOW);
		int xxx=OpenPath.Find (".");
		if(xxx>-1)
			m_LogName=OpenPath.Left(xxx);
		else
			m_LogName=OpenPath;	
		m_PathName=OpenPath;
	}
}

void CImageManDoc::OnReFresh()
{
	HandMarkToWell();
	SetDepthScale(m_DepthScale);
}

CMarkVectorObj* CImageManDoc::GetMarkVectorObject()
{
	CTrackObj *pObj;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CTrackObj*)m_TrackList.GetNext(pos);
		if(pObj->IsKindOf(RUNTIME_CLASS(CTrackObj)))
		{
			POSITION pos0;
			pos0 = pObj->m_ObjList.GetHeadPosition();
			CTrackObj* pObj0;
			while (pos0 != NULL)
			{
				pObj0 = (CTrackObj*)pObj->m_ObjList.GetNext(pos0);				
				if(pObj0->IsKindOf(RUNTIME_CLASS(CTrackObj)))
				{
					POSITION pos1;
					pos1 = pObj0->m_ObjList.GetHeadPosition();
					CTrackObj* pObj1;
					while (pos1 != NULL)
					{
						pObj1 = (CTrackObj*)pObj0->m_ObjList.GetNext(pos1);
						if(pObj1->IsKindOf(RUNTIME_CLASS(CTrackObj)))
						{
							POSITION pos2;
							pos2 = pObj1->m_ObjList.GetHeadPosition();
							CTrackObj* pObj2;
							while (pos2 != NULL)
							{
								pObj2 = (CTrackObj*)pObj1->m_ObjList.GetNext(pos2);
								if(pObj2->IsKindOf(RUNTIME_CLASS(CTrackObj)))
								{
									POSITION pos3;
									pos3 = pObj2->m_ObjList.GetHeadPosition();
									CDrawObj* pObj3 = (CStratumObj*)pObj2->m_ObjList.GetPrev(pos);
									if (pObj3 != NULL) 
										if(pObj3->IsKindOf (RUNTIME_CLASS(CMarkVectorObj)))
											return (CMarkVectorObj*)pObj3;
								}
								if(pObj2->IsKindOf (RUNTIME_CLASS(CMarkVectorObj)))
									return (CMarkVectorObj*)pObj2;
							}
						}
						if(pObj1->IsKindOf (RUNTIME_CLASS(CMarkVectorObj)))
							return (CMarkVectorObj*)pObj1;
					}
				}
				if(pObj0->IsKindOf (RUNTIME_CLASS(CMarkVectorObj)))
					return (CMarkVectorObj*)pObj0;
			}
		}
		if(pObj->IsKindOf (RUNTIME_CLASS(CMarkVectorObj)))
			return (CMarkVectorObj*)pObj;
	}
	return NULL;
}

CMarkFreqObj* CImageManDoc::GetMarkFreqObject()
{
	CTrackObj *pObj;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CTrackObj*)m_TrackList.GetNext(pos);
		if(pObj->IsKindOf(RUNTIME_CLASS(CTrackObj)))
		{
			POSITION pos0;
			pos0 = pObj->m_ObjList.GetHeadPosition();
			CTrackObj* pObj0;
			while (pos0 != NULL)
			{
				pObj0 = (CTrackObj*)pObj->m_ObjList.GetNext(pos0);				
				if(pObj0->IsKindOf(RUNTIME_CLASS(CTrackObj)))
				{
					POSITION pos1;
					pos1 = pObj0->m_ObjList.GetHeadPosition();
					CTrackObj* pObj1;
					while (pos1 != NULL)
					{
						pObj1 = (CTrackObj*)pObj0->m_ObjList.GetNext(pos1);
						if(pObj1->IsKindOf(RUNTIME_CLASS(CTrackObj)))
						{
							POSITION pos2;
							pos2 = pObj1->m_ObjList.GetHeadPosition();
							CTrackObj* pObj2;
							while (pos2 != NULL)
							{
								pObj2 = (CTrackObj*)pObj1->m_ObjList.GetNext(pos2);
								if(pObj2->IsKindOf(RUNTIME_CLASS(CTrackObj)))
								{
									POSITION pos3;
									pos3 = pObj2->m_ObjList.GetHeadPosition();
									CDrawObj* pObj3 = (CStratumObj*)pObj2->m_ObjList.GetPrev(pos);
									if (pObj3 != NULL) 
										if(pObj3->IsKindOf (RUNTIME_CLASS(CMarkFreqObj)))
											return (CMarkFreqObj*)pObj3;
								}
								if(pObj2->IsKindOf (RUNTIME_CLASS(CMarkFreqObj)))
									return (CMarkFreqObj*)pObj2;
							}
						}
						if(pObj1->IsKindOf (RUNTIME_CLASS(CMarkFreqObj)))
							return (CMarkFreqObj*)pObj1;
					}
				}
				if(pObj0->IsKindOf (RUNTIME_CLASS(CMarkFreqObj)))
					return (CMarkFreqObj*)pObj0;
			}
		}
		if(pObj->IsKindOf (RUNTIME_CLASS(CMarkFreqObj)))
			return (CMarkFreqObj*)pObj;
	}
	return NULL;
}

CStratumObj* CImageManDoc::GetStratumObject()
{
	CTrackObj *pObj;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CTrackObj*)m_TrackList.GetNext(pos);
		if(pObj->IsKindOf(RUNTIME_CLASS(CTrackObj)))
		{
			POSITION pos0;
			pos0 = pObj->m_ObjList.GetHeadPosition();
			CTrackObj* pObj0;
			while (pos0 != NULL)
			{
				pObj0 = (CTrackObj*)pObj->m_ObjList.GetNext(pos0);				
				if(pObj0->IsKindOf(RUNTIME_CLASS(CTrackObj)))
				{
					POSITION pos1;
					pos1 = pObj0->m_ObjList.GetHeadPosition();
					CTrackObj* pObj1;
					while (pos1 != NULL)
					{
						pObj1 = (CTrackObj*)pObj0->m_ObjList.GetNext(pos1);
						if(pObj1->IsKindOf(RUNTIME_CLASS(CTrackObj)))
						{
							POSITION pos2;
							pos2 = pObj1->m_ObjList.GetHeadPosition();
							CTrackObj* pObj2;
							while (pos2 != NULL)
							{
								pObj2 = (CTrackObj*)pObj1->m_ObjList.GetNext(pos2);
								if(pObj2->IsKindOf(RUNTIME_CLASS(CTrackObj)))
								{
									POSITION pos3;
									pos3 = pObj2->m_ObjList.GetHeadPosition();
									CDrawObj* pObj3 = (CStratumObj*)pObj2->m_ObjList.GetPrev(pos3);
									if (pObj3 != NULL) 
										if(pObj3->IsKindOf (RUNTIME_CLASS(CStratumObj)))
											return (CStratumObj*)pObj3;
								}
								if(pObj2->IsKindOf (RUNTIME_CLASS(CStratumObj)))
									return (CStratumObj*)pObj2;
							}
						}
						if(pObj1->IsKindOf (RUNTIME_CLASS(CStratumObj)))
							return (CStratumObj*)pObj1;
					}
				}
				if(pObj0->IsKindOf (RUNTIME_CLASS(CStratumObj)))
					return (CStratumObj*)pObj0;
			}
		}
		if(pObj->IsKindOf (RUNTIME_CLASS(CStratumObj)))
			return (CStratumObj*)pObj;
	}
	return NULL;
}

CFoldObj* CImageManDoc::GetFoldObject()
{
	CFoldObj *pObj=NULL;
	POSITION pos = m_RemObjList.GetTailPosition();
	while(pos != NULL)
	{
		pObj = (CFoldObj*)m_RemObjList.GetPrev(pos);
		if(pObj != NULL) 
			if(pObj->IsKindOf (RUNTIME_CLASS(CFoldObj)))
				return pObj;
	} 
	return NULL;
}

CDepSect* CImageManDoc::GetLineObject()
{
	CDepSect *pObj=NULL;
	POSITION pos = m_RemObjList.GetTailPosition();
	while (pos != NULL)
	{
		pObj = (CDepSect*)m_RemObjList.GetPrev(pos);
		if(pObj != NULL)
			if(pObj->IsKindOf (RUNTIME_CLASS(CDepSect)))
				 return pObj;
	} 
	return NULL;
}

float CImageManDoc::GetCurrentDep(int DepY)
{
	float dep=(float)GetLeftTrackObj ()->YCoorToDepth(DepY);
	dep=((int)(dep*1000+0.5))/1000.f;
	return dep;
}

CString CImageManDoc::GetCurveValue(int DepY)
{
	CString ValueStr,str;
	float dep=(float)GetLeftTrackObj ()->YCoorToDepth(DepY);
	dep=((int)(dep*1000+0.5))/1000.f;
	m_HorRulerDep=dep;
	float value,dep1,dep2;
	ValueStr.Format ("深度  %.1f",dep);
	CTrackObj* pObj;
	CFIDDrawObj* pCurve;
	POSITION posXX;
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CTrackObj*)m_TrackList.GetNext(pos);
		if(pObj->IsKindOf(RUNTIME_CLASS(CTrackObj)))
		{
			POSITION pos0;
			pos0 = pObj->m_ObjList.GetHeadPosition();
			CTrackObj* pObj0;
			while (pos0 != NULL)
			{
				pObj0 = (CTrackObj*)pObj->m_ObjList.GetNext(pos0);				
				if(pObj0->IsKindOf(RUNTIME_CLASS(CTrackObj)))
				{
					POSITION pos1;
					pos1 = pObj0->m_ObjList.GetHeadPosition();
					CTrackObj* pObj1;
					while (pos1 != NULL)
					{
						pObj1 = (CTrackObj*)pObj0->m_ObjList.GetNext(pos1);
						if(pObj1->IsKindOf(RUNTIME_CLASS(CTrackObj)))
						{
							POSITION pos2;
							pos2 = pObj1->m_ObjList.GetHeadPosition();
							CTrackObj* pObj2;
							while (pos2 != NULL)
							{
								pObj2 = (CTrackObj*)pObj1->m_ObjList.GetNext(pos2);
								if(pObj2->IsKindOf(RUNTIME_CLASS(CTrackObj)))
								{
									POSITION pos3;
									pos3 = pObj2->m_ObjList.GetHeadPosition();
									CTrackObj* pObj3;
									while (pos3 != NULL)
									{	
										pObj3 = (CTrackObj*)pObj2->m_ObjList.GetNext(pos3);
										posXX = pObj3->m_ObjList.GetHeadPosition();
										while (posXX != NULL)
										{
											pCurve = (CFIDDrawObj*)pObj3->m_ObjList.GetNext(posXX);
											if((pCurve!=NULL)&&
												(pCurve->IsKindOf (RUNTIME_CLASS(CCurveObj))))
											{
												CFIDIO* pFIDIO = new CFIDIO(pCurve->m_strPathName);         
												if (pFIDIO->Open())
												{	
													dep1 = pFIDIO->GetStartDepth();
													dep2 = pFIDIO->GetEndDepth();
													if((dep>dep1)&&(dep<dep2))
													{
														str=pCurve->m_strPathName.Mid (pCurve->m_strPathName.ReverseFind ('.')+1);
														ValueStr+="  "+str;
														pFIDIO->SeekToDepth (dep);
														pFIDIO->Read (2,1,&value);
														str.Format ("=%g",value);
														ValueStr+=str;
													}
												}
												delete pFIDIO;
											}
										}
									}
								}
								posXX = pObj2->m_ObjList.GetHeadPosition();
								while (posXX != NULL)
								{
									pCurve = (CFIDDrawObj*)pObj2->m_ObjList.GetNext(posXX);
									if((pCurve!=NULL)&&
										(pCurve->IsKindOf (RUNTIME_CLASS(CCurveObj))))
									{
										CFIDIO* pFIDIO = new CFIDIO(pCurve->m_strPathName);         
										if (pFIDIO->Open())
										{	
											dep1 = pFIDIO->GetStartDepth();
											dep2 = pFIDIO->GetEndDepth();
											if((dep>dep1)&&(dep<dep2))
											{
												str=pCurve->m_strPathName.Mid (pCurve->m_strPathName.ReverseFind ('.')+1);
												ValueStr+="  "+str;
												pFIDIO->SeekToDepth (dep);
												pFIDIO->Read (2,1,&value);
												str.Format ("=%g",value);
												ValueStr+=str;
											}
										}
										delete pFIDIO;
									}
								}
							}
						}

						posXX = pObj1->m_ObjList.GetHeadPosition();
						while (posXX != NULL)
						{
							pCurve = (CFIDDrawObj*)pObj1->m_ObjList.GetNext(posXX);
							if((pCurve!=NULL)&&
								(pCurve->IsKindOf (RUNTIME_CLASS(CCurveObj))))
							{
								CFIDIO* pFIDIO = new CFIDIO(pCurve->m_strPathName);         
								if (pFIDIO->Open())
								{	
									dep1 = pFIDIO->GetStartDepth();
									dep2 = pFIDIO->GetEndDepth();
									if((dep>dep1)&&(dep<dep2))
									{
										str=pCurve->m_strPathName.Mid (pCurve->m_strPathName.ReverseFind ('.')+1);
										ValueStr+="  "+str;
										pFIDIO->SeekToDepth (dep);
										pFIDIO->Read (2,1,&value);
										str.Format ("=%g",value);
										ValueStr+=str;
									}
								}
								delete pFIDIO;
							}
						}
					}
				}
				posXX = pObj0->m_ObjList.GetHeadPosition();
				while (posXX != NULL)
				{
					pCurve = (CFIDDrawObj*)pObj0->m_ObjList.GetNext(posXX);
					if((pCurve!=NULL)&&
						(pCurve->IsKindOf (RUNTIME_CLASS(CCurveObj))))
					{
						CFIDIO* pFIDIO = new CFIDIO(pCurve->m_strPathName);         
						if (pFIDIO->Open())
						{	
							dep1 = pFIDIO->GetStartDepth();
							dep2 = pFIDIO->GetEndDepth();
							if((dep>dep1)&&(dep<dep2))
							{
								str=pCurve->m_strPathName.Mid (pCurve->m_strPathName.ReverseFind ('.')+1);
								ValueStr+="  "+str;
								pFIDIO->SeekToDepth (dep);
								pFIDIO->Read (2,1,&value);
								str.Format ("=%g",value);
								ValueStr+=str;
							}
						}
						delete pFIDIO;
					}
				}
			}
		}
		posXX = pObj->m_ObjList.GetHeadPosition();
		while (posXX != NULL)
		{
			pCurve = (CFIDDrawObj*)pObj->m_ObjList.GetNext(posXX);
			if((pCurve!=NULL)&&
				(pCurve->IsKindOf (RUNTIME_CLASS(CCurveObj))))
			{
				CFIDIO* pFIDIO = new CFIDIO(pCurve->m_strPathName);         
				if (pFIDIO->Open())
				{	
					dep1 = pFIDIO->GetStartDepth();
					dep2 = pFIDIO->GetEndDepth();
					if((dep>dep1)&&(dep<dep2))
					{
						str=pCurve->m_strPathName.Mid (pCurve->m_strPathName.ReverseFind ('.')+1);
						ValueStr+="  "+str;
						pFIDIO->SeekToDepth (dep);
						pFIDIO->Read (2,1,&value);
						str.Format ("=%g",value);
						ValueStr+=str;
					}
				}
				delete pFIDIO;
			}
		}
	} 
	return ValueStr;
}

void CImageManDoc::DeleteAllRemDraw()
{
	m_RemObjList.RemoveAll ();
	SetSize();
}

void CImageManDoc::OnMouseTip() 
{
	// TODO: Add your command handler code here
	m_MouseTip=!m_MouseTip;
}

void CImageManDoc::OnUpdateMouseTip(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck (m_MouseTip);
}

void CImageManDoc::OnFillLithground() 
{
	// TODO: Add your command handler code here
	m_FillLithGround=!m_FillLithGround;
	OnReFresh();
}

void CImageManDoc::OnUpdateFillLithground(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck (m_FillLithGround);
}

void CImageManDoc::OnLayerFlush()
{
	CTrackObj* pObj;
	POSITION pos;
	pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CTrackObj*)m_TrackList.GetNext(pos);
		if(pObj!=NULL)
		{
			if(pObj->bDrawDepth &&(pObj->m_LayerFileName !=""))
				pObj->m_LayerFlush =TRUE;
		}
	}
	UpdateAllViews(NULL,HINT_UPDATE_SIZE);
}

void CImageManDoc::MatchAllObject(CString OpenPath)
{
	CString FullWellName=OpenPath.Left (OpenPath.ReverseFind ('\\'));
	m_WellName=FullWellName.Mid(FullWellName.ReverseFind ('#')+1);
	FullWellName=FullWellName+"\\"+m_WellName;

	//得到该口井的路径
	CString szDir;
	CFIDIO::GetPath(FullWellName,szDir.GetBuffer(512));
	szDir.ReleaseBuffer();
	
	if(!CopyAllFile(szDir,m_DataBuf))
	{
		AfxMessageBox("数据备份失败        ");
		return;
	}
	m_OldWellName=szDir;
	BeginWaitCursor();
	MatchListFile(FullWellName);
	
	//得到将要寻找的索引文件IWellName.*
	char szFind[512];
	CFIDIO::GetFileName(FullWellName,szFind,'I');
	strcat(szFind,".*");
	char szFIDName[512];
	struct _finddata_t c_file;    //Microsoft C++ 的文件结构
	long hFile;
	if ((hFile = _findfirst(szFind, &c_file )) != -1L)
	{ 
		CFIDIO::GetFIDName(c_file.name,szFIDName);
		MatchWellName(szFIDName, szDir + c_file.name);
		while(_findnext(hFile,&c_file) == 0 ) 
		{
			CFIDIO::GetFIDName(c_file.name,szFIDName);
			MatchWellName(szFIDName, szDir + c_file.name);
		}
		_findclose( hFile );
	}
	EndWaitCursor();
	POSITION pos = GetFirstViewPosition();
	CView* pView= GetNextView(pos);
	UpdateAllViews(pView,HINT_UPDATE_WINDOW);
	if(m_WellName.Find(".")>-1)
		m_WellName=m_WellName.Left (m_WellName.Find("."));
}

BOOL CImageManDoc::CopyAllFile(CString OldDirName, CString NewDirName)
{
/*	if(OldDirName.Right (1)!="\\") OldDirName+="\\";
	if(NewDirName.Right (1)!="\\") NewDirName+="\\";
	CString CopyFileName=OldDirName+"I"+m_WellName+"*.*";
	CString TargetFileName;CString ss,WellName;
	struct _finddata_t c_file;
    long hFile;int xxx;
	BeginWaitCursor();
	xxx=NewDirName.Find("#");
	WellName=NewDirName.Mid(xxx+1);
    if( (hFile = _findfirst(CopyFileName, &c_file )) != -1L )
    {	
		if(!(c_file.attrib&_A_SUBDIR))
		{
			CopyFileName=OldDirName+c_file.name;
			CFIDIO* pFIDIO = new CFIDIO(CopyFileName); 		
			if (pFIDIO->Open())
			{	
				int dim = pFIDIO->GetDimenSion();
				delete pFIDIO;
				if(dim<3)
				{
					TargetFileName=NewDirName+c_file.name;
					CopyFile(CopyFileName,TargetFileName,FALSE);
					ss=c_file.name;
					ss="D"+ss.Mid (1);
					CopyFileName=OldDirName+ss;
					TargetFileName=NewDirName+ss;
					CopyFile(CopyFileName,TargetFileName,FALSE);
				}
			}
			else
				delete pFIDIO;	
		}
	    while( _findnext( hFile, &c_file ) == 0 )
		{
			if(!(c_file.attrib&_A_SUBDIR))
			{
				CopyFileName=OldDirName+c_file.name;
				CFIDIO* pFIDIO = new CFIDIO(CopyFileName); 		
				if (pFIDIO->Open())
				{	
					int dim = pFIDIO->GetDimenSion();
					delete pFIDIO;
					if(dim<3)
					{
						TargetFileName=NewDirName+c_file.name;
						CopyFile(CopyFileName,TargetFileName,FALSE);
						ss=c_file.name;
						ss="D"+ss.Mid (1);
						CopyFileName=OldDirName+ss;
						TargetFileName=NewDirName+ss;
						CopyFile(CopyFileName,TargetFileName,FALSE);
					}
				}
				else
					delete pFIDIO;	
			}
		}    
	}
	_findclose( hFile );
	EndWaitCursor();*/
	return TRUE;
}

void CImageManDoc::OnSaveData() 
{
	// TODO: Add your command handler code here
	if(!CopyAllFile(m_OldWellName,m_DataBuf))
	{
		AfxMessageBox("保存数据源数据失败        ");
		return;
	}
	WellRefresh();
	m_UnDo=FALSE;
	POSITION pos = GetFirstViewPosition();
	CImageManView* pView = (CImageManView*)GetNextView(pos);
	if(pView!=NULL)
		pView->DelUnDoOpration();
}
void CImageManDoc::WellRefresh() 
{
	CString WellName=m_OldWellName;
	if(WellName.Right(1)=="\\")
		WellName=WellName.Left(WellName.GetLength()-1);
	CString str=WellName.Mid(WellName.ReverseFind('#')+1);
	WellName+="\\";
	WellName+=str;
	char TargetCurveName[256];
	strcpy(TargetCurveName,WellName);
	strcat(TargetCurveName,".FID");
	RefurbishFID(TargetCurveName);
}

void CImageManDoc::OnUpdateSaveData(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable (m_UnDo);
}

void CImageManDoc::BufDataToWell()
{
/*	if(m_UnDo)
	{
		Beep(700,150);
		if(AfxMessageBox("保存数据源的数据\n\n"+m_OldWellName+"    ",MB_YESNO|MB_ICONQUESTION)==IDNO)
 			CopyAllFile(m_DataBuf,m_OldWellName);
		WellRefresh();
	}*/
	CSinPoint* pObj;
	BOOL bSave=FALSE;
	POSITION pos = m_SinObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CSinPoint*) m_SinObjList.GetNext(pos);
		if(pObj->m_bSave)
		{
			bSave=TRUE;
			break;
		}
	}
	if(bSave||(m_HWOprateNum>0))
	{
		if(AfxMessageBox("保存交互解释结论\n\n"+m_OldWellName+"    ",
			MB_YESNO|MB_ICONQUESTION)==IDYES)
			HandMarkToWell();
	}
	pos = GetFirstViewPosition();
	CImageManView* pView = (CImageManView*)GetNextView(pos);
	if(pView!=NULL)
	{
		if(pView->m_ContourList.GetCount()>0&&pView->m_iPickupNum>0)
		{
			if(AfxMessageBox("保存交互解释结论\n\n"+m_OldWellName+"    ",
				MB_YESNO|MB_ICONQUESTION)==IDYES)
				pView->OnSaveContour();
		}
	}
}	

void CImageManDoc::OnSaveImagemark() 
{
	// TODO: Add your command handler code here
	CString Path;
	if(m_ImageHandWorkFile=="")
	{
		Path=m_LogName;
		Path=Path.Left(Path.ReverseFind('\\')+1);
		m_ImageHandWorkFile=Path+"HandWork.List";
	}
	SaveHandMark();
}

void CImageManDoc::HandMarkToWell()
{
	CSinPoint* pObj;
	BOOL bSave=FALSE;
	POSITION pos = m_SinObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CSinPoint*) m_SinObjList.GetNext(pos);
		if(pObj->m_bSave)
		{
			bSave=TRUE;
			break;
		}
	}
	if(bSave||(m_HWOprateNum>0))
		OnSaveImagemark();
}

void CImageManDoc::DelSinObjList()
{
	HandMarkToWell();
	POSITION pos = m_SinObjList.GetHeadPosition();
	while (pos != NULL)
		delete m_SinObjList.GetNext(pos);
	m_SinObjList.RemoveAll();
	m_HWOprateNum=0;
}

void CImageManDoc::DelDepSectSin(float sdep,float edep)
{
	CSinPoint* pObj;
	POSITION pos = m_SinObjList.GetHeadPosition();
	while(pos != NULL)
	{
		pObj = (CSinPoint*) m_SinObjList.GetNext(pos);
		if(pObj!=NULL&&pObj->m_Dep >sdep&&pObj->m_Dep <edep)
			RemoveObj(pObj);
	}
}

void CImageManDoc::ModifyDepSectSin(float sdep,float edep)
{
	CModifyHandWorkDlg Dlg;
	if(Dlg.DoModal ()==IDOK)
	{
		CString Title[6]={"层理","充填缝","张开缝","压裂缝","半充填缝","层间缝"};
		COLORREF Color[6]={0x009300,0x00FF00,0xFF0000,0x800080,0xFF8000,0xFFFF00};
		CSinPoint* pObj;
		POSITION pos = m_SinObjList.GetHeadPosition();
		while (pos != NULL)
		{
			pObj = (CSinPoint*) m_SinObjList.GetNext(pos);
			if(pObj!=NULL&&pObj->m_Dep >sdep&&pObj->m_Dep <edep)
			{
				pObj->m_Color=Color[Dlg.m_TypeSin];
				pObj->m_Flag=Title[Dlg.m_TypeSin];
				pObj->m_bSave=TRUE;
				pObj->Invalidate();
			}
		}
	}
}

void CImageManDoc::OnSaveasImagemark() 
{
	// TODO: Add your command handler code here
	CString Path;
	if(m_ImageHandWorkFile=="")
	{
		Path=m_LogName;
		Path=Path.Left(Path.ReverseFind('\\')+1);
		m_ImageHandWorkFile=Path+"HandWork.List";
	}
	else
	{
		Path=m_ImageHandWorkFile;
		Path=Path.Left(Path.ReverseFind('\\'));
	}
	static char BASED_CODE szFilter[] = 
	"交互解释结论(*.List)|*.List|所有文件(*.*)|*.*||";
	CFileDialog  Browse(FALSE, "交互解释结论(*.List)", m_ImageHandWorkFile, 
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	Browse.m_ofn.lpstrTitle = "保存文件";

	Browse.m_ofn.lpstrInitialDir=Path;
	if(Browse.DoModal()==IDOK)
	{
		m_ImageHandWorkFile =Browse.GetPathName();
		SaveHandMark();
	}
}

void CImageManDoc::OnUpdateSaveasImagemark(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(GetStratumObject()!=NULL);
}

void CImageManDoc::SaveHandMark()
{
	POSITION pos = GetFirstViewPosition();
	CImageManView* pView = (CImageManView*)GetNextView(pos);
	if(pView!=NULL)
	{
		pView->DeSelectObj();
		UpdateAllViews(pView,HINT_UPDATE_WINDOW);
	}
	CString Path;
	if(m_ImageHandWorkFile=="")
	{
		Path=m_LogName;
		Path=Path.Left(Path.ReverseFind('\\')+1);
		m_ImageHandWorkFile=Path+"HandWork.List";
	}
	CStratumObj* pStObj=GetStratumObject();
	if(pStObj!=NULL)
	{
		double VHr=-999.25f;
		POSITION pos=NULL;
		CTrackObj* pTrack=(CTrackObj*)GetStratumObject()->GetParentObj();
		if(pTrack!=NULL)
		{
			pos= pTrack->m_ObjList.GetHeadPosition();
			while(pos!=NULL)
			{
				CImageObj* pImage = (CImageObj*)pTrack->m_ObjList.GetNext(pos);
				if(pImage!= NULL) 
				{
					double HorScale=pImage->m_Bit*PI/(pImage->m_Rect.Width ()*0.254f);
					VHr=m_DepthScale/HorScale;
					break;
				}
			}
		}
		if(VHr<0.f)
		{
			AfxMessageBox("在该格线道内未找到图像对象      ");
			return;
		}
		CString str;
		CStdioFile File;
		if(File.Open(m_ImageHandWorkFile, CFile::modeWrite | CFile::modeCreate| CFile::typeText)) 
		{
			CString WellName;
			if(m_szBufPath!="")
				WellName=m_szBufPath;
			else
			{
				WellName=m_LogName.Left (m_LogName.ReverseFind ('\\'));
				WellName=WellName+"\\"
					+WellName.Mid (WellName.ReverseFind ('#')+1);
			}
			int i,nb=1;
			float dep,dip,dir,dip0,dir0;
			str="序号,深度,原始角度,原始方位,类型,真角度,真方位";
			File.WriteString(str);

			CSinPoint* pObj=NULL;
			pos = m_SinObjList.GetHeadPosition();	
			if(m_sSeries=="MIT")
			{
				int yyy;
				while(pos!=NULL)
				{
					pObj = (CSinPoint*) m_SinObjList.GetNext(pos);
					if(pObj->m_bSave)
					{
						int Num=pObj->m_nPoints/4;
						if(Num>95) Num=95;
						yyy=pObj->m_Points[0].y;
						for(i=1;i<Num;i++)
							yyy=(yyy+pObj->m_Points[i*4].y)/2;
						dep=(float)GetLeftTrackObj()->YCoorToDepth(yyy);
						dip=dir=dip0=dir0=0.f;
						pObj->m_bSave=FALSE;
					}
					else
					{
						dep=pObj->m_Dep ;
						dip=pObj->m_Dip;
						dir=pObj->m_Dir ;
						dip0=pObj->m_Dip0;
						dir0=pObj->m_Dir0 ;
					}
					str.Format("\n%d,%g,%g,%g,%s,%g,%g,%g",
							  nb,dep,dip,dir,pObj->m_Flag,dip0,dir0);
					File.WriteString(str);
					nb++;
				}
				m_HWOprateNum=0;
				File.Close();
				pStObj->m_strPathName=m_ImageHandWorkFile;
				return;
			}

			BOOL bCorre=TRUE;
			CLogProcess *pLog;float *XX;
			pLog = new CLogProcess(WellName,NULL);
			INCURVE *PInCurve = new INCURVE[4];
			strcpy(PInCurve[0].name,m_sDEVName);
			strcpy(PInCurve[1].name,m_sDAZName);
			strcpy(PInCurve[2].name,m_sAZ1Name);
			strcpy(PInCurve[3].name,m_sRBName);
			XX = new float[4];
			pLog->InitIn(4,PInCurve,(char *)XX);
			pLog->InitBuffer();
			if(pLog->DepLevel <= 0.0001)
			{
				AfxMessageBox("读取井斜、井斜方位等曲线数据失败        ",
					MB_OK|MB_ICONERROR);
				bCorre=FALSE;
			}
			while (pos != NULL)
			{
				pObj = (CSinPoint*) m_SinObjList.GetNext(pos);
				if(pObj->m_bSave)
				{
					CRect rect=GetStratumObject()->m_Rect;
					PPtoLP(&rect);
					CDIPCalculate dipObj;	
					int Num=pObj->m_nPoints/4;
					if(Num>95) Num=95;
					dipObj.vnn=Num;
					dipObj.x[0]=pObj->m_Points[0].x-rect.left;
					dipObj.y[0]=pObj->m_Points[0].y*VHr;
					for(i=1;i<Num;i++)
					{
						dipObj.x[i]=(double)(pObj->m_Points[i*4].x-rect.left);
						dipObj.y[i]=(double)pObj->m_Points[i*4].y*VHr;
					}
					dipObj.ll=rect.Width();
					dipObj.Compute ();	
					
					dip=(float)dipObj.m_dip ;
					dir=(float)dipObj.m_dir;
					if(dir>360.f) 
						dir-=360.;
					if(dip<0.05f) 
						dip=0.05f;
					dep=(float)GetLeftTrackObj ()->YCoorToDepth((int)(dipObj.m_dep/VHr));
					pObj->m_bSave=FALSE;
					
					if(bCorre)//作背景校正
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
							dir0=dir+XX[2]; 
							if(dir0>2.*PI) 
								dir0=(float)(dir0-2.*PI);
						}
					}
					else
					{
						dip0=dip;
						dir0=dir;
					}
					if(m_fDip1>0.5f)
					{
						dipObj.m_dip =dip0;
						dipObj.m_dir =dir0;
						dipObj.m_dip1 =m_fDip1;
						dipObj.m_dir1 =m_fDir1;
						dipObj.ComputeDIPs ();
						dip0=(float)dipObj.m_dips ;
						dir0=(float)dipObj.m_dirs ;
					}
				}
				else
				{
					dep=pObj->m_Dep ;
					dip=pObj->m_Dip;
					dir=pObj->m_Dir ;
					dip0=pObj->m_Dip0;
					dir0=pObj->m_Dir0 ;
				}
				while(dir0>360.f) 
					dir0-=360.f;
				while(dir0<0.f) 
					dir0+=360.f;
				str.Format("\n%d,%g,%g,%g,%s,%g,%g,%g",
							  nb,dep,dip,dir,pObj->m_Flag,dip0,dir0);
				File.WriteString(str);
				nb++;
			}
			delete []XX;
			delete PInCurve;
			delete pLog;
			m_HWOprateNum=0;
			File.Close();
		}
		pStObj->m_strPathName=m_ImageHandWorkFile;
	}
}

void CImageManDoc::UpdateVector() 
{
	// TODO: Add your command handler code here
	this->BeginWaitCursor();
	CStratumObj* pStObj=GetStratumObject();
	if(pStObj!=NULL)
		pStObj->UpdatePoint();
	CMarkVectorObj* pVectorObj=GetMarkVectorObject();
	if(pVectorObj!=NULL)
		pVectorObj->UpdatePoint();
	CMarkFreqObj* pFreq=GetMarkFreqObject();
	if(pFreq!=NULL) 
		pFreq->UpdatePoint();
	this->EndWaitCursor();
}

void CImageManDoc::OnSetMarkcorrepara() 
{
	// TODO: Add your command handler code here
	CMarkDipCorre dlg;
	dlg.m_sAZ1Name =m_sAZ1Name;
	dlg.m_sDAZName =m_sDAZName;
	dlg.m_sDEVName =m_sDEVName;
	dlg.m_sRBName =m_sRBName;
	dlg.m_sSeries =m_sSeries;
	dlg.m_WellName= m_LogName;
	dlg.m_fDip1 =m_fDip1;
	dlg.m_fDir1=m_fDir1;
	if(dlg.DoModal()==IDOK)
	{
		m_sAZ1Name=dlg.m_sAZ1Name ;
		m_sDAZName=dlg.m_sDAZName ;
		m_sDEVName=dlg.m_sDEVName ;
		m_sRBName=dlg.m_sRBName ;
		m_sSeries=dlg.m_sSeries ;
		m_WellName=dlg.m_WellName ;
		m_fDip1=dlg.m_fDip1 ;
		m_fDir1=dlg.m_fDir1 ;
	}
}

void CImageManDoc::OnUpdateSetMarkcorrepara(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_LogName!="");
}

void CImageManDoc::SetDepthNoRem(double sdep, double edep)
{
	m_PlotSdep = sdep; m_PlotEdep = edep;                                              
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		CTrackObj* pObj = (CTrackObj*)m_TrackList.GetNext(pos);
		pObj->SetDepth(m_PlotSdep,m_PlotEdep);
	} 
	if (bVertAutoAlign)
	{
		if (m_bVertAlignDepth) DoVertAlignDepth();
		if (m_bVertAlignTop) DoVertAlignTop();
	}
	SetSizeNoRem();
	m_ProSdep=m_PlotSdep;
	m_ProEdep=m_PlotEdep;
} 

void CImageManDoc::SetSizeNoRem()
{
	POSITION pos;
	CPoint point(0,0);
	SelectMode nSelectMode = CDrawObj::m_SelectMode;
	CDrawObj::m_SelectMode = move;
	CRect rect = GetTrackUnionRect(); 
	if (m_TrackRect.top > rect.top)
	{
		point.y = m_TrackRect.top - rect.top;
		point.x = 0;
		pos = m_TrackList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*) m_TrackList.GetNext(pos);
			if (pObj != NULL) 
				pObj->SetRect(pObj->GetRect()+ point);
		}
		pos = m_RemObjList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*) m_RemObjList.GetNext(pos);
			if (pObj == NULL) continue;
			if (!((pObj->IsKindOf(RUNTIME_CLASS(CRemDrawObj))) 
				&& (pObj->m_Rect.top < m_TrackRect.top)))
			pObj->SetRect(pObj->GetRect()+ point);
		}
		MoveObj(point);
		rect += point;
		m_TrackRect.bottom += point.x;
	}                          

	CRect m_TopRect = GetTopRemObjUnionRect(m_TrackRect.top);
	if (m_TopRect.top - m_TopSpace != m_Rect.top)
	{
		point.y = m_Rect.top + m_TopSpace - m_TopRect.top;
		MoveObj(point);
		rect += point;
		m_TrackRect += point;
	}
	int y = m_TrackRect.bottom;
	if (y > rect.bottom) y = rect.bottom;
	CRect m_BottomRect = GetBottomRemObjUnionRect(y);
	if (m_BottomRect.bottom == m_BottomRect.top)
		m_TrackRect.bottom = m_BottomRect.bottom = rect.bottom;
	else if (m_BottomRect.bottom != rect.bottom)
	{
		point.y = rect.bottom - m_TrackRect.bottom; 
		m_TrackRect.bottom = rect.bottom;
	}
}

void CImageManDoc::BmpDraw(CView* pView,CDC* pDC)
{
	CRemDrawObj* pRemObj;
	if (pView->IsKindOf(RUNTIME_CLASS(CImageManView)))
	{
		POSITION pos = m_TrackList.GetHeadPosition();
		while (pos != NULL)
		{
			CTrackObj* pObj = (CTrackObj*)m_TrackList.GetNext(pos);
			if (pObj != NULL)
				pObj->Draw(pView,pDC);
		}                   
 		pos = m_RemObjList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*)m_RemObjList.GetNext(pos);
			if((pObj!=NULL) &&(!pObj->IsKindOf(RUNTIME_CLASS(CDepSect))))
			{
				pRemObj=(CRemDrawObj*)pObj;
				if((pRemObj!=NULL))
				{
					if(pRemObj->m_LogBrush .lbColor !=0x0ffffff)
							pRemObj->m_LogBrush .lbColor =0x0;
					if(pRemObj->m_LogPen .lopnColor!=0x0ffffff)
						pRemObj->m_LogPen .lopnColor =0x0;
					pRemObj->Draw (pView,pDC);
				}
				else
					pObj->Draw(pView,pDC);
			}
		}                
	}
}

CRect CImageManDoc::GetUnionRectNoRem()
{
	CRect rect(m_TrackRect.left,m_TrackRect.top,m_TrackRect.right,m_TrackRect.bottom);
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		if (pObj != NULL) 
		{
			if (rect.IsRectEmpty()) rect = pObj->GetMaxRect();
			else rect |= pObj->GetMaxRect();  
		}	
	}
	return rect;
}  

CRect CImageManDoc::GetTopRemObjUnionRect(int y)
{
	CRect rect1;
	CRect rect(0,y,0,y);
	POSITION pos = m_RemObjList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_RemObjList.GetNext(pos);
		if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CRemDrawObj))))
		{
			rect1 = pObj->GetRect();
			if (rect1.top < y)
			{
				if (rect.IsRectEmpty()) rect = rect1;
				else rect |= rect1;  
			}
		}	
	}                
	return rect;
}               

CRect CImageManDoc::GetBottomRemObjUnionRect(int y)
{
	CRect rect1;
	CRect rect(0,y,0,y);
	POSITION pos = m_RemObjList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_RemObjList.GetNext(pos);
		if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CRemDrawObj))))
		{
			rect1 = pObj->GetRect();
			if (rect1.bottom > y)
			{
				if (rect.IsRectEmpty()) rect = rect1;
				else rect |= rect1;  
			}
		}	
	}                
	return rect;
}

void CImageManDoc::OnBoxAnisys()
{
	m_bBoxAnisys=!m_bBoxAnisys;
}

void CImageManDoc::OnUpdateBoxAnisys(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_bBoxAnisys);
}