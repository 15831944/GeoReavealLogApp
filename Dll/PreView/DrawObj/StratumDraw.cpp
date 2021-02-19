/*************************************************************************
*   StratumDraw.cpp - implementation of the Stratum draw object class      *           
*************************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "TrackDraw.h"
#include "GeoFIDio.h"
#include "StratumDraw.h"
#include "ImageDraw.h"

//#include "MinFontDialog.h"
//#include "LineListPage.h"
//#include "FillListPage.h"
//#include "HandWorkPage.h"


CSinPoint::CSinPoint(CRect rect,float Dep,float Dip,float Dir,CString Flag,WORD PenWidth,COLORREF Color)
{
	m_Dep=Dep;
	m_Dip=Dip;
	m_Dir=Dir;
	m_Flag=Flag;
	m_Rect = rect;
	m_Color=Color;
	m_PenWidth=PenWidth;
}

void CSinPoint::Draw(CDC* pDC,CLogPlotDoc* pDoc)
{
	CRect rect0=m_Rect;
	pDoc->LPtoPP(&rect0);
	if (!pDC->RectVisible(rect0)) return; 
	double dtor=m_Rect.Width ()*1./360.;
	DWORD Pos=(rect0.top+rect0.bottom)/2;
	DWORD High=rect0.Height()/2;
	int x,y;
	x=rect0.left;
	y=(int)(Pos+cos(-m_Dir*PI/180.)*High);
	Graphics *pic=NULL;
	pic=new Graphics(pDC->m_hDC);
	pic->SetSmoothingMode(SmoothingModeHighQuality);
	Color CurveColor(GetRValue(m_Color),GetGValue(m_Color),GetBValue(m_Color));
	Pen *pn= new Pen(CurveColor,(float)m_PenWidth);
	CPoint pt(x,y);
	for(int i=5;i<=360;i+=5)
	{
		x=(int)(rect0.left +dtor*i);
		y=(int)(Pos+cos((i*1.-m_Dir)*PI/180.)*High);
		pic->DrawLine(pn,pt.x,pt.y,x,y);
		pt.x=x;pt.y=y;
	}
	pic->ReleaseHDC(pDC->m_hDC);
	delete pn;
	delete pic;
}

IMPLEMENT_SERIAL(CStratumObj, CFIDDrawObj, 0)

CStratumObj::CStratumObj()
{
	m_DrawShape = stratumobj;
	bFlush=TRUE;
}
    
CStratumObj::CStratumObj(CString szPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDoc)
			:CFIDDrawObj(szPathName,rect,pParentObj,pDoc)
{
	m_DrawShape = stratumobj;
	m_AliasName = "≤„¿Ì¡—∑Ï∑÷Œˆ";
	m_ScaleFont = m_NameFont; 
	m_ScaleFont.lfHeight = -10;
	m_ScaleFont.lfWeight = 300;
	m_ScaleColor = m_NameColor;
	m_PenWidth=1;
	bFlush=TRUE;
	m_PlotContent=_T("");
}

CStratumObj::~CStratumObj()
{
	POSITION pos = m_SinPoint.GetHeadPosition();
	while (pos != NULL) 
		delete m_SinPoint.GetNext(pos);
	m_SinPoint.RemoveAll();
}    

void CStratumObj::Serialize( CArchive& ar )
{
	long l;
	CFIDDrawObj::Serialize( ar );
	WORD m_DepLevel=1,m_TailLength=10,m_CircleRadius=30,m_TailWidth=1;
	COLORREF m_CircleColor=0x009300,m_TailColor=0x009300;
	if(ar.IsStoring())
	{
		ar << m_DepLevel;
		ar << m_PenWidth;
		ar << m_CircleRadius;
		ar << (long) m_CircleColor;
		ar << m_TailWidth;
		ar << m_TailLength;
		ar << (long) m_TailColor;
		ar << (long) m_ScaleColor;
		ar << m_PlotContent;
		SerializeLogFont(ar,m_ScaleFont);
	}
	else
	{
		ar >> m_DepLevel;
		ar >> m_PenWidth;
		ar >> m_CircleRadius;
		ar >> l; m_CircleColor = (COLORREF) l;
		ar >> m_TailWidth;
		ar >> m_TailLength;
		ar >> l; m_TailColor = (COLORREF) l;
		ar >> l; m_ScaleColor = (COLORREF) l;
		ar >> m_PlotContent;
		SerializeLogFont(ar,m_ScaleFont);
		bFlush=TRUE;
	}		
}

void CStratumObj::SetRect(const CRect& rect)
{
	if (m_Rect == rect) return;
	CFIDDrawObj::SetRect(rect);
	bFlush=TRUE;
} 

void CStratumObj::CreatePoint()
{
	CRect rect=m_Rect;
	POSITION pos = m_SinPoint.GetHeadPosition();
	while (pos != NULL) 
		delete m_SinPoint.GetNext(pos);
	m_SinPoint.RemoveAll();
	bFlush=FALSE;
	if(DetectFileExist(m_strPathName))
	{
		DWORD Pos,High;
		CString Flag;COLORREF Color;
		float Dep,Dip,Dir;double q;
		double tmp = YCoorToDepth (1000+m_Rect.Width())-YCoorToDepth (1000); 
		double amp=(DepthToYCoor(1000.f+tmp)-DepthToYCoor(1000.f))*0.5/PI;
		double HVr=0.5;
		CTrackObj* pTrack=(CTrackObj*)GetParentObj();
		if(pTrack!=NULL)
		{
			HVr=0.f;
			int HorScale=0;
			CImageObj* pImage=(CImageObj*)pTrack->FindCurve("DYNMI");
			if(pImage!=NULL)
			{
				HorScale=(int)(pImage->m_Bit*PI/(pImage->m_Rect.Width ()*0.254));
				HVr=HorScale/pDoc->m_DepthScale;
			}
			else
			{
				pImage=(CImageObj*)pTrack->FindCurve("STAMI");
				if(pImage!=NULL)
				{
					HorScale=(int)(pImage->m_Bit*PI/(pImage->m_Rect.Width ()*0.254));
					HVr=HorScale/pDoc->m_DepthScale;
				}
			} 
		}
		amp=amp*HVr;
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
				if(m_PlotContent.Find(Flag)>-1)
				{		
					Pos= DepthToYCoor(Dep);
					q=Dip*PI/180.;
					High =(DWORD)(amp*tan(q));
					rect.top=Pos-High;
					rect.bottom=Pos+High;
					if(Flag=="≤„¿Ì")     //¬Ã…´--≤„¿Ì
						Color=0x009300;
					else if(Flag=="≥‰ÃÓ∑Ï")//«‡¿∂…´--≥‰ÃÓ∑Ï
						Color=0x00FF00;
					else if(Flag=="’≈ø™∑Ï")//¿∂…´--’≈ø™∑Ï
						Color=0xFF0000;
					else if(Flag=="∞Î≥‰ÃÓ∑Ï")//ª“¿∂…´--∞Î≥‰ÃÓ∑Ï
						Color=0xC09E7D;
					else if(Flag=="≤„º‰∑Ï")//…Ó¿∂…´--∞Î≥‰ÃÓ∑Ï
						Color=0x800000;
					else if(Flag=="π‹¥ÆΩ”πø")
						Color=0xFF8000;
					else               //◊œ…´--—π¡—∑Ï
						Color=0x800080;
					CSinPoint* pObj=new CSinPoint(rect,Dep,Dip,Dir,Flag,m_PenWidth,Color);
					m_SinPoint.AddTail(pObj);
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

void CStratumObj::DrawClient(CDC* pDC)
{    
	if ((m_pParentObj == NULL) || (m_ParentType != 0))
		CFIDDrawObj::DrawClient(pDC);
	DrawSin(pDC);
}

void CStratumObj::DrawHead(CDC* pDC)
{
	CString Title[7]={"≤„¿Ì","≥‰ÃÓ∑Ï","’≈ø™∑Ï","—π¡—∑Ï","∞Î≥‰ÃÓ∑Ï","≤„º‰∑Ï","π‹¥ÆΩ”πø"};
	COLORREF Color[7]={0x009300,0x00FF00,0xFF0000,0x800080,0xC09E7D,0x800000,0xFF8000};
	CRect rect = m_HeadRect;
	pDoc->LPtoPP(rect);
	if(pDC->RectVisible(rect))
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
		pDC->SelectObject(pOldFont);  
		
		CFont m_Font;
		m_Font.CreateFontIndirect(&m_ScaleFont);
	    pOldFont = pDC->SelectObject(&m_Font);
		pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
		int k=0;
		CPen pen1;
		pen1.CreatePen (PS_SOLID,5,Color[0]);
		CPen* Oldpen=pDC->SelectObject (&pen1);
		for(int j=0;j<7;j++)
		{
			if(m_PlotContent.Find(Title[j])>-1)
			{
				pDC->SetTextColor(Color[j]);  
				pDC->TextOut((rect.left + rect.right)/2, rect.bottom - 30*k-20, Title[j], (int)strlen(Title[j]));	
				CPen pen;
				pen.CreatePen (PS_SOLID,5,Color[j]);
				pDC->SelectObject (&pen);
				pDC->MoveTo (rect.left+30,rect.bottom - 30*k- 15);
				pDC->LineTo (rect.right-30,rect.bottom - 30*k- 15);
				k++;
			}
		}
		pDC->SelectObject (Oldpen);
		pDC->SelectObject(pOldFont); 
	}
}                              

BOOL CStratumObj::Properties(CView* pView)
{
	/*CString szHT;
	CPropertySheet m_Sheet("≤„¿Ì¡—∑Ï∑÷Œˆ");
		
	CHandWorkPage m_HandWorkPage;
	m_HandWorkPage.m_FileName =m_strPathName;
	m_HandWorkPage.m_PlotContent=m_PlotContent;
	m_Sheet.AddPage(&m_HandWorkPage);

	CLineListPage m_LinePage;
	LOGPEN m_LogPen;
	m_LogPen.lopnColor =RGB(0,0,255);
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
		m_HandWorkPage.OnOK();
		m_strPathName=m_HandWorkPage.m_FileName;
		m_PlotContent=m_HandWorkPage.m_PlotContent;
		bFlush=TRUE;
		if (m_ParentType != 0) 
			SaveRect.bottom = m_Rect.bottom;
		pDoc->SetModifiedFlag();
		CreatePoint();
		InvalidateRect(SaveRect);
		return TRUE;
	}*/
	return FALSE;
}

void CStratumObj::DrawSin(CDC* pDC)
{			
	if(bFlush) CreatePoint();
	CSinPoint* pObj;
	POSITION pos = m_SinPoint.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CSinPoint*) m_SinPoint.GetNext(pos);
		CRect rect = pObj->GetRect();
		pDoc->LPtoPP(&rect);
		if (pDC->RectVisible(rect))
			pObj->Draw(pDC,pDoc);
	}
}

void CStratumObj::SetSinRect(const CRect& rect)
{
	CSinPoint* pObj;
	CRect rect1;
	POSITION pos = m_SinPoint.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CSinPoint*) m_SinPoint.GetNext(pos);
		rect1 = pObj->GetRect();
		rect1.left = rect.left;
		rect1.right = rect.right;
		pObj->SetRect(rect1);
	}          
}
