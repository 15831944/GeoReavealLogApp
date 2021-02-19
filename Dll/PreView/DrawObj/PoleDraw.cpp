/*************************************************************************
*   PoleDraw.cpp - implementation of the Pole draw object class      *           
*************************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "TrackDraw.h"
#include "GeoFIDio.h"
#include "PoleDraw.h"
#include "ImageDraw.h"

//#include "MinFontDialog.h"
//#include "LineListPage.h"
//#include "FillListPage.h"
//#include "PolePage.h"


CPolePoint::CPolePoint(CRect rect,float Dep,float Dip,float Dir,CString Flag,
					   WORD PenWidth,COLORREF Color,int Pos,float LookAngle,double HVr,int DrawType)
{
	m_Dep=Dep;
	m_Dip=Dip;
	m_Dir=Dir;
	m_Flag=Flag;
	m_Rect = rect;
	m_Color=Color;
	m_PenWidth=PenWidth;
	m_LookAngle=LookAngle;
	m_Pos=Pos;
	m_HVr=HVr;
	m_DrawType=DrawType;
}

void CPolePoint::Draw(CDC* pDC,CLogPlotDoc* pDoc,int StartPos)
{
	CRect rect=m_Rect;
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return; 
	
	double r=rect.Width()/3;
	int x00,y00,x0,y0,x,y;
	Graphics *pic=NULL;
	pic=new Graphics(pDC->m_hDC);
	pic->SetSmoothingMode(SmoothingModeHighQuality);
	Pen *m_pPens=NULL;
	Color CurveColor(GetRValue(m_Color),GetGValue(m_Color),GetBValue(m_Color));
	m_pPens = new Pen(CurveColor,(float)m_PenWidth);
	if(m_DrawType==0)
	{
		x0=rect.left+rect.Width()/2-(int)r;
		y0=m_Pos+(int)(r*tan(m_Dip*PI/180.));
		x=rect.left+rect.Width()/2+(int)r;
		y=m_Pos-(int)(r*tan(m_Dip*PI/180.));
		if(m_Dir>90.f&&m_Dir<270.f)
			pic->DrawLine(m_pPens,x0,y,x,y0);
		else
			pic->DrawLine(m_pPens,x0,y0,x,y);
	}
	else
	{
		int StartX=rect.left+rect.Width()/2-(int)r;
		int EndX=rect.left+rect.Width()/2+(int)r;
		double dtor=r/90.;
		double High =(int)(r*tan(m_Dip*PI/180.)*m_HVr);
		double hh=sin(m_LookAngle*PI/180.0);
		x0=StartX;
		y0=m_Pos+(int)(cos((StartPos-m_Dir)*PI/180.)*High);
		x00=x0;y00=y0;
		
		int i,j;
		//œ¬∞Î÷‹
		for(i=0;i<180;i+=10)
		{
			x=StartX+(int)(dtor*i);
			y=m_Pos+(int)(cos((i*1.-m_Dir+StartPos)*PI/180.)*High);
			y=y+(int)(sqrt(r*r-(dtor*i-r)*(dtor*i-r))*hh);
			pic->DrawLine(m_pPens,x0,y0,x,y);
			x0=x;y0=y;
		}
		//…œ∞Î÷‹
		j=0;
		for(i=180;i>0;i-=10)
		{
			x=StartX+(int)(dtor*i);
			y=m_Pos-(int)(cos((j*1.-m_Dir+StartPos)*PI/180.)*High);
			y=y-(int)(sqrt(r*r-(dtor*i-r)*(dtor*i-r))*hh);
			pic->DrawLine(m_pPens,x0,y0,x,y);
			x0=x;y0=y;
			j+=10;
		}

		pic->DrawLine(m_pPens,x00,y00,x0,y0);
	}
	pic->ReleaseHDC(pDC->m_hDC);
	delete m_pPens;
	delete pic;
}

IMPLEMENT_SERIAL(CPoleObj, CFIDDrawObj, 0)

CPoleObj::CPoleObj()
{
	m_DrawShape = Poleobj;
	bFlush=TRUE;
}
    
CPoleObj::CPoleObj(CString szPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDoc)
			:CFIDDrawObj(szPathName,rect,pParentObj,pDoc)
{
	m_DrawShape = Poleobj;
	m_AliasName = "∏À◊¥Õº";
	m_ScaleFont = m_NameFont; 
	m_ScaleFont.lfHeight = -10;
	m_ScaleFont.lfWeight = 300;
	m_ScaleColor = m_NameColor;
	m_PenWidth=1;
	m_PenColor=0x0;
	m_LookAngle=30.f;
	m_StartPos=0;
	m_DrawType=0;
	m_Bits=216;
	bFlush=TRUE;
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
	if(ar.IsStoring())
	{
		m_CtrlVer = 1;
		ar << m_CtrlVer;
		ar << m_DrawType;
		ar << m_PenWidth;
		ar << m_PenColor;
		ar << m_LookAngle;
		ar << m_StartPos;
		ar << m_DazName;
		ar << m_Bits;
		SerializeLogFont(ar,m_ScaleFont);
	}
	else
	{
		ar >> m_CtrlVer;
		ar >> m_DrawType;
		ar >> m_PenWidth;
		ar >> m_PenColor;
		ar >> m_LookAngle;
		ar >> m_StartPos;
		ar >> m_DazName;
		ar >> m_Bits;
		SerializeLogFont(ar,m_ScaleFont);
		bFlush=TRUE;
	}		
}

void CPoleObj::SetRect(const CRect& rect)
{
	if (m_Rect == rect) return;
	CFIDDrawObj::SetRect(rect);
	bFlush=TRUE;
} 

void CPoleObj::CreatePoint()
{
	CRect rect=m_Rect;
	POSITION pos = m_PolePoint.GetHeadPosition();
	while (pos != NULL) 
		delete m_PolePoint.GetNext(pos);
	m_PolePoint.RemoveAll();
	bFlush=FALSE;
	if(DetectFileExist(m_strPathName))
	{
		DWORD Pos;
		CString Flag;COLORREF Color;
		float Dep,Dip,Dir;

		double amp=rect.Width()/3;
		int HorScale=(int)(m_Bits/(rect.Width()*0.254*2/3.));
		double HVr=HorScale/pDoc->m_DepthScale;
		int High;
		TCHAR *token, *end;
		TCHAR buffer[1024];
		CStdioFile File;
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
				Flag="≤„¿Ì";
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
				if(Dip>=0.f&&m_DazName.Find(Flag)>-1)
				{	
					Pos= DepthToYCoor(Dep);	
					if(m_DrawType==0)
					{
						//º∆À„∆ √Ê∑ΩŒªµƒ ”«„Ω«
						Dip=(float)(atan(tan(Dip*PI/180.)*cos((m_StartPos-Dir)*PI/180.)));
						Dip=(float)fabs(Dip*180./PI);
					}
					High =(int)(amp*tan(Dip*PI/180.));
					rect.top=Pos-High;
					rect.bottom=Pos+High;
					if(Flag=="≤„¿Ì")         //¬Ã…´--≤„¿Ì
						Color=0x009300;
					else if(Flag=="≥‰ÃÓ∑Ï")  //«‡¿∂…´--≥‰ÃÓ∑Ï
						Color=0x00FF00;
					else if(Flag=="’≈ø™∑Ï")  //¿∂…´--’≈ø™∑Ï
						Color=0xFF0000;
					else if(Flag=="∞Î≥‰ÃÓ∑Ï")//ª“¿∂…´--∞Î≥‰ÃÓ∑Ï
						Color=0xC09E7D;
					else if(Flag=="≤„º‰∑Ï")  //…Ó¿∂…´--∞Î≥‰ÃÓ∑Ï
						Color=0x800000;
					else if(Flag=="π‹¥ÆΩ”πø")
						Color=0xFF8000;
					else                     //◊œ…´--—π¡—∑Ï
						Color=0x800080;
					CPolePoint* pObj=new CPolePoint(rect,Dep,Dip,Dir,Flag,m_PenWidth,Color,
						                            Pos,m_LookAngle,HVr,m_DrawType);
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
		pDC->TextOut((rect.left + rect.right)/2, rect.top + size.cy -5, m_AliasName,(int)strlen(m_AliasName));
	
		CFont m_Font;
		m_Font.CreateFontIndirect(&m_ScaleFont);
		pDC->SelectObject(&m_Font);
		pDC->MoveTo(m_HeadRect.left+10,m_HeadRect.bottom-5);
		pDC->LineTo(m_HeadRect.right-15,m_HeadRect.bottom - 5);

		pDC->MoveTo(m_HeadRect.right-15,m_HeadRect.bottom-10);
		pDC->LineTo(m_HeadRect.right-10,m_HeadRect.bottom - 5);
		pDC->MoveTo(m_HeadRect.right-15,m_HeadRect.bottom);
		pDC->LineTo(m_HeadRect.right-10,m_HeadRect.bottom - 5);
		pDC->LineTo(m_HeadRect.right-9,m_HeadRect.bottom- 5);
		pDC->MoveTo(m_HeadRect.right-15,m_HeadRect.bottom);
		pDC->LineTo(m_HeadRect.right-15,m_HeadRect.bottom- 10);
		
		CString str;
		str.Format("%d",m_StartPos);
		size = pDC->GetTextExtent(str);
		pDC->TextOut((rect.left + rect.right)/2, m_HeadRect.bottom-size.cy-5, str,(int)strlen(str));
		pDC->SelectObject(pOldFont); 
	}
}                              

BOOL CPoleObj::Properties(CView* pView)
{
	/*CString szHT;
	CPropertySheet m_Sheet("∏À◊¥-¡¢ÃÂ∆ √ÊÕº");
		
	CPolePage m_PolePage;
	m_PolePage.m_FileName =m_strPathName;
	m_PolePage.m_LookAngle=m_LookAngle;
	m_PolePage.m_StartPos=m_StartPos;
	m_PolePage.m_DrawType=m_DrawType;
	m_PolePage.m_DazName=m_DazName;
	m_PolePage.m_Bits=m_Bits;
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
	m_TextFont1.m_ItemName = "Ω‚ ÕÕº¿˝";
	szHT.ReleaseBuffer();
	m_TextFont1.m_Text = "≥‰ÃÓ∑Ï";
	m_TextFont1.m_ForeGround = m_ScaleColor;
	m_TextFont1.lf = m_ScaleFont;
	m_FontPage.AddFont(&m_TextFont1);

	m_Sheet.AddPage(&m_FontPage);
	if (m_Sheet.DoModal() == IDOK)
	{
		m_PenWidth=(WORD)m_LogPen.lopnWidth.y ;
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
		m_DazName=m_PolePage.m_DazName;
		m_LookAngle=m_PolePage.m_LookAngle;
		m_StartPos=m_PolePage.m_StartPos;
		m_Bits=m_PolePage.m_Bits;
		m_DrawType=m_PolePage.m_DrawType;
		bFlush=TRUE;
		if (m_ParentType != 0) 
			SaveRect.bottom = m_Rect.bottom;
		pDoc->SetModifiedFlag();
		InvalidateRect(SaveRect);
		return TRUE;
	}*/
	return FALSE;
}

void CPoleObj::DrawPole(CDC* pDC)
{			
	if(bFlush) CreatePoint();
	CPolePoint* pObj;
	POSITION pos = m_PolePoint.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CPolePoint*) m_PolePoint.GetNext(pos);
		CRect rect = pObj->GetRect();
		pDoc->LPtoPP(&rect);
		if (pDC->RectVisible(rect))
			pObj->Draw(pDC,pDoc,m_StartPos);
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

int CPoleObj::HitTest(CView* pView,const CPoint& point)
{
	int nHandle = CFIDDrawObj::HitTest(pView,point); 
	if ((nHandle >= 0) || (!m_Rect.PtInRect(point))) return nHandle;
	if(m_Rect.PtInRect(point)) 
		return 9;
	else return -1;
}
