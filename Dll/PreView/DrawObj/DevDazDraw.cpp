/*************************************************************************
*   DevDazDraw.cpp - implementation of the DevDaz draw object class      *           
*************************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "TrackDraw.h"
#include "GeoFIDio.h"
#include "DevDazDraw.h"
//#include "MinFontDialog.h"
//#include "LineListPage.h"
//#include "FillListPage.h"
//#include "DevDazPage.h"
//#include "DevbasePage.h"

IMPLEMENT_SERIAL(CDevDazObj, CFIDDrawObj, 0)
        
CDevDazObj::CDevDazObj()
{
	point = NULL;
	daz = NULL;
	m_PointNum = 0;
	m_DrawShape = devdazobj;
	m_DazName ="AZI";
}
    
CDevDazObj::CDevDazObj(CString szPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDoc)
			:CFIDDrawObj(szPathName,rect,pParentObj,pDoc)
{
	point = NULL; daz = NULL;
	m_PointNum = 0;
	m_CircleRadius = 5; m_CircleWidth = 1; m_CircleColor = RGB(0,0x80,0);
	m_TailLength = 15;  m_TailWidth = 1; m_TailColor = RGB(0,0x80,0);
	m_DepLevel = 1.f;
	m_LeftVal = 0.f; m_RightVal = 10.f;
	m_DrawShape = devdazobj;
	m_AliasName = "¾®Ð±";
	m_ScaleFont = m_NameFont; 
	m_ScaleFont.lfHeight = -10;
	m_ScaleFont.lfWeight = 300;
	m_ScaleColor = m_NameColor;
	m_DazName ="AZI";
}

CDevDazObj::~CDevDazObj()
{
	if (point != NULL) delete point;
	if (daz != NULL) delete daz;
}    

void CDevDazObj::Serialize( CArchive& ar )
{
	long l;
	CFIDDrawObj::Serialize( ar );
	if(ar.IsStoring() )
	{
		ar << m_DepLevel;
		ar << m_CircleWidth;
		ar << m_CircleRadius;
		ar << (long) m_CircleColor;
		ar << m_TailWidth;
		ar << m_TailLength;
		ar << (long) m_TailColor;
		ar << (long) m_ScaleColor;
		ar << m_DazName;
		SerializeLogFont(ar,m_ScaleFont);
	}
	else
	{
		ar >> m_DepLevel;
		ar >> m_CircleWidth;
		ar >> m_CircleRadius;
		ar >> l; m_CircleColor = (COLORREF) l;
		ar >> m_TailWidth;
		ar >> m_TailLength;
		ar >> l; m_TailColor = (COLORREF) l;
		ar >> l; m_ScaleColor = (COLORREF) l;
		ar >> m_DazName;
		SerializeLogFont(ar,m_ScaleFont);
		bFlush = TRUE;
	}
}

void CDevDazObj::CreatePoint()
{
	double sdep,edep,dep,tmp;
	int num = 0;
	BOOL bFlag = FALSE;
	BOOL bGrade = FALSE;
	if (point != NULL) delete point;
	if (daz != NULL) delete daz;
	point = NULL; daz = NULL;
	m_PointNum = 0;

	if (bErrorFID) return;
	
	CString m_Path = "";
	CGeoFIDIO::GetFullWellName(m_strPathName,m_Path.GetBuffer(512));
	m_Path.ReleaseBuffer();

	CGeoFIDIO* pDevFIDIO = new CGeoFIDIO(m_strPathName);
	CString StrDaz=m_DazName.Mid(m_DazName.ReverseFind('.')+1);
	StrDaz=m_strPathName.Left(m_strPathName.ReverseFind('.')+1)+StrDaz;
	CGeoFIDIO* pDazFIDIO = new CGeoFIDIO(StrDaz);
	if(!(pDevFIDIO->Open(pDoc->m_szSever) && pDazFIDIO->Open(pDoc->m_szSever)
		&&IsDepthUnitOK(pDevFIDIO) && IsDepthUnitOK(pDazFIDIO)))
	{
		bErrorFID = TRUE;
		delete pDevFIDIO;
		delete pDazFIDIO;
		return;
	}

// may be check rlev,sdep,edep 
	m_Rlev = pDevFIDIO->GetDepthLevel();
	ASSERT(m_Rlev != 0);
	if(m_Rlev == 0)
	{
		bErrorFID = TRUE;
		delete pDevFIDIO;
		delete pDazFIDIO;
		return;
	}
	sdep = pDevFIDIO->GetStartDepth();
	edep = pDevFIDIO->GetEndDepth();
	float rlev=pDevFIDIO->GetDepthLevel();
	float DepLevel=((int)(m_DepLevel/rlev))*rlev;
	float stdep=((int)(m_SDep/DepLevel))*DepLevel;
	float endep=((int)(m_EDep/DepLevel)+1)*DepLevel;
	num = (int)((endep - stdep) / DepLevel) + 10 ;
    point = new CPoint[num];
	daz = new float[num];
	m_PointNum = 0;
	tmp = (float)(m_Rect.Width() / (m_RightVal - m_LeftVal)); 
	dep = stdep;
	int n=0;
	while(dep <= endep)
	{
		float dev;
		if((dep >= sdep)&&(dep <= edep))
		{
			pDevFIDIO->SeekToDepth((float)dep);     
			pDevFIDIO->Read(2,1,&dev);
			if(dev>=m_LeftVal&&dev<=m_RightVal)
			{
				m_PointNum ++;
				pDazFIDIO->SeekToDepth((float)dep);     
				pDazFIDIO->Read(2,1,&daz[m_PointNum-1]);
				point[m_PointNum-1].x = m_Rect.left + Round((dev - m_LeftVal)*tmp);
				point[m_PointNum-1].y = DepthToYCoor(dep) - pDoc->m_WinOrg.y;
			}
		}
		n++;
		dep=stdep+n*DepLevel;
	}	
	delete pDevFIDIO; delete pDazFIDIO;
}

void CDevDazObj::DrawClient(CDC* pDC)
{    
	int x,y,x1,y1,x2,y2;	
	if ((m_pParentObj == NULL) || (m_ParentType != 0)) 
		CFIDDrawObj::DrawClient(pDC);

	if (bErrorFID) return;
	CRect m_Rect0 = m_Rect;
	pDoc->LPtoPP(m_Rect0);
	CRect rect,m_ClipRect;
	pDC->GetClipBox(m_ClipRect);
	if(!pDC->RectVisible(m_Rect0)) return; 
	rect = m_Rect0 & m_ClipRect;
	CRect rect1 = rect;
	rect1.InflateRect(0,m_TailLength+m_CircleRadius);
	pDoc->PPtoLP(rect1);
	m_SDep = YCoorToDepth(rect1.top);
	m_EDep = YCoorToDepth(rect1.bottom);
	CreatePoint();
	if(m_PointNum<1) return;

	BOOL bClip = FALSE;
	if((m_ClipRect.top<m_Rect0.top)||(m_ClipRect.bottom>m_Rect0.bottom))
	{
		if(pDC->m_hDC != NULL) 
			::LPtoDP(pDC->m_hDC,(LPPOINT)(LPRECT)rect,2);
		CRgn rgn1; rgn1.CreateRectRgn(rect.left,rect.top,rect.right,rect.bottom);
		pDC->SelectClipRgn(&rgn1);
		bClip = TRUE;
	}

	CPen m_Pen1,m_Pen2;
	m_Pen1.CreatePen(0,m_CircleWidth,m_CircleColor);
	BOOL bSamePen = (m_CircleWidth == m_TailWidth) && (m_CircleColor == m_TailColor);
	if (!bSamePen)	m_Pen2.CreatePen(0,m_TailWidth,m_TailColor);
	CPen* pOldPen = pDC->SelectObject(&m_Pen1); 
	CBrush m_Brush(m_CircleColor);
	CBrush m_WhiteBrush(RGB(255,255,255));
	CBrush* pOldBrush = pDC->SelectObject(&m_Brush);    
	int len = m_TailLength + m_CircleRadius;
	double dtor =  PI / 180;

	for (int j=0; j<m_PointNum; j++)
	{
	    x = point[j].x; y = point[j].y;
		x1 = x - m_CircleRadius; x2 = x + m_CircleRadius;
        y1 = y - m_CircleRadius; y2 = y + m_CircleRadius;
		if (!bSamePen) pDC->SelectObject(&m_Pen1);
        pDC->Ellipse(x1,y1,x2,y2);           
        double dd = daz[j] * dtor;
		x1 = x + Round(m_CircleRadius * sin(dd));
	    y1 = y - Round(m_CircleRadius * cos(dd));
		x2 = x + Round(len * sin(dd));
		y2 = y - Round(len * cos(dd));
		if (!bSamePen) pDC->SelectObject(&m_Pen2);
		pDC->MoveTo(x1,y1); pDC->LineTo(x2,y2);
	}	
	
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	
	delete point; 
	delete daz;
	point = NULL;
	daz = NULL;
	m_PointNum = 0;

	if(bClip)
	{
		rect = m_ClipRect;
		if(pDC->m_hDC != NULL) 
			::LPtoDP(pDC->m_hDC,(LPPOINT)(LPRECT)rect,2);
		CRgn rgn2; rgn2.CreateRectRgn(rect.left,rect.top,rect.right,rect.bottom);
		pDC->SelectClipRgn(&rgn2);
	}
}

void CDevDazObj::DrawHead(CDC* pDC)
{
	CRect rect = m_HeadRect;
	pDoc->LPtoPP(rect);
	if(pDC->RectVisible(rect))
	{
		char ss[10];      
		CFIDDrawObj::DrawHead(pDC);   
		CFont m_Font;
		m_Font.CreateFontIndirect(&m_ScaleFont);
	    CFont* pOldFont = pDC->SelectObject(&m_Font);
	    pDC->SetTextColor(m_NameColor);
		pDC->SetBkMode(TRANSPARENT); 
		
		pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
		pDC->TextOut((rect.left + rect.right)/2, rect.bottom - 7, m_UnitText, (int)strlen(m_UnitText));

	    pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
		dtos(m_LeftVal,ss);
	    pDC->TextOut(rect.left+2,rect.bottom-7,ss,(int)strlen(ss));
		dtos(m_RightVal,ss);
		pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
		pDC->TextOut(rect.right-2,rect.bottom-7,ss,(int)strlen(ss));
		pDC->SelectObject(pOldFont);
		CFont m_Font1;
		m_Font1.CreateFontIndirect(&m_NameFont);
		pOldFont = pDC->SelectObject(&m_Font1);
   		CSize size = pDC->GetTextExtent("ÎÄ±¾");
	   	pDC->SetTextColor(m_NameColor);  
		pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
		pDC->TextOut((rect.left + rect.right)/2, rect.bottom - size.cy -10, m_AliasName,(int)strlen(m_AliasName));
		pDC->SelectObject(pOldFont);
		CPen pen;
		pen.CreatePen (PS_SOLID,1,m_CircleColor);
		CPen* Oldpen=pDC->SelectObject (&pen);
		pDC->MoveTo (rect.left,rect.bottom - 5);
		pDC->LineTo (rect.right,rect.bottom - 5);
		pDC->SelectObject (Oldpen);
	}
}                              

BOOL CDevDazObj::Properties(CView* pView)
{
	/*CDevBasePage m_DevBasePage(this);
	CDevDazPage m_DevDazPage(this);
	CString szHT;
	szHT.LoadString(IDS_DEVDAZPROPERTIES);
	CPropertySheet m_Sheet(szHT);
	m_Sheet.AddPage(&m_DevBasePage);
	m_Sheet.AddPage(&m_DevDazPage);

	CLineListPage m_LineList;
	CFillListPage m_FillList;
 	if (m_ParentType != 0)
	{
		szHT.LoadString(IDS_FRAMELINE);
		m_LineList.AddPen(&m_FramePen,szHT);
		szHT.LoadString(IDS_FRAMEFILL);
		m_FillList.AddFill(&m_Brush,szHT);
		m_Sheet.AddPage(&m_LineList);
		m_Sheet.AddPage(&m_FillList);
	}

	CMinFontPage m_FontPage;
	TEXTFONT m_TextFont;
	szHT.LoadString(IDS_ALIASFONT);
	m_TextFont.m_ItemName = szHT.GetBuffer(128);
	szHT.ReleaseBuffer();
	m_TextFont.m_Text = m_AliasName;
	m_TextFont.m_ForeGround = m_NameColor;
	m_TextFont.lf = m_NameFont;
	m_FontPage.AddFont(&m_TextFont);
	TEXTFONT m_TextFont1;
	szHT.LoadString(IDS_ZERFONT);
	m_TextFont1.m_ItemName = szHT.GetBuffer(128);
	szHT.ReleaseBuffer();
	m_TextFont1.m_Text = "012345";
	m_TextFont1.m_ForeGround = m_ScaleColor;
	m_TextFont1.lf = m_ScaleFont;
	m_FontPage.AddFont(&m_TextFont1);

	m_Sheet.AddPage(&m_FontPage);
	double m_PlotSdep1 = m_PlotSdep;
	double m_PlotEdep1 = m_PlotEdep;
	CString m_strSavePathName  = m_strPathName;
	if (m_Sheet.DoModal() == IDOK)
	{
		m_FontPage.OnOK();
		m_NameColor = m_TextFont.m_ForeGround;
		m_NameFont = m_TextFont.lf;
		m_ScaleFont = m_TextFont1.lf;
		m_NameFont.lfEscapement = 0;
		m_ScaleFont.lfEscapement = 0;		
		CRect m_SaveRect = m_Rect;
		m_SaveRect.top = m_HeadRect.top;
		if (m_strSavePathName  != m_strPathName) 
			bErrorFID = FALSE;
		if (m_ParentType != 0) 
		{
			if ((m_PlotSdep != m_PlotSdep1) || (m_PlotEdep != m_PlotEdep1))
			{
				double m_PlotSdep2 = m_PlotSdep;
				double m_PlotEdep2 = m_PlotEdep;
				m_PlotSdep = m_PlotSdep1;
				m_PlotEdep = m_PlotEdep1;
				SetDepth(m_PlotSdep2,m_PlotEdep2);
				if (m_SaveRect.bottom<m_Rect.bottom) 
					m_SaveRect.bottom = m_Rect.bottom;
			}
		}
		pDoc->SetModifiedFlag();
		InvalidateRect(m_SaveRect);
		return TRUE;
	}*/
	return FALSE;
}

int CDevDazObj::HitTest(CView* pView,const CPoint& point)
{
	int nHandle = CFIDDrawObj::HitTest(pView,point); 
	if ((nHandle >= 0) || (!m_Rect.PtInRect(point))) return nHandle;
	if(m_Rect.PtInRect(point)) 
		return 9;
	else return -1;
}

