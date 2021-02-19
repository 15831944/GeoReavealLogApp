/******************************************************************
*   ImageDraw.cpp - implementation of the Wf draw object class     *           
*******************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "ImageMan.h"
#include "ImageManDoc.h"
#include "FIDIO.h"
#include "styledialog.h"
#include "TrackDraw.h"
#include "ImageDraw.h"
#include "ImagePage.h"
#include "LineListPage.h"
#include "FillListPage.h"
#include "MinFontDialog.h"
#include "MainFrm.h"
#include "ExBitmap.h"

IMPLEMENT_SERIAL(CImageObj, CFIDDrawObj, 0)
        
CImageObj::CImageObj()
{
	m_DrawShape = imageobj;
	m_BkColor = 0xFF8000;
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
    
CImageObj::CImageObj(CString szName, const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDoc)
			:CFIDDrawObj(szName,rect,pParentObj,pDoc)
{
	m_DrawShape = imageobj;
	m_BkColor = 0xFF8000;
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
		"����"//"Times New Roman"			//lfFaceName
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
	if(!ar.IsStoring())
	{
		pDoc = (CImageManDoc*)ar.m_pDocument;
		ar >> m_CtrlVer;					//��ͼ����汾��
		ar >> m_ColorGrade;					//ɫ��Ҷȼ�
		ar >> m_CrMode;						
		if(m_CtrlVer>2)						
		{									
			ar >> m_LeftVal;				//�Ϳ̶�
			ar >> m_RightVal;				//�߿̶�
		}									
		else								
		{									
			ar >> m_LeftValue;				
			ar >> m_RightValue;
			m_LeftVal = m_LeftValue;
			m_RightVal = m_RightValue;
		}
		ar >> m_BkColor;                   //ä�����ɫ
		ar >> m_bColor;                    //�Ƿ�絼�ʳ���FALSEɫ�굹��
		ar >> m_Bit;                       //��ͷֱ��
		ar >> m_HorScale;                  //����̶�
		SerializeLogFont(ar,m_ScaleFont);
		if(m_CtrlVer>1)
			ar >> m_SchemeFile;            //ɫ���ļ���
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
	}	
}                        

BOOL CImageObj::Properties(CView* pView)
{
	// 1Ӣ��=100���ص� m_Bit ��λ mm
	// 1������=0.254mm
	// 1Ӣ��=100���ص� m_Bit ��λ mm
	// 1������=0.254mm
	m_HorScale=(int)(m_Bit*PI/(m_Rect.Width()*0.254));
//	m_Bit=(int)(m_Rect.Width ()*m_HorScale*0.254*0.5/PI+0.5);

//	m_HorScale=(int)(m_Bit*PI/(m_Rect.Width()*0.254)*1000);
	CString szHT;
	CPropertySheet m_Sheet("����ͼ����");
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
	if (m_Sheet.DoModal() == IDOK)
	{
		//����ͼ����  ����  1������=0.254mm
		int w = (int)(m_Bit*PI/m_HorScale/0.254);
	//	int w = (int)(m_Bit*PI*1000/m_HorScale/0.254);
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
		CreateImagePalette();
		CRect m_SaveRect = m_Rect;
		m_SaveRect.top = m_HeadRect.top;
		if (m_ParentType != 0) 
			m_SaveRect.bottom = m_Rect.bottom;
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
	//����ɫ��
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
	for(i=0; i<m_ColorGrade; i++)
	{
		rect.right = (int)(m_HeadRect.left + (i+1)*dx) + 2;
		pDC->FillSolidRect(rect,PALETTEINDEX(i));
		rect.left = rect.right;
	}
	pDC->SelectObject(pOldPalette);
	//���ɫֱ��
	pDC->MoveTo(m_Rect.left,m_HeadRect.bottom-1);
	pDC->LineTo(m_HeadRect.right,m_HeadRect.bottom-1); 

	char ss[256];
	CFont Font;
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_NameColor);
	//ɫ�����ҿ̶�
	Font.CreateFontIndirect(&m_ScaleFont);
    CFont *pOldFont=pDC->SelectObject(&Font);
	pDC->SetTextAlign(TA_LEFT | TA_TOP);
	dtos(m_LeftVal,ss);
    pDC->TextOut(m_HeadRect.left+2,rect.bottom,ss,strlen(ss));
	dtos(m_RightVal,ss);
    pDC->SetTextAlign(TA_RIGHT | TA_TOP);
    pDC->TextOut(m_HeadRect.right-2,rect.bottom,ss,strlen(ss));
	CSize size = pDC->GetTextExtent("��");
	if(m_strPathName.Find("DYNMI")>0
		||m_strPathName.Find("STAMI")>0
		||m_strPathName.Find("STAPIC")>0
		||m_strPathName.Find("DNYPIC")>0
		||m_strPathName.Find("STRMI")>0
		||m_strPathName.Find("IMAGE")>0)
	{
		//�������ҿ̶�
		pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
		dtos(0.f,ss);
		pDC->TextOut(m_HeadRect.left+2,m_HeadRect.bottom-3,ss,strlen(ss));
		dtos(360.f,ss);
		pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
		pDC->TextOut(m_HeadRect.right-2,m_HeadRect.bottom-3,ss,strlen(ss)); 
		pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
		//�������
		CString szStr;
		szStr.Format("�������(1:%d)",m_HorScale);
		pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
		pDC->TextOut((m_HeadRect.left+m_HeadRect.right)/2,m_HeadRect.bottom-3-size.cy,szStr);
		//���м�̶�
		float tmp=0.f;
		for(i=1;i<4;i++)
		{
			tmp=i*90.f;
			dtos(tmp,ss);
			pDC->TextOut(m_HeadRect.left+i*m_HeadRect.Width()/4,m_HeadRect.bottom-2,ss,strlen(ss));
			//���ɫС�̶���
			pDC->MoveTo(m_HeadRect.left+i*m_HeadRect.Width()/4,m_HeadRect.bottom-1);
			pDC->LineTo(m_HeadRect.left+i*m_HeadRect.Width()/4,m_HeadRect.bottom - 4); 
		}
		//����������
		Font.CreateFontIndirect(&m_NameFont);
		pDC->SelectObject(&Font);
		pDC->SetTextAlign(TA_CENTER | TA_TOP);
		pDC->TextOut((m_HeadRect.left + m_HeadRect.right)/2, rect.bottom, m_AliasName,strlen(m_AliasName));
	} 
	else
	{
		//�����߿̶ȵ�λ
		pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
		pDC->TextOut((m_HeadRect.left + m_HeadRect.right)/2, m_HeadRect.bottom-4, m_UnitText);
		//����������
		Font.CreateFontIndirect(&m_NameFont);
		pDC->SelectObject(&Font);
		pDC->TextOut((m_HeadRect.left + m_HeadRect.right)/2, m_HeadRect.bottom-4-size.cy, m_AliasName,strlen(m_AliasName));
	}
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
	CGradient m_Gradient;
	CString ColorFile=GetWorkPath("Color")+m_SchemeFile+".cip";
	CFile file(ColorFile, CFile::modeRead | CFile::shareDenyWrite );
	CArchive ar(&file, CArchive::load);	
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
	ar.Close();file.Close();
}

BOOL CImageObj::DrawBmp(CDC *pDC,CBitmap *bmp,CRect BmpRect)
{  
	CFIDIO *pFIDIO = new CFIDIO(m_strPathName);
	if(!pFIDIO->Open()) 
		{delete pFIDIO; return FALSE;}
	m_Rlev = pFIDIO->GetDepthLevel();
	m_SDep=pFIDIO->GetStartDepth();
	m_EDep=pFIDIO->GetEndDepth();

	double sdep= YCoorToDepth(BmpRect.top); 
	double edep= YCoorToDepth(BmpRect.bottom);
	
	if(sdep<m_SDep) sdep=m_SDep;
	if(edep>m_EDep) edep=m_EDep;
	
	bmp->CreateCompatibleBitmap(pDC,BmpRect.Width(),BmpRect.Height());

	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap =MemDC.SelectObject(bmp);
	MemDC.FillSolidRect(0,0,BmpRect.Width(),BmpRect.Height(),0xFFFFFF);	
		
	CPalette* pOldPalette;
	pOldPalette = (CPalette*)MemDC.SelectPalette(&m_Palette, FALSE);
	MemDC.RealizePalette();

	CWaitCursor wait;
	int y = DepthToYCoor(sdep)-pDoc->m_WinOrg.y;	 
	FIDCONTENT* Content = pFIDIO->GetContent(3);
	int nps = Content->nps;
	float* fwf = NULL;
	fwf = new float[nps+1];

	int Ps=m_ColorGrade-1;
	int i,x,cr,w=m_Rect.Width();
	float Dalta=m_RightVal-m_LeftVal;
	float LeftVal=m_LeftVal;
	float Grade=(float)m_ColorGrade;
	long PtNum=0;
	double dep=sdep;
	int cx=BmpRect.left-m_Rect.left;
	pDoc->LPtoPP(&BmpRect);
	int x0,y0=DepthToYCoor(dep)-pDoc->m_WinOrg.y;
	while(dep<edep)
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
				if(x<cx) continue;
				if((fwf[i]==-9999.f)||(fwf[i]==-999.25f))
				{
					x0=x;
					continue;
				}
				cr =(int)((fwf[i]-LeftVal)/Dalta*Grade);
				if(cr>Ps) cr=Ps;
				MemDC.FillSolidRect(x0-cx,y0-BmpRect.top,x-x0,y-y0,PALETTEINDEX(cr));
				x0=x;
			}
			y0=y;
		}
		PtNum++;
 		dep= sdep+((double)PtNum)*m_Rlev;
 	}
	delete pFIDIO;
	delete fwf;

	MemDC.SelectObject(pOldPalette);
	MemDC.SelectObject(pOldBitmap);
	MemDC.DeleteDC ();

	return TRUE;
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
	m_SDep = m_PlotSdep; 
	m_EDep = m_PlotEdep;
	CFIDIO *pFIDIO = new CFIDIO(m_strPathName);
	if (!pFIDIO->Open()) 
		{delete pFIDIO; return;}
	m_Rlev = pFIDIO->GetDepthLevel();
	if ((m_EDep < pFIDIO->GetStartDepth()) || (m_SDep > pFIDIO->GetEndDepth()))
		{delete pFIDIO; return;}
	if (m_SDep < pFIDIO->GetStartDepth()) 
		m_SDep = ((int)(pFIDIO->GetStartDepth() / m_Rlev) + 1) * m_Rlev;
	if (m_EDep > pFIDIO->GetEndDepth()) 
		m_EDep = ((int)(pFIDIO->GetEndDepth() / m_Rlev) -1 ) * m_Rlev;

	double sdep = YCoorToDepth(rect1.top);
	double edep = YCoorToDepth(rect1.bottom);
	if((sdep > m_EDep) || (edep < m_SDep)) 
	{
		delete pFIDIO;
		return;
	}
	if(sdep < m_SDep) sdep = m_SDep;
	if(edep > m_EDep) edep = m_EDep;

	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC,DrawRect.Width(),DrawRect.Height());
	CBitmap* pOldBitmap =MemDC.SelectObject(&bmp);

	CBrush brush;
	if (brush.CreateSolidBrush(m_BkColor))
	{
		brush.UnrealizeObject();     
		MemDC.FillRect(DrawRect, &brush);	
	}
	CPalette* pOldPalette = (CPalette*)MemDC.SelectPalette(&m_Palette, FALSE);
	MemDC.RealizePalette();

	CWaitCursor wait;
	
	int cr;
	int y = DepthToYCoor(sdep)-pDoc->m_WinOrg.y;	 
	FIDCONTENT* Content = pFIDIO->GetContent(3);
	int nps = Content->nps;
	float* fwf = new float[nps+1];

	int Ps=m_ColorGrade-1;
	int i,x,w=rect.Width();
	float Dalta=m_RightVal-m_LeftVal;
	float LeftVal=m_LeftVal;
	float Grade=(float)m_ColorGrade;
	long Num=0;
	double dep=sdep;
	int x0,y0=DepthToYCoor(dep)-pDoc->m_WinOrg.y;
	while(y<rect.bottom&&dep<edep)
	{
		y=DepthToYCoor(dep)-pDoc->m_WinOrg.y;
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
		Num++;
 		dep= sdep+Num*m_Rlev;
 	}
	delete pFIDIO; delete fwf;
	pDC->BitBlt(rect.left, m_ClipRect.top+Offset, rect.Width(),m_ClipRect.Height()-Offset,&MemDC, 0, Offset,SRCCOPY);
	MemDC.SelectObject(pOldPalette);
	MemDC.SelectObject(pOldBitmap);
	bmp.DeleteObject ();
}
