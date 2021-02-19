/******************************************************************
*   CurveDrw.cpp - implementation of the Curve draw object class  *           
*******************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "GeoFIDio.h"
#include "TrackDraw.h"
#include "CurveDraw.h"
//#include "TGraphics.h"

//#include "styledialog.h"
#include "CurvePage.h"
#include "CurveBasePage.h"
//#include "LineListPage.h"
//#include "FillListPage.h"
#include "MinFontDialog.h"
#include "CurveFillPage.h"

int flag = 1;
int nBufferPointNum = 5000;

VOID CALLBACK LineProc(int x,int y,LPARAM fpLine)
{         
	CLinePen* pPen = ((LINE*)(fpLine))->pPen;
	CDC* pDC = ((LINE*)(fpLine))->pDC;
	if((pPen->style >> (31-pPen->org.x))&1)
	{	
		if(flag==1) 
		{
			pDC->MoveTo(x,y);
			flag=0;
		}
		else 
			pDC->LineTo(x,y);
	}	
	else flag=1;
	if (pPen->org.x < pPen->len-1)
		pPen->org.x++; 
	else 
		pPen->org.x = 0; 
}

VOID CALLBACK AntiLineProc100(int x,int y,LPARAM fpLine)
{
	CPoint pt;   	
	CLinePen* pPen = ((LINE*)(fpLine))->pPen;
	CDC* pDC = ((LINE*)(fpLine))->pDC;
	int org=pPen->org.x;
	if (org<pPen->len-1) 
		org++; 
	else 
		org=0;
	if((pPen->style>>(31-pPen->org.x))&1)
	{	
		if (flag==1) 
		{
			pDC->MoveTo(x,y);
			flag=0;
		}
		else
		{
			pt=pDC->GetCurrentPosition();
			if(y-pt.y>1)
			{
				pDC->MoveTo(x,y);
				Graphics *pic=NULL;
				pic=new Graphics(pDC->m_hDC);
				pic->SetSmoothingMode(SmoothingModeHighQuality);
				Color CurveColor(GetRValue(pPen->GetColor()),GetGValue(pPen->GetColor()),GetBValue(pPen->GetColor()));
				Pen *pn= new Pen(CurveColor,(float)pPen->GetWidth());
				pic->DrawLine(pn,pt.x,pt.y,x,y);
				pic->ReleaseHDC(pDC->m_hDC);
				delete pn;delete pic;
			}
			else if((pPen->GetLineNo()!=1))
			{
				if(!((pPen->style>>(31-org))&1))
				{
					pDC->MoveTo(x,y);
					Graphics *pic=NULL;
					pic=new Graphics(pDC->m_hDC);
					pic->SetSmoothingMode(SmoothingModeHighQuality);
					Color CurveColor(GetRValue(pPen->GetColor()),GetGValue(pPen->GetColor()),GetBValue(pPen->GetColor()));
					Pen *pn= new Pen(CurveColor,(float)pPen->GetWidth());
					pic->DrawLine(pn,pt.x,pt.y,x,y);
					pic->ReleaseHDC(pDC->m_hDC);
					delete pn;delete pic;	
				}
			}
		}
	}	
	else flag=1;
	pPen->org.x =org;
}

VOID CALLBACK AntiLineProc(int x,int y,LPARAM fpLine)
{
	CPoint pt;   	
	CLinePen* pPen = ((LINE*)(fpLine))->pPen;
	CDC* pDC = ((LINE*)(fpLine))->pDC;
	int org=pPen->org.x;
	if (org<pPen->len-1) 
		org++; 
	else 
		org=0;
	if((pPen->style>>(31-pPen->org.x))&1)
	{	
		if (flag==1) 
		{
			pDC->MoveTo(x,y);
			flag=0;
		}
		else
		{
			pt=pDC->GetCurrentPosition();
			if(y-pt.y>1)
			{
				pDC->MoveTo(x,y);
				Graphics *pic=NULL;
				pic=new Graphics(pDC->m_hDC);
				pic->SetSmoothingMode(SmoothingModeHighQuality);
				Color CurveColor(GetRValue(pPen->GetColor()),GetGValue(pPen->GetColor()),GetBValue(pPen->GetColor()));
				Pen *pn= new Pen(CurveColor,(float)pPen->GetWidth());
				pic->DrawLine(pn,pt.x,pt.y,x,y);
				pic->ReleaseHDC(pDC->m_hDC);
				delete pn;delete pic;
			}
			else if((pPen->GetLineNo()!=1))
			{
				if(!((pPen->style>>(31-org))&1))
				{
					pDC->MoveTo(x,y);
					Graphics *pic=NULL;
					pic=new Graphics(pDC->m_hDC);
					pic->SetSmoothingMode(SmoothingModeHighQuality);
					Color CurveColor(GetRValue(pPen->GetColor()),GetGValue(pPen->GetColor()),GetBValue(pPen->GetColor()));
					Pen *pn= new Pen(CurveColor,(float)pPen->GetWidth());
					pic->DrawLine(pn,pt.x,pt.y,x,y);
					pic->ReleaseHDC(pDC->m_hDC);
					delete pn;delete pic;	
				}
			}
		}
	}	
	else flag=1;
	pPen->org.x =org;
}

IMPLEMENT_SERIAL(CCurveFill, CObject, 0)

CCurveFill::CCurveFill()
{
	m_Color = RGB(0,0,0);
	m_LthNo = 0;
	m_FillMode = FILLRIGHT;
	pSourceCurve = NULL;
	pTargetCurve = NULL;
}

CCurveFill::CCurveFill(CCurveFill& other)
{
	m_Color = other.m_Color;
	m_LthNo = other.m_LthNo;
	m_FillMode = other.m_FillMode;
	pSourceCurve = other.pSourceCurve;
	pTargetCurve = other.pTargetCurve;
	m_SourceCurveName = other.m_SourceCurveName;
	m_TargetCurveName = other.m_TargetCurveName;
	m_LthCurveName = other.m_LthCurveName;
	m_Text = other.m_Text;
}

void CCurveFill::Serialize( CArchive& ar )
{
	if( ar.IsStoring() )
	{
		ar << (LONG)m_Color;
		ar << m_LthNo;
		ar << m_FillMode;
		ar << m_Text;
		ar << m_LthCurveName;
		ar << m_SourceCurveName;
		ar << m_TargetCurveName;
	}
	else
	{
		long l;
		ar >> l; m_Color = (COLORREF)l;
		ar >> m_LthNo;
		ar >> m_FillMode;
		ar >> m_Text;
		ar >> m_LthCurveName;
		ar >> m_SourceCurveName;
		ar >> m_TargetCurveName;
		if (m_TargetCurveName == "左边界") m_TargetCurveName = "左边界";
		if (m_TargetCurveName == "右边界") m_TargetCurveName = "右边界";
	}
}

CScripPoint::CScripPoint(CPoint Point,float Dep,CString Content,COLORREF Color)
{
	m_Dep=Dep;
	m_Content=Content;
	m_Point = Point;
	m_Color=Color;
}

void CScripPoint::Draw(CDC* pDC,CLogPlotDoc* pDoc)
{
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
		"Courier New"               //"MS Serif" "Times New Roman"
	);
	CFont *pOldFont = pDC->SelectObject(&hTextFont);
	pDC->SetTextColor(m_Color);
	pDC->SetTextAlign(TA_LEFT |TA_TOP);//TA_CENTER
	pDC->SetBkMode(TRANSPARENT);
	CPoint pt=m_Point;
	pDoc->LPtoPP(&pt,1);
	pDC->TextOut(pt.x,pt.y,m_Content);	
	pDC->SelectObject(pOldFont);
}

IMPLEMENT_SERIAL(CCurveObj, CFIDDrawObj, 0)
        
CCurveObj::CCurveObj()
{
	m_Point = NULL;
	m_Value = NULL;
	m_bHeadShowFill = TRUE;
	m_FilterNum = 0;
	pFillCurveObj = NULL;
	m_DrawShape = curveobj;
	m_bDataEdit = FALSE;
	m_LinePen.CreateLinePen(m_CurveStyleNo,m_CurveWidth,m_CurveColor);
	bErrorLthCurve = FALSE;
	m_bScripFlush=TRUE;
}
    
CCurveObj::CCurveObj(CString szPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDoc)
			:CFIDDrawObj(szPathName,rect,pParentObj,pDoc)
{
	m_Point = NULL;
	m_Value = NULL;
	m_bHeadShowFill = TRUE;	
	m_FilterNum = 0;
	m_bDataEdit = FALSE;
	m_Factor = 0; m_CalType = 0;
	m_CurveColor = RGB(0,0,0xff);
	m_CurveStyleNo = 1;
	m_CurveWidth = 2;
	pFillCurveObj = NULL;
	m_DrawShape = curveobj;
	m_ScaleFont = m_NameFont; 
	m_ScaleFont.lfHeight = -10;
	m_ScaleFont.lfWeight = 300;
	bErrorLthCurve = FALSE;
	m_LinePen.CreateLinePen(m_CurveStyleNo,m_CurveWidth,m_CurveColor);
	m_bScripFlush=TRUE;
}


CCurveObj::~CCurveObj()
{
	int k = (int)m_FillList.GetSize();
	if (k>0)
	{
		for (int i=0; i<k; i++)
			delete m_FillList[i];
		m_FillList.RemoveAll();
	}
	if (m_Point != NULL) delete m_Point;
	if (m_Value != NULL) delete m_Value;

	POSITION pos = m_ScripPoint.GetHeadPosition();
	while (pos != NULL) 
		delete m_ScripPoint.GetNext(pos);
	m_ScripPoint.RemoveAll();
}    

void CCurveObj::Serialize( CArchive& ar )
{
	long count;
	CFIDDrawObj::Serialize( ar );
	if(ar.IsStoring() )
	{
		count =(long) m_FillList.GetSize();
		ar << count;
		for (int i=0; i<count; i++)
		{
			m_FillList[i]->Serialize(ar);
		}
		ar << m_bHeadShowFill;
		ar << m_FilterNum;
		ar << m_bDataEdit;
		ar << m_CalType;
		ar << m_Factor;
		ar << m_CurveStyleNo;
		ar << m_CurveWidth;
		ar << m_CurveColor;
		SerializeLogFont(ar,m_ScaleFont);
	}
	else
	{
		pDoc = (CLogPlotDoc*)ar.m_pDocument;
		ar >> count;
		for (int i=0; i<count; i++)
		{
			CCurveFill* pCurveFill = new CCurveFill;
			pCurveFill->Serialize( ar );
			m_FillList.Add(pCurveFill);
		}
		ar >> m_bHeadShowFill;
		ar >> m_FilterNum;
		ar >> m_bDataEdit;
		ar >> m_CalType;
		ar >> m_Factor;
		ar >> m_CurveStyleNo;
		ar >> m_CurveWidth;
		ar >> m_CurveColor;
		SerializeLogFont(ar,m_ScaleFont);
		m_LinePen.CreateLinePen(m_CurveStyleNo,m_CurveWidth,m_CurveColor);
	}
}

void CCurveObj::CreatScripPoint()
{
	POSITION pos = m_ScripPoint.GetHeadPosition();
	while (pos != NULL) 
		delete m_ScripPoint.GetNext(pos);
	m_ScripPoint.RemoveAll();
	m_bScripFlush=FALSE;
	if(m_Factor==4)
	{
		int idep=(int)m_PlotSdep;
		if(idep<m_PlotSdep) idep++;
		idep+=2;
		int irlev=m_CurveWidth;
		if(irlev<1) irlev=1;
		CGeoFIDIO* pFIDIO = new CGeoFIDIO(m_strPathName);
		if(pFIDIO->Open(pDoc->m_szSever))
		{
			float sdep= pFIDIO->GetStartDepth();
			float edep = pFIDIO->GetEndDepth();
			if(sdep>idep) idep=(int)sdep;
			if(edep<idep) idep=(int)edep;
			float Value;
			CString content;CPoint point;
			CRect rect = m_Rect;
			pDoc->LPtoPP(&rect);
			while(idep<m_PlotEdep) 
			{
				point.y=DepthToYCoor(idep);
				if(point.y<rect.top +10||point.y>rect.bottom-10) 
				{
					idep+=irlev;
					continue;
				}
				point.x=rect.left+5;
				pFIDIO->SeekToDepth((float)idep);
				Value=-999.25f;
				pFIDIO->Read(2,1,&Value);
				if(Value!=-999.25f&&Value!=-9999.f&&Value!=-99999.f)
				{
					content.Format ("%g",Value);
					CScripPoint* pObj;
					pObj = new CScripPoint(point,(float)idep,content,m_CurveColor);
					m_ScripPoint.AddTail(pObj);
				}
				idep+=irlev;
			}
		}
		delete(pFIDIO);
		return;
	}
	int xyz=m_ScripFileName.Find (".List");
	if((xyz>0)&&DetectFileExist(m_ScripFileName))
	{
		TCHAR *token, *end;
		TCHAR buffer[1024];
		CStdioFile File;
		if (!File.Open(m_ScripFileName, CFile::modeRead | CFile::typeText)) 
			return;
		int i;
		float tmp;
		if (m_CalType == 1) 
		{
			ASSERT(m_LeftVal >0.000000000000001f);
			ASSERT(m_RightVal >0.000000000000001f);
			tmp = (float) (m_Rect.Width() / (log10(m_RightVal)-log10(m_LeftVal)));
		}
		else 
			tmp = (float)(m_Rect.Width() / (m_RightVal - m_LeftVal)); 

		CPoint point;		
		CString str;
		CString content;
		float dep,value;
		// Read Header off file
		File.ReadString(buffer, 1024);		
		while (File.ReadString(buffer, 1024)) 
		{
			// Get first token
			for (token=buffer, end=buffer; 
				 *end && (*end != _T(',')) && (*end != _T('\n')); end++);
			if ((*end == _T('\0')) && (token == end)) token = NULL;
			*end = _T('\0');				
			i=0;
			while (token) 
			{
				str=token;
				str.TrimLeft ();str.TrimRight ();
				if(i==1)
					dep=(float)atof(str);
				else if(i==2)
					value=(float)atof(str);
				else if(i==3)
					content=str;
				// Get next token
				for (token=++end; *end && (*end != _T(',')) && (*end != _T('\n'));
					 end++);
				if ((*end == _T('\0')) && (token == end)) token = NULL;
				*end = _T('\0');
				i++;
			}
			if((dep>m_PlotSdep)&&(dep<m_PlotEdep))
			{
				point.y=DepthToYCoor(dep);
				if (m_CalType == 1) 
				{
					if (value<0.000000000000001f) 
						value =0.000000000000001f;
					point.x=m_Rect.left + Round((log10(value)-log10(m_LeftVal))*tmp);
				}
				else
					point.x=m_Rect.left + Round((value - m_LeftVal)*tmp);
				CScripPoint* pObj;
				pObj = new CScripPoint(point,dep,content,0x0);
				m_ScripPoint.AddTail(pObj);
			}
		}
		File.Close();
	}
}

BOOL CCurveObj::Properties(CView* pView)
{
	CString szHT;
	CPropertySheet m_Sheet("曲线属性");
	CCurveBasePage m_BasePage(this);
	CCurvePage m_Page(this);
	CCurveFillPage m_Fill(this);
	m_Sheet.AddPage(&m_BasePage);
	m_Sheet.AddPage(&m_Page);
	if(m_ParentType==0)
		m_Sheet.AddPage(&m_Fill);

	CMinFontPage m_FontPage;
	TEXTFONT m_TextFont[2];
	szHT.LoadString(IDS_ALIASFONT);
	m_TextFont[0].m_ItemName = szHT.GetBuffer(128);
	szHT.ReleaseBuffer();
	m_TextFont[0].m_Text = m_AliasName;
	m_TextFont[0].m_ForeGround = m_NameColor;
	m_TextFont[0].m_BackGround = RGB(255,255,255);
	m_TextFont[0].lf = m_NameFont;
	m_FontPage.AddFont(&m_TextFont[0]);
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

	if(m_Sheet.DoModal() == IDOK)
	{
		m_FontPage.OnOK();
		m_NameFont = m_TextFont[0].lf;
		m_ScaleFont = m_TextFont[1].lf;
		m_NameFont.lfEscapement = 0;
		m_ScaleFont.lfEscapement = 0;
		bFlush = TRUE;
		m_LinePen.CreateLinePen(m_CurveStyleNo,m_CurveWidth,m_CurveColor);
		CGeoFIDIO* pFIDIO = new CGeoFIDIO(m_strPathName);
		m_Rlev = m_SDep = m_EDep = 0;
		CRect m_SaveRect = m_Rect;
		m_SaveRect.top = m_HeadRect.top;
		bErrorFID = TRUE;
		bErrorLthCurve = FALSE;
		if(pFIDIO->Open(pDoc->m_szSever)&& IsDepthUnitOK(pFIDIO))
		{
			m_Rlev = pFIDIO->GetDepthLevel();
			if(m_Rlev > 0.001)
			{
				m_SDep = pFIDIO->GetStartDepth();
				m_EDep = pFIDIO->GetEndDepth();
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
				bErrorFID = FALSE;
			}
		}
		if(bErrorFID)
		{
			m_SDep = m_EDep = m_SSdep = m_EEdep = 0;
			bFlush = FALSE;
		}
		delete pFIDIO;
		m_bScripFlush=TRUE;
		pDoc->SetModifiedFlag();
		InvalObj(pView);
		return TRUE;
	}
	return FALSE;
}

void CCurveObj::Line(int x1, int y1,int x2, int y2, CDC* pDC)
{     
	if((x1<-9000)||(x2<-9000))
	{
		pDC->MoveTo(x2,y2);
		return;
	}
	pDC->MoveTo(x1,y1);
	pDC->LineTo (x2,y2);
}

void CCurveObj::DLine(int x1, int y1,int x2, int y2, LINE* fpLine)
{    
	if((x1<-9000)||(x2<-9000))
	{
		pt[0].x = x2; pt[0].y = y2;
		flag = 1;
		m_LinePen.org = 0;
		return;
	} 	      				
	LineDDA(x1,y1,x2,y2,(LINEDDAPROC)LineProc,(LPARAM)fpLine);
	pt[0].x = x2; pt[0].y = y2;
}

void CCurveObj::SetClipRect(CDC* pDC , CRect rect)
{
	if (pDC->m_hDC != NULL) ::LPtoDP(pDC->m_hDC,(LPPOINT)(LPRECT)rect,2);
	CRgn rgn; rgn.CreateRectRgn(rect.left,rect.top,rect.right,rect.bottom);
	pDC->SelectClipRgn(&rgn);
}

void CCurveObj::AntiDLine(int x1, int y1,int x2, int y2, LINE* fpLine)
{    
	if((x1<-9000)||(x2<-9000))
	{
		pt[0].x = x2; pt[0].y = y2;
		flag = 1;
		m_LinePen.org = 0;
		return;
	} 	      				
	LineDDA(x1,y1,x2,y2,(LINEDDAPROC)AntiLineProc,(LPARAM)fpLine);
	pt[0].x = x2; pt[0].y = y2;
}

void CCurveObj::AntiDLine100(int x1, int y1,int x2, int y2, LINE* fpLine)
{    
	if((x1<-9000)||(x2<-9000))
	{
		pt[0].x = x2; pt[0].y = y2;
		flag = 1;
		m_LinePen.org = 0;
		return;
	} 	      				
	LineDDA(x1,y1,x2,y2,(LINEDDAPROC)AntiLineProc100,(LPARAM)fpLine);
	pt[0].x = x2; pt[0].y = y2;
}

void CCurveObj::DrawHead(CDC *pDC)
{
	CRect rect = m_HeadRect;
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;
	char ss[10];      
	int k = (int)m_FillList.GetSize();
	if((k > 0) && (m_bHeadShowFill))
	{
		CRect m_FillRect = rect;	
		CFont m_Font;
		m_Font.CreateFontIndirect(&m_NameFont);
	    CFont* pOldFont = pDC->SelectObject(&m_Font);
		CCurveFill* pFill = m_FillList[0];
		if(pDoc->m_FillLithGround&&(pFill->m_LthNo!=1))
			LithFillBox(m_FillRect, pFill->m_LthNo,m_CurveColor,m_FillRect.TopLeft(),pDC,pFill->m_Color);
		else
			LithFillBox(m_FillRect, pFill->m_LthNo,pFill->m_Color,m_FillRect.TopLeft(),pDC);
		CPen m_Pen;
		m_Pen.CreatePen(0,1,m_CurveColor);
		CPen* pOldPen = pDC->SelectObject(&m_Pen);
		if(!(pFill->m_Text.IsEmpty()))
		{
			CRect rect;
		    CSize textsize = pDC->GetTextExtent(pFill->m_Text);
			rect.top = m_FillRect.top + (m_FillRect.Height() - textsize.cy) / 2 - 3;
			rect.bottom = m_FillRect.bottom - (m_FillRect.Height() - textsize.cy) / 2+3;
			rect.left = m_FillRect.left + (m_FillRect.Width() - textsize.cx) / 2 - 10;
			rect.right = m_FillRect.right - (m_FillRect.Width() - textsize.cx) / 2 + 10;
			CBrush TextBoxBrush;
			TextBoxBrush.CreateSolidBrush(RGB(255,255,255));
			CBrush* pOldBrush=pDC->SelectObject (&TextBoxBrush);
			pDC->RoundRect (rect.left,rect.top ,rect.right ,rect.bottom ,7,7);
			pDC->SelectObject (pOldBrush);
			pDC->SetTextColor(m_CurveColor);  
	    	pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
			pDC->TextOut((rect.left+rect.right)/2+1, (rect.top+rect.bottom+textsize.cy)/2, pFill->m_Text);
		}		
		pDC->MoveTo(m_FillRect.left,m_FillRect.top);
		pDC->LineTo(m_FillRect.left,m_FillRect.bottom);
		pDC->MoveTo(m_FillRect.right,m_FillRect.top);
		pDC->LineTo(m_FillRect.right,m_FillRect.bottom);

		pDC->MoveTo(rect.left,m_FillRect.top);
		pDC->LineTo(rect.right,m_FillRect.top);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldFont);
		return;
	}
	CFont* pOldFont;
	CFont m_Font;
	m_Font.CreateFontIndirect(&m_ScaleFont);
	pOldFont = pDC->SelectObject(&m_Font);
	CSize textsize;
	textsize = pDC->GetTextExtent("国"); 
	pDC->SetTextColor(m_CurveColor);
	pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
	if(m_LeftVal != m_RightVal)
	{
		pDC->TextOut((rect.left + rect.right)/2, rect.bottom - 7, m_UnitText, (int)strlen(m_UnitText));
		if(m_Factor!=4)
		{
			pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
			dtos(m_LeftVal,ss);
			pDC->TextOut(rect.left+2,rect.bottom-7,ss,(int)strlen(ss));
			dtos(m_RightVal,ss);
			pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
			pDC->TextOut(rect.right-2,rect.bottom-7,ss,(int)strlen(ss));
			pDC->SelectObject(pOldFont);
		}
	}
	pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
	CFont m_Font1;
	m_Font1.CreateFontIndirect(&m_NameFont);
    pDC->SelectObject(&m_Font1);
    pDC->TextOut((rect.left + rect.right)/2, rect.bottom - textsize.cy-10, m_AliasName,(int)strlen(m_AliasName));
	pDC->SelectObject(pOldFont);

	if(!IsGridCurve()) 
		DrawHorLine(rect.left,rect.right,rect.bottom - 5,&m_LinePen,pDC);
}

int CCurveObj::GetPoint(double& sdep,double& edep, CPoint* &pt)
{
	if (bErrorFID) return 0;
	if (bFlush)
	{
		CGeoFIDIO m_FIDIO(m_strPathName);
		bErrorFID = TRUE;
		if(m_FIDIO.Open(pDoc->m_szSever))
		{
			if(IsDepthUnitOK(&m_FIDIO))
			{
				m_Rlev = m_FIDIO.GetDepthLevel();
				if(m_Rlev>0.0001) 
				{
					m_SSdep = m_FIDIO.GetStartDepth();
					m_EEdep = m_FIDIO.GetEndDepth();
					bErrorFID = FALSE;
				}
			}
			m_FIDIO.Close();
		}
	}
	if(bErrorFID) 
	{
		m_SDep = m_EDep = m_SSdep = m_EEdep = 0;
		bFlush = FALSE;
		return 0;
	}
	double eps = fabs(m_Rlev/4.);
	ASSERT(sdep <= edep);
	if ((sdep > m_EEdep) || (edep < m_SSdep)) 
		if (!bFlush) return 0;
	else
	{
		if (sdep < m_SSdep) sdep = m_SSdep;
		if (edep > m_EEdep) edep = m_EEdep;
	}
	if (bFlush || (sdep+eps <m_SDep) || (edep-eps > m_EDep))
	{
		double dep1 = sdep - nBufferPointNum*m_Rlev;
		if (dep1 < m_PlotSdep) dep1 = m_PlotSdep;
		double dep2 = edep + nBufferPointNum*m_Rlev;
		if (dep2 > m_PlotEdep) dep2 = m_PlotEdep;
		if (dep1 > dep2) return 0;		
		CreatePoint(dep1,dep2);
	}
	if (m_PointNum == 0) return 0;
	if ((sdep > m_EDep) || (edep < m_SDep)) return 0;

	if (sdep < m_SDep) sdep = m_SDep;
	if (edep > m_EDep) edep = m_EDep;

	int k = Round((sdep - m_SDep) / m_Rlev);
	int num = Round((edep - sdep) / m_Rlev )+ 1;
	pt = m_Point + k;		
	if (num+k > m_PointNum) 
		num = m_PointNum-k;
	
	return num;
}

int CCurveObj::GetValue(double& sdep,double& edep, float*& value)
{
	if (bErrorFID) return 0;
	if (bFlush)
	{
		bErrorFID = TRUE;
		CGeoFIDIO m_FIDIO(m_strPathName);
		if (m_FIDIO.Open(pDoc->m_szSever)&& IsDepthUnitOK(&m_FIDIO))
		{
			m_Rlev = m_FIDIO.GetDepthLevel();
			if (m_Rlev > 0.001) 
			{
				m_SSdep = m_FIDIO.GetStartDepth();
				m_EEdep = m_FIDIO.GetEndDepth();
				bErrorFID = FALSE;
			}
		}
	}
	if (bErrorFID) 
	{
		m_SDep = m_EDep = m_SSdep = m_EEdep = 0;
		bFlush = FALSE;
		return 0;
	}
	double eps = fabs(m_Rlev/4);
	ASSERT(sdep <= edep);
	if ((sdep > m_EEdep) || (edep < m_SSdep)) 
		if (!bFlush) return 0;
	else
	{
		if (sdep < m_SSdep) sdep = m_SSdep;
		if (edep > m_EEdep) edep = m_EEdep;
	}

	if (bFlush || (sdep+eps <m_SDep) || (edep-eps > m_EDep))
	{
		double dep1 = sdep - nBufferPointNum*m_Rlev;
		if (dep1 < m_PlotSdep) dep1 = m_PlotSdep;
		double dep2 = edep + nBufferPointNum*m_Rlev;
		if (dep2 > m_PlotEdep) dep2 = m_PlotEdep;
		if (dep1 > dep2) return 0;
		CreatePoint(dep1,dep2);
	}
	if (m_PointNum == 0) return 0;
	if ((sdep > m_EDep) || (edep < m_SDep)) return 0;

	if (sdep < m_SDep) sdep = m_SDep;
	if (edep > m_EDep) edep = m_EDep;

	int k = Round((sdep - m_SDep) / m_Rlev);
	int num = Round((edep - sdep) / m_Rlev )+ 1;
	value= m_Value + k;		
	if (num+k > m_PointNum) 
		num = m_PointNum-k;
	return num;
}

int CCurveObj::CreatePoint(double& sdep, double& edep)
{
	if (m_Point != NULL) delete m_Point;
	m_Point = NULL;
	if (m_Value != NULL) delete m_Value;
	m_Value = NULL;
	m_EDep = edep;
	m_SDep = sdep;
	m_PointNum = 0;
	if (bErrorFID) return 0;
	bErrorFID = TRUE;
	CGeoFIDIO* pFIDIO = new CGeoFIDIO(m_strPathName);
	if(pFIDIO->Open(pDoc->m_szSever) && IsDepthUnitOK(pFIDIO))
	{
		m_Rlev = pFIDIO->GetDepthLevel();
		if (m_Rlev > 0.001) 
			bErrorFID = FALSE;
	}
	if(bErrorFID) 
	{
		m_SDep = m_EDep = m_SSdep = m_EEdep = 0;
		goto rtn;
	}
	m_SSdep = pFIDIO->GetStartDepth();
	m_EEdep = pFIDIO->GetEndDepth();

	if ((m_SDep > m_EEdep) || (m_EDep < m_SSdep)) goto rtn;
	if (m_SDep < m_SSdep) m_SDep = m_SSdep;
	if (m_EDep > m_EEdep) m_EDep = m_EEdep;
	m_PointNum = (int)((m_EDep - m_SDep) / m_Rlev) + 1 ;
	if (m_PointNum < 0) m_PointNum = 0;
	m_xMin = pDoc->GetRect().left;
	m_xMax = pDoc->GetRect().right;
	if (m_PointNum > 0)
	{
		float tmp;int i;
		m_Value = new float[m_PointNum];
		memset(m_Value,-9999,m_PointNum);	
		m_Point = new CPoint[m_PointNum];
		pFIDIO->SeekToDepth((float)m_SDep);     
		m_PointNum = (int)pFIDIO->Read(2,(unsigned int )m_PointNum,m_Value);
		
		int n;
		for(i=0;i<m_FilterNum;i++)
		{
			n=1;
			while(n<m_PointNum-2)
			{
				if((m_Value[n-1]!=-9999.f)
					&&(m_Value[n]!=-9999.f)
					&&(m_Value[n+1]!=-9999.f)
					&&(m_Value[n-1]!=-99999.f)
					&&(m_Value[n]!=-99999.f)
					&&(m_Value[n+1]!=-99999.f)
					&&(m_Value[n-1]!=-999.25f)
					&&(m_Value[n]!=-999.25f)
					&&(m_Value[n+1]!=-999.25f))	
				{
					//三点钟形滤波         0.212Ti-1+0.576Ti+0.212Ti+1
					m_Value[n]=0.212f*m_Value[n-1]+0.576f*m_Value[n]+0.212f*m_Value[n+1];
				}
				n++;
			}
		}

		m_EDep = m_SDep + (m_PointNum-1) * m_Rlev; 
		if(m_CalType == 1) 
		{
			ASSERT(m_LeftVal >0.000000000000001f);
			ASSERT(m_RightVal >0.000000000000001f);
			tmp = (float) (m_Rect.Width() / (log10(m_RightVal)-log10(m_LeftVal)));
		}
		else tmp = (float)(m_Rect.Width() / (m_RightVal - m_LeftVal)); 
		int width = m_Rect.Width();
		double yy0=0;
		long XXX;
		for (i=0; i<m_PointNum; i++)
		{   
			m_Point[i].y = DepthToYCoor(m_SDep +i*m_Rlev); 
			if ((m_Value[i]==-9999.f)
				||(m_Value[i]==-99999.f)
				||(m_Value[i]==-999.25f))
			{
				if(m_bDataEdit)
					m_Point[i].x=-9999;
				else
				{
					if(m_LeftVal<m_RightVal)
						m_Point[i].x=m_Rect.left;
					else
						m_Point[i].x=m_Rect.right;
				}
				continue;
			}
			if (m_Factor<0)
			{
				if (m_Point[i].x > m_xMax) 
					m_Point[i].x = m_xMax;
				if (m_Point[i].x < m_xMin) 
					m_Point[i].x = m_xMin;
			}
			else if (m_Factor == 0)
			{
				if (m_LeftVal > m_RightVal) 
				{
					if (m_Value[i] > m_LeftVal) 
						m_Value[i]  = m_LeftVal;
					else if (m_Value[i] < m_RightVal) 
						m_Value[i]  = m_RightVal;
				}
				else  
				{
					if (m_Value[i] < m_LeftVal) 
						m_Value[i]  = m_LeftVal;
					else if (m_Value[i] > m_RightVal) 
						m_Value[i]  = m_RightVal;
				}        
			}
			if(m_CalType == 1) 
			{
				if(m_Value[i]<0.000000000000001f) 
				{
					if (i == 0) 
						m_Value[i] = 0.000000000000001f;
					else 
						m_Value[i] = m_Value[i-1];
				}
				XXX=m_Rect.left + Round((log10(m_Value[i])-log10(m_LeftVal))*tmp);
				if(XXX>72760) XXX=72760;
				if(XXX<-9999) XXX=-9999;
				m_Point[i].x = XXX;
			}
			else
			{
				XXX=m_Rect.left + Round((m_Value[i] - m_LeftVal)*tmp);			
				if(XXX>72760) XXX=72760;
				if(XXX<-9999) XXX=-9999;
				m_Point[i].x = XXX;
			}
		}
	}
rtn:
	delete(pFIDIO);    
	bFlush = FALSE;
	edep = m_EDep;
	sdep = m_SDep;
	return m_PointNum;
}

int CCurveObj::GetGridValue()
{
	char ext[128]; 
	CGeoFIDIO::GetFIDName(m_strPathName,ext); 
	int v = atoi(ext);
	if (v>100) v = 100;
	if (v < 0) v = 0;
	return m_Rect.left + m_Rect.Width() * v / 100;
}

BOOL CCurveObj::IsGridCurve()
{
	char ext[128]; 
	CGeoFIDIO::GetFIDName(m_strPathName,ext); 
	int len = (int)strlen(ext);
	if ((len > 3) || (len < 1))return FALSE;
	for (int i=0; i<len; i++)
		if (!isdigit(ext[i])) return FALSE;
	int v = atoi(ext);
	if (v>100) return FALSE;
	return TRUE;
}

void CCurveObj::DrawFill(CView* pView, CDC* pDC)
{
	if (IsGridCurve()) 
		bErrorFID = FALSE;
	if (bErrorFID) return;
	int i,k=(int)m_FillList.GetSize();
	if(k<1) return;
	CRect rect = m_Rect;
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;		
	CRect m_ClipRect;
	pDC->GetClipBox(m_ClipRect);
	m_ClipRect &= rect;
	double sdep;
	double edep;
	sdep = YCoorToDepth(m_ClipRect.top + pDoc->m_WinOrg.y) - m_Rlev;
	edep = YCoorToDepth(m_ClipRect.bottom  + pDoc->m_WinOrg.y) + m_Rlev;
	if (sdep < m_PlotSdep) sdep = m_PlotSdep;
	if (edep > m_PlotEdep) edep = m_PlotEdep;
	CPoint* point;
	int PointNum;
	CCurveFill* pFill = m_FillList[0];
	////////////////////////////////////////
	PointNum = GetPoint(sdep,edep,point);
	CPoint BrushOrg(0,0);
	CRect Rect=m_Rect;
	pDoc->LPtoPP(Rect);
	if (PointNum > 1)
	{ 
		pDoc->LPtoPP(point,PointNum);
		for(i=0;i<PointNum;i++)
		{
			if(point[i].x<Rect.left) 
				point[i].x=Rect.left;
			if(point[i].x>Rect.right) 
				point[i].x=Rect.right;
		}
		if (pFill->m_TargetCurveName == "左边界")
		{
			if(pDoc->m_FillLithGround&&(pFill->m_LthNo!=1))
				FillCurve(Rect.left,point,PointNum,pFill->m_LthNo,m_CurveColor,BrushOrg,pDC,FILLLEFTRIGHT,pFill->m_Color);
			else
				FillCurve(Rect.left,point,PointNum,pFill->m_LthNo,pFill->m_Color,BrushOrg,pDC,FILLLEFTRIGHT);
		}
		else if (pFill->m_TargetCurveName == "右边界")
		{
			if(pDoc->m_FillLithGround&&(pFill->m_LthNo!=1))
				FillCurve(Rect.right,point,PointNum,pFill->m_LthNo,m_CurveColor,BrushOrg,pDC,FILLLEFTRIGHT,pFill->m_Color);
			else
				FillCurve(Rect.right,point,PointNum,pFill->m_LthNo,pFill->m_Color,BrushOrg,pDC,FILLLEFTRIGHT);
		}
		else if (pFill->m_TargetCurveName == m_AliasName)
		{
			CPoint* point2 = new CPoint[PointNum];
			for(i=0;i<PointNum;i++)
			{
				point2[i].x = Rect.left + Rect.right - point[i].x;
				point2[i].y = point[i].y;
			}
			if(pFill->m_FillMode==2)
			{
				if(pDoc->m_FillLithGround&&(pFill->m_LthNo!=1))
					FillCurve(point,point2,PointNum,pFill->m_LthNo,m_CurveColor,BrushOrg,pDC,pFill->m_FillMode,pFill->m_Color);
				else
					FillCurve(point,point2,PointNum,pFill->m_LthNo,pFill->m_Color,BrushOrg,pDC,pFill->m_FillMode);
			}
			else
			{
				if(pDoc->m_FillLithGround&&(pFill->m_LthNo!=1))
					FillCurve(Rect.left,point,PointNum,pFill->m_LthNo,m_CurveColor,BrushOrg,pDC,FILLLEFTRIGHT,pFill->m_Color);
				else
					FillCurve(Rect.left,point,PointNum,pFill->m_LthNo,pFill->m_Color,BrushOrg,pDC,FILLLEFTRIGHT);

				if(pDoc->m_FillLithGround&&(pFill->m_LthNo!=1))
					FillCurve(Rect.right,point2,PointNum,pFill->m_LthNo,m_CurveColor,BrushOrg,pDC,FILLLEFTRIGHT,pFill->m_Color);
				else
					FillCurve(Rect.right,point2,PointNum,pFill->m_LthNo,pFill->m_Color,BrushOrg,pDC,FILLLEFTRIGHT);
			}
			delete point2;
		}
		else 
		{
			if (pFill->pTargetCurve == NULL)
				pFill->pTargetCurve = ((CTrackObj*)m_pParentObj)->FindAliasCurve(pFill->m_TargetCurveName);
			if (pFill->pTargetCurve == NULL) return;
			if (pFill->pTargetCurve->IsGridCurve())
			{
				int x = pFill->pTargetCurve->GetGridValue();
				int m_FillMode = pFill->m_FillMode;
				if (m_FillMode == FILLLEFT) m_FillMode = FILLRIGHT;
				else if (m_FillMode == FILLRIGHT) m_FillMode = FILLLEFT;
				if(pDoc->m_FillLithGround&&(pFill->m_LthNo!=1))
					FillCurve(x,point,PointNum,pFill->m_LthNo,m_CurveColor,BrushOrg,pDC,m_FillMode,pFill->m_Color);
				else
					FillCurve(x,point,PointNum,pFill->m_LthNo,pFill->m_Color,BrushOrg,pDC,m_FillMode);
			}
			else
			{
				CPoint* point1=NULL;
				int PointNum1 = pFill->pTargetCurve->GetPoint(sdep,edep,point1);
				if(PointNum1>1)
				{
					pDoc->LPtoPP(point1,PointNum1);
					CPoint* point2=new CPoint[PointNum];
					for(i=0;i<PointNum;i++)
					{
						k=(int)(i*m_Rlev/pFill->pTargetCurve->m_Rlev);
						if(k>PointNum1-1) k=PointNum1-1;
						point2[i].y=point[i].y;
						point2[i].x=point1[k].x;
						if(point2[i].x<Rect.left) 
							point2[i].x=Rect.left;
						if(point2[i].x>Rect.right) 
							point2[i].x=Rect.right;
					}
					if(pDoc->m_FillLithGround&&(pFill->m_LthNo!=1))
						FillCurve(point,point2,PointNum,pFill->m_LthNo,m_CurveColor,BrushOrg,pDC,pFill->m_FillMode,pFill->m_Color);
					else
						FillCurve(point,point2,PointNum,pFill->m_LthNo,pFill->m_Color,BrushOrg,pDC,pFill->m_FillMode);
					delete point2;
					pDoc->PPtoLP(point1,PointNum1);
				}	
			}
		}
		pDoc->PPtoLP(point,PointNum);
	}
}

void CCurveObj::DrawClient(CDC *pDC)
{                     
	if (IsGridCurve()) bErrorFID = FALSE;
	CRect rect = m_Rect;
	pDoc->LPtoPP(&rect);
	if(!pDC->RectVisible(rect)) return;
	if((m_pParentObj == NULL)||(m_ParentType != 0)) 
		CFIDDrawObj::DrawClient(pDC);
	if(bErrorFID) return;	
	CRect m_ClipRect;
	pDC->GetClipBox(m_ClipRect);
	CRect rc=m_ClipRect;
	m_ClipRect &= rect;
	if(IsGridCurve())
	{
		int x = GetGridValue();
		LINE m_Line;
		m_Line.pPen = &m_LinePen;
		flag = 1;
		m_LinePen.org = 0;
		m_Line.pDC = pDC;
		DLine(x,m_ClipRect.top,x,m_ClipRect.bottom,&m_Line);
		return;
	}
	if(m_Factor==4)
	{
		if(m_bScripFlush) 
			CreatScripPoint();
		CScripPoint* pObj;
		POSITION pos = m_ScripPoint.GetHeadPosition();
		while(pos != NULL)
		{
			pObj = (CScripPoint*) m_ScripPoint.GetNext(pos);
			CPoint pt = pObj->m_Point;
			pDoc->LPtoPP(&pt,1);
			if(m_ClipRect.PtInRect(pt))
				pObj->Draw(pDC,pDoc);
		}
		return;
	}
	double sdep;
	double edep;
	sdep = YCoorToDepth(m_ClipRect.top + pDoc->m_WinOrg.y) - m_Rlev;
	edep = YCoorToDepth(m_ClipRect.bottom  + pDoc->m_WinOrg.y) + m_Rlev;
	if (sdep < m_PlotSdep) sdep = m_PlotSdep;
	if (edep > m_PlotEdep) edep = m_PlotEdep;	
	CPoint* point;
	int PointNum = 0;int i;
	CRect ObjRect=rect;
	if(m_Factor==2)
	{
		if(GetParentType()==0)
		{
			CDrawObj* pObj=GetParentObj ()->GetParentObj();
			if(pObj!=NULL)
			{
				ObjRect=pObj->m_Rect;
				pDoc->LPtoPP(&ObjRect);
			}
		}
	}
	int ww=ObjRect.Width();		
	int Factor=m_Factor;
	m_Factor=1;
	PointNum = GetPoint(sdep,edep,point);
	if(PointNum>2) 
	{ 
		pDoc->LPtoPP(point,PointNum);                
		CPoint* point2 = NULL;
		for(i=0; i<m_FillList.GetSize(); i++)
		{
			if (m_FillList[i]->m_TargetCurveName == m_AliasName)
			{
				point2 = new CPoint[PointNum];
				for (i=0; i<PointNum; i++)
				{
					point2[i].x = m_Rect.left + m_Rect.right - point[i].x;
					point2[i].y = point[i].y;
				}
				break;
			}
		}
		CPen m_Pen;
		m_Pen.CreatePen(0,m_CurveWidth,m_CurveColor);
		CPen* pOldPen = pDC->SelectObject(&m_Pen); 
		LINE m_Line;
		m_Line.pPen = &m_LinePen;
		flag = 1;
		m_LinePen.org = 0;
		m_Line.pDC = pDC;
		int PN=m_Line.pPen->GetLineNo();
		if(PN>6&&PN<14)
		{			
			int r=0;
			CPen pr(0,m_LinePen.GetWidth(),m_CurveColor);
			CPen* pOldpr= pDC->SelectObject(&pr); 
			CBrush br;
			br.CreateSolidBrush(m_CurveColor);
			CBrush *pOldBr=(CBrush*)pDC->SelectStockObject(NULL_BRUSH);
			CPen pr1(0,1,m_CurveColor);
			switch(PN)
			{
				case 7:  //-----○
					for(i=0; i<PointNum; i++)
					{
						if((point[i].x==-9999)||(point[i].x<rect.left+1))
							continue;
						pDC->MoveTo(rect.left+1,point[i].y);
						pDC->LineTo(point[i].x,point[i].y);
						pDC->SelectObject(&pr1);
						pDC->Ellipse(point[i].x-m_CurveWidth-1,point[i].y-m_CurveWidth-1,
							point[i].x+m_CurveWidth+1,point[i].y+m_CurveWidth+1);
						pDC->SelectObject(&pr); 
					}
					break;
				case 8:  //--○--
					for (i=r; i<PointNum; i++)
					{				
						if(point[i].x==-9999)
							continue;
						{
							if(r>0)
								pDC->LineTo(point[i].x,point[i].y);
							pDC->MoveTo(point[i].x,point[i].y);
							r++;
						}
						pDC->SelectObject(&pr1);
						pDC->Ellipse(point[i].x-m_CurveWidth-1,point[i].y-m_CurveWidth-1,
							point[i].x+m_CurveWidth+1,point[i].y+m_CurveWidth+1);
						pDC->SelectObject(&pr); 
					}
					break;
				case 9:  //○-----
					for (i=0;i<PointNum; i++)
					{
						if((point[i].x==-9999)||(point[i].x>rect.right-1))
							continue;
						pDC->MoveTo(rect.right-1,point[i].y);
						pDC->LineTo(point[i].x,point[i].y);
						pDC->SelectObject(&pr1);
						pDC->Ellipse (point[i].x-m_CurveWidth-1,point[i].y-m_CurveWidth-1,
							point[i].x+m_CurveWidth+1,point[i].y+m_CurveWidth+1);
						pDC->SelectObject(&pr); 
					}
					break;
				case 10:  // ○○○○
					pDC->SelectObject(&pr1);
					for (i=0;i<PointNum; i++)
					{
						pDC->Ellipse (point[i].x-m_CurveWidth-1,point[i].y-m_CurveWidth-1,
							point[i].x+m_CurveWidth+1,point[i].y+m_CurveWidth+1);
					}
					pDC->SelectObject(&pr); 
					break;
				case 11:  // ●●●●
					pDC->SelectObject(&br);
					pDC->SelectObject(&pr1);
					for (i=0;i<PointNum; i++)
						pDC->Ellipse (point[i].x-m_CurveWidth-1,point[i].y-m_CurveWidth-1,
								point[i].x+m_CurveWidth+1,point[i].y+m_CurveWidth+1);
					pDC->SelectObject(&pr); 
					break;
				case 12:  // □□□□□
					pDC->SelectObject(&pr1);
					for (i=0;i<PointNum; i++)
						pDC->Rectangle(point[i].x-m_CurveWidth-1,point[i].y-m_CurveWidth-1,
								point[i].x+m_CurveWidth+1,point[i].y+m_CurveWidth+1);
					pDC->SelectObject(&pr); 
					break;
				case 13:  // ■■■■■ 
					pDC->SelectObject(&br);
					pDC->SelectObject(&pr1);
					for(i=0;i<PointNum; i++)
						pDC->Rectangle(point[i].x-m_CurveWidth-1,point[i].y-m_CurveWidth-1,
								point[i].x+m_CurveWidth+1,point[i].y+m_CurveWidth+1);
					pDC->SelectObject(&pr); 
					break;
				default:break;
			}
			pDC->SelectObject(pOldBr);
			pDC->SelectObject(pOldpr);
		}
		else
		{			
			if(Factor!=1)
				SetClipRect(pDC,ObjRect);
			
			int nSaveDC;
			if(pDoc->m_ScaleNo==7&&pDoc->m_bPrint&&m_FilterNum>=0)
			{
				nSaveDC = pDC->SaveDC();
				pDC->SetMapMode(MM_TEXT);
			}
			int Dn=0;	
			REAL dashvals[6][4]={{50.f,0.f,50.f,0.f},
								 {6.f,2.f,6.f,2.f},
								 {8.f,2.f,2.f,2.f},
								 {2.f,2.f,2.f,2.f},
								 {4.f,2.f,4.f,2.f},
								 {10.f,2.f,10.f,2.f}};	
			Graphics *pic=NULL;
			pic=new Graphics(pDC->m_hDC);
			pic->SetSmoothingMode(SmoothingModeHighQuality);
			Color CurveColor(GetRValue(m_Line.pPen->GetColor()),GetGValue(m_Line.pPen->GetColor()),GetBValue(m_Line.pPen->GetColor()));
			Pen *ppn= new Pen(CurveColor,(float)m_Line.pPen->GetWidth());
			ppn->SetDashPattern(dashvals[PN-1],4);
			Point *ppt=new Point[PointNum];

			if(m_FilterNum<0)//使用CDC绘图
			{
				if(Factor>1)
				{
					flag=1;
					for (i=1; i<PointNum; i++)
						DLine(point[i-1].x-ww,point[i-1].y,point[i].x-ww,point[i].y,&m_Line);
					flag=1;
					for (i=1; i<PointNum; i++)
						DLine(point[i-1].x+ww,point[i-1].y,point[i].x+ww,point[i].y,&m_Line);
				}
				flag=1;
				for (i=1; i<PointNum; i++)
					DLine(point[i-1].x,point[i-1].y,point[i].x,point[i].y,&m_Line);
				if(flag == 0) 
					pDC->LineTo(pt[0]);	
			}
			else//使用Graphics绘图
			{	
				if(Factor>1)//处理第二比例绘图
				{
					Dn=0;
					for(i=0; i<PointNum; i++)//处理左第二比例
					{
						if(point[i].x<-9000)//处理缺省值断开显示
						{
							pic->DrawLines(ppn,ppt,Dn);
							Dn=0;
						}
						else
						{
							ppt[Dn]=Point(point[i].x-ww,point[i].y);
							Dn++;
						}
					}
					if(Dn>0) 
						pic->DrawLines(ppn,ppt,Dn);
					Dn=0;
					for(i=0; i<PointNum; i++)//处理右第二比例
					{
						if(point[i].x<-9000)//处理缺省值断开显示
						{
							pic->DrawLines(ppn,ppt,Dn);
							Dn=0;
						}
						else
						{
							ppt[Dn]=Point(point[i].x+ww,point[i].y);
							Dn++;
						}
					}
					if(Dn>0) 
						pic->DrawLines(ppn,ppt,Dn);
				}
				Dn=0;
				for(i=0;i<PointNum;i++)
				{
					if(point[i].x<-9000)//处理缺省值断开显示
					{
						pic->DrawLines(ppn,ppt,Dn);
						Dn=0;
					}
					else
					{
						ppt[Dn]=Point(point[i].x,point[i].y);
						Dn++;
					}
				}
				if(Dn>0) 
					pic->DrawLines(ppn,ppt,Dn);
			}					
			if(point2!= NULL)
			{
				if(m_FilterNum<0)//使用CDC绘图
				{
					if(Factor>1)
					{
						flag=1;
						for (i=1;i<PointNum;i++)
							DLine(point2[i-1].x-ww,point2[i-1].y,point2[i].x-ww,point2[i].y,&m_Line);
						flag=1;
						for (i=1;i<PointNum;i++)
							DLine(point2[i-1].x+ww,point2[i-1].y,point2[i].x+ww,point2[i].y,&m_Line);
					}
					flag=1;
					for (i=1;i<PointNum;i++)
						DLine(point2[i-1].x,point2[i-1].y,point2[i].x,point2[i].y,&m_Line);
					if(flag == 0) 
						pDC->LineTo(pt[0]);	
				}
				else//使用Graphics绘图
				{
					if(Factor>1)//处理第二比例绘图
					{
						Dn=0;
						for(i=0; i<PointNum; i++)//处理左第二比例
						{
							if(point[i].x<-9000)//处理缺省值断开显示
							{
								pic->DrawLines(ppn,ppt,Dn);
								Dn=0;
							}
							else
							{
								ppt[Dn]=Point(point2[i].x-ww,point2[i].y);
								Dn++;
							}
						}
						if(Dn>0) 
							pic->DrawLines(ppn,ppt,Dn);
						Dn=0;
						for(i=0; i<PointNum; i++)//处理右第二比例
						{
							if(point[i].x<-9000)//处理缺省值断开显示
							{
								pic->DrawLines(ppn,ppt,Dn);
								Dn=0;
							}
							else
							{
								ppt[Dn]=Point(point2[i].x+ww,point2[i].y);
								Dn++;
							}
						}
						if(Dn>0) 
							pic->DrawLines(ppn,ppt,Dn);
					}
					Dn=0;
					for(i=0;i<PointNum;i++)
					{
						if(point[i].x<-9000)//处理缺省值断开显示
						{
							pic->DrawLines(ppn,ppt,Dn);
							Dn=0;
						}
						else
						{
							ppt[Dn]=Point(point2[i].x,point2[i].y);
							Dn++;
						}
					}
					if(Dn>0) 
						pic->DrawLines(ppn,ppt,Dn);
				}				
			}
			pic->ReleaseHDC(pDC->m_hDC);
			delete pic;delete ppn;delete ppt;

			if(pDoc->m_ScaleNo==7&&pDoc->m_bPrint&&m_FilterNum>=0)
				pDC->RestoreDC(nSaveDC);
			if(Factor!=1)
				SetClipRect(pDC,rc);	
		}
		pDC->SelectObject(pOldPen);
		if (point2 != NULL)	
			delete point2;
		pDoc->PPtoLP(point,PointNum);                
	}
	m_Factor=Factor;
	CScripPoint* pObj;
	POSITION pos = m_ScripPoint.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CScripPoint*) m_ScripPoint.GetNext(pos);
		CPoint pt = pObj->m_Point;
		pDoc->LPtoPP(&pt,1);
		if(m_ClipRect.PtInRect(pt))
			pObj->Draw(pDC,pDoc);
	}
}

BOOL CCurveObj::IsFillCurve(CCurveObj* pObj)
{
	ASSERT(pObj != NULL);
	for (int i=0; i<m_FillList.GetSize(); i++)
		if ((m_FillList[i]->pTargetCurve != NULL) && (m_FillList[i]->pTargetCurve == pObj))
		   return TRUE;
	return FALSE;
}

BOOL CCurveObj::IsFillCurve(CString name)
{
	for (int i=0; i<m_FillList.GetSize(); i++)
		if (m_FillList[i]->m_TargetCurveName == name) return TRUE;
	return FALSE;
}

void CCurveObj::FindAllFillCurve()
{
	CCurveObj* pObj;
	if (m_pParentObj == NULL) return;
	if ((m_pParentObj->IsKindOf(RUNTIME_CLASS(CTrackObj))) && (m_ParentType == 0))
	{
		for (int i=(int)m_FillList.GetSize()-1; i>=0; i--)
		{
			if ((m_FillList[i]->m_TargetCurveName == "左边界") || (m_FillList[i]->m_TargetCurveName == "右边界")) continue;
			if ((pObj = ((CTrackObj*)m_pParentObj)->FindAliasCurve(m_FillList[i]->m_TargetCurveName)) != NULL)
				m_FillList[i]->pTargetCurve = pObj;
			else
			{
				delete m_FillList[i];
				m_FillList.RemoveAt(i);
			}

		}
	}
}

int CCurveObj::RemoveFillCurve(CCurveObj* pObj)
{
	ASSERT(pObj != NULL);
	for (int i=0; i<m_FillList.GetSize(); i++)
	{
		if ((m_FillList[i]->pTargetCurve != NULL) && (m_FillList[i]->pTargetCurve == pObj)) 
		{
			delete m_FillList[i];
			m_FillList.RemoveAt(i);
			return 0;
		}
	}
	return -1;
}

int CCurveObj::RemoveFillCurve(CString name)
{
	for (int i=0; i<m_FillList.GetSize(); i++)
	{
		if (m_FillList[i]->m_TargetCurveName == name)
		{
			delete m_FillList[i];
			m_FillList.RemoveAt(i);
			return 0;
		}
	}
	return -1;
}

int CCurveObj::AddFillCurve(CCurveFill* pFill)
{
	ASSERT(pFill != NULL);
	ASSERT(pFill->IsKindOf(RUNTIME_CLASS(CCurveFill)));
	pFill->m_SourceCurveName = m_AliasName;
	pFill->pSourceCurve = this;
	return (int)m_FillList.Add(pFill);
}

void CCurveObj::RemoveAllFillCurve()
{
	for (int i=0; i<m_FillList.GetSize(); i++)
		delete m_FillList[i];
	m_FillList.RemoveAll();
}

void CCurveObj::OnLButtonDown(CView* pView, UINT nFlags, const CPoint& point)
{
    if (m_bSelected) 
	{
		nDragHandle = HitTest(pView,point); 
		if (nDragHandle < 9) 
		{
			CFIDDrawObj::OnLButtonDown(pView, nFlags, point);
			return;
		}
	}
	else CFIDDrawObj::OnLButtonDown(pView, nFlags, point);
}

void CCurveObj::OnMouseMove(CView* pView, UINT nFlags, const CPoint& point)
{
	if (nDragHandle < 9) 
	{	
		CFIDDrawObj::OnMouseMove(pView, nFlags, point);		
		return;
	}
}

void CCurveObj::OnLButtonUp(CView* pView, UINT nFlags, const CPoint& point)
{
	if(!m_bSelected)
	{
		CFIDDrawObj::OnLButtonUp(pView,nFlags,point);
		return;
	}
	if (nDragHandle < 9)
	{
		BOOL bFill = FALSE;
		CDrawObj* pObj = m_pParentObj;
		if ((pObj != NULL) && (pObj->IsKindOf(RUNTIME_CLASS(CTrackObj))))
			bFill = ((CTrackObj*)pObj)->IsFillCurve(m_AliasName);
		CFIDDrawObj::OnLButtonUp(pView,nFlags,point);
		if ((bFill) || (m_FillList.GetSize() > 0))
		{
			if ((pObj != NULL) && ((m_pParentObj == NULL) || (pObj != m_pParentObj)))
			{
				for (int i=0; i<m_FillList.GetSize(); i++)
				{
					CString name = m_FillList[i]->m_TargetCurveName;
					if ((name != "左边界") && (name != "右边界") && (name != m_AliasName))
					{
						delete m_FillList[i];
						m_FillList.RemoveAt(i);
						i --;
					}
				}
				if (bFill)
				((CTrackObj*)pObj)->RemoveFillCurve(m_AliasName);
			}
		}
		if ((m_pParentObj != NULL) && (m_pParentObj->IsKindOf(RUNTIME_CLASS(CTrackObj))))
		{
			while(1)
			{
				CCurveObj* pObj = ((CTrackObj*)m_pParentObj)->FindAliasCurve(m_AliasName);
				if ((pObj != NULL) && (pObj != this))
				{
					/*CString szHT;
					szHT.LoadString(IDS_INPUTALIAS);
					CInputTextDialog m_Dialog(szHT);
					m_Dialog.SetText(m_AliasName);
					m_Dialog.DoModal();
					m_AliasName = m_Dialog.GetText();*/
					m_AliasName +="$";
				}
				else break;
			}
		}
	}
}

void CCurveObj::DrawTracker(CView* pView, CDC* pDC, TrackerState state)
{
	CFIDDrawObj::DrawTracker(pView,pDC,state);
}

int CCurveObj::HitTest(CView* pView,const CPoint& point)
{
	int nHandle = CFIDDrawObj::HitTest(pView,point); 
	if ((nHandle >= 0) || (!m_Rect.PtInRect(point))) return nHandle;
	if (bErrorFID) return -1;
	double sdep = YCoorToDepth(point.y);
	if ((sdep < m_SSdep) || (sdep > m_EEdep)) return -1;
	CGeoFIDIO* pFID = new CGeoFIDIO(m_strPathName);
	nHandle = -1;
	if (pFID->Open(pDoc->m_szSever))
	{
		pFID->SeekToDepth((float)(sdep-m_Rlev));
		pFID->Read(2,3,value);
		double tmpc,tmpval;
		m_LeftVal1 = m_LeftVal;	m_RightVal1 = m_RightVal;
		if (m_CalType == 1)
		{
			m_LeftVal1 = log10(m_LeftVal);
			m_RightVal1 = log10(m_RightVal);
		}
		tmpc = (double)(m_Rect.Width() / (m_RightVal1 - m_LeftVal1)); 
		for (int i=0; i<3; i++)
		{ 
			pt[i].y = DepthToYCoor(sdep + (i-1)*m_Rlev) - pDoc->m_WinOrg.y;
			tmpval = value[i];			
			if (m_CalType == 1) 
			{
				if (tmpval < 0.000000000000001f) 
					tmpval =0.000000000000001f;
				tmpval = log10(tmpval);
			}
			pt[i].x = m_Rect.left + Round((tmpval-m_LeftVal1)*tmpc);
			if (m_Factor > 0)	
			{
				if (i == 1) continue;
				while (pt[i].x < m_Rect.left) 
					pt[i].x = m_Rect.right - (m_Rect.left - pt[i].x) / m_Factor;
				while (pt[i].x > m_Rect.right) 
					pt[i].x = m_Rect.left + (pt[i].x - m_Rect.right) / m_Factor;
			}
			else if (m_Factor == 0) 
			{
				if (pt[i].x < m_Rect.left) pt[i].x = m_Rect.left;
				if (pt[i].x > m_Rect.right) pt[i].x = m_Rect.right;
			}
		}
		if (m_Factor > 0)
		{
			double v;
			while (pt[1].x < m_Rect.left) 
			{
				pt[1].x = m_Rect.right - (m_Rect.left - pt[1].x) / m_Factor;
				v = m_RightVal1 - m_LeftVal1;
				m_RightVal1 = m_LeftVal1;
				m_LeftVal1 = m_RightVal1 - v * m_Factor;
			}
			while (pt[1].x > m_Rect.right) 
			{
				pt[1].x = m_Rect.left + (pt[1].x - m_Rect.right) / m_Factor;
				v = m_RightVal1 - m_LeftVal1;
				m_LeftVal1 = m_RightVal1;
				m_RightVal1 = m_LeftVal1 + v * m_Factor;
			}
		}
		if ((point.x < pt[1].x+2) && (point.x >= pt[1].x-2)) nHandle = 9;
	}
	delete pFID;
	return nHandle;
}

void CCurveObj::DrawFocus(CView* pView)
{
	if (nDragHandle < 9) 
	{ 
		CFIDDrawObj::DrawFocus(pView);
		return;
	}
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc,NULL);
	dc.SetROP2(R2_XORPEN);
    CPen m_Pen(PS_DOT,1,RGB(0,0,0));
   	CPen *pOldPen = dc.SelectObject(&m_Pen);
	dc.Polyline(pt,3);
	dc.SelectObject(pOldPen);
	dc.SetROP2(R2_COPYPEN);
}

BOOL CCurveObj::OnToolTip(CView* pView,CRect& rect,CString& text)
{
	CPoint point = rect.TopLeft();
	int nHandle = HitTest(pView,point);
	if (nHandle == 9)
	{
		rect.InflateRect(2,2);
		char str[200];
		double PointDep = YCoorToDepth(point.y);
		PointDep=((int)(PointDep/m_Rlev))*m_Rlev;
		dtos(PointDep,str);
		CString ss=m_strPathName;
		ss=ss.Mid(ss.ReverseFind ('.')+1);
		strcat(str,"  ");
		strcat(str,ss);
		strcat(str,"  ");
		char s[100];
		dtos((double)value[1],s);
		strcat(str,s);
		text = str;
		return TRUE;
	}
	return FALSE;
}

void CCurveObj::DrawCurveFocuse(CView* pView)
{
		if (IsGridCurve()) bErrorFID = FALSE;	
		CRect rect = m_Rect;
		pDoc->LPtoPP(&rect);
		CDC *pDC=pView->GetDC ();
		pView->OnPrepareDC (pDC,NULL);
		if (!pDC->RectVisible(rect)) return;
		if ((m_pParentObj == NULL) || (m_ParentType != 0)) 
			CFIDDrawObj::DrawClient(pDC);
		if (bErrorFID) return;	
		CRect m_ClipRect;
		pDC->GetClipBox(m_ClipRect);
		m_ClipRect &= rect;
		if (IsGridCurve())
		{
			int x = GetGridValue();
			LINE m_Line;
			m_Line.pPen = &m_LinePen;
			flag = 1;
			m_LinePen.org = 0;
			m_Line.pDC = pDC;
			DLine(x,m_ClipRect.top,x,m_ClipRect.bottom,&m_Line);
			return;
		}
		double sdep;
		double edep;
		sdep = YCoorToDepth(m_ClipRect.top + pDoc->m_WinOrg.y) - m_Rlev;
		edep = YCoorToDepth(m_ClipRect.bottom  + pDoc->m_WinOrg.y) + m_Rlev;
		if (sdep < m_PlotSdep) sdep = m_PlotSdep;
		if (edep > m_PlotEdep) edep = m_PlotEdep;
		CPoint* point;
		int m_PointNum = 0;
		m_PointNum = GetPoint(sdep,edep,point);
		if (m_PointNum > 2) 
		{ 
			pDoc->LPtoPP(point,m_PointNum);                
			pDC->SetROP2(R2_XORPEN);
			CPen m_Pen(PS_DOT,1,RGB(0,0,0));
   			CPen *pOldPen = pDC->SelectObject(&m_Pen);
			pDC->Polyline (point,m_PointNum-2);
			pDC->SetROP2(R2_COPYPEN);
			pDC->SelectObject(pOldPen);
			pDoc->PPtoLP(point,m_PointNum);                
		}
}