/******************************************************************
*   Curve3D.cpp - implementation of the Wf draw object class     *           
*******************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "GeoFIDio.h"
#include "TrackDraw.h"
#include "Curve3D.h"
#include "Gradient.h"
//#include "styledialog.h"
//#include "Curve3DPage.h"
//#include "LineListPage.h"
//#include "FillListPage.h"
//#include "MinFontDialog.h"
//#include "MainFrm.h"

IMPLEMENT_SERIAL(CCurve3D, CFIDDrawObj, 0)
        
CCurve3D::CCurve3D()
{
	m_DrawShape = curve3D;
	m_Color = 0xFF8000;
	m_FilterType = 4;
	m_ColorGrade = 64;
	m_bColor=FALSE;
	m_Space=5;
	m_PointNum=0;
	m_PlotType=0;
	m_PoleArray=FALSE;
	m_CurveArray.RemoveAll ();
	m_LeftValue =0;
	m_RightValue = 128;
	m_CrMode = 0;
	m_StartPos=0;
	m_LookAngle=30.f;
	m_CurVal0=0;
	m_CurVal1=100;
	m_SchemeFile="Wood";
}
    
CCurve3D::CCurve3D(CString szName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDoc)
			:CFIDDrawObj(szName,rect,pParentObj,pDoc)
{
	m_DrawShape = curve3D;
	m_Color = 0xFF8000;
	m_CurveWidth = 1;
	m_bColor=FALSE;
	m_Space=5;
	m_PointNum=0;
	m_CurveArray.RemoveAll ();
	m_LeftValue =0;
	m_RightValue = 360;
	m_ColorGrade = 64;
	m_CrMode = 0;
	m_StartPos=0;
	m_CurVal0=0;
	m_CurVal1=100;

	m_LookAngle=30.f;
	m_PlotType=0;
	m_PoleArray=FALSE;
	m_FilterType = 4;
	m_HeadRect.top -= 40;
	m_Rect.top = m_HeadRect.top;
	LOGFONT m_LogFont=
	{
		-10,						//lfHeight
		0,							//lfWidth
		0,							//lfEscapement
		0,							//lfOrientation
		FW_NORMAL,					//lfWeight
		0,							//lfItalic
		0,							//lfUnderLine;
		0,							//lfStrikeOut;
		ANSI_CHARSET,				//lfCharSet
		OUT_TT_ONLY_PRECIS,			//lfOutPrecision
		CLIP_DEFAULT_PRECIS,		//lfClipPrecision
		DEFAULT_QUALITY,			//lfQuality
		VARIABLE_PITCH | FF_SWISS,	//lfPitchAndFamily
		"Times New Roman"//"Times New Roman"			//lfFaceName
	};
	m_ScaleFont = m_LogFont; 
	m_SchemeFile="Wood";
}


CCurve3D::~CCurve3D()
{
}    

void CCurve3D::Serialize( CArchive& ar )
{
	CFIDDrawObj::Serialize( ar );
	if( ar.IsStoring() )
	{
		ar << m_CurveWidth;
		ar << m_ColorGrade;
		ar << m_CrMode;
		ar << m_LeftValue;
		ar << m_RightValue;
		ar << m_PlotType;
		ar << m_PoleArray;
		ar << m_FilterType;
		ar << m_Color;
		ar << m_bColor;
		ar << m_Space;
		ar << m_StartPos;
		ar << m_LookAngle;
		ar << m_CurVal0;
		ar << m_CurVal1;
		ar << m_SchemeFile;
		int Num=(int)m_CurveArray .GetSize ();
		ar << Num;
		for(int i=0;i<Num;i++)
			ar<<m_CurveArray[i];
		SerializeLogFont(ar,m_ScaleFont);
	}
	else
	{
		pDoc = (CLogPlotDoc*)ar.m_pDocument;
		ar >> m_CurveWidth;
		ar >> m_ColorGrade;
		ar >> m_CrMode;
		ar >> m_LeftValue;
		ar >> m_RightValue;
		ar >> m_PlotType;
		ar >> m_PoleArray;
		ar >> m_FilterType;
		ar >> m_Color;
		ar >> m_bColor;
		ar >> m_Space;
		ar >> m_StartPos;
		ar >> m_LookAngle;
		ar >> m_CurVal0;
		ar >> m_CurVal1;
		ar >> m_SchemeFile;
		CreateCurvePalette();
		int Num=0;
		ar >> Num;
		CString ss;
		m_CurveArray.RemoveAll ();
		for(int i=0;i<Num;i++)
		{
			ar>>ss;
			m_CurveArray.Add (ss);
		}
		if(Num>0)	CompuePlotPara();
		SerializeLogFont(ar,m_ScaleFont);
		m_LeftVal = m_LeftValue; m_RightVal = m_RightValue;
	}
}

void CCurve3D::OnScaleFont()
{
	int lfEscapement = m_ScaleFont.lfEscapement;
	CFontDialog m_FontDialog((LPLOGFONT)&m_ScaleFont); 
	if(m_FontDialog.DoModal() == IDOK)
	{
		m_ScaleFont.lfEscapement = lfEscapement;
		Invalidate();
		pDoc->SetModifiedFlag();   
	}	
}                        

BOOL CCurve3D::Properties(CView* pView)
{
	/*CString szHT;
	CPropertySheet m_Sheet("特效曲线属性");
	CCurve3DBase m_WaveBase(this);
	CCurve3DPage m_Page(this);
	CLineListPage m_LineList;
	CFillListPage m_FillList;
 	
	m_Sheet.AddPage(&m_WaveBase);
	m_Sheet.AddPage(&m_Page);
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
	TEXTFONT m_TextFont[2];
	szHT.LoadString(IDS_ALIASFONT);
	m_TextFont[0].m_ItemName = szHT.GetBuffer(128);
	szHT.ReleaseBuffer();
	m_TextFont[0].m_Text = m_AliasName;
	m_TextFont[0].m_ForeGround = m_NameColor;
	m_TextFont[0].m_BackGround = RGB(255,255,255);
	m_TextFont[0].lf = m_NameFont;
	int nHeight = m_NameFont.lfHeight;
	m_NameFont.lfHeight = (int) (nHeight * 0.72 - 0.5);
	m_FontPage.AddFont(&m_TextFont[0]);
	m_NameFont.lfHeight = nHeight;
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
	if (m_Sheet.DoModal() == IDOK)
	{
		m_FontPage.OnOK();
		m_NameColor = m_TextFont[0].m_ForeGround;
		m_NameFont = m_TextFont[0].lf;
		m_ScaleFont = m_TextFont[1].lf;
		m_NameFont.lfEscapement = 0;
		m_ScaleFont.lfEscapement = 0;
		m_LeftVal = m_LeftValue; 
		m_RightVal = m_RightValue;
		CRect m_SaveRect = m_Rect;
		CreateCurvePalette();
		pDoc->SetModifiedFlag();
		if(m_CurveArray .GetSize ()>0) 
			CompuePlotPara();
		InvalidateRect(m_SaveRect);
		return TRUE;
	}*/
	return FALSE;
}

void CCurve3D::Draw(CView* pView,CDC* pDC)
{
	if ((m_LeftValue == m_RightValue) && (!bErrorFID))
	{
		bErrorFID = TRUE;
		CGeoFIDIO* pFIDIO = new CGeoFIDIO(m_strPathName);
		if (pFIDIO->Open(pDoc->m_szSever))
		{
			m_Rlev = pFIDIO->GetDepthLevel();
			if (m_Rlev > 0.001)
			{
				m_SDep = pFIDIO->GetStartDepth();
				m_EDep = pFIDIO->GetEndDepth();
				FIDCONTENT* Content = pFIDIO->GetContent(2);
				if ((Content != NULL) && (Content->rlev > 1))
				{
					m_LeftVal = Content->min;
					m_RightVal = Content->min + Content->rlev * Content->nps;
					bErrorFID = FALSE;
				}
			}
		}
		delete pFIDIO;
	}
	CFIDDrawObj::Draw(pView,pDC);
}

void CCurve3D::DrawHead(CDC *pDC)
{
	CFIDDrawObj::DrawHead(pDC);
	CRect saveHeadRect;
	saveHeadRect = m_HeadRect;
	pDoc->LPtoPP(m_HeadRect);
	if (!pDC->RectVisible(m_HeadRect)) 
	{
		m_HeadRect = saveHeadRect;
		return;
	}
	char ss[10];      
	
	CFont m_Font1;
	m_Font1.CreateFontIndirect(&m_NameFont);
	CFont* pOldFont =  pDC->SelectObject(&m_Font1);
    pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_NameColor);
	CSize textsize = pDC->GetTextExtent("文本"); 
    pDC->TextOut((m_HeadRect.left + m_HeadRect.right)/2, m_HeadRect.bottom - textsize.cy -10, m_AliasName,(int)strlen(m_AliasName));
	
	CRect rect = m_HeadRect;
	rect.bottom = rect.top + 30;
	if (rect.bottom > m_HeadRect.bottom) rect.bottom = m_HeadRect.bottom;
	rect.InflateRect(-2,-2);
	CRect rect0=rect;
	int i;double dx;
	dx = (m_HeadRect.Width()-4.0)/m_ColorGrade;
	rect.left = m_HeadRect.left + 2;
	CPalette* pOldPalette = (CPalette*)pDC->SelectPalette(&m_Palette, FALSE);
	pDC->RealizePalette();	
	for (i=0; i<m_ColorGrade; i++)
	{
		rect.right = (int)(m_HeadRect.left + (i+1)*dx) + 2;
		pDC->FillSolidRect(rect,PALETTEINDEX(i));
		rect.left = rect.right;
	}
	pDC->SelectObject(pOldPalette);
	
	CFont m_Font;
	m_Font.CreateFontIndirect(&m_ScaleFont);
    pDC->SelectObject(&m_Font);
	pDC->MoveTo(m_Rect.left,m_HeadRect.bottom-5);
	pDC->LineTo(m_HeadRect.right,m_HeadRect.bottom - 5);
    pDC->SetTextAlign(TA_CENTER | TA_TOP);
	textsize = pDC->GetTextExtent("M");
    pDC->TextOut((m_HeadRect.left + m_HeadRect.right)/2, m_HeadRect.bottom-textsize.cy-10, m_UnitText);
    pDC->SetTextAlign(TA_LEFT | TA_TOP);
	dtos(m_LeftVal,ss);
    pDC->TextOut(m_HeadRect.left+2,rect.bottom,ss,(int)strlen(ss));
	CString str;
	str.Format("%.2f",m_CurVal0);
    pDC->TextOut(m_HeadRect.left+2,m_HeadRect.bottom-textsize.cy-10,str,(int)strlen(str));

	dtos(m_RightVal,ss);
    pDC->SetTextAlign(TA_RIGHT | TA_TOP);
    pDC->TextOut(m_HeadRect.right-2,rect.bottom,ss,(int)strlen(ss));  
	str.Format("%.2f",m_CurVal1);
	pDC->TextOut(m_HeadRect.right-2,m_HeadRect.bottom-textsize.cy-10,str,(int)strlen(str)); 

	pDC->SelectObject(pOldFont);
	pDC->SelectObject(pOldFont);
	m_HeadRect = saveHeadRect;
}

void CCurve3D::SetClipRect(CDC* pDC , CRect rect)
{
	if (pDC->m_hDC != NULL) ::LPtoDP(pDC->m_hDC,(LPPOINT)(LPRECT)rect,2);
	CRgn rgn; rgn.CreateRectRgn(rect.left,rect.top,rect.right,rect.bottom);
	pDC->SelectClipRgn(&rgn);
}

void CCurve3D::CreateCurvePalette()
{
	m_Palette.DeleteObject();		
	LPLOGPALETTE pLogPal = (LPLOGPALETTE) new BYTE[sizeof(LOGPALETTE)+m_ColorGrade * sizeof(PALETTEENTRY)];
	pLogPal->palVersion = 0x300; 
	pLogPal->palNumEntries = m_ColorGrade;	
	
	TCHAR szPath[MAX_PATH]={0};// 定义路径变量
    GetSystemDirectory(szPath, MAX_PATH); 
	CString SysPath=szPath;
	CString ColorFile=SysPath+"\\"+m_SchemeFile+".cip";
	if(!DetectFileExist(ColorFile))
		ColorFile=SysPath+"\\"+"Black.cip";
	CFile file(ColorFile, CFile::modeRead | CFile::shareDenyWrite );
	CArchive ar(&file, CArchive::load);
	CGradient m_Gradient;	
	m_Gradient.Serialize(ar);

	RGBTRIPLE *pal = new RGBTRIPLE[m_ColorGrade], *entry;
	m_Gradient.SetQuantization (m_ColorGrade);
	m_Gradient.MakeEntries(pal,m_ColorGrade);
	for (int i=0; i<m_ColorGrade; i++) 
	{
		if(m_bColor)
			entry = &pal[i];
		else
			entry = &pal[m_ColorGrade-i-1];
		pLogPal->palPalEntry[i].peRed = entry->rgbtRed;
		pLogPal->palPalEntry[i].peGreen = entry->rgbtGreen;
		pLogPal->palPalEntry[i].peBlue =  entry->rgbtBlue;
		pLogPal->palPalEntry[i].peFlags = PC_RESERVED; 
	}
	delete[] pal;
	m_Palette.CreatePalette(pLogPal);
	delete [] (BYTE *)pLogPal;
}

void CCurve3D::DrawClient(CDC *pDC)
{
	CRect rect = m_Rect;
	pDoc->LPtoPP(rect);
	if (!pDC->RectVisible(rect)) return;
	if ((m_pParentObj == NULL) || (m_ParentType != 0)) 
		CFIDDrawObj::DrawClient(pDC);
	if (bErrorFID) return;
	
	double r=rect.Width()/3;
	int StartX=rect.left+rect.Width()/2-(int)r;
	int nps =(int)(r*2.);
	int Amp=(int)r;
	CRect rect1 = rect;
	CRect m_ClipRect;
	pDC->GetClipBox(m_ClipRect);
	rect1 &= m_ClipRect;
	rect1.InflateRect(0,Amp);
	pDoc->PPtoLP(rect1);

	CPalette* pOldPalette;
	pOldPalette = (CPalette*)pDC->SelectPalette(&m_Palette, FALSE);
	pDC->RealizePalette();
	
	int CurveNum=(int)m_CurveArray.GetSize ();
	int pos=rect.left;
	CString WellName=m_strPathName;
	int xxx=m_strPathName.Find (".");
	if(xxx>-1)
		WellName=m_strPathName.Left(xxx+1);
	else
		WellName+=".";
	COLORREF color;
	int i,cr,y;
	double sdep,edep,DrawElipseDep;
	double DataDep=YCoorToDepth(rect.top +(int)(r*sin(m_LookAngle*PI/180.0))+4)-m_PlotSdep;
	if(m_PlotType!=3) 
	{
		DataDep=0;
		nps=(rect.Width()-(CurveNum-1)*m_Space)/CurveNum;
	}
	if(m_PlotType==1||m_PlotType==2)
	{
		CGeoFIDIO *pFIDIO = new CGeoFIDIO(WellName+m_CurveArray[0]);
		if (pFIDIO->Open(pDoc->m_szSever)) 
		{
			m_Rlev = pFIDIO->GetDepthLevel();
			m_SDep = m_PlotSdep;
			DrawElipseDep=m_PlotSdep+DataDep;
			m_EDep = m_PlotEdep;
			if (m_SDep < pFIDIO->GetStartDepth()) 
				m_SDep = ((int)(pFIDIO->GetStartDepth() / m_Rlev) + 1) * m_Rlev;
			if (m_EDep > pFIDIO->GetEndDepth()) 
				m_EDep = ((int)(pFIDIO->GetEndDepth() / m_Rlev) -1 ) * m_Rlev;

			sdep = YCoorToDepth(rect1.top);
			edep = YCoorToDepth(rect1.bottom);
			if (sdep < m_SDep) sdep = m_SDep;
			if (edep > m_EDep) edep = m_EDep;		
		
			y = DepthToYCoor(sdep)-pDoc->m_WinOrg.y;	
			float Value;
			int width;
			CPen Pen3;	
			Pen3.CreatePen(PS_SOLID,1,0xFF8000);
			CPen* OldPen3=pDC->SelectObject (&Pen3);
			while((y<rect.bottom) && (sdep < edep))
			{
				sdep=YCoorToDepth (y+pDoc->m_WinOrg.y);
				pFIDIO->SeekToDepth((float)sdep);
				pFIDIO->Read(2,1,&Value);
				cr = (int)((Value-m_LeftValue*1.0)/((m_RightValue-m_LeftValue)*1.0)* m_ColorGrade);
				if (cr > m_ColorGrade-1) 
					cr = m_ColorGrade-1;
				color=PALETTEINDEX(cr);
				Pen3.DeleteObject();
				Pen3.CreatePen (PS_SOLID,1,color);
				pDC->SelectObject (&Pen3);
				width=(int)((Value-m_CurVal0)/(m_CurVal1-m_CurVal0)* rect.Width());
				if(m_PlotType==1)
				{
					pDC->MoveTo(rect.left ,y);
					pDC->LineTo(rect.left+width,y);
				}
				else
				{
					pDC->MoveTo(rect.right,y);
					pDC->LineTo(rect.right-width,y);
				}
				y++;
			}
			pDC->SelectObject(OldPen3);
		}
		delete pFIDIO;
	}
	else if(m_PlotType==3)
	{
		CGeoFIDIO *pFIDIO = new CGeoFIDIO(WellName+m_CurveArray[0]);
		if (pFIDIO->Open(pDoc->m_szSever)) 
		{
			m_Rlev = pFIDIO->GetDepthLevel();
			m_SDep = m_PlotSdep+DataDep;
			DrawElipseDep=m_PlotSdep+DataDep;
			m_EDep = m_PlotEdep;
			if (m_SDep < pFIDIO->GetStartDepth()) 
			{
				m_SDep = ((int)(pFIDIO->GetStartDepth() / m_Rlev) + 1) * m_Rlev;
				DrawElipseDep=m_SDep;
			}
			if (m_EDep > pFIDIO->GetEndDepth()) 
				m_EDep = ((int)(pFIDIO->GetEndDepth() / m_Rlev) -1 ) * m_Rlev;

			sdep = YCoorToDepth(rect1.top);
			edep = YCoorToDepth(rect1.bottom);
			m_EDep -=DataDep;
			if (sdep < m_SDep) sdep = m_SDep;
			if (edep > m_EDep) edep = m_EDep;		
			
			y = DepthToYCoor(sdep)-pDoc->m_WinOrg.y;	
			float Value;
			int yy;
			double hh=sin(m_LookAngle*PI/180.0);
			double WW=nps*0.5/(rect.Width()*2./3.);
			while ((y< rect.bottom) && (sdep < edep))
			{
				sdep=this->YCoorToDepth (y+pDoc->m_WinOrg.y);
				pFIDIO->SeekToDepth((float)sdep);
				pFIDIO->Read(2,1,&Value);
				cr = (int)((Value-m_LeftValue*1.0)/((m_RightValue-m_LeftValue)*1.0)* m_ColorGrade);
				if (cr > m_ColorGrade-1) cr = m_ColorGrade-1;
				color=PALETTEINDEX(cr);
				for (i=0; i<nps; i++)
				{
					yy=y+(int)(sqrt(r*r-(i-r)*(i-r))*hh);
					pDC->SetPixel (StartX+i,yy,color);
				}
 				y++;
			}
			if(m_SDep ==DrawElipseDep)
			{
				pFIDIO->SeekToDepth((float)(m_SDep+2*m_Rlev));
				pFIDIO->Read(2,1,&Value);
				cr = (int)((Value-m_LeftValue*1.0)/((m_RightValue-m_LeftValue)*1.0)* m_ColorGrade);
				if (cr > m_ColorGrade-1) cr = m_ColorGrade-1;
				color=PALETTEINDEX(cr);
				CBrush Brush2(color);
				CPen Pen2;
				Pen2.CreatePen (PS_SOLID,1,color);
				CPen* OldPen2=pDC->SelectObject (&Pen2);
				CBrush* pOldBrush2 = pDC->SelectObject(&Brush2); 
				int StartY = DepthToYCoor(DrawElipseDep)-pDoc->m_WinOrg.y;
				pDC->Ellipse(StartX,StartY-(int)(r*sin(m_LookAngle*PI/180.0)),StartX+(int)(2.0*r),StartY+(int)(r*sin(m_LookAngle*PI/180.0))+2);
				CBrush Brush3;
				Brush3.CreateSolidBrush (m_Color);
				pDC->SelectObject(&Brush3);				
				pDC->Ellipse(StartX+3,StartY-(int)(r*sin(m_LookAngle*PI/180.0))+2,StartX+(int)(2.0*r)-3,StartY+(int)(r*sin(m_LookAngle*PI/180.0)));
				pDC->SelectObject(pOldBrush2);
				pDC->SelectObject(OldPen2);
			}
		}
		delete pFIDIO;
	}
	else
	{
		for(int CN=0;CN<CurveNum;CN++)
		{
			CGeoFIDIO *pFIDIO = new CGeoFIDIO(WellName+m_CurveArray[CN]);
			if (!pFIDIO->Open(pDoc->m_szSever)) 
				{delete pFIDIO; continue;}
			m_Rlev = pFIDIO->GetDepthLevel();
			m_SDep = m_PlotSdep+DataDep;
			DrawElipseDep=m_PlotSdep+DataDep;
			m_EDep = m_PlotEdep;
			if (m_SDep < pFIDIO->GetStartDepth()) 
			{
				m_SDep = ((int)(pFIDIO->GetStartDepth() / m_Rlev) + 1) * m_Rlev;
				DrawElipseDep=m_SDep;
			}
			if (m_EDep > pFIDIO->GetEndDepth()) 
				m_EDep = ((int)(pFIDIO->GetEndDepth() / m_Rlev) -1 ) * m_Rlev;

			sdep = YCoorToDepth(rect1.top);
			edep = YCoorToDepth(rect1.bottom);
			m_EDep -=DataDep;
			if ((sdep > m_EDep) || (edep < m_SDep)) 
				{delete pFIDIO; break;}
			if (sdep < m_SDep) sdep = m_SDep;
			if (edep > m_EDep) edep = m_EDep;		
		
			y = DepthToYCoor(sdep)-pDoc->m_WinOrg.y;	
			float Value;
			CPen Pen3;	
			Pen3.CreatePen(PS_SOLID,1,0xFF8000);
			CPen* OldPen3=pDC->SelectObject (&Pen3);
			int ps=m_ColorGrade-1;
			while ((y<rect.bottom)&&(sdep<edep))
			{
				sdep=YCoorToDepth (y+pDoc->m_WinOrg.y);
				pFIDIO->SeekToDepth((float)sdep);
				pFIDIO->Read(2,1,&Value);
				cr = (int)((Value-m_LeftValue*1.0)/((m_RightValue-m_LeftValue)*1.0)* m_ColorGrade);
				if (cr >ps) cr = ps;
				color=PALETTEINDEX(cr);
				Pen3.DeleteObject();
				Pen3.CreatePen (PS_SOLID,1,color);
				pDC->SelectObject (&Pen3);
				pDC->MoveTo(pos,y);
				pDC->LineTo(pos+nps,y);
				y++;
			}
			pDC->SelectObject(OldPen3);
			delete pFIDIO;
			pos+=nps;
			if(!m_PoleArray||(m_PoleArray&&((CN%2)==1)))
				pos+=m_Space;
		}
	}
}

void CCurve3D::OnLButtonDblClk(CView* pView, UINT nFlags, const CPoint& point)
{
	if (Properties(pView))
	{
		pDoc->SetModifiedFlag();
		Invalidate(pView);
	}
}

void CCurve3D::CompuePlotPara()
{
	//Compute plot number perdeppoint; 
	m_PointNum=0;
	int num=(int)m_CurveArray.GetSize ();
	CString WellName=m_strPathName;
	int xxx=m_strPathName.Find (".");
	if(xxx>-1)
		WellName=m_strPathName.Left(xxx);
	WellName+=".";
	for (int i=0; i<num; i++)
	{
		CGeoFIDIO* pFIDIO = new CGeoFIDIO(WellName +m_CurveArray[i]);
		if (!pFIDIO->Open(pDoc->m_szSever)) 
			{delete pFIDIO; continue;}
		FIDCONTENT* Content = pFIDIO->GetContent(3);
		if (Content != NULL)
			m_PointNum+= Content->nps;
		delete pFIDIO;
		m_PointNum+=m_Space;
		if((m_PoleArray&&((i%2)==0)))
			m_PointNum-=m_Space;
	}
	if((m_PlotType!=3)||(num==1))
		m_PointNum -=m_Space;
}

int CCurve3D::HitTest(CView* pView,const CPoint& point)
{
	int nHandle = CFIDDrawObj::HitTest(pView,point); 
	if ((nHandle >= 0) || (!m_Rect.PtInRect(point))) return nHandle;
	if(m_Rect.PtInRect(point)) 
		return 9;
	else return -1;
}