/*************************************************************************
*   DangDdirDraw.cpp - implementation of the DangDDir draw object class  *           
*************************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "ImageMan.h"
#include "ImageManDoc.h"
#include "TrackDraw.h"
#include "FIDIO.h"
#include "MarkVectorDraw.h"
#include "Grid.h"
#include "GridDialog.h"
#include "MarkVectorPage.h"
#include "LineListPage.h"
#include "FillListPage.h"
#include "MinFontDialog.h"
#include "HandWorkPage.h"

CVectorPoint::CVectorPoint(CPoint Point,float Dep,float Dip,float Dir,
						   CString Flag,COLORREF Color,BOOL Origin)
{
	m_Dep=Dep;
	m_Dip=Dip;
	m_Dir=Dir;
	m_Flag=Flag;
	m_Point = Point;
	m_Color=Color;
	m_Origin=Origin;
}

void CVectorPoint::Draw(CDC* pDC,CImageManDoc* pDoc)
{
	CRect m_Rect0 = m_Rect;
	pDoc->LPtoPP(m_Rect0);
	CRect rect,m_ClipRect;
	pDC->GetClipBox(m_ClipRect);
	if (!pDC->RectVisible(m_Rect0)) return; 
		
	int x,y,x1,y1,x2,y2;
	CPen m_Pen1;
	m_Pen1.CreatePen(0,m_TailWidth,m_Color);
	CPen* pOldPen = pDC->SelectObject(&m_Pen1); 
	CBrush m_Brush(m_Color);
	CBrush* pOldBrush = pDC->SelectObject(&m_Brush);  
	if(m_Origin)
		 pDC->SelectStockObject(NULL_BRUSH);

	int len = m_TailLength + m_CircleRadius;
	double dtor =  PI / 180;

	CPoint pt;
	CString para;
	CFont hTextFont;
    hTextFont.CreateFont(
		-12,					    //lfHeight
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

	pt=m_Point;
	pDoc->LPtoPP(&pt);
	x = pt.x; y = pt.y;
	x1 = x - m_CircleRadius; x2 = x + m_CircleRadius;
    y1 = y - m_CircleRadius; y2 = y + m_CircleRadius;
	double dd = m_Dir* dtor;
	pDC->Ellipse(x1,y1,x2,y2);  
	x2 = x + Round(len * sin(dd));
	y2 = y - Round(len * cos(dd));
	pDC->MoveTo(pt); pDC->LineTo(x2,y2);
	if(m_PrintPara)
	{
		para.Format (m_ParaFormat,m_Dip,m_Dir);
		pDC->TextOut (x,y+m_CircleRadius,para);
	}
	pDC->SelectObject(pOldFont);
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}

IMPLEMENT_SERIAL(CMarkVectorObj, CFIDDrawObj, 0)
        
CMarkVectorObj::CMarkVectorObj()
{
	m_DrawShape = markvectorobj;
	m_MinGrade = (float)11.1;
	m_bFlush = TRUE;
	m_HeadStyle=0;
	m_bPrintOriginPara=FALSE;
	m_bPrintTruePara=TRUE;
	m_bPrintTrueDip=TRUE;
	m_bPrintOriginDip=TRUE;

	m_ParaFormat="%.f %.f";
	m_DirName ="HandMark.List";
}
    
CMarkVectorObj::CMarkVectorObj(CString szPathName, const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDoc)
			:CFIDDrawObj(szPathName,rect,pParentObj,pDoc)
{
	m_CircleRadius = 5; m_CircleWidth = 1; m_CircleColor = RGB(0,0x80,0);
	m_TailLength = 15;  m_TailWidth = 1; m_TailColor = RGB(0,0x80,0);
	m_HeadStyle=0;
	m_bPrintOriginPara=FALSE;
	m_bPrintTruePara=TRUE;
	m_bPrintTrueDip=TRUE;
	m_bPrintOriginDip=TRUE;
	m_ParaFormat="%.f/%.f";
	
	m_PlotContent="≤„¿Ì≥‰ÃÓ∑Ï’≈ø™∑Ï∞Î≥‰ÃÓ∑Ï≤„º‰∑Ï";
	m_MinGrade = (float)11.1;                       
	m_LeftVal = 0.f; m_RightVal = 90.f;
	m_DrawShape = markvectorobj;
	m_AliasName = "Ωªª•Ω‚ Õ ∏¡øÕº";
	m_pGrid = new CGridObj(m_Rect,m_PlotSdep,m_PlotEdep,DIPGRID);
	m_pGrid->SetDepC(pDoc->m_DepC);
	m_ScaleFont = m_NameFont; 
	m_NameFont.lfHeight = -20;
	m_ScaleFont.lfHeight = -10;
	m_ScaleFont.lfWeight = 300;
	m_Rect.right = m_HeadRect.right = m_Rect.left + 150;
	m_DepLevel=1;
	m_DirName ="HandMark.List";
	m_bFlush = TRUE;
}

CMarkVectorObj::~CMarkVectorObj()
{
	POSITION pos = m_VectorPoint.GetHeadPosition();
	while (pos != NULL) 
		delete m_VectorPoint.GetNext(pos);
	m_VectorPoint.RemoveAll();
	if (m_pGrid != NULL) delete m_pGrid;
}    

void CMarkVectorObj::Serialize( CArchive& ar )
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
 		ar >> m_bPrintOriginPara;
		ar >> m_bPrintTruePara;
		ar >> m_ParaFormat;
		ar >> m_bPrintTrueDip;
		ar >> m_bPrintOriginDip;

		ar >> m_DirName;
		ar >> m_PlotContent;
		ar >> m_MinGrade;

		SerializeLogFont(ar,m_ScaleFont);
		m_pGrid = new CGridObj();
		m_pGrid->Serialize(ar);
		m_pGrid->SetDepC(pDoc->m_DepC);
	}
}

void CMarkVectorObj::SetRect(const CRect& rect)
{
	if (m_Rect == rect) return;
	CFIDDrawObj::SetRect(rect);
	m_pGrid->SetRect(rect);
	m_bFlush=TRUE;
}                    

void CMarkVectorObj::OnChangeDepC(double depc, int flag)
{
	CFIDDrawObj::OnChangeDepC(depc,flag);
	m_pGrid->SetDepC(pDoc->m_DepC);
}
               
void CMarkVectorObj::SetDepth(double sdep,double edep)
{
	CFIDDrawObj::SetDepth(sdep,edep);
	m_pGrid->SetDepth(sdep,edep);
}

void CMarkVectorObj::AddPoint(float Dep,float Dip,float Dir,
							  float Dip0,float Dir0,int Flag)
{
	CPoint point;CRect rect;
	CString Title[6]={"≤„¿Ì","≥‰ÃÓ∑Ï","’≈ø™∑Ï","—π¡—∑Ï","∞Î≥‰ÃÓ∑Ï","≤„º‰∑Ï"};
	COLORREF Color[6]={0x009300,0x00FF00,0xFF0000,0x800080,0xFF8000,0xFFFF00};
	int k=Flag/10-1;
	int Height=m_CircleRadius+m_TailLength/2+1;
	point.y = DepthToYCoor(Dep);
	rect.top=point.y-Height;
	rect.bottom=point.y+Height;
	int i;
	if(m_bPrintOriginDip)
	{
		i=0;
		while (Dip > m_pGrid->m_AngX[i+1]) i++;
		point.x = (int)(abs(m_pGrid->m_ZerX[i]) 
			+ (abs(m_pGrid->m_ZerX[i+1]) - abs(m_pGrid->m_ZerX[i])) 
			* (Dip - m_pGrid->m_AngX[i]) / (m_pGrid->m_AngX[i+1] - m_pGrid->m_AngX[i]));   
		rect.left=point.x-Height;
		rect.right=point.x+Height;
		
		CVectorPoint* pObj=new CVectorPoint(point,Dep,Dip,Dir,Title[k],Color[k],TRUE);
		pObj->m_TailLength=m_TailLength;
		pObj->m_TailWidth=m_TailWidth;
		pObj->m_CircleRadius=m_CircleRadius;
		pObj->m_PrintPara=m_bPrintOriginPara;
		pObj->m_ParaFormat=m_ParaFormat;
		pObj->m_Rect=rect;
		m_VectorPoint.AddTail(pObj);
	}

	if(m_bPrintTrueDip)
	{
		i=0;
		while (Dip0 > m_pGrid->m_AngX[i+1]) i++;
		point.x = (int)(abs(m_pGrid->m_ZerX[i]) 
			+ (abs(m_pGrid->m_ZerX[i+1]) - abs(m_pGrid->m_ZerX[i])) 
			* (Dip0 - m_pGrid->m_AngX[i]) / (m_pGrid->m_AngX[i+1] - m_pGrid->m_AngX[i]));   
		rect.left=point.x-Height;
		rect.right=point.x+Height;
		CVectorPoint* pObj1=new CVectorPoint(point,Dep,Dip0,Dir0,Title[k],Color[k],FALSE);
		pObj1->m_TailLength=m_TailLength;
		pObj1->m_TailWidth=m_TailWidth;
		pObj1->m_CircleRadius=m_CircleRadius;
		pObj1->m_PrintPara=m_bPrintTruePara;
		pObj1->m_ParaFormat=m_ParaFormat;
		pObj1->m_Rect=rect;
		m_VectorPoint.AddTail(pObj1);
	}
}

void CMarkVectorObj::CreatePoint()
{
	CRect rect=m_Rect;
	POSITION pos = m_VectorPoint.GetHeadPosition();
	while (pos != NULL) 
		delete m_VectorPoint.GetNext(pos);
	m_VectorPoint.RemoveAll();
	if(DetectFileExist(m_strPathName))
	{
		m_bFlush=FALSE;
		CPoint point;
		float Dep,Dip,Dir,Dip0,Dir0;
		CString Flag;COLORREF Color;
		int Height=m_CircleRadius+m_TailLength/2+1;
		TCHAR *token, *end;
		TCHAR buffer[1024];
		CStdioFile File;
		CFileException ex;
		if (!File.Open(m_strPathName, CFile::modeRead | CFile::typeText)) 
			return;
		try 
		{
			File.ReadString(buffer, 1024);	// Read Header off file	
			while (File.ReadString(buffer, 1024)) 
			{
				// Get first token
				for (token=buffer, end=buffer; 
					 *end && (*end != _T(',')) && (*end != _T('\n')); end++);
				if ((*end == _T('\0')) && (token == end)) token = NULL;
				*end = _T('\0');
				
				int i=0;CString str;
				while (token) 
				{
					str=token;
					while(str.Left(1)==" ")
						str.TrimLeft (" ");
					while(str.Right(1)==" ")
						str.TrimRight (" ");
					if(i==1)
						Dep=(float)atof(str);
					else if(i==2)
						Dip=(float)atof(str);
					else if(i==3)
						Dir=(float)atof(str);
					else if(i==4)
						Flag=str;
					else if(i==5)
						Dip0=(float)atof(str);
					else if(i==6)
						Dir0=(float)atof(str);
					// Get next token
					for (token=++end; *end && (*end != _T(',')) && (*end != _T('\n'));
						 end++);
					if ((*end == _T('\0')) && (token == end)) token = NULL;
					*end = _T('\0');
					i++;
				}
				if(Dep<m_PlotSdep)
					continue;
				if(Dep>m_PlotEdep)
					continue;
				if ((Dip > m_RightVal) || (Dip < m_LeftVal))
					continue;
				if(Dip>0.005&&m_PlotContent.Find(Flag)>-1)
				{	
					point.y = DepthToYCoor(Dep);
					rect.top=point.y-Height;
					rect.bottom=point.y+Height;
					if(Flag=="≤„¿Ì")     //¬Ã…´--≤„¿Ì
						Color=0x009300;
					else if(Flag=="≥‰ÃÓ∑Ï")//«‡¿∂…´--≥‰ÃÓ∑Ï
						Color=0x00FF00;
					else if(Flag=="’≈ø™∑Ï")//¿∂…´--’≈ø™∑Ï
						Color=0xFF0000;
					else if(Flag=="∞Î≥‰ÃÓ∑Ï")//ÃÏ¿∂…´--∞Î≥‰ÃÓ∑Ï
						Color=0xFF8000;
					else if(Flag=="≤„º‰∑Ï")//«‡…´--≤„º‰∑Ï
						Color=0xFFFF00;
					else               //◊œ…´--—π¡—∑Ï
						Color=0x800080;
					int i;
					if(m_bPrintOriginDip)
					{
						i=0;
						while (Dip > m_pGrid->m_AngX[i+1]) i++;
						point.x = (int)(abs(m_pGrid->m_ZerX[i]) 
							+ (abs(m_pGrid->m_ZerX[i+1]) - abs(m_pGrid->m_ZerX[i])) 
							* (Dip - m_pGrid->m_AngX[i]) / (m_pGrid->m_AngX[i+1] - m_pGrid->m_AngX[i]));   
						rect.left=point.x-Height;
						rect.right=point.x+Height;
						
						CVectorPoint* pObj=new CVectorPoint(point,Dep,Dip,Dir,Flag,Color,TRUE);
						pObj->m_TailLength=m_TailLength;
						pObj->m_TailWidth=m_TailWidth;
						pObj->m_CircleRadius=m_CircleRadius;
						pObj->m_PrintPara=m_bPrintOriginPara;
						pObj->m_ParaFormat=m_ParaFormat;
						pObj->m_Rect=rect;
						m_VectorPoint.AddTail(pObj);
					}
			
					if(m_bPrintTrueDip)
					{
						i=0;
						while (Dip0 > m_pGrid->m_AngX[i+1]) i++;
						point.x = (int)(abs(m_pGrid->m_ZerX[i]) 
							+ (abs(m_pGrid->m_ZerX[i+1]) - abs(m_pGrid->m_ZerX[i])) 
							* (Dip0 - m_pGrid->m_AngX[i]) / (m_pGrid->m_AngX[i+1] - m_pGrid->m_AngX[i]));   
						rect.left=point.x-Height;
						rect.right=point.x+Height;
						CVectorPoint* pObj1=new CVectorPoint(point,Dep,Dip0,Dir0,Flag,Color,FALSE);
						pObj1->m_TailLength=m_TailLength;
						pObj1->m_TailWidth=m_TailWidth;
						pObj1->m_CircleRadius=m_CircleRadius;
						pObj1->m_PrintPara=m_bPrintTruePara;
						pObj1->m_ParaFormat=m_ParaFormat;
						pObj1->m_Rect=rect;
						m_VectorPoint.AddTail(pObj1);
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
}

void CMarkVectorObj::DrawVector(CDC* pDC)
{  
	if(m_bFlush) CreatePoint();
	CVectorPoint* pObj;
	POSITION pos = m_VectorPoint.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CVectorPoint*) m_VectorPoint.GetNext(pos);
		CRect rect = pObj->m_Rect;
		pDoc->LPtoPP(&rect);
		if (pDC->RectVisible(rect))
			pObj->Draw(pDC,pDoc);
	}
}

void CMarkVectorObj::UpdatePoint()
{
	m_bFlush=TRUE;
	CRect SaveRect = m_Rect;
	SaveRect.top = m_HeadRect.top;
	InvalidateRect(SaveRect);
}

void CMarkVectorObj::DrawClient(CDC* pDC)
{        
	if ((m_pParentObj == NULL) || (m_ParentType != 0))
		CFIDDrawObj::DrawClient(pDC);
	DrawVector(pDC);
}

void CMarkVectorObj::DrawHead(CDC* pDC)
{
	CString Title[6]={"≤„¿Ì","≥‰ÃÓ∑Ï","’≈ø™∑Ï","—π¡—∑Ï","∞Î≥‰ÃÓ∑Ï","≤„º‰∑Ï"};
	COLORREF Color[6]={0x009300,0xD0E040,0xFF0000,0x800080,0xFF8000,0xFFFF00};
	CRect rect = m_HeadRect;
	pDoc->LPtoPP(rect);
	if (pDC->RectVisible(rect))
	{
		CFIDDrawObj::DrawHead(pDC);
		pDC->SetTextColor(m_NameColor);
		pDC->SetBkMode(TRANSPARENT); 
		pDC->SetTextAlign(TA_CENTER | TA_TOP); 
		CFont m_Font1;
		m_Font1.CreateFontIndirect(&m_NameFont);
		CFont* pOldFont = pDC->SelectObject(&m_Font1);
   		CSize size = pDC->GetTextExtent("Œƒ±æ");
		pDC->TextOut((rect.left + rect.right)/2, rect.top + size.cy -5, m_AliasName,strlen(m_AliasName));
		pDC->SelectObject(pOldFont);  
		
		CFont m_Font;
		m_Font.CreateFontIndirect(&m_ScaleFont);
	    pOldFont = pDC->SelectObject(&m_Font);
		pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
		pDC->SetTextColor(m_NameColor);
		char ss[10];
		dtos(m_LeftVal,ss);
	    pDC->TextOut(rect.left+10,rect.bottom-2,ss,strlen(ss));
		dtos(m_RightVal,ss);
		pDC->TextOut(rect.right-10,rect.bottom-2,ss,strlen(ss));
		int x0,x1,y1,x2,y2;
		x0=(rect.left+rect.right)/2;
		int k=0;
		for(int j=0;j<6;j++)
		{
			if(m_PlotContent.Find(Title[j])>-1)
			{
				pDC->SetTextColor(Color[j]);  
				pDC->TextOut((rect.left + rect.right)/2, rect.bottom - 30*k-size.cy-4, Title[j], strlen(Title[j]));	
				CPen pen;
				pen.CreatePen (PS_SOLID,1,Color[j]);
				CPen* Oldpen=pDC->SelectObject (&pen);
				CBrush m_Brush(Color[j]);
				CBrush* pOldBrush = pDC->SelectObject(&m_Brush); 
				if(m_bPrintTrueDip)
				{
					x1=x0-50;x2=x0-40;
					y1=rect.bottom - 30*k-size.cy-15;y2=rect.bottom - 30*k-size.cy-5;
					pDC->Ellipse(x1,y1,x2,y2);
					pDC->MoveTo(x0-45,y1+5);pDC->LineTo(x0-45,y1-10);
				}
				if(m_bPrintOriginDip)
				{
					pDC->SelectStockObject(NULL_BRUSH);
					x1=x0+40;x2=x0+51;
					y1=rect.bottom - 30*k-size.cy-15;y2=rect.bottom - 30*k-size.cy-5;
					pDC->Ellipse(x1,y1,x2,y2);
					pDC->MoveTo(x0+45,y1);pDC->LineTo(x0+45,y1-10);
				}
				pDC->SelectObject (Oldpen);
				pDC->SelectObject (pOldBrush);
				k++;
			}
		}
		pDC->SelectObject(pOldFont); 
	}
}                              

BOOL CMarkVectorObj::Properties(CView* pView)
{
	CString szHT;
	szHT.LoadString(IDS_DIPPROPERTIES);
	CPropertySheet m_Sheet("Ωªª•Ω‚ Õ ∏¡øÕº");
	CHandWorkPage  m_HandWorkPage;
	m_HandWorkPage.m_FileName =m_strPathName;
	m_HandWorkPage.m_PlotContent=m_PlotContent;
	CMarkVectorPage m_VectorPage(this);
	m_VectorPage.m_DepLevel =m_DepLevel;
	m_Sheet.AddPage(&m_HandWorkPage);
	m_Sheet.AddPage(&m_VectorPage);
	CLineListPage m_LineList;
	CFillListPage m_FillList;
 	if (m_ParentType != 0)
	{
		m_LineList.AddPen(&m_FramePen,"øÚœﬂ");
		szHT.LoadString(IDS_FRAMEFILL);
		m_FillList.AddFill(&m_Brush,szHT);
		m_Sheet.AddPage(&m_LineList);
		m_Sheet.AddPage(&m_FillList);
	}

	CMinFontPage m_FontPage;
	TEXTFONT m_TextFont[2];
	szHT.LoadString(IDS_ALIASFONT);
	m_TextFont[0].m_ItemName = m_AliasName;
	szHT.ReleaseBuffer();
	m_TextFont[0].m_Text = m_AliasName;
	m_TextFont[0].m_ForeGround = m_NameColor;
	m_TextFont[0].m_BackGround = RGB(255,255,255);
	m_TextFont[0].lf = m_NameFont;
	m_FontPage.AddFont(&m_TextFont[0]);
	szHT.LoadString(IDS_ZERFONT);
	m_TextFont[1].m_ItemName = "Ω‚ ÕÕº¿˝";
	szHT.ReleaseBuffer();
	m_TextFont[1].m_Text = "≥‰ÃÓ∑Ï";
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
		m_DepLevel=m_VectorPage.m_DepLevel;
		m_NameColor = m_TextFont[0].m_ForeGround;
		m_NameFont = m_TextFont[0].lf;
		m_ScaleFont = m_TextFont[1].lf;
		m_NameFont.lfEscapement = 0;
		m_ScaleFont.lfEscapement = 0;
		CRect SaveRect = m_Rect;
		SaveRect.top = m_HeadRect.top;
		m_HandWorkPage.OnOK();
		m_strPathName=m_HandWorkPage.m_FileName;
		m_PlotContent=m_HandWorkPage.m_PlotContent;
		m_bFlush=TRUE;
		if (m_ParentType != 0) 
			SaveRect.bottom = m_Rect.bottom;
		InvalidateRect(SaveRect);
		return TRUE;
	}
	return FALSE;
}



