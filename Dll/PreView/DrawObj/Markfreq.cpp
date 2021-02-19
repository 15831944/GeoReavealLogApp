/*************************************************************************
*   AzimFreqDraw.cpp - implementation of the AzimFreq draw object class  *           
*************************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "TrackDraw.h"
#include "GeoFIDio.h"
#include "MarkFreq.h"

//#include "LineListPage.h"
//#include "FillListPage.h"
//#include "MarkAziPage.h"
//#include "MinFontDialog.h"
//#include "HandWorkPage.h"

CMarkPoint::CMarkPoint(float Dep,float Dip,float Dir,CString Flag)
{
	m_Dep=Dep;
	m_Dip=Dip;
	m_Dir=Dir;
	m_Flag=Flag;
}

IMPLEMENT_SERIAL(CMarkFreqObj, CFIDDrawObj, 0)
        
CMarkFreqObj::CMarkFreqObj()
{
	m_DrawShape = Markfreqobj;
	m_PieWidth = 1;
	m_PieColor = RGB(255,0,0);
	m_bDrawDipFreq=FALSE;
	m_PlotContent=_T("");
}
    
CMarkFreqObj::CMarkFreqObj(CString szPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDoc)
			:CFIDDrawObj(szPathName,rect,pParentObj,pDoc)
{
	m_DrawShape = Markfreqobj;
	m_bDrawDipFreq=FALSE;
	m_AliasName="统计频率";
	m_NameFont.lfHeight = -20;
	m_PieBrush.lbStyle = BS_SOLID;
	m_PieBrush.lbColor = 0xFFCA5B;
	m_PieBrush.lbHatch = HS_HORIZONTAL;
	m_CircleWidth = 1;
	m_CircleColor = RGB(0,255,0);
	m_CircleRadius = 50;
	m_PieWidth = 1;
	m_PieColor = RGB(255,0,0);
	m_DangBaseLineWidth = 2;
	m_DangBaseLineColor = RGB(0,0,255);
	m_DangBaseLineLength = 100;
	m_DangFreqLineWidth = 2;
	m_DangFreqLineColor = RGB(0,0,255);
	m_DangFreqLineLength = 60;
	m_AzimNum = 36; m_DangNum = 18;
	m_DepLevel = 5;
	m_LeftVal = 0.f; m_RightVal=90.f;
	m_PlotContent=_T("");
}

CMarkFreqObj::~CMarkFreqObj()
{
	POSITION pos = m_MarkPoint.GetHeadPosition();
	while (pos != NULL) 
		delete m_MarkPoint.GetNext(pos);
	m_MarkPoint.RemoveAll();
}    

void CMarkFreqObj::Serialize( CArchive& ar )
{
	CFIDDrawObj::Serialize( ar );
	if( ar.IsStoring() )
	{
		ar << m_CircleRadius;
		ar << m_CircleWidth;
		ar << (long)m_PieColor;
		ar << m_PieWidth;
		ar << (long)m_CircleColor;
		ar << m_DangBaseLineLength;
		ar << m_DangBaseLineWidth;
		ar << (long) m_DangBaseLineColor;
		ar << m_DangFreqLineLength;
		ar << m_DangFreqLineWidth;
		ar << (long) m_DangFreqLineColor;
		ar << m_AzimNum;
		ar << m_DangNum;
		ar << m_DepLevel;
		ar << m_PlotContent;;
		ar << m_bDrawDipFreq;
		SerializeLogBrush(ar,m_PieBrush);
	}
	else
	{
		long l;
		ar >> m_CircleRadius;
		ar >> m_CircleWidth;
		ar >> l;  m_PieColor= (COLORREF)l;
		ar >> m_PieWidth;
		ar >> l;  m_CircleColor= (COLORREF)l;
		ar >> m_DangBaseLineLength;
		ar >> m_DangBaseLineWidth;
		ar >> l; m_DangBaseLineColor = (COLORREF)l;
		ar >> m_DangFreqLineLength;
		ar >> m_DangFreqLineWidth;
		ar >> l; m_DangFreqLineColor = (COLORREF)l;
		ar >> m_AzimNum;
		ar >> m_DangNum;
		ar >> m_DepLevel;
		ar >> m_PlotContent;
		ar >> m_bDrawDipFreq;
		SerializeLogBrush(ar,m_PieBrush);
		m_PieBrush.lbColor =0xFFCA5B;
		bFlush=TRUE;
	}
}

void CMarkFreqObj::CreatePoint()
{
	POSITION pos = m_MarkPoint.GetHeadPosition();
	while (pos != NULL) 
		delete m_MarkPoint.GetNext(pos);
	m_MarkPoint.RemoveAll();
	if(DetectFileExist(m_strPathName))
	{
		bFlush=FALSE;
		m_PointNum=0;
		float Dep,Dip,Dir,Dip0,Dir0;
		CString Flag;
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
				if(Dip0>0.005f&&Dir0>0.05f&&m_PlotContent.Find(Flag)>-1)
				{						
					CMarkPoint* pObj1=new CMarkPoint(Dep,Dip0,Dir0,Flag);
					m_MarkPoint.AddTail(pObj1);
					m_PointNum++;
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

void CMarkFreqObj::DrawClient(CDC* pDC)
{    
	CRect rect,m_ClipRect,m_AzimRect,m_DangRect;
	rect = m_Rect;
	pDoc->LPtoPP(rect);
	if (!pDC->RectVisible(rect)) return;
	CFIDDrawObj::DrawClient(pDC);   
	if(bFlush) CreatePoint();	
	if (m_PointNum == 0) return;
	pDC->GetClipBox(m_ClipRect);
	CRect m_SaveClipRect = m_ClipRect;
	m_ClipRect &= rect;
	m_ClipRect.InflateRect(0,2);
	
	int m_TopDepth =(int)m_PlotSdep;
	int m_BottomDepth =(int)m_PlotEdep;

	int x = (m_Rect.left + m_Rect.right) / 2;
	int y;
	m_AzimRect.left = m_DangRect.left = x - m_CircleRadius;
	m_AzimRect.right = m_DangRect.right = x + m_CircleRadius;
	m_DangRect.left = x - m_DangBaseLineLength / 2;
	m_DangRect.right = m_DangRect.left + m_DangBaseLineLength;
	int yTop = DepthToYCoor(m_TopDepth);
	int yLevel = DepthToYCoor(m_TopDepth + m_DepLevel) - yTop;
	yTop -= pDoc->m_WinOrg.y;
	m_AzimRect.top = yTop + yLevel/2 - m_CircleRadius;
	m_AzimRect.bottom = m_AzimRect.top + 2 * m_CircleRadius;
	m_DangRect.top = m_AzimRect.bottom+10;
	m_DangRect.bottom = m_DangRect.top + m_DangFreqLineLength;

	BOOL bClip = FALSE;
	if ((m_SaveClipRect.top < rect.top) || (m_SaveClipRect.bottom > rect.bottom))
	{
		CRect rect2 = m_ClipRect;
		if (pDC->m_hDC != NULL) ::LPtoDP(pDC->m_hDC,(LPPOINT)(LPRECT)rect2,2);
		CRgn rgn1; rgn1.CreateRectRgn(rect2.left,rect2.top,rect2.right,rect2.bottom);
		pDC->SelectClipRgn(&rgn1);
		bClip = TRUE;
	}

	double m_AzimLevel = 360.0 / m_AzimNum;
	double m_DangLevel = (m_RightVal-m_LeftVal) / m_DangNum;
	float dep0,dep1,dang,ddir;
	while (m_AzimRect.top < m_ClipRect.bottom)
	{                
		if (m_DangRect.bottom > m_ClipRect.top)
		{
			dep0=(float)m_TopDepth;;
			dep1=dep0+ m_DepLevel;
			for (int i=0; i<m_AzimNum; i++) 
				m_AzimFreq[i] = 0;
			m_MaxAzimFreq = 0;

			for (i=0; i<m_DangNum; i++) 
				m_DangFreq[i] = 0;
			m_MaxDangFreq = 0;
			
			CMarkPoint* pObj;
			POSITION pos = m_MarkPoint.GetHeadPosition();
			while (pos != NULL)
			{
				pObj = (CMarkPoint*) m_MarkPoint.GetNext(pos);
				if(pObj->m_Dep<dep0) continue;
				if(pObj->m_Dep>dep1) continue;
				dang=pObj->m_Dip;
				ddir=pObj->m_Dir;
				
				if((ddir==-9999.f)||(ddir==-999.25f)) 
					continue;
				
				i = (int)(ddir/m_AzimLevel)-1;
				if(i<0||i>m_AzimNum)
					continue;
				m_AzimFreq[i]++;
				if(m_MaxAzimFreq<m_AzimFreq[i]) 	
					m_MaxAzimFreq=m_AzimFreq[i];

				if((dang==-9999.f)||(dang==-999.25f)) 
					continue;
				if(!((dang<=m_RightVal)&&(dang>= m_LeftVal))) 
					continue;
				i = (int)(dang/m_DangLevel)-1;
				if(i<0||i>m_DangNum)
					continue;
				m_DangFreq[i]++;
				if(m_MaxDangFreq<m_DangFreq[i]) 
					m_MaxDangFreq=m_DangFreq[i];
	  		}

			CPen m_Pen;	
			CPen *pOldPen;
			m_Pen.CreatePen(0,m_CircleWidth,m_CircleColor);
			pOldPen = pDC->SelectObject(&m_Pen);
			CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);	
			if((m_MaxAzimFreq>0)&&(m_CircleWidth>0))
			{		
				pDC->Ellipse(m_AzimRect);			
				pDC->MoveTo (m_AzimRect.left ,m_AzimRect.top +m_AzimRect.Height ()/2);
				pDC->LineTo (m_AzimRect.right ,m_AzimRect.top +m_AzimRect.Height ()/2);
				pDC->MoveTo (m_AzimRect.left+m_AzimRect.Width ()/2 ,m_AzimRect.top );
				pDC->LineTo (m_AzimRect.left+m_AzimRect.Width ()/2  ,m_AzimRect.bottom );
			}

			x = (m_AzimRect.left + m_AzimRect.right) / 2;
			y = (m_AzimRect.top + m_AzimRect.bottom) / 2;
			CBrush brush;
			brush.CreateSolidBrush(m_PieColor);
			pDC->SelectObject(&brush);
			//绘制方位频率图
			if (m_MaxAzimFreq>0)
			{
				CPoint *ptPie;
				ptPie=new CPoint[m_AzimNum*2+2];
				int x1,x2,y1,y2,x3,y3;			
				for (int i=0; i<m_AzimNum; i++)
			    {
					double r = (double)m_CircleRadius  * m_AzimFreq[i] / m_MaxAzimFreq;
					x1 = x + Round(r*sin(i*m_AzimLevel/180.0*PI));
					y1 = y - Round(r*cos(i*m_AzimLevel/180.0*PI));
					if(i>0)
					{
						ptPie[i*2].x=x1;ptPie[i*2].y=y1;
					}
					else 
					{
						ptPie[i*2].x=x1;ptPie[i*2].y=y1;
						x3 = x1; y3 = y1;
						ptPie[m_AzimNum*2].x=x1;ptPie[m_AzimNum*2].y=y1;
					}
					x2 = x + Round(r*sin((i+1)*m_AzimLevel/180.0*PI));
					y2 = y - Round(r*cos((i+1)*m_AzimLevel/180.0*PI));
					ptPie[i*2+1].x=x2;ptPie[i*2+1].y=y2;
				}
				pDC->Polygon(ptPie,m_AzimNum*2+1);
				delete ptPie;
			}
			else
				pDC->Rectangle(x-5,y-5,x+5,y+5);
			//绘制角度频率图
			if(m_bDrawDipFreq&&(m_MaxDangFreq > 0))
			{
				m_Pen.DeleteObject();
				m_Pen.CreatePen(0,m_DangBaseLineWidth,m_DangBaseLineColor);
				pDC->SelectObject(&m_Pen);
				pDC->MoveTo(m_DangRect.left,m_DangRect.bottom);
				pDC->LineTo(m_DangRect.right,m_DangRect.bottom);
			
				y = (m_AzimRect.top + m_AzimRect.bottom) / 2;
				m_Pen.DeleteObject();
				m_Pen.CreatePen(0,m_DangFreqLineWidth,m_DangFreqLineColor);
				pDC->SelectObject(&m_Pen);
				for (i=0; i<m_DangNum; i++)
			    {
					int r = Round(((double)m_DangFreqLineLength) * m_DangFreq[i] / m_MaxDangFreq) ;
					x = m_DangRect.left + i * m_DangBaseLineLength / m_DangNum;
					pDC->MoveTo(x,m_DangRect.bottom);
					pDC->LineTo(x,m_DangRect.bottom - r);
				}
			}
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(pOldPen);
		}
		m_TopDepth += m_DepLevel; m_BottomDepth += m_DepLevel;
		yTop = DepthToYCoor(m_TopDepth);
		yTop -= pDoc->m_WinOrg.y;
		m_AzimRect.top = yTop + yLevel/2 - m_CircleRadius;
		m_AzimRect.bottom = m_AzimRect.top + 2 * m_CircleRadius;
		m_DangRect.top = m_AzimRect.bottom+10;
		m_DangRect.bottom = m_DangRect.top + m_DangFreqLineLength;
	}

	if (bClip)
	{
		rect = m_SaveClipRect;
		if (pDC->m_hDC != NULL) ::LPtoDP(pDC->m_hDC,(LPPOINT)(LPRECT)rect,2);
		CRgn rgn2; rgn2.CreateRectRgn(rect.left,rect.top,rect.right,rect.bottom);
		pDC->SelectClipRgn(&rgn2);
	}
}

void CMarkFreqObj::DrawHead(CDC* pDC)
{
	CRect rect = m_HeadRect;
	pDoc->LPtoPP(rect);
	if(pDC->RectVisible(rect))
	{
		CFIDDrawObj::DrawHead(pDC);   
		CFont m_Font;
		m_Font.CreateFontIndirect(&m_NameFont);
		CFont* pOldFont = pDC->SelectObject(&m_Font);
   		CSize size = pDC->GetTextExtent("文本");
	   	pDC->SetTextColor(m_NameColor);  
		pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
		pDC->SetBkMode(TRANSPARENT); 
		pDC->TextOut((rect.left+rect.right)/2,(rect.top + rect.bottom + size.cy)/2,m_AliasName);
		pDC->SelectObject(pOldFont);
	}
}                              

BOOL CMarkFreqObj::Properties(CView* pView)
{
	//CPropertySheet m_Sheet("频率统计图");
	//CHandWorkPage  m_HandWorkPage;
	//m_HandWorkPage.m_FileName =m_strPathName;
	//m_HandWorkPage.m_PlotContent=m_PlotContent;
	//m_Sheet.AddPage(&m_HandWorkPage);
	//CMarkDipPage m_pDangPage(this);
	//CMarkAziPage m_pAzimPage(this);
	//TEXTFONT m_TextFont;
	//CMinFontPage m_FontPage;

	//m_Sheet.AddPage(&m_pDangPage);
	//m_Sheet.AddPage(&m_pAzimPage);

	//CLineListPage m_LineList;
	//CString szHT;
	//szHT.LoadString(IDS_FRAMELINE);
	//m_LineList.AddPen(&m_FramePen,szHT);
	//m_Sheet.AddPage(&m_LineList);
	//
	//m_Sheet.AddPage(&m_FontPage);

	//m_TextFont.m_ItemName = m_AliasName;
	//m_TextFont.m_Text = m_AliasName;
	//m_TextFont.m_ForeGround = m_NameColor;
	//m_TextFont.m_BackGround = RGB(255,255,255);
	//m_TextFont.lf = m_NameFont;
	//m_FontPage.AddFont(&m_TextFont);
	//if(m_Sheet.DoModal() == IDOK)
	//{
	//	m_FontPage.OnOK();
	//	m_NameColor = m_TextFont.m_ForeGround;
	//	m_NameFont = m_TextFont.lf;
	//	m_NameFont.lfEscapement = 0;
	//	m_AliasName=m_TextFont.m_Text;
	//	CRect m_SaveRect = m_Rect;
	//	m_SaveRect.top = m_HeadRect.top;
	//	m_strPathName=m_HandWorkPage.m_FileName;
	//	m_PlotContent=m_HandWorkPage.m_PlotContent;
	//	bFlush = TRUE;
	//	if (m_ParentType != 0) 
	//		m_SaveRect.bottom = m_Rect.bottom;
	//	pDoc->SetModifiedFlag();
	//	InvalidateRect(m_SaveRect);
	//	return TRUE;
	//}
	return FALSE;
}

int CMarkFreqObj::HitTest(CView* pView,const CPoint& point)
{
	int nHandle = CFIDDrawObj::HitTest(pView,point); 
	if ((nHandle >= 0) || (!m_Rect.PtInRect(point))) return nHandle;
	if(m_Rect.PtInRect(point)) 
		return 9;
	else return -1;
}


