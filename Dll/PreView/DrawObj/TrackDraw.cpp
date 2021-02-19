/******************************************************************
*   TrackDrw.cpp - implementation of the Track draw object class  *           
*******************************************************************/

#include "stdafx.h" 
#include <math.h>         
#include "GeoFIDio.h"
#include "PreView.h"
#include "LogPlotDoc.h"
#include "LogPlotView.h"
#include "TrackDraw.h"
#include "FIDDraw.h"
#include "ResultDraw.h"
#include "LthDraw.h"
#include "DepthDialog.h"
#include "WidthDialog.h"
#include "HeightDialog.h"

//#include "StyleDialog.h"         
#include "GridPage.h"
#include "MinFontDialog.h"
#include "LineListPage.h"
#include "FillListPage.h"
#include "TrackBasePage.h"
#include "LayerPage.h"

CLayerPoint::CLayerPoint(CRect rect,CPoint pt, COLORREF ResultColor,
						 LOGFONT ResultFont,CString NoStr)
{
	m_NoStr=NoStr;
	m_Rect = rect;
	m_point=pt;
	m_ResultColor=ResultColor;
	m_ResultFont=ResultFont;
}

void CLayerPoint::Draw(CDC* pDC,CLogPlotDoc* pDoc,int PenW)
{
	COLORREF Color0=m_ResultColor;
	CRect rect=m_Rect;
	CPoint pt=m_point;
	pDoc->LPtoPP(&rect);
	pDoc->LPtoPP(&pt);
	if(m_NoStr.Find ("LINE")>-1)
	{
		CPen m_Pen(0,PenW,m_ResultColor);
		CPen* pOldPen = pDC->SelectObject(&m_Pen);
		pDC->MoveTo (rect.left,rect.bottom );
		pDC->LineTo (rect.BottomRight ());
		pDC->SelectObject (pOldPen);
		return;
	}
	if(m_NoStr.Find ("DASHED")>-1)
	{
		CPen m_Pen(0,1,m_ResultColor);
		CPen* pOldPen = pDC->SelectObject(&m_Pen);
		for(int i=rect.left;i<rect.right;i+=6)
		{
			pDC->MoveTo (i,rect.bottom);
			pDC->LineTo (i+3,rect.bottom);
			pDC->MoveTo (i,rect.bottom+1);
			pDC->LineTo (i+3,rect.bottom+1);
		}
		pDC->SelectObject (pOldPen);
		return;
	}
	if(m_NoStr.Find ("FAULT")>-1)
	{
		CPen m_Pen(0,PenW,m_ResultColor);
		CPen* pOldPen = pDC->SelectObject(&m_Pen);
		pDC->MoveTo (rect.left,rect.bottom );
		CPoint point=rect.BottomRight ();
		pDC->LineTo (point);
		pDC->LineTo (point.x-8,point.y-9);
		point.x=rect.left ;
		pDC->MoveTo (point);
		pDC->LineTo (point.x+8,point.y+7);
		pDC->SelectObject (pOldPen);
		return;
	}
	CFont m_Font;
	m_Font.CreateFontIndirect(&m_ResultFont);
	pDC->SetTextColor(m_ResultColor);
	pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
	pDC->SetBkMode(TRANSPARENT); 
	CFont* pOldFont = pDC->SelectObject(&m_Font);
	pDC->TextOut(pt.x,pt.y,m_NoStr);
	pDC->SelectObject(pOldFont);
	m_ResultColor=Color0;
}

void CTrackObj::DrawLayer(CDC* pDC)
{			
	if (m_LayerFlush) CreatePoint(pDC);
	CLayerPoint* pObj;
	POSITION pos = m_LayerPoint.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CLayerPoint*) m_LayerPoint.GetNext(pos);
		CRect rect = pObj->GetRect();
		pDoc->LPtoPP(&rect);
		CRect r=m_DepthRect;
		pDoc->LPtoPP(&r);
		if(pDC->RectVisible(rect)&&r.top<=rect.top&&r.bottom>=rect.bottom)
			pObj->Draw(pDC,pDoc,m_TrackFramePen.lopnWidth.x);
	}
}

IMPLEMENT_SERIAL(CTrackObj, CDrawObj, 0)

CTrackObj::CTrackObj()
{
	m_bSelected = FALSE;
	bSetHeadHeight = FALSE;  
	m_bChildTrackHeadAlignMax = TRUE;
	m_bChildTrackHeadHeightAuto = TRUE;
	m_bHeadHeightAuto = TRUE;
	m_LayerFileName="";
	m_DepUnitName="(米)";
	m_LayerFlush=FALSE;
}

CTrackObj::CTrackObj(const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDocument,double sdep,double edep) 
		: CDrawObj(rect,pParentObj,pDocument)
{                 
	bSetHeadHeight = FALSE;  
	m_bChildTrackHeadAlignMax = TRUE;
	m_bChildTrackHeadHeightAuto = TRUE;
	m_bHeadHeightAuto = TRUE;
	m_PlotSdep = sdep;
	m_PlotEdep = edep;
	m_ClientRect.top = rect.top;
	m_ClientRect.left = rect.left;
	m_ClientRect.right = rect.left + TRACK_WIDTH;
	m_ClientRect.bottom = DepthToYCoor(m_PlotEdep);
	m_pGrid = new CGridObj(m_ClientRect,m_PlotSdep,m_PlotEdep);
	m_pGrid->SetDepC(pDoc->m_DepC);
    m_HeadRect = m_ClientRect;
    m_HeadRect.top = m_ClientRect.top-20;
    m_HeadRect.bottom = m_ClientRect.top;
    m_TitleRect = m_ClientRect;
    m_TitleRect.top = m_HeadRect.top - TITLE_HEIGHT;
    m_TitleRect.bottom = m_HeadRect.top;
    m_Rect = m_ClientRect;
    m_Rect.top = m_TitleRect.top;
    m_Rect.bottom = m_ClientRect.bottom;           
    m_DepthRect.left = m_ClientRect.left; m_DepthRect.right = m_DepthRect.left+40;
    m_DepthRect.top = m_TitleRect.bottom; m_DepthRect.bottom = m_ClientRect.bottom;
	m_Width = 2.0;
	m_bSelected = FALSE;
	m_TrackFramePen.lopnWidth = CPoint(2,2); 
	m_TrackFramePen.lopnColor = 0x0;
	m_TrackFramePen.lopnStyle = 0;
	m_TrackTitleBrush.lbStyle = BS_NULL;
	m_TrackTitleBrush.lbColor = 0xffffff; 
	m_TrackTitleBrush.lbHatch = 1;
	m_TrackHeadBrush = m_TrackTitleBrush;
	m_TrackClientBrush = m_TrackTitleBrush;
	m_TrackDepthBrush = m_TrackTitleBrush;

	LOGFONT m_LogFont={-14,0,0,0,FW_BOLD,0,0,0,ANSI_CHARSET,OUT_TT_ONLY_PRECIS,
			CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,VARIABLE_PITCH | FF_SWISS,"Times New Roman"};
    m_TrackNameFont = m_LogFont;
    m_TrackNameColor = 0x0;
    m_TrackName="曲线道";
    bDrawDepth = false;
	bDrawTitle = true;
	bTrackResize =true;
    m_DepthName="深度";
	m_DepthTrackPosition = IDM_LEFT;            
	m_DepthPosition = IDM_LEFT;            
	m_DepthLevel = 10;
	LOGFONT m_DepthLogFont= {
		-20,					    //lfHeight
		0,							//lfWidth
		2700,						//lfEscapement
		0, 							//lfOrientation
		FW_NORMAL,					//lfWeight
		0,							//lfItalic
		0,							//lfUnderLine;
		0,							//lfStrikeOut;
		ANSI_CHARSET,				//lfCharSet
		OUT_TT_ONLY_PRECIS,			//lfOutPrecision
		CLIP_DEFAULT_PRECIS,		//lfClipPrecision
		DEFAULT_QUALITY,			//lfQuality
		VARIABLE_PITCH | FF_SWISS,	//lfPitchAndFamily
		"Times New Roman"			//lfFaceName
	};          
	m_DepthNameFont = m_DepthLogFont;
	m_DepUnitNameFont = m_DepthLogFont;
	m_DepthValueFont = m_DepthLogFont;
	m_LayerNameFont = m_DepthLogFont;

    m_DepthNameColor = 0x0;
    m_DepUnitNameColor = 0x0;
	m_DepthValueColor = 0x0;
	m_LayerNameColor = 0x0;
	m_bAutoArrangle = TRUE;                
	m_HoriFIDRect = m_HeadRect;
	m_TrackRect = m_HeadRect;
	m_VertFIDRect = m_HeadRect;
	m_VertFIDHoriArrangleMode = IDM_RIGHT;
	m_VertFIDVertAlignMode = IDM_JUST;
	m_HoriFIDHoriArrangleMode = IDM_NONE;
	m_HoriFIDVertArrangleMode = IDM_BOTTOM;
	m_HoriFIDHoriAlignMode = IDM_JUST;
	m_HoriFIDVertAlignMode = IDM_NONE;
	m_TrackHoriArrangleMode = IDM_LEFT;
	m_TrackVertAlignMode = IDM_NONE;
	m_LayerFileName="";
	m_DepUnitName="(1:200m)";
	m_LayerFlush=FALSE;
	m_bDrawRuler=TRUE;
	m_bLayerHi=TRUE;
	m_pGrid->m_bDrawRuler=m_bDrawRuler;
	m_pGrid->m_DepthTrackPosition=m_DepthTrackPosition;
}

CTrackObj::~CTrackObj()
{   
	if (m_pGrid != NULL) delete m_pGrid;
	POSITION pos = m_LayerPoint.GetHeadPosition();
	while (pos != NULL) 
		delete m_LayerPoint.GetNext(pos);
	m_LayerPoint.RemoveAll();
}
    
void CTrackObj::Serialize(CArchive& ar)
{                
	LONG l;
	CDrawObj::Serialize(ar);
	if(ar.IsStoring())
	{
		SerializeLogPen(ar,m_TrackFramePen);
		ar << m_TitleRect;
		ar << bDrawTitle;
		ar << bTrackResize;
		ar << m_TrackName;

		ar << m_LayerFileName;
		ar << m_DepUnitName;
		ar << m_bDrawRuler;
		ar << (LONG)m_DepUnitNameColor;
		SerializeLogFont(ar,m_DepUnitNameFont);
		ar << (LONG)m_LayerNameColor;
		SerializeLogFont(ar,m_LayerNameFont);
		ar << m_bLayerHi;

		ar << (LONG)m_TrackNameColor;
		SerializeLogFont(ar,m_TrackNameFont);
		SerializeLogBrush(ar,m_TrackTitleBrush);

		ar << m_DepthRect;
		ar << bDrawDepth;
		ar << m_DepthTrackPosition;
		ar << m_DepthPosition;
		ar << m_DepthName;
		ar << (LONG)m_DepthNameColor;
		SerializeLogFont(ar,m_DepthNameFont);
		ar << (LONG)m_DepthValueColor;
		SerializeLogFont(ar,m_DepthValueFont);
		ar << m_DepthLevel;
		SerializeLogBrush(ar,m_TrackDepthBrush);

		ar << m_HeadRect;
		SerializeLogBrush(ar,m_TrackHeadBrush);

		ar << m_ClientRect;
		SerializeLogBrush(ar,m_TrackClientBrush);
		ar << m_PlotSdep;
		ar << m_PlotEdep;                   
		ar << m_VertFIDRect;
		ar << m_HoriFIDRect;
		ar << m_TrackRect;
		ar << m_bAutoArrangle;
		ar << m_HoriFIDHoriArrangleMode;
		ar << m_HoriFIDHoriAlignMode;
		ar << m_HoriFIDVertArrangleMode;
		ar << m_HoriFIDVertAlignMode;
		ar << m_VertFIDHoriArrangleMode;
		ar << m_VertFIDVertAlignMode;
		ar << m_TrackHoriArrangleMode;
		ar << m_TrackVertAlignMode;
		ar << m_bHeadHeightAuto;
		ar << m_bChildTrackHeadHeightAuto;
		ar << m_bChildTrackHeadAlignMax;
		m_pGrid->Serialize(ar);
	}
	else
	{
		// get the document back pointer from the archive
		pDoc = (CLogPlotDoc*)ar.m_pDocument;
		ASSERT_VALID(pDoc);
		ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CLogPlotDoc)));
		SerializeLogPen(ar,m_TrackFramePen);
		ar >> m_TitleRect;
		ar >> bDrawTitle;
		ar >> bTrackResize;
		ar >> m_TrackName;
		
		if(pDoc->m_VersionNo>2.5f)
		{
			ar >> m_LayerFileName;
			ar >> m_DepUnitName;
			ar >> m_bDrawRuler;
			ar >> l; 
			m_DepUnitNameColor = (COLORREF) l;
			SerializeLogFont(ar,m_DepUnitNameFont);
			ar >> l; 
			m_LayerNameColor = (COLORREF) l;
			SerializeLogFont(ar,m_LayerNameFont);
			ar >> m_bLayerHi;
		}
		else
		{
			int xxx=m_TrackName.Find ("+");
			if(xxx>-1)
			{
				CString ss=m_TrackName.Mid (xxx+1);
				m_TrackName=m_TrackName.Left (xxx);
				xxx=ss.Find("+");
				if(xxx>-1)
					ss=ss.Left (xxx);
				m_LayerFileName=ss;
			}
			LOGFONT m_LogFont={-16,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_TT_ONLY_PRECIS,
				CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,VARIABLE_PITCH | FF_SWISS,"Times New Roman"};
			m_DepUnitNameFont=m_LogFont;
			m_DepUnitNameColor=0x0;
		}

		ar >> l; 
		m_TrackNameColor = (COLORREF) l;
		SerializeLogFont(ar,m_TrackNameFont);
		SerializeLogBrush(ar,m_TrackTitleBrush);	
		ar >> m_DepthRect;
		ar >> bDrawDepth;
		ar >> m_DepthTrackPosition;
		ar >> m_DepthPosition;
		ar >> m_DepthName;
		ar >> l; m_DepthNameColor = (COLORREF) l;
		SerializeLogFont(ar,m_DepthNameFont);
		if(pDoc->m_VersionNo<=2.5f)
		{
			m_LayerNameFont=m_DepthNameFont;
			m_LayerNameColor=m_DepthNameColor;
		}

		ar >> l; m_DepthValueColor = (COLORREF) l;
		SerializeLogFont(ar,m_DepthValueFont);
		ar >> m_DepthLevel;
		SerializeLogBrush(ar,m_TrackDepthBrush);
		
		ar >> m_HeadRect;
		SerializeLogBrush(ar,m_TrackHeadBrush);

		ar >> m_ClientRect;
		SerializeLogBrush(ar,m_TrackClientBrush);

		ar >> m_PlotSdep;
		ar >> m_PlotEdep;
		ar >> m_VertFIDRect;
		ar >> m_HoriFIDRect;
		ar >> m_TrackRect;
		ar >> m_bAutoArrangle;
		ar >> m_HoriFIDHoriArrangleMode;
		ar >> m_HoriFIDHoriAlignMode;
		ar >> m_HoriFIDVertArrangleMode;
		ar >> m_HoriFIDVertAlignMode;
		ar >> m_VertFIDHoriArrangleMode;
		ar >> m_VertFIDVertAlignMode;
		ar >> m_TrackHoriArrangleMode;
		ar >> m_TrackVertAlignMode;
		ar >> m_bHeadHeightAuto;
		ar >> m_bChildTrackHeadHeightAuto;
		ar >> m_bChildTrackHeadAlignMax;
		m_pGrid = new CGridObj();
		m_pGrid->Serialize(ar);
		m_pGrid->SetDepC(pDoc->m_DepC);
		m_pGrid->bDrawDepth=bDrawDepth;	
		m_pGrid->m_bDrawRuler=m_bDrawRuler;
		m_pGrid->m_DepthTrackPosition=m_DepthTrackPosition;
		if(m_LayerFileName!="")
			m_LayerFlush=TRUE;
	}               
}

BOOL CTrackObj::Properties(CView* pView)
{   
	CPropertySheet m_Sheet("格线道属性");
	CTrackBasePage m_BasePage(this);
	m_BasePage.m_bLayerHi=m_bLayerHi;
	m_Sheet.AddPage(&m_BasePage);

	CString szHT;
	CLineListPage m_LineList;
	szHT.LoadString(IDS_FRAMELINE);
	m_LineList.AddPen(&m_TrackFramePen,szHT);
	m_Sheet.AddPage(&m_LineList);

	CFillListPage m_FillList;
	szHT.LoadString(IDS_HEADFILL);
	m_FillList.AddFill(&m_TrackTitleBrush,szHT);
	szHT.LoadString(IDS_ZERHEADFILL);
	m_FillList.AddFill(&m_TrackHeadBrush,szHT);
	szHT.LoadString(IDS_DRAWREGIONFILL);
	m_FillList.AddFill(&m_TrackClientBrush,szHT);
	szHT.LoadString(IDS_DEPTHREGIONFILL);
	m_FillList.AddFill(&m_TrackDepthBrush,szHT);
	m_Sheet.AddPage(&m_FillList);

	CMinFontPage m_FontPage;
	TEXTFONT m_TextFont[5];
	m_TextFont[0].m_ItemName = "道标题";
	m_TextFont[0].m_Text = m_TrackName;
	m_TextFont[0].m_ForeGround = m_TrackNameColor;
	m_TextFont[0].m_BackGround = RGB(255,255,255);
	m_TextFont[0].lf = m_TrackNameFont;
	m_FontPage.AddFont(&m_TextFont[0]);  

	m_TextFont[1].m_ItemName = "深度";
	m_TextFont[1].m_Text = m_DepthName;
	m_TextFont[1].m_ForeGround = m_DepthNameColor;
	m_TextFont[1].m_BackGround = RGB(255,255,255);
	m_TextFont[1].lf = m_DepthNameFont;
	m_FontPage.AddFont(&m_TextFont[1]);

	m_TextFont[2].m_ItemName = "深度单位";
	m_TextFont[2].m_Text = m_DepUnitName;
	m_TextFont[2].m_ForeGround = m_DepUnitNameColor;
	m_TextFont[2].m_BackGround = RGB(255,255,255);
	m_TextFont[2].lf = m_DepUnitNameFont;
	m_FontPage.AddFont(&m_TextFont[2]);

	m_TextFont[3].m_ItemName = "深度值";
	m_TextFont[3].m_Text = "1234";
	m_TextFont[3].m_ForeGround = m_DepthValueColor;
	m_TextFont[3].m_BackGround = RGB(255,255,255);
	m_TextFont[3].lf = m_DepthValueFont;
	m_FontPage.AddFont(&m_TextFont[3]);

	m_TextFont[4].m_ItemName = "层位";
	m_TextFont[4].m_Text = "AjzD";
	m_TextFont[4].m_ForeGround = m_LayerNameColor;
	m_TextFont[4].m_BackGround = RGB(255,255,255);
	m_TextFont[4].lf = m_LayerNameFont;
	m_FontPage.AddFont(&m_TextFont[4]);
	m_Sheet.AddPage(&m_FontPage);

	CGridPage m_GridPage(m_pGrid);
	m_Sheet.AddPage(&m_GridPage);

	CLayerPage LayerPage;
	LayerPage.m_FileName =m_LayerFileName;
	m_Sheet.AddPage(&LayerPage);

	double m_PlotSdep1 = m_PlotSdep;
	double m_PlotEdep1 = m_PlotEdep;

	if (m_Sheet.DoModal() == IDOK)
	{
		m_LayerFileName=LayerPage.m_FileName;
		m_bLayerHi=m_BasePage.m_bLayerHi;
		m_FontPage.OnOK();
		m_DepthLevel = m_pGrid->m_DepLine3;
		CRect rect = m_Rect;
		if (pDoc->m_DrawFIDHeadMode == 1)
			rect.bottom = m_ClientRect.bottom + m_ClientRect.top - m_Rect.top;
		m_TrackNameColor = m_TextFont[0].m_ForeGround;
		m_TrackNameFont = m_TextFont[0].lf;
		m_TrackNameFont.lfEscapement = 0;
		m_TrackName = m_TextFont[0].m_Text;
		m_DepthNameColor = m_TextFont[1].m_ForeGround;
		m_DepthNameFont = m_TextFont[1].lf;
		m_DepthName = m_TextFont[1].m_Text;

		m_DepUnitNameColor = m_TextFont[2].m_ForeGround;
		m_DepUnitNameFont = m_TextFont[2].lf;
		m_DepUnitName = m_TextFont[2].m_Text;

		m_DepthValueColor = m_TextFont[3].m_ForeGround;
		m_DepthValueFont = m_TextFont[3].lf;
		
		m_LayerNameColor = m_TextFont[4].m_ForeGround;
		m_LayerNameFont = m_TextFont[4].lf;

		OnShowDepth(m_BasePage.bDrawDepth);
		OnDepthTrackPosition(m_BasePage.m_DepthTrackPosition);
		OnDepthPosition(m_BasePage.m_DepthValuePosition);
		OnShowTitle(m_BasePage.bDrawTitle);
		bTrackResize=m_BasePage.bTrackResize;
		if((m_PlotSdep != m_PlotSdep1) || (m_PlotEdep != m_PlotEdep1))
		{
			double m_PlotSdep2 = m_PlotSdep;
			double m_PlotEdep2 = m_PlotEdep;
			m_PlotSdep = m_PlotSdep1;
			m_PlotEdep = m_PlotEdep1;
			SetDepth(m_PlotSdep2,m_PlotEdep2);
		}
		CRect rect1 = m_Rect;
		if(pDoc->m_DrawFIDHeadMode == 1)
			rect1.bottom = m_ClientRect.bottom + m_ClientRect.top - m_Rect.top;
		rect |= rect1;
		if(m_LayerFileName!="")
			m_LayerFlush=TRUE;
		pDoc->SetModifiedFlag();
		m_pGrid->bDrawDepth=bDrawDepth;	
		m_pGrid->m_bDrawRuler=m_bDrawRuler;
		m_pGrid->m_DepthTrackPosition=m_DepthTrackPosition;
		InvalObj(pView);
		return TRUE;
	}
	return FALSE;
}

void CTrackObj::OnSetDepth()
{   
	CPlotDepthDialog dialog; 
	double sdep,edep;
	dialog.m_DepthScale=pDoc->m_DepthScale;
	dialog.SetDepth(m_PlotSdep,m_PlotEdep);
	if (dialog.DoModal() == IDOK)
	{
		dialog.GetDepth(sdep,edep);
		SetDepth(sdep,edep);

		if (pDoc->bVertAutoAlign) pDoc->DoVertAlign();   
		pDoc->UpdateAllViews(NULL,HINT_UPDATE_WINDOW);
	}	
}

void CTrackObj::OnDepthPosition(int nPosition)
{
	if (m_DepthPosition == nPosition) return;
	m_DepthPosition = nPosition;
}

void CTrackObj::OnDepthTrackPosition(int nPosition)
{
	if (m_DepthTrackPosition == nPosition) return;
	if (m_DepthTrackPosition == IDM_CENTER)	
	{
		OnShowDepth(FALSE);
		m_DepthTrackPosition = nPosition;
		OnShowDepth(TRUE);
	}                                    
	else if (nPosition == IDM_CENTER)
	{
		OnShowDepth(FALSE);
		m_DepthTrackPosition = nPosition;
		OnShowDepth(TRUE);
	}
	else
	{
		m_DepthTrackPosition = nPosition;
		CPoint point(0,0);
		if (nPosition == IDM_LEFT) 
		{
			point.x = +m_DepthRect.Width();
			m_DepthRect.right = m_Rect.left + m_DepthRect.Width();
			m_DepthRect.left = m_Rect.left;
		}	
		else 
		{
			point.x = -m_DepthRect.Width();
			m_DepthRect.left = m_Rect.right - m_DepthRect.Width();
			m_DepthRect.right = m_Rect.right;
		}	
		m_HeadRect += point;
		m_VertFIDRect += point;
		m_HoriFIDRect += point;
		m_TrackRect += point;
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*) m_ObjList.GetNext(pos);
			if (pObj != NULL)
			{
		    	CRect rect1 = pObj->GetRect();
		    	if ((rect1.left > m_ClientRect.left) && (rect1.right < m_ClientRect.right))
 				pObj->SetRect(rect1+point);
			} 
		}	
		m_ClientRect += point;
	}
}        


void CTrackObj::OnShowDepth(BOOL bShow)
{
	if (bShow == bDrawDepth) return;
	int w = m_DepthRect.Width();
	bDrawDepth = bShow;
	if (bShow)  
	{
		
		if (w < 10) w = 10;
		if (m_DepthTrackPosition == IDM_CENTER)
		{
			m_DepthRect.left = (m_Rect.right + m_Rect.left - w) / 2;
			m_DepthRect.right = m_DepthRect.left + w;
		}
		else 
		{
			int w1 = GetMinHeadWidth();
			if (m_ClientRect.Width() - m_DepthRect.Width() < w1)
			{
        	 	AfxMessageBox("\n该道的宽度太小\n增加道宽才能显示深度\n显示深度错误" , MB_OK | MB_ICONSTOP);
				bDrawDepth = FALSE;
         		return;
 			}
			else if (m_DepthTrackPosition == IDM_LEFT)
			{                                   
				m_DepthRect.right = m_Rect.left + w;
				m_DepthRect.left = m_Rect.left;	                      
				m_HeadRect.left += w;
				CRect rect = m_ClientRect;
				rect.left += w;
				SetClientRect(rect);
			}
			else if (m_DepthTrackPosition == IDM_RIGHT)
			{                                   
				m_DepthRect.right = m_Rect.right;	                      
				m_DepthRect.left = m_Rect.right - w;
				m_HeadRect.right -= w;
				CRect rect = m_ClientRect;
				rect.right -= w;
				SetClientRect(rect);
			}
		}
	}	
	else 
	{
		if (m_DepthTrackPosition == IDM_LEFT)
		{                                   
			m_HeadRect.left -= w;
			CRect rect = m_ClientRect;
			rect.left -= w;
			SetClientRect(rect);
		}
		else if (m_DepthTrackPosition == IDM_RIGHT)
		{                                   
			m_HeadRect.right += w;
			CRect rect = m_ClientRect;
			rect.right += w;
			SetClientRect(rect);
		}
	}
}

void CTrackObj::OnShowTitle(BOOL bShow)
{
	if (bDrawTitle == bShow) return;
	if(bShow) 
	{
		int height = m_TitleRect.Height();
		if (height < 10) height = 10;
		SetTitleRectHeight(height);
	}                          
	else SetTitleRectHeight(0);
}

double CTrackObj::YCoorToDepth(int y)
{
	return (double)((y - m_ClientRect.top) * pDoc->m_DepC + m_PlotSdep);
}

long CTrackObj::DepthToYCoor(double depth)
{
	return (long) ((depth - m_PlotSdep) / pDoc->m_DepC + m_ClientRect.top);
}

void CTrackObj::SetTitleRectHeight(int nHeight)
{
	int nSave = m_Rect.top;
	if (nHeight == 0)
	{
		m_Rect.top = m_HeadRect.top;
		m_DepthRect.top = m_HeadRect.top;
		bDrawTitle = FALSE;  
	}                            
	else
	{
		bDrawTitle = TRUE;
		m_TitleRect.top = m_TitleRect.bottom - nHeight;
		m_Rect.top = m_TitleRect.top;
	}	               

	//如果道标题高度改变，通知父对象
	
	if (nSave != m_Rect.top)
	{
		if (m_pParentObj == NULL)	
			pDoc->OnChildHeightChange(this);
		else 
			m_pParentObj->OnChildHeightChange(this);
	}
}

void CTrackObj::SetFIDWidth(int direct)
{
	CDrawObj* pObj;
	CRect rect;
	UINT m_MinWidth,m_MaxWidth;
	m_MinWidth = 0;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) && (pObj->GetParentType() == 0))
		{
			if (((CFIDDrawObj*)pObj)->GetDrawDirect() == direct)			
			{
				UINT w = ((CFIDDrawObj*)pObj)->GetMinWidth();
				if (m_MinWidth < w) m_MinWidth = w;
			}
		}
	}
	m_MaxWidth = m_Rect.Width();
	if (m_MinWidth > m_MaxWidth) m_MinWidth = m_MaxWidth;
	CWidthDialog m_Dialog;
	m_Dialog.m_MinWidth = (int)(m_MinWidth*0.254);
	m_Dialog.m_MaxWidth = (int)(m_MaxWidth*0.254);
	if (m_Dialog.DoModal() == IDOK)
	{
		int w = (int)(m_Dialog.m_Width / 0.254);
		bEnableChildSizeMessage = FALSE;		// 禁止子道消息
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			pObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if ((pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) && (pObj->GetParentType() == 0))
			{
				if (((CFIDDrawObj*)pObj)->GetDrawDirect() == direct)			
				{
					rect = ((CFIDDrawObj*)pObj)->GetHeadRect();
					rect.right = rect.left + m_Dialog.m_Width;
					if (rect.right > m_Rect.right)
					{
						rect.right = m_Rect.right;
						rect.left = m_Rect.right - w;
					}
					((CFIDDrawObj*)pObj)->SetHeadRect(rect);
				}
			}
		}
		if (m_bAutoArrangle) DoHoriArrangle();
		rect = m_Rect;
		if (pDoc->m_DrawFIDHeadMode == 1)
			rect.bottom = m_ClientRect.bottom + m_ClientRect.top - m_Rect.top;
		InvalidateRect(rect);
		bEnableChildSizeMessage = TRUE;			// 允许子道消息	
	}
}

void CTrackObj::SetFIDHeight(int direct)
{
	CDrawObj* pObj;
	CRect rect;
	UINT m_MinHeight,m_MaxHeight;
	m_MinHeight = 0;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) && (pObj->GetParentType() == 0))
		{
			if (((CFIDDrawObj*)pObj)->GetDrawDirect() == direct)			
			{
				UINT w = ((CFIDDrawObj*)pObj)->GetMinHeight();
				if (m_MinHeight < w) m_MinHeight = w;
			}
		}
	}
	m_MaxHeight = 200;
	if (m_MinHeight > m_MaxHeight) m_MinHeight = m_MaxHeight;
	CHeightDialog m_Dialog;
	m_Dialog.m_MinHeight = (int)(m_MinHeight*0.254);
	m_Dialog.m_MaxHeight = (int)(m_MaxHeight*0.254);
	if (m_Dialog.DoModal() == IDOK)
	{
		int h = (int)(m_Dialog.m_Height / 0.254);
		bEnableChildSizeMessage = FALSE;		// 禁止子道消息
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			pObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if ((pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) && (pObj->GetParentType() == 0))
			{
				if (((CFIDDrawObj*)pObj)->GetDrawDirect() == direct)			
				{
					rect = ((CFIDDrawObj*)pObj)->GetHeadRect();
					rect.top = rect.bottom - h;
					((CFIDDrawObj*)pObj)->SetHeadRect(rect);
				}
			}
		}
		if (m_bAutoArrangle) DoVertArrangle();
		rect = m_Rect;
		if (pDoc->m_DrawFIDHeadMode == 1)
			rect.bottom = m_ClientRect.bottom + m_ClientRect.top - m_Rect.top;
		InvalidateRect(rect);
		bEnableChildSizeMessage = TRUE;			// 允许子道消息	
	}
}

void CTrackObj::SetChildTrackWidth()
{
	CDrawObj* pObj;
	CRect rect;
	UINT m_MinWidth,m_MaxWidth;
	m_MinWidth = 0;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetParentType() == 0))
		{
			UINT w = ((CTrackObj*)pObj)->GetMinWidth();
			if (m_MinWidth < w) m_MinWidth = w;
		}
	}
	m_MaxWidth = m_Rect.Width();
	if (m_MinWidth > m_MaxWidth) m_MinWidth = m_MaxWidth;
	CWidthDialog m_Dialog;
	m_Dialog.m_MinWidth = (int)(m_MinWidth*0.254);
	m_Dialog.m_MaxWidth = (int)(m_MaxWidth*0.254);
	if (m_Dialog.DoModal() == IDOK)
	{
		int w = (int)(m_Dialog.m_Width / 0.254);
		bEnableChildSizeMessage = FALSE;		// 禁止子道消息
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			pObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetParentType() == 0))
			{
				rect = pObj->GetRect();
				rect.right = rect.left + w;
				if (rect.right > m_Rect.right)
				{
					rect.right = m_Rect.right;
					rect.left = m_Rect.right - m_Dialog.m_Width;
				}
				pObj->SetRect(rect);
			}
		}
		if (m_bAutoArrangle) DoHoriArrangle();
		rect = m_Rect;
		if (pDoc->m_DrawFIDHeadMode == 1)
			rect.bottom = m_ClientRect.bottom + m_ClientRect.top - m_Rect.top;
		InvalidateRect(rect);
		bEnableChildSizeMessage = TRUE;			// 允许子道消息	
	}
}

void CTrackObj::SetChildTrackTitleHeight()
{
	CDrawObj* pObj;
	CHeightDialog m_Dialog;
	m_Dialog.m_MinHeight = 0;
	m_Dialog.m_MaxHeight = 25;
	if (m_Dialog.DoModal() == IDOK)
	{
		int h = (int)(m_Dialog.m_Height / 0.254);
		bEnableChildSizeMessage = FALSE;		// 禁止子道消息
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			pObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetParentType() == 0))
				((CTrackObj*)pObj)->SetTitleRectHeight(h);
		}
		DoVertArrangle();
		bEnableChildSizeMessage = TRUE;			// 允许子道消息	
		pDoc->SetModifiedFlag();
		pDoc->UpdateAllViews(pDoc->m_pView,HINT_UPDATE_WINDOW);
	}
}

void CTrackObj::DrawChildTrackTitle(BOOL bShow)
{
	bEnableChildSizeMessage = FALSE;		// 禁止子道消息
	CDrawObj* pObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetParentType() == 0))
			((CTrackObj*)pObj)->OnShowTitle(bShow);
	}
	DoVertArrangle();
	pDoc->SetModifiedFlag();
	pDoc->UpdateAllViews(pDoc->m_pView,HINT_UPDATE_WINDOW);
	bEnableChildSizeMessage = TRUE;			// 允许子道消息	
}


void CTrackObj::AlignChildTrackHeadHeight()
{
	if (!(m_bChildTrackHeadAlignMax && m_bAutoArrangle)) return;
	CDrawObj* pObj;
	int nSave = -1;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*) m_ObjList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetParentType() == 0))
		{
			nSave = ((CTrackObj*)pObj)->m_bHeadHeightAuto;
			break;
		}
	}
	if (nSave == -1) 
	{
		DoVertArrangle();	// 一个道已经移走;
		return;
	}
	else if (nSave)
	{
		// 禁止子道自动调整头高度
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			pObj = (CDrawObj*) m_ObjList.GetNext(pos);
				if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetParentType() == 0))
					((CTrackObj*)pObj)->m_bHeadHeightAuto = FALSE;
		}
	}
	
	bEnableChildSizeMessage = FALSE;		// 禁止子道消息
	
	// 计算子道最大高度
	UINT m_MaxHeight = 0;
	pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*) m_ObjList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetParentType() == 0))
		{
			UINT h = ((CTrackObj*)pObj)->GetHeadRect().Height();
			if (m_MaxHeight < h) m_MaxHeight = h;
		};
	}
	
	// 对齐子道高度
	pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetParentType() == 0))
			((CTrackObj*)pObj)->SetHeadRectHeight(m_MaxHeight);
	}
	bEnableChildSizeMessage = TRUE;	// 允许子道消息	
	DoVertArrangle();				// 重新排列子对象
	if (nSave)
	{
		// 允许子道自动调整头高度
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			pObj = (CDrawObj*) m_ObjList.GetNext(pos);
			if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetParentType() == 0))
				((CTrackObj*)pObj)->m_bHeadHeightAuto = TRUE;
		}
	}
}

void CTrackObj::AutoChildTrackHeadHeight()
{
	if (!m_bChildTrackHeadHeightAuto) return;
	bEnableChildSizeMessage = FALSE;		// 禁止子道消息
	CDrawObj* pObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*) m_ObjList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetParentType() == 0))
			((CTrackObj*)pObj)->AutoHeadHeight();
	}
	bEnableChildSizeMessage = TRUE;	// 允许子道消息	
}


void CTrackObj::AutoHeadHeight()
{
	SetHeadRectHeight(GetMinHeadHeight());
}

void CTrackObj::OnChildTrackHeadHeightChange(CDrawObj* pObj)
{
	if (!bEnableChildSizeMessage) return;   
	if (pObj->GetParentType() == 0)
	{
		AutoChildTrackHeadHeight();
		AlignChildTrackHeadHeight();
	}
}

void CTrackObj::OnChildHeightChange(CDrawObj* pObj)
{
	if (!bEnableChildSizeMessage) return;   
	if (pObj->GetParentType() == 0) DoVertArrangle();
}

void CTrackObj::OnChildWidthChange(CDrawObj* pObj)
{
	if (!bEnableChildSizeMessage) return;   
	if ((m_bAutoArrangle) && (pObj->GetParentType() == 0)) DoHoriArrangle();
}

void CTrackObj::OnChildPosChange(CDrawObj* pObj)
{
	if (!bEnableChildSizeMessage) return;   
	if (pObj->GetParentType() == 0)
	{
		OnChildWidthChange(pObj);	
		OnChildHeightChange(pObj);
	}
}

void CTrackObj::OnChildRemove(CDrawObj* pObj,int nParentType)
{
	if (!bEnableChildSizeMessage) return;   
	if (nParentType == 0)
	{
		if (m_bAutoArrangle) DoHoriArrangle();
		if (pObj->IsKindOf(RUNTIME_CLASS(CTrackObj)))
			AlignChildTrackHeadHeight();
		else
			DoVertArrangle();
	}
}

void CTrackObj::SetHeadHeightAuto(BOOL bAuto)
{
	if (m_bHeadHeightAuto != bAuto)
	{
		m_bHeadHeightAuto = bAuto;
		if (bAuto) SetHeadRectHeight(GetMinHeadHeight());
	}
}

void CTrackObj::SetChildTrackHeadHeightAlignMax(BOOL bMax)
{
	if (m_bChildTrackHeadAlignMax != bMax)  
	{
		m_bChildTrackHeadAlignMax = bMax;
		if (bMax) 
		{
			AlignChildTrackHeadHeight();
			pDoc->SetModifiedFlag();
			pDoc->UpdateAllViews(pDoc->m_pView,HINT_UPDATE_WINDOW);
		}
	}
}

void CTrackObj::SetChildTrackHeadHeightAuto(BOOL bAuto)
{
	if (m_bChildTrackHeadHeightAuto == bAuto) return;
	m_bChildTrackHeadHeightAuto = bAuto;
	bEnableChildSizeMessage = FALSE;		// 禁止子道消息
	CDrawObj* pObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetParentType() == 0))
			((CTrackObj*)pObj)->SetHeadHeightAuto(bAuto);
	}
	if (bAuto) 
	{
		if (m_bChildTrackHeadAlignMax) AlignChildTrackHeadHeight();
		else DoVertArrangle();
		pDoc->SetModifiedFlag();
		pDoc->UpdateAllViews(pDoc->m_pView,HINT_UPDATE_WINDOW);
	}
	bEnableChildSizeMessage = TRUE;	// 允许子道消息	
}

void CTrackObj::SetHeadRectHeight(int nHeight)
{
	if (nHeight == m_HeadRect.Height()) return;
	int nSave = m_HeadRect.top;
	m_HeadRect.top = m_HeadRect.bottom - nHeight;
	m_TitleRect.top = m_HeadRect.top - m_TitleRect.Height();
	m_TitleRect.bottom = m_HeadRect.top;
	if (bDrawTitle) m_Rect.top = m_TitleRect.top;
	else m_Rect.top = m_HeadRect.top;
	m_DepthRect.top = m_HeadRect.top;
	m_HoriFIDRect.top = m_HeadRect.top;
	m_VertFIDRect.top = m_HeadRect.top;        
	bSetHeadHeight = TRUE;  //设置标记，使排列时不向父对象发消息
	DoVertArrangle();
	bSetHeadHeight = FALSE;  

	//如果道头高度改变，通知父对象

	if (m_HeadRect.top != nSave) 	
	{
		if (m_pParentObj == NULL)
			pDoc->OnChildTrackHeadHeightChange(this);
		else if ((m_pParentObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (m_ParentType == 0))
		{
			((CTrackObj*)m_pParentObj)->OnChildTrackHeadHeightChange(this);
		}		
	}
 }       

void CTrackObj::DoVertArrangle()
{
	int nSave = m_HeadRect.top;
	BOOL bSetHeadHeightAuto = FALSE;

	//如果自动调整头高度，头高度还没有调整，调整它

	if (m_bHeadHeightAuto && (GetMinHeadHeight() != m_HeadRect.Height()))
	{
		m_HeadRect.top = m_HeadRect.bottom - GetMinHeadHeight();
		m_TitleRect.top = m_HeadRect.top - m_TitleRect.Height();
		m_TitleRect.bottom = m_HeadRect.top;
		if (bDrawTitle) m_Rect.top = m_TitleRect.top;
		else m_Rect.top = m_HeadRect.top;
		m_DepthRect.top = m_HeadRect.top;
		m_HoriFIDRect.top = m_HeadRect.top;
		m_VertFIDRect.top = m_HeadRect.top;        
	}
	
	//首先确定子道区，保证横向曲线在子道上面

	CTrackObj* pObj = GetMaxHeightTrackObj();
	if (pObj != NULL) m_TrackRect.top = pObj->GetRect().top;
	else m_TrackRect.top = m_TrackRect.bottom;

	
	//  当横向曲线纵向不排列时：

	if (((m_HoriFIDVertArrangleMode == IDM_NONE) && (m_HoriFIDVertAlignMode == IDM_NONE)) || (!m_bAutoArrangle))
	{
		CRect rect = GetFIDUnionRect(HORDIR);

	//  如果横向曲线区底部小于子道区底部，向上移动横向曲线

		if (m_TrackRect.top != m_HoriFIDRect.bottom)
		{
			m_HoriFIDRect.bottom = m_TrackRect.top;
		    if (rect.bottom > m_TrackRect.top)
		    {
				CPoint point(0,0);
				point.y = rect.bottom - m_TrackRect.top;
				POSITION pos = m_ObjList.GetHeadPosition();
				while (pos != NULL)
				{
					CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
					if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) &&
					    (((CFIDDrawObj*)pObj)->GetDrawDirect() == HORDIR))
					{
						CRect rect1 = ((CFIDDrawObj*)pObj)->GetHeadRect();
						((CFIDDrawObj*)pObj)->SetHeadRect(rect1-point);
					}
				}	
		    }
		}	 

	//  如果横向曲线区顶部小于道头顶部，向下移动横向曲线

		else if (m_HeadRect.top > rect.top)
		{                                                            
			CPoint point(0,0);
			point.y = m_HeadRect.top - rect.top;
			if (point.y > m_HeadRect.bottom - rect.bottom) 	//  不能超过道头底部
				point.y = m_HeadRect.bottom - rect.bottom;
			POSITION pos = m_ObjList.GetHeadPosition();
			while (pos != NULL)
			{
				CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
				if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) &&
				    (((CFIDDrawObj*)pObj)->GetDrawDirect() == HORDIR))
				{
					CRect rect1 = ((CFIDDrawObj*)pObj)->GetHeadRect();
					((CFIDDrawObj*)pObj)->SetHeadRect(rect1+point);
				}
			}	
		}
	}                                      

	if ((m_VertFIDVertAlignMode == IDM_NONE) || (!m_bAutoArrangle))
	{
		CRect rect = GetFIDUnionRect(VERDIR);
		if (m_HeadRect.top > rect.top)
		{
			CPoint point(0,0);
			point.y = m_HeadRect.top - rect.top;
			if (point.y > m_HeadRect.bottom - rect.bottom) 	//  不能超过道头底部
				point.y = m_HeadRect.bottom - rect.bottom;
			POSITION pos = m_ObjList.GetHeadPosition();
			while (pos != NULL)
			{
				CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
				if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) &&
				    (((CFIDDrawObj*)pObj)->GetDrawDirect() == VERDIR))
				{
					CRect rect1 = ((CFIDDrawObj*)pObj)->GetHeadRect();
					((CFIDDrawObj*)pObj)->SetHeadRect(rect1+point);
				}
			}	
		}	
	}
	
	//  如果道头高度不够，增加道头高度
	int dh = GetMinHeadHeight() - m_HeadRect.Height();
	if (dh > 0)            
	{
		m_HeadRect.top -= dh;
		m_TitleRect.top -= dh;
		m_TitleRect.bottom -= dh;
		m_DepthRect.top -= dh;
		m_Rect.top -= dh;
	}	
	m_HoriFIDRect.top = m_HeadRect.top;
	m_VertFIDRect.top = m_HeadRect.top;       
	m_HoriFIDRect.bottom = m_TrackRect.top;

	// 排列各子对象
	if ((m_HoriFIDVertArrangleMode != IDM_NONE) && (m_bAutoArrangle))
		DoVertArrangleFID(m_HoriFIDRect, m_HoriFIDVertArrangleMode,HORDIR);
	else if ((m_HoriFIDVertAlignMode != IDM_NONE) && (m_bAutoArrangle))
		DoVertAlignFID(m_HoriFIDRect,m_HoriFIDVertAlignMode, HORDIR);	
	if ((m_VertFIDVertAlignMode != IDM_NONE) && (m_bAutoArrangle))
		DoVertAlignFID(m_VertFIDRect,m_VertFIDVertAlignMode, VERDIR);	
	if (bSetHeadHeight) return;   // 如果被SetHeadRectHeight()调用，返回.
	
	//如果道头高度改变，通知父对象
	if (m_HeadRect.top != nSave) 	
	{
		if (m_pParentObj == NULL)
			pDoc->OnChildTrackHeadHeightChange(this);
		else if ((m_pParentObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (m_ParentType == 0))
		{
			((CTrackObj*)m_pParentObj)->OnChildTrackHeadHeightChange(this);
		}		
	}
}                                        

void CTrackObj::DoHoriArrangle()
{
	SetClientRect(m_ClientRect);
}

void CTrackObj::Arrangle()
{
	DoVertArrangle();
	DoHoriArrangle();
}

void CTrackObj::SetHoriFIDSize(const CRect& rect)
{
   	ASSERT(rect.top == m_HoriFIDRect.top);
   	if (m_HoriFIDRect.Width() <= 0) return;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*) m_ObjList.GetNext(pos);
		if (((pObj != NULL) && (pObj->GetParentType() == 0)) &&
			((pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) && (((CFIDDrawObj*)pObj)->GetDrawDirect() == HORDIR)
			|| (pObj->IsKindOf(RUNTIME_CLASS(CTrackObj)))))
		{
	    	CRect rect1 = pObj->GetRect();
	    	rect1.left = rect.left + Round (rect.Width() * 1.0 / m_HoriFIDRect.Width() * (rect1.left - m_HoriFIDRect.left));
	    	rect1.right = rect.left + Round (rect.Width() * 1.0 / m_HoriFIDRect.Width() * (rect1.right - m_HoriFIDRect.left));
 			pObj->SetRect(rect1);
		} 
	}                            
}

void CTrackObj::DoHoriArrangleVertFID()
{
	CRect rect = m_ClientRect;
	if (m_VertFIDHoriArrangleMode == IDM_LEFT)
	{
		m_VertFIDRect.left = rect.left;
		m_VertFIDRect.right = rect.left + GetVertFIDMinWidth();
		if (m_VertFIDRect.right > rect.right) m_VertFIDRect.right = rect.right;
		DoHoriArrangleFID(m_VertFIDRect,IDM_LEFT,VERDIR);
	}
	else if (m_VertFIDHoriArrangleMode == IDM_RIGHT)
	{
		m_VertFIDRect.right = rect.right;
		m_VertFIDRect.left = rect.right - GetVertFIDMinWidth();
		if (m_VertFIDRect.left < rect.left) m_VertFIDRect.left = rect.left;
		DoHoriArrangleFID(m_VertFIDRect,IDM_RIGHT,VERDIR);
	}
}

void CTrackObj::SetClientRect(const CRect& rect)
{
	CRect rect1;                                 
	ASSERT(rect.top == m_ClientRect.top);
	CPoint dp(0,0);
	if (!m_bAutoArrangle)
	{
		CRect rect1 = GetUnionRect();
		ASSERT(rect.Width() >= rect1.Width());
		int dw;
		if (m_ClientRect.Width() == rect1.Width())
			dw = 0;
		else
		{
			double k;
			k = 1.0 * (rect.Width() - rect1.Width()) / (m_ClientRect.Width() - rect1.Width());
			dw = Round((rect1.left - m_ClientRect.left) * k);
		}	
		if (rect1.Width() + dw > rect.Width()) dw = rect.Width() - rect1.Width();
		dp.x = rect.left + dw - rect1.left;
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*) m_ObjList.GetNext(pos);
			if ((pObj != NULL) && (pObj->GetParentType() == 0))
    			pObj->SetRect(pObj->GetRect()+dp);
		}                     
		rect1 = m_HeadRect;
		rect1.left = rect.left;	
		rect1.right = rect.right;	
		m_HoriFIDRect = m_TrackRect = m_VertFIDRect = rect1;
	}
	else if ((m_VertFIDHoriArrangleMode == IDM_NONE) || (m_VertFIDHoriArrangleMode == IDM_JUST))
	{
		if ((GetFIDCount(VERDIR) > 0) && (m_VertFIDHoriArrangleMode == IDM_NONE))
		{
			CRect rect1 = GetFIDUnionRect(VERDIR);
			ASSERT(rect.Width() >= rect1.Width());
			int dw;
			if (m_VertFIDRect.Width() == rect1.Width())
				dw = 0;
			else
			{
				double k = 1.0 * (rect.Width() - rect1.Width()) / (m_VertFIDRect.Width() - rect1.Width());
				dw = Round((rect1.left - m_VertFIDRect.left) * k);
			}
			if (rect1.Width() + dw > rect.Width()) 
				dw = rect.Width() - rect1.Width();
			dp.x = rect.left + dw - rect1.left;
			POSITION pos = m_ObjList.GetHeadPosition();
			while (pos != NULL)
			{
				CDrawObj* pObj = (CDrawObj*) m_ObjList.GetNext(pos);
				if((pObj != NULL) && (pObj->GetParentType() == 0))
				{
					if ((pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) && (((CFIDDrawObj*)pObj)->GetDrawDirect() == VERDIR))
					pObj->SetRect(pObj->GetRect()+dp);
				}                           
			}	
		}
		if (m_VertFIDHoriArrangleMode == IDM_JUST) DoHoriArrangleFID(rect,IDM_JUST,VERDIR);
		m_VertFIDRect.left = rect.left;	
		m_VertFIDRect.right = rect.right;
		CRect rect1 = rect;
		rect1.top = m_HoriFIDRect.top;
		rect1.bottom = m_HoriFIDRect.bottom;
		SetHoriFIDSize(rect1);
		m_HoriFIDRect.left = rect.left;	
		m_HoriFIDRect.right = rect.right;	
		m_TrackRect.left = rect.left;	
		m_TrackRect.right = rect.right;	
		if (m_HoriFIDHoriArrangleMode != IDM_NONE) DoHoriArrangleFID(m_HoriFIDRect,m_HoriFIDHoriArrangleMode,HORDIR);
		else if (m_HoriFIDHoriAlignMode != IDM_NONE) DoHoriAlignFID(m_HoriFIDRect,m_HoriFIDHoriAlignMode,HORDIR);
		if (m_TrackHoriArrangleMode != IDM_NONE) DoHoriArrangleTrack(m_TrackRect,m_TrackHoriArrangleMode);
	}                                    
	else if (m_VertFIDHoriArrangleMode == IDM_LEFT)
	{
		m_VertFIDRect.left = rect.left;
		m_VertFIDRect.right = rect.left + GetVertFIDMinWidth();
		if (m_VertFIDRect.right > rect.right) m_VertFIDRect.right = rect.right;
		DoHoriArrangleFID(m_VertFIDRect,IDM_LEFT,VERDIR);
		CRect rect2 = m_VertFIDRect;
		rect2.top = m_HoriFIDRect.top;
		rect2.bottom = m_HoriFIDRect.bottom;
		rect2.right = rect.right;
		rect2.left = m_VertFIDRect.right;
		int w = GetHoriFIDMinWidth();
		int w1 = GetTrackMinWidth();
		if (w < w1) w = w1;
		if (rect2.Width() < w) rect2.left = rect2.right - w;
		if (rect2.left < rect.left) rect2.left = rect.left;
		SetHoriFIDSize(rect2);
		m_HoriFIDRect.left = rect2.left;	
		m_HoriFIDRect.right = rect2.right;	
		m_TrackRect.left = rect2.left;	
		m_TrackRect.right = rect2.right;	
		if (m_HoriFIDHoriArrangleMode != IDM_NONE) DoHoriArrangleFID(m_HoriFIDRect,m_HoriFIDHoriArrangleMode,HORDIR);
		else if (m_HoriFIDHoriAlignMode != IDM_NONE) DoHoriAlignFID(m_HoriFIDRect,m_HoriFIDHoriAlignMode,HORDIR);
		if (m_TrackHoriArrangleMode != IDM_NONE) DoHoriArrangleTrack(m_TrackRect,m_TrackHoriArrangleMode);
	}
	else if (m_VertFIDHoriArrangleMode == IDM_RIGHT)
	{
		m_VertFIDRect.right = rect.right;
		m_VertFIDRect.left = rect.right - GetVertFIDMinWidth();
		if (m_VertFIDRect.left < rect.left) m_VertFIDRect.left = rect.left;
		DoHoriArrangleFID(m_VertFIDRect,IDM_RIGHT,VERDIR);
		CRect rect2 = m_VertFIDRect;
		rect2.top = m_HoriFIDRect.top;
		rect2.bottom = m_HoriFIDRect.bottom;
		rect2.left = rect.left;
		rect2.right = m_VertFIDRect.left;
		int w = GetHoriFIDMinWidth();
		int w1 = GetTrackMinWidth();
		if (w < w1) w = w1;
		if (rect2.Width() < w) rect2.right = rect2.left + w;
		if (rect2.right > rect.right) rect2.right = rect.right;
		SetHoriFIDSize(rect2);
		m_HoriFIDRect.left = rect2.left;	
		m_HoriFIDRect.right = rect2.right;	
		m_TrackRect.left = rect2.left;	
		m_TrackRect.right = rect2.right;	
		if (m_HoriFIDHoriArrangleMode != IDM_NONE) DoHoriArrangleFID(m_HoriFIDRect,m_HoriFIDHoriArrangleMode,HORDIR);
		else if (m_HoriFIDHoriAlignMode != IDM_NONE) DoHoriAlignFID(m_HoriFIDRect,m_HoriFIDHoriAlignMode,HORDIR);
		if (m_TrackHoriArrangleMode != IDM_NONE) DoHoriArrangleTrack(m_TrackRect,m_TrackHoriArrangleMode);
	}                                   
	m_ClientRect = rect;
	m_HeadRect.left = rect.left;
	m_HeadRect.right = rect.right;
}

void CTrackObj::SetRect(const CRect& rect)
{
	CPoint point;
	int wDepth,wd;            
	ASSERT(rect.Height() == m_Rect.Height());
	if (rect == m_Rect) return;
	m_pGrid->SetRect(rect);
	point.x = rect.left - m_Rect.left;
	point.y = rect.top - m_Rect.top;
	if (!bDrawDepth) wDepth = 0;
	else wDepth = m_DepthRect.Width();
	m_DepthRect += point; 
	if (m_DepthTrackPosition == IDM_CENTER)
	{
		int width = m_DepthRect.Width();
		m_DepthRect.left = (rect.right + rect.left - width) / 2;
		m_DepthRect.right = m_DepthRect.left + width;
		wDepth = 0;
	}              
	else if (m_DepthTrackPosition == IDM_RIGHT)
	{
		m_DepthRect.left = rect.right - m_DepthRect.Width();
		m_DepthRect.right = rect.right;
	}
	wd = rect.Width() - wDepth;
	m_TitleRect += point; 
	m_HeadRect += point;
	m_HeadRect.right = m_HeadRect.left + wd;
	m_TitleRect.right = rect.right;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*) m_ObjList.GetNext(pos);
		if (pObj != NULL) 
		{
			CPoint point1 = point;
			point1.x  += (wd - m_ClientRect.Width()) / 2;
			if (pObj->GetParentType() == 0) continue;
			CRect rect1 = pObj->GetRect() + point1;
			if (pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) 
			{	
				((CFIDDrawObj*)pObj)->SetHeadRect(((CFIDDrawObj*)pObj)->GetHeadRect()+point1);
			}	
			pObj->SetRect(rect1);
		}	
	}
    if ((point.y > 0) || (rect.Width() == m_Rect.Width())) 
    {
		CPoint point1 = point;                       
		if (rect.Width() != m_Rect.Width()) point1.x = 0;
		pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*) m_ObjList.GetNext(pos);
			if (pObj != NULL) 
			{
				if (pObj->GetParentType() != 0) continue;
				CRect rect1 = pObj->GetRect() + point1;
				if (pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) 
				{	
					((CFIDDrawObj*)pObj)->SetHeadRect(((CFIDDrawObj*)pObj)->GetHeadRect()+point1);
				}	
				pObj->SetRect(rect1);
			}	
		}	
		m_HoriFIDRect += point1;
		m_VertFIDRect += point1;
		m_TrackRect += point1;
		m_ClientRect += point1; 		
	}   
	if (rect.Width() != m_Rect.Width())
	{
		CRect rect1 = m_ClientRect;
		rect1 += point;
		rect1.right = rect1.left + wd;
		rect1.bottom = rect.bottom;    
		SetClientRect(rect1);
	}                           
	m_Rect = rect;
	m_LayerFlush=TRUE;
}
        
void CTrackObj::SetDepth(double sdep,double edep)
{                           
	CPoint point(0,0);
	point.y = m_ClientRect.top - DepthToYCoor(sdep);
	m_PlotSdep = sdep; m_PlotEdep = edep;
	m_Rect.bottom = DepthToYCoor(m_PlotEdep);
	m_ClientRect.bottom = m_Rect.bottom;
	m_DepthRect.bottom = m_Rect.bottom;
	POSITION pos = m_ObjList.GetTailPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_ObjList.GetPrev(pos);
		if (pObj != NULL)
		{
			if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetParentType() == 0))
				((CTrackObj*)pObj)->SetDepth(m_PlotSdep,m_PlotEdep);
			else if	((pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj)))  && (pObj->GetParentType() == 0))
				((CFIDDrawObj*)pObj)->SetDepth(m_PlotSdep,m_PlotEdep);
			else 	           
				pObj->SetRect(pObj->GetRect()+point);
		}	
	}               
	m_pGrid->SetDepth(sdep,edep);
	m_LayerFlush=TRUE;
}

void CTrackObj::OnChangeDepC(double depc, int flag)
{
	CPoint pt(0,0);
	pt.y = Round ((depc / pDoc->m_DepC -1) * m_ClientRect.top);
	m_Rect += pt; m_DepthRect+= pt; m_HeadRect += pt; m_TitleRect += pt;
	m_ClientRect += pt; m_HoriFIDRect += pt; m_VertFIDRect += pt; m_TrackRect += pt;
	m_Rect.bottom = DepthToYCoor(m_PlotEdep);
	m_ClientRect.bottom = m_Rect.bottom;
	m_DepthRect.bottom = m_Rect.bottom;
	CDrawObj::OnChangeDepC(depc,flag);
	m_pGrid->SetDepC(pDoc->m_DepC);
}


CFIDDrawObj* CTrackObj::GetMaxWidthFIDObj(int direct)
{                       
	int w = 0;  
	CFIDDrawObj* pObj1 = NULL;
	CFIDDrawObj* pObj;
	CDrawObj* pDrawObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((!pDrawObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) || (pDrawObj->GetParentType() != 0))continue;
		pObj = (CFIDDrawObj*) pDrawObj;
		if ((pObj->GetDrawDirect() == direct) && (pObj->GetHeadRect().Width() > w)) 
		{
			pObj1 = pObj; w = pObj->GetHeadRect().Width();
		};
	}
	return pObj1;
}

CFIDDrawObj* CTrackObj::GetMinWidthFIDObj(int direct)
{                       
	int w = 20000;  
	CFIDDrawObj* pObj1 = NULL;
	CFIDDrawObj* pObj;
	CDrawObj* pDrawObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((!pDrawObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) || (pDrawObj->GetParentType() != 0)) continue;
		pObj = (CFIDDrawObj*) pDrawObj;
		if ((pObj->GetDrawDirect() == direct) && (pObj->GetHeadRect().Width() < w)) 
		{
			pObj1 = pObj; w = pObj->GetHeadRect().Width();
		};
	}
	return pObj1;
}                         

CFIDDrawObj* CTrackObj::GetMaxHeightFIDObj(int direct)
{                       
	int h = 0;  
	CFIDDrawObj* pObj1 = NULL;
	CFIDDrawObj* pObj;
	CDrawObj* pDrawObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((!pDrawObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) || (pDrawObj->GetParentType() != 0)) continue;
		pObj = (CFIDDrawObj*) pDrawObj;
		if ((pObj->GetDrawDirect() == direct) && (pObj->GetHeadRect().Height() > h)) 
		{
			pObj1 = pObj; h = pObj->GetHeadRect().Height();
		};
	}
	return pObj1;
}

CFIDDrawObj* CTrackObj::GetMinHeightFIDObj(int direct)
{                       
	int h = 20000;  
	CFIDDrawObj* pObj1 = NULL;
	CFIDDrawObj* pObj;
	CDrawObj* pDrawObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((!pDrawObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) || (pDrawObj->GetParentType() != 0)) continue;
		pObj = (CFIDDrawObj*) pDrawObj;
		if ((pObj->GetDrawDirect() == direct) && (pObj->GetHeadRect().Height() < h)) 
		{
			pObj1 = pObj; h = pObj->GetHeadRect().Height();
		};
	}
	return pObj1;
}               

int CTrackObj::GetFIDCount(int direct)
{
	int i=0;
	CDrawObj* pObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) && (pObj->GetParentType() == 0)
		&& (((CFIDDrawObj*)pObj)->GetDrawDirect() == direct)) i++;
	}                                                             
	return i;
}           

CRect CTrackObj::GetFIDUnionRect(int direct)
{                           
	CRect rect;
	rect.left = rect.top = 20000; 
	rect.right = rect.bottom = 0;
	int i=0;
	CDrawObj* pDrawObj;
	CFIDDrawObj* pObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((!pDrawObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) || (pDrawObj->GetParentType() != 0)) continue;
		pObj = (CFIDDrawObj*) pDrawObj;
		if (pObj->GetDrawDirect() == direct)
		{
			if (i == 0) {rect = pObj->GetHeadRect();i=1;}
			else rect |= pObj->GetHeadRect();
		}
	}
	if (rect.left > rect.right) rect.left = rect.right;                  
	if (rect.top > rect.bottom) rect.top = rect.bottom;                  
	return rect;
} 
          

CFIDDrawObj* CTrackObj::GetBottomFIDObj(int direct)
{                       
	int yb = 0;  
	CFIDDrawObj* pObj1 = NULL;
	CFIDDrawObj* pObj;
	CDrawObj* pDrawObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((!pDrawObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) || (pDrawObj->GetParentType() != 0)) continue;
		pObj = (CFIDDrawObj*) pDrawObj;
		if ((pObj->GetDrawDirect() == direct) && (pObj->GetHeadRect().bottom > yb)) 
		{
			pObj1 = pObj; yb = pObj->GetHeadRect().bottom;
		};
	}
	return pObj1;
}

CFIDDrawObj* CTrackObj::GetTopFIDObj(int direct)
{                       
	int yt = 20000;  
	CFIDDrawObj* pObj1 = NULL;
	CFIDDrawObj* pObj;
	CDrawObj* pDrawObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((!pDrawObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) || (pDrawObj->GetParentType() != 0)) continue;
		pObj = (CFIDDrawObj*) pDrawObj;
		if ((pObj->GetDrawDirect() == direct) && (pObj->GetHeadRect().top < yt)) 
		{
			pObj1 = pObj; yt = pObj->GetHeadRect().top;
		};
	}
	return pObj1;
}



CFIDDrawObj* CTrackObj::GetLeftFIDObj(int direct)
{                       
	int xl = 20000;  
	CFIDDrawObj* pObj1 = NULL;
	CFIDDrawObj* pObj;
	CDrawObj* pDrawObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((!pDrawObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) || (pDrawObj->GetParentType() != 0)) continue;
		pObj = (CFIDDrawObj*) pDrawObj;
		if ((pObj->GetDrawDirect() == direct) && (pObj->GetHeadRect().left < xl)) 
		{
			pObj1 = pObj; xl = pObj->GetHeadRect().left;
		};
	}
	return pObj1;
}

CFIDDrawObj* CTrackObj::GetRightFIDObj(int direct)
{                       
	int xr = 0;  
	CFIDDrawObj* pObj1 = NULL;
	CFIDDrawObj* pObj;
	CDrawObj* pDrawObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((!pDrawObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) || (pDrawObj->GetParentType() != 0)) continue;
		pObj = (CFIDDrawObj*) pDrawObj;
		if ((pObj->GetDrawDirect() == direct) && (pObj->GetHeadRect().right > xr)) 
		{
			pObj1 = pObj; xr = pObj->GetHeadRect().right;
		};
	}
	return pObj1;
}

CFIDDrawObj* CTrackObj::GetMinBottomFIDObj(int direct)
{                       
	int yb = 20000;  
	CFIDDrawObj* pObj1 = NULL;
	CFIDDrawObj* pObj;
	CDrawObj* pDrawObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((!pDrawObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) || (pDrawObj->GetParentType() != 0)) continue;
		pObj = (CFIDDrawObj*) pDrawObj;
		if ((pObj->GetDrawDirect() == direct) && (pObj->GetHeadRect().bottom < yb)) 
		{
			pObj1 = pObj; yb = pObj->GetHeadRect().bottom;
		};
	}
	return pObj1;
}

CFIDDrawObj* CTrackObj::GetMaxTopFIDObj(int direct)
{                       
	int yt = 0;  
	CFIDDrawObj* pObj1 = NULL;
	CFIDDrawObj* pObj;
	CDrawObj* pDrawObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((!pDrawObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) || (pDrawObj->GetParentType() != 0)) continue;
		pObj = (CFIDDrawObj*) pDrawObj;
		if ((pObj->GetDrawDirect() == direct) && (pObj->GetHeadRect().top > yt)) 
		{
			pObj1 = pObj; yt = pObj->GetHeadRect().top;
		};
	}
	return pObj1;
}



CFIDDrawObj* CTrackObj::GetMaxLeftFIDObj(int direct)
{                       
	int xl = 0;  
	CFIDDrawObj* pObj1 = NULL;
	CFIDDrawObj* pObj;
	CDrawObj* pDrawObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((!pDrawObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) || (pDrawObj->GetParentType() != 0)) continue;
		pObj = (CFIDDrawObj*) pDrawObj;
		if ((pObj->GetDrawDirect() == direct) && (pObj->GetHeadRect().left > xl)) 
		{
			pObj1 = pObj; xl = pObj->GetHeadRect().left;
		};
	}
	return pObj1;
}

CFIDDrawObj* CTrackObj::GetMinRightFIDObj(int direct)
{                       
	int xr = 20000;  
	CFIDDrawObj* pObj1 = NULL;
	CFIDDrawObj* pObj;
	CDrawObj* pDrawObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((!pDrawObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) || (pDrawObj->GetParentType() != 0)) continue;
		pObj = (CFIDDrawObj*) pDrawObj;
		if ((pObj->GetDrawDirect() == direct) && (pObj->GetHeadRect().right < xr)) 
		{
			pObj1 = pObj; xr = pObj->GetHeadRect().right;
		};
	}
	return pObj1;
}

int CTrackObj::GetFIDTotalWidth(int direct)
{
	int w=0;
	CFIDDrawObj* pObj;
	CDrawObj* pDrawObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((!pDrawObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) || (pDrawObj->GetParentType() != 0)) continue;
		pObj = (CFIDDrawObj*) pDrawObj;
		if (pObj->GetDrawDirect() == direct)
			w += pObj->GetHeadRect().Width();
	}                  
	return w;
}         

int CTrackObj::GetFIDTotalHeight(int direct)
{
	int h=0;
	CFIDDrawObj* pObj;
	CDrawObj* pDrawObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((!pDrawObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) || (pDrawObj->GetParentType() != 0)) continue;
		pObj = (CFIDDrawObj*) pDrawObj;
		if (pObj->GetDrawDirect() == direct)
			h += pObj->GetHeadRect().Height();
	}                  
	return h;
}


CTrackObj* CTrackObj::GetLeftTrackObj()
{                       
	int xl = 20000;  
	CTrackObj* pObj1 = NULL;
	CDrawObj* pObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetRect().left < xl) && (pObj->GetParentType() == 0))
		{
			pObj1 = (CTrackObj*) pObj; 
			xl = pObj->GetRect().left;
		};
	}
	return pObj1;
}

CTrackObj* CTrackObj::GetRightTrackObj()
{                       
	int xr = 0;  
	CTrackObj* pObj1 = NULL;
	CDrawObj* pObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetRect().right > xr) && (pObj->GetParentType() == 0))
		{
			pObj1 = (CTrackObj*)pObj; 
			xr = pObj->GetRect().right;
		};
	}
	return pObj1;
}

CTrackObj* CTrackObj::GetMaxLeftTrackObj()
{                       
	int xl = 0;  
	CTrackObj* pObj1 = NULL;
	CDrawObj* pObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetRect().left > xl) && (pObj->GetParentType() == 0))
		{
			pObj1 = (CTrackObj*) pObj; 
			xl = pObj->GetRect().left;
		};
	}
	return pObj1;
}

CTrackObj* CTrackObj::GetMinRightTrackObj()
{                       
	int xr = 20000;  
	CTrackObj* pObj1 = NULL;
	CDrawObj* pObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetRect().right < xr) && (pObj->GetParentType() == 0))
		{
			pObj1 = (CTrackObj*) pObj; 
			xr = pObj->GetRect().right;
		};
	}
	return pObj1;
}

CTrackObj* CTrackObj::GetMaxWidthTrackObj()
{                       
	int w = 0;  
	CTrackObj* pObj1 = NULL;
	CDrawObj* pObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetRect().Width() > w) && (pObj->GetParentType() == 0))
		{
			pObj1 = (CTrackObj*) pObj; 
			w = pObj->GetRect().Width();
		};
	}
	return pObj1;
}

CTrackObj* CTrackObj::GetMinWidthTrackObj()
{                       
	int w = 20000;  
	CTrackObj* pObj1 = NULL;
	CDrawObj* pObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetRect().Width() < w) && (pObj->GetParentType() == 0))
		{
			pObj1 = (CTrackObj*) pObj; 
			w = pObj->GetRect().Width();
		};
	}
	return pObj1;
}

CTrackObj* CTrackObj::GetMaxHeightTrackObj()
{                       
	int h = 0;  
	CTrackObj* pObj1 = NULL;
	CDrawObj* pObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetParentType() == 0))
		{
			int h1 = ((CTrackObj*)pObj)->GetHeadRect().bottom - pObj->GetRect().top;
			if (h1 > h)
			{
				pObj1 = (CTrackObj*)pObj; 
				h = h1;
			}	
		};
	}
	return pObj1;
}

CTrackObj* CTrackObj::GetMinHeightTrackObj()
{                       
	int h = 20000;  
	CTrackObj* pObj1 = NULL;
	CDrawObj* pObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetParentType() == 0))
		{
			int h1 = ((CTrackObj*)pObj)->GetHeadRect().bottom - pObj->GetRect().top;
			if (h1 < h)
			{
				pObj1 = (CTrackObj*) pObj; 
				h = h1;
			}	
		};
	}
	return pObj1;
}

int CTrackObj::GetTrackTotalWidth()
{                       
	int w=0;  
	CDrawObj* pObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetParentType() == 0))
			w += pObj->GetRect().Width();
	}
	return w;
}           

CRect CTrackObj::GetTrackUnionRect()
{                           
	CRect rect;
	rect.left = rect.top = 20000; 
	rect.right = rect.bottom = 0;
	CDrawObj* pObj=NULL;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetParentType() == 0))
		{
			if (rect.left > pObj->GetRect().left) rect.left = pObj->GetRect().left;
			if (rect.right < pObj->GetRect().right) rect.right = pObj->GetRect().right;
			if (rect.top > pObj->GetRect().top) rect.top = pObj->GetRect().top;
			if (rect.bottom < ((CTrackObj*)pObj)->GetHeadRect().bottom) rect.bottom = ((CTrackObj*)pObj)->GetHeadRect().bottom;
		}
		
	}
	if (rect.left > rect.right) rect.left = rect.right;                  
	if (rect.top > rect.bottom) rect.top = rect.bottom;                  
	return rect;              
} 

int CTrackObj::GetTrackCount()
{
	int i=0;
	CDrawObj* pObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetParentType() == 0)) i++;
	}                                                             
	return i;
}           

int CTrackObj::GetVertFIDMinWidth()
{
	int w = 0;
	if (m_VertFIDHoriArrangleMode != IDM_NONE)
		w = GetFIDTotalWidth(VERDIR) ;
	else
		w = GetFIDUnionRect(VERDIR).Width();
	return w;
}       

int CTrackObj::GetVertFIDMinHeight()
{
	CDrawObj* pDrawObj;
	CFIDDrawObj* pObj;
	int h = 0;
	if (m_VertFIDVertAlignMode == IDM_NONE)
		h = GetFIDUnionRect(VERDIR).Height();
	else  if (m_VertFIDVertAlignMode == IDM_JUST)
	{
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if ((pDrawObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) && (pDrawObj->GetParentType() == 0))
			{
				pObj = (CFIDDrawObj*) pDrawObj;
				if (pObj->GetDrawDirect() == VERDIR)
				{
					int h1 = pObj->GetMinHeight();
					if (h1 > h) h = h1;
				}	                   
			}
		}
	}   
	else
	{
		pObj = GetMaxHeightFIDObj(VERDIR);
		if (pObj != NULL) h = pObj->GetHeadRect().Height();
	}
	return h;
}           

int CTrackObj::GetHoriFIDMinHeight()
{
	CDrawObj* pDrawObj;
	CFIDDrawObj* pObj;
	int h = 0;
	if (m_HoriFIDVertArrangleMode != IDM_NONE)
		h = GetFIDTotalHeight(HORDIR); 
	else if (m_VertFIDVertAlignMode == IDM_NONE)
		h = GetFIDUnionRect(HORDIR).Height();
	else  if (m_VertFIDVertAlignMode == IDM_JUST)
	{
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if ((pDrawObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) && (pDrawObj->GetParentType() == 0))
			{
				pObj = (CFIDDrawObj*) pDrawObj;
				if (pObj->GetDrawDirect() == HORDIR)
				{
					int h1 = pObj->GetMinHeight();
					if (h1 > h) h = h1;
				}	                   
			}
		}
	}   
	else
	{
		pObj = GetMaxHeightFIDObj(HORDIR);
		if (pObj != NULL) h = pObj->GetHeadRect().Height();
	}
	return h;
}           

int CTrackObj::GetHoriFIDMinWidth()
{
	int w = 0;
	CDrawObj* pDrawObj;
	CFIDDrawObj* pObj;
	double x=0;
	if ((m_HoriFIDHoriArrangleMode != IDM_NONE) || (m_HoriFIDHoriAlignMode != IDM_JUST))
	{
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if ((pDrawObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) && (pDrawObj->GetParentType() == 0))
			{
				pObj = (CFIDDrawObj*) pDrawObj;
				if (pObj->GetDrawDirect() == HORDIR)
				{
					int w1 = pObj->GetHeadRect().Width();
					ASSERT(w1>0);
					double x1 = ((double)pObj->GetMinWidth()) / w1;
					if (x < x1) x = x1;
				}	                   
			}
		}
		if (m_HoriFIDHoriArrangleMode != IDM_NONE ) w = Round(x * GetFIDTotalWidth(HORDIR));		
		else w = Round(x*m_HoriFIDRect.Width());
	}	
	else 
	{
		w = 9999;
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if ((pDrawObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) && (pDrawObj->GetParentType() == 0))
			{
				pObj = (CFIDDrawObj*) pDrawObj;
				if (pObj->GetDrawDirect() == HORDIR)
				{
					int w1 = pObj->GetMinWidth();
					if (w > w1) w = w1;
				}	                   
			}
		}
		if (w == 9999) w = 0;
	}                                          
	return w;
}           

int CTrackObj::GetTrackMinWidth()
{
	CDrawObj* pDrawObj;
	CTrackObj* pObj;
	double x=0,x1;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pDrawObj->IsKindOf(RUNTIME_CLASS(CTrackObj)))  && (pDrawObj->GetParentType() == 0))
		{
			pObj = (CTrackObj*) pDrawObj;
			int w = pObj->GetRect().Width();
			ASSERT(w>0);
			x1 = pObj->GetMinWidth() / w;
			if (x < x1) x = x1;
		}
	}    
	if (m_TrackHoriArrangleMode == IDM_NONE)
		return Round(x * m_TrackRect.Width());		
	else
		return Round(x * GetTrackTotalWidth());	
}

CRect CTrackObj::GetUnionRect()
{
	CRect rect(0,0,0,0);
	CRect rect1,rect2;
	rect1 = GetFIDUnionRect(VERDIR);
	if (!rect1.IsRectEmpty())
	{
		rect = rect1;
	}
	rect1 = GetTrackUnionRect();
	if (!rect1.IsRectEmpty())
	{
		if (!rect.IsRectEmpty()) rect |= rect1;
		else rect = rect1;
	}
	rect2 = GetFIDUnionRect(HORDIR);
	if (!rect2.IsRectEmpty())
	{
		if (!rect.IsRectEmpty()) rect |= rect2;
		else rect = rect2;
	}          
	return rect;
}

int CTrackObj::GetMinHeadWidth()
{
	int w;
	if (!m_bAutoArrangle) 
	{
		w = GetUnionRect().Width();
		if ((!bDrawDepth) && (w < 10)) w = 10;
		return w;
	}
	w = GetHoriFIDMinWidth();
	int w1 = GetTrackMinWidth();
	if (w < w1) w = w1;
	w1 = GetVertFIDMinWidth();
	if ((m_VertFIDHoriArrangleMode == IDM_LEFT) || (m_VertFIDHoriArrangleMode == IDM_RIGHT))
		w += w1;
	else if (w < w1) w = w1;	
	if ((!bDrawDepth) && (w < 10)) w = 10;
	return w;
}                                            

int CTrackObj::GetMinWidth()
{
	int w = GetMinHeadWidth();
	if ((bDrawDepth) && (m_DepthTrackPosition != IDM_CENTER))  w += m_DepthRect.Width();
	if (w < 10) w = 10;
	return w;
}       

int CTrackObj::GetMinHeadHeight()
{
	int h;
	if (!m_bAutoArrangle) 
	{
		h = GetUnionRect().Height();
		if (h < 20) h = 20;
		return h;
	}
	h = GetHoriFIDMinHeight();      
	CTrackObj* pObj = GetMaxHeightTrackObj();
	if (pObj != NULL) h += (pObj->GetHeadRect().bottom - pObj->GetRect().top);
	int h1 = GetVertFIDMinHeight();
	if (h < h1) h = h1;	
	if (h < 20) h = 20;
	return h;
}            

int CTrackObj::GetMinHeight()
{
	int h = GetMinHeadHeight();
	if (bDrawTitle) h += m_TitleRect.Height();
	return h;
}                                             

void CTrackObj::DoVertArrangleFID(CRect rect, int nArrangleMode, int direct)
{
	if (nArrangleMode == IDM_NONE) return;
	int k = GetFIDCount(direct);
	if (k == 0) return;
	CObList m_List;
	CFIDDrawObj* pObj;       
	int h = GetFIDTotalHeight(direct);
	if (nArrangleMode == IDM_TOP)
	{
		CRect rect1 = rect;
		if (h > rect.Height() && (k > 1))
		{
			pObj = GetMaxTopFIDObj(direct);
			if (pObj!= NULL)
			{                              
				int d =  rect.Height() - pObj->GetHeadRect().Height();
				ASSERT(d>=0);
				int k1,k2;
				if (k == 1)	k1 = k2 = 0; 
				else
				{
					k1 = d / (k-1);
					k2 = d - k1 * (k-1);         
				}
				d = 0;
				while  ((pObj = GetTopFIDObj(direct)) != NULL) 
				{ 
					m_List.AddTail(pObj);
					RemoveObj(pObj);         
					rect1.bottom = rect1.top + pObj->GetHeadRect().Height();
					rect1.left = pObj->GetHeadRect().left;
					rect1.right = pObj->GetHeadRect().right; 
					if (rect1.bottom > rect.bottom)
					{
					    CRect rect2 = rect1;
					    rect2.top = rect.bottom - pObj->GetHeadRect().Height();
						rect2.bottom = rect.bottom;
						rect1.bottom = rect.bottom;
						pObj->SetHeadRect(rect2);
					}
					else pObj->SetHeadRect(rect1);
					rect1.top += k1;
					if (k2 > 0)
					{
						rect1.top++;
						k2 --;
					}            
					if (rect1.top > rect1.bottom)
					{
						k2 += rect1.top - rect1.bottom;
						rect1.top = rect1.bottom;
					}
				}	
				m_ObjList.AddHead(&m_List);
				OrderObj();
				return;
			}
		}		
		while  ((pObj = GetTopFIDObj(direct)) != NULL) 
		{ 
			m_List.AddTail(pObj);
			RemoveObj(pObj);         
			rect1.bottom = rect1.top + pObj->GetHeadRect().Height();
			rect1.left = pObj->GetHeadRect().left;
			rect1.right = pObj->GetHeadRect().right; 
			pObj->SetHeadRect(rect1);
			rect1.top = rect1.bottom;
		}	
		m_ObjList.AddHead(&m_List);
		OrderObj();
	}
	else if (nArrangleMode == IDM_BOTTOM)
	{
		CRect rect1 = rect;
		if (h > rect.Height())
		{
			pObj = GetMinBottomFIDObj(direct);
			if (pObj!= NULL)
			{                              
				int d =  rect.Height() - pObj->GetHeadRect().Height();
				ASSERT(d>=0);
				int k1,k2;
				if (k == 1)	k1 = k2 = 0; 
				else
				{
					k1 = d / (k-1);
					k2 = d - k1 * (k-1);         
				}
				d = 0;
				while  ((pObj = GetBottomFIDObj(direct)) != NULL) 
				{ 
					m_List.AddTail(pObj);
					RemoveObj(pObj);         
					rect1.top = rect1.bottom - pObj->GetHeadRect().Height();
					rect1.left = pObj->GetHeadRect().left;
					rect1.right = pObj->GetHeadRect().right; 
					if (rect1.top < rect.top)
					{
					    CRect rect2 = rect1;
					    rect2.bottom += rect.top + pObj->GetHeadRect().Height();
						rect2.top = rect.top;      
						rect1.top = rect.top;
						pObj->SetHeadRect(rect2);
					}
					else pObj->SetHeadRect(rect1);
					rect1.bottom -= k1;
					if (k2 > 0)
					{
						rect1.bottom--;
						k2 --;
					}            
					if (rect1.bottom < rect1.top)
					{
						k2 += rect1.top - rect1.bottom;
						rect1.bottom = rect1.top;
					}
				}	
				m_ObjList.AddHead(&m_List);
				OrderObj();
				return;
			}
		}		
		while  ((pObj = GetBottomFIDObj(direct)) != NULL) 
		{ 
			m_List.AddTail(pObj);
			RemoveObj(pObj);         
			rect1.top = rect1.bottom - pObj->GetHeadRect().Height();
			rect1.left = pObj->GetHeadRect().left;
			rect1.right = pObj->GetHeadRect().right; 
			pObj->SetHeadRect(rect1);
			rect1.bottom = rect1.top;
		}	
		m_ObjList.AddHead(&m_List);
		OrderObj();
	}   
	else if (nArrangleMode == IDM_JUST)
	{
		CRect rect1 = rect;
		CRect rect2 = rect;
		if ((h > rect.Height()) && (k > 1))
		{
			pObj = GetMaxTopFIDObj(direct);
			if (pObj!= NULL)
			{                              
				int d =  rect.Height() - pObj->GetHeadRect().Height();
				ASSERT(d>=0);
				int k1,k2;
				if (k == 1)	k1 = k2 = 0; 
				else
				{
					k1 = d / (k-1);
					k2 = d - k1 * (k-1);         
				}
				d = 0;
				while  ((pObj = GetTopFIDObj(direct)) != NULL) 
				{ 
					m_List.AddTail(pObj);
					RemoveObj(pObj);         
					rect1.bottom = rect1.top + pObj->GetHeadRect().Height();
					rect1.left = pObj->GetHeadRect().left;
					rect1.right = pObj->GetHeadRect().right; 
					pObj->SetHeadRect(rect1);
					rect1.top += k1;
					if (k2 > 0)
					{
						rect1.top++;
						k2 --;
					}            
					if (rect1.top > rect1.bottom)
					{
						k2 += rect1.top - rect1.bottom;
						rect1.top = rect1.bottom;
					}
				}	
				m_ObjList.AddHead(&m_List);
				OrderObj();
				return;
			}
		}		
		int d =  rect.Height() - h;
		ASSERT(d>=0);
		int k1,k2;
		if (k == 1)	k1 = k2 = 0; 
		else
		{
			k1 = d / (k-1);
			k2 = d - k1 * (k-1);         
		}
		while  ((pObj = GetTopFIDObj(direct)) != NULL) 
		{ 
			m_List.AddTail(pObj);
			RemoveObj(pObj);         
			rect1.bottom = rect1.top + pObj->GetHeadRect().Height();
			rect1.left = pObj->GetHeadRect().left;
			rect1.right = pObj->GetHeadRect().right; 
			if (rect1.bottom > rect.bottom)
			{
			    CRect rect3 = rect1;
			    rect3.top = rect.bottom - pObj->GetHeadRect().Height();
				rect3.bottom = rect.bottom;
				rect1.bottom = rect.bottom;
				pObj->SetHeadRect(rect3);
			}
			else pObj->SetHeadRect(rect1);
			rect1.top = rect1.bottom + k1;
			if (k2 > 0)
			{
				rect1.top++;
				k2 --;
			}            
			if ((pObj = GetBottomFIDObj(direct)) != NULL) 
			{ 
				m_List.AddTail(pObj);
				RemoveObj(pObj);         
				rect2.top = rect2.bottom - pObj->GetHeadRect().Height();
				rect2.left = pObj->GetHeadRect().left;
				rect2.right = pObj->GetHeadRect().right; 
				if (rect2.top < rect.top)
				{
				    CRect rect3 = rect2;
				    rect3.bottom = rect.top + pObj->GetHeadRect().Height();
					rect3.top = rect.top;
					rect2.top = rect.top;
					pObj->SetHeadRect(rect3);
				}
				else pObj->SetHeadRect(rect2);
				rect2.bottom = rect2.top - k1;
				if (k2 > 0)
				{
					rect2.bottom--;
					k2 --;
				}            
			}	
		}	
		m_ObjList.AddHead(&m_List);
		OrderObj();
	}
}

void CTrackObj::DoVertAlignFID(CRect rect, int nAlignMode, int direct)
{
	if (nAlignMode == IDM_NONE) return;
	POSITION pos = m_ObjList.GetHeadPosition();
	if (pos == NULL) return;
	CRect rect1;
	if (nAlignMode == IDM_TOP)         
	{
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) &&
			    (pObj->GetParentType() == 0) && (((CFIDDrawObj*)pObj)->GetDrawDirect() == direct))
			{
				rect1 = ((CFIDDrawObj*)pObj)->GetHeadRect();
				if (rect1.top == rect.top) continue;
				rect1.bottom = rect.top + rect1.Height();
				rect1.top = rect.top;
				((CFIDDrawObj*)pObj)->SetHeadRect(rect1);
			}	
		}	
	}                  
	else if (nAlignMode == IDM_BOTTOM)         
	{
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) &&
			    (pObj->GetParentType() == 0) && (((CFIDDrawObj*)pObj)->GetDrawDirect() == direct))
			{
				rect1 = ((CFIDDrawObj*)pObj)->GetHeadRect();
				if (rect1.bottom == rect.bottom) continue;
				rect1.top = rect.bottom - rect1.Height();
				rect1.bottom = rect.bottom;
				((CFIDDrawObj*)pObj)->SetHeadRect(rect1);
			}	
		}	
	}                  
	else if (nAlignMode == IDM_JUST)         
	{
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) &&
			    (pObj->GetParentType() == 0) && (((CFIDDrawObj*)pObj)->GetDrawDirect() == direct))
			{
				rect1 = ((CFIDDrawObj*)pObj)->GetHeadRect();
				if ((rect1.bottom == rect.bottom) && (rect.top == rect1.top)) continue;
				rect1.top = rect.top;
				rect1.bottom = rect.bottom;
				((CFIDDrawObj*)pObj)->SetHeadRect(rect1);
			}	
		}	
	}                  
}

void CTrackObj::DoHoriAlignFID(CRect rect, int nAlignMode, int direct)
{
	if (nAlignMode == IDM_NONE) return;
	POSITION pos = m_ObjList.GetHeadPosition();
	if (pos == NULL) return;
	CRect rect1;
	if (nAlignMode == IDM_LEFT)         
	{
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) &&
			    (pObj->GetParentType() == 0) && (((CFIDDrawObj*)pObj)->GetDrawDirect() == direct))
			{
				rect1 = ((CFIDDrawObj*)pObj)->GetHeadRect();
				if (rect1.left == rect.left) continue;
				rect1.right = rect.left + rect1.Width();
				rect1.left = rect.left;
				((CFIDDrawObj*)pObj)->SetHeadRect(rect1);
			}	
		}	
	}                  
	else if (nAlignMode == IDM_RIGHT)         
	{
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) &&
			    (pObj->GetParentType() == 0) && (((CFIDDrawObj*)pObj)->GetDrawDirect() == direct))
			{
				rect1 = ((CFIDDrawObj*)pObj)->GetHeadRect();
				if (rect1.right == rect.right) continue;
				rect1.left = rect.right - rect1.Width();
				rect1.right = rect.right;
				((CFIDDrawObj*)pObj)->SetHeadRect(rect1);
			}	
		}	
	}                  
	else if (nAlignMode == IDM_JUST)         
	{
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) &&
			    (pObj->GetParentType() == 0) && (((CFIDDrawObj*)pObj)->GetDrawDirect() == direct))
			{
				rect1 = ((CFIDDrawObj*)pObj)->GetHeadRect();
				if ((rect1.left == rect.left) && (rect.right == rect1.right)) continue;
				rect1.left = rect.left;
				rect1.right = rect.right;
				((CFIDDrawObj*)pObj)->SetHeadRect(rect1);
			}	
		}	
	}                  
}                            

void CTrackObj::DoHoriArrangleFID(CRect rect, int nArrangleMode, int direct)
{
	if (nArrangleMode == IDM_NONE) return;
	int k = GetFIDCount(direct);
	if (k == 0) return;
	CObList m_List;
	CFIDDrawObj* pObj;       
	int w = GetFIDTotalWidth(direct);
	if ((w > rect.Width()) && (direct == HORDIR))
	{
		double w1 = 0;
		CDrawObj* pDrawObj;       
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if ((pDrawObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) && (pDrawObj->GetParentType() == 0))
			{
				pObj = (CFIDDrawObj*) pDrawObj;
				if (pObj->GetDrawDirect() == HORDIR)
				{
					int w3 = pObj->GetHeadRect().Width();
					ASSERT(w3>0);
					double w2 = pObj->GetMinWidth() / w3;
					if (w1 < w2) w1 = w2;
				}	                   
			}
		}                          
		double w2 = rect.Width() / ((double)GetFIDTotalWidth(HORDIR));
		if (w1 < w2) w1 = w2;
		pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if ((pDrawObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) && (pDrawObj->GetParentType() == 0))
			{
				pObj = (CFIDDrawObj*) pDrawObj;
				if (pObj->GetDrawDirect() == HORDIR)
				{
					CRect rect1 = pObj->GetHeadRect();
					rect1.right = rect1.left + Round(rect1.Width()*w1);
					pObj->SetHeadRect(rect1);
				}	                   
			}
		}
		w = GetFIDTotalWidth(HORDIR);
	}
	if (nArrangleMode == IDM_LEFT)
	{
		CRect rect1 = rect;
		if ((w > rect.Width()) && (k > 1))
		{
			pObj = GetMaxLeftFIDObj(direct);
			if (pObj!= NULL)
			{                              
				int d =  rect.Width() - pObj->GetHeadRect().Width();
				ASSERT(d>=0);
				int k1,k2;
				if (k == 1)	k1 = k2 = 0; 
				else
				{
					k1 = d / (k-1);
					k2 = d - k1 * (k-1);         
				}
				d = 0;
				while  ((pObj = GetLeftFIDObj(direct)) != NULL) 
				{ 
					m_List.AddTail(pObj);
					RemoveObj(pObj);         
					rect1.right = rect1.left + pObj->GetHeadRect().Width();
					rect1.top = pObj->GetHeadRect().top; 
					rect1.bottom = pObj->GetHeadRect().bottom;
					if (rect1.right > rect.right)
					{
					    CRect rect2 = rect1;
					    rect2.left = rect.right - pObj->GetHeadRect().Width();
						rect2.right = rect.right;
						rect1.right = rect.right;
						pObj->SetHeadRect(rect2);
					}
					else pObj->SetHeadRect(rect1);
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
				m_ObjList.AddHead(&m_List);
				OrderObj();
				return;
			}
		}		
		while  ((pObj = GetLeftFIDObj(direct)) != NULL) 
		{ 
			m_List.AddTail(pObj);
			RemoveObj(pObj);         
			CRect rect = pObj->GetHeadRect();
			rect1.top = rect.top; rect1.bottom = rect.bottom;         
			rect1.right = rect1.left + pObj->GetHeadRect().Width();
			pObj->SetHeadRect(rect1);
			rect1.left = rect1.right;
		}	
		m_ObjList.AddHead(&m_List);
		OrderObj();
	}
	else if (nArrangleMode == IDM_RIGHT)
	{
		CRect rect1 = rect;
		if (w > rect.Width())
		{
			pObj = GetMinRightFIDObj(direct);
			if (pObj!= NULL)
			{                              
				int d =  rect.Width() - pObj->GetHeadRect().Width();
				ASSERT(d>=0);
				int k1,k2;
				if (k == 1)	k1 = k2 = 0; 
				else
				{
					k1 = d / (k-1);
					k2 = d - k1 * (k-1);         
				}
				d = 0;
				while  ((pObj = GetRightFIDObj(direct)) != NULL) 
				{ 
					m_List.AddTail(pObj);
					RemoveObj(pObj);         
					rect1.left = rect1.right - pObj->GetHeadRect().Width();
					rect1.top = pObj->GetHeadRect().top; 
					rect1.bottom = pObj->GetHeadRect().bottom;
					if (rect1.left < rect.left)
					{
					    CRect rect2 = rect1;
					    rect2.right += rect.left + pObj->GetHeadRect().Width();
						rect2.left = rect.left;      
						rect1.left = rect.left;
						pObj->SetHeadRect(rect2);
					}
					else pObj->SetHeadRect(rect1);
					rect1.right -= k1;
					if (k2 > 0)
					{
						rect1.right--;
						k2 --;
					}            
					if (rect1.right < rect1.left)
					{
						k2 += rect1.left - rect1.right;
						rect1.right = rect1.left;
					}
				}	
				m_ObjList.AddHead(&m_List);
				OrderObj();
				return;
			}
		}		
		while  ((pObj = GetRightFIDObj(direct)) != NULL) 
		{ 
			m_List.AddTail(pObj);
			RemoveObj(pObj);         
			rect1.top = pObj->GetHeadRect().top; 
			rect1.bottom = pObj->GetHeadRect().bottom;
			rect1.left = rect1.right - pObj->GetHeadRect().Width();
			pObj->SetHeadRect(rect1);
			rect1.right = rect1.left;
		}	
		m_ObjList.AddHead(&m_List);
		OrderObj();
	}   
	else if (nArrangleMode == IDM_JUST)
	{
		CRect rect1 = rect;
		CRect rect2 = rect;
		if ((w > rect.Width()) && (k > 1))
		{
			pObj = GetMaxLeftFIDObj(direct);
			if (pObj!= NULL)
			{                              
				int d =  rect.Width() - pObj->GetHeadRect().Width();
				ASSERT(d>=0);
				int k1,k2;
				if (k == 1)	k1 = k2 = 0; 
				else
				{
					k1 = d / (k-1);
					k2 = d - k1 * (k-1);         
				}
				d = 0;
				while  ((pObj = GetLeftFIDObj(direct)) != NULL) 
				{ 
					m_List.AddTail(pObj);
					RemoveObj(pObj);         
					rect1.right = rect1.left + pObj->GetHeadRect().Width();
					rect1.top = pObj->GetHeadRect().top; 
					rect1.bottom = pObj->GetHeadRect().bottom;
					pObj->SetHeadRect(rect1);
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
				m_ObjList.AddHead(&m_List);
				OrderObj();
				return;
			}
		}		
		int d =  rect.Width() - w;
		int k1,k2;
		if (k == 1)	k1 = k2 = 0; 
		else
		{
			k1 = d / (k-1);
			k2 = d - k1 * (k-1);         
		}
		while  ((pObj = GetLeftFIDObj(direct)) != NULL) 
		{ 
			m_List.AddTail(pObj);
			RemoveObj(pObj);         
			rect1.right = rect1.left + pObj->GetHeadRect().Width();
			rect1.top = pObj->GetHeadRect().top; 
			rect1.bottom = pObj->GetHeadRect().bottom;
			if (rect1.right > rect.right)
			{
			    CRect rect3 = rect1;
			    rect3.left = rect.right - pObj->GetHeadRect().Width();
				rect3.right = rect.right;
				rect1.right = rect.right;
				pObj->SetHeadRect(rect3);
			}
			else pObj->SetHeadRect(rect1);
			rect1.left = rect1.right + k1;
			if (k2 > 0)
			{
				rect1.left++;
				k2 --;
			}            
			if ((pObj = GetRightFIDObj(direct)) != NULL) 
			{ 
				m_List.AddTail(pObj);
				RemoveObj(pObj);         
				rect2.left = rect2.right - pObj->GetHeadRect().Width();
				rect2.top = pObj->GetHeadRect().top; 
				rect2.bottom = pObj->GetHeadRect().bottom;
				if (rect2.left < rect.left)
				{
				    CRect rect3 = rect2;
				    rect3.right = rect.left + pObj->GetHeadRect().Width();
					rect3.left = rect.left;
					rect2.left = rect.left;
					pObj->SetHeadRect(rect3);
				}
				else pObj->SetHeadRect(rect2);
				rect2.right = rect2.left - k1;
				if (k2 > 0)
				{
					rect2.right--;
					k2 --;
				}            
			}	
		}	
		m_ObjList.AddHead(&m_List);
		OrderObj();
	}
}

void CTrackObj::DoHoriAlignTrack(CRect rect, int nAlignMode)
{
	if (nAlignMode == IDM_NONE) return;
	POSITION pos = m_ObjList.GetHeadPosition();
	if (pos == NULL) return;
	CRect rect1;
	if (nAlignMode == IDM_LEFT)         
	{
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))))
			{
				rect1 = pObj->GetRect();
				if (rect1.left == rect.left) continue;
				rect1.right = rect.left + rect1.Width();
				rect1.left = rect.left;
				pObj->SetRect(rect1);
			}	
		}	
	}                  
	else if (nAlignMode == IDM_RIGHT)         
	{
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))))
			{
				rect1 = pObj->GetRect();
				if (rect1.right == rect.right) continue;
				rect1.left = rect.right - rect1.Width();
				rect1.right = rect.right;
				pObj->SetRect(rect1);
			}	
		}	
	}                  
	else if (nAlignMode == IDM_JUST)         
	{
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))))
			{
				rect1 = pObj->GetRect();
				if ((rect1.left == rect.left) && (rect.right == rect1.right)) continue;
				rect.left = rect.left;
				rect.right = rect.right;
				pObj->SetRect(rect1);
			}	
		}	
	}                  
}                            

void CTrackObj::DoHoriArrangleTrack(CRect rect, int nArrangleMode)
{
	if (nArrangleMode == IDM_NONE) return;
	int k = GetTrackCount();
	if (k == 0) return;
	CObList m_List;
	CTrackObj* pObj;       
	int w = GetTrackTotalWidth();
	if (w > rect.Width())
	{
		double w1 = 0;
		CDrawObj* pDrawObj;       
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if ((pDrawObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pDrawObj->GetParentType() == 0))
			{
				pObj = (CTrackObj*) pDrawObj;
				int w3 = pObj->GetRect().Width();
				ASSERT(w3>0);
				double w2 = pObj->GetMinWidth() / w3;
				if (w1 < w2) w1 = w2;
			}
		}                          
		double w2 = rect.Width() / ((double)GetTrackTotalWidth());
		if (w1 < w2) w1 = w2;
		pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			pDrawObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if ((pDrawObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pDrawObj->GetParentType() == 0))
			{
				pObj = (CTrackObj*) pDrawObj;
				CRect rect1 = pObj->GetRect();
				rect1.right = rect1.left + Round(rect1.Width()*w1);
				pObj->SetRect(rect1);
			}
		}
		w = GetTrackTotalWidth();
	}
	if (nArrangleMode == IDM_LEFT)
	{
		CRect rect1 = rect;
		if (w > rect.Width() && (k > 1))
		{
			if ((pObj = GetMaxLeftTrackObj()) != NULL)
			{                              
				int d =  rect.Width() - pObj->GetRect().Width();
				ASSERT(d>0);
				int k1 = d / (k-1);
				int k2 = d - k1 * k;         
				d = 0;
				while  ((pObj = GetLeftTrackObj()) != NULL) 
				{ 
					m_List.AddTail(pObj);
					RemoveObj(pObj);         
					rect1.right = rect1.left + pObj->GetRect().Width();
					rect1.top = pObj->GetRect().top; 
					rect1.bottom = pObj->GetRect().bottom;
					if (rect1.right > rect.right)
					{
					    CRect rect2 = rect1;
					    rect2.left = rect.right - pObj->GetRect().Width();
						rect2.right = rect.left;
						rect1.right = rect.left;
						pObj->SetRect(rect2);
					}
					else pObj->SetRect(rect1);
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
				m_ObjList.AddHead(&m_List);
				OrderObj();
				return;
			}
		}		
		while  ((pObj = GetLeftTrackObj()) != NULL) 
		{ 
			m_List.AddTail(pObj);
			RemoveObj(pObj);         
			rect1.right = rect1.left + pObj->GetRect().Width();
			rect1.top = pObj->GetRect().top; 
			rect1.bottom = pObj->GetRect().bottom;
			pObj->SetRect(rect1);
			rect1.left = rect1.right;
		}	
		m_ObjList.AddHead(&m_List);
		OrderObj();
	}
	else if (nArrangleMode == IDM_RIGHT)
	{
		CRect rect1 = rect;
		if (w > rect.Width())
		{
			if ((pObj = GetMinRightTrackObj()) != NULL)
			{                              
				int d =  rect.Width() - pObj->GetRect().Width();
				ASSERT(d>0);
				int k1 = 0;
				if (k != 1) k1 = d / (k-1);
				int k2 = d - k1 * k;         
				d = 0;
				while  ((pObj = GetRightTrackObj()) != NULL) 
				{ 
					m_List.AddTail(pObj);
					RemoveObj(pObj);         
					rect1.left = rect1.right - pObj->GetRect().Width();
					rect1.top = pObj->GetRect().top; 
					rect1.bottom = pObj->GetRect().bottom;
					if (rect1.left < rect.left)
					{
					    CRect rect2 = rect1;
					    rect2.right += rect.left + pObj->GetRect().Width();
						rect2.left = rect.left;      
						rect1.left = rect.left;
						pObj->SetRect(rect2);
					}
					else pObj->SetRect(rect1);
					rect1.right -= k1;
					if (k2 > 0)
					{
						rect1.right--;
						k2 --;
					}            
					if (rect1.right < rect1.left)
					{
						k2 += rect1.left - rect1.right;
						rect1.right = rect1.left;
					}
				}	
				m_ObjList.AddHead(&m_List);
				OrderObj();
				return;
			}
		}		
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
		m_ObjList.AddHead(&m_List);
		OrderObj();
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
				ASSERT(d>0);
				int k1 = 0;
				if (k != 1) k1 = d / (k-1);
				int k2 = d - k1 * k;         
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
				m_ObjList.AddHead(&m_List);
				OrderObj();
				return;
			}
		}		
		int d =  rect.Width() - w;
		ASSERT(d>0);
		int k1;
		if (k==1) k1= 0;
		else k1 =  d / (k-1);
		int k2 = d - k1 * k;         
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
				rect3.right = rect.left;
				rect1.right = rect.left;
				pObj->SetRect(rect3);
			}
			else pObj->SetRect(rect1);
			rect1.top += k1;
			if (k2 > 0)
			{
				rect1.left++;
				k2 --;
			}            
			if ((pObj = GetRightTrackObj()) != NULL) 
			{ 
				m_List.AddTail(pObj);
				RemoveObj(pObj);         
				rect2.top = pObj->GetRect().top; 
				rect2.bottom = pObj->GetRect().bottom;
				rect2.left = rect2.right - pObj->GetRect().Width();
				if (rect2.left < rect.left)
				{
				    CRect rect3 = rect2;
				    rect3.right = rect.left + pObj->GetRect().Width();
					rect3.left = rect.left;
					rect2.left = rect.left;
					pObj->SetRect(rect3);
				}
				else pObj->SetRect(rect2);
				rect2.right -= k1;
				if (k2 > 0)
				{
					rect2.right--;
					k2 --;
				}            
			}	
		}	
		m_ObjList.AddHead(&m_List);
		OrderObj();
	}
}

int CTrackObj::AddFID(CFIDDrawObj* pObj)
{   
	CCurveObj* pCurveObj = FindAliasCurve(pObj->m_AliasName);
	if ((pCurveObj != NULL) && (pCurveObj != pObj))
		pCurveObj->m_AliasName+="$";
	m_ObjList.AddTail(pObj);
	pObj->SetParentObj(this,0);
	OrderObj();
	CRect rect;
	rect = pObj->GetRect();
	rect.bottom = m_ClientRect.top + rect.Height();
	rect.top = m_ClientRect.top; 
	pObj->SetRect(rect);
	pObj->SetDepth(m_PlotSdep,m_PlotEdep);           
	return TRUE;
}                         

int CTrackObj::InsertObj(CDrawObj* pObj,CRect rect)
{
	return FALSE;
}

void CTrackObj::OrderObj()
{
	CObList m_List;
	CDrawObj* pObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*) m_ObjList.GetAt(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (pObj->GetParentType() == 0))
		{
			m_List.AddTail(pObj);
			m_ObjList.RemoveAt(pos);
			pos = m_ObjList.GetHeadPosition();
			if (pos == NULL) goto rtn;
			else continue;
		}
		m_ObjList.GetNext(pos);
	}
	pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*) m_ObjList.GetAt(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) && (pObj->GetParentType() == 0) 
			&& (((CFIDDrawObj*)pObj)->GetDrawDirect() == VERDIR))
		{
			m_List.AddTail(pObj);
			m_ObjList.RemoveAt(pos);
			pos = m_ObjList.GetHeadPosition();
			if (pos == NULL) goto rtn;
			else continue;
		}
		m_ObjList.GetNext(pos);
	}
	pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*) m_ObjList.GetAt(pos);
		if ((pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) && (pObj->GetParentType() == 0) 
			&& (((CFIDDrawObj*)pObj)->GetDrawDirect() == HORDIR))
		{
			m_List.AddTail(pObj);
			m_ObjList.RemoveAt(pos);
			pos = m_ObjList.GetHeadPosition();
			if (pos == NULL) goto rtn;
			else continue;
		}
		m_ObjList.GetNext(pos);
	}
rtn:
	m_ObjList.AddTail(&m_List);
}

int CTrackObj::RemoveObj(CDrawObj* pObj, int flag)
{
	CDrawObj::RemoveObj(pObj);
	if (pObj->IsKindOf(RUNTIME_CLASS(CCurveObj))) 
	{
		if (flag == 1)
			RemoveFillCurve((CCurveObj*)pObj);
	}
	return 1;
}

int CTrackObj::AddObj(CDrawObj* pObj,WORD nParentType)
{   
	ASSERT(pObj != NULL);
	if (nParentType != 0) 
	{
		CDrawObj::AddObj(pObj,nParentType);
		return 1;
	}		
	if ((pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) && (((CFIDDrawObj*)pObj)->GetDrawDirect() == HORDIR))
	{
		CRect rect = ((CFIDDrawObj*)pObj)->GetHeadRect();
		if (((CFIDDrawObj*)pObj)->GetMinWidth() < m_HoriFIDRect.Width())
		{
			CPoint point;
			point.x = (rect.left + rect.right) / 2;
			point.y = (rect.top + rect.bottom) / 2;
			CRect rect1 = m_HoriFIDRect;
			if (rect1.Height() == 0) rect1.InflateRect(0,3);
			if (rect1.PtInRect(point)) 
			{
				point.y = 0;
				if (rect.Width() > m_HoriFIDRect.Width())
				{
					rect.left = m_HoriFIDRect.left; rect.right = m_HoriFIDRect.right;
				}                                           
				else if (rect.left < m_HoriFIDRect.left)
				{
					rect.right = m_HoriFIDRect.left + rect.Width();
					rect.left = m_HoriFIDRect.left;
				}                                  
				else if (rect.right > m_HoriFIDRect.right)
				{
					rect.left = m_HoriFIDRect.right - rect.Width();
					rect.right = m_HoriFIDRect.right;
				}     
				if (rect.top < m_HoriFIDRect.top)
				{
					rect.bottom = m_HoriFIDRect.top + rect.Height();
					rect.top = m_HoriFIDRect.top;
				}
				if (rect.bottom > m_HoriFIDRect.bottom)
				{
					rect.top = m_HoriFIDRect.bottom - rect.Height();
					rect.bottom = m_HoriFIDRect.bottom;
				}
								
				((CFIDDrawObj*)pObj)->SetHeadRect(rect);
				AddFID((CFIDDrawObj*)pObj);                           
				if(m_bAutoArrangle) 
				{
					if (m_HoriFIDHoriArrangleMode != IDM_NONE) 
						DoHoriArrangleFID(m_HoriFIDRect,m_HoriFIDHoriArrangleMode,HORDIR);
					else if (m_HoriFIDHoriAlignMode != IDM_NONE) 
						DoHoriAlignFID(m_HoriFIDRect,m_HoriFIDHoriAlignMode,HORDIR);
				}	
				DoVertArrangle();
			}
		}		
  		return 1;
	}
	else if (pObj->IsKindOf(RUNTIME_CLASS(CTrackObj)))
	{                                             
		CRect rect = ((CTrackObj*)pObj)->GetTitleRect();
		int y = ((CTrackObj*)pObj)->GetRect().top;
		CRect rect1 = m_TrackRect;
		if ((rect.left >= rect1.left) && (rect.right <= rect1.right) &&
		   (((rect.bottom >= m_HeadRect.top) && (rect.bottom <= m_HeadRect.bottom))
		   ||((y >= m_HeadRect.top) && (y <= m_HeadRect.bottom))))
		{   
			m_ObjList.AddTail(pObj);
			pObj->SetParentObj(this,0);
			OrderObj();
			bEnableChildSizeMessage = FALSE;
			((CTrackObj*)pObj)->SetHeadHeightAuto(FALSE);
			((CTrackObj*)pObj)->SetHeadHeightAuto(m_bChildTrackHeadHeightAuto);
			CPoint pt(0,0);          
			pt.y = ((CTrackObj*)pObj)->GetHeadRect().bottom - m_TrackRect.bottom;
			if (pt.y != 0) pObj->SetRect(pObj->GetRect()-pt);
			if ((m_bAutoArrangle) && (m_TrackHoriArrangleMode != IDM_NONE))
				DoHoriArrangleTrack(m_TrackRect,m_TrackHoriArrangleMode);
			AlignChildTrackHeadHeight();
			DoVertArrangle();     
			bEnableChildSizeMessage = TRUE;
			((CTrackObj*)pObj)->SetDepth(m_PlotSdep,m_PlotEdep);
		}
		return 1;
	}
	else if ((pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) && (((CFIDDrawObj*)pObj)->GetDrawDirect() == VERDIR))
	{
		CRect rect = ((CFIDDrawObj*)pObj)->GetHeadRect();
		if (((CFIDDrawObj*)pObj)->GetMinWidth() < m_HeadRect.Width())
		{
			CPoint point;
			point.x = (rect.left + rect.right) / 2;
			point.y = (rect.top + rect.bottom) / 2;
			CRect rect1 = m_HeadRect;
			if (rect1.Height() == 0) rect1.InflateRect(0,5);
			if (rect1.PtInRect(point)) 
			{
				point.y = 0;
				if (rect.left < m_HeadRect.left)
				{
					rect.right = m_HeadRect.left + rect.Width();
					rect.left = m_HeadRect.left;
				}                                  
				if (rect.right > m_HeadRect.right)
				{
					rect.left = m_HeadRect.right - rect.Width();
					rect.right = m_HeadRect.right;
				}     
				if (rect.top < m_HeadRect.top)
				{
					rect.bottom = m_HeadRect.top + rect.Height();
					rect.top = m_HeadRect.top;
				}
				if (rect.bottom > m_HeadRect.bottom)
				{
					rect.top = m_HeadRect.bottom - rect.Height();
					rect.bottom = m_HeadRect.bottom;
				}
				((CFIDDrawObj*)pObj)->SetHeadRect(rect);
				AddFID((CFIDDrawObj*)pObj);                      
				if (!m_bAutoArrangle) m_VertFIDRect = m_HeadRect;         
				if (m_bAutoArrangle) DoHoriArrangle();
				DoVertArrangle();
			}
		}		
		return 1;
	}
	return 0;
}


CDrawObj* CTrackObj::GetLocateObject(CDrawObj* pObj)
{
	ASSERT(pObj != NULL);
	if (pObj == this) return NULL;
	CDrawObj* pObj1;
	if ((pObj1 = CDrawObj::GetLocateObject(pObj)) != NULL) return pObj1;
	if ((pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) && (((CFIDDrawObj*)pObj)->GetDrawDirect() == HORDIR))
	{
		CRect rect = ((CFIDDrawObj*)pObj)->GetHeadRect();
		if (((CFIDDrawObj*)pObj)->GetMinWidth() < m_HoriFIDRect.Width())
		{
			CPoint point;
			point.x = (rect.left + rect.right) / 2;
			point.y = (rect.top + rect.bottom) / 2;
			CRect rect1 = m_HoriFIDRect;
			if (rect1.Height() == 0) rect1.InflateRect(0,5);
			if (rect1.PtInRect(point)) return this;
		}		
		return NULL;
	}
	if (pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) 
	{
		CRect rect = ((CTrackObj*)pObj)->GetTitleRect();
		int y = pObj->GetRect().top;
		CRect rect1 = m_TrackRect;
		if ((rect.left >= rect1.left) && (rect.right <= rect1.right) &&
		   (((rect.bottom >= m_HeadRect.top) && (rect.bottom <= m_HeadRect.bottom))
		   ||((y >= m_HeadRect.top) && (y <= m_HeadRect.bottom)))) return this;
		return NULL;
	}          
	if ((pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))) && (((CFIDDrawObj*)pObj)->GetDrawDirect() == VERDIR))
	{
		CRect rect = ((CFIDDrawObj*)pObj)->GetHeadRect();
		if (((CFIDDrawObj*)pObj)->GetMinWidth() < m_HeadRect.Width())
		{
			CPoint point;
			point.x = (rect.left + rect.right) / 2;
			point.y = (rect.top + rect.bottom) / 2;
			CRect rect1 = m_HeadRect;
			if (rect1.Height() == 0) rect1.InflateRect(0,5);
			if (rect1.PtInRect(point)) return this;
		}		
		return NULL;
	}          
	return NULL;
}

void CTrackObj::DrawTracker(CView* pView, CDC* pDC, TrackerState state)
{
    if ((m_bSelected) && (m_SelectMode == none))
    {
		CPen m_Pen(0,m_TrackFramePen.lopnWidth.x,RGB(0xff,0,0));
		CPen* pOldPen = pDC->SelectObject(&m_Pen);
		CRect rect = m_Rect;
		CRect rect1 = m_TitleRect;
		pDoc->LPtoPP(&rect);
		pDoc->LPtoPP(&rect1);
		pDC->MoveTo(rect.left,rect1.bottom);
		pDC->LineTo(rect.right,rect1.bottom);
		pDC->MoveTo(rect.left,rect.top);
		pDC->LineTo(rect.left,rect.bottom);
		pDC->LineTo(rect.right,rect.bottom);
		pDC->LineTo(rect.right,rect.top);
		pDC->LineTo(rect.left,rect.top);
		pDC->SelectObject(pOldPen);
		m_Pen.DeleteObject();
    }	
}

int CTrackObj::HitTest(CView* pView,const CPoint& point)
{
	CRect rect,rect1,rect2;
    rect1 = m_Rect;
    rect1.left = m_Rect.right - 3;
    rect1.right = m_Rect.right + 3;
   	if (rect1.PtInRect(point)) return 1;
    rect1.left = m_Rect.left - 3;
    rect1.right = m_Rect.left + 3;
   	if (rect1.PtInRect(point)) return 2;
   	if (bDrawTitle)
   	{
	   	CRect rect = m_TitleRect;
	   	rect.top = m_TitleRect.top - 3;
	   	rect.bottom = m_TitleRect.top + 3;
    	if (rect.PtInRect(point)) return 3;
		rect.top = m_TitleRect.bottom - 3;
		rect.bottom = m_TitleRect.bottom + 3;
    	if (rect.PtInRect(point)) return 6;
	   	if (m_TitleRect.PtInRect(point))  return 0;
	}
	else
	{
	   	CRect rect = m_Rect;
    	rect.top = m_Rect.top - 10;
    	rect.bottom = m_Rect.top-5;
    	if (rect.PtInRect(point)) return 0;
		rect.top = m_Rect.top - 3;
		rect.bottom = m_Rect.top + 3;
    	if (rect.PtInRect(point)) return 6;
	}  	
	if (bDrawDepth)
	{
		if (m_DepthTrackPosition == IDM_LEFT)
		{
			CRect rect = m_DepthRect;
		    rect.left = m_DepthRect.right - 3;
		    rect.right = m_DepthRect.right + 3;
	    	if (rect.PtInRect(point)) return 4;
		}
		if (m_DepthTrackPosition == IDM_RIGHT)
		{
			CRect rect = m_DepthRect;
		    rect.left = m_DepthRect.left - 3;
		    rect.right = m_DepthRect.left + 3;
	    	if (rect.PtInRect(point)) return 5;
		}
	}
	return -1;
}

HCURSOR CTrackObj::GetHandleCursor(int nHandle)
{
	ASSERT_VALID(this);

	switch (nHandle)
	{
    case 0:
		if ((bDrawTitle) || (m_SelectMode == move)) 
			return AfxGetApp()->LoadCursor(IDC_CURSOR_SELECT);
		else 
			return AfxGetApp()->LoadCursor(IDC_CURSOR_SIZEWE);
	case 3:
		return AfxGetApp()->LoadCursor(IDC_CURSOR_SIZENS);
	case 6:
		return AfxGetApp()->LoadCursor(IDC_CURSOR_SIZENS);
	default:
		return AfxGetApp()->LoadCursor(IDC_CURSOR_SIZEWE);
	}
}


void CTrackObj::Draw(CView* pView, CDC* pDC)
{
    if ((CDrawObj::c_ToolShape == tracktool) && (m_bSelected)) return;
	if (bDrawTitle) DrawTitle(pView,pDC);
    DrawHead(pView,pDC);
    DrawFrame(pView,pDC);
    DrawClient(pView,pDC);
	CRect m_GridRect = m_HoriFIDRect;
	m_GridRect.top = m_ClientRect.top;
	m_GridRect.bottom = m_ClientRect.bottom;
	m_GridRect -= pDoc->m_WinOrg;
	m_pGrid->SetRect(m_GridRect);
	if(!m_pGrid->m_bHoriOpaque)
		m_pGrid->DrawHoriLine(pDC);
	if(!m_pGrid->m_bVertOpaque)
		m_pGrid->DrawVertLine(pDC);
 	if (bDrawDepth) DrawDepth(pView,pDC);
	CDrawObj::DrawFill(pView,pDC);
	if (m_pGrid->m_bHoriOpaque)
		m_pGrid->DrawHoriLine(pDC);
	
	CDrawObj::Draw(pView,pDC);
    DrawFrame(pView,pDC);
	if(pDoc->m_DrawFIDHeadMode == 1)
	{
		CPoint pt(0,0);
		pt.y = m_ClientRect.Height() + m_HeadRect.Height();;
		m_HeadRect += pt;
		DrawHead(pView,pDC);
		m_HeadRect -= pt;
		if (bDrawTitle)
		{
			pt.y += m_TitleRect.Height() + m_HeadRect.Height();
			m_TitleRect += pt;
			DrawTitle(pView,pDC);
			m_TitleRect -= pt;
		}
	}
	if(bDrawDepth&&(m_LayerFileName!=""))
		DrawLayer(pDC);	
	if (m_pGrid->m_bVertOpaque)
		m_pGrid->DrawVertLine(pDC);
}

void CTrackObj::DrawFrame(CView* pView, CDC* pDC)
{
	CRect rect;
	CPen m_Pen,*pOldPen;
	if(m_TrackFramePen.lopnWidth.x == 0)
	{
		m_Pen.CreatePen(m_TrackFramePen.lopnStyle,1,m_TrackFramePen.lopnColor);
		pOldPen = pDC->SelectObject(&m_Pen);
		rect = m_Rect; rect.bottom = m_HeadRect.bottom;
		rect.InflateRect(2,2);
		pDoc->LPtoPP(&rect);
		if (pDC->RectVisible(rect))
		{
			rect.InflateRect(-2,-2);
			pDC->MoveTo(rect.left,rect.top);
			pDC->LineTo(rect.left,rect.bottom);
			pDC->LineTo(rect.right,rect.bottom);
			pDC->LineTo(rect.right,rect.top);
			pDC->LineTo(rect.left,rect.top);
			pDC->MoveTo(rect.left,m_TitleRect.bottom);
			pDC->LineTo(rect.right,m_TitleRect.bottom);
		}
		pDC->SelectObject(pOldPen);
		m_Pen.DeleteObject();
		return;
	}
	m_Pen.CreatePenIndirect(&m_TrackFramePen);
	pOldPen = pDC->SelectObject(&m_Pen);
	if (bDrawDepth)
	{
		rect = m_DepthRect;		
		if (pDoc->m_DrawFIDHeadMode == 1)
		rect.bottom = m_Rect.bottom + m_HeadRect.bottom - m_TitleRect.top
						-m_TitleRect.Height ();
		rect.InflateRect(2,2);
		pDoc->LPtoPP(&rect);
		if (pDC->RectVisible(rect))
		{
			rect.InflateRect(-2,-2);
			CRect m_ClipRect;
			pDC->GetClipBox(&m_ClipRect);
			m_ClipRect &= rect;
			if (m_DepthTrackPosition == IDM_LEFT) 
			{
				if (m_DepthRect.right != m_Rect.right)
				{
					pDC->MoveTo(rect.right,m_ClipRect.top);
					pDC->LineTo(rect.right,m_ClipRect.bottom);
				}
			}
			if (m_DepthTrackPosition == IDM_RIGHT) 
			{
				if (m_DepthRect.left != m_Rect.left)
				{
					pDC->MoveTo(rect.left,m_ClipRect.top);
					pDC->LineTo(rect.left,m_ClipRect.bottom);
				}
			}
		}
	}       
	if (bDrawTitle)
	{
		rect = m_TitleRect;
		rect.top = rect.bottom = m_TitleRect.bottom;
		rect.InflateRect(2,2);
		pDoc->LPtoPP(&rect);
		if (pDC->RectVisible(rect))
		{
			rect.InflateRect(-2,-2);
			pDC->MoveTo(rect.left,rect.bottom);
			pDC->LineTo(rect.right,rect.bottom);
		}
		if (pDoc->m_DrawFIDHeadMode == 1)
		{
			rect = m_TitleRect;
			rect.top = rect.bottom = m_Rect.bottom + m_ClientRect.top - m_TitleRect.bottom;
			rect.InflateRect(2,2);
			pDoc->LPtoPP(&rect);
			if (pDC->RectVisible(rect))
			{
				rect.InflateRect(-2,-2);
				pDC->MoveTo(rect.left,rect.top);
				pDC->LineTo(rect.right,rect.top);
			}
		}
	}
	
	if (m_HoriFIDRect.Height()> 0)
	{
		rect = m_HoriFIDRect;		
		rect.InflateRect(2,2);
		pDoc->LPtoPP(&rect);
		if (pDC->RectVisible(rect))
		{
			rect.InflateRect(-2,-2);
			if ((m_HoriFIDRect != m_VertFIDRect) || (GetFIDCount(VERDIR) == 0))
			{
				pDC->MoveTo(rect.left,rect.bottom);
				pDC->LineTo(rect.right,rect.bottom);
			}
			if (m_HoriFIDRect.left != m_HeadRect.left)
			{
				pDC->MoveTo(rect.left,rect.top);
				pDC->LineTo(rect.left,rect.bottom);
			}
			if (m_HoriFIDRect.right != m_HeadRect.right)
			{
				pDC->MoveTo(rect.right,rect.top);
				pDC->LineTo(rect.right,rect.bottom);
			}
		}
		if (pDoc->m_DrawFIDHeadMode == 1)
		{
			rect = m_HoriFIDRect;		
			CPoint pt(0,0);
			pt.y = m_ClientRect.Height() + (m_ClientRect.top - m_HoriFIDRect.bottom)*2 
				   + m_HoriFIDRect.Height();
			rect += pt;
			rect.InflateRect(2,2);
			pDoc->LPtoPP(&rect);
			if (pDC->RectVisible(rect))
			{
				rect.InflateRect(-2,-2);
				if ((m_HoriFIDRect != m_VertFIDRect) || (GetFIDCount(VERDIR) == 0))
				{
					pDC->MoveTo(rect.left,rect.top);
					pDC->LineTo(rect.right,rect.top);
				}
				if (m_HoriFIDRect.left != m_HeadRect.left)
				{
					pDC->MoveTo(rect.left,rect.top);
					pDC->LineTo(rect.left,rect.bottom);
				}
				if (m_HoriFIDRect.right != m_HeadRect.right)
				{
					pDC->MoveTo(rect.right,rect.top);
					pDC->LineTo(rect.right,rect.bottom);
				}
			}
		}
	}		 
	if (m_TrackFramePen.lopnWidth.x > 0) 
	{
		rect = m_Rect;		
		if (pDoc->m_DrawFIDHeadMode == 1) 
			rect.bottom = m_ClientRect.bottom + m_ClientRect.top - m_Rect.top;
		rect.InflateRect(2,2);
		pDoc->LPtoPP(&rect);
		if (pDC->RectVisible(rect))
		{
			rect.InflateRect(-2,-2);
			CRect m_ClipRect;
			pDC->GetClipBox(&m_ClipRect);
			m_ClipRect &= rect;
			pDC->MoveTo(rect.left,m_ClipRect.top);
			pDC->LineTo(rect.left,m_ClipRect.bottom);
			pDC->MoveTo(rect.right,m_ClipRect.bottom);
			pDC->LineTo(rect.right,m_ClipRect.top);
 			pDC->MoveTo(rect.left,rect.top);
			pDC->LineTo(rect.right,rect.top);
			pDC->MoveTo(rect.left,rect.bottom);
			pDC->LineTo(rect.right,rect.bottom);
		}

	}
	pDC->SelectObject(pOldPen);
	m_Pen.DeleteObject();
}

void CTrackObj::DrawClient(CView* pView, CDC* pDC)
{
	CRect rect = m_ClientRect;
	if (m_ClientRect.IsRectEmpty()) return;
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;
	CRect m_ClipRect;
	pDC->GetClipBox(&m_ClipRect);
	m_ClipRect &= rect;
 	if (m_TrackClientBrush.lbStyle != BS_NULL) 
 	{
		if (m_TrackClientBrush.lbStyle == BS_OWNER)
		{
			if (m_TrackClientBrush.lbHatch > 0) 
				LithFillBox(m_ClipRect,(int)m_TrackClientBrush.lbHatch,m_TrackClientBrush.lbColor,m_ClientRect.TopLeft(),pDC);
		}
		else
		{
			CBrush br;
		 	if (m_TrackClientBrush.lbStyle == BS_HATCHED) 
			 	br.CreateHatchBrush((int)m_TrackClientBrush.lbHatch,m_TrackClientBrush.lbColor);
		   	else br.CreateBrushIndirect(&m_TrackClientBrush);
			pDC->FillRect(m_ClipRect,&br);
		}
	}	
}

void CTrackObj::DrawHead(CView* pView, CDC* pDC)
{   
	if (m_HeadRect.IsRectEmpty()) return;
	CRect rect = m_HeadRect;
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;
	CRect m_ClipRect;
	pDC->GetClipBox(&m_ClipRect);
	m_ClipRect &= rect;
 	if (m_TrackHeadBrush.lbStyle != BS_NULL) 
 	{
		if (m_TrackHeadBrush.lbStyle == BS_OWNER)
		{
			if (m_TrackHeadBrush.lbHatch < 0) return;
			LithFillBox(m_ClipRect,(int)m_TrackClientBrush.lbHatch,m_TrackClientBrush.lbColor,m_HeadRect.TopLeft(),pDC);
		}
		else
		{
			CBrush br;
		 	if (m_TrackHeadBrush.lbStyle == BS_HATCHED) 
			 	br.CreateHatchBrush((int)m_TrackHeadBrush.lbHatch,m_TrackHeadBrush.lbColor);
	   		else br.CreateBrushIndirect(&m_TrackHeadBrush);
				pDC->FillRect(m_ClipRect,&br);
		}
	}	
}

void CTrackObj::DrawDepth(CView* pView, CDC* pDC)
{
	if (m_DepthRect.IsRectEmpty()) return;
	CRect rect = m_DepthRect;
	CRect rect1 = m_ClientRect;
	pDoc->LPtoPP(&rect);
	pDoc->LPtoPP(&rect1);
	int dh=0;
	if(pDoc->m_DrawFIDHeadMode==1)
	{
		dh=rect1.top-rect.top;
		rect.bottom+=dh;
	}
	if (!pDC->RectVisible(rect)) return;
	CRect m_ClipRect;
	pDC->GetClipBox(&m_ClipRect);
	CRect sClipRect  = m_ClipRect;
	m_ClipRect &= rect;
 	if (m_TrackDepthBrush.lbStyle != BS_NULL) 
 	{
		if (m_TrackDepthBrush.lbStyle == BS_OWNER)
		{
			if (m_TrackDepthBrush.lbHatch > 0) 
				LithFillBox(m_ClipRect,(int)m_TrackClientBrush.lbHatch,m_TrackClientBrush.lbColor,m_DepthRect.TopLeft(),pDC);
		}
		else
		{
		 	CBrush br;
		 	if (m_TrackDepthBrush.lbStyle == BS_HATCHED) 
			 	br.CreateHatchBrush((int)m_TrackDepthBrush.lbHatch,m_TrackDepthBrush.lbColor);
		   	else br.CreateBrushIndirect(&m_TrackDepthBrush);
			pDC->FillRect(m_ClipRect,&br);
		}
	}	
	
	CFont m_Font,*pOldFont;
	int nSaveDC,i;
	CPoint pt,pt1;
	CSize size;

	if(m_DepthNameFont.lfEscapement == 2700)
	{
		size.cx = m_DepthNameFont.lfHeight; 
		size.cy = m_DepthNameFont.lfWidth;
		CSize size1 = size;
		pDC->LPtoDP(&size);
		m_DepthNameFont.lfHeight = size.cx;
		m_DepthNameFont.lfWidth = size.cy;
		m_Font.CreateFontIndirect(&m_DepthNameFont);
		m_DepthNameFont.lfHeight = size1.cx;
		m_DepthNameFont.lfWidth = size1.cy;
	}
	else m_Font.CreateFontIndirect(&m_DepthNameFont);
	pOldFont = pDC->SelectObject(&m_Font);
	size = pDC->GetTextExtent("文本");
   	pDC->SetTextColor(m_DepthNameColor);  
	pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	pDC->SetBkMode(TRANSPARENT); 
	if(m_DepthNameFont.lfEscapement == 2700)
	{
		pt.x = (rect.left+rect.right) / 2;
		pt.y = (rect.top + rect1.top) / 2;
		pt1=pt;
		pt1.y = rect1.bottom+dh/ 2;
		pDC->LPtoDP(&pt);
		pDC->LPtoDP(&pt1);
		nSaveDC = pDC->SaveDC();
		pDC->SetMapMode(MM_TEXT);
		pDC->DPtoLP(&pt);
		pDC->DPtoLP(&pt1);
		pDC->TextOut(pt.x-size.cy/2, pt.y,m_DepthName);
		if(pDoc->m_DrawFIDHeadMode==1)
			pDC->TextOut(pt1.x-size.cy/2, pt1.y,m_DepthName);
		pDC->RestoreDC(nSaveDC);
	}
	else
	{
		if(m_DepthName=="深度(米)层位")
		{
			pt.x = (rect.left+rect.right) / 2;
			pDC->TextOut(pt.x,(rect.top + rect1.top + size.cy)/2-3*size.cy,"深");
			pDC->TextOut(pt.x,(rect.top + rect1.top + size.cy)/2-2*size.cy,"度");
			pDC->TextOut(pt.x, (rect.top + rect1.top + size.cy)/2,"(米)");
			pDC->TextOut(pt.x,(rect.top + rect1.top + size.cy)/2+2*size.cy,"层");
			pDC->TextOut(pt.x,(rect.top + rect1.top + size.cy)/2+3*size.cy,"位");
			if(pDoc->m_DrawFIDHeadMode==1)
			{
				pDC->TextOut(pt.x,rect1.bottom+(dh+ size.cy)/2-3*size.cy,"深");
				pDC->TextOut(pt.x,rect1.bottom+(dh+ size.cy)/2-2*size.cy,"度");
				pDC->TextOut(pt.x,rect1.bottom+(dh + size.cy)/2,"(米)");
				pDC->TextOut(pt.x,rect1.bottom+(dh + size.cy)/2+2*size.cy,"层");
				pDC->TextOut(pt.x,rect1.bottom+(dh + size.cy)/2+3*size.cy,"位");
			}
		}
		else
		{
			int AliasLen=m_DepthName.GetLength ();
			pt.x = (rect.left+rect.right)/2;
			CString Str;
			for(i=0;i<AliasLen;i+=2)
			{
				Str=m_DepthName.Mid (i,2);
				pt.y=(rect.top+rect1.top+size.cy)/2-AliasLen*size.cy/4+(i+1)*size.cy/2;
				pDC->TextOut(pt.x,pt.y,Str);
			}
			if(pDoc->m_DrawFIDHeadMode==1)
			{
				for(i=0;i<AliasLen;i+=2)
				{
					Str=m_DepthName.Mid (i,2);
					pt.y=rect1.bottom+(dh+size.cy)/2-AliasLen*size.cy/4+(i+1)*size.cy/2;
					pDC->TextOut(pt.x,pt.y,Str);
				}
			}
		}
	}

	if((m_DepthName!="深度(米)层位")&&
		(m_DepthPosition!=IDM_NONE))
	{
		if(m_DepUnitNameFont.lfEscapement == 2700)
		{
			size.cx = m_DepUnitNameFont.lfHeight; 
			size.cy = m_DepUnitNameFont.lfWidth;
			CSize size1 = size;
			pDC->LPtoDP(&size);
			m_DepUnitNameFont.lfHeight = size.cx;
			m_DepUnitNameFont.lfWidth = size.cy;
			m_Font.CreateFontIndirect(&m_DepUnitNameFont);
			m_DepUnitNameFont.lfHeight = size1.cx;
			m_DepUnitNameFont.lfWidth = size1.cy;
		}
		else m_Font.CreateFontIndirect(&m_DepUnitNameFont);
		pDC->SelectObject(&m_Font);
		size = pDC->GetTextExtent(m_DepUnitName);
   		pDC->SetTextColor(m_DepUnitNameColor);
		if(m_DepUnitNameFont.lfEscapement == 2700)
		{
			pt.x=(rect.left+rect.right)/2;
			pt.y=rect1.top;
			pt1=pt;
			pt1.y=rect1.bottom;
			pDC->LPtoDP(&pt);
			pDC->LPtoDP(&pt1);
			nSaveDC = pDC->SaveDC();
			pDC->SetMapMode(MM_TEXT);
			pDC->DPtoLP(&pt);
			pDC->DPtoLP(&pt1);
			pDC->TextOut(pt.x-size.cy/2, pt.y-size.cx/2-3,m_DepUnitName);
			if(pDoc->m_DrawFIDHeadMode==1)
				pDC->TextOut(pt1.x-size.cy/2,pt1.y+size.cx/2+3,m_DepUnitName);
			pDC->RestoreDC(nSaveDC);
		}
		else
		{
			pDC->TextOut((rect.left+rect.right)/2, rect1.top-3,m_DepUnitName);
			if(pDoc->m_DrawFIDHeadMode==1)
				pDC->TextOut((rect.left+rect.right)/2, rect1.bottom+size.cy+3,m_DepUnitName);
		}
		m_Font.DeleteObject();
	}

	CRect m_DepthValueRect = rect;
	m_DepthValueRect.top = rect1.top;
	if (!m_ClipRect.IntersectRect(m_DepthValueRect,sClipRect)) return;

	int m_Depth,m_Sdep,m_Edep,y;
	char ss[10];                                           
	
	CRect rect2 = m_ClipRect;
	if (pDC->m_hDC != NULL) ::LPtoDP(pDC->m_hDC,(LPPOINT)(LPRECT)rect2,2);
	CRgn rgn; rgn.CreateRectRgn(rect2.left,rect2.top,rect2.right,rect2.bottom);
	pDC->SelectClipRgn(&rgn);

	if (m_DepthValueFont.lfEscapement == 2700)
	{
		size.cx = m_DepthValueFont.lfHeight;
		size.cy = m_DepthValueFont.lfWidth;
		CSize size1 = size;
		pDC->LPtoDP(&size);
		m_DepthValueFont.lfHeight = size.cx;
		m_DepthValueFont.lfWidth = size.cy;
		m_Font.CreateFontIndirect(&m_DepthValueFont);
		m_DepthValueFont.lfHeight = size1.cx;
		m_DepthValueFont.lfWidth = size1.cy;
	}
	else m_Font.CreateFontIndirect(&m_DepthValueFont);
	pDC->SelectObject(&m_Font);
	size = pDC->GetTextExtent("文本");
    pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);  
    pDC->SetTextColor(m_DepthValueColor);  
	
	rect2 = m_ClipRect;
	pDoc->PPtoLP(&rect2);
   	m_Sdep = (int) (YCoorToDepth(rect2.top));
   	m_Edep = (int) (YCoorToDepth(rect2.bottom));
    m_Sdep = ((int)(m_Sdep * 1.0 / m_DepthLevel+0.5))*m_DepthLevel;
    m_Edep = ((int)(m_Edep * 1.0 / m_DepthLevel+0.5))*m_DepthLevel;

	m_Depth = m_Sdep;
	while(m_Depth <= m_Edep&&m_DepthPosition!=IDM_NONE)
	{
		y = DepthToYCoor((double)m_Depth);
		y -= pDoc->m_WinOrg.y;
		if((y<rect1.top+10)||(y>rect1.bottom-10))
		{
			m_Depth += m_DepthLevel;
			continue;
		}
		sprintf(ss,"%i",m_Depth);
		if (m_DepthValueFont.lfEscapement == 2700)
		{
			switch (m_DepthPosition)
			{
			case IDM_CENTER: 
				{
					pt.x = (rect.left + rect.right) / 2; pt.y = y;
					pDC->LPtoDP(&pt);
					nSaveDC = pDC->SaveDC();
					pDC->SetMapMode(MM_TEXT);
					pDC->DPtoLP(&pt,1);
				    pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);  
					pDC->TextOut(pt.x-size.cy/2,pt.y,ss);
					pDC->RestoreDC(nSaveDC);
				}break;
 			case IDM_LEFT: 
	 			{
					pt.x = rect.left + 2; pt.y = y;
					pDC->LPtoDP(&pt);
					nSaveDC = pDC->SaveDC();
					pDC->SetMapMode(MM_TEXT);
					pDC->DPtoLP(&pt);
				    pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);  
 					pDC->TextOut(pt.x,pt.y,ss);
					pDC->RestoreDC(nSaveDC);
 				}break;
			case IDM_RIGHT: 
				{
					pt.x = rect.right - 2; pt.y = y;
					pDC->LPtoDP(&pt);
					nSaveDC = pDC->SaveDC();
					pDC->SetMapMode(MM_TEXT);
					pDC->DPtoLP(&pt);
				    pDC->SetTextAlign(TA_CENTER | TA_TOP);  
					pDC->TextOut(pt.x,pt.y,ss);
					pDC->RestoreDC(nSaveDC);
				}break;
			}	
		}
		else
		{
			switch (m_DepthPosition)
			{
			case IDM_CENTER: 
				{
				    pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);  
					pDC->TextOut((rect.left+rect.right)/2,y+size.cy/2,ss);
				}break;
 			case IDM_LEFT: 
	 			{
				    pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);  
 					pDC->TextOut(rect.left+2,y+size.cy/2,ss);
 				}break;
			case IDM_RIGHT: 
				{
				    pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);  
					pDC->TextOut(rect.right-2,y+size.cy/2,ss);
				}break;	
			}	
		}		
		m_Depth += m_DepthLevel;
	}
	pDC->SelectObject(pOldFont);
	m_ClipRect = sClipRect;
	if (pDC->m_hDC != NULL) ::LPtoDP(pDC->m_hDC,(LPPOINT)(LPRECT)m_ClipRect,2);
	CRgn rgn1;
	rgn1.CreateRectRgn(m_ClipRect.left,m_ClipRect.top,m_ClipRect.right,m_ClipRect.bottom);
	pDC->SelectClipRgn(&rgn1);
}

void CTrackObj::DrawTitle(CView* pView, CDC* pDC)
{   
	if (m_TitleRect.IsRectEmpty()) return;
	CRect rect = m_TitleRect;
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;
	CRect m_ClipRect;
	pDC->GetClipBox(&m_ClipRect);
	m_ClipRect &= rect;
 	if (m_TrackTitleBrush.lbStyle != BS_NULL) 
 	{
		if (m_TrackTitleBrush.lbStyle == BS_OWNER)
		{
			if (m_TrackTitleBrush.lbHatch > 0) 
				LithFillBox(m_ClipRect,(int)m_TrackClientBrush.lbHatch,m_TrackClientBrush.lbColor,m_TitleRect.TopLeft(),pDC);
		}
		else
		{
			CBrush br;
		 	if (m_TrackTitleBrush.lbStyle == BS_HATCHED) 
			 	br.CreateHatchBrush((int)m_TrackTitleBrush.lbHatch,m_TrackTitleBrush.lbColor);
		   	else br.CreateBrushIndirect(&m_TrackTitleBrush);
			pDC->FillRect(m_ClipRect,&br);
		}
	}	
    CFont m_Font, *pOldFont;
    m_Font.CreateFontIndirect(&m_TrackNameFont);
    pOldFont = pDC->SelectObject(&m_Font);
    pDC->SetTextColor(m_TrackNameColor);
    pDC->SetTextAlign(TA_CENTER | TA_TOP);
    CSize size = pDC->GetTextExtent("文本");   
	pDC->SetBkMode(TRANSPARENT);
	CString str;
	if(m_TrackName.Find ("+")>-1)
		str=m_TrackName.Left (m_TrackName.Find ("+"));
	else
		str=m_TrackName;
    pDC->TextOut((rect.left + rect.right)/2, (rect.bottom + rect.top - size.cy)/2, str);
    pDC->SelectObject(pOldFont);
}

void CTrackObj::OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point)
{
    if(m_bSelected)
    {
		nDragHandle = HitTest(pView,point); 
		if (nDragHandle == 0) 
		{
			CRect rect = pDoc->GetRect();
			m_SelectMode = move;            
			m_xMin = rect.left;
			m_xMax = rect.right;
			if (m_pParentObj != NULL)
			{
				m_yMin = rect.top;
				m_yMax = rect.bottom;
			}
		}	
		else if (nDragHandle > 0) 
		{
			m_SelectMode = size; 
			switch (nDragHandle)
			{
			case 2:
				{   
				m_xMax = m_Rect.right - GetMinWidth();
				if (m_xMax == m_Rect.right) m_xMax = m_Rect.right - 10;
				if ((m_pParentObj == NULL) || (m_ParentType != 0))
					m_xMin = pDoc->GetRect().left;
				else 
				{
					ASSERT(m_pParentObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
					m_xMin = ((CTrackObj*)m_pParentObj)->GetTrackRect().left;
				}		
				}
				break;	 
			case 1:
				{   
					m_xMin = m_Rect.left + GetMinWidth();
					if (m_xMin == m_Rect.left) m_xMin = m_Rect.left + 10;
					if ((m_pParentObj == NULL) || (m_ParentType != 0))
						m_xMax = pDoc->GetRect().right;
					else 
					{
						ASSERT(m_pParentObj->IsKindOf(RUNTIME_CLASS(CTrackObj)));
						m_xMax = ((CTrackObj*)m_pParentObj)->GetTrackRect().right;
					}		
				}
				break;	 
			case 4:
				{   
					m_xMax = m_Rect.right - GetMinHeadWidth();
					m_xMin = m_DepthRect.left + 10;
				}
				break;	 
			case 5:
				{   
					m_xMin = m_Rect.left + GetMinHeadWidth();
					m_xMax = m_DepthRect.right - 10;
				}
				break;	              
			case 3:
				{
					if (m_pParentObj != NULL) m_yMin = pDoc->GetRect().top;
					m_yMax = m_TitleRect.bottom - 10;
				}	                                 
				break;
			case 6:
				{
					if (!m_bAutoArrangle)
					{
						CRect rect = GetUnionRect();
						m_yMax = rect.top;
					}
					else m_yMax = m_HeadRect.bottom - GetMinHeadHeight();
					if (m_pParentObj != NULL) m_yMin = pDoc->GetRect().top;
				}	      
				break;
			default:
				ASSERT(FALSE);
				break;	
			}
		}	
		else return;
		SetCursor(GetHandleCursor(nDragHandle));
		CDrawObj::OnLButtonDown(pView,nFlags,point);
	}	
	if (m_SelectMode == move)
	{
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if (pObj != NULL) pObj->OnLButtonDown(pView, nFlags, point);
		}                  
	}	
	if ((m_SelectMode == size) || (m_SelectMode == move)) 
		CreateFocusObj(pView);
}

void CTrackObj::OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point)
{
	if (m_SelectMode == move)
	{
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if (pObj != NULL) pObj->OnLButtonUp(pView, nFlags, point);
		}                  
		if (!m_bSelected)
		{
			DeleteFocusObj(pView);
			return;
		}	
	}	
	if (m_bSelected)
	{
		if (c_last == c_down) 
		{
			DeleteFocusObj(pView);
		}
		else if (m_SelectMode == move)
		{
	    	m_pFocusObj->m_Rect.NormalizeRect();
			SetRect(m_pFocusObj->m_Rect);                          
			DeleteFocusObj(pView);
			if (m_ParentType != 2)
			{
				CDrawObj* pObj1 = m_pParentObj;    
				int nParentType = m_ParentType;
				CDrawObj* pObj = pDoc->GetLocateObject(this);
				if (m_pParentObj == NULL) pDoc->RemoveObj(this);
				else m_pParentObj->RemoveObj(this);
				m_pParentObj = NULL;
				if (pObj != NULL) pObj->AddObj(this);
				else pDoc->AddObj(this);  
				if (pObj1 != pObj)
				{
					if (pObj1 == NULL) pDoc->OnChildRemove(this,nParentType);
					else pObj1->OnChildRemove(this,nParentType);
				}	   
			}  
		}	
		else if (m_SelectMode == size)
		{
			CRect m_SaveRect = m_Rect;
			switch (nDragHandle)
			{
			default:
				ASSERT(FALSE);
			case 1:
			case 2:
				SetRect(m_pFocusObj->m_Rect);
				break;
			case 3:
				SetTitleRectHeight(m_pFocusObj->m_TitleRect.Height());	
				break;
			case 4:
				{
					m_DepthRect.right = m_pFocusObj->m_DepthRect.right;
					m_LayerFlush=TRUE;
					SetClientRect(m_pFocusObj->m_ClientRect);					
				}	
				break;
			case 5:
				{
					m_DepthRect.left = m_pFocusObj->m_DepthRect.left;
					m_LayerFlush=TRUE;
					SetClientRect(m_pFocusObj->m_ClientRect);					
				}	                 
				break;
			case 6:
				SetHeadRectHeight(m_pFocusObj->m_HeadRect.Height());	
				break;
			}	
			DeleteFocusObj(pView);
			if ((m_Rect.left != m_SaveRect.left) || (m_Rect.right != m_SaveRect.right))
			{
				if (m_pParentObj == NULL) pDoc->OnChildWidthChange(this);
				else m_pParentObj->OnChildWidthChange(this);
			}
		}
		CDrawObj::OnLButtonUp(pView,nFlags,point);
	}	
}

void CTrackObj::OnMouseMove(CView* pView, UINT nFlags, const CPoint& point)
{
	CPoint delta = (CPoint)(point - c_last);
	m_pFocusObj->DrawFocus(pView);
	if (m_SelectMode == move)
	{
		CPoint point1 = point;
		if (m_bSelected)
		{
			CRect rect = m_pFocusObj->m_Rect;
			rect.bottom = m_pFocusObj->m_HeadRect.bottom;
			rect.NormalizeRect();
			if ((rect.right + delta.x > m_xMax) && (delta.x > 0)) 
			{
				if (rect.right < m_xMax)
					point1.x = m_xMax - rect.right + c_last.x;
				else point1.x = c_last.x;	
			}
			if ((rect.left + delta.x < m_xMin) && (delta.x < 0)) 
			{
				if (rect.left > m_xMin)
					point1.x = m_xMin - rect.left + c_last.x;
				else point1.x = c_last.x;	
			}	
			if (m_pParentObj != NULL)
			{	
				if ((rect.bottom + delta.y > m_yMax) && (delta.y > 0)) 
				{
					if (rect.bottom < m_yMax)
						point1.y = m_yMax - rect.bottom + c_last.y;
					else point1.y = c_last.y;
				}	
				if ((rect.top + delta.y < m_yMin) && (delta.y < 0)) 
				{
					if (rect.top > m_yMin)
						point1.y = m_yMin - rect.top + c_last.y;
					else point1.y = c_last.y;	
				}	
						
			}		
			if (point1.x != point.x) delta.x = point1.x - c_last.x;
			if (point1.y != point.y) delta.y = point1.y - c_last.y;
		}	
		POSITION pos = m_ObjList.GetHeadPosition();
		while (pos != NULL)
		{
			CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
			if (pObj != NULL) pObj->OnMouseMove(pView, nFlags, point);
		}                  
		m_pFocusObj->MoveHandle(pView,nDragHandle, delta);
	}	
	if ((m_SelectMode == size)&&bTrackResize)
		m_pFocusObj->MoveHandle(pView,nDragHandle, delta);
	m_pFocusObj->DrawFocus(pView);
	if (m_bSelected) 
	{
		DoScroll((CScrollView*)pView);
		CDrawObj::OnMouseMove(pView,nFlags,point);
	}
}

void CTrackObj::MoveHandle(CView* pView,int nHandle, const CPoint& point)
{
	ASSERT_VALID(this);
	if ((point.x == 0) && (point.y == 0 )) return;
	BOOL bChange = FALSE;
	switch (nHandle)
	{
	default:
		ASSERT(FALSE);

	case 0: 
		{
			m_Rect += point;
			m_TitleRect += point;
			m_HeadRect += point;
			m_ClientRect += point;
			m_DepthRect += point;
			bChange = TRUE; 
		}	
		break;
	case 1:
		if (((m_Rect.right + point.x <= m_xMax) && (point.x > 0)) ||
			((m_Rect.right + point.x >= m_xMin) && (point.x < 0)))
		{
			m_Rect.right += point.x;
			m_ClientRect.right += point.x;  
			m_HeadRect.right += point.x;
			m_TitleRect.right += point.x;
			bChange = TRUE; 
			if (m_DepthTrackPosition == IDM_RIGHT)
			{
				m_DepthRect.left = m_ClientRect.right;
				m_DepthRect.right = m_Rect.right;
			}
		}	
		break;
	case 2:
		if (((m_Rect.left + point.x <= m_xMax) && (point.x > 0)) ||
			((m_Rect.left + point.x >= m_xMin) && (point.x < 0)))
		{
			m_Rect.left += point.x;
			m_ClientRect.left += point.x;
			m_HeadRect.left += point.x;
			m_TitleRect.left += point.x;
			bChange = TRUE; 
			if (m_DepthTrackPosition == IDM_LEFT)
			{
				m_DepthRect.right = m_ClientRect.left;
				m_DepthRect.left = m_Rect.left;
			}
		}	
		break;
	case 3:
		if (((point.y > 0) && (m_TitleRect.top + point.y < m_yMax)) ||
		    ((point.y < 0) && ((m_TitleRect.top + point.y > m_yMin) || (m_pParentObj == NULL))))
		{
			bChange = TRUE; 
			m_TitleRect.top += point.y;
			m_Rect.top = m_TitleRect.top;
		}
		break;
	case 4:
		if (((m_DepthRect.right + point.x <= m_xMax) && (point.x > 0)) ||
			((m_DepthRect.right + point.x >= m_xMin) && (point.x < 0)))
		{
			bChange = TRUE; 
			m_DepthRect.right += point.x;
			m_ClientRect.left = m_DepthRect.right;
		}	
		break;
	case 5:
		if (((m_DepthRect.left + point.x <= m_xMax) && (point.x > 0)) ||
			((m_DepthRect.left + point.x >= m_xMin) && (point.x < 0)))
		{
			bChange = TRUE; 
			m_DepthRect.left += point.x;
			m_ClientRect.right = m_DepthRect.left;
		}	
		break;
	case 6:
		if (((point.y > 0) && (m_HeadRect.top + point.y < m_yMax)) ||
		    ((point.y < 0) && ((m_HeadRect.top + point.y > m_yMin) || (m_pParentObj == NULL))))
		{
			bChange = TRUE; 
			m_HeadRect.top += point.y;
			m_TitleRect.bottom += point.y;
			m_TitleRect.top += point.y;
			m_Rect.top += point.y;
		}
		break;
	}
	if (bChange) pDoc->SetModifiedFlag();
}

void CTrackObj::CreateFocusObj(CView* pView)
{
	m_pFocusObj = new CTrackObj(m_Rect,m_pParentObj,pDoc,m_PlotSdep,m_PlotEdep);
	m_pFocusObj->m_Rect = m_Rect;
	m_pFocusObj->m_TitleRect = m_TitleRect;
	m_pFocusObj->m_HeadRect = m_HeadRect;
	m_pFocusObj->m_ClientRect = m_ClientRect;
	m_pFocusObj->m_DepthRect = m_DepthRect;
	m_pFocusObj->bDrawDepth = bDrawDepth;
	m_pFocusObj->bDrawTitle = bDrawTitle;
	m_pFocusObj->m_DepthTrackPosition = m_DepthTrackPosition;
	m_pFocusObj->DrawFocus(pView);
}

void CTrackObj::DeleteFocusObj(CView* pView)
{
	ASSERT((m_pFocusObj != NULL) && (pView != NULL));
	if (m_pFocusObj == NULL) return;
	if (pView != NULL) m_pFocusObj->DrawFocus(pView);
	delete m_pFocusObj;
	m_pFocusObj = NULL;
}   

void CTrackObj::DrawFocusObj(CView* pView)
{
	ASSERT((m_pFocusObj != NULL) && (pView != NULL));
	if ((m_pFocusObj == NULL) || (pView == NULL)) return;
	m_pFocusObj->DrawFocus(pView);
	if (m_SelectMode != move) return;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawObj* pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if (pObj != NULL) pObj->DrawFocusObj(pView);
	}                  
}

void CTrackObj::DrawFocus(CView* pView)
{
	ASSERT(pView != NULL);
	if (pView == NULL) return;
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc,NULL);
	if (nDragHandle >= 0)
	{
		COLORREF color = m_TrackFramePen.lopnColor;
		int style = m_TrackFramePen.lopnStyle;
		CPoint width = m_TrackFramePen.lopnWidth;
		m_TrackFramePen.lopnColor = RGB(0,0,0);
		m_TrackFramePen.lopnStyle = PS_DOT;
		m_TrackFramePen.lopnWidth = CPoint(1,1);
		dc.SetROP2(R2_XORPEN);
		CRect rect = m_HoriFIDRect;
		m_HoriFIDRect = m_HeadRect;
		DrawFrame(pView,&dc);		
		m_TrackFramePen.lopnColor = color;
		m_TrackFramePen.lopnStyle = style;
		m_TrackFramePen.lopnWidth = width;
		m_HoriFIDRect = rect;
		dc.SetROP2(R2_COPYPEN);
	}
}

BOOL CTrackObj::IsFillCurve(CCurveObj* pCurveObj)
{
	CDrawObj* pObj=NULL;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CCurveObj))) 
			&& (pObj->GetParentType() == 0)) 
		{
			if (((CCurveObj*)pObj)->IsFillCurve(pCurveObj)) return TRUE;
		}
	}
	return FALSE;
}

BOOL CTrackObj::IsFillCurve(CString name)
{
	CDrawObj* pObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CCurveObj))) 
			&& (pObj->GetParentType() == 0)) 
		{
			if (((CCurveObj*) pObj)->IsFillCurve(name)) return TRUE;
		}
	}
	return FALSE;
}

int CTrackObj::RemoveFillCurve(CCurveObj* pCurveObj)
{
	int i=1;
	CDrawObj* pObj;
	ASSERT(pCurveObj != NULL);
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CCurveObj))) 
			&& (pObj->GetParentType() == 0)) 
		{
			i &= (((CCurveObj*) pObj)->RemoveFillCurve(pCurveObj));
		}
	}
	return i;
}

int CTrackObj::RemoveFillCurve(CString name)
{
	CDrawObj* pObj;
	int i = 1;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CCurveObj))) 
			&& (pObj->GetParentType() == 0)) 
		{
			i &= (((CCurveObj*) pObj)->RemoveFillCurve(name));
		}
	}
	return i;
}

BOOL CTrackObj::MatchWellName(CString strName, CString NewstrPathName)
{
	CString Name;
	CFIDDrawObj* pObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFIDDrawObj*)m_ObjList.GetNext(pos);
		if((pObj != NULL)&& (pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))))
		{		
			if(pObj->m_DrawShape==DSCobj||pObj->m_DrawShape==resultobj
				||pObj->m_DrawShape==Yieldobj||pObj->m_DrawShape==lthobj||pObj->m_DrawShape==lthnoteobj
				||pObj->m_DrawShape==stratumobj||pObj->m_DrawShape==markvectorobj||pObj->m_DrawShape==Markfreqobj)
			{
				Name=pObj->m_strPathName.Mid(pObj->m_strPathName.ReverseFind('\\'));
				if(Name.GetAt(0)!='\\') 
					Name="\\"+Name;
				pObj->m_strPathName=NewstrPathName.Left(NewstrPathName.ReverseFind('\\'))+Name;
			}
			else
				pObj->m_strPathName=NewstrPathName.Left(NewstrPathName.ReverseFind('.'))
									+pObj->m_strPathName.Mid(pObj->m_strPathName.ReverseFind('.'));
			pObj->bFlush = TRUE;
			pObj->bErrorFID = FALSE;
		}
	}
	return TRUE;
}

CFIDDrawObj* CTrackObj::FindCurve(CString name)
{
	CFIDDrawObj* pObj;
	char szFIDName[40];
	CString CurveName;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CFIDDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj != NULL)&& (pObj->IsKindOf(RUNTIME_CLASS(CFIDDrawObj))))
		{
			CGeoFIDIO::GetFIDName(pObj->m_strPathName,szFIDName);
			CurveName=szFIDName;			
			if(CurveName == name) 
				return pObj;
		}
	}
	return NULL;
}

CCurveObj* CTrackObj::FindAliasCurve(CString name)
{
	CDrawObj* pObj;
	POSITION pos = m_ObjList.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CDrawObj*)m_ObjList.GetNext(pos);
		if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CCurveObj))) 
			&& (pObj->GetParentType() == 0)) 
		{
			if (((CCurveObj*) pObj)->m_AliasName == name) return (CCurveObj*) pObj;
		}
	}
	return NULL;
}

CCurveObj* CTrackObj::FindLastCurve(CCurveObj* pObj)
{
	ASSERT(pObj != NULL);
	POSITION pos = m_ObjList.Find(pObj,NULL);
	if (pos != NULL) 
	{
		m_ObjList.GetPrev(pos);
		if (pos != NULL) 
		{
			CObject* pObj1 = m_ObjList.GetNext(pos);
			if (pObj1->IsKindOf(RUNTIME_CLASS(CCurveObj))) return (CCurveObj*)pObj1;
		}
	}
	return NULL;
}

void CTrackObj::SetLayerRect(const CRect& rect)
{
	CLayerPoint* pObj;
	CRect rect1;
	POSITION pos = m_LayerPoint.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CLayerPoint*) m_LayerPoint.GetNext(pos);
		rect1 = pObj->GetRect();
		rect1.left = rect.left;
		rect1.right = rect.right;
		pObj->SetRect(rect1);
	}          
}

void CTrackObj::CreatePoint(CDC* pDC)
{
	CRect rect;
	if(m_DepthTrackPosition==2)
	{
		rect.left = m_Rect.left;
		rect.right = m_Rect.right;
	}
	else
	{
		rect.left = m_DepthRect.left;
		rect.right = m_DepthRect.right;
	}
	POSITION pos = m_LayerPoint.GetHeadPosition();
	while (pos != NULL) 
		delete m_LayerPoint.GetNext(pos);
	m_LayerPoint.RemoveAll();
	int xyz=m_LayerFileName.Find (".List");
	if((xyz>0)&&DetectFileExist(m_LayerFileName))
	{
		BOOL End=FALSE;
		float dep=(float)m_PlotSdep,dep0=(float)m_PlotSdep;
		int top0=DepthToYCoor(m_PlotSdep);
		TCHAR *token, *end;
		TCHAR buffer[1024];
		CStdioFile File;
		CFileException ex;
		if (!File.Open(m_LayerFileName, CFile::modeRead | CFile::typeText)) 
			return;
		try 
		{
			CString str,Resultstr,Flagstr;
			int ResultNum=-1;
			int FlagNum=-1;
			int i=0;
			// Read Header off file
			File.ReadString(buffer, 1024);

			// Get first token
			for (token=buffer, end=buffer; 
				 *end && (*end != _T(',')) && (*end != _T('\n')); end++);
			if ((*end == _T('\0')) && (token == end)) token = NULL;
			*end = _T('\0');
			while (token) 
			{
				str=token;
				while(str.Left(1)==" ")
					str.TrimLeft (" ");
				while(str.Right(1)==" ")
					str.TrimRight (" ");
				if(str=="标注")
					ResultNum=i;
				else if(str=="选项")
					FlagNum=i;
				// Get next token
				for (token=++end; *end && (*end != _T(',')) && (*end != _T('\n'));
					 end++);
				if ((*end == _T('\0')) && (token == end)) token = NULL;
				*end = _T('\0');
				i++;
			}		
			while (File.ReadString(buffer, 1024)) 
			{
				// Get first token
				for (token=buffer, end=buffer; 
					 *end && (*end != _T(',')) && (*end != _T('\n')); end++);
				if ((*end == _T('\0')) && (token == end)) token = NULL;
				*end = _T('\0');
				
				i=0;Resultstr="";
				while (token) 
				{
					str=token;
					while(str.Left(1)==" ")
						str.TrimLeft (" ");
					while(str.Right(1)==" ")
						str.TrimRight (" ");
					if(i==ResultNum)
						Resultstr=str;
					else if(i==FlagNum)
						Flagstr=str;
					else if(i!=0)
						dep=(float)atof(str);
					// Get next token
					for (token=++end; *end && (*end != _T(',')) && (*end != _T('\n'));
						 end++);
					if ((*end == _T('\0')) && (token == end)) token = NULL;
					*end = _T('\0');
					i++;
				}
				if(dep>m_PlotSdep)
				{
					if((Flagstr.Find ("定位")==-1)&&
						(Resultstr.Find("LINE")==-1)&&(Resultstr!=""))
					{
						rect.top=top0;
						if(dep>m_PlotEdep&&dep-dep0>10)
						{
							rect.bottom= DepthToYCoor(m_PlotEdep);
							if(rect.bottom-rect.top>80)
								SubDeviseLayer(rect,Resultstr,Flagstr,pDC,dep0,(float)m_PlotEdep);
							break;
						}
						rect.bottom= DepthToYCoor(dep);
						if(rect.bottom-rect.top>80)
							SubDeviseLayer(rect,Resultstr,Flagstr,pDC,dep0,dep);
						top0=rect.bottom;
						dep0=dep;
					}
					else
					{
						if((Resultstr.Find("LINE")>-1)||(Resultstr==""))
						{
							rect.top=top0;
							rect.bottom= DepthToYCoor(dep);
							CPoint pt=rect.BottomRight ();
							CLayerPoint* pObj;
							if(Flagstr.Find ("断层标识")>-1)
								pObj = new CLayerPoint(rect,pt,m_LayerNameColor,m_LayerNameFont,"FAULT");
							else if(Flagstr.Find ("虚线")>-1)
								pObj = new CLayerPoint(rect,pt,m_LayerNameColor,m_LayerNameFont,"DASHED");
							else
								pObj = new CLayerPoint(rect,pt,m_LayerNameColor,m_LayerNameFont,"LINE");
							m_LayerPoint.AddTail(pObj);
							top0=rect.bottom;
						}						
						else
						{
							CRect rect0=rect;
							rect0.bottom =DepthToYCoor(dep);
							rect0.top=rect0.bottom;
							SubDeviseLayer(rect0,Resultstr,Flagstr,pDC,dep0,dep);
							dep0=dep;
						}
					}
				}
			}
			File.Close();
		}
		catch (CFileException* e)
		{
			AfxMessageBox(_T("Unable to load grid data"));
			e->Delete();
		}		
	}
	m_LayerFlush = FALSE;
}

void CTrackObj::SubDeviseLayer(CRect rect,CString m_NoStr,CString Flag,CDC* pDC,float dep0,float dep)
{
	CRect rect0=rect;
	CPoint pt;
	CFont m_Font,m_Font2700;
	LOGFONT m_ResultFont=m_LayerNameFont;
	LOGFONT LogFont2700=m_ResultFont;
	CSize size0;
	size0.cx = m_ResultFont.lfWidth; size0.cy =m_ResultFont.lfHeight ;
	CSize size1 = size0;
	LogFont2700.lfHeight = size0.cy;
	LogFont2700.lfWidth = size0.cx;
	LogFont2700.lfEscapement = 2700;
	m_Font2700.CreateFontIndirect(&LogFont2700);	
	if (m_ResultFont.lfEscapement == 2700)
	{
		m_ResultFont=LogFont2700;
		m_Font.CreateFontIndirect(&m_ResultFont);
		m_ResultFont.lfHeight = size1.cy;
		m_ResultFont.lfWidth = size1.cx;
	}	
	else m_Font.CreateFontIndirect(&m_ResultFont);
	
	CFont* pOldFont;
	CSize size,size00;
	if((m_NoStr.Find("(未完)")>-1)
		||(m_NoStr.Find("（未完)")>-1)
		||(m_NoStr.Find("(未完）")>-1))
	{
		pOldFont = pDC->SelectObject(&m_Font2700);
		size = pDC->GetTextExtent("国"); 		
		pt.x = (rect.left+rect.right-size.cx) / 2;
		pt.y=rect.bottom-3*size.cy/2;
		CRect rect1(rect);
		rect1.top =pt.y-size.cy/2;
		rect1.bottom =pt.y+size.cy/2;
		CLayerPoint* pObj1 = new CLayerPoint(rect1,pt,m_LayerNameColor,LogFont2700,"（");
		m_LayerPoint.AddTail(pObj1);
		pt.y=rect.bottom+3*size.cy/2;
		rect1.top =pt.y-size.cy/2;
		rect1.bottom =pt.y+size.cy/2;
		CLayerPoint* pObj2 = new CLayerPoint(rect1,pt,m_LayerNameColor,LogFont2700,"）");
		m_LayerPoint.AddTail(pObj2);
		pDC->SelectObject(&m_Font);
		if (m_ResultFont.lfEscapement == 2700)
		{
			pt.y=rect.bottom -size.cy/2;
			rect1.top =pt.y-size.cy/2;
			rect1.bottom =pt.y+size.cy/2;
		}
		else
		{
			pt.x = (rect.left+rect.right) / 2;
			pt.y=rect.bottom;
			rect1.top =pt.y-size.cy;
			rect1.bottom =pt.y;
		}
		CLayerPoint* pObj3 = new CLayerPoint(rect1,pt,m_LayerNameColor,m_ResultFont,"未");
		m_LayerPoint.AddTail(pObj3);
		if (m_ResultFont.lfEscapement == 2700)
		{
			pt.y=rect.bottom +size.cy/2;
			rect1.top =pt.y-size.cy/2;
			rect1.bottom =pt.y+size.cy/2;
		}
		else
		{
			pt.x = (rect.left+rect.right) / 2;
			pt.y=rect.bottom +size.cy;
			rect1.top =pt.y-size.cy;
			rect1.bottom =pt.y;
		}
		CLayerPoint* pObj4 = new CLayerPoint(rect1,pt,m_LayerNameColor,m_ResultFont,"完");
		m_LayerPoint.AddTail(pObj4);
		pDC->SelectObject(pOldFont);
		return;
	}
	pOldFont = pDC->SelectObject(&m_Font);
	size = pDC->GetTextExtent("国");   		
	if (m_ResultFont.lfEscapement == 2700)
		pt.x = (rect.left+rect.right-size.cx)/2;
	else
		pt.x = (rect.left+rect.right) / 2;
	int AliasLen=m_NoStr.GetLength ();
	int StrNum=(AliasLen-(AliasLen%2))/2;
	int Num=rect.Height ()/(StrNum+1);		
	int y0,DepTop;
	int SpaceNum=0;
 	double dep00 = YCoorToDepth(rect.top );
    DepTop= ((long)(dep00 * 1.0 / m_DepthLevel+0.5))*m_DepthLevel;
	DepTop=DepthToYCoor((double)DepTop);
	long OutDepLeve=(long)(DepthToYCoor((double)m_DepthLevel)-DepthToYCoor(0.));
	if(DepTop<rect.top)
		DepTop+=OutDepLeve;

	y0=DepTop;	
	while(y0<rect.bottom )
	{
		y0+=OutDepLeve;
		if(y0<=rect.bottom )
			SpaceNum++;
	}
	long yy=rect.top;
	CString Str;int i;
	if(Flag.Find ("垂直定位")>-1)
	{
		pt.y=rect.bottom+size.cy/2-(StrNum-1)*(size.cy+6)/2;
		for(i=0;i<StrNum;i++)
		{			
			Str=m_NoStr.Mid (i*2,2);
			if (m_ResultFont.lfEscapement == 2700)
			{
				rect.top =pt.y-size.cy/2;
				rect.bottom =pt.y+size.cy/2;
			}
			else
			{
				rect.top =pt.y-size.cy;
				rect.bottom =pt.y;
			}
			CLayerPoint* pObj5 = new CLayerPoint(rect,pt,m_LayerNameColor,m_ResultFont,Str);
			m_LayerPoint.AddTail(pObj5);
			pt.y+=size.cy+6;
		}
		if(i>0)	pt.y-=size.cy+3;
	}
	else if(Flag.Find ("水平定位")>-1)
	{
		if(m_ResultFont.lfEscapement == 2700)
		{
			pt.y=rect.bottom;
			pt.x=rect.left;
		}
		else
		{
			pt.y=rect.bottom+size.cy/2;
			pt.x=rect.left+size.cx/2;
		}
		int ll=m_NoStr.GetLength()-2;
		for(i=0;i<StrNum;i++)
		{			
			if(m_ResultFont.lfEscapement == 2700)
				Str=m_NoStr.Mid (ll-i*2,2);
			else
				Str=m_NoStr.Mid (i*2,2);
			rect.top =pt.y-size.cy;
			rect.bottom =pt.y;
			CLayerPoint* pObj6 = new CLayerPoint(rect,pt,m_LayerNameColor,m_ResultFont,Str);
			m_LayerPoint.AddTail(pObj6);
			pt.x+=size.cx;
		}
		if(i>0)	pt.x-=size.cx;
	}
	else if(Flag.Find ("顶底标识")>-1)
	{
		if(!m_bLayerHi)
		{
			size00 = pDC->GetTextExtent(m_NoStr);
			if(m_ResultFont.lfEscapement == 2700)
				pt.x=(rect.left+rect.right-size00.cy)/2;
			else
				pt.x=(rect.left+rect.right)/2;
			
			if(m_ResultFont.lfEscapement == 2700)
				pt.y=rect.top+size00.cx/2+4;
			else
				pt.y=rect.top+size00.cy+4;
			CLayerPoint* pObj44 = new CLayerPoint(rect,pt,m_LayerNameColor,m_ResultFont,m_NoStr);
			m_LayerPoint.AddTail(pObj44);

			if(m_ResultFont.lfEscapement == 2700)
				pt.y=rect.bottom-size00.cx/2-4;
			else
				pt.y=rect.bottom-size00.cy/2+4;
			CLayerPoint* pObj55 = new CLayerPoint(rect,pt,m_LayerNameColor,m_ResultFont,m_NoStr);
			m_LayerPoint.AddTail(pObj55);
			i=AliasLen;
		}
		else
		{
			Str=m_NoStr.Left(StrNum*2);
			i=StrNum;
			size00 = pDC->GetTextExtent(Str);
			//顶部层位标注
			if(m_ResultFont.lfEscapement == 2700)
			{
				pt.x=(rect.left+rect.right-size00.cy)/2;
				pt.y=rect.top+size00.cx/2+4;
			}
			else
			{
				pt.x=(rect.left+rect.right)/2;
				pt.y=rect.top+size00.cy+6;
			}
			CLayerPoint* pObj66 = new CLayerPoint(rect,pt,m_LayerNameColor,m_ResultFont,Str);
			m_LayerPoint.AddTail(pObj66);
			
			if(m_ResultFont.lfEscapement == 2700)
				pt.y+=size.cy/2;
			else
				pt.x+=size.cx/2;
			//顶部层位标注 上标
			if(i*2<AliasLen)
			{
				CFont Script0;
				pt.x+=size.cx/2;
				if (m_ResultFont.lfEscapement == 2700)
				{	
					pt.y+=size.cy/2+3;
					Script0.CreateFont(14, 0, 2700, 0, FW_BOLD, FALSE, FALSE, 0,
	 							DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
								CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
								VARIABLE_PITCH | FF_SWISS, "Times New Roman");
				}
				else
				{	
					pt.y-=size.cy/2+3;
					Script0.CreateFont(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
	 							DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
								CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
								VARIABLE_PITCH | FF_SWISS, "Times New Roman");
				}
				CFont* pOldFont1=pDC->SelectObject(&Script0);
				size00 = pDC->GetTextExtent("国"); 
				Str=m_NoStr.Right (1);
				pt.x+=3;
				LOGFONT LogScript0;
				Script0.GetLogFont (&LogScript0);
				if (LogScript0.lfEscapement == 2700)
				{
					rect.top =pt.y-size00.cy/2;
					rect.bottom =pt.y+size00.cy/2;
					rect.left=pt.x;
					rect.right =pt.x+size00.cx;
				}
				else
				{
					rect.top =pt.y-size00.cy;
					rect.bottom =pt.y;
					rect.left=pt.x-size00.cx/2;
					rect.right =pt.x+size00.cx/2;
				}
				pDC->SelectObject(pOldFont1);
				CLayerPoint* pObj22 = new CLayerPoint(rect,pt,m_LayerNameColor,LogScript0,Str);
				m_LayerPoint.AddTail(pObj22);
			}
			//底部层位标注	
			Str=m_NoStr.Left(StrNum*2);
			i=StrNum;
			size00 = pDC->GetTextExtent(Str);
			rect=rect0;
			if(m_ResultFont.lfEscapement == 2700)
			{
				pt.x=(rect.left+rect.right-size00.cy)/2;
				pt.y=rect.bottom-size00.cx/2-6;
			}
			else
			{
				pt.x=(rect.left+rect.right)/2;
				pt.y=rect.bottom-size00.cy/2+4;
			}
			CLayerPoint* pObj77 = new CLayerPoint(rect,pt,m_LayerNameColor,m_ResultFont,Str);
			m_LayerPoint.AddTail(pObj77);
			if(m_ResultFont.lfEscapement == 2700)
				pt.y+=size.cy/2;
			else
				pt.x+=size.cx/2;
		}
	}
	else
	{		
		if(SpaceNum<StrNum)
		{
			if(SpaceNum<1)
			{
				int yx0=DepTop-rect.top;
				int yx1=rect.bottom-DepTop;
				if(yx0>yx1)
					pt.y=rect.top +(yx0-StrNum*size.cy)/2+size.cy;
				else
					pt.y=rect.bottom -(yx1-StrNum*size.cy)/2-size.cy;
				for(i=0;i<StrNum;i++)
				{			
					Str=m_NoStr.Mid (i*2,2);
					if (m_ResultFont.lfEscapement == 2700)
					{
						rect.top =pt.y-size.cy/2;
						rect.bottom =pt.y+size.cy/2;
					}
					else
					{
						rect.top =pt.y-size.cy;
						rect.bottom =pt.y;
					}
					CLayerPoint* pObj7 = new CLayerPoint(rect,pt,m_LayerNameColor,m_ResultFont,Str);
					m_LayerPoint.AddTail(pObj7);
					pt.y+=size.cy;
				}
				if(i>0) pt.y-=size.cy+3;
			}
			else 
			{
				pt.y=DepTop+(OutDepLeve-StrNum*size.cy)/2+size.cy;
				for(i=0;i<StrNum;i++)
				{			
					Str=m_NoStr.Mid (i*2,2);
					if (m_ResultFont.lfEscapement == 2700)
					{
						rect.top =pt.y-size.cy/2;
						rect.bottom =pt.y+size.cy/2;
					}
					else
					{
						rect.top =pt.y-size.cy;
						rect.bottom =pt.y;
					}
					CLayerPoint* pObj7 = new CLayerPoint(rect,pt,m_LayerNameColor,m_ResultFont,Str);
					m_LayerPoint.AddTail(pObj7);
					pt.y+=size.cy;
				}
				if(i>0) pt.y-=size.cy+3;
			}
		}
		else
		{						
			for(i=0;i<StrNum;i++)
			{
				yy+=Num;								
				if(yy>DepTop)
				{
					while(DepTop<yy)
						DepTop+=OutDepLeve;
					DepTop-=OutDepLeve;
				}
				pt.y=DepTop+OutDepLeve/2+size.cy/2;			
				Str=m_NoStr.Mid (i*2,2);
				if (m_ResultFont.lfEscapement == 2700)
				{
					rect.top =pt.y-size.cy/2;
					rect.bottom =pt.y+size.cy/2;
				}
				else
				{
					rect.top =pt.y-size.cy;
					rect.bottom =pt.y;
				}
				CLayerPoint* pObj10 = new CLayerPoint(rect,pt,m_LayerNameColor,m_ResultFont,Str);
				m_LayerPoint.AddTail(pObj10);
			}
		}
	}
	if(i*2<AliasLen)
	{
		CFont Script;
		pt.x+=size.cx/2;
		if (m_ResultFont.lfEscapement == 2700)
		{	
			pt.y+=size.cy/2+3;
			Script.CreateFont(14, 0, 2700, 0, FW_BOLD, FALSE, FALSE, 0,
	 					 DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
						 CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
						 VARIABLE_PITCH | FF_SWISS, "Times New Roman");
		}
		else
		{	
			pt.y-=size.cy/2+3;
			Script.CreateFont(14, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
	 					 DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
						 CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
						 VARIABLE_PITCH | FF_SWISS, "Times New Roman");
		}
		pDC->SelectObject(&Script);
		size = pDC->GetTextExtent("国"); 
		Str=m_NoStr.Right (1);
		pt.x+=3;
		LOGFONT LogScript;
		Script.GetLogFont (&LogScript);
		if (LogScript.lfEscapement == 2700)
		{
			rect.top =pt.y-size.cy/2;
			rect.bottom =pt.y+size.cy/2;
			rect.left=pt.x;
			rect.right =pt.x+size.cx;
		}
		else
		{
			rect.top =pt.y-size.cy;
			rect.bottom =pt.y;
			rect.left=pt.x-size.cx/2;
			rect.right =pt.x+size.cx/2;
		}
		
		CLayerPoint* pObj11 = new CLayerPoint(rect,pt,m_LayerNameColor,LogScript,Str);
		m_LayerPoint.AddTail(pObj11);
	}
	pDC->SelectObject(pOldFont);
	if(Flag.Find ("定位")==-1)
	{
		pt=rect0.BottomRight ();
		CLayerPoint* pObj12;
		if(Flag.Find ("断层标识")>-1)
			pObj12= new CLayerPoint(rect0,pt,m_LayerNameColor,m_LayerNameFont,"FAULT");
		else if(Flag.Find ("虚线")>-1)
			pObj12= new CLayerPoint(rect0,pt,m_LayerNameColor,m_LayerNameFont,"DASHED");
		else
			pObj12= new CLayerPoint(rect0,pt,m_LayerNameColor,m_LayerNameFont,"LINE");
		m_LayerPoint.AddTail(pObj12);
	}
}