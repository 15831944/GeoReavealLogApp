/******************************************************************
*   ResulDrw.cpp - implementation of the GrDenResult draw object class *           
*******************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "GeoFIDio.h"
#include "TrackDraw.h"
#include "GrDenResultDraw.h"

//#include "StyleDialog.h"
//#include "MinFontDialog.h"
//#include "LineListPage.h"
//#include "FillListPage.h"
//#include "FIDBasePage.h"
//#include "ResultPage.h"
//#include "resource.h"

CGrDenResultPoint::CGrDenResultPoint(CRect rect, COLORREF Color1,COLORREF Color2,COLORREF Color31,COLORREF Color32,
		              LOGFONT GrDenResultFont,CString nGrDenResultNo,CString NoStr,CLogPlotDoc *pDocument)
{
	pDoc = pDocument;
	m_GrDenResultNo = nGrDenResultNo;
	m_NoStr=NoStr;
	m_Rect = rect;
	m_Color1=Color1;
	m_Color2=Color2;
	m_Color31=Color31;
	m_Color32=Color32;
	m_GrDenResultFont=GrDenResultFont;
}

void CGrDenResultPoint::Draw(CDC* pDC)
{
	CRect rect = m_Rect;		
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;
	if(m_GrDenResultNo="")
	{
		CPen pen;
		pen.CreatePen(0,1,m_Color1);
        CPen *pOldPen=(CPen *)pDC->SelectObject(&pen);
		CBrush brush;
		brush.CreateSolidBrush(m_Color1);
		CBrush* pOldBrush = (CBrush*)pDC->SelectObject(&brush);
		CRect r=rect;
		r.right=r.left+(rect.Width()/3);
		pDC->Rectangle(r);
		brush.DeleteObject();
		pen.DeleteObject();

		pen.CreatePen(0,1,m_Color2);
        pDC->SelectObject(&pen);
		brush.CreateSolidBrush(m_Color2);
		pDC->SelectObject(&brush);
		r.left=r.right;
		r.right=r.left+(rect.Width()/3);
		pDC->Rectangle(r);
		brush.DeleteObject();
		pen.DeleteObject();

		pen.CreatePen(0,1,m_Color31);
        pDC->SelectObject(&pen);
		brush.CreateSolidBrush(m_Color31);
		pDC->SelectObject(&brush);
		r.left=r.right;
		r.right=rect.right;
		r.bottom=r.top+r.Height()/2;
		pDC->Rectangle(r);

		pen.CreatePen(0,1,m_Color32);
        pDC->SelectObject(&pen);
		brush.CreateSolidBrush(m_Color32);
		pDC->SelectObject(&brush);
		r.top=r.bottom;
		r.bottom=rect.bottom;
		pDC->Rectangle(r);

		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
	}
}

IMPLEMENT_SERIAL(CGrDenResultObj, CFIDDrawObj, 0)

CGrDenResultObj::CGrDenResultObj()
{
	m_DrawShape = resultobj;
	bRButtonDown = FALSE;
	bFlush = TRUE;
}

CGrDenResultObj::CGrDenResultObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDoc)
		:CFIDDrawObj(strPathName,rect,pParentObj,pDoc)
{
	bRButtonDown = FALSE;
	m_HeadRect = rect;
	m_HeadRect.bottom = m_HeadRect.top + 100;
	m_HeadRect.right = m_HeadRect.left + 40;
	m_GrDenResultLR= IDM_LEFT;
	m_Rect = m_HeadRect;
	m_Rect.top = m_HeadRect.bottom;
	m_DrawDirect = VERDIR;
	m_DrawShape = resultobj;
    m_NameFont.lfHeight = -20;
	m_NameFont.lfEscapement = 2700;
    m_GrDenResultOrderFont.lfHeight = -14;
	strcpy(m_GrDenResultOrderFont.lfFaceName,"宋体"); 
	m_GrDenResultOrderFont=m_NameFont;
	
	m_GrDenResultOrderColor = RGB(0,0,0);
	m_AliasName = "固井综合解释结论";
	bFlush = TRUE;
	m_FramePen.lopnWidth = CPoint(2,2); 
	m_FramePen.lopnColor = RGB(0,0x80,0); 
	m_FramePen.lopnStyle = 0;
}

CGrDenResultObj::~CGrDenResultObj()
{
	POSITION pos = m_GrDenResultPoint.GetHeadPosition();
	while (pos != NULL) delete m_GrDenResultPoint.GetNext(pos);
	m_GrDenResultPoint.RemoveAll();
}    

void CGrDenResultObj::CreatePoint()
{
	CRect rect;
	rect.left = m_Rect.left;
	rect.right = m_Rect.right;

	POSITION pos = m_GrDenResultPoint.GetHeadPosition();
	while (pos != NULL) 
		delete m_GrDenResultPoint.GetNext(pos);
	m_GrDenResultPoint.RemoveAll();
	CString str,GrDenResultstr;
	int GrDenResultNum=-1;
	int i=0,xx;
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
			if(str=="固井综合评价结论")
				GrDenResultNum=i;
			// Get next token
			for (token=++end; *end && (*end != _T(',')) && (*end != _T('\n'));
				 end++);
			if ((*end == _T('\0')) && (token == end)) token = NULL;
			*end = _T('\0');
			i++;
		}
		if(GrDenResultNum<0)
		{
			File.Close();return;
		}
		CString NoStr;
		bool DepFlag;
		double m_Sdep,m_Edep;	
		COLORREF Color1,Color2,Color31,Color32;
		while (File.ReadString(buffer, 1024)) 
		{
			// Get first token
			for (token=buffer, end=buffer; 
				 *end && (*end != _T(',')) && (*end != _T('\n')); end++);
			if ((*end == _T('\0')) && (token == end)) token = NULL;
			*end = _T('\0');
			
			DepFlag=TRUE;
			i=0;
			NoStr="";
			GrDenResultstr="";
			while (token) 
			{
				str=token;
				while(str.Left(1)==" ")
					str.TrimLeft (" ");
				while(str.Right(1)==" ")
					str.TrimRight (" ");
				if(i==GrDenResultNum)
					NoStr=str;
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
   			    if(NoStr=="无缺陷的波特兰水泥环")                                     //1
				{Color1=0x0;Color2=0x0;Color31=0x0;Color32=0x0;}
				else if(NoStr=="偏心无缺陷的波特兰水泥环")                            //2
				{Color1=0x0;Color2=0x0;Color31=0x008000;Color32=0x008000;}
				else if(NoStr=="不确定条件下无缺陷的波特兰水泥环")                    //3
				{Color1=0x0;Color2=0x0;Color31=0xFF00FF;Color32=0xFF00FF;}
				else if(NoStr=="波特兰水泥环第一界面间隙小于60微米")                  //4
				{Color1=0x008000;Color2=0x0;Color31=0xFF00FF;Color32=0xFF00FF;}
				else if(NoStr=="波特兰水泥环与套管无接触")                            //5
				{Color1=0xFFFFFF;Color2=0x0;Color31=0xFF00FF;Color32=0xFF00FF;}
				else if(NoStr=="波特兰水泥环体积缺陷小于45度")                        //6
				{Color1=0x00FFFF;Color2=0x0;Color31=0x0;Color32=0xFF00FF;}  
                else if(NoStr=="有混合类型缺陷的波特兰水泥环(接触型和体积型缺陷)")    //7
				{Color1=0xFF0080;Color2=0x0;Color31=0x0;Color32=0xFF00FF;}
				else if(NoStr=="波特兰水泥环体积缺陷大于45度")                        //8
				{Color1=0xFFFF00;Color2=0x0;Color31=0xFF00FF;Color32=0xFF00FF;}
				else if(NoStr=="无缺陷轻型水泥环")                                    //9
				{Color1=0x000080;Color2=0x000080;Color31=0x000080;Color32=0x000080;}
				else if(NoStr=="偏心无缺陷轻型水泥环")                                //10
				{Color1=0x000080;Color2=0x000080;Color31=0x008000;Color32=0x008000;}
				else if(NoStr=="不确定接触情形下的无缺陷轻型水泥环(第一界面)")        //11
				{Color1=0x000080;Color2=0x000080;Color31=0xFF00FF;Color32=0xFF00FF;}
				else if(NoStr=="轻型水泥环与第一界面间隙为0-60微米")                  //12
				{Color1=0x008000;Color2=0x000080;Color31=0xFF00FF;Color32=0xFF00FF;}
				else if(NoStr=="与套管无接触的轻型水泥环")                            //13
				{Color1=0xFFFFFF;Color2=0x000080;Color31=0xFF00FF;Color32=0xFF00FF;}
				else if(NoStr=="体积缺陷小于45度的轻型水泥环")                        //14
				{Color1=0x00FFFF;Color2=0x000080;Color31=0xFF00FF;Color32=0x000080;}
				else if(NoStr=="混合类型缺陷的轻型水泥环")                            //15
				{Color1=0xFF0080;Color2=0x000080;Color31=0xFF00FF;Color32=0x000080;}
				else if(NoStr=="体积缺陷张开度大于45度的轻型水泥环")                  //16
				{Color1=0xFFFF00;Color2=0x000080;Color31=0xFF00FF;Color32=0xFF00FF;}
				else if(NoStr=="空套管")                                              //17
				{Color1=0xFFFFFF;Color2=0xFFFFFF;Color31=0xFF00FF;Color32=0xFF00FF;}
				else
				{
					Color1=0xFFFFFF;Color2=0xFFFFFF;Color31=0xFFFFFF;Color32=0xFFFFFF;
					GrDenResultstr="BLANCK";
				}
				CGrDenResultPoint* pObj=NULL;
				pObj = new CGrDenResultPoint(rect,Color1,Color2,Color31,Color32,m_GrDenResultOrderFont,GrDenResultstr,NoStr,pDoc);
				m_GrDenResultPoint.AddTail(pObj);
			}
		}
		File.Close();
	}
	bFlush = FALSE;
}

void CGrDenResultObj::Serialize(CArchive& ar)
{                      
	WORD w;
	CFIDDrawObj::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << (WORD) m_GrDenResultLR;
		ar << (LONG) m_GrDenResultOrderColor;
		SerializeLogFont(ar,m_GrDenResultOrderFont);
	}
	else
	{
		long l;
		ar >> w; m_GrDenResultLR = (int) w;
		ar >> l; m_GrDenResultOrderColor = (COLORREF) l;
		SerializeLogFont(ar,m_GrDenResultOrderFont);
		bFlush = TRUE;
	}	
}

void CGrDenResultObj::SetRect(const CRect& rect)
{
	CPoint delta;    
	CGrDenResultPoint* pObj;
	CRect rect1;
	delta.x = rect.right - m_Rect.right;
	delta.y = rect.top - m_Rect.top;
	int cx =20;
	POSITION pos = m_GrDenResultPoint.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CGrDenResultPoint*) m_GrDenResultPoint.GetNext(pos);
		rect1 = pObj->GetRect() + delta;
		rect1.left = rect.left;
		rect1.right = rect.right;
		pObj->SetRect(rect1);
	}          
	CFIDDrawObj::SetRect(rect);
}       

BOOL CGrDenResultObj::Properties(CView* pView)
{
	/*CString szHT;
	szHT.LoadString(IDS_DRAWINGPROPERTIES);
	CPropertySheet m_Sheet("固井综合评价标注属性");
	CResultPage m_GrDenResultPage;
	m_GrDenResultPage.m_FileName=m_strPathName;
	m_Sheet.AddPage(&m_GrDenResultPage);
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
	m_TextFont[1].m_ForeGround = m_GrDenResultOrderColor;
	m_TextFont[1].m_BackGround = RGB(255,255,255);
	m_TextFont[1].lf = m_GrDenResultOrderFont;
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
		m_GrDenResultOrderFont = m_TextFont[1].lf;
		m_GrDenResultOrderColor = m_TextFont[1].m_ForeGround;
		if (m_strPathName != m_GrDenResultPage.m_FileName)
			m_strPathName = m_GrDenResultPage.m_FileName;
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

void CGrDenResultObj::DrawHead(CDC* pDC)
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
	CString Name="固井质量";
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
	Name="综合评价";
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

void CGrDenResultObj::DrawClient(CDC* pDC)
{
	CRect rect = m_Rect;
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;
	if(bFlush) CreatePoint();
	CGrDenResultPoint* pObj;
	POSITION pos = m_GrDenResultPoint.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CGrDenResultPoint*) m_GrDenResultPoint.GetNext(pos);
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

int CGrDenResultObj::HitTest(CView* pView,const CPoint& point)
{
	int nHandle = CFIDDrawObj::HitTest(pView,point); 
	if ((nHandle >= 0) || (!m_Rect.PtInRect(point))) return nHandle;
	if(m_Rect.PtInRect(point)) 
		return 9;
	else return -1;
}
