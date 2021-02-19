/******************************************************************
*   ResulDrw.cpp - implementation of the Yield draw object class *           
*******************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "GeoFIDio.h"
#include "TrackDraw.h"
#include "YieldDraw.h"
#include "resource.h"

//#include "StyleDialog.h"
//#include "MinFontDialog.h"
//#include "LineListPage.h"
//#include "FillListPage.h"
//#include "FIDBasePage.h"
//#include "YieldPage.h"


CYieldPoint::CYieldPoint(CRect rect, COLORREF VittaColor,COLORREF BkColor,COLORREF PenColor,CLogPlotDoc* pDocument)
{
	pDoc = pDocument;
	m_Rect = rect;
	m_VittaColor=VittaColor;
	m_BkColor=BkColor;
	m_PenColor=PenColor;
}

void CYieldPoint::Draw(CDC* pDC)
{
	CRect rect = m_Rect;	
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;
	
	CPen Pen2;
	Pen2.CreatePen (PS_SOLID,1,m_PenColor);
	CPen* OldPen=pDC->SelectObject (&Pen2);
	int StartY = rect.bottom;
	int r=rect.Width()/2+1;
	int StartX=rect.left-1;
	float m_LookAngle=20.;
	
	CRect rt(StartX,StartY-(int)(r*sin(m_LookAngle*PI/180.0)),StartX+(int)(2.0*r),StartY+(int)(r*sin(m_LookAngle*PI/180.0))+2);
	FillEllipse(rt,94,m_BkColor,rect.TopLeft(),pDC,m_VittaColor);
	CBrush Brush3;
	Brush3.CreateStockObject(NULL_BRUSH);
	CBrush* pOldBrush2 =(CBrush*)pDC->SelectObject(&Brush3);
	pDC->Ellipse(rt);

	LithFillBox(rect,94,m_BkColor,rect.TopLeft(),pDC,m_VittaColor);
	
	CBrush Brush2;
	Brush2.CreateSolidBrush(m_BkColor);
	pDC->SelectObject(&Brush2);
	StartY = rect.top;
	pDC->Ellipse(StartX,StartY-(int)(r*sin(m_LookAngle*PI/180.0)),StartX+(int)(2.0*r),StartY+(int)(r*sin(m_LookAngle*PI/180.0))+2);
	
	pDC->SelectObject(pOldBrush2);
	pDC->SelectObject (OldPen);
}

IMPLEMENT_SERIAL(CYieldObj, CFIDDrawObj, 0)

CYieldObj::CYieldObj()
{
	m_DrawShape = Yieldobj;
	bFlush = TRUE;
}

CYieldObj::CYieldObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDoc)
		:CFIDDrawObj(strPathName,rect,pParentObj,pDoc)
{
	m_HeadRect = rect;
	m_HeadRect.bottom = m_HeadRect.top + 100;
	m_HeadRect.right = m_HeadRect.left + 40;
	m_Rect = m_HeadRect;
	m_Rect.top = m_HeadRect.bottom;
	m_DrawDirect = VERDIR;
	m_DrawShape = Yieldobj;
 	
	m_VittaColor = RGB(128,128,128);
	m_AliasName = "产出层位";
	bFlush = TRUE;
	m_FramePen.lopnWidth = CPoint(2,2); 
	m_FramePen.lopnColor = RGB(0,0x80,0); 
	m_FramePen.lopnStyle = 0;
	m_VittaDep=(float)m_PlotSdep;
}

CYieldObj::~CYieldObj()
{
	POSITION pos = m_YieldPoint.GetHeadPosition();
	while (pos != NULL) delete m_YieldPoint.GetNext(pos);
	m_YieldPoint.RemoveAll();
}    

void CYieldObj::CreatePoint()
{
	CRect rect;
	rect.left = m_Rect.left;
	rect.right = m_Rect.right;

	POSITION pos = m_YieldPoint.GetHeadPosition();
	while (pos != NULL) 
		delete m_YieldPoint.GetNext(pos);
	m_YieldPoint.RemoveAll();
	CString str;
	int YieldNum=-1;
	int i=0,xx,Remark;
	int xyz=m_strPathName.Find (".List");
	if((xyz>0)&&DetectFileExist(m_strPathName))
	{
		TCHAR *token, *end;
		TCHAR buffer[1024];
		CStdioFile File;
		if (!File.Open(m_strPathName, CFile::modeRead | CFile::typeText)) 
			return;
		// Read Header off file
		File.ReadString(buffer, 1024);
		// Get first token
		for (token=buffer, end=buffer; 
			 *end && (*end != _T(',')) && (*end != _T('\n')); end++);
		if ((*end == _T('\0')) && (token == end)) token = NULL;
		*end = _T('\0');
		while (token) 
		{
			str=token;
			while(str.Left(1)==" ")
				str.TrimLeft (" ");
			while(str.Right(1)==" ")
				str.TrimRight (" ");
			if(str=="解释结论")
			{
				YieldNum=i;
				break;
			}
			// Get next token
			for (token=++end; *end && (*end != _T(',')) && (*end != _T('\n'));
				 end++);
			if ((*end == _T('\0')) && (token == end)) token = NULL;
			*end = _T('\0');
			i++;
		}
		if(YieldNum<0)
		{
			File.Close();return;
		}
		bool DepFlag;
		double m_Sdep,m_Edep;			
		while (File.ReadString(buffer, 1024)) 
		{
			// Get first token
			for (token=buffer, end=buffer; 
				 *end && (*end != _T(',')) && (*end != _T('\n')); end++);
			if ((*end == _T('\0')) && (token == end)) token = NULL;
			*end = _T('\0');
			
			DepFlag=TRUE;
			Remark=-1;i=0;
			while (token) 
			{
				str=token;
				while(str.Left(1)==" ")
					str.TrimLeft (" ");
				while(str.Right(1)==" ")
					str.TrimRight (" ");
				if(i==YieldNum)
					Remark=8;
				else if(DepFlag)
				{
					xx=str.Find ("--");
					if((xx>0)&&(DepFlag))
					{
						m_Sdep=atof(str.Left (xx));
						m_Edep=atof(str.Mid (xx+2));
						if(m_Sdep>100&&m_Edep>100)
						{
							if(m_Sdep<m_PlotSdep) m_Sdep=m_PlotSdep;
							if(m_Edep>m_PlotEdep) m_Edep=m_PlotEdep;
							DepFlag=FALSE;
						}
					}
				}
				// Get next token
				for (token=++end; *end && (*end != _T(',')) && (*end != _T('\n'));
					 end++);
				if ((*end == _T('\0')) && (token == end)) token = NULL;
				*end = _T('\0');
				i++;
			}
			if((!DepFlag)&(Remark==8))
			{
				rect.top = DepthToYCoor(m_Sdep);
				rect.bottom = DepthToYCoor(m_Edep);
				CYieldPoint* pObj = new CYieldPoint(rect,m_VittaColor,m_Brush.lbColor,m_FramePen.lopnColor,pDoc);
				m_YieldPoint.AddTail(pObj);
			}
		}
		File.Close();
	}
	bFlush = FALSE;
}

void CYieldObj::Serialize(CArchive& ar)
{                      
	CFIDDrawObj::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << (LONG) m_VittaColor;
		ar << m_VittaDep;
	}
	else
	{
		long l;
		ar >> l; 
		m_VittaColor = (COLORREF) l;
		ar >> m_VittaDep;
		bFlush = TRUE;
	}	
}

void CYieldObj::SetRect(const CRect& rect)
{
	CPoint delta;    
	CYieldPoint* pObj;
	CRect rect1;
	delta.x = rect.right - m_Rect.right;
	delta.y = rect.top - m_Rect.top;
	int cx =20;
	POSITION pos = m_YieldPoint.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CYieldPoint*) m_YieldPoint.GetNext(pos);
		rect1 = pObj->GetRect() + delta;
		rect1.left = rect.left;
		rect1.right = rect.right;
		pObj->SetRect(rect1);
	}          
	CFIDDrawObj::SetRect(rect);
}       

BOOL CYieldObj::Properties(CView* pView)
{
	//CString szHT;
	//szHT.LoadString(IDS_DRAWINGPROPERTIES);
	//CPropertySheet m_Sheet(szHT);
	//CYieldPage m_YieldPage;
	//m_YieldPage.m_FileName=m_strPathName;
	//m_YieldPage.m_VittaDep=m_VittaDep;
	//m_YieldPage.m_VittaColor=m_VittaColor;
	//m_Sheet.AddPage(&m_YieldPage);
	//CLineListPage m_LineList;
	//CFillListPage m_FillList;
 //	if ((m_ParentType != 0) || (m_DrawDirect == VERDIR))
	//{
	//	szHT.LoadString(IDS_FRAMELINE);
	//	m_LineList.AddPen(&m_FramePen,szHT);
	//	szHT.LoadString(IDS_FRAMEFILL);
	//	m_FillList.AddFill(&m_Brush,szHT);
	//	m_Sheet.AddPage(&m_LineList);
	//	m_Sheet.AddPage(&m_FillList);
	//}
	//CMinFontPage m_FontPage;
	//TEXTFONT m_TextFont[2];
	//if(m_AliasName=="") m_AliasName="标题";
	//m_TextFont[0].m_ItemName = m_AliasName;
	//m_TextFont[0].m_Text = m_AliasName;
	//m_TextFont[0].m_ForeGround = m_NameColor;
	//m_TextFont[0].m_BackGround = RGB(255,255,255);
	//m_TextFont[0].lf = m_NameFont;
	//m_FontPage.AddFont(&m_TextFont[0]);
	//m_Sheet.AddPage(&m_FontPage);
	//double m_PlotSdep1 = m_PlotSdep;
	//double m_PlotEdep1 = m_PlotEdep;
	//if (m_Sheet.DoModal() == IDOK)
	//{
	//	m_FontPage.OnOK();
	//	m_AliasName=m_TextFont[0].m_Text ;
	//	m_NameColor = m_TextFont[0].m_ForeGround;
	//	m_NameFont = m_TextFont[0].lf;
	//	m_VittaDep=m_YieldPage.m_VittaDep;
	//	m_VittaColor=m_YieldPage.m_VittaColor;
	//	if (m_strPathName != m_YieldPage.m_FileName)
	//		m_strPathName = m_YieldPage.m_FileName;
	//	CRect SaveRect = m_Rect;
	//	SaveRect.top = m_HeadRect.top;
	//	bFlush=TRUE;
	//	if (m_ParentType != 0) 
	//		SaveRect.bottom = m_Rect.bottom;
	//	InvalidateRect(SaveRect);
	//	pDoc->SetModifiedFlag();
	//	return TRUE;
	//}
	return FALSE;
}                

void CYieldObj::DrawHead(CDC* pDC)
{
	CRect rect = m_HeadRect;
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;

//	CFIDDrawObj::DrawHead(pDC);   
	CFont m_Font;
	if (m_NameFont.lfEscapement == 2700)
	{
		CSize size;
		size.cx = m_NameFont.lfHeight; size.cy = m_NameFont.lfWidth;
		CSize size1 = size;
		pDC->LPtoDP(&size);
		m_NameFont.lfHeight = size.cx;
		m_NameFont.lfWidth = size.cy;
		m_Font.CreateFontIndirect(&m_NameFont);
		m_NameFont.lfHeight = size1.cx;
		m_NameFont.lfWidth = size1.cy;
	}
	else m_Font.CreateFontIndirect(&m_NameFont);
	
	CFont* pOldFont = pDC->SelectObject(&m_Font);
	CSize size = pDC->GetTextExtent("文本");
   	pDC->SetTextColor(m_NameColor);  
	pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
	pDC->SetBkMode(TRANSPARENT); 
	
	if (m_NameFont.lfEscapement == 2700)
	{
		CPoint pt;
		pt.x = (rect.left+rect.right) / 2;
		pt.y =  (rect.top + rect.bottom) / 2;
		pDC->LPtoDP(&pt);
		int nSaveDC = pDC->SaveDC();
		pDC->SetMapMode(MM_TEXT);
		pDC->DPtoLP(&pt);
		pDC->TextOut(pt.x-size.cy/2, pt.y,m_AliasName);
		pDC->RestoreDC(nSaveDC);
	}
	else
	{
		CPoint pt;
		pt.x = (rect.left+rect.right) / 2;
		pt.y =  (rect.top + rect.bottom) / 2;
		int AliasLen=m_AliasName.GetLength ();
		CString Str;
		for(int i=0;i<AliasLen;i+=2)
		{
			Str=m_AliasName.Mid (i,2);
			pDC->TextOut(pt.x,pt.y+(i-1)*size.cy/2,Str);
		}
	}
	pDC->SelectObject(pOldFont);
}   

void CYieldObj::DrawClient(CDC* pDC)
{
	CFIDDrawObj::DrawClient(pDC);
	CRect rect = m_Rect;
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;
	if(bFlush) CreatePoint();
	CYieldPoint* pObj;
	POSITION pos = m_YieldPoint.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CYieldPoint*) m_YieldPoint.GetNext(pos);
		CRect rect = pObj->m_Rect;
		pDoc->LPtoPP(&rect);
		CRect rect1 =m_ClientRect;
		pDoc->LPtoPP(&rect1);
		rect|=rect1;
		if (pDC->RectVisible(rect))
			pObj->Draw(pDC);
	}

//	if(m_VittaDep<m_PlotSdep+5.0)
//		m_VittaDep=(float)m_PlotSdep+5.f;
//	if(m_VittaDep>m_PlotEdep-5.0)
//		m_VittaDep=(float)m_PlotEdep-5.f;
	int YY=DepthToYCoor(m_VittaDep);
	if(YY>rect.top )
	{
		CBrush br1;
		br1.CreateSolidBrush(m_VittaColor);
		int W=rect.Width ()/4;
		CRect r(m_Rect.left+W,m_Rect.top +2,m_Rect.right-W,YY);
		pDoc->LPtoPP(&r);
		pDC->FillRect(r,&br1);
		CRect r1(m_Rect.left+W-W/3,YY-W/2,m_Rect.right-W+W/3,YY);
		pDoc->LPtoPP(&r1);
		pDC->FillRect(r1,&br1);
	}	
}

int CYieldObj::HitTest(CView* pView,const CPoint& point)
{
	int nHandle = CFIDDrawObj::HitTest(pView,point); 
	if ((nHandle >= 0) || (!m_Rect.PtInRect(point))) return nHandle;
	if(m_Rect.PtInRect(point)) 
		return 9;
	else return -1;
}
