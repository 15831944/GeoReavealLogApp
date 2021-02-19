/*************************************************************************
*   PoleDraw.cpp - implementation of the Pole draw object class      *           
*************************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "ImageMan.h"
#include "ImageManDoc.h"
#include "TrackDraw.h"
#include "FIDIO.h"
#include "PoleDraw.h"
#include "MinFontDialog.h"
#include "LineListPage.h"
#include "FillListPage.h"
#include "PolePage.h"
#include "PoleDraw.h"
#include "ImageDraw.h"

CPolePoint::CPolePoint(CRect rect,float Dep,float Dip,float Dir,CString Flag,
					   int DrawType,WORD PenWidth,COLORREF Color)
{
	m_Dep=Dep;
	m_Dip=Dip;
	m_Dir=Dir;
	m_Flag=Flag;
	m_Rect = rect;
	m_Color=Color;
	m_PenWidth=PenWidth;
	m_DrawType=DrawType;
}

void CPolePoint::Draw(CDC* pDC,CImageManDoc* pDoc)
{
	CRect rect0=m_Rect;
	pDoc->LPtoPP(&rect0);
	if (!pDC->RectVisible(rect0)) return; 
	pDoc->LPtoPP(m_Pt,4);
	if(m_DrawType>0)
	{
		int nSaveDC = pDC->SaveDC();	
		CBrush br(m_Color);
		CBrush* pOldBrush = pDC->SelectObject(&br);
		pDC->SetROP2(R2_MASKNOTPEN);//R2_NOTXORPEN R2_MASKNOTPEN R2_XORPEN
		pDC->Polygon (m_Pt,4);
		pDC->SelectObject(pOldBrush);
		pDC->RestoreDC(nSaveDC);
	}
	else
	{
		CPen pen(0,m_PenWidth,m_Color);
		CPen* Oldpen=(CPen*)pDC->SelectObject(&pen);
		pDC->MoveTo(m_Pt[0]);
		pDC->LineTo(m_Pt[2]);
		pDC->SelectObject(Oldpen);
	}
}

IMPLEMENT_SERIAL(CPoleObj, CFIDDrawObj, 0)

CPoleObj::CPoleObj()
{
	m_DrawShape = Poleobj;
	m_bFlush=TRUE;
}
    
CPoleObj::CPoleObj(CString szPathName, const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDoc)
			:CFIDDrawObj(szPathName,rect,pParentObj,pDoc)
{
	m_DrawShape = Poleobj;
	m_AliasName = "¸Ë×´Í¼";
	m_ScaleFont = m_NameFont; 
	m_ScaleFont.lfHeight = -10;
	m_ScaleFont.lfWeight = 300;
	m_ScaleColor = m_NameColor;
	m_PenWidth=1;
	m_PenColor=0x0;
	m_LookAngle=30.f;
	m_StartPos=0;
	m_DrawType=0;
	m_bFlush=TRUE;
}

CPoleObj::~CPoleObj()
{
	POSITION pos = m_PolePoint.GetHeadPosition();
	while (pos != NULL) 
		delete m_PolePoint.GetNext(pos);
	m_PolePoint.RemoveAll();
}    

void CPoleObj::Serialize( CArchive& ar )
{
	CFIDDrawObj::Serialize( ar );
	if(!ar.IsStoring())
	{
		ar >> m_DrawType;
		ar >> m_PenWidth;
		ar >> m_PenColor;
		ar >> m_LookAngle;
		ar >> m_StartPos;
		SerializeLogFont(ar,m_ScaleFont);
		m_bFlush=TRUE;
	}		
}

void CPoleObj::SetRect(const CRect& rect)
{
	if (m_Rect == rect) return;
	CFIDDrawObj::SetRect(rect);
	m_bFlush=TRUE;
} 

void CPoleObj::CreatePoint()
{
	CRect rect=m_Rect;
	POSITION pos = m_PolePoint.GetHeadPosition();
	while (pos != NULL) 
		delete m_PolePoint.GetNext(pos);
	m_PolePoint.RemoveAll();
	if(DetectFileExist(m_strPathName))
	{
		m_bFlush=FALSE;
		DWORD Pos,High;
		CString Flag;COLORREF Color;
		float Dep,Dip,Dir;double q; 
			
		TCHAR *token, *end;
		TCHAR buffer[1024];
		CStdioFile File;
		double r=m_Rect.Width()/2;
		CPoint pt[4];	
		double cl=r*cos(m_LookAngle*PI/180.0);
		double sl=r*sin(m_LookAngle*PI/180.0);
		if (File.Open(m_strPathName, CFile::modeRead | CFile::typeText)) 
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
				if((Dip>0.005)&&
					((Flag=="²ãÀí")||(Flag=="³äÌî·ì")
					||(Flag=="ÕÅ¿ª·ì")||(Flag=="Ñ¹ÁÑ·ì")
					||(Flag=="°ë³äÌî·ì")||(Flag=="²ã¼ä·ì")))
				{	
					Pos= DepthToYCoor(Dep);
					q=Dip*PI/180.;				
					Dir=(float)(Dir*PI/180.);
					float dira=(float)(m_StartPos*PI/180.);
					float dipa=(float)(atan(tan(q)*cos(dira-Dir)));
					
					q=dipa*PI/180.;
					High =(DWORD)(m_Rect.Width()/2.*tan(q));
					rect.left=m_Rect.left+(long)(r-r*cos(q));
					rect.right=m_Rect.left+(long)(r+r*cos(q));
					CPoint pt[4];	
					pt[0]=rect.left;pt[0].y=Pos-High;
					pt[2]=rect.right;pt[2].y=Pos+High;
					pt[1].x=pt[2].x-(long)cl;
					pt[3].x=pt[0].x+(long)cl;
					pt[1].y=pt[2].y+(long)(sl);
					pt[3].y=pt[0].y-(long)(sl);
					rect.top=pt[3].y;
					rect.bottom=pt[1].y;
					if(Flag=="²ãÀí")     //ÂÌÉ«--²ãÀí
						Color=0x009300;
					else if(Flag=="³äÌî·ì")//ÇàÀ¶É«--³äÌî·ì
						Color=0x00FF00;
					else if(Flag=="ÕÅ¿ª·ì")//À¶É«--ÕÅ¿ª·ì
						Color=0xFF0000;
					else if(Flag=="°ë³äÌî·ì")//ÌìÀ¶É«--°ë³äÌî·ì
						Color=0xFF8000;
					else if(Flag=="²ã¼ä·ì")//ÇàÉ«--°ë³äÌî·ì
						Color=0xFFFF00;
					else               //×ÏÉ«--Ñ¹ÁÑ·ì
						Color=0x800080;
					CPolePoint* pObj=new CPolePoint(rect,Dep,dipa,dira,Flag,m_DrawType,m_PenWidth,Color);
					for(i=0;i<4;i++)
						pObj->m_Pt[i]=pt[i];
					m_PolePoint.AddTail(pObj);
				}
			}
			File.Close();
		}
	}
}

void CPoleObj::DrawClient(CDC* pDC)
{    
	if ((m_pParentObj == NULL) || (m_ParentType != 0))
		CFIDDrawObj::DrawClient(pDC);
	DrawPole(pDC);
}

void CPoleObj::DrawHead(CDC* pDC)
{
	CString Title[6]={"²ãÀí","³äÌî·ì","ÕÅ¿ª·ì","Ñ¹ÁÑ·ì","°ë³äÌî·ì","²ã¼ä·ì"};
	COLORREF Color[6]={0x009300,0x00FF00,0xFF0000,0x800080,0xFF8000,0xFFFF00};
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
   		CSize size = pDC->GetTextExtent("ÎÄ±¾");
		pDC->TextOut((rect.left + rect.right)/2, rect.top + size.cy -5, m_AliasName,strlen(m_AliasName));
		pDC->SelectObject(pOldFont);  
	}
}                              

BOOL CPoleObj::Properties(CView* pView)
{
	CString szHT;
	CPropertySheet m_Sheet("½»»¥½âÊÍ½áÂÛ");
		
	CPolePage m_PolePage;
	m_PolePage.m_FileName =m_strPathName;
	m_PolePage.m_LookAngle=m_LookAngle;
	m_PolePage.m_StartPos=m_StartPos;
	m_PolePage.m_DrawType=m_DrawType;
	m_Sheet.AddPage(&m_PolePage);

	CLineListPage m_LinePage;
	LOGPEN m_LogPen;
	m_LogPen.lopnColor =m_PenColor;
	m_LogPen.lopnWidth.x=m_PenWidth;
	m_LogPen.lopnWidth.y =m_PenWidth;
	m_LogPen.lopnStyle =BS_SOLID;
	m_LinePage.AddPen(&m_LogPen,"Sin-line");
	m_Sheet.AddPage(&m_LinePage);

	CMinFontPage m_FontPage;
	TEXTFONT m_TextFont;
	szHT.LoadString(IDS_ALIASFONT);
	m_TextFont.m_ItemName = m_AliasName;
	szHT.ReleaseBuffer();
	m_TextFont.m_Text = m_AliasName;
	m_TextFont.m_ForeGround = m_NameColor;
	m_TextFont.lf = m_NameFont;
	m_FontPage.AddFont(&m_TextFont);
	TEXTFONT m_TextFont1;
	szHT.LoadString(IDS_ZERFONT);
	m_TextFont1.m_ItemName = "½âÊÍÍ¼Àý";
	szHT.ReleaseBuffer();
	m_TextFont1.m_Text = "³äÌî·ì";
	m_TextFont1.m_ForeGround = m_ScaleColor;
	m_TextFont1.lf = m_ScaleFont;
	m_FontPage.AddFont(&m_TextFont1);

	m_Sheet.AddPage(&m_FontPage);
	if (m_Sheet.DoModal() == IDOK)
	{
		m_PenWidth=(WORD)m_LogPen.lopnWidth .y ;
		m_FontPage.OnOK();
		m_AliasName=m_TextFont.m_Text;
		m_NameColor = m_TextFont.m_ForeGround;
		m_NameFont = m_TextFont.lf;
		m_ScaleFont = m_TextFont1.lf;
		m_NameFont.lfEscapement = 0;
		m_AliasName=m_TextFont.m_Text;
		m_ScaleFont.lfEscapement = 0;
		CRect SaveRect = m_Rect;
		SaveRect.top = m_HeadRect.top;
		m_PolePage.OnOK();
		m_strPathName=m_PolePage.m_FileName;
		m_LookAngle=m_PolePage.m_LookAngle;
		m_StartPos=m_PolePage.m_StartPos;
		m_DrawType=m_PolePage.m_DrawType;
		m_bFlush=TRUE;
		if (m_ParentType != 0) 
			SaveRect.bottom = m_Rect.bottom;
		InvalidateRect(SaveRect);
		return TRUE;
	}
	return FALSE;
}

void CPoleObj::DrawPole(CDC* pDC)
{			
	if(m_bFlush) CreatePoint();
	CPolePoint* pObj;
	POSITION pos = m_PolePoint.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CPolePoint*) m_PolePoint.GetNext(pos);
		CRect rect = pObj->GetRect();
		pDoc->LPtoPP(&rect);
		if (pDC->RectVisible(rect))
			pObj->Draw(pDC,pDoc);
	}
}

void CPoleObj::SetSinRect(const CRect& rect)
{
	CPolePoint* pObj;
	CRect rect1;
	POSITION pos = m_PolePoint.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CPolePoint*) m_PolePoint.GetNext(pos);
		rect1 = pObj->GetRect();
		rect1.left = rect.left;
		rect1.right = rect.right;
		pObj->SetRect(rect1);
	}          
}
