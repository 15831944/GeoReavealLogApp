/******************************************************************
*   Curve3D.cpp - implementation of the Wf draw object class     *           
*******************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "ImageMan.h"
#include "ImageManDoc.h"
#include "FIDIO.h"
#include "styledialog.h"
#include "TrackDraw.h"
#include "Curve3D.h"
#include "Curve3DPage.h"
#include "LineListPage.h"
#include "FillListPage.h"
#include "MinFontDialog.h"
#include "MainFrm.h"

IMPLEMENT_SERIAL(CCurve3D, CFIDDrawObj, 0)
        
CCurve3D::CCurve3D()
{
	m_DrawShape = curve3D;
	m_FilterType = 4;
	m_ColorGrade = 0;
	m_Palsize=30;
	m_bColor=FALSE;
	m_Space=5;
	m_PointNum=0;
	m_Daz=TRUE;
	m_PoleArray=FALSE;
	m_CurveArray.RemoveAll ();
	m_LeftValue =0;
	m_RightValue = 128;
	m_CrMode = 0;
	m_StartPos=0;
	m_LookAngle=30.f;
	m_Bit=216;
	m_HorScale=40;
}
    
CCurve3D::CCurve3D(CString szName, const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDoc)
			:CFIDDrawObj(szName,rect,pParentObj,pDoc)
{
	m_DrawShape = curve3D;
	m_CurveColor = RGB(0,0,0);
	m_CurveWidth = 1;
	m_bColor=FALSE;
	m_Space=5;
	m_PointNum=0;
	m_CurveArray.RemoveAll ();
	m_LeftValue =0;
	m_RightValue = 360;
	m_ColorGrade = 0;
	m_CrMode = 0;
	m_StartPos=0;
	m_Bit=216;
	m_HorScale=40;

	m_LookAngle=30.f;
	m_Daz=TRUE;
	m_PoleArray=FALSE;
	m_FilterType = 4;
	m_Palsize=30;	
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
		"楷体_GB2312"//"Times New Roman"			//lfFaceName
	};
	m_ScaleFont = m_LogFont; 
}


CCurve3D::~CCurve3D()
{
}    

void CCurve3D::Serialize( CArchive& ar )
{
	CFIDDrawObj::Serialize( ar );
	if(!ar.IsStoring())
	{
		pDoc = (CImageManDoc*)ar.m_pDocument;
		ar >> m_CurveWidth;
		ar >> m_ColorGrade;
		ar >> m_CrMode;
		ar >> m_LeftValue;
		ar >> m_RightValue;
		ar >> m_Daz;
		ar >> m_PoleArray;
		ar >> m_FilterType;
		ar >> m_CurveColor;
		ar >> m_bColor;
		ar >> m_Space;
		ar >> m_StartPos;
		ar >> m_LookAngle;
		ar >> m_Bit;
		ar >> m_HorScale;
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
		CreateCurvePalette();		
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
	}	
}                        

BOOL CCurve3D::Properties(CView* pView)
{
	CString szHT;
	CPropertySheet m_Sheet("成像图属性");
	CCurve3DBase m_WaveBase(this);
	CCurve3DPage m_Page(this);
	CLineListPage m_LineList;
	CFillListPage m_FillList;
 	
	m_Sheet.AddPage(&m_WaveBase);
	m_Sheet.AddPage(&m_Page);
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
		if(m_ColorGrade==0)
			m_Palsize=30;
		else
			m_Palsize=60;
		CreateCurvePalette();
		if(m_CurveArray .GetSize ()>0) 
			CompuePlotPara();
		InvalidateRect(m_SaveRect);
		return TRUE;
	}
	return FALSE;
}

void CCurve3D::Draw(CView* pView,CDC* pDC)
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
				FIDCONTENT* Content = pFIDIO->GetContent("t");
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
    pDC->TextOut((m_HeadRect.left + m_HeadRect.right)/2, m_HeadRect.bottom - textsize.cy -10, m_AliasName,strlen(m_AliasName));
	
	CRect rect = m_HeadRect;
	rect.bottom = rect.top + 30;
	if (rect.bottom > m_HeadRect.bottom) rect.bottom = m_HeadRect.bottom;
	rect.InflateRect(-2,-2);
	CRect rect0=rect;
	int i;double dx;
	dx = (m_HeadRect.Width()-4.0) / m_Palsize;
	rect.left = m_HeadRect.left + 2;
	CPalette* pOldPalette = (CPalette*)pDC->SelectPalette(&m_Palette, FALSE);
	pDC->RealizePalette();	
	for(i=0; i<m_Palsize; i++)
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
    pDC->SetTextAlign(TA_CENTER | TA_TOP);//BOTTOM
    pDC->TextOut((m_HeadRect.left + m_HeadRect.right)/2, rect.bottom, m_UnitText);
    pDC->SetTextAlign(TA_LEFT | TA_TOP);
	dtos(m_LeftVal,ss);
    pDC->TextOut(m_HeadRect.left+2,rect.bottom,ss,strlen(ss));
	dtos(m_RightVal,ss);
    pDC->SetTextAlign(TA_RIGHT | TA_TOP);
    pDC->TextOut(m_HeadRect.right-2,rect.bottom,ss,strlen(ss));  
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
	if(m_ColorGrade==0)
		m_Palsize=16;
	else
	{
		if(m_CrMode==2)
			m_Palsize=64;
		else
			m_Palsize=32;
	}
	if(m_CrMode ==4)
		m_Palsize=16;
	COLORREF MinClr,MaxClr;	
	double fRed, fGreen, fBlue;
	LPLOGPALETTE pLogPal = (LPLOGPALETTE) new BYTE[sizeof(LOGPALETTE)+m_Palsize * sizeof(PALETTEENTRY)];
	pLogPal->palVersion = 0x300; /* bloody mysterious */
	pLogPal->palNumEntries = m_Palsize;
	int i;
	if(m_CrMode ==5)//阿特拉斯配色方案
	{
		if(m_ColorGrade==0)
		{
			COLORREF Color[16] = {
			0xFFFFFF,0xD5F7FF,0xAAEAFF,0x7AE1FF,0x4ED5FF,0x21CCFF,0x00B5FF,0x0080FF,
			0x004BFA,0x0065E3,0x007FC8,0x0058AE,0x002D95,0x000577,0x00003C,0x000000};
			if(m_bColor)
			{
				for (i=0; i<16; i++) 
				{
					pLogPal->palPalEntry[i].peRed = GetRValue(Color[i]);
					pLogPal->palPalEntry[i].peGreen = GetGValue(Color[i]);
					pLogPal->palPalEntry[i].peBlue =  GetBValue(Color[i]);
					pLogPal->palPalEntry[i].peFlags = PC_RESERVED; 
				}
			}
			else
			{
				for (i=0; i<16; i++) 
				{
					pLogPal->palPalEntry[i].peRed = GetRValue(Color[15-i]);
					pLogPal->palPalEntry[i].peGreen = GetGValue(Color[15-i]);
					pLogPal->palPalEntry[i].peBlue =  GetBValue(Color[15-i]);
					pLogPal->palPalEntry[i].peFlags = PC_RESERVED; 
				}
			}
		}
		else
		{
			COLORREF Color1[32] = {
			0xFFFFFF,0XEDFBFF,0xD5F7FF,0xC1F1FF,0xAAEAFF,0x94E6FF,0x7AE1FF,0x65DAFF,
			0x4ED5FF,0x3AD0FF,0x21CCFF,0x0FC3FF,0x00B5FF,0x009AFF,0x0080FF,0x0066FF,
			0x004EFF,0x004BFA,0x0058F2,0x0065E3,0x0072D9,0x007FC8,0x006EBF,0x0058AE,
			0x0043A4,0x002D95,0x001789,0x000577,0x000057,0x00003C,0x00002B,0x000000};
			if(m_bColor)
			{
				for (i=0; i<32; i++) 
				{
					pLogPal->palPalEntry[i].peRed = GetRValue(Color1[i]);
					pLogPal->palPalEntry[i].peGreen = GetGValue(Color1[i]);
					pLogPal->palPalEntry[i].peBlue =  GetBValue(Color1[i]);
					pLogPal->palPalEntry[i].peFlags = PC_RESERVED; 
				}
			}
			else
			{
				for (i=0; i<32; i++) 
				{
					pLogPal->palPalEntry[i].peRed = GetRValue(Color1[31-i]);
					pLogPal->palPalEntry[i].peGreen = GetGValue(Color1[31-i]);
					pLogPal->palPalEntry[i].peBlue =  GetBValue(Color1[31-i]);
					pLogPal->palPalEntry[i].peFlags = PC_RESERVED; 
				}
			}
		}
		m_Palette.CreatePalette(pLogPal);
		delete [] (BYTE *)pLogPal;
		return;
	}
	if(m_CrMode ==4)
	{
		COLORREF Color[16] = {
		0xFFFFFF,0x8FEFFF,0x5FD7FF,0x67BFFF,0x00AFFF,0x009FFF,0x0097EF,0x0087DF,
		0x0077CF,0x0067BF,0x005FAF,0x004F9F,0x003F8F,0x002F77,0x001F57,0x00002F};
		if(m_bColor)
		{
			for (i=0; i<16; i++) 
			{
				pLogPal->palPalEntry[i].peRed = GetRValue(Color[i]);
				pLogPal->palPalEntry[i].peGreen = GetRValue(Color[i]);
				pLogPal->palPalEntry[i].peBlue =  GetRValue(Color[i]);
				pLogPal->palPalEntry[i].peFlags = PC_RESERVED; 
			}
		}
		else
		{
			for (i=0; i<16; i++) 
			{
				pLogPal->palPalEntry[i].peRed = GetRValue(Color[15-i]);
				pLogPal->palPalEntry[i].peGreen = GetRValue(Color[15-i]);
				pLogPal->palPalEntry[i].peBlue =  GetRValue(Color[15-i]);
				pLogPal->palPalEntry[i].peFlags = PC_RESERVED; 
			}
		}
		m_Palette.CreatePalette(pLogPal);
		delete [] (BYTE *)pLogPal;
		return;
	}
	if(m_CrMode==2)//斯仑贝谢配色方案
	{
		if(m_ColorGrade==0)
		{
			COLORREF Color0[16] = {
			0xFFFFFF,0x8FEFFF,0x5FD7FF,0x67BFFF,0x00AFFF,0x009FFF,0x0097EF,0x0087DF,
			0x0077CF,0x0067BF,0x005FAF,0x004F9F,0x003F8F,0x002F77,0x001F57,0x00002F};
			if(m_bColor)
			{
				for (i=0; i<16; i++) 
				{
					pLogPal->palPalEntry[i].peRed = GetRValue(Color0[i]);
					pLogPal->palPalEntry[i].peGreen = GetGValue(Color0[i]);
					pLogPal->palPalEntry[i].peBlue =  GetBValue(Color0[i]);
					pLogPal->palPalEntry[i].peFlags = PC_RESERVED; 
				}
			}
			else
			{
				for (i=0; i<16; i++) 
				{
					pLogPal->palPalEntry[i].peRed = GetRValue(Color0[15-i]);
					pLogPal->palPalEntry[i].peGreen = GetGValue(Color0[15-i]);
					pLogPal->palPalEntry[i].peBlue =  GetBValue(Color0[15-i]);
					pLogPal->palPalEntry[i].peFlags = PC_RESERVED; 
				}
			}
		}
		else
		{
			COLORREF Color0[64] = {
			0xFFFFFF,0xE7FFFF,0xC7FFFF,0xAFFFFF,0x97F7FF,0x77EFFF,0x5FE7FF,0x57DFFF,
			0x5FD7FF,0x5FCFFF,0x5FC7FF,0x5FC7FF,0x67BFFF,0x67BFFF,0x3FB7FF,0x07B7FF,
			0x00AFFF,0x00AFFF,0x00A7FF,0x00A7FF,0x00A7FF,0x009FFF,0x009FFF,0x0097FF,
			0x0097F7,0x0097F7,0x008FEF,0x008FEF,0x0087E7,0x0087E7,0x0087DF,0x007FDF,
			0x007FD7,0x0077D7,0x0077CF,0x0077CF,0x006FC7,0x006FC7,0x0067BF,0x0067BF,
			0x0067B7,0x005FB7,0x005FAF,0x0057AF,0x0057A7,0x0057A7,0x004F9F,0x004F9F,
			0x004797,0x004797,0x003F8F,0x003F8F,0x003F87,0x00377F,0x00377F,0x002F77,
			0x002F6F,0x002767,0x001F5F,0x001757,0x00174F,0x000F47,0x000737,0x00002F};
			if(m_bColor)
			{
				for (i=0; i<64; i++) 
				{
					pLogPal->palPalEntry[i].peRed = GetRValue(Color0[i]);
					pLogPal->palPalEntry[i].peGreen = GetGValue(Color0[i]);
					pLogPal->palPalEntry[i].peBlue =  GetBValue(Color0[i]);
					pLogPal->palPalEntry[i].peFlags = PC_RESERVED; 
				}
			}
			else
			{
				for (i=0; i<64; i++) 
				{
					pLogPal->palPalEntry[i].peRed = GetRValue(Color0[63-i]);
					pLogPal->palPalEntry[i].peGreen = GetGValue(Color0[63-i]);
					pLogPal->palPalEntry[i].peBlue =  GetBValue(Color0[63-i]);
					pLogPal->palPalEntry[i].peFlags = PC_RESERVED; 
				}
			}
		}
		m_Palette.CreatePalette(pLogPal);
		delete [] (BYTE *)pLogPal;
		return;
	}
	unsigned long cr0,cr1,cr2,cr3;
	if(m_bColor)
	{
		cr0=0xffffff;cr3=0;
		if(m_CrMode ==0)
		{
			cr1=RGB(255,255,0);
			cr2=RGB(255,0,0);			
		}
		else if(m_CrMode ==2)
		{
			cr3=0x00002F;
			cr1=0x67BFFF;
			cr2=0x0087DF;
		}
		else
		{
			cr1=0x39C0FF;
			cr2=0x004EFF;
		}
	}
	else
	{
		cr3=0xffffff;cr0=0;
		if(m_CrMode==0)
		{
			cr2=RGB(255,255,0);
			cr1=RGB(255,0,0);
		}
		else if(m_CrMode ==2)
		{
			cr0=0x00002F;
			cr1=0x0087DF;
			cr2=0x67BFFF;
		}
		else
		{
			cr2=0x39C0FF;
			cr1=0x004EFF;
		}
	}
	if(m_ColorGrade!=0)
	{
		// 32 grade color
		MinClr =cr0;
		MaxClr =cr1; 		
		fRed =  (GetRValue(MaxClr) - GetRValue(MinClr))/11;
		fGreen = (GetGValue(MaxClr) - GetGValue(MinClr))/11;
		fBlue =  (GetBValue(MaxClr) - GetBValue(MinClr))/11;			
		for (i=0; i<10; i++) 
		{
			pLogPal->palPalEntry[i].peRed = GetRValue(MinClr) + (int)(i*fRed);
			pLogPal->palPalEntry[i].peGreen = GetGValue(MinClr) + (int)(i*fGreen);
			pLogPal->palPalEntry[i].peBlue =  GetBValue(MinClr) + (int)(i*fBlue);
			pLogPal->palPalEntry[i].peFlags = PC_RESERVED; 
		}
		MinClr =cr1;
		MaxClr =cr2;
		fRed =  (GetRValue(MaxClr) - GetRValue(MinClr))/13;
		fGreen = (GetGValue(MaxClr) - GetGValue(MinClr))/13;
		fBlue =  (GetBValue(MaxClr) - GetBValue(MinClr))/13;	
		for (i=0; i<12; i++) 
		{
			pLogPal->palPalEntry[10+i].peRed = GetRValue(MinClr) + (int)(i*fRed);
			pLogPal->palPalEntry[10+i].peGreen = GetGValue(MinClr) + (int)(i*fGreen);
			pLogPal->palPalEntry[10+i].peBlue =  GetBValue(MinClr) + (int)(i*fBlue);
			pLogPal->palPalEntry[10+i].peFlags = PC_RESERVED; 
		}
		MinClr =cr2;	
		MaxClr =cr3;
		fRed =  (GetRValue(MaxClr) - GetRValue(MinClr))/11;
		fGreen = (GetGValue(MaxClr) - GetGValue(MinClr))/11;
		fBlue =  (GetBValue(MaxClr) - GetBValue(MinClr))/11;	
		for (i=0; i<10; i++) 
		{
			pLogPal->palPalEntry[22+i].peRed = GetRValue(MinClr) + (int)(i*fRed);
			pLogPal->palPalEntry[22+i].peGreen = GetGValue(MinClr) + (int)(i*fGreen);
			pLogPal->palPalEntry[22+i].peBlue =  GetBValue(MinClr) + (int)(i*fBlue);
			pLogPal->palPalEntry[22+i].peFlags = PC_RESERVED; 
		}
	}
	else
	{
		// 16 grade color
		MinClr =cr0;
		MaxClr =cr1; 		
		fRed =  (GetRValue(MaxClr) - GetRValue(MinClr))/6;
		fGreen = (GetGValue(MaxClr) - GetGValue(MinClr))/6;
		fBlue =  (GetBValue(MaxClr) - GetBValue(MinClr))/6;			
		for (i=0; i<5; i++) 
		{
			pLogPal->palPalEntry[i].peRed = GetRValue(MinClr) + (int)(i*fRed);
			pLogPal->palPalEntry[i].peGreen = GetGValue(MinClr) + (int)(i*fGreen);
			pLogPal->palPalEntry[i].peBlue =  GetBValue(MinClr) + (int)(i*fBlue);
			pLogPal->palPalEntry[i].peFlags = PC_RESERVED; 
		}
		MinClr =cr1;
		MaxClr =cr2;
		fRed =  (GetRValue(MaxClr) - GetRValue(MinClr))/7;
		fGreen = (GetGValue(MaxClr) - GetGValue(MinClr))/7;
		fBlue =  (GetBValue(MaxClr) - GetBValue(MinClr))/7;	
		for (i=0; i<6; i++) 
		{
			pLogPal->palPalEntry[5+i].peRed = GetRValue(MinClr) + (int)(i*fRed);
			pLogPal->palPalEntry[5+i].peGreen = GetGValue(MinClr) + (int)(i*fGreen);
			pLogPal->palPalEntry[5+i].peBlue =  GetBValue(MinClr) + (int)(i*fBlue);
			pLogPal->palPalEntry[5+i].peFlags = PC_RESERVED; 
		}
		MinClr =cr2;	
		MaxClr =cr3;
		fRed =  (GetRValue(MaxClr) - GetRValue(MinClr))/6;
		fGreen = (GetGValue(MaxClr) - GetGValue(MinClr))/6;
		fBlue =  (GetBValue(MaxClr) - GetBValue(MinClr))/6;	
		for (i=0; i<5; i++) 
		{
			pLogPal->palPalEntry[11+i].peRed = GetRValue(MinClr) + (int)(i*fRed);
			pLogPal->palPalEntry[11+i].peGreen = GetGValue(MinClr) + (int)(i*fGreen);
			pLogPal->palPalEntry[11+i].peBlue =  GetBValue(MinClr) + (int)(i*fBlue);
			pLogPal->palPalEntry[11+i].peFlags = PC_RESERVED; 
		}
	}
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
	if (m_bErrorFID) return;
	
	double r=m_Rect.Width()/3;
	int StartX=m_Rect.left +m_Rect.Width()/2-(int)r;
	int nps =(int)(r*2.);
	if(!m_Daz)
	{
		nps=m_Rect.Width();
		StartX=m_Rect.left;
	}
	int Amp=(int)r;
	CRect m_ClipRect;
	pDC->GetClipBox(m_ClipRect);
	rect &= m_ClipRect;
	CRect rect1 = rect;
	rect1.InflateRect(0,Amp);
	pDoc->PPtoLP(rect1);

	int i;
	CPalette* pOldPalette;
	pOldPalette = (CPalette*)pDC->SelectPalette(&m_Palette, FALSE);
	pDC->RealizePalette();
	
	int CurveNum=m_CurveArray.GetSize ();
	int pos=0;
	CString WellName=m_strPathName;
	int xxx=m_strPathName.Find (".");
	if(xxx>-1)
		WellName=m_strPathName.Left(xxx+1);
	else
		WellName+=".";
	CRect m_CurveRect;
	COLORREF color;
	int cr,y0,y,m_num1,m_num2;
	double sdep,edep,DrawElipseDep;
	double DataDep=YCoorToDepth(m_Rect.top +(int)(r*sin(m_LookAngle*PI/180.0))+2)-m_PlotSdep;
	if(!m_Daz) DataDep=0;

/*	BOOL bDazi=TRUE;
	CFIDIO *pFIDAZI=NULL;
	pFIDAZI = new CFIDIO(m_strPathName);
		if (!pFIDAZI->Open()) 
			{delete pFIDAZI; bDazi=FALSE;}
*/	
	for(int CN=0;CN<CurveNum;CN++)
	{
		CFIDIO *pFIDIO = new CFIDIO(WellName+m_CurveArray[CN]);
		if (!pFIDIO->Open()) 
			{delete pFIDIO; continue;}
		m_Rlev = pFIDIO->GetDepthLevel();
		m_SDep = m_PlotSdep+DataDep; 
		DrawElipseDep=m_SDep;
		m_EDep = m_PlotEdep;
		if ((m_EDep < pFIDIO->GetStartDepth()) 
			|| (m_SDep > pFIDIO->GetEndDepth()))
			{delete pFIDIO; continue;}
		if (m_SDep < pFIDIO->GetStartDepth()) 
		{
			m_SDep = ((int)(pFIDIO->GetStartDepth() / m_Rlev) + 1) * m_Rlev;
			DrawElipseDep=m_SDep;
		}
		if (m_EDep > pFIDIO->GetEndDepth()) 
			m_EDep = ((int)(pFIDIO->GetEndDepth() / m_Rlev) -1 ) * m_Rlev;

		sdep = YCoorToDepth(rect1.top);
		edep = YCoorToDepth(rect1.bottom-4);
		m_EDep -=DataDep;
		if ((sdep > m_EDep) || (edep < m_SDep)) 
			{delete pFIDIO; break;}
		if (sdep < m_SDep) sdep = m_SDep;
		if (edep > m_EDep) edep = m_EDep;		
		m_num1 = (int)((sdep - m_SDep) / m_Rlev);
		m_num2 = (int)((edep - m_SDep) / m_Rlev+1);
		sdep = m_num1*m_Rlev+m_SDep;
	
		y = DepthToYCoor(sdep)-pDoc->m_WinOrg.y;
		m_CurveRect.left =rect.Width ()/2-Amp;
		m_CurveRect.right =rect.Width ()/2+Amp;

		m_CurveRect.top = y - Amp; 
		m_CurveRect.bottom = y + Amp;
		
		float Value;
		y0=y;			
		int yy;
		if(m_Daz)
		{
			pFIDIO->SeekToDepth((float)sdep);
			double hh=sin(m_LookAngle*PI/180.0);
			double WW=nps*0.5/(m_Rect.Width()*2./3.);
			while ((m_CurveRect.top < rect.bottom) && (sdep < m_EDep))
			{
				int y1 = DepthToYCoor(sdep)-pDoc->m_WinOrg.y;				
			/*	if(bDazi)
				{			
					pFIDAZI->Read(2,1,&Value);
					pFIDAZI->Next ();
					cr = (int) ((Value-m_LeftValue*1.0)/((m_RightValue-m_LeftValue)*1.0)* m_Palsize);
					if (cr > m_Palsize-1) cr = m_Palsize-1;
					color=PALETTEINDEX(cr);
					for(y=y0;y<y1+1;y++)
					{
						for (i=m_Rect.left; i<StartX; i++)
							pDC->SetPixel ( i,y,color);
						for (i=StartX+nps; i<m_Rect.right ; i++)
							pDC->SetPixel (i,y,color);
					}
				}*/
				pFIDIO->Read(2,1,&Value);
				sdep += m_Rlev;	
				pFIDIO->Next ();
				cr = (int) ((Value-m_LeftValue*1.0)/((m_RightValue-m_LeftValue)*1.0)* m_Palsize);
				if (cr > m_Palsize-1) cr = m_Palsize-1;
				color=PALETTEINDEX(cr);
				for(y=y0;y<y1+1;y++)
					for (i=0; i<nps; i++)
					{
						yy=y+(int)(sqrt(r*r-(i-r)*(i-r))*hh);
						pDC->SetPixel (StartX+i,yy,color);
					}
  				y0=y1;
				m_CurveRect.top = y- Amp;
				m_CurveRect.bottom = y + Amp;
			}
			if(m_SDep ==DrawElipseDep)
			{
				pFIDIO->SeekToDepth((float)(m_SDep+2*m_Rlev));
				pFIDIO->Read(2,1,&Value);
				cr = (int) ((Value-m_LeftValue*1.0)/((m_RightValue-m_LeftValue)*1.0)* m_Palsize);
				if (cr > m_Palsize-1) cr = m_Palsize-1;
				color=PALETTEINDEX(cr);
				CBrush Brush2(color);
				CPen Pen2;
				Pen2.CreatePen (PS_SOLID,1,color);
				CPen* OldPen2=pDC->SelectObject (&Pen2);
				CBrush* pOldBrush2 = pDC->SelectObject(&Brush2); 
				int StartY = DepthToYCoor(DrawElipseDep)-pDoc->m_WinOrg.y;
				pDC->Ellipse(StartX,StartY-(int)(r*sin(m_LookAngle*PI/180.0)),StartX+(int)(2.0*r),StartY+(int)(r*sin(m_LookAngle*PI/180.0))+2);
				CBrush Brush3;
				CTrackObj* pTrack=(CTrackObj*)this->GetParentObj ();
				if(pTrack!=NULL)
					Brush3.CreateSolidBrush (pTrack->m_TrackClientBrush .lbColor );
				else
					Brush3.CreateSolidBrush (RGB(255,255,255));
				pDC->SelectObject(&Brush3);				
				pDC->Ellipse(StartX+3,StartY-(int)(r*sin(m_LookAngle*PI/180.0))+2,StartX+(int)(2.0*r)-3,StartY+(int)(r*sin(m_LookAngle*PI/180.0)));
				pDC->SelectObject(pOldBrush2);
				pDC->SelectObject(OldPen2);
			}
		}
		else
		{
			pFIDIO->SeekToDepth((float)sdep);
			while ((m_CurveRect.top < rect.bottom) && (sdep < m_EDep))
			{
				int y1 = DepthToYCoor(sdep)-pDoc->m_WinOrg.y;
				pFIDIO->Read(2,1,&Value);
				sdep += m_Rlev;	
				pFIDIO->Next ();
				cr = (int) ((Value-m_LeftValue*1.0)/((m_RightValue-m_LeftValue)*1.0)* m_Palsize);
				if (cr > m_Palsize-1) cr = m_Palsize-1;
				color=PALETTEINDEX(cr);
				for(y=y0;y<y1+1;y++)
					for (i=0; i<nps; i++)
						pDC->SetPixel (StartX+i,y,color);
 				y0=y1;
				m_CurveRect.top = y- Amp;
				m_CurveRect.bottom = y + Amp;
			}
		}
		delete pFIDIO;
//		if(bDazi) delete pFIDAZI;
		SetClipRect(pDC,m_ClipRect);
		pos+=nps;
		if(!m_PoleArray||(m_PoleArray&&((CN%2)==1)))
			pos+=m_Space;
	}	
	pDC->SelectObject(pOldPalette);
}

void CCurve3D::OnLButtonDblClk(CView* pView, UINT nFlags, const CPoint& point)
{
	if (Properties(pView))
		Invalidate(pView);
}

void CCurve3D::CompuePlotPara()
{
	//Compute plot number perdeppoint; 
	m_PointNum=0;
	int num=m_CurveArray.GetSize ();
	CString WellName=m_strPathName;
	int xxx=m_strPathName.Find (".");
	if(xxx>-1)
		WellName=m_strPathName.Left(xxx);
	WellName+=".";
	for (int i=0; i<num; i++)
	{
		CFIDIO* pFIDIO = new CFIDIO(WellName +m_CurveArray[i]);
		if (!pFIDIO->Open()) 
			{delete pFIDIO; continue;}
		FIDCONTENT* Content = pFIDIO->GetContent(3);
		if (Content != NULL)
			m_PointNum+= Content->nps;
		delete pFIDIO;
		m_PointNum+=m_Space;
		if((m_PoleArray&&((i%2)==0)))
			m_PointNum-=m_Space;
	}
	if((!m_Daz)||(num==1))
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