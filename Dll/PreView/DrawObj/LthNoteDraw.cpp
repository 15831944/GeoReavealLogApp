/******************************************************************
*   LthDrw.cpp - implementation of the Lth draw object class      *           
*******************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "GeoFIDio.h"
#include "LthNoteDraw.h"
#include "style.h"
#include "TrackDraw.h"

//#include "LayerPage.h"
//#include "MinFontDialog.h"
//#include "LineListPage.h"
//#include "FillListPage.h"


CLthNotePoint::CLthNotePoint(CRect rect, COLORREF LthNoteColor,LOGFONT LthNoteFont,CString NoteStr,CLogPlotDoc* pDocument)
{
	pDoc = pDocument;
	m_NoteStr=NoteStr;
	m_Rect = rect;
	m_LthNoteColor=LthNoteColor;
	m_LthNoteFont=LthNoteFont;
}
void CLthNotePoint::Draw(CDC* pDC)
{
	CRect rect = m_Rect;	
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;	
	CPen m_Pen(0,1,m_LthNoteColor);
	CPen* pOldPen = pDC->SelectObject(&m_Pen);
	pDC->MoveTo(rect.TopLeft ());
	pDC->LineTo(rect.right ,rect.top );
	pDC->SelectObject(pOldPen);
	CFont Font;
	Font.CreateFontIndirect(&m_LthNoteFont);
	CFont* pOldFont = pDC->SelectObject(&Font);
	pDC->SetTextColor(m_LthNoteColor);  
	pDC->SetTextAlign(TA_LEFT | TA_TOP);
	pDC->SetBkMode(TRANSPARENT); 
	CSize size = pDC->GetTextExtent("备");
	if(m_LthNoteFont.lfEscapement==2700)
	{
		pDC->TextOut(rect.left+rect.Width ()/2+size.cx/2,rect.top+3,m_NoteStr, (int)strlen(m_NoteStr));
		return;
	}
	CString DataListStr=m_NoteStr;
	DataListStr.TrimLeft ();
	DataListStr.TrimRight ();
	int Dy=size.cy +10;
	int y=rect.top +5;
			
	float kWx=1;
	int Dx=rect.Width ()-15;
	int Dx0=0;//段落起始位置
	CString Str,ss,m_OkText;	
	int m,n,j,StrNum;
	char CV;
	m_OkText="";
	int xxx=DataListStr.Find("^");
	if(xxx>0)
	{
		while(xxx>0)
		{
			ss=DataListStr.Left(xxx);	
			ss.TrimLeft ();
			ss.TrimRight ();
			pDC->TextOut(rect.left +5+Dx0,y,ss);
			y+=Dy;
			DataListStr=DataListStr.Mid(xxx+1);
			xxx=DataListStr.Find("^");
		}
		if(DataListStr!="")
			pDC->TextOut(rect.left +5+Dx0,y,DataListStr);
	}
	else
	{
		for(;;)
		{
			m=DataListStr.Find(char(13));
			if(m==-1)	ss=DataListStr;
			else		ss=DataListStr.Left(m);	
			ss.TrimLeft ();
			ss.TrimRight ();
			StrNum=ss.GetLength ();
			j=0;Str="";n=0;
			m_OkText+=char(10);
			while(j<StrNum)
			{
				CV=ss.GetAt  (j);
				Str+=CV;						
				if(CV<0)
				{
					j++;
					Str+=ss.GetAt(j);
				}
				size=pDC->GetTextExtent(Str);
				if(n==0)
				{
					if(size.cx>=Dx-Dx0)
					{
						pDC->TextOut(rect.left +5+Dx0,y,Str);
						m_OkText+=Str+char(13);
						y+=Dy;
						Str="";
						n++;
					}
				}
				else
				{
					if(size.cx>=Dx)
					{
						pDC->TextOut(rect.left +5,y,Str);
						m_OkText+=Str+char(13);
						y+=Dy;
						Str="";
					}
				}
				j++;
			}
			if(n==0)
			{
				pDC->TextOut(rect.left +5+Dx0,y,Str);
				m_OkText+=Str+char(13);
			}
			else
			{
				pDC->TextOut(rect.left +5,y,Str);
				m_OkText+=Str+char(13);
			}
			y+=Dy;
			if(m==-1) break;
			DataListStr=DataListStr.Mid(m+2);				
		}
	}
	pDC->SelectObject(pOldFont);
}

IMPLEMENT_SERIAL(CLthNoteObj, CFIDDrawObj, 0)

CLthNoteObj::CLthNoteObj()
{
	m_DrawShape = lthnoteobj;
	m_LthNoteColor=RGB(0,0,0);
	bFlush=TRUE;
}

CLthNoteObj::CLthNoteObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDoc)
		:CFIDDrawObj(strPathName,rect,pParentObj,pDoc)
{
	m_DrawDirect = VERDIR;
	m_DrawShape = lthnoteobj;
	m_AliasName = "岩性描述";
	m_LthNoteFont=m_NameFont;
	m_LthNoteColor=RGB(0,0,0);
	m_NameFont.lfHeight = -16;
	bFlush=TRUE;
}

CLthNoteObj::~CLthNoteObj()
{
	POSITION pos = m_LthNotePoint.GetHeadPosition();
	while (pos != NULL) delete m_LthNotePoint.GetNext(pos);
	m_LthNotePoint.RemoveAll();
}    

void CLthNoteObj::CreatePoint()
{
	CRect rect = m_Rect;                                     
	POSITION pos = m_LthNotePoint.GetHeadPosition();
	while (pos != NULL) 
		delete m_LthNotePoint.GetNext(pos);
	m_LthNotePoint.RemoveAll();
	
	int xyz=m_strPathName.Find (".List");
	if((xyz>0)&&DetectFileExist(m_strPathName))
	{
		TCHAR *token, *end;
		TCHAR buffer[1024];
		CStdioFile File;
		CFileException ex;
		if (!File.Open(m_strPathName, CFile::modeRead | CFile::typeText)) 
			return;
		int x,i=0;
		// Read Header off file
		File.ReadString(buffer, 1024);
		double dep;
		CString str,NoteStr;
		unsigned long m_ColorNo=0x08000;
		while (File.ReadString(buffer, 1024)) 
		{
			// Get first token
			for (token=buffer, end=buffer; 
				 *end && (*end != _T(',')) && (*end != _T('\n')); end++);
			if ((*end == _T('\0')) && (token == end)) token = NULL;
			*end = _T('\0');
			
			i=0;dep=0;
			while (token) 
			{
				str=token;
				while(str.Left(1)==" ")
					str.TrimLeft (" ");
				while(str.Right(1)==" ")
					str.TrimRight (" ");
				if(i==1)
				{
					x=str.Find ("--");
					if(x>0)
						str=str.Left (x);
					dep=atof(str);
				}
				else if(i==2)
					NoteStr=str;
				// Get next token
				for (token=++end; *end && (*end != _T(',')) && (*end != _T('\n'));
					 end++);
				if ((*end == _T('\0')) && (token == end)) token = NULL;
				*end = _T('\0');
				i++;
			}
			if((dep<m_PlotEdep)&&(dep>m_PlotSdep)&&(!NoteStr.IsEmpty ()))
			{
				rect = m_Rect;
				rect.top = DepthToYCoor(dep);
				rect.bottom = DepthToYCoor(dep+50.);
				CLthNotePoint* pObj = new CLthNotePoint(rect,m_LthNoteColor,m_LthNoteFont,NoteStr,pDoc);
				m_LthNotePoint.AddTail(pObj);
			}
		}
		File.Close();
	}
	bFlush = FALSE;
}

void CLthNoteObj::Serialize(CArchive& ar)
{
	CFIDDrawObj::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << (LONG) m_LthNoteColor;
		SerializeLogFont(ar,m_LthNoteFont);
	}
	else
	{
		long l;
		ar >> l; m_LthNoteColor = (COLORREF) l;
		SerializeLogFont(ar,m_LthNoteFont);
		bFlush = TRUE;
	}	
}

void CLthNoteObj::SetRect(const CRect& rect)
{
	CPoint delta;    
	CRect rect1;
	CLthNotePoint* pObj;
	delta.x = rect.right - m_Rect.right;
	delta.y = rect.top - m_Rect.top;
	POSITION pos = m_LthNotePoint.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CLthNotePoint*) m_LthNotePoint.GetNext(pos);
		rect1 = pObj->GetRect() + delta;
		rect1.left = rect.left;
		rect1.right = rect.right;
		pObj->SetRect(rect1);
	}                                         
	CFIDDrawObj::SetRect(rect);
}       

void CLthNoteObj::DrawHead(CDC* pDC)
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
	CPoint pt;
	if (m_NameFont.lfEscapement == 2700)
	{		
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
		int AliasLen=m_AliasName.GetLength ();
		pt.x = (rect.left+rect.right) / 2;
		pt.y =  (rect.top + rect.bottom-size.cy*AliasLen/2)/2+size.cy;
		CString Str;
		for(int i=0;i<AliasLen;i+=2)
		{
			Str=m_AliasName.Mid (i,2);
			pDC->TextOut(pt.x,pt.y+i*size.cy/2,Str);
		}
	}
	pDC->SelectObject(pOldFont);
}   

void CLthNoteObj::DrawClient(CDC* pDC)
{
	CFIDDrawObj::DrawClient(pDC); 
	CRect rect = m_Rect;
	pDoc->LPtoPP(&rect);
	if (pDC->RectVisible(rect))
	{		  
		if (bFlush) CreatePoint();
		CLthNotePoint* pObj;                                  
		POSITION pos = m_LthNotePoint.GetHeadPosition();
		while (pos != NULL)
		{
			pObj = (CLthNotePoint*) m_LthNotePoint.GetNext(pos);
			pObj->Draw(pDC);
		}
	}
}

BOOL CLthNoteObj::Properties(CView* pView)
{
	/*CString szHT;
	CPropertySheet m_Sheet("岩性描述属性");
	CLayerPage m_LthNoteBasePage;
	m_LthNoteBasePage.m_FileName=m_strPathName;
	m_Sheet.AddPage(&m_LthNoteBasePage);

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

	TEXTFONT m_TextFont1;
	m_TextFont1.m_ItemName = "描述内容";
	m_TextFont1.m_Text = "描述内容";
	m_TextFont1.m_ForeGround = m_LthNoteColor;
	m_TextFont1.m_BackGround = RGB(255,255,255);
	m_TextFont1.lf = m_LthNoteFont;

	CMinFontPage m_FontPage;
	m_FontPage.AddFont(&m_TextFont);
	m_FontPage.AddFont(&m_TextFont1);
	m_Sheet.AddPage(&m_FontPage);
	if (m_Sheet.DoModal() == IDOK)
	{
		bFlush = TRUE;
		if (m_strPathName != m_LthNoteBasePage.m_FileName)
			m_strPathName = m_LthNoteBasePage.m_FileName;
		m_FontPage.OnOK();
		m_NameColor = m_TextFont.m_ForeGround;
		m_NameFont = m_TextFont.lf;
		m_AliasName=m_TextFont.m_Text ;

		m_LthNoteColor=m_TextFont1.m_ForeGround;
		m_LthNoteFont=m_TextFont1.lf;

		CRect m_SaveRect = m_Rect;
		m_SaveRect.top = m_HeadRect.top;
		pDoc->SetModifiedFlag();
		InvalidateRect(m_SaveRect);
		return TRUE;
	}*/
	return FALSE;
}

int CLthNoteObj::HitTest(CView* pView,const CPoint& point)
{
	int nHandle = CFIDDrawObj::HitTest(pView,point); 
	if ((nHandle >= 0) || (!m_Rect.PtInRect(point))) return nHandle;
	if(m_Rect.PtInRect(point)) 
		return 9;
	else return -1;
}