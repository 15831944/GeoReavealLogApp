/******************************************************************
*   ImageDraw.cpp - implementation of the Wf draw object class     *           
*******************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "GeoFIDio.h"
#include "TrackDraw.h"
#include "ImageDraw.h"
#include "Gradient.h"

//#include "styledialog.h"
#include "ImagePage.h"
//#include "LineListPage.h"
//#include "FillListPage.h"
#include "MinFontDialog.h"

IMPLEMENT_SERIAL(CImageObj, CFIDDrawObj, 0)
        
CImageObj::CImageObj()
{
	m_DrawShape = imageobj;
	m_BkColor = 0xFFFFFF;
	m_ColorGrade = 32;
	m_bColor=TRUE;
	m_LeftValue =0;
	m_RightValue = 1024;
	m_CrMode = 0;
	m_Bit=216;
	m_HorScale=10;
	m_SchemeFile="Wood";
	m_CtrlVer = 2;
}
    
CImageObj::CImageObj(CString szName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDoc)
			:CFIDDrawObj(szName,rect,pParentObj,pDoc)
{
	m_DrawShape = imageobj;
	m_BkColor = 0xFFFFFF;
	m_bColor=TRUE;
	m_LeftValue =0;
	m_RightValue = 1024;
	m_ColorGrade = 32;
	m_CrMode = 0;
	m_Bit=216;
	m_HorScale=(int)(m_Bit*PI/(m_Rect.Width ()*0.254));
	m_HeadRect.top -= 40;
	m_Rect.top = m_HeadRect.top;
	LOGFONT m_LogFont=
	{
		-10,						//lfHeight
		0,							//lfWidth
		0,							//lfEscapement
		0,							//lfOrientation
		FW_NORMAL,						//lfWeight
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
	m_SchemeFile="Wood";
	m_CtrlVer = 3;
}


CImageObj::~CImageObj()
{
}    

void CImageObj::Serialize( CArchive& ar )
{
	CFIDDrawObj::Serialize( ar );
	if( ar.IsStoring() )
	{
		m_CtrlVer=3;
		ar << m_CtrlVer;
		ar << m_ColorGrade;
		ar << m_CrMode;
		ar << m_LeftVal;
		ar << m_RightVal;
		ar << m_BkColor;
		ar << m_bColor;
		ar << m_Bit;
		ar << m_HorScale;
		SerializeLogFont(ar,m_ScaleFont);
		ar << m_SchemeFile;
	}
	else
	{
		pDoc = (CLogPlotDoc*)ar.m_pDocument;
		ar >> m_CtrlVer;
		ar >> m_ColorGrade;
		ar >> m_CrMode;
		if(m_CtrlVer>2)
		{
			ar >> m_LeftVal;
			ar >> m_RightVal;
		}
		else
		{
			ar >> m_LeftValue;
			ar >> m_RightValue;
			m_LeftVal = m_LeftValue;
			m_RightVal = m_RightValue;
		}
		ar >> m_BkColor;
		ar >> m_bColor;
		ar >> m_Bit;
		ar >> m_HorScale;
		SerializeLogFont(ar,m_ScaleFont);
		if(m_CtrlVer>1)
			ar >> m_SchemeFile;
		else
			m_ColorGrade=32;
		CreateImagePalette();		
	}
}

void CImageObj::OnScaleFont()
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

BOOL CImageObj::Properties(CView* pView)
{
	// 1英寸=100像素点 m_Bit 单位 mm
	m_HorScale=(int)(m_Bit*PI/(m_Rect.Width ()*0.254));
	CString szHT;
	CPropertySheet m_Sheet("成像图属性");
	CImageBase m_Base(this);
	CImagePage m_Page(this);
	m_Sheet.AddPage(&m_Base);
	m_Sheet.AddPage(&m_Page);
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
		int w = (int)(m_Bit*PI/m_HorScale/0.254);
		CTrackObj* pObj=(CTrackObj*) this->GetParentObj ();
		if(pObj!=NULL)
		{
			CRect rect;
			rect=pObj->m_Rect ;
			rect.right = rect.left + w;
			pObj->SetRect(rect);
		}
		m_FontPage.OnOK();
		m_NameColor = m_TextFont[0].m_ForeGround;
		m_NameFont = m_TextFont[0].lf;
		m_ScaleFont = m_TextFont[1].lf;
		m_NameFont.lfEscapement = 0;
		m_ScaleFont.lfEscapement = 0;
		CGeoFIDIO* pFIDIO = new CGeoFIDIO(m_strPathName);
		m_Rlev = m_SDep = m_EDep = 0;
		CRect m_SaveRect = m_Rect;
		m_SaveRect.top = m_HeadRect.top;
		if (pFIDIO->Open(pDoc->m_szSever))
		{
			m_Rlev = pFIDIO->GetDepthLevel();
			m_SDep = pFIDIO->GetStartDepth();
			m_EDep = pFIDIO->GetEndDepth();
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
					if (m_SaveRect.bottom<m_Rect.bottom) m_SaveRect.bottom = m_Rect.bottom;
				}
			}
		}
		else bErrorFID = TRUE;
		delete pFIDIO;
		CreateImagePalette();
		pDoc->SetModifiedFlag();
		InvalidateRect(m_SaveRect);
		return TRUE;
	}
	return FALSE;
}

void CImageObj::DrawHead(CDC *pDC)
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
	char ss[256];      
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
	dx = (m_HeadRect.Width()-4.0) / m_ColorGrade;
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
	if(m_strPathName.Find("DYNMI")>0
		||m_strPathName.Find("STAMI")>0
		||m_strPathName.Find("STRMI")>0)
	{
		pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
		if(m_bColor)
			pDC->TextOut(m_HeadRect.left+4,m_HeadRect.bottom - 7,"电导率成像(0-360)");
		else
			pDC->TextOut(m_HeadRect.left+4,m_HeadRect.bottom - 7,"电阻率成像(0-360)");
	}
	
    pDC->SetTextAlign(TA_CENTER | TA_TOP);
    pDC->TextOut((m_HeadRect.left + m_HeadRect.right)/2, rect.bottom, m_UnitText);
    pDC->SetTextAlign(TA_LEFT | TA_TOP);
	dtos(m_LeftVal,ss);
    pDC->TextOut(m_HeadRect.left+2,rect.bottom,ss,(int)strlen(ss));
	dtos(m_RightVal,ss);
    pDC->SetTextAlign(TA_RIGHT | TA_TOP);
    pDC->TextOut(m_HeadRect.right-2,rect.bottom,ss,(int)strlen(ss));  
	pDC->SelectObject(pOldFont);
	m_HeadRect = saveHeadRect;
}

void CImageObj::SetClipRect(CDC* pDC , CRect rect)
{
	if (pDC->m_hDC != NULL) 
		::LPtoDP(pDC->m_hDC,(LPPOINT)(LPRECT)rect,2);
	CRgn rgn; 
	rgn.CreateRectRgn(rect.left,rect.top,rect.right,rect.bottom);
	pDC->SelectClipRgn(&rgn);
}

void CImageObj::CreateImagePalette()
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
void CImageObj::DrawClient(CDC *pDC)
{     
	CRect rect = m_Rect;
	pDoc->LPtoPP(rect);
	if (!pDC->RectVisible(rect)) return;
	if ((m_pParentObj == NULL) || (m_ParentType != 0)) 
		CFIDDrawObj::DrawClient(pDC);
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

	double sdep,edep;
	CGeoFIDIO *pFIDIO = new CGeoFIDIO(m_strPathName);
	if(!pFIDIO->Open(pDoc->m_szSever)) 
		{delete pFIDIO; return;}
	m_Rlev = pFIDIO->GetDepthLevel();
	m_SDep = m_PlotSdep; m_EDep = m_PlotEdep;
	if ((m_EDep < pFIDIO->GetStartDepth()) || (m_SDep > pFIDIO->GetEndDepth()))
		{delete pFIDIO; return;}
	if (m_SDep < pFIDIO->GetStartDepth()) 
		m_SDep = ((int)(pFIDIO->GetStartDepth() / m_Rlev) + 1) * m_Rlev;
	if (m_EDep > pFIDIO->GetEndDepth()) 
		m_EDep = ((int)(pFIDIO->GetEndDepth() / m_Rlev) -1 ) * m_Rlev;

	sdep = YCoorToDepth(rect1.top);
	edep = YCoorToDepth(rect1.bottom);
	if ((sdep > m_EDep) || (edep < m_SDep)) 
		{delete pFIDIO; return;}
	if (sdep < m_SDep) sdep = m_SDep;
	if (edep > m_EDep) edep = m_EDep;		

	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC,DrawRect.Width(),DrawRect.Height());
	CBitmap* pOldBitmap =MemDC.SelectObject(&bmp);

	CBrush brush;
	if(brush.CreateSolidBrush(m_BkColor))
	{
		brush.UnrealizeObject();     
		MemDC.FillRect(DrawRect, &brush);	
	}
	CPalette* pOldPalette;
	pOldPalette = (CPalette*)MemDC.SelectPalette(&m_Palette, FALSE);
	MemDC.RealizePalette();
	CWaitCursor wait;

	long y = DepthToYCoor(sdep)-pDoc->m_WinOrg.y;	 
	FIDCONTENT* Content = pFIDIO->GetContent(3);
	int nps = Content->nps;
	float* fwf = NULL;
	fwf = new float[nps+1];
	char *cwf=NULL;
	cwf = new char[nps+1];
	short *iwf=NULL;
	iwf = new short[nps+1];

	int Ps=m_ColorGrade-1;
	int i,x,w=rect.Width();
	float Dalta=m_RightVal-m_LeftVal;
	float LeftVal=m_LeftVal;
	float Grade=(float)m_ColorGrade;
	int cr;
	long PtNum=0;
	double dep=sdep;
	int x0,y0=DepthToYCoor(dep)-pDoc->m_WinOrg.y;
	if(Content->length<2)
	{
		while(y<rect.bottom&&dep<edep)
		{
			y=this->DepthToYCoor(dep)-pDoc->m_WinOrg.y;
			if(y>y0)
			{
				pFIDIO->SeekToDepth((float)dep);
				pFIDIO->Read(3,nps,cwf);
				for(i=0;i<nps;i++)
					fwf[i]=(float)cwf[i];
				x0=0;
				for(i=0;i<nps;i++)
				{
					x=i*w/nps;
					if((fwf[i]==-9999.f)||(fwf[i]==-999.25f))
					{
						x0=x;
						continue;
					}
					cr =(int)((fwf[i]-LeftVal)/Dalta*Grade);
					if(cr>Ps) cr=Ps;
					MemDC.FillSolidRect(x0,y0-m_ClipRect.top,x-x0,y-y0,PALETTEINDEX(cr));
					x0=x;
				}
				y0=y;
			}
			PtNum++;
 			dep= sdep+((double)PtNum)*m_Rlev;
 		}
	}
	else if(Content->length==2)
	{
		while(y<rect.bottom&&dep<edep)
		{
			y=this->DepthToYCoor(dep)-pDoc->m_WinOrg.y;
			if(y>y0)
			{
				pFIDIO->SeekToDepth((float)dep);
				pFIDIO->Read(3,nps,iwf);
				for(i=0;i<nps;i++)
					fwf[i]=(float)iwf[i];
				x0=0;
				for(i=0;i<nps;i++)
				{
					x=i*w/nps;
					if((fwf[i]==-9999.f)||(fwf[i]==-999.25f))
					{
						x0=x;
						continue;
					}
					cr =(int)((fwf[i]-LeftVal)/Dalta*Grade);
					if(cr>Ps) cr=Ps;
					MemDC.FillSolidRect(x0,y0-m_ClipRect.top,x-x0,y-y0,PALETTEINDEX(cr));
					x0=x;
				}
				y0=y;
			}
			PtNum++;
 			dep= sdep+((double)PtNum)*m_Rlev;
 		}
	}
	else
	{
		while(y<rect.bottom&&dep<edep)
		{
			y=this->DepthToYCoor(dep)-pDoc->m_WinOrg.y;
			if(y>y0)
			{
				pFIDIO->SeekToDepth((float)dep);
				pFIDIO->Read(3,nps,fwf);
				x0=0;
				for(i=0;i<nps;i++)
				{
					x=i*w/nps;
					if((fwf[i]==-9999.f)||(fwf[i]==-999.25f))
					{
						x0=x;
						continue;
					}
					cr =(int)((fwf[i]-LeftVal)/Dalta*Grade);
					if(cr>Ps) cr=Ps;
					MemDC.FillSolidRect(x0,y0-m_ClipRect.top,x-x0,y-y0,PALETTEINDEX(cr));
					x0=x;
				}
				y0=y;
			}
			PtNum++;
 			dep= sdep+((double)PtNum)*m_Rlev;
 		}
	}
	delete pFIDIO;
	delete fwf;delete iwf;delete cwf;
	pDC->BitBlt(rect.left, m_ClipRect.top+Offset, rect.Width(),m_ClipRect.Height()-Offset,&MemDC, 0, Offset,SRCCOPY);
	MemDC.SelectObject(pOldPalette);
	MemDC.SelectObject(pOldBitmap);
	MemDC.DeleteDC ();
	bmp.DeleteObject ();
}
//void CImageObj::DrawClient(CDC *pDC)
//{     
//	CRect rect = m_Rect;
//	pDoc->LPtoPP(rect);
//	if (!pDC->RectVisible(rect)) return;
//	if ((m_pParentObj == NULL) || (m_ParentType != 0)) 
//		CFIDDrawObj::DrawClient(pDC);
//	CRect rect1 = rect;
//	CRect m_ClipRect;
//	pDC->GetClipBox(m_ClipRect);
//	rect1 &= m_ClipRect;
//	pDoc->PPtoLP(rect1);
//
//	int Offset=0;
//	if(rect.top>m_ClipRect.top)
//		Offset=rect.top-m_ClipRect.top;
//	if(m_ClipRect.bottom>rect.bottom)
//		m_ClipRect.bottom=rect.bottom;
//	CRect DrawRect(0,0,rect.Width(),m_ClipRect.Height());
//
//	double sdep,edep;
//	CGeoFIDIO *pFIDIO = new CGeoFIDIO(m_strPathName);
//	if (!pFIDIO->Open(pDoc->m_szSever)) 
//		{delete pFIDIO; return;}
//	m_Rlev = pFIDIO->GetDepthLevel();
//	m_SDep = m_PlotSdep; m_EDep = m_PlotEdep;
//	if ((m_EDep < pFIDIO->GetStartDepth()) || (m_SDep > pFIDIO->GetEndDepth()))
//		{delete pFIDIO; return;}
//	if (m_SDep < pFIDIO->GetStartDepth()) 
//		m_SDep = ((int)(pFIDIO->GetStartDepth() / m_Rlev) + 1) * m_Rlev;
//	if (m_EDep > pFIDIO->GetEndDepth()) 
//		m_EDep = ((int)(pFIDIO->GetEndDepth() / m_Rlev) -1 ) * m_Rlev;
//
//	sdep = YCoorToDepth(rect1.top);
//	edep = YCoorToDepth(rect1.bottom);
//	if ((sdep > m_EDep) || (edep < m_SDep)) 
//		{delete pFIDIO; return;}
//	if (sdep < m_SDep) sdep = m_SDep;
//	if (edep > m_EDep) edep = m_EDep;		
//	
//	CDC MemDC;
//	MemDC.CreateCompatibleDC(pDC);
//	CBitmap bmp;
//	bmp.CreateCompatibleBitmap(pDC,DrawRect.Width(),DrawRect.Height());
//	CBitmap* pOldBitmap =MemDC.SelectObject(&bmp);
//
//	CBrush brush;
//	if(brush.CreateSolidBrush(m_BkColor))
//	{
//		brush.UnrealizeObject();     
//		MemDC.FillRect(DrawRect, &brush);	
//	}
//	CPalette* pOldPalette;
//	pOldPalette = (CPalette*)MemDC.SelectPalette(&m_Palette, FALSE);
//	MemDC.RealizePalette();
//	CWaitCursor wait;
//
//	long y = DepthToYCoor(sdep)-pDoc->m_WinOrg.y;	 
//	FIDCONTENT* Content = pFIDIO->GetContent(3);
//	int nps = Content->nps;
//	
//	//将井段sdep-edep的数据一次性读入
//	unsigned long nRec=(int)((edep-sdep)/m_Rlev);
//	unsigned long nPt=nRec*nps;
//	float* fwf = new float[nPt+nps];
//	pFIDIO->SeekToDepth((float)sdep);
//	unsigned long i;
//	if(Content->length<2)
//	{
//		char *cwf= new char[nPt];
//		pFIDIO->Read(3,nPt,cwf);
//		for(i=0;i<nPt;i++)
//			fwf[i]=(float)cwf[i];
//		delete cwf;
//	}
//	else if(Content->length==2)
//	{
//		short *iwf= new short[nPt];
//		pFIDIO->Read(3,nPt,iwf);
//		for(i=0;i<nPt;i++)
//			fwf[i]=(float)iwf[i];
//		delete iwf;
//	}
//	else
//		pFIDIO->Read(3,nPt,fwf);
//	delete pFIDIO;
//
//	int Ps=m_ColorGrade-1;
//	float Dalta=m_RightVal-m_LeftVal;
//	float LeftVal=m_LeftVal;
//	float Grade=(float)m_ColorGrade;
//	int cr,x,w=rect.Width();
//	long PtNum=0;
//	double dep=sdep;
//	int x0,y0=DepthToYCoor(dep)-pDoc->m_WinOrg.y;
//	while(y<rect.bottom&&dep<edep)
//	{
//		y=DepthToYCoor(dep)-pDoc->m_WinOrg.y;
//		if(y>y0)
//		{
//			x0=0;
//			for(i=0;i<nps;i++)
//			{
//				x=i*w/nps;
//				if((fwf[PtNum*nps+i]==-9999.f)||(fwf[PtNum*nps+i]==-999.25f))
//				{
//					x0=x;
//					continue;
//				}
//				cr =(int)((fwf[PtNum*nps+i]-LeftVal)/Dalta*Grade);
//				if(cr>Ps) cr=Ps;
//				MemDC.FillSolidRect(x0,y0-m_ClipRect.top,x-x0,y-y0,PALETTEINDEX(cr));
//				x0=x;
//			}
//			y0=y;
//		}
//		PtNum++;
// 		dep= sdep+((double)PtNum)*m_Rlev;
// 	}
//	delete fwf;
//	pDC->BitBlt(rect.left, m_ClipRect.top+Offset, rect.Width(),m_ClipRect.Height()-Offset,&MemDC, 0, Offset,SRCCOPY);
//	MemDC.SelectObject(pOldPalette);
//	MemDC.SelectObject(pOldBitmap);
//	MemDC.DeleteDC ();
//	bmp.DeleteObject();
//}

int CImageObj::HitTest(CView* pView,const CPoint& point)
{
	int nHandle = CFIDDrawObj::HitTest(pView,point); 
	if ((nHandle >= 0) || (!m_Rect.PtInRect(point))) return nHandle;
	if(m_Rect.PtInRect(point)) 
		return 9;
	else return -1;
}
