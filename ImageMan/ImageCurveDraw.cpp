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
#include "ImageCurveDraw.h"
#include "ImageCurvePage.h"
#include "LineListPage.h"
#include "FillListPage.h"
#include "MinFontDialog.h"
#include "MainFrm.h"

IMPLEMENT_SERIAL(CImageCurveObj, CFIDDrawObj, 0)
        
CImageCurveObj::CImageCurveObj()
{
	m_DrawShape = imageCurveobj;
	m_CurveColor = 0x0;
	m_FillColor=0x0;
	m_HorOffVal=0.f;
	m_LeftValue =0;
	m_RightValue = 1024;
	m_sCurveArray=_T("");
	m_HorOff=0;
	m_ScaleType=0;
	m_NoNum=NULL;
}
    
CImageCurveObj::CImageCurveObj(CString szName, const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDoc)
			:CFIDDrawObj(szName,rect,pParentObj,pDoc)
{
	m_DrawShape = imageCurveobj;
	m_CurveColor = 0x0;
	m_FillColor=0x0;
	m_HorOffVal=0.f;
	m_bGDIPlus=FALSE;
	m_LeftValue =0;
	m_RightValue = 1024;
	m_sCurveArray=_T("");
	m_HorOff=0;
	m_ScaleType=0;
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
		"宋体"//"Times New Roman"			//lfFaceName
	};
	m_ScaleFont = m_LogFont; 
	m_NoNum=NULL;
}


CImageCurveObj::~CImageCurveObj()
{
	if(m_NoNum!=NULL)
		delete m_NoNum;
}    

void CImageCurveObj::Serialize( CArchive& ar )
{
	CFIDDrawObj::Serialize( ar );
	if(!ar.IsStoring() )
	{
		pDoc = (CImageManDoc*)ar.m_pDocument;
		ar >> m_LeftValue;
		ar >> m_RightValue;	
		ar >> m_CurveColor;
		ar >> m_sCurveArray;
		ar >> m_HorOff;
		ar >> m_ScaleType;
		ar >> m_bGDIPlus;
		ar >> m_FillColor;
		ar >> m_HorOffVal;
		SerializeLogFont(ar,m_ScaleFont);
		m_GradNum= GetDrawCurveNum();
	}
}

int CImageCurveObj::GetDrawCurveNum()
{
	m_bFlush=FALSE;
	int i,j,dim,nps=0;
	CFIDIO *pFIDIO = new CFIDIO(m_strPathName);
	if(pFIDIO->Open()) 
	{
		dim=pFIDIO->GetDimenSion();
		if(dim>2)
		{
			FIDCONTENT* Content = pFIDIO->GetContent(dim);
			if(Content!= NULL)
				nps = Content->nps;
		}
	}
	delete pFIDIO;
	if(nps<2)
		return 0;
	
	if(m_NoNum!=NULL)
		delete m_NoNum;
	m_NoNum=new int[nps];
	for(i=0;i<nps;i++)
		m_NoNum[i]=-1;
	
	CString sCurveNo=m_sCurveArray;
	sCurveNo.TrimLeft ();
	sCurveNo.TrimRight ();
	
	int ps=sCurveNo.Find (",");
	CString ls;
	i=0;
	while(ps>0)
	{
		ls=sCurveNo.Left (ps);
		sCurveNo=sCurveNo.Mid(ps+1);
		if(dim<3) 
			m_NoNum[i]=0;
		else 
		{
			j=atoi(ls);
			if((j>-1)&&(j<nps)) 
				m_NoNum[i]=j;
		}
		i++;
		ps=sCurveNo.Find (",");
	}
	if(sCurveNo!="")
	{
		ps=sCurveNo.Find ("-");
		if(ps>0)
		{
			ls=sCurveNo.Left (ps);
			int k1=atoi(ls);
			if((k1>-1)&&(k1<nps))
			{
				ls=sCurveNo.Mid (ps+1);
				int k2=atoi(ls);
				if((k2>k1)&&(k2<nps))
				{
					for(j=k1;j<=k2;j++)
					{
						if(dim<3) m_NoNum[i]=0;
						else	  m_NoNum[i]=j;
						i++;
					}
				}
			}
		}
		else
		{
			if(dim<3) m_NoNum[i]=0;
			else
			{
				j=atoi(sCurveNo);
				if((j>-1)&&(j<nps)) 
					m_NoNum[i]=j;
			}
		}
	}
	int CurveNum=0;
	for(i=0;i<nps;i++)
	{
		if(m_NoNum[i]<0) continue;
		CurveNum++;
	}
	return CurveNum;
}

void CImageCurveObj::OnScaleFont()
{
	int lfEscapement = m_ScaleFont.lfEscapement;
	CFontDialog m_FontDialog((LPLOGFONT)&m_ScaleFont); 
	if(m_FontDialog.DoModal() == IDOK)
	{
		m_ScaleFont.lfEscapement = lfEscapement;
		Invalidate();
	}	
}                        

BOOL CImageCurveObj::Properties(CView* pView)
{
	CString szHT;
	CPropertySheet m_Sheet("阵列曲线属性");
	CImageCurveBase m_Base(this);
	CImageCurvePage m_Page(this);
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
	if(m_Sheet.DoModal() == IDOK)
	{
		m_FontPage.OnOK();
		m_NameColor = m_TextFont[0].m_ForeGround;
		m_NameFont = m_TextFont[0].lf;
		m_ScaleFont = m_TextFont[1].lf;
		m_NameFont.lfEscapement = 0;
		m_ScaleFont.lfEscapement = 0;
		CRect m_SaveRect = m_Rect;
		m_SaveRect.top = m_HeadRect.top;
		if (m_ParentType != 0) 
			m_SaveRect.bottom = m_Rect.bottom;
		m_GradNum= GetDrawCurveNum();
		InvalidateRect(m_SaveRect);
		return TRUE;
	}
	return FALSE;
}

void CImageCurveObj::DrawHead(CDC *pDC)
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

	CRect rect = m_HeadRect;
	char ss[10];      
	CFont m_Font;
	m_Font.CreateFontIndirect(&m_NameFont);
    CFont* pOldFont= pDC->SelectObject(&m_Font);
	pDC->SetTextColor(m_CurveColor);
    pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
	pDC->SetBkMode(TRANSPARENT);
    CSize textsize;
	if (m_UnitText == "") textsize.cy = 0;
	else textsize = pDC->GetTextExtent("文本");  
    pDC->TextOut((rect.left + rect.right)/2, rect.bottom - textsize.cy -10, m_AliasName,strlen(m_AliasName));
	m_Font.DeleteObject();
	m_Font.CreateFontIndirect(&m_ScaleFont);
	pDC->SelectObject(&m_Font);
	if(m_LeftVal!=m_RightVal)
	{
	    pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
		pDC->TextOut((rect.left + rect.right)/2, rect.bottom - 7, m_UnitText, strlen(m_UnitText));
	    pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
		dtos(m_LeftVal,ss);
	    pDC->TextOut(rect.left+2,rect.bottom-7,ss,strlen(ss));
		dtos(m_RightVal,ss);
	    pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
		pDC->TextOut(rect.right-2,rect.bottom-7,ss,strlen(ss));
	}

	rect = m_HeadRect;
	CPen pen;
	pen.CreatePen (0,1,m_CurveColor);
	CPen *OldPen=pDC->SelectObject (&pen);
	pDC->MoveTo (rect.left,rect.bottom - 5);
	pDC->LineTo (rect.right,rect.bottom - 5);

	if(m_GradNum>2&&m_CurveColor!=m_FillColor)
	{
		rect.bottom = rect.top + 30;
		rect.InflateRect(-2,-2);
		pDC->SetTextAlign(TA_LEFT|TA_TOP);
		int y=rect.bottom;
		int x0=m_HeadRect.left+2;
		int sN=m_sCurveArray.GetLength();
		int i=0,x=x0;
		CString str;
		while(i<sN)
		{
			str=m_sCurveArray.GetAt(i);
			pDC->TextOut(x,y,str);
			textsize = pDC->GetTextExtent(str);
			x+=textsize.cx;
			if(x>m_HeadRect.right-2)
			{
				x=x0;
				y+=textsize.cy;
			}
			i++;
		}
		double dx;
		dx = (m_HeadRect.Width()-4.0)/m_GradNum;
		rect.left = m_HeadRect.left + 2;
		int peRed,peGreen,peBlue;
		double fRed =  (GetRValue(m_CurveColor) - GetRValue(m_FillColor))/(m_GradNum-1.0);
		double fGreen = (GetGValue(m_CurveColor) - GetGValue(m_FillColor))/(m_GradNum-1.0);
		double fBlue =  (GetBValue(m_CurveColor) - GetBValue(m_FillColor))/(m_GradNum-1.0);		
		for(i=0;i<m_GradNum;i++)
		{
			rect.right =m_HeadRect.left+(int)((i+1)*dx)+2;

			peRed = GetRValue(m_FillColor)+(int)(i*fRed);
			peGreen = GetGValue(m_FillColor)+(int)(i*fGreen);
			peBlue =  GetBValue(m_FillColor)+(int)(i*fBlue);
			pDC->FillSolidRect(rect,RGB(peRed,peGreen,peBlue));
			rect.left = rect.right;
		}
	}
	pDC->SelectObject (OldPen);
	pDC->SelectObject(pOldFont);
	m_HeadRect = saveHeadRect;
}

void CImageCurveObj::SetClipRect(CDC* pDC , CRect rect)
{
	if (pDC->m_hDC != NULL) 
		::LPtoDP(pDC->m_hDC,(LPPOINT)(LPRECT)rect,2);
	CRgn rgn; 
	rgn.CreateRectRgn(rect.left,rect.top,rect.right,rect.bottom);
	pDC->SelectClipRgn(&rgn);
}

void CImageCurveObj::DrawClient(CDC *pDC)
{     
	if(m_bGDIPlus)
		DrawClientGDIPlus(pDC);
	else
		DrawClientGDI(pDC);
}

void CImageCurveObj::DrawClientGDI(CDC *pDC)
{     
	CRect rect = m_Rect;
	pDoc->LPtoPP(rect);
	if(!pDC->RectVisible(rect)) return;
	
	if ((m_pParentObj == NULL) || (m_ParentType != 0)) 
		CFIDDrawObj::DrawClient(pDC);
	if(m_bFlush) 
	{
		m_GradNum= GetDrawCurveNum();
		DrawHead(pDC);
	}
	if(m_GradNum<1) return;
	CRect rect1 = rect;
	CRect m_ClipRect;
	pDC->GetClipBox(m_ClipRect);
	rect1 &= m_ClipRect;
	pDoc->PPtoLP(rect1);
		
	CWaitCursor wait;
	double sdep,edep;
	CFIDIO *pFIDIO = new CFIDIO(m_strPathName);
	if(!pFIDIO->Open()) 
		{delete pFIDIO; return;}
	int dim=pFIDIO->GetDimenSion();
	m_Rlev = pFIDIO->GetDepthLevel();
	m_SDep = m_PlotSdep; 
	m_EDep = m_PlotEdep;
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

	FIDCONTENT* Content = pFIDIO->GetContent(dim);
	if (Content == NULL)
		{delete pFIDIO; return;}
	int nps = Content->nps;
	short* ibuf = NULL;
	float* fbuf = NULL;
	char* cbuf = NULL;
	fbuf = new float[nps];
	if(Content->type == REPR_FLOAT) 
		;
	else if (Content->type == REPR_CHAR) 
		cbuf = new char[nps];
	else ibuf = new short[nps];

	int i,j;
	int y;double dep;
	CPen pen;
	pen.CreatePen (0,1,m_CurveColor);
	CPen *OldPen=pDC->SelectObject (&pen);

	float m_LeftVal1=m_LeftVal;
	float m_RightVal1=m_RightVal;
	if(m_ScaleType == 1)
	{
		m_LeftVal1 = (float)log10(m_LeftVal);
		m_RightVal1 = (float)log10(m_RightVal);
	}
	float Dalta=m_RightVal1-m_LeftVal1;	
	CPoint *Pt;
	Pt=new CPoint[m_GradNum+1];
	CPoint *Pt0;	
	Pt0=new CPoint[m_GradNum+1];
	float Value;
	dep=sdep;
//  一个一个深度点绘制，节约读盘时间增加绘曲线时间
//  计算该屏第一个深度点
	y=DepthToYCoor(sdep)-pDoc->m_WinOrg.y;
	pFIDIO->SeekToDepth((float)dep);
	if (Content->type == REPR_FLOAT)
		pFIDIO->Read(dim,nps,fbuf);
	else if (Content->type == REPR_CHAR)
	{
		pFIDIO->Read(dim,nps,cbuf);
		for(j=0;j<nps;j++)
			fbuf[j]=(float)cbuf[j];
	}
	else 
	{
		pFIDIO->Read(dim,nps,ibuf);
		for(j=0;j<nps;j++)
			fbuf[j]=(float)ibuf[j];
	}
	int n=0;
	int XOffset=rect.left ;
	for(i=0;i<nps;i++)
	{		
		if(m_NoNum[i]<0) continue;
		if(m_NoNum[i]>nps-1) continue;
		if(fbuf[m_NoNum[i]]==-9999.f) 
			fbuf[m_NoNum[i]]=m_LeftVal;
		if (m_ScaleType == 1)
			Value=(float)log10(fbuf[m_NoNum[i]]+m_HorOffVal*i);
		else
			Value=fbuf[m_NoNum[i]]+m_HorOffVal*i;
		Pt0[n].x=XOffset+(int)(rect.Width ()*(Value-m_LeftVal1)/Dalta);
		Pt0[n].y=y;
		XOffset+=m_HorOff;
		n++;		
	}
 	dep+= m_Rlev;
	y = DepthToYCoor(dep)-pDoc->m_WinOrg.y;
	int PtNum=1;

//  计算该屏所有深度点				
	if(m_GradNum<2||m_CurveColor==m_FillColor)
	{
		while((y< rect.bottom) && (dep < edep))
		{
			pFIDIO->SeekToDepth((float)dep);
			if (Content->type == REPR_FLOAT)
				pFIDIO->Read(dim,nps,fbuf);
			else if (Content->type == REPR_CHAR)
			{
				pFIDIO->Read(dim,nps,cbuf);
				for(j=0;j<nps;j++)
					fbuf[j]=(float)cbuf[j];
			}
			else 
			{
				pFIDIO->Read(dim,nps,ibuf);
				for(j=0;j<nps;j++)
					fbuf[j]=(float)ibuf[j];
			}
			int n=0;
			XOffset=rect.left;
			for(i=0;i<nps;i++)
			{		
				if(m_NoNum[i]<0) continue;
				if(m_NoNum[i]>nps-1) continue;
				if(fbuf[m_NoNum[i]]==-9999.f) 
					fbuf[m_NoNum[i]]=m_LeftVal;
				if(m_ScaleType == 1)
				{
					if(fbuf[m_NoNum[i]]<0.0001f) 
						fbuf[m_NoNum[i]]=0.0001f;
					Value=(float)log10(fbuf[m_NoNum[i]]+m_HorOffVal*i);
				}
				else
					Value=fbuf[m_NoNum[i]]+m_HorOffVal*i;
				Pt[n].x=XOffset+(int)(rect.Width ()*(Value-m_LeftVal1)/Dalta);
				Pt[n].y=y;
				XOffset+=m_HorOff;
				n++;		
			}
			for(n=0;n<m_GradNum;n++)
			{
				pDC->MoveTo(Pt0[n]);
				pDC->LineTo(Pt[n]);
				Pt0[n]=Pt[n];
			}
			PtNum++;
 			dep= sdep+PtNum*m_Rlev;
			y = DepthToYCoor(dep)-pDoc->m_WinOrg.y;
		}
	}
	else   
	{
		int peRed,peGreen,peBlue;
		double fRed =  (GetRValue(m_CurveColor) - GetRValue(m_FillColor))/(m_GradNum-1.0);
		double fGreen = (GetGValue(m_CurveColor) - GetGValue(m_FillColor))/(m_GradNum-1.0);
		double fBlue =  (GetBValue(m_CurveColor) - GetBValue(m_FillColor))/(m_GradNum-1.0);	
		while((y< rect.bottom) && (dep < edep))
		{
			pFIDIO->SeekToDepth((float)dep);
			if (Content->type == REPR_FLOAT)
				pFIDIO->Read(dim,nps,fbuf);
			else if (Content->type == REPR_CHAR)
			{
				pFIDIO->Read(dim,nps,cbuf);
				for(j=0;j<nps;j++)
					fbuf[j]=(float)cbuf[j];
			}
			else 
			{
				pFIDIO->Read(dim,nps,ibuf);
				for(j=0;j<nps;j++)
					fbuf[j]=(float)ibuf[j];
			}
			int n=0;
			XOffset=rect.left;
			for(i=0;i<nps;i++)
			{		
				if(m_NoNum[i]<0) continue;
				if(m_NoNum[i]>nps-1) continue;
				if(fbuf[m_NoNum[i]]==-9999.f) 
					fbuf[m_NoNum[i]]=m_LeftVal;
				if (m_ScaleType == 1)
				{
					if(fbuf[m_NoNum[i]]<0.0001f) 
						fbuf[m_NoNum[i]]=0.0001f;
					Value=(float)log10(fbuf[m_NoNum[i]]+m_HorOffVal*i);
				}
				else
					Value=fbuf[m_NoNum[i]]+m_HorOffVal*i;
				Pt[n].x=XOffset+(int)(rect.Width ()*(Value-m_LeftVal1)/Dalta);
				Pt[n].y=y;
				XOffset+=m_HorOff;
				n++;		
			}
			for(n=0;n<m_GradNum;n++)
			{
				pDC->MoveTo(Pt0[n]);
				CPen ppen;
				peRed = GetRValue(m_FillColor)+(int)(n*fRed);
				peGreen = GetGValue(m_FillColor)+(int)(n*fGreen);
				peBlue =  GetBValue(m_FillColor)+(int)(n*fBlue);
				ppen.CreatePen (0,1,RGB(peRed,peGreen,peBlue));
				pDC->SelectObject(&ppen);
				pDC->LineTo(Pt[n]);
				Pt0[n]=Pt[n];
				ppen.DeleteObject();
			}
 			PtNum++;
 			dep= sdep+PtNum*m_Rlev;
			y = DepthToYCoor(dep)-pDoc->m_WinOrg.y;
		}
	}
	pDC->SelectObject (OldPen);
	delete pFIDIO;
	delete Pt0;
	delete Pt;
	delete fbuf;
	if(cbuf != NULL)	delete cbuf;
	if(ibuf != NULL)	delete ibuf;		
}

void CImageCurveObj::DrawClientGDIPlus(CDC *pDC)
{     
	CRect rect = m_Rect;
	pDoc->LPtoPP(rect);
	if(!pDC->RectVisible(rect)) return;
	
	if ((m_pParentObj == NULL) || (m_ParentType != 0)) 
		CFIDDrawObj::DrawClient(pDC);
	if(m_bFlush) 
	{
		m_GradNum= GetDrawCurveNum();
		DrawHead(pDC);
	}
	if(m_GradNum<1) return;
	CRect rect1 = rect;
	CRect m_ClipRect;
	pDC->GetClipBox(m_ClipRect);
	rect1 &= m_ClipRect;
	pDoc->PPtoLP(rect1);
		
	CWaitCursor wait;
	double sdep,edep;
	CFIDIO *pFIDIO = new CFIDIO(m_strPathName);
	if(!pFIDIO->Open()) 
		{delete pFIDIO; return;}
	int dim=pFIDIO->GetDimenSion();
	m_Rlev = pFIDIO->GetDepthLevel();
	m_SDep = m_PlotSdep; 
	m_EDep = m_PlotEdep;
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

	FIDCONTENT* Content = pFIDIO->GetContent(dim);
	if (Content == NULL)
		{delete pFIDIO; return;}
	int nps = Content->nps;
	short* ibuf = NULL;
	float* fbuf = NULL;
	char* cbuf = NULL;
	fbuf = new float[nps];
	if(Content->type == REPR_FLOAT) 
		;
	else if (Content->type == REPR_CHAR) 
		cbuf = new char[nps];
	else ibuf = new short[nps];

	int i,j;
	int y;double dep;
	float m_LeftVal1=m_LeftVal;
	float m_RightVal1=m_RightVal;
	if(m_ScaleType == 1)
	{
		m_LeftVal1 = (float)log10(m_LeftVal);
		m_RightVal1 = (float)log10(m_RightVal);
	}
	float Dalta=m_RightVal1-m_LeftVal1;	

	CPoint *Pt;
	Pt=new CPoint[m_GradNum+1];
	CPoint *Pt0;	
	Pt0=new CPoint[m_GradNum+1];
	float Value;
	dep=sdep;
//  一个一个深度点绘制，节约读盘时间增加绘曲线时间
//  计算该屏第一个深度点
	y=DepthToYCoor(sdep)-pDoc->m_WinOrg.y;
	pFIDIO->SeekToDepth((float)dep);
	if (Content->type == REPR_FLOAT)
		pFIDIO->Read(dim,nps,fbuf);
	else if (Content->type == REPR_CHAR)
	{
		pFIDIO->Read(dim,nps,cbuf);
		for(j=0;j<nps;j++)
			fbuf[j]=(float)cbuf[j];
	}
	else 
	{
		pFIDIO->Read(dim,nps,ibuf);
		for(j=0;j<nps;j++)
			fbuf[j]=(float)ibuf[j];
	}
	int n=0;
	int XOffset=rect.left ;
	for(i=0;i<nps;i++)
	{		
		if(m_NoNum[i]<0) continue;
		if(m_NoNum[i]>nps-1) continue;
		if(fbuf[m_NoNum[i]]==-9999.f) 
			fbuf[m_NoNum[i]]=m_LeftVal;
		if (m_ScaleType == 1)
			Value=(float)log10(fbuf[m_NoNum[i]])+m_HorOffVal*i;
		else
			Value=fbuf[m_NoNum[i]]+m_HorOffVal*i;

		Pt0[n].x=XOffset+(int)(rect.Width ()*(Value-m_LeftVal1)/Dalta);
		Pt0[n].y=y;
		XOffset+=m_HorOff;
		n++;		
	}
 	dep+= m_Rlev;
	y = DepthToYCoor(dep)-pDoc->m_WinOrg.y;
	int PtNum=1;

//  计算该屏所有深度点
	Graphics *pic=NULL;
	pic=new Graphics(pDC->m_hDC);
	pic->SetSmoothingMode(SmoothingModeHighQuality);
					
	if(m_GradNum<2||m_CurveColor==m_FillColor)
	{
		while((y< rect.bottom) && (dep < edep))
		{
			pFIDIO->SeekToDepth((float)dep);
			if (Content->type == REPR_FLOAT)
				pFIDIO->Read(dim,nps,fbuf);
			else if (Content->type == REPR_CHAR)
			{
				pFIDIO->Read(dim,nps,cbuf);
				for(j=0;j<nps;j++)
					fbuf[j]=(float)cbuf[j];
			}
			else 
			{
				pFIDIO->Read(dim,nps,ibuf);
				for(j=0;j<nps;j++)
					fbuf[j]=(float)ibuf[j];
			}
			int n=0;
			XOffset=rect.left;
			for(i=0;i<nps;i++)
			{		
				if(m_NoNum[i]<0) continue;
				if(m_NoNum[i]>nps-1) continue;
				if(fbuf[m_NoNum[i]]==-9999.f) 
					fbuf[m_NoNum[i]]=m_LeftVal;
				if(m_ScaleType == 1)
				{
					if(fbuf[m_NoNum[i]]<0.0001f) 
						fbuf[m_NoNum[i]]=0.0001f;
					Value=(float)log10(fbuf[m_NoNum[i]]+m_HorOffVal*i);
				}
				else
					Value=fbuf[m_NoNum[i]]+m_HorOffVal*i;
				Pt[n].x=XOffset+(int)(rect.Width ()*(Value-m_LeftVal1)/Dalta);
				Pt[n].y=y;
				XOffset+=m_HorOff;
				n++;		
			}
			for(n=0;n<m_GradNum;n++)
			{
				Pen *pPen=NULL;
				Color CurveColor(GetRValue(m_CurveColor),GetGValue(m_CurveColor),GetBValue(m_CurveColor));
				pPen = new Pen(CurveColor,1.f);
				pic->DrawLine(pPen,Pt0[n].x,Pt0[n].y,Pt[n].x,Pt[n].y);
				delete pPen;
				Pt0[n]=Pt[n];
			}
			PtNum++;
 			dep= sdep+PtNum*m_Rlev;
			y = DepthToYCoor(dep)-pDoc->m_WinOrg.y;
		}
	}
	else   
	{
		int peRed,peGreen,peBlue;
		double fRed =  (GetRValue(m_CurveColor) - GetRValue(m_FillColor))/(m_GradNum-1.0);
		double fGreen = (GetGValue(m_CurveColor) - GetGValue(m_FillColor))/(m_GradNum-1.0);
		double fBlue =  (GetBValue(m_CurveColor) - GetBValue(m_FillColor))/(m_GradNum-1.0);	
		while((y< rect.bottom) && (dep < edep))
		{
			pFIDIO->SeekToDepth((float)dep);
			if (Content->type == REPR_FLOAT)
				pFIDIO->Read(dim,nps,fbuf);
			else if (Content->type == REPR_CHAR)
			{
				pFIDIO->Read(dim,nps,cbuf);
				for(j=0;j<nps;j++)
					fbuf[j]=(float)cbuf[j];
			}
			else 
			{
				pFIDIO->Read(dim,nps,ibuf);
				for(j=0;j<nps;j++)
					fbuf[j]=(float)ibuf[j];
			}
			int n=0;
			XOffset=rect.left;
			for(i=0;i<nps;i++)
			{		
				if(m_NoNum[i]<0) continue;
				if(m_NoNum[i]>nps-1) continue;
				if(fbuf[m_NoNum[i]]==-9999.f) 
					fbuf[m_NoNum[i]]=m_LeftVal;
				if (m_ScaleType == 1)
				{
					if(fbuf[m_NoNum[i]]<0.0001f) 
						fbuf[m_NoNum[i]]=0.0001f;
					Value=(float)log10(fbuf[m_NoNum[i]]+m_HorOffVal*i);
				}
				else
					Value=fbuf[m_NoNum[i]]+m_HorOffVal*i;
				Pt[n].x=XOffset+(int)(rect.Width ()*(Value-m_LeftVal1)/Dalta);
				Pt[n].y=y;
				XOffset+=m_HorOff;
				n++;		
			}
			for(n=0;n<m_GradNum;n++)
			{
				peRed = GetRValue(m_FillColor)+(int)(n*fRed);
				peGreen = GetGValue(m_FillColor)+(int)(n*fGreen);
				peBlue =  GetBValue(m_FillColor)+(int)(n*fBlue);
				Color CurveColor(peRed,peGreen,peBlue);	
				Pen *pPen=NULL;
				pPen = new Pen(CurveColor,1.f);
				pic->DrawLine(pPen,Pt0[n].x,Pt0[n].y,Pt[n].x,Pt[n].y);
				delete pPen;
				Pt0[n]=Pt[n];
			}
 			PtNum++;
 			dep= sdep+PtNum*m_Rlev;
			y = DepthToYCoor(dep)-pDoc->m_WinOrg.y;
		}
	}
	pic->ReleaseHDC(pDC->m_hDC);
	delete pic;	
	delete pFIDIO;
	delete Pt0;
	delete Pt;
	delete fbuf;
	if(cbuf != NULL)	delete cbuf;
	if(ibuf != NULL)	delete ibuf;		
}

int CImageCurveObj::HitTest(CView* pView,const CPoint& point)
{
	int nHandle = CFIDDrawObj::HitTest(pView,point); 
	if ((nHandle >= 0) || (!m_Rect.PtInRect(point))) return nHandle;
	if(m_Rect.PtInRect(point)) 
		return 9;
	else return -1;
}