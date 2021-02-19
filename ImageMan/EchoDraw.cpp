/******************************************************************
*   EchoDraw.cpp - implementation of the Wf draw object class     *           
*******************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "ImageMan.h"
#include "ImageManDoc.h"
#include "FIDIO.h"
#include "styledialog.h"
#include "TrackDraw.h"
#include "EchoDraw.h"
#include "EchoPage.h"
#include "LineListPage.h"
#include "FillListPage.h"
#include "MinFontDialog.h"
#include "MainFrm.h"

IMPLEMENT_SERIAL(CEchoObj, CFIDDrawObj, 0)
        
CEchoObj::CEchoObj()
{	
	m_DrawShape = echoobj;
	m_EchoType = 0;
	m_CtrlVer=2;
	m_FillScale=FALSE;
	m_EchoFilter=FALSE;
	m_bYlog=FALSE;
	m_bXlog=FALSE;
	PALSIZE=128;
}
    
CEchoObj::CEchoObj(CString szName, const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDoc)
			:CFIDDrawObj(szName,rect,pParentObj,pDoc)
{
	m_CtrlVer=2;
	m_FillScale=FALSE;
	m_EchoFilter=FALSE;
	m_bYlog=FALSE;
	m_bXlog=FALSE;
	m_DrawShape = curveobj;
	m_EchoType = 0;
	m_EchoColor= 0x008000;
	m_FillColor= 0x00D7FF;
	m_EchoWidth = 1;
	m_WfMin=0.0f;
	m_LeftValue =0;
	m_RightValue = 100;
	m_Amp = 40;
	m_WfMax = 10;
	m_Level = 5;
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
	PALSIZE=128;
	CreateWavePalette();
}

CEchoObj::~CEchoObj()
{
}    

void CEchoObj::Serialize( CArchive& ar )
{
	CFIDDrawObj::Serialize( ar );
	if(!ar.IsStoring())
	{
		ar >> m_CtrlVer;
		ar >> m_FillScale;
		ar >> m_EchoFilter;
		ar >> m_EchoWidth;
		ar >> m_Amp;
		if(m_CtrlVer<3)
		{
			int d;
			ar >> d;
			m_WfMax=(float)d;
			WORD dd;
			ar >> dd;
			m_LeftValue=(float)dd;
			ar >> dd;
			m_RightValue=(float)dd;
		}
		else
		{
			ar >> m_WfMax;
			ar >> m_LeftValue;
			ar >> m_RightValue;
		}
		ar >> m_Level;
		ar >> m_EchoType;
		ar >> m_EchoColor;
		ar >> m_WfMin;
		ar >> m_bYlog;
		ar >> m_bXlog;
		if(m_CtrlVer>1)
			ar >> m_FillColor;
		SerializeLogFont(ar,m_ScaleFont);
		m_LeftVal = m_LeftValue; m_RightVal = m_RightValue;
		CreateWavePalette();
	}
}

void CEchoObj::CreateWavePalette()
{
	m_Palette.DeleteObject();
	COLORREF MinClr,MaxClr;
	
	double fRed, fGreen, fBlue;
	LPLOGPALETTE pLogPal = (LPLOGPALETTE) new BYTE[sizeof(LOGPALETTE)+PALSIZE * sizeof(PALETTEENTRY)];
	pLogPal->palVersion = 0x300; /* bloody mysterious */
	pLogPal->palNumEntries = PALSIZE;
	int i;
	MaxClr = m_EchoColor;
	MinClr =m_FillColor;//0xffffff;	
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
	m_Palette.CreatePalette(pLogPal);
	delete [] (BYTE *)pLogPal;
}

void CEchoObj::OnScaleFont()
{
	int lfEscapement = m_ScaleFont.lfEscapement;
	CFontDialog m_FontDialog((LPLOGFONT)&m_ScaleFont); 
	if(m_FontDialog.DoModal() == IDOK)
	{
		m_ScaleFont.lfEscapement = lfEscapement;
		Invalidate();
	}	
}                        

BOOL CEchoObj::Properties(CView* pView)
{
	CString szHT;
	CPropertySheet m_Sheet("回波属性");
	CEchoBase m_EchoBase(this);
	CEchoPage m_Page(this);
	CLineListPage m_LineList;
	CFillListPage m_FillList;
 	
	m_Sheet.AddPage(&m_EchoBase);
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
		CFIDIO* pFIDIO = new CFIDIO(m_strPathName);
		m_Rlev = m_SDep = m_EDep = 0;
		m_LeftVal = m_LeftValue; m_RightVal = m_RightValue;
		CRect m_SaveRect = m_Rect;
		m_SaveRect.top = m_HeadRect.top;
		if (pFIDIO->Open())
		{
			m_Rlev = pFIDIO->GetDepthLevel();
			m_SDep = pFIDIO->GetStartDepth();
			m_EDep = pFIDIO->GetEndDepth();
			m_bErrorFID = FALSE;
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
		else m_bErrorFID = TRUE;
		delete pFIDIO;
		CreateWavePalette();
		InvalidateRect(m_SaveRect);
		return TRUE;
	}
	return FALSE;
}

void CEchoObj::Draw(CView* pView,CDC* pDC)
{
	if ((m_LeftValue == m_RightValue) && (!m_bErrorFID))
	{
		m_bErrorFID = TRUE;
		CFIDIO* pFIDIO = new CFIDIO(m_strPathName);
		if (pFIDIO->Open())
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
					m_bErrorFID = FALSE;
				}
			}
		}
		delete pFIDIO;
	}
	CFIDDrawObj::Draw(pView,pDC);
}

void CEchoObj::DrawHead(CDC *pDC)
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
	CFont m_Font;
	m_Font.CreateFontIndirect(&m_ScaleFont);
    CFont* pOldFont = pDC->SelectObject(&m_Font);
	CPen TxtPen(PS_SOLID,m_EchoWidth,m_EchoColor);
	CPen *pOldPen=pDC->SelectObject (&TxtPen);
	pDC->MoveTo(m_Rect.left,m_HeadRect.bottom-5);
	pDC->LineTo(m_HeadRect.right,m_HeadRect.bottom - 5);
	pDC->SetBkMode(TRANSPARENT);
    pDC->SetTextColor(m_NameColor);
    pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
    pDC->TextOut((m_HeadRect.left + m_HeadRect.right)/2, m_HeadRect.bottom - 7, m_UnitText);
    pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
    dtos(m_LeftVal,ss);
    pDC->TextOut(m_HeadRect.left+2,m_HeadRect.bottom-7,ss,strlen(ss));
    dtos(m_RightVal,ss);
    pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
    pDC->TextOut(m_HeadRect.right-2,m_HeadRect.bottom-7,ss,strlen(ss));
    CSize textsize = pDC->GetTextExtent("文本");   
	pDC->SelectObject(pOldFont);
	CFont m_Font1;
	m_Font1.CreateFontIndirect(&m_NameFont);
    pOldFont = pDC->SelectObject(&m_Font1);
    pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
	if(m_FillScale)
	{
		CBrush br1(m_EchoColor);
		pDC->FillRect(m_HeadRect,&br1);
		if(!m_AliasName.IsEmpty ())
		{
			CRect r;
		    CSize textsize = pDC->GetTextExtent(m_AliasName,m_AliasName.GetLength());   
			r.top = m_HeadRect.top + (m_HeadRect.Height() - textsize.cy) / 2 - 2;
			r.bottom = m_HeadRect.bottom - (m_HeadRect.Height() - textsize.cy) / 2;
			r.left = m_HeadRect.left + (m_HeadRect.Width() - textsize.cx) / 2 - 2;
			r.right = m_HeadRect.right - (m_HeadRect.Width() - textsize.cx) / 2 + 2;
			CBrush TextBoxBrush;
			TextBoxBrush.CreateSolidBrush(RGB(255,255,255));
			CBrush* pOldBrush=pDC->SelectObject (&TextBoxBrush);
			pDC->RoundRect (r.left ,r.top ,r.right ,r.bottom ,7,7);
			pDC->SelectObject (pOldBrush);		
			pDC->TextOut((r.left + r.right)/2, r.bottom , m_AliasName,strlen(m_AliasName));
		}
	}
	else
		pDC->TextOut((m_HeadRect.left + m_HeadRect.right)/2, m_HeadRect.bottom - textsize.cy -10, m_AliasName,strlen(m_AliasName));
	pDC->SelectObject(pOldFont);
	pDC->SelectObject (pOldPen);
	if(m_EchoColor!=m_FillColor&&m_EchoType!=0)
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
			rect.right = (int) (m_Rect.left + (i+1)*dx) + 2;
			pDC->FillSolidRect(rect,PALETTEINDEX(i));
			rect.left = rect.right;
		}
		pDC->SelectObject(pOldPalette);
	
		m_Font.CreateFontIndirect(&m_ScaleFont);
		pOldFont =pDC->SelectObject(&m_Font);
		pDC->SetTextAlign(TA_LEFT | TA_TOP);
		dtos(m_WfMin,ss);
		pDC->TextOut(m_HeadRect.left+2,rect.bottom,ss,strlen(ss));
		dtos(m_WfMax,ss);
		pDC->SetTextAlign(TA_RIGHT | TA_TOP);
		pDC->TextOut(m_HeadRect.right-2,rect.bottom,ss,strlen(ss));  
		pDC->SelectObject(pOldFont);
	}
	m_HeadRect = saveHeadRect;
}

void CEchoObj::SetClipRect(CDC* pDC , CRect rect)
{
	if (pDC->m_hDC != NULL) ::LPtoDP(pDC->m_hDC,(LPPOINT)(LPRECT)rect,2);
	CRgn rgn; rgn.CreateRectRgn(rect.left,rect.top,rect.right,rect.bottom);
	pDC->SelectClipRgn(&rgn);
}

void CEchoObj::DrawClient(CDC *pDC)
{                     
	CRect rect = m_Rect;
	pDoc->LPtoPP(rect);
	if (!pDC->RectVisible(rect)) return;
	if ((m_pParentObj == NULL) || (m_ParentType != 0)) 
		CFIDDrawObj::DrawClient(pDC);
	if (m_bErrorFID) return;
	CFIDIO* pFIDIO = new CFIDIO(m_strPathName);
	if (!pFIDIO->Open()) 
	{
		delete pFIDIO; 
		return;
	}
	m_Rlev = pFIDIO->GetDepthLevel();
	if (m_Rlev < 0.001) 
	{
		delete pFIDIO; return;
	}
 
	CRect rect1 = rect;
	CRect m_ClipRect;
	pDC->GetClipBox(m_ClipRect);
	rect1 &= m_ClipRect;	
	rect1.InflateRect(0,m_Amp);
	BOOL bClip = FALSE;

	m_SDep = m_PlotSdep; m_EDep = m_PlotEdep;
	if ((m_EDep < pFIDIO->GetStartDepth()) || (m_SDep > pFIDIO->GetEndDepth()))
		{delete pFIDIO; return;}
	if (m_SDep < pFIDIO->GetStartDepth()) 
		m_SDep = ((int)(pFIDIO->GetStartDepth() / m_Rlev) + 1) * m_Rlev;
	if (m_EDep > pFIDIO->GetEndDepth()) 
		m_EDep = ((int)(pFIDIO->GetEndDepth() / m_Rlev) -1 ) * m_Rlev;
	pDoc->PPtoLP(rect1);

	double sdep = YCoorToDepth(rect1.top);
	double edep = YCoorToDepth(rect1.bottom);
	if ((sdep > m_EDep) || (edep < m_SDep)) 
	{
		delete pFIDIO;
		return;
	}
	if (sdep < m_SDep) sdep = m_SDep;
	if (edep > m_EDep) edep = m_EDep;

	CPoint* point = NULL;
	short* iwf = NULL;
	float* fwf = NULL;
	char* cwf = NULL;
	CPen m_Pen,*pOldPen;
	int k1,k2,k3,k4,x1,x2,i;
	int k = 0;
	int nps = 0;
	double m_Max,dx;
	
	FIDCONTENT* Content = pFIDIO->GetContent(2);
	if (Content != NULL)
	{
		ASSERT(Content->nps > 0);
		nps = Content->nps;
		point = new CPoint[nps+2];
		m_Max = m_WfMax-m_WfMin;
		if (fabs(m_Max )< 0.1) 
		{
			if (Content->nps == 960) 
				m_Max = 128;
			else m_Max = 2048;
		}
		ASSERT(Content->rlev > 0.001);
		k1 = (int) ((m_LeftVal-Content->min) / Content->rlev);
		k2 = (int) ((m_RightVal-Content->min) / Content->rlev);
		if (k2>k1)
		{
			k3 = (k1 > 1) ? k1 : 1;
			k4 =  (k2 < Content->nps) ? k2 : Content->nps;
			dx = rect.Width() * 1.0 / (k2 - k1);
			if (rect.Width()<k2 - k1)
			{
				x1 = rect.left + (int) ((k3 - k1) * dx);
				x2 = rect.left + (int) ((k4 - k1) * dx);
				k = x2 - x1 + 1;
				for (i=0;i<k;i++)
					point[i+1] = x1 + i;
			}
			else
			{
				k = k4 - k3 + 1;
				if(m_bXlog)
				{
					double left=m_LeftVal-Content->min;
					dx=rect.Width() * 1.0 /(log10(m_RightVal-Content->min)-log10(m_LeftVal-Content->min));					
					for (i=0;i<k;i++)
						point[i+1].x = rect.left + (int) ((log10(left+Content->rlev*i)-log10(left))*dx);
				}
				else
				{
					for (i=0; i<k; i++)
						point[i+1].x = rect.left + (int) ((i + k3 - k1) * dx );
				}
			}
			point[0].x = rect.left; point[k+1].x = rect.right;
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
	CWaitCursor wait;		
	m_Pen.CreatePen(0,m_EchoWidth,m_EchoColor);
	pOldPen = pDC->SelectObject(&m_Pen); 
	float dep;int y, m_num1;
	CRect m_EchoRect=rect;
	if(m_EchoType!=0)                       //填充
	{
		m_num1= (int)((sdep - m_SDep) / m_Rlev);
		m_num1 = (m_num1 / m_Level) * m_Level;
		dep = (float)(m_num1*m_Rlev+m_SDep);
		y=DepthToYCoor(dep);
		y-=pDoc->m_WinOrg.y;
		m_EchoRect.top = y - m_Amp; 
		m_EchoRect.bottom = y;
		
		CPalette* pOldPalette; 
		pOldPalette = (CPalette*)pDC->SelectPalette(&m_Palette, FALSE);
		pDC->RealizePalette();	
	
		double dltay;
		int j,x,cr;
		while(dep<edep)
		{		
			point[0].y = point[k+1].y = y;
			pFIDIO->SeekToDepth(dep);
			if(Content->type == REPR_FLOAT)
			{
				pFIDIO->Read(3,nps,fwf);
				for(i=1;i<nps;i++)
				{
					if(m_EchoFilter)
						fwf[i]=0.212f*fwf[i-1]+0.576f*fwf[i]+0.212f*fwf[i+1];
				}
			}
			else if(Content->type == REPR_CHAR)
			{
				pFIDIO->Read(3,nps,cwf);
				fwf[0]=cwf[0];
				for(i=1;i<nps;i++)
				{
					if(m_EchoFilter)
						fwf[i]=0.212f*cwf[i-1]+0.576f*cwf[i]+0.212f*cwf[i+1];
					else
						fwf[i]=cwf[i];
				}
			}
			else 
			{
				pFIDIO->Read(3,nps,iwf);
				fwf[0]=iwf[0];
				for(i=1;i<nps;i++)
				{
					if(m_EchoFilter)
						fwf[i]=0.212f*iwf[i-1]+0.576f*iwf[i]+0.212f*iwf[i+1];
					else
						fwf[i]=iwf[i];
				}
			}
			if (rect.Width()< k2 - k1)
			{
				int kk;
				for (i=0;i<k; i++)
				{
					kk = (int) (k3 + i / dx - 0.5);
					if (kk > nps-1) 
						kk = nps-1;
					if((fwf[kk]!=0.0f)&&m_bYlog)
						fwf[kk]=(float)log10(fwf[kk]);
					point[i+1].y = y-(int)((fwf[kk]-m_WfMin)* m_Amp/m_Max);
				}
			}
			else
			{
				for (i=0;i<k;i++)
				{
					if((fwf[i+k3-1]!=0.0f)&&m_bYlog)
						fwf[i+k3-1]=(float)log10(fwf[i+k3-1]);
					point[i+1].y = y-(int)((fwf[i+k3-1]-m_WfMin)* m_Amp/m_Max);
				}
			}
			point[k+1].y=point[k].y;

			m_EchoRect.top = y-m_Amp;
			m_EchoRect.bottom = y;
			if (m_EchoRect.top < rect.top) 
				m_EchoRect.top = rect.top;
			SetClipRect(pDC,m_EchoRect);
	/*		for(int j=0;j<k-1;j++)
			{
				int cr = (int)(abs(y-point[j].y)*1.0/m_Amp* PALSIZE);
				if (cr>PALSIZE-1) 
					cr = PALSIZE-1;
				color=PALETTEINDEX(cr);
				pDC->FillSolidRect(point[j].x,y,point[j+1].x-point[j].x,point[j].y-y,color);
			}*/
			point[k+1].y=y;
			for(j=0;j<k;j++)
			{
				for(x=point[j].x;x<point[j+1].x;x++)
				{
					dltay=abs(y-point[j].y)+(point[j].y-point[j+1].y)*(x-point[j].x)/(point[j+1].x-point[j].x);
					if(dltay>0)
					{
						cr = (int)(dltay/m_Amp* PALSIZE);
						if (cr>PALSIZE-1) 
							cr = PALSIZE-1;
						pDC->FillSolidRect(x,y,1,(int)dltay*-1,PALETTEINDEX(cr));
					}
				}
			}
			m_num1 += m_Level;
			dep = (float)(m_SDep + m_num1 * m_Rlev);
			y = DepthToYCoor(dep)-pDoc->m_WinOrg.y;	
		}
		pDC->SelectObject(pOldPalette);
	}
	if(m_EchoType!=1)          //波形
	{
		m_num1= (int)((sdep - m_SDep) / m_Rlev);
		m_num1 = (m_num1 / m_Level) * m_Level;
		dep = (float)(m_num1*m_Rlev+m_SDep);
		y = DepthToYCoor(dep);
		y -= pDoc->m_WinOrg.y;
		m_EchoRect.top = y - m_Amp; 
		m_EchoRect.bottom = y;
		while(dep<edep)
		{		
			point[0].y = point[k+1].y = y;
			pFIDIO->SeekToDepth(dep);
			if (Content->type == REPR_FLOAT)
			{
				pFIDIO->Read(3,nps,fwf);
				for(i=1;i<nps;i++)
				{
					if(m_EchoFilter)
						fwf[i]=0.212f*fwf[i-1]+0.576f*fwf[i]+0.212f*fwf[i+1];
				}
			}
			else if(Content->type == REPR_CHAR)
			{
				pFIDIO->Read(3,nps,cwf);
				fwf[0]=cwf[0];
				for(i=1;i<nps;i++)
				{
					if(m_EchoFilter)
						fwf[i]=0.212f*cwf[i-1]+0.576f*cwf[i]+0.212f*cwf[i+1];
					else
						fwf[i]=cwf[i];
				}
			}
			else 
			{
				pFIDIO->Read(3,nps,iwf);
				fwf[0]=iwf[0];
				for(i=1;i<nps;i++)
				{
					if(m_EchoFilter)
						fwf[i]=0.212f*iwf[i-1]+0.576f*iwf[i]+0.212f*iwf[i+1];
					else
						fwf[i]=iwf[i];
				}
			}
			if (rect.Width()< k2 - k1)
			{
				int kk;
				for (i=0;i<k; i++)
				{
					kk = (int) (k3 + i / dx - 0.5);
					if (kk > nps-1) 
						kk = nps-1;
					if((fwf[kk]!=0.0f)&&m_bYlog)
						fwf[kk]=(float)log10(fwf[kk]);
					point[i+1].y = y-(int)((fwf[kk]-m_WfMin)* m_Amp/m_Max);
					if(point[i+1].y>y) point[i+1].y=y;
				}
			}
			else
			{
				for (i=0;i<k;i++)
				{
					if((fwf[i+k3-1]!=0.0f)&&m_bYlog)
						fwf[i+k3-1]=(float)log10(fwf[i+k3-1]);
					point[i+1].y = y-(int)((fwf[i+k3-1]-m_WfMin)* m_Amp/m_Max);
					if(point[i+1].y>y) point[i+1].y=y;
				}
			}
			point[k+1].y=point[k].y;
				
			m_EchoRect.top = y-m_Amp;
			m_EchoRect.bottom = y+1;
			if (m_EchoRect.top < rect.top) 
				m_EchoRect.top = rect.top;
			if (m_EchoRect.bottom > rect.bottom) 
				m_EchoRect.bottom = rect.bottom;
			SetClipRect(pDC,m_EchoRect);
			pDC->Polyline(point,k+1);

			m_num1 += m_Level;
			dep = float(m_SDep + m_num1*m_Rlev);
			y = DepthToYCoor(dep)-pDoc->m_WinOrg.y;	
		}
	}
	delete pFIDIO;delete fwf;
	if (point != NULL)	delete point;
	if (cwf != NULL)	delete cwf;
	if (iwf != NULL)	delete iwf;
	m_ClipRect.InflateRect(3,3);
	SetClipRect(pDC,m_ClipRect);
	pDC->SelectObject(pOldPen);
}

void CEchoObj::OnLButtonDblClk(CView* pView, UINT nFlags, const CPoint& point)
{
	if (Properties(pView))
		Invalidate(pView);
}

int CEchoObj::HitTest(CView* pView,const CPoint& point)
{
	int nHandle = CFIDDrawObj::HitTest(pView,point); 
	if ((nHandle >= 0) || (!m_Rect.PtInRect(point))) return nHandle;
	if(m_Rect.PtInRect(point)) 
		return 9;
	else return -1;
}
