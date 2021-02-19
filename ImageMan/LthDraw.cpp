/******************************************************************
*   LthDrw.cpp - implementation of the Lth draw object class      *           
*******************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "ImageMan.h"
#include "ImageManDoc.h"
#include "FIDIO.h"
#include "LthDraw.h"
#include "style.h"
#include "ResultPage.h"
#include "MinFontDialog.h"
#include "LineListPage.h"
#include "FillListPage.h"
#include "TrackDraw.h"

CLthPoint::CLthPoint(CRect rect, int LithNo,unsigned long ColorNo, int LithWidth,CImageManDoc* pDocument)
{
	pDoc = pDocument;
	m_LithNo = LithNo;
	m_ColorNo=ColorNo;
	m_LithWidth=LithWidth;
	m_Rect = rect;
}

void CLthPoint::Draw(CDC* pDC,COLORREF LithColor)
{
	CRect rect = m_Rect;
	pDoc->LPtoPP(&rect);
	if (pDC->RectVisible(rect))
	{
		if(m_LithWidth==1)
			pDC->Rectangle (rect);
		LithFillBox(rect,m_LithNo,LithColor,rect.TopLeft() ,pDC,m_ColorNo);
	}
}

IMPLEMENT_SERIAL(CLthObj, CFIDDrawObj, 0)

CLthObj::CLthObj()
{
	m_DrawShape = lthobj;
	m_LithWidth=0;
}

CLthObj::CLthObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDoc)
		:CFIDDrawObj(strPathName,rect,pParentObj,pDoc)
{
	m_HeadRect = rect;
	m_HeadRect.bottom = m_HeadRect.top + 100;
	m_HeadRect.right = m_HeadRect.left + 40;
	m_Rect = m_HeadRect;
	m_Rect.top = m_HeadRect.bottom;
	m_DrawDirect = VERDIR;
	m_DrawShape = lthobj;
	m_NameFont.lfEscapement = 2700;
    m_NameFont.lfHeight = -20;
	m_AliasName = "岩芯柱";
	m_LithWidth=0;
}

CLthObj::~CLthObj()
{
	POSITION pos = m_LthPoint.GetHeadPosition();
	while (pos != NULL) delete m_LthPoint.GetNext(pos);
	m_LthPoint.RemoveAll();
}    

void CLthObj::CreatePoint()
{

	CRect rect = m_Rect;                                     
	POSITION pos = m_LthPoint.GetHeadPosition();
	while (pos != NULL) delete m_LthPoint.GetNext(pos);
	m_LthPoint.RemoveAll();
	
	int xyz=m_strPathName.Find (".List");
	if((xyz>0)&&DetectFileExist(m_strPathName))
	{
		TCHAR *token, *end;
		TCHAR buffer[1024];
		CStdioFile File;
		if(!File.Open(m_strPathName, CFile::modeRead | CFile::typeText)) 
			return;
		CString str,Resultstr;
		int SDepPos=-1,EDepPos=-1,LithPos=-1,ColorPos=-1;
		int i=0;
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
			if(str=="岩性代号")
				LithPos=i;
			else if(str=="岩性颜色")
				ColorPos=i;
			else if(str=="起始深度")
				SDepPos=i;
			else if(str=="结束深度")
				EDepPos=i;
			// Get next token
			for (token=++end; *end && (*end != _T(',')) && (*end != _T('\n'));
					end++);
			if ((*end == _T('\0')) && (token == end)) token = NULL;
			*end = _T('\0');
			i++;
		}
		if((LithPos<0)||(ColorPos<0)||(SDepPos<0)||(EDepPos<0))
		{
			File.Close();return;
		}
		
		double m_Sdep,m_Edep;
		int m_LthNo=0;
		unsigned long m_ColorNo=0x08000;
		while (File.ReadString(buffer, 1024)) 
		{
			// Get first token
			for (token=buffer, end=buffer; 
					*end && (*end != _T(',')) && (*end != _T('\n')); end++);
			if ((*end == _T('\0')) && (token == end)) token = NULL;
			*end = _T('\0');
			
			i=0;m_Sdep=m_Edep=0;
			while (token) 
			{
				str=token;
				while(str.Left(1)==" ")
					str.TrimLeft (" ");
				while(str.Right(1)==" ")
					str.TrimRight (" ");
				if(i==SDepPos)
					m_Sdep=atof(str);
				else if(i==EDepPos)
					m_Edep=atof(str);
				else if(i==LithPos)
					m_LthNo=atoi(str);
				else if(i==ColorPos)
				{
					m_ColorNo=atol(str);
					if(str.IsEmpty ()||(m_ColorNo<0)) 
						m_ColorNo=0x08000;
				}
				// Get next token
				for (token=++end; *end && (*end != _T(',')) && (*end != _T('\n'));
						end++);
				if ((*end == _T('\0')) && (token == end)) token = NULL;
				*end = _T('\0');
				i++;
			}
			if(m_Sdep<m_PlotSdep)	m_Sdep=m_PlotSdep;
			if(m_Sdep>m_PlotEdep)   continue;
			if(m_Edep<m_PlotSdep)   continue;
			if(m_Edep>m_PlotEdep)	m_Edep=m_PlotEdep;
			if(((m_Sdep<m_PlotSdep&&m_Edep>m_PlotSdep)||
				(m_Edep>m_PlotEdep&&m_Sdep<m_PlotEdep)||
				(m_Sdep>=m_PlotSdep&&m_Sdep<=m_PlotEdep&&m_Edep>=m_PlotSdep&&m_Edep<=m_PlotEdep))
				&&(m_LthNo>=0))
			{
				rect = m_Rect;
				rect.top = DepthToYCoor(m_Sdep);
				rect.bottom = DepthToYCoor(m_Edep);
				if(m_LithWidth==1)
				{
					int xx1=rect.Width ()/6;
					int xx2=rect.Width ()-xx1;
					int xx=xx2*(m_ColorNo+1)/16;
					rect.right =rect.left+xx1+xx;
				}
				CLthPoint* pObj = new CLthPoint(rect,m_LthNo,m_ColorNo,m_LithWidth,pDoc);
				m_LthPoint.AddTail(pObj);
			}
		}
		File.Close();
	}
	m_bFlush = FALSE;
}

void CLthObj::Serialize(CArchive& ar)
{
	CFIDDrawObj::Serialize(ar);
	CreatePoint();
}

void CLthObj::SetRect(const CRect& rect)
{
	CPoint delta;    
	CRect rect1;
	CLthPoint* pObj;
	delta.x = rect.right - m_Rect.right;
	delta.y = rect.top - m_Rect.top;
	POSITION pos = m_LthPoint.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CLthPoint*) m_LthPoint.GetNext(pos);
		rect1 = pObj->GetRect() + delta;
		rect1.left = rect.left;
		rect1.right = rect.right;
		pObj->SetRect(rect1);
	}                                         
	CFIDDrawObj::SetRect(rect);
}       

void CLthObj::DrawHead(CDC* pDC)
{
	CRect rect = m_HeadRect;
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;

	CFIDDrawObj::DrawHead(pDC);   
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
		int AliasLen=m_AliasName.GetLength ();
		pt.y =  (rect.top + rect.bottom-size.cy*AliasLen/4) / 2;
		CString Str;
		for(int i=0;i<AliasLen;i+=2)
		{
			Str=m_AliasName.Mid (i,2);
			pDC->TextOut(pt.x,pt.y+(i-1)*size.cy/2,Str);
		}
	}
	pDC->SelectObject(pOldFont);
}   

void CLthObj::DrawClient(CDC* pDC)
{
	CRect rect = m_Rect;
	pDoc->LPtoPP(&rect);
	if (pDC->RectVisible(rect))
	{
		CFIDDrawObj::DrawClient(pDC);   
		if (m_bFlush) CreatePoint();
		CLthPoint* pObj;                                  
		POSITION pos = m_LthPoint.GetHeadPosition();
		while (pos != NULL)
		{
			pObj = (CLthPoint*) m_LthPoint.GetNext(pos);
			pObj->Draw(pDC,m_NameColor);
		}
	}
}

BOOL CLthObj::Properties(CView* pView)
{
	CString szHT;
	CPropertySheet m_Sheet("岩性柱属性");
	CResultPage m_ResultPage;
	m_ResultPage.m_GroupCaption="宽度";
	m_ResultPage.m_LeftCaption="道宽";
	m_ResultPage.m_RightCaption="颜色代码";
	m_ResultPage.m_ResultLR =m_LithWidth;
	m_ResultPage.m_FileName=m_strPathName;
	m_Sheet.AddPage(&m_ResultPage);

	CLineListPage m_LineList;
	CFillListPage m_FillList;
 	if ((m_ParentType != 0) || (m_DrawDirect == VERDIR))
	{
		szHT.LoadString(IDS_FRAMELINE);
		m_LineList.AddPen(&m_FramePen,szHT);
		szHT.LoadString(IDS_FRAMELINE);
		m_FillList.AddFill(&m_Brush,szHT);
		m_Sheet.AddPage(&m_LineList);
		m_Sheet.AddPage(&m_FillList);
	}
	TEXTFONT m_TextFont;
	m_TextFont.m_ItemName = m_AliasName;
	m_TextFont.m_Text = m_AliasName;
	m_TextFont.m_ForeGround = m_NameColor;
	m_TextFont.m_BackGround = RGB(255,255,255);
	m_TextFont.lf = m_NameFont;
	CMinFontPage m_FontPage;
	m_FontPage.AddFont(&m_TextFont);
	m_Sheet.AddPage(&m_FontPage);
	double m_PlotSdep1 = m_PlotSdep;
	double m_PlotEdep1 = m_PlotEdep;
	if (m_Sheet.DoModal() == IDOK)
	{
		m_LithWidth=m_ResultPage.m_ResultLR ;
		if (m_strPathName != m_ResultPage.m_FileName)
			m_strPathName = m_ResultPage.m_FileName;
		m_bFlush = TRUE;
		m_FontPage.OnOK();
		m_NameColor = m_TextFont.m_ForeGround;
		m_NameFont = m_TextFont.lf;
		m_AliasName=m_TextFont.m_Text ;
		if (m_DrawDirect == HORDIR) m_NameFont.lfEscapement = 0;
		CRect m_SaveRect = m_Rect;
		m_SaveRect.top = m_HeadRect.top;
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
		InvalidateRect(m_SaveRect);
		return TRUE;
	}
	return FALSE;
}

int CLthObj::HitTest(CView* pView,const CPoint& point)
{
	int nHandle = CFIDDrawObj::HitTest(pView,point); 
	if ((nHandle >= 0) || (!m_Rect.PtInRect(point))) return nHandle;
	if(m_Rect.PtInRect(point)) 
		return 9;
	else return -1;
}