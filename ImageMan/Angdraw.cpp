/*************************************************************************
*   DangDdirDraw.cpp - implementation of the DangDDir draw object class  *           
*************************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "ImageMan.h"
#include "ImageManDoc.h"
#include "TrackDraw.h"
#include "FIDIO.h"
#include "AngDraw.h"
#include "Grid.h"
#include "GridDialog.h"
#include "DipPage.h"
#include "LineListPage.h"
#include "FillListPage.h"
#include "MinFontDialog.h"
#include "DipBasePage.h"

//#define PI 3.14159265

IMPLEMENT_SERIAL(CDangDdirObj, CFIDDrawObj, 0)
        
CDangDdirObj::CDangDdirObj()
{
	point = NULL;
	dip=NULL;
	ddir = NULL;
	m_PointNum = 0;
	m_DrawShape = dangddirobj;
	m_MinGrade = (float)11.1;
	m_bFlush = TRUE;
	m_HeadStyle=0;
	m_PrintPara=FALSE;
	m_ParaFormat="%.f %.f";
	m_GradName="GRAD";
	m_DirName ="DIR";
}
    
CDangDdirObj::CDangDdirObj(CString szPathName, const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDoc)
			:CFIDDrawObj(szPathName,rect,pParentObj,pDoc)
{
	point = NULL; ddir = NULL;
	m_PointNum = 0;
	m_CircleRadius = 5; m_CircleWidth = 1; m_CircleColor = RGB(0,0x80,0);
	m_TailLength = 15;  m_TailWidth = 1; m_TailColor = RGB(0,0x80,0);
	m_HeadStyle=0;
	m_PrintPara=FALSE;
	m_ParaFormat="%.f %.f";
	m_GradName="GRAD";
	m_MinGrade = (float)11.1;                       
	m_LeftVal = 0.f; m_RightVal = 90.f;
	m_DrawShape = dangddirobj;
	m_AliasName = "地层倾角";
	m_pGrid = new CGridObj(m_Rect,m_PlotSdep,m_PlotEdep,DIPGRID);
	m_pGrid->SetDepC(pDoc->m_DepC);
	m_ScaleFont = m_NameFont; 
	m_NameFont.lfHeight = -20;
	m_ScaleFont.lfHeight = -10;
	m_ScaleFont.lfWeight = 300;
	m_Rect.right = m_HeadRect.right = m_Rect.left + 150;
	m_DepLevel=1;
	m_DirName ="DIR";
	m_bFlush = TRUE;
}

CDangDdirObj::~CDangDdirObj()
{
	if (point != NULL) delete point;
	if (dip != NULL) delete dip;
	if (ddir != NULL) delete ddir;
	if (m_pGrid != NULL) delete m_pGrid;
}    

void CDangDdirObj::Serialize( CArchive& ar )
{
	long l;
	CFIDDrawObj::Serialize( ar );
	if(!ar.IsStoring())
	{
		ar >> m_DepLevel;
		ar >> m_CircleWidth;
		ar >> m_CircleRadius;
		ar >> l; m_CircleColor = (COLORREF) l;
		ar >> m_TailWidth;
		ar >> m_TailLength;
		ar >> l; m_TailColor = (COLORREF) l;
		ar >> m_HeadStyle;
 		ar >> m_PrintPara;
		ar >> m_ParaFormat;

		ar >> m_DirName;
		ar >> m_GradName;
		ar >> m_MinGrade;

		SerializeLogFont(ar,m_ScaleFont);
		m_pGrid = new CGridObj();
		m_pGrid->Serialize(ar);
		m_pGrid->SetDepC(pDoc->m_DepC);
		m_bFlush = TRUE;
	}
}

void CDangDdirObj::SetRect(const CRect& rect)
{
	if (m_Rect == rect) return;
	CFIDDrawObj::SetRect(rect);
	m_pGrid->SetRect(rect);
}                    

void CDangDdirObj::OnChangeDepC(double depc, int flag)
{
	CFIDDrawObj::OnChangeDepC(depc,flag);
	m_pGrid->SetDepC(pDoc->m_DepC);
}
               
void CDangDdirObj::SetDepth(double sdep,double edep)
{
	CFIDDrawObj::SetDepth(sdep,edep);
	m_pGrid->SetDepth(sdep,edep);
}
void CDangDdirObj::CreatePoint()
{
	BOOL bGrade = FALSE;
	if (point != NULL) delete point;
	if (dip != NULL) delete dip;
	if (ddir != NULL) delete ddir;
	point = NULL; dip=NULL;ddir = NULL;
	m_PointNum = 0;
	if (m_bErrorFID) return;

	CFIDIO* pDangFIDIO = new CFIDIO(m_strPathName);
	CFIDIO* pDdirFIDIO = new CFIDIO(m_DirName);
	if (!(pDangFIDIO->Open() && pDdirFIDIO->Open() && IsDepthUnitOK(pDangFIDIO) && IsDepthUnitOK(pDdirFIDIO))) 
	{
		m_bErrorFID = TRUE;
		delete pDangFIDIO;
		delete pDdirFIDIO;
		return;
	}
	CFIDIO* pGradeFIDIO = new CFIDIO(m_GradName);
	if (pGradeFIDIO->Open()) 
		bGrade = TRUE;
	else 
		delete pGradeFIDIO;

// may be check rlev,sdep,edep 
	m_Rlev = pDangFIDIO->GetDepthLevel();
	ASSERT(m_Rlev != 0);
	if (m_Rlev == 0) goto rtn;

	m_SDep = pDangFIDIO->GetStartDepth();
	if(pDoc->m_PlotSdep>m_SDep)
		m_SDep=pDoc->m_PlotSdep;
	m_EDep = pDangFIDIO->GetEndDepth();
	if(pDoc->m_PlotEdep<m_EDep)
		m_EDep=pDoc->m_PlotEdep ;
	m_PointNum = (int)((m_EDep - m_SDep) / m_Rlev/m_DepLevel) + 1 ;
	if (m_PointNum < 0) m_PointNum = 0;
	if (m_PointNum > 0)
	{
	    float grade,dang,dep;
	    point = new CPoint[m_PointNum];
	    ddir = new float[m_PointNum];
		dip=new float[m_PointNum];
		for (int j=0; j<m_PointNum; j++)  
		{
			dep=(float)(m_SDep+m_DepLevel*j*m_Rlev);
			pDangFIDIO->SeekToDepth(dep);     
			pDangFIDIO->Read(2,1,&dang);
			dip[j]=dang;
			pDdirFIDIO->SeekToDepth(dep);     
			pDdirFIDIO->Read(2,1,&ddir[j]);
			if ((dang > m_RightVal) || (dang < m_LeftVal))
			{
				point[j].x = 9999;
				continue;
			}
			if (bGrade)
			{
				pGradeFIDIO->SeekToDepth(dep);     
				pGradeFIDIO->Read(2,1,&grade);
			}
			int i=0;
			while (dang > m_pGrid->m_AngX[i+1]) i++;
			point[j].x = (int)(abs(m_pGrid->m_ZerX[i]) + (abs(m_pGrid->m_ZerX[i+1]) - abs(m_pGrid->m_ZerX[i])) 
				* (dang - m_pGrid->m_AngX[i]) / (m_pGrid->m_AngX[i+1] - m_pGrid->m_AngX[i]));   
			point[j].y = DepthToYCoor(dep);
			if (bGrade && (grade < m_MinGrade)) 
				point[j].x = -point[j].x;                 
		}
	}	
rtn:
	delete pDangFIDIO; 
	delete pDdirFIDIO;
	if (bGrade) 
		delete pGradeFIDIO;
	m_bFlush = FALSE;
}

void CDangDdirObj::DrawDangDdir(CDC* pDC)
{  
	int x,y,x1,y1,x2,y2;
	CFIDDrawObj::DrawClient(pDC);   
	if (m_bErrorFID) return;
	CRect m_Rect0 = m_Rect;
	pDoc->LPtoPP(m_Rect0);
	CRect rect,m_ClipRect;
	pDC->GetClipBox(m_ClipRect);
	if (!pDC->RectVisible(m_Rect0)) return; 
	rect = m_Rect0 & m_ClipRect;
	CRect rect1 = rect;
	rect1.InflateRect(0,m_TailLength+m_CircleRadius);
	pDoc->PPtoLP(rect1);
	m_SDep = YCoorToDepth(rect1.top);
	m_EDep = YCoorToDepth(rect1.bottom);
	if(m_bFlush) CreatePoint();
	if (m_PointNum == 0) return; 
	
	CPen m_Pen1,m_Pen2;
	m_Pen1.CreatePen(0,m_CircleWidth,m_CircleColor);
	BOOL bSamePen = (m_CircleWidth == m_TailWidth) && (m_CircleColor == m_TailColor);
	if (!bSamePen)	m_Pen2.CreatePen(0,m_TailWidth,m_TailColor);
	CPen* pOldPen = pDC->SelectObject(&m_Pen1); 
	CBrush m_Brush(m_CircleColor);
	CBrush m_WhiteBrush(RGB(255,255,255));
	CBrush* pOldBrush = pDC->SelectObject(&m_Brush);    
	BOOL bNullBrush = FALSE;
	int len = m_TailLength + m_CircleRadius;
	double dtor =  PI / 180;

	CPoint pt;
	CString para;
	CFont hTextFont;
    hTextFont.CreateFont(
		-14,					        //lfHeight
		0,							//lfWidth
		0,							//lfEscapement
		0,							//lfOrientation
		FW_BOLD,					//lfWeight
		0,							//lfItalic
		0,							//lfUnderLine;
		0,							//lfStrikeOut;	
		ANSI_CHARSET,				//lfCharSet
		OUT_TT_ONLY_PRECIS,			//lfOutPrecision
		CLIP_DEFAULT_PRECIS,		//lfClipPrecision
		DEFAULT_QUALITY,			//lfQuality
		VARIABLE_PITCH | FF_SWISS,	//lfPitchAndFamily
		"Courier New"           //"MS Serif" "Courier New""Times New Roman"
	);

	CFont* pOldFont=pDC->SelectObject (&hTextFont);
	pDC->SetTextAlign(TA_LEFT|TA_TOP);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor (RGB(0,0,0));
	for (int j=0; j<m_PointNum; j++)
	{
		pt=point[j];
		pDoc->LPtoPP(&pt);
	    x = pt.x; y = pt.y;
	    if (x == 9999) continue;
		if (x < 0)
		{
			if (!bNullBrush) pDC->SelectStockObject(NULL_BRUSH);
			x = -x;
			bNullBrush = TRUE;
		}
		else if (bNullBrush)
		{
			pDC->SelectObject(&m_Brush);
			bNullBrush = FALSE;
		}
		x1 = x - m_CircleRadius; x2 = x + m_CircleRadius;
        y1 = y - m_CircleRadius; y2 = y + m_CircleRadius;
		if (!bSamePen) pDC->SelectObject(&m_Pen1);
		double dd = ddir[j] * dtor;
		if(m_HeadStyle==1)
			pDC->Rectangle (x1,y1,x2,y2); 
		else
		    pDC->Ellipse(x1,y1,x2,y2);  
		x1 = x + Round(m_CircleRadius * sin(dd));
		y1 = y - Round(m_CircleRadius * cos(dd));
		x2 = x + Round(len * sin(dd));
		y2 = y - Round(len * cos(dd));
		if (!bSamePen) pDC->SelectObject(&m_Pen2);
		pDC->MoveTo(x1,y1); pDC->LineTo(x2,y2);
		if(m_PrintPara)
		{
			para.Format (m_ParaFormat,dip[j],ddir[j]);
			pDC->TextOut (x+m_CircleRadius+3,y,para);
		}
	}		
	pDC->SelectObject(pOldFont);
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}

void CDangDdirObj::DrawClient(CDC* pDC)
{        
	if ((m_pParentObj == NULL) || (m_ParentType != 0)) CFIDDrawObj::DrawClient(pDC);
	DrawDangDdir(pDC);
}

void CDangDdirObj::DrawHead(CDC* pDC)
{
	CFIDDrawObj::DrawHead(pDC);   
	CRect rect = m_HeadRect;
	pDoc->LPtoPP(rect);
	if (pDC->RectVisible(rect))
	{
		char ss[20];
		pDC->SetBkMode(TRANSPARENT); 
		CFont m_Font,*pOldFont;
		m_Font.CreateFontIndirect(&m_ScaleFont);
	    pOldFont = pDC->SelectObject(&m_Font);
		pDC->SetTextColor(m_NameColor);
			    
		pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
		pDC->TextOut((rect.left + rect.right)/2, rect.bottom - 7, m_UnitText, strlen(m_UnitText));

	    pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
		dtos(m_LeftVal,ss);
	    pDC->TextOut(rect.left+2,rect.bottom-7,ss,strlen(ss));
		dtos(m_RightVal,ss);
	    pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
		pDC->TextOut(rect.right-2,rect.bottom-7,ss,strlen(ss));
		pDC->SelectObject(pOldFont);
		CFont m_Font1;
		m_Font1.CreateFontIndirect(&m_NameFont);
		pOldFont = pDC->SelectObject(&m_Font1);
		pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
		CSize size;
		size = pDC->GetTextExtent("文本");   
		pDC->TextOut((rect.left + rect.right)/2, rect.bottom - size.cy -10, m_AliasName,strlen(m_AliasName));
		pDC->SelectObject(pOldFont);
		CPen pen;
		pen.CreatePen (PS_SOLID,1,m_CircleColor);
		CPen* Oldpen=pDC->SelectObject (&pen);
		pDC->MoveTo (rect.left,rect.bottom - 5);
		pDC->LineTo (rect.right,rect.bottom - 5);
		pDC->SelectObject (Oldpen);
	}
}                              

BOOL CDangDdirObj::Properties(CView* pView)
{
	CString szHT;
	szHT.LoadString(IDS_DIPPROPERTIES);
	CPropertySheet m_Sheet(szHT);
	CDipBasePage  m_BasePage(this);
	CDipPage m_DipPage(this);
	m_DipPage.m_DepLevel =m_DepLevel;
	m_Sheet.AddPage(&m_BasePage);
	m_Sheet.AddPage(&m_DipPage);
	CLineListPage m_LineList;
	CFillListPage m_FillList;
 	if (m_ParentType != 0)
	{
		m_LineList.AddPen(&m_FramePen,"框线");
		szHT.LoadString(IDS_FRAMEFILL);
		m_FillList.AddFill(&m_Brush,szHT);
		m_Sheet.AddPage(&m_LineList);
		m_Sheet.AddPage(&m_FillList);
	}

	CMinFontPage m_FontPage;
	TEXTFONT m_TextFont[2];
	szHT.LoadString(IDS_ALIASFONT);
	m_TextFont[0].m_ItemName = szHT.GetBuffer(128);
	szHT.ReleaseBuffer();
	m_TextFont[0].m_Text = m_AliasName;
	m_TextFont[0].m_ForeGround = m_NameColor;
	m_TextFont[0].m_BackGround = RGB(255,255,255);
	m_TextFont[0].lf = m_NameFont;
	m_FontPage.AddFont(&m_TextFont[0]);
	szHT.LoadString(IDS_ZERFONT);
	m_TextFont[1].m_ItemName = szHT.GetBuffer(128);
	szHT.ReleaseBuffer();
	m_TextFont[1].m_Text = "012345";
	m_TextFont[1].m_ForeGround = m_NameColor;
	m_TextFont[1].m_BackGround = RGB(255,255,255);
	m_TextFont[1].lf = m_ScaleFont;
	m_FontPage.AddFont(&m_TextFont[1]);
	m_Sheet.AddPage(&m_FontPage);

	double m_PlotSdep1 = m_PlotSdep;
	double m_PlotEdep1 = m_PlotEdep;
	CString m_strSavePathName  = m_strPathName;
	if (m_Sheet.DoModal() == IDOK)
	{
		m_FontPage.OnOK();
		m_DepLevel=m_DipPage.m_DepLevel;
		m_NameColor = m_TextFont[0].m_ForeGround;
		m_NameFont = m_TextFont[0].lf;
		m_ScaleFont = m_TextFont[1].lf;
		m_NameFont.lfEscapement = 0;
		m_ScaleFont.lfEscapement = 0;
		CRect m_SaveRect = m_Rect;
		m_SaveRect.top = m_HeadRect.top;
		if (m_strSavePathName  != m_strPathName) m_bErrorFID = FALSE;
		if ((m_PlotSdep != m_PlotSdep1) || (m_PlotEdep != m_PlotEdep1))
		{
			double m_PlotSdep2 = m_PlotSdep;
			double m_PlotEdep2 = m_PlotEdep;
			m_PlotSdep = m_PlotSdep1;
			m_PlotEdep = m_PlotEdep1;
			SetDepth(m_PlotSdep2,m_PlotEdep2);
			if (m_SaveRect.bottom<m_Rect.bottom) m_SaveRect.bottom = m_Rect.bottom;
		}
		m_bFlush = TRUE;
		InvalidateRect(m_SaveRect);
		return TRUE;
	}
	return FALSE;
}



