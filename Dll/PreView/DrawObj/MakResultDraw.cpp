/******************************************************************
*   ResulDrw.cpp - implementation of the MakResult draw object class *           
*******************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "GeoFIDio.h"
#include "TrackDraw.h"
#include "MakResultDraw.h"

//#include "StyleDialog.h"
//#include "MinFontDialog.h"
//#include "LineListPage.h"
//#include "FillListPage.h"
//#include "FIDBasePage.h"
//#include "ResultPage.h"
//#include "resource.h"

CMakResultPoint::CMakResultPoint(CRect rect, COLORREF MakResultColor,LOGFONT MakResultFont,CString nMakResultNo,
						   CString NoStr,int nDirect,CLogPlotDoc* pDocument)
{
	pDoc = pDocument;
	m_MakResultNo = nMakResultNo;
	m_NoStr=NoStr;
	m_Direct = nDirect;
	m_Rect = rect;
	m_MakResultColor=MakResultColor;
	m_MakResultFont=MakResultFont;
}

void CMakResultPoint::Draw(CDC* pDC)
{
	CRect rect = m_Rect;		
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;
	CRect r1(rect.left ,rect.top ,rect.left +rect.Width()/2,rect.bottom);
	CRect r2(rect.left +rect.Width()/2,rect.top ,rect.right,rect.bottom);
	int m_LithNo=2,m_ColorNo=RGB(255,255,255);
	if(m_MakResultNo!="无水泥")
	{
		if(m_MakResultNo=="完全胶结")
		{
			m_LithNo=3;
			m_ColorNo=RGB(168,169,173);
		}
		else if(m_MakResultNo=="部分胶结")
		{
			m_LithNo=57;
			m_ColorNo=RGB(117,217,163);
		}
		else if(m_MakResultNo=="无法确定")
			m_LithNo=96;
		LithFillBox(r1,m_LithNo,0x0,rect.TopLeft(),pDC,m_ColorNo);
	}
	if(m_NoStr!="无水泥")
	{
		m_LithNo=2;m_ColorNo=RGB(255,255,255);
		if(m_NoStr=="完全胶结")
		{
			m_LithNo=3;
			m_ColorNo=RGB(168,169,173);
		}
		else if(m_NoStr=="部分胶结")
		{
			m_LithNo=57;
			m_ColorNo=RGB(117,217,163);
		}
		else if(m_NoStr=="无法确定")
			m_LithNo=96;
		LithFillBox(r2,m_LithNo,0x0,rect.TopLeft(),pDC,m_ColorNo);
	}
}

IMPLEMENT_SERIAL(CMakResultObj, CFIDDrawObj, 0)

CMakResultObj::CMakResultObj()
{
	m_DrawShape = resultobj;
	bRButtonDown = FALSE;
	bFlush = TRUE;
}

CMakResultObj::CMakResultObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDoc)
		:CFIDDrawObj(strPathName,rect,pParentObj,pDoc)
{
	bRButtonDown = FALSE;
	m_HeadRect = rect;
	m_HeadRect.bottom = m_HeadRect.top + 100;
	m_HeadRect.right = m_HeadRect.left + 40;
	m_MakResultLR= IDM_LEFT;
	m_Rect = m_HeadRect;
	m_Rect.top = m_HeadRect.bottom;
	m_DrawDirect = VERDIR;
	m_DrawShape = resultobj;
    m_NameFont.lfHeight = -20;
	m_NameFont.lfEscapement = 2700;
    m_MakResultOrderFont.lfHeight = -14;
	strcpy(m_MakResultOrderFont.lfFaceName,"宋体"); 
	m_MakResultOrderFont=m_NameFont;
	
	m_MakResultOrderColor = RGB(0,0,0);
	m_AliasName = "水泥胶结解释结论";
	bFlush = TRUE;
	m_FramePen.lopnWidth = CPoint(2,2); 
	m_FramePen.lopnColor = RGB(0,0x80,0); 
	m_FramePen.lopnStyle = 0;
}

CMakResultObj::~CMakResultObj()
{
	POSITION pos = m_MakResultPoint.GetHeadPosition();
	while (pos != NULL) delete m_MakResultPoint.GetNext(pos);
	m_MakResultPoint.RemoveAll();
}    

void CMakResultObj::CreatePoint()
{
	CRect rect;
	rect.left = m_Rect.left;
	rect.right = m_Rect.right;

	POSITION pos = m_MakResultPoint.GetHeadPosition();
	while (pos != NULL) 
		delete m_MakResultPoint.GetNext(pos);
	m_MakResultPoint.RemoveAll();
	CString str,MakResultstr;
	int MakResultNum=-1;
	int MakResultNum2=-1;
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
			if(str=="第一界面")
				MakResultNum=i;
			if(str=="第二界面")
				MakResultNum2=i;
			// Get next token
			for (token=++end; *end && (*end != _T(',')) && (*end != _T('\n'));
				 end++);
			if ((*end == _T('\0')) && (token == end)) token = NULL;
			*end = _T('\0');
			i++;
		}
		if(MakResultNum<0||MakResultNum2<0)
		{
			File.Close();return;
		}
		CString NoStr;
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
				if(i==MakResultNum2)
					NoStr=str;
				else if(i==MakResultNum)
				    MakResultstr=str;
				else if(DepFlag)
				{
					xx=str.Find ("--");
					if((xx>0)&&(DepFlag))
					{
						m_Sdep=atof(str.Left (xx));
						m_Edep=atof(str.Mid (xx+2));
						if(m_Sdep>m_PlotSdep||m_Edep<m_PlotEdep)
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
			if(!DepFlag)
			{
				rect.top = DepthToYCoor(m_Sdep);
				rect.bottom = DepthToYCoor(m_Edep);
				if(Remark==-1)
					str=MakResultstr;
				else
					str.Format ("%d",Remark);
				CMakResultPoint* pObj=NULL;
				pObj = new CMakResultPoint(rect,m_MakResultOrderColor,m_MakResultOrderFont,str,NoStr,m_MakResultLR,pDoc);
				m_MakResultPoint.AddTail(pObj);
			}
		}
		File.Close();
	}
	bFlush = FALSE;
}

void CMakResultObj::Serialize(CArchive& ar)
{                      
	WORD w;
	CFIDDrawObj::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << (WORD) m_MakResultLR;
		ar << (LONG) m_MakResultOrderColor;
		SerializeLogFont(ar,m_MakResultOrderFont);
	}
	else
	{
		long l;
		ar >> w; m_MakResultLR = (int) w;
		ar >> l; m_MakResultOrderColor = (COLORREF) l;
		SerializeLogFont(ar,m_MakResultOrderFont);
		bFlush = TRUE;
	}	
}

void CMakResultObj::SetRect(const CRect& rect)
{
	CPoint delta;    
	CMakResultPoint* pObj;
	CRect rect1;
	delta.x = rect.right - m_Rect.right;
	delta.y = rect.top - m_Rect.top;
	int cx =20;
	POSITION pos = m_MakResultPoint.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CMakResultPoint*) m_MakResultPoint.GetNext(pos);
		rect1 = pObj->GetRect() + delta;
		rect1.left = rect.left;
		rect1.right = rect.right;
		pObj->SetRect(rect1);
	}          
	CFIDDrawObj::SetRect(rect);
}       

BOOL CMakResultObj::Properties(CView* pView)
{
	/*CString szHT;
	szHT.LoadString(IDS_DRAWINGPROPERTIES);
	CPropertySheet m_Sheet("水泥胶结评价标注属性");
	CResultPage m_MakResultPage;
	m_MakResultPage.m_FileName=m_strPathName;
	m_Sheet.AddPage(&m_MakResultPage);
	CLineListPage m_LineList;
	CFillListPage m_FillList;
 	if ((m_ParentType != 0) || (m_DrawDirect == VERDIR))
	{
		szHT.LoadString(IDS_FRAMELINE);
		m_LineList.AddPen(&m_FramePen,szHT);
		szHT.LoadString(IDS_FRAMEFILL);
		m_FillList.AddFill(&m_Brush,szHT);
		m_Sheet.AddPage(&m_LineList);
		m_Sheet.AddPage(&m_FillList);
	}
	CMinFontPage m_FontPage;
	TEXTFONT m_TextFont[2];
	if(m_AliasName=="") m_AliasName="标题";
	m_TextFont[0].m_ItemName = m_AliasName;
	m_TextFont[0].m_Text = m_AliasName;
	m_TextFont[0].m_ForeGround = m_NameColor;
	m_TextFont[0].m_BackGround = RGB(255,255,255);
	m_TextFont[0].lf = m_NameFont;
	m_FontPage.AddFont(&m_TextFont[0]);
	szHT.LoadString(IDS_RESULTFONT);
	m_TextFont[1].m_ItemName = szHT.GetBuffer(128);
	szHT.ReleaseBuffer();
	m_TextFont[1].m_Text = "12345";
	m_TextFont[1].m_ForeGround = m_MakResultOrderColor;
	m_TextFont[1].m_BackGround = RGB(255,255,255);
	m_TextFont[1].lf = m_MakResultOrderFont;
	m_FontPage.AddFont(&m_TextFont[1]);
	m_Sheet.AddPage(&m_FontPage);
	double m_PlotSdep1 = m_PlotSdep;
	double m_PlotEdep1 = m_PlotEdep;
	if (m_Sheet.DoModal() == IDOK)
	{
		m_FontPage.OnOK();
		m_AliasName=m_TextFont[0].m_Text ;
		m_NameColor = m_TextFont[0].m_ForeGround;
		m_NameFont = m_TextFont[0].lf;
		m_MakResultOrderFont = m_TextFont[1].lf;
		m_MakResultOrderColor = m_TextFont[1].m_ForeGround;
		if (m_strPathName != m_MakResultPage.m_FileName)
			m_strPathName = m_MakResultPage.m_FileName;
		CRect SaveRect = m_Rect;
		SaveRect.top = m_HeadRect.top;
		bFlush=TRUE;
		if (m_ParentType != 0) 
			SaveRect.bottom = m_Rect.bottom;
		InvalidateRect(SaveRect);
		pDoc->SetModifiedFlag();
		return TRUE;
	}*/
	return FALSE;
}                

void CMakResultObj::DrawHead(CDC* pDC)
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
	CRect r1=rect;
	r1.right =rect.left +rect.Width()/2;
	CRect r2=rect;
	r2.left=rect.left +rect.Width()/2;
	rect=r1;
	CString Name="第一界面";
	if (m_NameFont.lfEscapement == 2700)
	{		
		pt.x = (rect.left+rect.right) / 2;
		pt.y =  (rect.top + rect.bottom) / 2;
		pDC->LPtoDP(&pt);
		int nSaveDC = pDC->SaveDC();
		pDC->SetMapMode(MM_TEXT);
		pDC->DPtoLP(&pt);
		pDC->TextOut(pt.x-size.cy/2, pt.y,Name);
		pDC->RestoreDC(nSaveDC);
	}
	else
	{
		int AliasLen=Name.GetLength ();
		pt.x = (rect.left+rect.right) / 2;
		pt.y =  (rect.top + rect.bottom-size.cy*AliasLen/4) / 2;
		CString Str;
		for(int i=0;i<AliasLen;i+=2)
		{
			Str=Name.Mid (i,2);
			pDC->TextOut(pt.x,pt.y+i*size.cy/2,Str);
		}
	}

	rect=r2;
	Name="第二界面";
	if (m_NameFont.lfEscapement == 2700)
	{		
		pt.x = (rect.left+rect.right) / 2;
		pt.y =  (rect.top + rect.bottom) / 2;
		pDC->LPtoDP(&pt);
		int nSaveDC = pDC->SaveDC();
		pDC->SetMapMode(MM_TEXT);
		pDC->DPtoLP(&pt);
		pDC->TextOut(pt.x-size.cy/2, pt.y,Name);
		pDC->RestoreDC(nSaveDC);
	}
	else
	{
		int AliasLen=Name.GetLength ();
		pt.x = (rect.left+rect.right) / 2;
		pt.y =  (rect.top + rect.bottom-size.cy*AliasLen/4)/2;
		CString Str;
		for(int i=0;i<AliasLen;i+=2)
		{
			Str=Name.Mid (i,2);
			pDC->TextOut(pt.x,pt.y+i*size.cy/2,Str);
		}
	}
	pDC->SelectObject(pOldFont);
}   

void CMakResultObj::DrawClient(CDC* pDC)
{
	CRect rect = m_Rect;
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;
	if(bFlush) CreatePoint();
	CMakResultPoint* pObj;
	POSITION pos = m_MakResultPoint.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CMakResultPoint*) m_MakResultPoint.GetNext(pos);
		CRect rect = pObj->m_Rect;
		pDoc->LPtoPP(&rect);
		CRect rect1 =m_ClientRect;
		pDoc->LPtoPP(&rect1);
		rect|=rect1;
		if (pDC->RectVisible(rect))
			pObj->Draw(pDC);
	}
	CFIDDrawObj::DrawClient(pDC);
}

int CMakResultObj::HitTest(CView* pView,const CPoint& point)
{
	int nHandle = CFIDDrawObj::HitTest(pView,point); 
	if ((nHandle >= 0) || (!m_Rect.PtInRect(point))) return nHandle;
	if(m_Rect.PtInRect(point)) 
		return 9;
	else return -1;
}
