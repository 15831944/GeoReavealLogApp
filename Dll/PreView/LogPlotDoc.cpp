// LogPlotDoc.cpp : 实现文件
//

#include "stdafx.h"
#include "PreView.h"

#include "TrackDraw.h"
#include "CntrItem.h"
#include <io.h>
#include "LogPlotDoc.h" 
#include "LogPlotView.h"
#include "GeoFIDio.h"
#include "WidthDialog.h"
#include "HeightDialog.h"
#include "OleDraw.h"
#include "ResultListdraw.h"
#include "LthDraw.h"
#include "ResultDraw.h"
#include <Process.h>
#include "MarkVectorDraw.h"
#include "StratumDraw.h"
#include "MarkFreq.h"
#include "YieldDraw.h"
#include "LthNoteDraw.h"
#include "ImageDraw.h"
#include "DSCDraw.h"
#include "DepSect.h"
#include "GeoStorage.h"
#include "PageSetup.h"
// CLogPlotDoc
IMPLEMENT_DYNCREATE(CLogPlotDoc, COleDocument)

CLogPlotDoc::CLogPlotDoc()
{
	EnableCompoundFile();
	bNewDocument = FALSE;
	bEnableChildSizeMessage = TRUE;
	m_LeftSpace = m_RightSpace = m_TopSpace= m_BottomSpace =40;
	m_HeadTopSpace = m_HeadBottomSpace = 40;
	m_Rect.top = m_Rect.left = 0;
	m_Rect.right   = 880; m_Rect.bottom = 1080;

	m_TrackRect.left = m_LeftSpace; 
	m_TrackRect.right = m_Rect.right - m_RightSpace;
	m_TrackRect.top = m_TopSpace; 
	m_TrackRect.bottom = m_Rect.bottom - m_BottomSpace;
	m_HeadRect = m_Rect;
	m_HeadRect.bottom = 280;	
	m_DepthScale = 200;
	m_PlotSdep = 1000;
	m_PlotEdep = 1500;
	m_bChildTrackHeadAlignMax = TRUE;
	m_bChildTrackHeadHeightAuto = TRUE;
	m_HoriArrangleMode = IDM_LEFT;
	m_bVertAlignTop = TRUE;
	m_bVertAlignHead = FALSE;
	m_bVertAlignDepth = TRUE;
	bVertAutoAlign = TRUE;
	bHoriAutoArrangle = TRUE;
	m_paperColor = RGB(255, 255, 255);
	m_xGrid = m_yGrid = 3;
	bCopyFromTemplate = FALSE;
	bNullTemplate = FALSE;
	m_DocumentType = STAND_DOC;
	m_Flag = 1;
	m_LineType=0;
	m_DepthUnit = UNIT_M;
	m_DepC = m_DepthScale * 2.54 / 10000.;
	m_ScaleNo = 7;
	m_ScaleFactor = 100;
	m_DrawFIDHeadMode = 0;
	m_bPlotTopRem =m_bPlotBottomRem=TRUE;
	m_bMaxPageSize=m_bReserveSpace=FALSE;
	m_bPlotHead =m_bPlotResultList=FALSE;
	m_bPlotTrack=TRUE;
	m_VersionNo=3.5f;
	m_FillLithGround=TRUE;
	m_bPrint=FALSE;
	m_PlotResultModel=1;

	m_pView=NULL;
	//数据为链接字符串
	//如：10.132.204.16;sa;log;LogBase
	m_szSever=_T("");
	//用户角色
	//"SuperAdmin"	超级管理员
    //"Admin"			管理员
    //"Author"		员工
	m_szRole=_T("");
	m_szWellName=_T("");
	m_szProjectName=_T("");

	m_bRunFirst=FALSE;
	m_pGuage =NULL;
}

BOOL CLogPlotDoc::OnNewDocument()
{
	if (!COleDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CLogPlotDoc::~CLogPlotDoc()
{
	DeleteObList();
}


BEGIN_MESSAGE_MAP(CLogPlotDoc, COleDocument)

END_MESSAGE_MAP()


BOOL CLogPlotDoc::OpenPlotDoc(CString FileName)
{
	if(OnOpenDocument(FileName))
		return TRUE;
	else
		return FALSE;
}

BOOL CLogPlotDoc::SetupPage() 
{
	CPageSetup m_Page(this);
	if(m_Page.DoModal() == IDOK)
	{
		CPoint pt;
		pt.x = m_Page.m_LeftSpace - m_LeftSpace; 
		pt.y = m_Page.m_TopSpace - m_TopSpace; 
		SelectMode nSelectMode = CDrawObj::m_SelectMode;
		CDrawObj::m_SelectMode = move;
		
		MoveObj(pt);
		MoveHeadObj(pt);
		m_TrackRect += pt;
		m_Rect.right = m_Page.m_Width + m_Rect.left;
		m_Rect.bottom += m_Page.m_TopSpace - m_TopSpace + m_Page.m_BottomSpace - m_BottomSpace;

		m_LeftSpace = m_Page.m_LeftSpace;
		m_RightSpace = m_Page.m_RightSpace;
		m_TopSpace = m_Page.m_TopSpace;
		m_BottomSpace = m_Page.m_BottomSpace;
		m_HeadTopSpace = m_Page.m_HeadTopSpace;
		m_HeadBottomSpace = m_Page.m_HeadBottomSpace;

		m_TrackRect.left = m_Rect.left + m_LeftSpace;
		m_TrackRect.right = m_Rect.right - m_RightSpace;
		m_HeadRect.bottom = m_HeadRect.top + m_Page.m_Height;
		m_HeadRect.right = m_HeadRect.left + m_Page.m_Width;
		CDrawObj::m_SelectMode = nSelectMode;
		SetModifiedFlag();
		return TRUE;
	}
	return FALSE;
}
// CLogPlotDoc 序列化
void CLogPlotDoc::Serialize(CArchive& ar)
{
	char m_AppName[] = "LOGPLOT   ";
	char Version[]="3.5      ";
	CString m_MethodName;
	m_Flag = 1;
	char tmp[100];
	strcpy(tmp,"Copyright (C) 2000-2005 SICHUAN LOG Company All rights reserved.");
	if(ar.IsStoring())
	{
		ar.Write(m_AppName,10);
		ar.Write(Version,6);
		ar << m_DepthScale;
		ar << m_DepthUnit;
		ar << m_Rect;
		ar << m_TrackRect;
		ar << m_paperColor;
		ar << m_PlotSdep;
		ar << m_PlotEdep;
		ar << m_LeftSpace;
		ar << m_RightSpace;
		ar << m_TopSpace;
		ar << m_BottomSpace;
		ar << m_HeadRect;     
		ar << m_HeadTopSpace;
		ar << m_HeadBottomSpace;
		ar << m_HoriArrangleMode;
		ar << m_bVertAlignDepth; 
		ar << m_bVertAlignHead; 
		ar << m_bVertAlignTop; 
		ar << bHoriAutoArrangle;
		ar << bVertAutoAlign;
		ar << m_xGrid;
		ar << m_yGrid;
		ar << m_bChildTrackHeadAlignMax;
		ar << m_bChildTrackHeadHeightAuto;
		if(m_bPlotHead) 
			tmp[0]='1';
		else
			tmp[0]='0';
		if(m_bPlotResultList) 
			tmp[1]='1';
		else
			tmp[1]='0';
		if(m_bPlotTrack) 
			tmp[2]='1';
		else
			tmp[2]='0';
		if(m_PlotResultModel==1)
			tmp[3]='1';
		else if(m_PlotResultModel==3)
			tmp[3]='3';
		else if(m_PlotResultModel==4)
			tmp[3]='4';
		else if(m_PlotResultModel==5)
			tmp[3]='5';
		else if(m_PlotResultModel==6)
			tmp[3]='6';
		else tmp[3]='2';
		tmp[4]='1';
		if(m_DrawFIDHeadMode==1)
			tmp[99]='1';
		else	tmp[99]='0';
		if(m_FillLithGround)
			tmp[98]='0';
		else	tmp[98]='1';
		ar.Write(tmp,100);
	}
	else                 
	{
		ar.Read(m_AppName,10);
		m_AppName[10] = '\0';
		if(!strncmp(m_AppName,"LOGPLOT   ",10)) 
			m_Flag = 1;
		else if(!strncmp(m_AppName,"LOGPROCESS",10))
			m_Flag = 2;
		else
		{
			m_Flag = 0;
			AfxMessageBox("不能识别该文件格式",MB_OK | MB_ICONEXCLAMATION);
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
		if(tmp[0]=='0') 
			m_bPlotHead=FALSE;
		else
			m_bPlotHead=TRUE;
		if(tmp[1]=='0') 
			m_bPlotResultList=FALSE;
		else
			m_bPlotResultList=TRUE;
		if(tmp[2]=='0') 
			m_bPlotTrack=FALSE;
		else
			m_bPlotTrack=TRUE;
		
		if(tmp[3]=='1')
			m_PlotResultModel=1;
		else if(tmp[3]=='3')
			m_PlotResultModel=3;
		else if(tmp[3]=='4')
			m_PlotResultModel=4;
		else if(tmp[3]=='5')
			m_PlotResultModel=5;
		else if(tmp[3]=='6')
			m_PlotResultModel=6;
		else m_PlotResultModel=2;
		if(tmp[99]=='1')
			m_DrawFIDHeadMode=1;
		else
			m_DrawFIDHeadMode=0;		
		if(tmp[98]=='1')
			m_FillLithGround=FALSE;
		else	m_FillLithGround=TRUE;
		//网页预览不绘图头和解释表
		m_bPlotHead =m_bPlotResultList=FALSE;
		m_bPlotTrack=TRUE;
	} 
	if (m_DepthUnit == UNIT_M) m_DepC = m_DepthScale * 2.54 / 10000.;
	else m_DepC = m_DepthScale  / 1200.;
	COleDocument::Serialize(ar);
	
	m_TrackList.Serialize(ar);
	m_RemObjList.Serialize(ar);
	m_HeadList.Serialize(ar);
	if(!ar.IsStoring())
		SetResultListRect();
}

void CLogPlotDoc::Draw(CView* pView,CDC* pDC)
{
	if(m_bRunFirst)
	{
		if(m_pGuage==NULL)
		{
			m_ObjNum=GetObjCount();
			m_pGuage = new CProgressDlg;
			m_pGuage->Create();
			m_pGuage->SetPropMesg("查询网络井资源...");
			m_pGuage->SetRange(0,m_ObjNum);
		}
		m_DrawNo=0;
		m_pGuage->SetPos(m_DrawNo);
	}
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
		if((pObj!=NULL)&&(!pObj->IsKindOf(RUNTIME_CLASS(CDepSect)))) 
			pObj->Draw(pView,pDC);
	}          
	if(m_bRunFirst)	
		m_bRunFirst=FALSE;
	if(m_pGuage!=NULL)
	{
		delete m_pGuage;
		m_pGuage=NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//  Coordinate Convert
//
void CLogPlotDoc::DPtoLP(CDC* pDC,LPPOINT lpPoints, int nCount)
{
	pDC->DPtoLP(lpPoints,nCount);
	PPtoLP(lpPoints,nCount);
}

void CLogPlotDoc::DPtoLP(CDC* pDC,LPRECT lpRect)
{
	pDC->DPtoLP(lpRect);
	PPtoLP(lpRect);
}

void CLogPlotDoc::LPtoDP(CDC* pDC,LPPOINT lpPoints, int nCount)
{
	LPtoPP(lpPoints,nCount);
	pDC->LPtoDP(lpPoints,nCount);
}

void CLogPlotDoc::LPtoDP(CDC* pDC,LPRECT lpRect)
{
	LPtoPP(lpRect);
	pDC->LPtoDP(lpRect);
}

void CLogPlotDoc::PPtoLP(LPPOINT lpPoints, int nCount)
{
	if (m_WinOrg == CPoint(0,0)) return;
	for (int i=0; i<nCount; i++)
		lpPoints[i].y += m_WinOrg.y;
}

void CLogPlotDoc::PPtoLP(LPRECT lpRect)
{
	if (m_WinOrg == CPoint(0,0)) return;
	lpRect->top += m_WinOrg.y;
	lpRect->bottom += m_WinOrg.y;
}

void CLogPlotDoc::LPtoPP(LPPOINT lpPoints, int nCount)
{
	if (m_WinOrg != CPoint(0,0))
	{
		for (int i=0; i<nCount; i++)
			lpPoints[i].y -= m_WinOrg.y;
	}
}


void CLogPlotDoc::LPtoPP(LPRECT lpRect)
{
	BOOL bEQU = (lpRect->top == lpRect->bottom);
	if (m_WinOrg != CPoint(0,0)) 
	{
		lpRect->top -= m_WinOrg.y;
		lpRect->bottom -= m_WinOrg.y;
	}
	if ((lpRect->top == lpRect->bottom) && (!bEQU)) lpRect->bottom += 5;
}


///////////////////////////////////////////////////////////////////////////////
//
// Track Arrangle Operations
//
CTrackObj* CLogPlotDoc::GetLeftTrackObj()
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

CTrackObj* CLogPlotDoc::GetRightTrackObj()
{                       
	int xr = 0;  
	CTrackObj* pObj1 = NULL;
	CDrawObj* pObj;
	POSITION pos = m_TrackList.GetHeadPosition();
	while(pos != NULL)
	{
		pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		if((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetRect().right > xr) && (pObj->GetParentType() == 0))
		{
			pObj1 = (CTrackObj*)pObj; 
			xr = pObj->GetRect().right;
		}
	}
	return pObj1;
}

CTrackObj* CLogPlotDoc::GetMaxLeftTrackObj()
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

CTrackObj* CLogPlotDoc::GetMinRightTrackObj()
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

CTrackObj* CLogPlotDoc::GetMaxWidthTrackObj()
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

CTrackObj* CLogPlotDoc::GetMinWidthTrackObj()
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

CTrackObj* CLogPlotDoc::GetMaxHeightTrackObj()
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

CTrackObj* CLogPlotDoc::GetMinHeightTrackObj()
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


CRect CLogPlotDoc::GetUnionRect()
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
	pos = m_RemObjList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_RemObjList.GetNext(pos);
		if (pObj != NULL) 
		{
			if (rect.IsRectEmpty()) rect = pObj->GetMaxRect();
			else rect |= pObj->GetMaxRect();
		}	
	}                          
	return rect;
}               

CRect CLogPlotDoc::GetUnionRectNoRem()
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
CRect CLogPlotDoc::GetTrackUnionRect()
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

CRect CLogPlotDoc::GetTopRemObjUnionRect(int y)
{
	CRect rect1;
	CRect rect(0,y,0,y);
	POSITION pos = m_RemObjList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_RemObjList.GetNext(pos);
		if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CRemDrawObj)))
			&& (!(pObj->IsKindOf(RUNTIME_CLASS(CResultListObj)))))
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

CRect CLogPlotDoc::GetBottomRemObjUnionRect(int y)
{
	CRect rect1;
	CRect rect(0,y,0,y);
	POSITION pos = m_RemObjList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_RemObjList.GetNext(pos);
		if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CRemDrawObj)))
			&& (!(pObj->IsKindOf(RUNTIME_CLASS(CResultListObj)))))
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

int CLogPlotDoc::GetTrackTotalWidth()
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

int CLogPlotDoc::GetTrackCount()
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
void CLogPlotDoc::DoVertAlign()
{
	if (m_bVertAlignDepth) DoVertAlignDepth();
	if (m_bVertAlignTop) DoVertAlignTop();
	SetSize();
}

void CLogPlotDoc::DoVertAlignHead()
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

void CLogPlotDoc::DoVertAlignDepth()
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

void CLogPlotDoc::DoVertAlignTop()
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

void CLogPlotDoc::DoHoriArrangle(int nArrangleMode)
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

/////////////////////////////////////////////////////////////////////////////////
//
// Track Size Operations
//
void CLogPlotDoc::SetChildTrackWidth()
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
		SetModifiedFlag();
		UpdateAllViews(m_pView,HINT_UPDATE_WINDOW);
		bEnableChildSizeMessage = TRUE;			// 允许子道消息	
	}
}

void CLogPlotDoc::SetSelTrackWidth(CTrackObj* pObj)
{
	CRect rect;
	UINT m_MinWidth,m_MaxWidth;
	m_MinWidth = 0;
	m_MaxWidth = m_Rect.Width();
	if (m_MinWidth > m_MaxWidth) m_MinWidth = m_MaxWidth;
	CWidthDialog m_Dialog;
	m_Dialog.m_MinWidth = (int)(m_MinWidth*0.254+0.5);
	m_Dialog.m_MaxWidth = (int)(m_MaxWidth*0.254+0.5);
	rect = pObj->GetRect();
	m_Dialog.m_Width=(int)(rect.Width ()*0.254+0.5);
	if (m_Dialog.DoModal() == IDOK)
	{
		int w = (int)(m_Dialog.m_Width / 0.254+0.5);
		bEnableChildSizeMessage = FALSE;		// 禁止子道消息		
		rect.right = rect.left + w;
		pObj->SetRect(rect);
		if (bHoriAutoArrangle) DoHoriArrangle();
		SetModifiedFlag();
		UpdateAllViews(m_pView,HINT_UPDATE_WINDOW);
		bEnableChildSizeMessage = TRUE;			// 允许子道消息	
	}
}

void CLogPlotDoc::SetChildTrackTitleHeight()
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
		SetModifiedFlag();
		UpdateAllViews(m_pView,HINT_UPDATE_WINDOW);
	}
}

void CLogPlotDoc::DrawChildTrackTitle(BOOL bShow)
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
	SetModifiedFlag();
	UpdateAllViews(m_pView,HINT_UPDATE_WINDOW);
}

void CLogPlotDoc::SetChildTrackHeadHeightAlignMax(BOOL bMax)
{
	if (m_bChildTrackHeadAlignMax != bMax)  
	{
		m_bChildTrackHeadAlignMax = bMax;
		if (bMax) 
		{
			AlignChildTrackHeadHeight();
			SetModifiedFlag();
			UpdateAllViews(m_pView,HINT_UPDATE_WINDOW);
		}
	}
}

void CLogPlotDoc::SetChildTrackHeadHeightAuto(BOOL bAuto)
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
		if(m_bChildTrackHeadAlignMax) AlignChildTrackHeadHeight();
		else 
		{
			if(m_bVertAlignTop && bVertAutoAlign) DoVertAlignTop();
				SetSize();
		}
		SetModifiedFlag();


		UpdateAllViews(m_pView,HINT_UPDATE_WINDOW);
	}
	bEnableChildSizeMessage = TRUE;	// 允许子道消息	
}

void CLogPlotDoc::AutoChildTrackHeadHeight()
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

void CLogPlotDoc::AlignChildTrackHeadHeight()
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

//////////////////////////////////////////////////////////////////////////////////
//
// Child Message
//
void CLogPlotDoc::OnChildTrackHeadHeightChange(CDrawObj* pObj)
{
	ASSERT(pObj != NULL);
	if (!bEnableChildSizeMessage) return;   
	if (pObj->GetParentType() == 0) 
	{
		AutoChildTrackHeadHeight();
		AlignChildTrackHeadHeight();
	}
}

void CLogPlotDoc::OnChildHeightChange(CDrawObj* pObj)
{
	ASSERT(pObj != NULL);
	if (!bEnableChildSizeMessage) return;  
	if (IsHeadObj(pObj)) 
		return;
	if (pObj->IsKindOf(RUNTIME_CLASS(CRemDrawObj))) SetSize();
	else if (pObj->GetParentType() == 0) 
	{
		if (m_bVertAlignTop) DoVertAlignTop();
		SetSize();
	}
}

void CLogPlotDoc::OnChildWidthChange(CDrawObj* pObj)
{
	ASSERT(pObj != NULL);
	if (!bEnableChildSizeMessage) return;   
	if ((bHoriAutoArrangle) && (pObj->GetParentType() == 0)) DoHoriArrangle(m_HoriArrangleMode);
}

void CLogPlotDoc::OnChildPosChange(CDrawObj* pObj)
{
	ASSERT(pObj != NULL);
	if (!bEnableChildSizeMessage) return;  
	if (IsHeadObj(pObj)) 
		return;
	if (pObj->IsKindOf(RUNTIME_CLASS(CRemDrawObj))) SetSize();
	else if (pObj->GetParentType() == 0)
	{
		if (bHoriAutoArrangle) DoHoriArrangle(m_HoriArrangleMode);
		DoVertAlign();
	}
}

void CLogPlotDoc::OnChildRemove(CDrawObj* pObj, int nParentType)
{
	ASSERT(pObj != NULL);
	if (!bEnableChildSizeMessage) return;   
	if (IsHeadObj(pObj)) 
		return;
	if (pObj->IsKindOf(RUNTIME_CLASS(CRemDrawObj))) SetSize();
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

////////////////////////////////////////////////////////////////////////////////
//
// Size Operations	
//
double CLogPlotDoc::YCoorToDepth(int y)
{
	return (double)((y - m_TrackRect.top) * 2.54 * m_DepthScale /10000 + m_PlotSdep);
}

int CLogPlotDoc::DepthToYCoor(double depth)
{
	return (int) ((depth - m_PlotSdep) * 100 * 100 / 2.54 / m_DepthScale + m_TrackRect.top);
}

void CLogPlotDoc::SetDepth(double sdep, double edep)
{
	int DaltaY=GetLeftTrackObj ()->GetClientRect ().top -m_TopSpace;
	int DaltaH=DepthToYCoor(m_PlotSdep)-DepthToYCoor(sdep);
	int DaltaHB=DepthToYCoor(edep)-DepthToYCoor(m_PlotEdep);
	m_PlotSdep = sdep; m_PlotEdep = edep;
	POSITION pos = m_RemObjList.GetTailPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_RemObjList.GetPrev(pos);
		if (pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj)))
			((CFIDDrawObj*)pObj)->SetDepth(m_PlotSdep,m_PlotEdep);
		else
		{		
			CRemDrawObj* pRemObj=(CRemDrawObj*)pObj;			
			CRect rect=pRemObj->m_Rect;
			if(rect.bottom >m_TrackRect .bottom)
				continue;		
			if(rect.top <m_TrackRect.top+DaltaY)
				continue;
			if(rect.bottom >m_TrackRect .bottom+DaltaHB)
			{
				RemoveObj(pRemObj);
				continue;
			}
			rect.OffsetRect (0,DaltaH);			
			if(rect.top <m_TrackRect.top+DaltaY)
				RemoveObj(pRemObj);
			else
				pRemObj->SetRect (rect);
		}
	}                                                       
	pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		CTrackObj* pObj = (CTrackObj*)m_TrackList.GetNext(pos);
		pObj->SetDepth(m_PlotSdep,m_PlotEdep);
	} 
	if (bVertAutoAlign) DoVertAlign();
	else SetSize();
} 

void CLogPlotDoc::SetDepthNoRem(double sdep, double edep)
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
} 

void CLogPlotDoc::SetDepC(double depc, int flag)
{
	m_DepC = depc;
	POSITION pos = m_RemObjList.GetTailPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_RemObjList.GetPrev(pos);
		if (pObj->IsKindOf(RUNTIME_CLASS(CRemDrawObj)))
		{
			if ((pObj->GetRect().top < m_TrackRect.top) ||
				(pObj->GetRect().bottom > m_TrackRect.bottom)) 
				continue;
		}
		pObj->OnChangeDepC(depc,flag);
	}                                                       
	pos = m_TrackList.GetHeadPosition();
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
				if (pObj != NULL) 
					pObj->SetRect(pObj->GetRect()+ point);
			}
			pos = m_RemObjList.GetHeadPosition();
			while (pos != NULL)
			{
				CDrawObj* pObj = (CDrawObj*)m_RemObjList.GetNext(pos);
				pObj->SetRect(pObj->GetRect()+point);
				if (pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj)))
					((CFIDDrawObj*)pObj)->SetHeadRect(((CFIDDrawObj*)pObj)->GetHeadRect()+point);
			}                                                       
		}
	}                                    
	SetSize();
	UpdateAllViews(m_pView,HINT_UPDATE_SIZE);
}

void CLogPlotDoc::SetDepthScale(double scale) 
{
	double depc;
	m_DepthScale = scale;
	if (m_DepthUnit == UNIT_M)
		depc = scale * 2.54 / 10000.;
	else
		depc = scale / 1200.;
	SetDepC(depc,0);
}


CSize CLogPlotDoc::ComputerPrintSize()
{
	int h = 0;
	if (m_bPlotHead) 
	{
		h = m_HeadRect.Height();
		if (!m_bReserveSpace) h -= m_HeadBottomSpace;
	}
	if(m_bPlotTopRem || m_bPlotBottomRem || m_bPlotTrack || m_bPlotResultList)
	{
		if (m_bReserveSpace)
			h += m_TopSpace;
		h += m_BottomSpace;
	}
	if (m_bPlotTopRem) h += m_TrackRect.top - m_TopSpace;
	if (m_bPlotTrack) h += m_TrackRect.Height();
	int ht = ComputerResultListHeight();
	if (m_bPlotResultList) h += ht;
	if (m_bPlotBottomRem) h += m_Rect.bottom - m_BottomSpace - ht - m_TrackRect.bottom;
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

void CLogPlotDoc::ComputePageSize()
{
	CPrintDialog dlg(FALSE);
	if (!AfxGetApp()->GetPrinterDeviceDefaults(&dlg.m_pd))	return;
	// GetPrinterDC returns a HDC so attach it
	CDC dc;
	HDC hDC= dlg.CreatePrinterDC();
	if (hDC==NULL) return;
	dc.Attach(hDC);

	// Get the size of the page in loenglish
	CSize size(MulDiv(dc.GetDeviceCaps(HORZSIZE),1000,254),
		       MulDiv(dc.GetDeviceCaps(VERTSIZE),1000,254));
	CSize m_PageSize = size;
	if (m_LeftSpace + m_RightSpace > size.cx)
		size.cx = m_LeftSpace + m_RightSpace;
	m_Rect.right =size.cx -1;
	m_TrackRect.left = m_Rect.left + m_LeftSpace;
	m_TrackRect.right = m_Rect.right- m_RightSpace;
	m_HeadRect.right = m_Rect.right;
	m_HeadRect.left = m_Rect.left;
}

void CLogPlotDoc::GetDepthRangle(double& minDepth,double& maxDepth)
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

void CLogPlotDoc::SetSize()
{
	BOOL bChange = FALSE;
	BOOL bChangeSize = FALSE;
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
		bChange = TRUE;
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
		pos = m_RemObjList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*) m_RemObjList.GetNext(pos);
			if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CRemDrawObj))) 
				&& (pObj->m_Rect.bottom > y))
				pObj->SetRect(pObj->GetRect()+ point);
		}
		m_TrackRect.bottom = rect.bottom;
	}
	m_BottomRect = GetBottomRemObjUnionRect(m_TrackRect.bottom);
	CRect m_ResultListRect = ComputerResultListRect(m_BottomRect.bottom);
	if(m_ResultListRect.top != m_BottomRect.bottom)
	{
		point.y = m_BottomRect.bottom - m_ResultListRect.top; 
		pos = m_RemObjList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*) m_RemObjList.GetNext(pos);
			if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CResultListObj)))) 
				pObj->SetRect(pObj->GetRect()+ point);
		}
		m_ResultListRect += point;
	}
	CDrawObj::m_SelectMode = nSelectMode;
	if (m_Rect.bottom - m_ResultListRect.bottom != m_BottomSpace) 
	{
		m_Rect.bottom = m_ResultListRect.bottom + m_BottomSpace;
		bChangeSize = TRUE;
		bChange = TRUE;
	}	
	if(bChangeSize) 
		UpdateAllViews(m_pView,HINT_UPDATE_SIZE);
	else 
		UpdateAllViews(m_pView,HINT_UPDATE_WINDOW);
	if (bChange) SetModifiedFlag();          
}

void CLogPlotDoc::SetSizeNoRem()
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

CRect CLogPlotDoc::ComputerResultListRect(int y)
{
	CRect rect1;
	CRect rect(0,y,0,y);
	if(m_bPlotResultList)
	{
		POSITION pos = m_RemObjList.GetHeadPosition();
		while(pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*)m_RemObjList.GetNext(pos);
			if((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CResultListObj))))
			{
				rect1 = pObj->GetRect();
				if(rect.IsRectEmpty()) 
					rect = rect1;
				else rect |= rect1;  
			}	
		}     
	}
	return rect;
}               

int CLogPlotDoc::ComputerResultListHeight()
{
	int h=0;
	if(m_bPlotResultList)
	{
		POSITION pos = m_RemObjList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*) m_RemObjList.GetNext(pos);
			if((pObj != NULL)&&(pObj->IsKindOf(RUNTIME_CLASS(CResultListObj))))
				h += pObj->GetRect().Height();
		}
	}
	return h;
}

void CLogPlotDoc::SetResultListRect()
{
	POSITION pos = m_RemObjList.GetHeadPosition();
	CRect rect = GetBottomRemObjUnionRect(m_TrackRect.bottom);
	rect.top = rect.bottom;
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*) m_RemObjList.GetNext(pos);
		if((pObj != NULL) 
			&&(pObj->IsKindOf(RUNTIME_CLASS(CResultListObj)))
			&&m_bPlotResultList)
		{
			pObj->SetRect(rect);
			rect.top = pObj->GetRect().bottom;
		}
	}
	if (m_Rect.bottom - rect.top != m_BottomSpace) 
	{
		m_Rect.bottom = rect.top + m_BottomSpace;
		UpdateAllViews(m_pView,HINT_UPDATE_SIZE);
	}
	SetModifiedFlag();          
}

//////////////////////////////////////////////////////////////////////////////
//
//Data Update Message
//
void CLogPlotDoc::OnWellDataUpdate(CString strName)
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
	if (m_List.GetCount()>0)
	{
		pos = m_List.GetHeadPosition();
		while (pos != NULL)
		{
			pObj = (CDrawObj*)m_List.GetNext(pos);
			((CFIDDrawObj*)pObj)->bFlush = TRUE;
			((CFIDDrawObj*)pObj)->bErrorFID = FALSE;
		}                   
		UpdateAllViews(m_pView,HINT_UPDATE_WINDOW);
	}
	m_List.RemoveAll();
}

void CLogPlotDoc::OnCurveDataUpdate(CString strName)
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
	if(m_List.GetCount()>0)
	{
		pos = m_List.GetHeadPosition();
		while (pos != NULL)
		{
			pObj = (CDrawObj*)m_List.GetNext(pos);
			((CFIDDrawObj*)pObj)->bFlush = TRUE;
			((CFIDDrawObj*)pObj)->bErrorFID = FALSE;
			pObj->Invalidate(m_pView);
		}                   
	}
	m_List.RemoveAll();
}

BOOL CLogPlotDoc::MatchWellName(CString strName, CString NewstrPathName)
{
	BOOL FindMatch=FALSE;
	CTrackObj* pObj;
	POSITION pos= m_TrackList.GetHeadPosition();
	while(pos != NULL)
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
									while(pos3 != NULL)
									{	
										pObj3 = (CTrackObj*)pObj2->m_ObjList.GetNext(pos3);
										if(pObj3->MatchWellName(strName,NewstrPathName))
										{
											SetModifiedFlag ();
											FindMatch=TRUE;
										}
									} 
								}
								if(pObj2->MatchWellName(strName,NewstrPathName))
								{
									SetModifiedFlag ();
									FindMatch=TRUE;
								}
							}
						}
						if(pObj1->MatchWellName(strName,NewstrPathName))
						{
							SetModifiedFlag ();
							FindMatch=TRUE;
						}
					}
				}
				if(pObj0->MatchWellName(strName,NewstrPathName))
				{
					SetModifiedFlag ();
					FindMatch=TRUE;
				}
			}
		}
		if(pObj->MatchWellName(strName,NewstrPathName))
		{
			SetModifiedFlag ();
			FindMatch=TRUE;
		}
	}
	return FindMatch;
}

void CLogPlotDoc::MatchListFile(CString WellName)
{
	//得到该口井的路径
	CString szDir;
	CGeoFIDIO::GetPath(WellName,szDir.GetBuffer(512));
	szDir.ReleaseBuffer();

	CTrackObj* pObj;
	CFIDDrawObj* pCurve;
	POSITION pos;
	CString Name;int FilePos;
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
								if(pCurve->IsKindOf (RUNTIME_CLASS(CResultObj))
									||pCurve->IsKindOf (RUNTIME_CLASS(CDSCObj))
									||pCurve->IsKindOf (RUNTIME_CLASS(CYieldObj))
									||pCurve->IsKindOf (RUNTIME_CLASS(CLthNoteObj))
									||pCurve->IsKindOf (RUNTIME_CLASS(CLthObj))
									||(pCurve->IsKindOf (RUNTIME_CLASS(CStratumObj)))
									||(pCurve->IsKindOf (RUNTIME_CLASS(CMarkVectorObj)))
									||(pCurve->IsKindOf (RUNTIME_CLASS(CMarkFreqObj))))
								{
									Name=pCurve->m_strPathName;
									FilePos=Name.ReverseFind ('\\');									
									if(FilePos!=-1)
										Name=szDir+Name.Mid (FilePos+1);
									else
										Name=szDir+Name;
									pCurve->m_strPathName=Name;
									pCurve->bFlush = TRUE;
									pCurve->bErrorFID = FALSE;
									pCurve->Invalidate(m_pView);
									SetModifiedFlag ();
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
					if(pCurve->IsKindOf (RUNTIME_CLASS(CResultObj))
						||pCurve->IsKindOf (RUNTIME_CLASS(CDSCObj))
						||pCurve->IsKindOf (RUNTIME_CLASS(CYieldObj))
						||pCurve->IsKindOf (RUNTIME_CLASS(CLthNoteObj))
					    ||pCurve->IsKindOf (RUNTIME_CLASS(CLthObj))
					    ||(pCurve->IsKindOf (RUNTIME_CLASS(CStratumObj)))
					    ||(pCurve->IsKindOf (RUNTIME_CLASS(CMarkVectorObj)))
					    ||(pCurve->IsKindOf (RUNTIME_CLASS(CMarkFreqObj))))
					{
						Name=pCurve->m_strPathName;
						FilePos=Name.ReverseFind ('\\');						
						if(FilePos!=-1)
							Name=szDir+Name.Mid (FilePos+1);
						else
							Name=szDir+Name;
						pCurve->m_strPathName=Name;
						pCurve->bFlush = TRUE;
						pCurve->bErrorFID = FALSE;
						pCurve->Invalidate(m_pView);
						SetModifiedFlag ();
					}
				}
			}
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
	CRemDrawObj *pRenList=NULL;
	CResultListObj *pList=NULL;
	pos = m_RemObjList.GetHeadPosition();
	while(pos != NULL)
	{
		pRenList= (CRemDrawObj*)m_RemObjList.GetNext(pos);
		if(pRenList!=NULL)
		{
			if(pRenList->IsKindOf(RUNTIME_CLASS(CResultListObj)))
			{
				pList=(CResultListObj*)pRenList;
				Name=pList->m_WellName;
				FilePos=Name.ReverseFind ('\\');
				if(FilePos!=-1)
					Name=szDir+Name.Mid (FilePos+1);
				else
					Name=szDir+Name;
				pList->m_WellName=Name;
				pList->ReadFile();
			}
		}
	}
	SetResultListRect();
}

void CLogPlotDoc::MatchCurrentWorkSpace()
{
	CString PathStr=GetCurrentUserPath();
	CTrackObj* pObj;
	CFIDDrawObj* pCurve;
 
	POSITION pos = m_TrackList.GetHeadPosition();
	BeginWaitCursor();
	int xxx;CString DirWellStr;
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
							while(pos2 != NULL)
							{
								pObj2 = (CTrackObj*)pObj1->m_ObjList.GetNext(pos2);
								POSITION posFID = pObj2->m_ObjList.GetHeadPosition();
								while (posFID != NULL)
								{
									pCurve = (CFIDDrawObj*)pObj2->m_ObjList.GetNext(posFID);
									if ((pCurve != NULL) && (pCurve->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) 
										&& (pCurve->GetParentType() == 0)) 
									{
										DirWellStr=pCurve->m_strPathName;
										xxx=DirWellStr.ReverseFind('#');
										if(xxx!=-1)
										{
											pCurve->m_strPathName=PathStr+DirWellStr.Mid (xxx);
											pCurve->bFlush = TRUE;
											pCurve->bErrorFID = FALSE;
											pCurve->Invalidate(m_pView);
											SetModifiedFlag ();
										}
									}
								}
							}
						}

						POSITION posFID1 = pObj1->m_ObjList.GetHeadPosition();
						while (posFID1 != NULL)
						{
							pCurve = (CFIDDrawObj*)pObj1->m_ObjList.GetNext(posFID1);
							if ((pCurve != NULL) && (pCurve->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) 
								&& (pCurve->GetParentType() == 0)) 
							{
								DirWellStr=pCurve->m_strPathName;
								xxx=DirWellStr.ReverseFind('#');
								if(xxx!=-1)
								{
									pCurve->m_strPathName=PathStr+DirWellStr.Mid (xxx);
									pCurve->bFlush = TRUE;
									pCurve->bErrorFID = FALSE;
									pCurve->Invalidate(m_pView);
									SetModifiedFlag ();
								}
							}
						}
					}
				}
				POSITION posFID2 = pObj0->m_ObjList.GetHeadPosition();
				while (posFID2 != NULL)
				{
					pCurve = (CFIDDrawObj*)pObj0->m_ObjList.GetNext(posFID2);
					if ((pCurve != NULL) && (pCurve->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) 
						&& (pCurve->GetParentType() == 0)) 
					{
						DirWellStr=pCurve->m_strPathName;
						xxx=DirWellStr.ReverseFind('#');
						if(xxx!=-1)
						{
							pCurve->m_strPathName=PathStr+DirWellStr.Mid (xxx);
							pCurve->bFlush = TRUE;
							pCurve->bErrorFID = FALSE;
							pCurve->Invalidate(m_pView);
							SetModifiedFlag ();
						}
					}
				}
			}
		}
		POSITION posFID3 = pObj->m_ObjList.GetHeadPosition();
		while (posFID3 != NULL)
		{
			pCurve = (CFIDDrawObj*)pObj->m_ObjList.GetNext(posFID3);
			if ((pCurve != NULL) && (pCurve->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) 
				&& (pCurve->GetParentType() == 0)) 
			{
				DirWellStr=pCurve->m_strPathName;
				xxx=DirWellStr.ReverseFind('#');
				if(xxx!=-1)
				{
					pCurve->m_strPathName=PathStr+DirWellStr.Mid (xxx);
					pCurve->bFlush = TRUE;
					pCurve->bErrorFID = FALSE;
					pCurve->Invalidate(m_pView);
					SetModifiedFlag ();
				}
			}
		}
		CString Name=pObj->m_LayerFileName ;
		if(Name!="")
		{
			int FilePos=Name.ReverseFind('#');
			if(FilePos!=-1)
			{
				Name=PathStr+Name.Mid (FilePos);
				pObj->m_LayerFileName =Name;
				pObj->m_LayerFlush =TRUE;
				SetModifiedFlag ();
			}
		}
	}
	
	pos = m_RemObjList.GetHeadPosition();
	while (pos != NULL)
	{	
		CDrawObj* pDraw = (CDrawObj*)m_RemObjList.GetNext(pos);
		if(pDraw!=NULL)
		{	
			if(pDraw->IsKindOf (RUNTIME_CLASS(CResultListObj)))
			{
				CResultListObj* pRes=(CResultListObj*) pDraw;
				if(pRes!=NULL)
				{
					CString Name=pRes->m_WellName;
					int FilePos=Name.ReverseFind('#');
					if(FilePos!=-1)
					{
						Name=PathStr+Name.Mid (FilePos);
						pRes->m_WellName=Name;
						pRes->ReadFile();	
						SetResultListRect();
						SetModifiedFlag ();
					}
				}
			}
		}
	}

	UpdateAllViews(m_pView,HINT_UPDATE_WINDOW);
	EndWaitCursor();
}

//////////////////////////////////////////////////////////////////////////////
//
// Object move Operations
//
BOOL CLogPlotDoc::MatchAllObject()
{
	if(m_ProjectID<1) return FALSE;
	BeginWaitCursor();
	DocToTemplate();
	
	BOOL flag=FALSE;
	CGeoStorage Storage;
	if(Storage.InitData(m_szSever)) 
	{
		if(Storage.GetData(m_ProjectID))
		{
			if(Storage.m_StorageKind=="测井项目")
			{
				m_szProjectName=Storage.m_StorageName;
				DWORD WellID=Storage.m_UpperID;
				if(Storage.GetData(WellID))
				{
					m_szWellName=Storage.m_StorageName;
					CString szDir="@GeoDB\\"+m_szWellName+"\\"+m_szProjectName+"\\";
					CString szProject=m_szProjectName;
					if(szProject.GetAt(0)=='#')
						szProject=szProject.Mid(1);
					szDir=szDir+szProject+".FID";
					flag=MatchWellName("GR", szDir);
				}
			}
		}
		Storage.Disconnect();
	}
	m_bRunFirst=flag;
	EndWaitCursor();
	return flag;
}

void CLogPlotDoc::DocToTemplate()
{
	int FilePos;
	CString Name;
	CDrawObj* pObj;
	CTrackObj* pTrack;
	CFIDDrawObj* pCurve;
	POSITION pos;
	char CurveName[64];
	pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_TrackList.GetNext(pos);
		if(pObj->IsKindOf(RUNTIME_CLASS(CTrackObj)))
		{
			pTrack = (CTrackObj*) pObj;
			POSITION pos0 = pTrack->m_ObjList.GetHeadPosition();
			CDrawObj* pObj0;
			while (pos0 != NULL)
			{
				pObj0 = (CDrawObj*)pTrack->m_ObjList.GetNext(pos0);
				if(pObj0->IsKindOf(RUNTIME_CLASS(CTrackObj)))
				{
					CTrackObj* pTrack1 = (CTrackObj*) pObj0;
					POSITION pos1 = pTrack1->m_ObjList.GetHeadPosition();
					CDrawObj* pObj1;
					while (pos1 != NULL)
					{
						pObj1 = (CDrawObj*)pTrack1->m_ObjList.GetNext(pos1);
						if(pObj1->IsKindOf(RUNTIME_CLASS(CFIDDrawObj)))
						{		
							pCurve=(CFIDDrawObj*) pObj1;
							if((pCurve->IsKindOf(RUNTIME_CLASS(CResultObj)))
								||(pCurve->IsKindOf(RUNTIME_CLASS(CDSCObj)))
								||(pCurve->IsKindOf(RUNTIME_CLASS(CYieldObj)))
								||(pCurve->IsKindOf(RUNTIME_CLASS(CLthNoteObj)))
								||(pCurve->IsKindOf(RUNTIME_CLASS(CLthObj)))
								||(pCurve->IsKindOf(RUNTIME_CLASS(CStratumObj)))
								||(pCurve->IsKindOf(RUNTIME_CLASS(CMarkVectorObj)))
								||(pCurve->IsKindOf (RUNTIME_CLASS(CMarkFreqObj))))
							{
									Name=pCurve->m_strPathName;
									FilePos=Name.ReverseFind ('\\');
									if(FilePos!=-1)
										Name=Name.Mid (FilePos+1);
									pCurve->m_strPathName=Name;
							}
							else
							{
								CGeoFIDIO::GetFIDName(pCurve->m_strPathName,CurveName);
								pCurve->m_strPathName="TEMPLATE.";
								pCurve->m_strPathName+=CurveName;
							}
						}
					}
				}
				if(pObj0->IsKindOf(RUNTIME_CLASS(CFIDDrawObj)))
				{
					pCurve=(CFIDDrawObj*) pObj0;
					if((pCurve->IsKindOf(RUNTIME_CLASS(CResultObj)))
						||(pCurve->IsKindOf(RUNTIME_CLASS(CDSCObj)))
						||(pCurve->IsKindOf(RUNTIME_CLASS(CYieldObj)))
						||(pCurve->IsKindOf(RUNTIME_CLASS(CLthNoteObj)))
						||(pCurve->IsKindOf(RUNTIME_CLASS(CLthObj)))
						||(pCurve->IsKindOf(RUNTIME_CLASS(CStratumObj)))
						||(pCurve->IsKindOf(RUNTIME_CLASS(CMarkVectorObj)))
						||(pCurve->IsKindOf (RUNTIME_CLASS(CMarkFreqObj))))
					{
							Name=pCurve->m_strPathName;
							FilePos=Name.ReverseFind ('\\');
							if(FilePos!=-1)
								Name=Name.Mid (FilePos+1);
							pCurve->m_strPathName=Name;
					}
					else
					{
						CGeoFIDIO::GetFIDName(pCurve->m_strPathName,CurveName);
						pCurve->m_strPathName="TEMPLATE.";
						pCurve->m_strPathName+=CurveName;
					}
				}
			}
		}
		if(pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj)))
		{
			pCurve=(CFIDDrawObj*) pObj;
			pCurve->m_strPathName="";
		}
	}
}

void CLogPlotDoc::OnLayerFlush()
{
	CTrackObj* pObj;
	POSITION pos= m_TrackList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CTrackObj*)m_TrackList.GetNext(pos);
		if(pObj!=NULL)
		{
			if(pObj->bDrawDepth &&(pObj->m_LayerFileName !=""))
				pObj->m_LayerFlush =TRUE;
		}
	}
	UpdateAllViews(m_pView,HINT_UPDATE_SIZE);
}

void CLogPlotDoc::OnReFresh()
{
	SetDepthScale(m_DepthScale);
}

BOOL CLogPlotDoc::OpenWell()
{
	if(m_ProjectID>0)
	{
		if(MatchAllObject())
		{
			float dep1,dep2,Sdep=-9999.f,Edep=-9999.f;
			CGeoAdo		m_ado;
			if(!m_ado.InitData(m_szSever))
				return FALSE;
			//寻找项目测量井段
			CString cID;
			cID.Format("%lu",m_ProjectID);
			_RecordsetPtr pRS = m_ado.GetRS("SELECT * FROM Curve WHERE UpperID=" + cID);
			while(pRS->adoEOF == 0)//执行SELECT语句
			{
				dep1=(float)atof((LPCTSTR)(_bstr_t)pRS->GetCollect("DepSt"));
				dep2=(float)atof((LPCTSTR)(_bstr_t)pRS->GetCollect("DepEn"));
				if(dep1>Sdep)	Sdep=dep1;
				if(dep2>Edep)	Edep=dep2;
				pRS->MoveNext();
			}
			m_ado.Disconnect();
			if(Sdep<0.f) Sdep=0.f;
			SetDepth(Sdep,Edep);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CLogPlotDoc::IsHeadObj(CDrawObj* pObj)
{
	ASSERT(pObj != NULL);
	CDrawObj* pObj1;
	POSITION pos = m_HeadList.GetTailPosition();
	while (pos != NULL)
	{
		pObj1 = (CDrawObj*)m_RemObjList.GetPrev(pos);
		if (pObj1 == pObj) return TRUE;
	}                  
	return FALSE;
}

void CLogPlotDoc::DeleteObList()
{
	POSITION pos = m_TrackList.GetHeadPosition();
	while (pos != NULL)
		delete m_TrackList.GetNext(pos);
	m_TrackList.RemoveAll();
	pos = m_RemObjList.GetHeadPosition();
	while (pos != NULL)
		delete m_RemObjList.GetNext(pos);
	m_RemObjList.RemoveAll();
	pos = m_HeadList.GetHeadPosition();
	while (pos != NULL)
		delete m_HeadList.GetNext(pos);
	m_HeadList.RemoveAll();
}

void CLogPlotDoc::DeleteContents()
{
	COleDocument::DeleteContents();
	DeleteObList(); 
}

void CLogPlotDoc::MoveObj(CPoint pt)
{
	if ((pt.x == 0) && (pt.y == 0)) return;
	POSITION pos = m_TrackList.GetHeadPosition();
	while(pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*) m_TrackList.GetNext(pos);
		if (pObj != NULL) 
			pObj->SetRect(pObj->GetRect()+ pt);
	}
	pos = m_RemObjList.GetHeadPosition();
	while(pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*) m_RemObjList.GetNext(pos);
		if (pObj != NULL) 
			pObj->SetRect(pObj->GetRect()+ pt);
	}
}

void CLogPlotDoc::MoveHeadObj(CPoint pt)
{
	if ((pt.x == 0) && (pt.y == 0)) return;
	POSITION pos = m_HeadList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*) m_HeadList.GetNext(pos);
		if (pObj != NULL) pObj->SetRect(pObj->GetRect()+ pt);
	}
}

void CLogPlotDoc::AddObj(CDrawObj* pObj)
{
	ASSERT(pObj != NULL);
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
	else 
	{
		m_RemObjList.AddTail(pObj);
		pObj->SetParentObj(NULL,1);
		SetSize();
	}
}

void CLogPlotDoc::AddHeadObj(CDrawObj* pObj)
{
	ASSERT(pObj != NULL);
	m_HeadList.AddTail(pObj);
	pObj->SetParentObj(NULL,1);
}

void CLogPlotDoc::RemoveHeadObj(CDrawObj* pObj)
{
	if (pObj->GetParentObj() != NULL)  
	{
		pObj->GetParentObj()->RemoveObj(pObj);
	}	
	else 
	{
		POSITION pos = m_HeadList.Find(pObj,NULL);
		if (pos != NULL) m_HeadList.RemoveAt(pos);
	}
}

void CLogPlotDoc::RemoveObj(CDrawObj* pObj)
{
	ASSERT(pObj != NULL);
	POSITION pos=NULL;
	if (pObj->GetParentObj() != NULL)  
	{
		pObj->GetParentObj()->RemoveObj(pObj);
	}	
	else if (pObj->IsKindOf(RUNTIME_CLASS(CTrackObj)))
	{
		pos= m_TrackList.Find(pObj,NULL);
		if (pos != NULL) m_TrackList.RemoveAt(pos);
	}
	else 
	{
		pos = m_RemObjList.Find(pObj,NULL);
		if (pos != NULL) m_RemObjList.RemoveAt(pos);
		if (pObj->IsKindOf(RUNTIME_CLASS(CResultListObj))) SetResultListRect();
		else if (pObj->IsKindOf(RUNTIME_CLASS(CRemDrawObj))) SetSize();
	}
	SetModifiedFlag();
}

int CLogPlotDoc::MoveBack(CDrawObj* pObj)
{
	ASSERT(pObj != NULL);
	POSITION pos1; 
	POSITION pos = m_HeadList.Find(pObj,NULL);
	if (pos != NULL) 
	{
		pos1 = pos;
		m_HeadList.GetNext(pos1);
		if (pos1 != NULL)
		{
			m_HeadList.InsertAfter(pos1,pObj);
			m_HeadList.RemoveAt(pos);
			return 0;
		}
		return 1;
	}
	pos = m_TrackList.Find(pObj,NULL);
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
	return 1;
}

int CLogPlotDoc::MoveForward(CDrawObj* pObj)
{
	ASSERT(pObj != NULL);
	POSITION pos1; 
	POSITION pos = m_HeadList.Find(pObj,NULL);
	if (pos != NULL) 
	{
		pos1 = pos;
		m_HeadList.GetPrev(pos1);
		if (pos1 != NULL)
		{
			m_HeadList.InsertBefore(pos1,pObj);
			m_HeadList.RemoveAt(pos);
			return 0;
		}
		return 1;
	}
	pos = m_TrackList.Find(pObj,NULL);
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
	return 1;
}

int CLogPlotDoc::MoveToTail(CDrawObj* pObj)
{
	ASSERT(pObj != NULL);
	POSITION pos1; 
	POSITION pos = m_HeadList.Find(pObj,NULL);
	if (pos != NULL) 
	{
		pos1 = pos;
		m_HeadList.GetNext(pos1);
		if (pos1 != NULL)
		{
			m_HeadList.RemoveAt(pos);
			m_HeadList.AddTail(pObj);
			return 0;
		}
		return 1;
	}
	pos = m_TrackList.Find(pObj,NULL);
	if (pos != NULL) 
	{
		pos1 = pos;
		m_TrackList.GetNext(pos1);
		if (pos1 != NULL)
		{
			m_TrackList.RemoveAt(pos);
			m_HeadList.AddTail(pObj);
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
			m_HeadList.AddTail(pObj);
			return 0;
		}
		return 1;
	}
	return 1;
}

int CLogPlotDoc::MoveToHead(CDrawObj* pObj)
{
	POSITION pos1; 
	POSITION pos = m_HeadList.Find(pObj,NULL);
	if (pos != NULL) 
	{
		pos1 = pos;
		m_HeadList.GetPrev(pos1);
		if (pos1 != NULL)
		{
			m_HeadList.RemoveAt(pos);
			m_HeadList.AddHead(pObj);
			return 0;
		}
		return 1;
	}
	pos = m_TrackList.Find(pObj,NULL);
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
	pos = m_RemObjList.Find(pObj,NULL);
	if (pos != NULL) 
	{
		pos1 = pos;
		m_RemObjList.GetPrev(pos1);
		if (pos1 != NULL)
		{
			m_RemObjList.RemoveAt(pos);
			m_HeadList.AddHead(pObj);
			return 0;
		}
		return 1;
	}
	return 1;
}

CDrawObj* CLogPlotDoc::ObjectAt(CView* pView,const CPoint& point)
{
	ASSERT(pView != NULL);
	CDrawObj* pObj;
	if (pView->IsKindOf(RUNTIME_CLASS(CLogPlotView)))
	{
		POSITION pos = m_RemObjList.GetTailPosition();
		while (pos != NULL)
		{
			CDrawObj* pDrawObj = (CDrawObj*)m_RemObjList.GetPrev(pos);
			if ((pObj = pDrawObj->ObjectAt(pView,point)) != NULL) return pObj;
		}                                                       

		pos = m_TrackList.GetHeadPosition();
		while (pos != NULL)
		{
			CTrackObj* pTrackObj = (CTrackObj*)m_TrackList.GetNext(pos);
			if ((pObj = pTrackObj->ObjectAt(pView,point)) != NULL) return pObj;
		}                                                       
	}
	//else if (pView->IsKindOf(RUNTIME_CLASS(CHeadPlotView)))
	//{
	//	POSITION pos = m_HeadList.GetTailPosition();
	//	while (pos != NULL)
	//	{
	//		CDrawObj* pDrawObj = (CDrawObj*)m_HeadList.GetPrev(pos);
	//		if ((pObj = pDrawObj->ObjectAt(pView,point)) != NULL) return pObj;
	//	}                                                       
	//}
	return(NULL);
}

CDrawObj* CLogPlotDoc::GetLocateObject(CDrawObj* pObj2)
{
	ASSERT(pObj2 != NULL);
	CDrawObj *pObj1,*pObj;
	POSITION pos = m_TrackList.GetTailPosition();
	while (pos != NULL)
	{
		pObj = (CTrackObj*)m_TrackList.GetPrev(pos);
		if ((pObj1 = pObj->GetLocateObject(pObj2)) != NULL) return pObj1;
	}                  
	pos = m_RemObjList.GetTailPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_RemObjList.GetPrev(pos);
		if ((pObj1 = pObj->GetLocateObject(pObj2)) != NULL) return pObj1;
	}                  
	return NULL;
}

BOOL CLogPlotDoc::OnSetCursor(CView* pView,const CPoint& point)
{
	if (pView->IsKindOf(RUNTIME_CLASS(CLogPlotView)))
	{
		POSITION pos = m_RemObjList.GetTailPosition();
		while (pos != NULL)
		{
			CDrawObj* pDrawObj = (CDrawObj*)m_RemObjList.GetPrev(pos);
			if (pDrawObj->OnSetCursor(pView,point)) return TRUE;
		}                                                       

		pos = m_TrackList.GetTailPosition();
		while (pos != NULL)
		{
			CTrackObj* pTrackObj = (CTrackObj*)m_TrackList.GetPrev(pos);
			if (pTrackObj->OnSetCursor(pView,point)) return TRUE;
		}                                                       
	}
	/*else if (pView->IsKindOf(RUNTIME_CLASS(CHeadPlotView)))
	{
		POSITION pos = m_HeadList.GetTailPosition();
		while (pos != NULL)
		{
			CDrawObj* pDrawObj = (CDrawObj*)m_HeadList.GetPrev(pos);
			if (pDrawObj->OnSetCursor(pView,point)) return TRUE;
		}                                                       
	}*/
	return FALSE;
}

BOOL CLogPlotDoc::OnToolTip(CView* pView,CRect& rect,CString& text)
{
	POSITION pos = m_RemObjList.GetTailPosition();
	while (pos != NULL)
	{
		CDrawObj* pDrawObj = (CDrawObj*)m_RemObjList.GetPrev(pos);
		if (pDrawObj->OnToolTip(pView,rect,text)) return TRUE;
	}                                                       
	pos = m_TrackList.GetTailPosition();
	while (pos != NULL)
	{
		CTrackObj* pTrackObj = (CTrackObj*)m_TrackList.GetPrev(pos);
		if (pTrackObj->OnToolTip(pView,rect,text)) return TRUE;
	}                                                       
	return FALSE;
}

void CLogPlotDoc::Print(CDC* pDC,CView* pView)
{
	m_WinOrg.x = 0;
	m_WinOrg.y = 0;
	CPoint pt(0,0);
	CDrawObj* pObj;
	CRemDrawObj* pRemObj;
	POSITION pos;
//  Print Head
	//if(m_bPlotHead)
	//{
	//	POSITION pos = m_HeadList.GetHeadPosition();
	//	while (pos != NULL)
	//	{
	//		pObj = (CDrawObj*)m_HeadList.GetNext(pos);
	//		if(pObj != NULL)
	//		{
	//			pRemObj=(CRemDrawObj*)pObj;
	//			if(pRemObj!=NULL)
	//			{
	//				if(m_DarkSignl)
	//				{
	//					if(pRemObj->m_LogBrush.lbColor !=0x0ffffff)
	//						pRemObj->m_LogBrush.lbColor =0x0;
	//					if(pRemObj->m_LogPen.lopnColor!=0x0ffffff)
	//						pRemObj->m_LogPen.lopnColor =0x0;
	//				}
	//				pObj->Draw(pView,pDC);
	//			}
	//		}
	//	}
	//	//  Chang Coordinate Origin	
	//	pt.y = m_HeadRect.Height(); 
	//	MoveObj(pt);
	//}                   
	//  Pring Curve
	if(m_bPlotTrack)
	{
		pos = m_TrackList.GetHeadPosition();
		while (pos != NULL)
		{
			pObj = (CDrawObj*)m_TrackList.GetNext(pos);
			if(pObj!=NULL)
				pObj->Draw(pView,pDC);
		} 
		//  Print Note	
		pos = m_RemObjList.GetHeadPosition();
		while(pos != NULL)
		{
			pRemObj = (CRemDrawObj*)m_RemObjList.GetNext(pos);
			if((pObj!=NULL)&&(!pObj->IsKindOf(RUNTIME_CLASS(CDepSect))))
				pObj->Draw(pView,pDC);
		} 
	}
	/*if(m_bPlotHead)
	{
		pt.y =-m_HeadRect.Height(); 
		MoveObj(pt);
	}*/
}
int	CLogPlotDoc::GetObjCount()
{
	int Num=0;
	CTrackObj* pObj;
	POSITION pos= m_TrackList.GetHeadPosition();
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
									while(pos3 != NULL)
									{	
										pObj3 = (CTrackObj*)pObj2->m_ObjList.GetNext(pos3);
										Num+=pObj3->m_ObjList.GetCount();
									} 
								}
								Num+=pObj2->m_ObjList.GetCount();
							}
						}
						Num+=pObj1->m_ObjList.GetCount();
					}
				}
				Num+=pObj0->m_ObjList.GetCount();
			}
		}
		Num++;
	}
	Num+=m_RemObjList.GetCount();
	return Num;
}

CImageObj* CLogPlotDoc::GetImageObject()
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
										if(pObj3->IsKindOf (RUNTIME_CLASS(CImageObj)))
											return (CImageObj*)pObj3;
								}
								if(pObj2->IsKindOf (RUNTIME_CLASS(CImageObj)))
									return (CImageObj*)pObj2;
							}
						}
						if(pObj1->IsKindOf (RUNTIME_CLASS(CImageObj)))
							return (CImageObj*)pObj1;
					}
				}
				if(pObj0->IsKindOf (RUNTIME_CLASS(CImageObj)))
					return (CImageObj*)pObj0;
			}
		}
		if(pObj->IsKindOf (RUNTIME_CLASS(CImageObj)))
			return (CImageObj*)pObj;
	}
	return NULL;
}