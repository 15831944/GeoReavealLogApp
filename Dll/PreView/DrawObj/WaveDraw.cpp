/******************************************************************
*   WaveDraw.cpp - implementation of the Wf draw object class     *           
*******************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "GeoFIDio.h"
#include "TrackDraw.h"
#include "WaveDraw.h"
//#include "WavePage.h"
//#include "LineListPage.h"
//#include "FillListPage.h"
//#include "MinFontDialog.h"
//#include "styledialog.h"

//#include "MainFrm.h"


IMPLEMENT_SERIAL(CWaveObj, CFIDDrawObj, 0)
        
CWaveObj::CWaveObj()
{	
	m_DrawShape = curveobj;
	m_WaveType = 0;
	PALSIZE=128;
}
    
CWaveObj::CWaveObj(CString szName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDoc)
			:CFIDDrawObj(szName,rect,pParentObj,pDoc)
{
	m_DrawShape = curveobj;
	m_WaveType = 0;
	PALSIZE=128;
	m_WaveColor = RGB(0,0,0);
	m_WaveWidth = 1;
	m_StartTime=0.0f;
	m_LeftValue =200;
	m_RightValue = 1200;
	m_Amp = 10;
	m_WfMax = 10;
	m_Level = 10;
	m_HeadRect.top -= 40;
	m_Rect.top = m_HeadRect.top;
	LOGFONT m_LogFont=
	{
		-10,						//lfHeight
		0,							//lfWidth
		0,							//lfEscapement
		0,							//lfOrientation
		300,						//lfWeight
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
	m_ScaleFont = m_LogFont; 
	CreateWavePalette();
}

CWaveObj::~CWaveObj()
{
}    

void CWaveObj::Serialize( CArchive& ar )
{
	CFIDDrawObj::Serialize( ar );
	if( ar.IsStoring() )
	{
		ar << m_WaveWidth;
		ar << m_Amp;
		ar << m_WfMax;
		ar << m_LeftValue;
		ar << m_RightValue;
		ar << m_Level;
		ar << m_WaveType;
		ar << m_WaveColor;
		ar << m_StartTime;
		ar << PALSIZE;
		SerializeLogFont(ar,m_ScaleFont);
	}
	else
	{
		pDoc = (CLogPlotDoc*)ar.m_pDocument;
		ar >> m_WaveWidth;
		ar >> m_Amp;
		ar >> m_WfMax;
		ar >> m_LeftValue;
		ar >> m_RightValue;
		ar >> m_Level;
		ar >> m_WaveType;
		ar >> m_WaveColor;
		ar >> m_StartTime;
		if(pDoc->m_VersionNo>1.f)
			ar >> PALSIZE;
		SerializeLogFont(ar,m_ScaleFont);
		m_LeftVal = m_LeftValue; 
		m_RightVal = m_RightValue;
		if ((m_WaveType != 0)&&(m_WaveType != 1)&&(m_WaveType != 3)&&(m_WaveType != 6))
			CreateWavePalette();
	}
}

void CWaveObj::OnScaleFont()
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

BOOL CWaveObj::Properties(CView* pView)
{
	/*CString szHT;
	szHT.LoadString(IDS_WAVEFULLPROPERTIES);
	CPropertySheet m_Sheet(szHT);
	CWaveBase m_WaveBase(this);
	CWavePage m_Page(this);
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
		CGeoFIDIO* pFIDIO = new CGeoFIDIO(m_strPathName);
		m_Rlev = m_SDep = m_EDep = 0;
		m_LeftVal = m_LeftValue; m_RightVal = m_RightValue;
		CRect m_SaveRect = m_Rect;
		m_SaveRect.top = m_HeadRect.top;
		if (pFIDIO->Open(pDoc->m_szSever))
		{
			m_Rlev = pFIDIO->GetDepthLevel();
			m_SDep = pFIDIO->GetStartDepth();
			m_EDep = pFIDIO->GetEndDepth();
			bErrorFID = FALSE;
			if (m_LeftValue == m_RightValue)
			{
				FIDCONTENT* Content = pFIDIO->GetContent(2);
				if (Content != NULL)
				{
					m_LeftVal = Content->min;
					m_RightVal = Content->min +Content->rlev * Content->nps;
				}
			}
			if (m_ParentType != 0) 
			{
				if ((m_PlotSdep != m_PlotSdep1) || (m_PlotEdep != m_PlotEdep1))
				{
					double m_PlotSdep2 = m_PlotSdep;
					double m_PlotEdep2 = m_PlotEdep;
					m_PlotSdep = m_PlotSdep1;
					m_PlotEdep = m_PlotEdep1;
					SetDepth(m_PlotSdep2,m_PlotEdep2);
					if (m_SaveRect.bottom<m_Rect.bottom) m_SaveRect.bottom = m_Rect.bottom;
				}
			}
		}
		else bErrorFID = TRUE;
		delete pFIDIO;
		if ((m_WaveType != 0)&&(m_WaveType != 1)&&(m_WaveType != 3)&&(m_WaveType != 6))
			CreateWavePalette();
		pDoc->SetModifiedFlag();
		InvalidateRect(m_SaveRect);
		return TRUE;
	}*/
	return FALSE;
}

void CWaveObj::Draw(CView* pView,CDC* pDC)
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

void CWaveObj::DrawHead(CDC *pDC)
{
	CFIDDrawObj::DrawHead(pDC);
	CRect saveHeadRect;
	saveHeadRect = m_HeadRect;
	pDoc->LPtoPP(m_HeadRect);
	if(!pDC->RectVisible(m_HeadRect)) 
	{
		m_HeadRect = saveHeadRect;
		return;
	}
	CPen Pen(0,m_WaveWidth,m_WaveColor);
	CPen *pOldPen = pDC->SelectObject(&Pen); 
	pDC->MoveTo(m_Rect.left,m_HeadRect.bottom-5);
	pDC->LineTo(m_HeadRect.right,m_HeadRect.bottom - 5);
	pDC->SelectObject(pOldPen);

	char ss[10];      
	CFont m_Font;
	m_Font.CreateFontIndirect(&m_ScaleFont);
    CFont* pOldFont = pDC->SelectObject(&m_Font);
	pDC->SetBkMode(TRANSPARENT);
    pDC->SetTextColor(m_WaveColor);
    pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
    pDC->TextOut((m_HeadRect.left + m_HeadRect.right)/2, m_HeadRect.bottom - 7, m_UnitText);
    pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
    dtos(m_LeftVal,ss);
    pDC->TextOut(m_HeadRect.left+2,m_HeadRect.bottom-7,ss,(int)strlen(ss));
    dtos(m_RightVal,ss);
    pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
    pDC->TextOut(m_HeadRect.right-2,m_HeadRect.bottom-7,ss,(int)strlen(ss));
    CSize textsize = pDC->GetTextExtent("文本");   
	pDC->SelectObject(pOldFont);
	CFont m_Font1;
	m_Font1.CreateFontIndirect(&m_NameFont);
    pOldFont = pDC->SelectObject(&m_Font1);
    pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
    pDC->TextOut((m_HeadRect.left + m_HeadRect.right)/2, m_HeadRect.bottom - textsize.cy -10, m_AliasName,(int)strlen(m_AliasName));
	pDC->SelectObject(pOldFont);
	if ((m_WaveType == 2)||(m_WaveType == 4)||(m_WaveType == 5)||(m_WaveType == 7))
	{
		CRect rect = m_HeadRect;
		rect.bottom = rect.top + 30;
		if (rect.bottom > m_HeadRect.bottom) rect.bottom = m_HeadRect.bottom;
		rect.InflateRect(-2,-2);
		CRect rect0=rect;
		int i;double dx;
		dx = (m_HeadRect.Width()-4.0) / PALSIZE;
		rect.left = m_HeadRect.left + 2;
		CPalette* pOldPalette = (CPalette*)pDC->SelectPalette(&m_Palette, FALSE);
		pDC->RealizePalette();	
		for (i=0; i<PALSIZE; i++)
		{
			rect.right = (int)(m_HeadRect.left + (i+1)*dx) + 2;
			pDC->FillSolidRect(rect,PALETTEINDEX(i));
			rect.left = rect.right;
		}
		pDC->SelectObject(pOldPalette);
		
		m_Font.CreateFontIndirect(&m_ScaleFont);
		pOldFont =pDC->SelectObject(&m_Font);
		pDC->SetTextAlign(TA_LEFT | TA_TOP);
		pDC->TextOut(m_HeadRect.left+2,rect.bottom,"o");
		dtos(m_WfMax,ss);
		pDC->SetTextAlign(TA_RIGHT | TA_TOP);
		pDC->TextOut(m_HeadRect.right-2,rect.bottom,ss,(int)strlen(ss));  
		pDC->SelectObject(pOldFont);
	}
	m_HeadRect = saveHeadRect;
}

void CWaveObj::SetClipRect(CDC* pDC,CRect rect)
{
	if(pDC->m_hDC != NULL) 
		::LPtoDP(pDC->m_hDC,(LPPOINT)(LPRECT)rect,2);
	CRgn rgn; 
	rgn.CreateRectRgn(rect.left,rect.top,rect.right,rect.bottom);
	pDC->SelectClipRgn(&rgn);
}

void CWaveObj::CreateWavePalette()
{
	m_Palette.DeleteObject();
	COLORREF MinClr,MaxClr;
	
	double fRed, fGreen, fBlue;
	if(m_WaveType == 4)
		PALSIZE=64;
	LPLOGPALETTE pLogPal = (LPLOGPALETTE) new BYTE[sizeof(LOGPALETTE)+PALSIZE * sizeof(PALETTEENTRY)];
	pLogPal->palVersion = 0x300; /* bloody mysterious */
	pLogPal->palNumEntries = PALSIZE;
	int i;
	if((m_WaveType == 4)||(m_WaveType == 7))
	{
		COLORREF Color1[64] = {
		0xFFFFFF,0xE7FFFF,0xC7FFFF,0xAFFFFF,0x97F7FF,0x77EFFF,0x5FE7FF,0x57DFFF,
		0x5FD7FF,0x5FCFFF,0x5FC7FF,0x5FC7FF,0x67BFFF,0x67BFFF,0x3FB7FF,0x07B7FF,
		0x00AFFF,0x00AFFF,0x00A7FF,0x00A7FF,0x00A7FF,0x009FFF,0x009FFF,0x0097FF,
		0x0097F7,0x0097F7,0x008FEF,0x008FEF,0x0087E7,0x0087E7,0x0087DF,0x007FDF,
		0x007FD7,0x0077D7,0x0077CF,0x0077CF,0x006FC7,0x006FC7,0x0067BF,0x0067BF,
		0x0067B7,0x005FB7,0x005FAF,0x0057AF,0x0057A7,0x0057A7,0x004F9F,0x004F9F,
		0x004797,0x004797,0x003F8F,0x003F8F,0x003F87,0x00377F,0x00377F,0x002F77,
		0x002F6F,0x002767,0x001F5F,0x001757,0x00174F,0x000F47,0x000737,0x00002F};
		for (i=0; i<PALSIZE; i++) 
		{
			pLogPal->palPalEntry[i].peRed = GetRValue(Color1[i]);
			pLogPal->palPalEntry[i].peGreen = GetGValue(Color1[i]);
			pLogPal->palPalEntry[i].peBlue =  GetBValue(Color1[i]);
			pLogPal->palPalEntry[i].peFlags = PC_RESERVED; 
		}
	}
	else
	{
		MaxClr = m_WaveColor;
		if (MaxClr == 0xffffff) MaxClr = 0;
		MinClr =0xffffff;	
		fRed =  (GetRValue(MaxClr) - GetRValue(MinClr))/(PALSIZE-1.0);
		fGreen = (GetGValue(MaxClr) - GetGValue(MinClr))/(PALSIZE-1.0);
		fBlue =  (GetBValue(MaxClr) - GetBValue(MinClr))/(PALSIZE-1.0);	
		for (i=0; i<PALSIZE; i++) 
		{
			pLogPal->palPalEntry[i].peRed = GetRValue(MinClr) + (int)(i*fRed);
			pLogPal->palPalEntry[i].peGreen = GetGValue(MinClr) + (int)(i*fGreen);
			pLogPal->palPalEntry[i].peBlue =  GetBValue(MinClr) + (int)(i*fBlue);
			pLogPal->palPalEntry[i].peFlags = PC_RESERVED; 
		}
	}
	m_Palette.CreatePalette(pLogPal);
	delete [] (BYTE *)pLogPal;
}

void CWaveObj::DrawClient(CDC *pDC)
{   
	CRect rect = m_Rect;
	pDoc->LPtoPP(rect);
	if (!pDC->RectVisible(rect)) return;
	if ((m_pParentObj == NULL) || (m_ParentType != 0)) 
		CFIDDrawObj::DrawClient(pDC);
	if (bErrorFID) return;
	CRect rect1 = rect;
	CRect m_ClipRect;
	pDC->GetClipBox(m_ClipRect);
	rect1 &= m_ClipRect;
	pDoc->PPtoLP(rect1);

	int Offset=0;
	if(rect.top>m_ClipRect.top)
		Offset=rect.top-m_ClipRect.top;
	if(m_ClipRect.bottom>rect.bottom)
		m_ClipRect.bottom=rect.bottom;
	CRect DrawRect(0,0,rect.Width(),m_ClipRect.Height());

	CGeoFIDIO* pFIDIO = new CGeoFIDIO(m_strPathName);
	if (!pFIDIO->Open(pDoc->m_szSever)) 
	{
		delete pFIDIO; 
		return;
	}
	m_Rlev = pFIDIO->GetDepthLevel();
	if (m_Rlev < 0.001) 
	{
		delete pFIDIO; return;
	}

	if (m_WaveType >1) 
		m_Level = 1;

	m_SDep = m_PlotSdep; m_EDep = m_PlotEdep;
	if ((m_EDep < pFIDIO->GetStartDepth()) || (m_SDep > pFIDIO->GetEndDepth()))
		{delete pFIDIO; return;}
	if (m_SDep < pFIDIO->GetStartDepth()) 
		m_SDep = ((int)(pFIDIO->GetStartDepth() / m_Rlev) + 1) * m_Rlev;
	if (m_EDep > pFIDIO->GetEndDepth()) 
		m_EDep = ((int)(pFIDIO->GetEndDepth() / m_Rlev) -1 ) * m_Rlev;

	double sdep = YCoorToDepth(rect1.top);
	double edep = YCoorToDepth(rect1.bottom);
	if ((sdep>m_EDep)||(edep<m_SDep)) 
	{
		delete pFIDIO;
		return;
	}
	if (sdep < m_SDep) sdep = m_SDep;
	if (edep > m_EDep) edep = m_EDep;

	int m_num1 = (int)((sdep - m_SDep) / m_Rlev);
	m_num1 = (m_num1 / m_Level) * m_Level;
	sdep = m_num1*m_Rlev+m_SDep;
	
	CPoint* point = NULL;
	short* iwf = NULL;
	float* fwf = NULL;
	char* cwf = NULL;
	float* ftemp=NULL;
	int k1,k2,k3,k4,x1,x2,i;
	int k = 0;
	int nps = 0;
	float m_Max,dx;
	BOOL bRevolve=FALSE;
	float LeftScale=m_LeftVal;
	float RightScale=m_RightVal;
	if(m_LeftVal>m_RightVal) 
	{
		bRevolve=TRUE;
		LeftScale=m_RightVal;
		RightScale=m_LeftVal;
	}
	FIDCONTENT* Content = pFIDIO->GetContent(2);
	if (Content != NULL)
	{
		ASSERT(Content->nps > 0);
		nps = Content->nps;
		point = new CPoint[nps+2];
		m_Max = m_WfMax;
		if (m_Max < 0.1) 
		{
			if (Content->nps == 960) m_Max = 128;
			else m_Max = 2048;
		}
		ASSERT(Content->rlev > 0.001);
		k1 = (int) ((LeftScale-m_StartTime-Content->min) / Content->rlev);
		k2 = (int) ((RightScale-m_StartTime-Content->min) / Content->rlev);
		if (k2 > k1)
		{
			k3 = (k1 > 1) ? k1 : 1;
			k4 =  (k2 < Content->nps) ? k2 : Content->nps;
			dx = rect.Width() * 1.f/ (k2 - k1);
			if (rect.Width() < k2 - k1)
			{
				x1 = (int) ((k3 - k1) * dx);
				x2 = (int) ((k4 - k1) * dx);
				k = x2 - x1 + 1;
				for (i=0;i<k;i++)
					point[i+1] = x1 + i;
			}
			else
			{
				k = k4 - k3 + 1;
				for (i=0; i<k; i++)
					point[i+1].x = (int) ((i + k3 - k1) * dx + 0.5);
			}
			point[0].x = point[1].x; point[k+1].x = point[k].x;
		}
	}
	if(k==0)
	{
		delete pFIDIO;
		if (point != NULL)	
			delete point;
		return;
	}
	Content = pFIDIO->GetContent(3);
	if(Content != NULL)
	{
		ftemp= new float[nps];
		fwf = new float[nps];	
		if(Content->type == REPR_CHAR) 
			cwf = new char[nps];
		else if(Content->type != REPR_FLOAT)
			iwf = new short[nps];
	}
	else 
	{
		delete pFIDIO;
		if (point != NULL)	
			delete point;
		return;
	}
	
	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC,DrawRect.Width(),DrawRect.Height());
	CBitmap* pOldBitmap =MemDC.SelectObject(&bmp);
	CBrush brush;
	if(brush.CreateSolidBrush(0xffffff))
	{
		brush.UnrealizeObject();     
		MemDC.FillRect(DrawRect, &brush);	
	}

	CWaitCursor wait;
	CPen m_Pen,*pOldPen;
	CPalette* pOldPalette; 
	pOldPalette = (CPalette*)MemDC.SelectPalette(&m_Palette, FALSE);
	MemDC.RealizePalette();	
	m_Pen.CreatePen(0,m_WaveWidth,m_WaveColor);
	pOldPen = MemDC.SelectObject(&m_Pen); 

	CBrush br(m_WaveColor);
	CBrush* pOldBrush = MemDC.SelectObject(&br);
	int y = DepthToYCoor(sdep)-pDoc->m_WinOrg.y;
	if(m_WaveType <2)
	{				
		CRect m_WaveRect = DrawRect;
		m_WaveRect.top = y - m_Amp; 
		m_WaveRect.bottom = y + m_Amp;
		while ((m_WaveRect.top < rect.bottom) && (sdep < edep))
		{		
			if(m_WaveRect.bottom > rect.top)
			{
				point[0].y = point[k+1].y = y;
				pFIDIO->SeekToDepth((float)sdep);
				if(Content->type == REPR_FLOAT)
					pFIDIO->Read(3,nps,fwf);
				else if (Content->type == REPR_CHAR)
				{
					pFIDIO->Read(3,nps,cwf);
					for(i=0;i<nps;i++)
						fwf[i]=(float)cwf[i];
				}
				else 
				{
					pFIDIO->Read(3,nps,iwf);
					for(i=0;i<nps;i++)
						fwf[i]=(float)iwf[i];
				}
				//绘制对数变换波形
				if(m_WaveType ==1)
				{
					for (i=0;i<nps; i++)
					{					
						fwf[i]+=m_Max*6.f;
						if(fwf[i]>0.f)
							fwf[i]=(log10(fwf[i])-log10(m_Max*5.f))/(log10(m_Max*7.f)-log10(m_Max*5.f))*m_Max*2.f;
						fwf[i]-=m_Max;
					}
				}
				if(bRevolve)
				{
					for(i=0;i<nps;i++)
						ftemp[i]=fwf[i];
					for(i=0;i<nps;i++)
						fwf[i]=ftemp[nps-1-i];
				}

				if (rect.Width() < k2 - k1)
				{
					int kk;
					for (i=0; i<k; i++)
					{
						kk = (int) (k3 + i / dx - 0.5);
						if (kk > nps-1) 
							kk = nps-1;
						point[i+1].y = y - (int) (fwf[kk]*m_Amp/m_Max)-m_ClipRect.top;
					}
				}
				else
					for (i=0; i<k; i++)
						point[i+1].y = y - (int) (fwf[i+k3-1]*m_Amp/m_Max)-m_ClipRect.top;
				
				if(m_WaveRect.top < rect.top) 
					m_WaveRect.top = rect.top;
				if(m_WaveRect.bottom > rect.bottom) 
					m_WaveRect.bottom = rect.bottom;	
				m_WaveRect.top-=m_ClipRect.top;
				m_WaveRect.bottom-=m_ClipRect.top;
				SetClipRect(&MemDC,m_WaveRect);
				MemDC.Polyline(point,k+2);
			}
			m_num1 += m_Level;
			sdep = m_SDep + m_num1 * m_Rlev;
			y = DepthToYCoor(sdep)-pDoc->m_WinOrg.y;
			m_WaveRect.top = y - m_Amp;
			m_WaveRect.bottom = y + m_Amp;
		}
	}
	else
	{ 
		while ((y < rect.bottom)&& (sdep < edep))
		{		
			if(y-m_Amp>rect.top)
			{
				sdep=YCoorToDepth(y+pDoc->m_WinOrg.y);
				point[0].y = point[k+1].y = y;
				pFIDIO->SeekToDepth((float)sdep);
				if (Content->type == REPR_FLOAT)
				{					
					pFIDIO->Read(3,nps,fwf);
					if(m_WaveType>4)
						for(i=0;i<nps;i++)
							fwf[i]*=-1.f;
				}
				else if (Content->type == REPR_CHAR)
				{
					pFIDIO->Read(3,nps,cwf);
					if(m_WaveType>4)
					{
						for(i=0;i<nps;i++)
							fwf[i]=(float)cwf[i]*-1.f;
					}
					else
					{
						for(i=0;i<nps;i++)
							fwf[i]=(float)cwf[i];
					}
				}
				else 
				{
					pFIDIO->Read(3,nps,iwf);
					if(m_WaveType>4)
					{
						for(i=0;i<nps;i++)
							fwf[i]=(float)iwf[i]*-1.f;
					}
					else
					{
						for(i=0;i<nps;i++)
							fwf[i]=(float)iwf[i];
					}
				}
				if(bRevolve)
				{
					for(i=0;i<nps;i++)
						ftemp[i]=fwf[i];
					for(i=0;i<nps;i++)
						fwf[i]=ftemp[nps-1-i];
				}
				if (rect.Width() < k2 - k1)
				{
					int kk;
					for (i=0; i<k; i++)
					{
						kk = (int) (k3 + i / dx - 0.5);
						if (kk > nps-1) 
							kk = nps-1;
						point[i+1].y = y - (int) ((fwf[kk]/m_Max) * m_Amp);
					}
				}
				else
				{
					for (i=0; i<k; i++)
						point[i+1].y = y - (int) ((fwf[i+k3-1]/m_Max) * m_Amp);
				}
				if ((m_WaveType == 3)||(m_WaveType==6))
				{
					for(int xx=0;xx<k+1;xx++)
					{
						if(point[xx].y<y)
						{
							MemDC.MoveTo(point[xx].x,y-m_Amp-m_ClipRect.top);
							MemDC.LineTo(point[xx+1].x,y-m_ClipRect.top);
						}
					}
				}
	 		    else
				{
					COLORREF color;
					for(int xx=0;xx<k+1;xx++)
					{
						if(point[xx].y<y)
						{
							m_Pen.DeleteObject();
							int cr = (int) ((y-point[xx].y)*1.0/ m_Amp * PALSIZE);
							if (cr > PALSIZE-1) cr = PALSIZE-1;
							color=PALETTEINDEX(cr);
							m_Pen.CreatePen(0,m_WaveWidth,color);
							MemDC.SelectObject(&m_Pen);
							MemDC.MoveTo(point[xx].x,y-m_Amp-m_ClipRect.top);
							MemDC.LineTo(point[xx+1].x,y-m_ClipRect.top);
						}
					}
				}
			}
			y+=m_WaveWidth;
		}
	}
	delete pFIDIO;delete ftemp;
	if (point != NULL)	delete point;
	if (cwf != NULL)	delete cwf;
	if (iwf != NULL)	delete iwf;
	if (fwf != NULL)	delete fwf;
	pDC->BitBlt(rect.left, m_ClipRect.top+Offset, rect.Width(),m_ClipRect.Height()-Offset,&MemDC, 0, Offset,SRCCOPY);//SRCAND);
	MemDC.SelectObject(pOldPalette);
	MemDC.SelectObject(pOldBitmap);	
	MemDC.SelectObject(pOldBrush);
	MemDC.SelectObject(pOldPen);
	MemDC.DeleteDC ();
	bmp.DeleteObject ();
}

void CWaveObj::DrawDirectClient(CDC *pDC)
{   
	CRect rect = m_Rect;
	pDoc->LPtoPP(rect);
	if (!pDC->RectVisible(rect)) return;
	if ((m_pParentObj == NULL) || (m_ParentType != 0)) 
		CFIDDrawObj::DrawClient(pDC);
	if (bErrorFID) return;
	CRect rect1 = rect;
	CRect m_ClipRect;
	pDC->GetClipBox(m_ClipRect);
	rect1 &= m_ClipRect;
	pDoc->PPtoLP(rect1);
	CGeoFIDIO* pFIDIO = new CGeoFIDIO(m_strPathName);
	if (!pFIDIO->Open(pDoc->m_szSever)) 
	{
		delete pFIDIO; 
		return;
	}
	m_Rlev = pFIDIO->GetDepthLevel();
	if (m_Rlev < 0.001) 
	{
		delete pFIDIO; return;
	}

	if (m_WaveType >1) 
		m_Level = 1;

	m_SDep = m_PlotSdep; m_EDep = m_PlotEdep;
	if ((m_EDep < pFIDIO->GetStartDepth()) || (m_SDep > pFIDIO->GetEndDepth()))
		{delete pFIDIO; return;}
	if (m_SDep < pFIDIO->GetStartDepth()) 
		m_SDep = ((int)(pFIDIO->GetStartDepth() / m_Rlev) + 1) * m_Rlev;
	if (m_EDep > pFIDIO->GetEndDepth()) 
		m_EDep = ((int)(pFIDIO->GetEndDepth() / m_Rlev) -1 ) * m_Rlev;

	double sdep = YCoorToDepth(rect1.top);
	double edep = YCoorToDepth(rect1.bottom);
	if ((sdep > m_EDep) || (edep < m_SDep)) 
	{
		delete pFIDIO;
		return;
	}
	if (sdep < m_SDep) sdep = m_SDep;
	if (edep > m_EDep) edep = m_EDep;

	int m_num1 = (int)((sdep - m_SDep) / m_Rlev);
	m_num1 = (m_num1 / m_Level) * m_Level;
	sdep = m_num1*m_Rlev+m_SDep;
	
	CPoint* point = NULL;
	short* iwf = NULL;
	float* fwf = NULL;
	char* cwf = NULL;
	float* ftemp=NULL;
	int k1,k2,k3,k4,x1,x2,i;
	int k = 0;
	int nps = 0;
	float m_Max,dx;
	BOOL bRevolve=FALSE;
	float LeftScale=m_LeftVal;
	float RightScale=m_RightVal;
	if(m_LeftVal>m_RightVal) 
	{
		bRevolve=TRUE;
		LeftScale=m_RightVal;
		RightScale=m_LeftVal;
	}

	FIDCONTENT* Content = pFIDIO->GetContent(2);
	if (Content != NULL)
	{
		ASSERT(Content->nps > 0);
		nps = Content->nps;
		point = new CPoint[nps+2];
		m_Max = m_WfMax;
		if (m_Max < 0.1) 
		{
			if (Content->nps == 960) m_Max = 128;
			else m_Max = 2048;
		}
		ASSERT(Content->rlev > 0.001);
		k1 = (int) ((LeftScale-m_StartTime-Content->min) / Content->rlev);
		k2 = (int) ((RightScale-m_StartTime-Content->min) / Content->rlev);
		if (k2 > k1)
		{
			k3 = (k1 > 1) ? k1 : 1;
			k4 =  (k2 < Content->nps) ? k2 : Content->nps;
			dx = rect.Width() * 1.f / (k2 - k1);
			if (rect.Width() < k2 - k1)
			{
				x1 = rect.left+(int) ((k3 - k1) * dx);
				x2 = rect.left+(int) ((k4 - k1) * dx);
				k = x2 - x1 + 1;
				for (i=0;i<k;i++)
					point[i+1] = x1 + i;
			}
			else
			{
				k = k4 - k3 + 1;
				for (i=0; i<k; i++)
					point[i+1].x = rect.left+(int) ((i + k3 - k1) * dx + 0.5);
			}
			point[0].x = point[1].x; point[k+1].x = point[k].x;
		}
	}
	if(k==0)
	{
		delete pFIDIO;
		if (point != NULL)	
			delete point;
		return;
	}
	Content = pFIDIO->GetContent(3);
	if(Content != NULL)
	{
		ftemp=new float[nps];
		fwf = new float[nps];
		if (Content->type == REPR_CHAR) 
			cwf = new char[nps];
		else if (Content->type != REPR_FLOAT)
			iwf = new short[nps];
	}
	else 
	{
		delete pFIDIO;
		if (point != NULL)	
			delete point;
		return;
	}
		
	CWaitCursor wait;
	CPen m_Pen,*pOldPen;
	CPalette* pOldPalette; 
	pOldPalette = (CPalette*)pDC->SelectPalette(&m_Palette, FALSE);
	pDC->RealizePalette();	
	m_Pen.CreatePen(0,m_WaveWidth,m_WaveColor);
	pOldPen = pDC->SelectObject(&m_Pen); 
	
	CBrush br(m_WaveColor);
	CBrush* pOldBrush = pDC->SelectObject(&br);
	int y = DepthToYCoor(sdep)-pDoc->m_WinOrg.y;
	if(m_WaveType <2)
	{				
		CRect m_WaveRect = rect;
		m_WaveRect.top = y - m_Amp; 
		m_WaveRect.bottom = y + m_Amp;
		while ((m_WaveRect.top < rect.bottom) && (sdep < edep))
		{		
			if(m_WaveRect.bottom > rect.top)
			{
				point[0].y = point[k+1].y = y;
				pFIDIO->SeekToDepth((float)sdep);
				if (Content->type == REPR_FLOAT)
					pFIDIO->Read(3,nps,fwf);
				else if (Content->type == REPR_CHAR)
				{
					pFIDIO->Read(3,nps,cwf);
					for(i=0;i<nps;i++)
						fwf[i]=(float)cwf[i];
				}
				else 
				{
					pFIDIO->Read(3,nps,iwf);
					for(i=0;i<nps;i++)
						fwf[i]=(float)iwf[i];
				}
				//绘制对数变换波形
				if(m_WaveType ==1)
				{
					for (i=0;i<nps; i++)
					{					
						fwf[i]+=m_Max*2.f;
						if(fwf[i]>0.f)
							fwf[i]=(log10(fwf[i])-log10(m_Max))/(log10(m_Max*3.f)-log10(m_Max))*m_Max*2.f;
						fwf[i]-=m_Max;
					}
				}
				if(bRevolve)
				{
					for(i=0;i<nps;i++)
						ftemp[i]=fwf[i];
					for(i=0;i<nps;i++)
						fwf[i]=ftemp[nps-1-i];
				}
				if (rect.Width() < k2 - k1)
				{
					int kk;
					for (i=0; i<k; i++)
					{
						kk = (int) (k3 + i / dx - 0.5);
						if (kk > nps-1) 
							kk = nps-1;
						point[i+1].y = y - (int) ((fwf[kk]/m_Max) * m_Amp)-m_ClipRect.top;
					}
				}
				else
					for (i=0; i<k; i++)
						point[i+1].y = y - (int) ((fwf[i+k3-1]/m_Max) * m_Amp)-m_ClipRect.top;

				if (m_WaveRect.top < rect.top) 
					m_WaveRect.top = rect.top;
				if (m_WaveRect.bottom > rect.bottom) 
					m_WaveRect.bottom = rect.bottom;	
				SetClipRect(pDC,m_WaveRect);
				pDC->Polyline(point,k+2);
			}
			m_num1 += m_Level;
			sdep = m_SDep + m_num1 * m_Rlev;
			y = DepthToYCoor(sdep)-pDoc->m_WinOrg.y;
			m_WaveRect.top = y - m_Amp;
			m_WaveRect.bottom = y + m_Amp;
		}
	}
	else
	{ 
		while ((y < rect.bottom)&& (sdep < edep))
		{		
			if(y-m_Amp>rect.top)
			{
				sdep=YCoorToDepth (y+pDoc->m_WinOrg.y);
				point[0].y = point[k+1].y = y;
				pFIDIO->SeekToDepth((float)sdep);
				if (Content->type == REPR_FLOAT)
				{					
					pFIDIO->Read(3,nps,fwf);
					if(m_WaveType>4)
						for(i=0;i<nps;i++)
							fwf[i]*=-1.f;
				}
				else if (Content->type == REPR_CHAR)
				{
					pFIDIO->Read(3,nps,cwf);
					if(m_WaveType>4)
					{
						for(i=0;i<nps;i++)
							fwf[i]=(float)cwf[i]*-1.f;
					}
					else
					{
						for(i=0;i<nps;i++)
							fwf[i]=(float)cwf[i];
					}
				}
				else 
				{
					pFIDIO->Read(3,nps,iwf);
					if(m_WaveType>4)
					{
						for(i=0;i<nps;i++)
							fwf[i]=(float)iwf[i]*-1.f;
					}
					else
					{
						for(i=0;i<nps;i++)
							fwf[i]=(float)iwf[i];
					}
				}
				if(bRevolve)
				{
					for(i=0;i<nps;i++)
						ftemp[i]=fwf[i];
					for(i=0;i<nps;i++)
						fwf[i]=ftemp[nps-1-i];
				}
				if (rect.Width() < k2 - k1)
				{
					int kk;
					for (i=0; i<k; i++)
					{
						kk = (int) (k3 + i / dx - 0.5);
						if (kk > nps-1) 
							kk = nps-1;
						point[i+1].y = y - (int) ((fwf[kk]/m_Max) * m_Amp);
					}
				}
				else
				{
					for (i=0; i<k; i++)
						point[i+1].y = y - (int) ((fwf[i+k3-1]/m_Max) * m_Amp);
				}				
				if ((m_WaveType == 3)||(m_WaveType==6))
				{
					for(int xx=0;xx<k+1;xx++)
					{
						if(point[xx].y<y)
						{
							pDC->MoveTo (point[xx].x,y);
							pDC->LineTo(point[xx+1].x,y);
						}
					}
				}
	 		    else
				{
					COLORREF color;
					for(int xx=0;xx<k+1;xx++)
					{
						if(point[xx].y<y)
						{
							m_Pen.DeleteObject();
							int cr = (int) ((y-point[xx].y)*1.0/ m_Amp * PALSIZE);
							if (cr > PALSIZE-1) cr = PALSIZE-1;
							color=PALETTEINDEX(cr);
							m_Pen.CreatePen(0,m_WaveWidth,color);
							pDC->SelectObject(&m_Pen);
							pDC->MoveTo (point[xx].x,y);
							pDC->LineTo(point[xx+1].x,y);
						}
					}
				}
			}
			y+=m_WaveWidth;
		}
	}
	delete pFIDIO;
	if (point != NULL)	delete point;
	if (cwf != NULL)	delete cwf;
	if (iwf != NULL)	delete iwf;
	if (fwf != NULL)	delete fwf;
	if (ftemp != NULL)	delete ftemp;
	pDC->SelectObject(pOldPalette);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
}

void CWaveObj::OnLButtonDblClk(CView* pView, UINT nFlags, const CPoint& point)
{
	if (Properties(pView))
	{
		pDoc->SetModifiedFlag();
		Invalidate(pView);
	}
}

int CWaveObj::HitTest(CView* pView,const CPoint& point)
{
	int nHandle = CFIDDrawObj::HitTest(pView,point); 
	if ((nHandle >= 0) || (!m_Rect.PtInRect(point))) return nHandle;
	if(m_Rect.PtInRect(point)) 
		return 9;
	else return -1;
}
