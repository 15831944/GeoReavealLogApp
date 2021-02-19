/*************************************************************************
*   AzimFreqDraw.cpp - implementation of the AzimFreq draw object class  *           
*************************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "TrackDraw.h"
#include "GeoFIDio.h"
#include "AzimFreq.h"
//
//#include "LineListPage.h"
//#include "FillListPage.h"
//#include "AzimFreqPage.h"
//#include "MinFontDialog.h"
//#include "FreqBasePage.h"

IMPLEMENT_SERIAL(CAzimFreqObj, CFIDDrawObj, 0)
        
CAzimFreqObj::CAzimFreqObj()
{
	m_DrawShape = azimfreqobj;
	ddir = NULL; dang = NULL; 
	m_DrawDirect = VERDIR;
	m_PieWidth = 1;
	m_PieColor = RGB(255,0,0);
}
    
CAzimFreqObj::CAzimFreqObj(CString szPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDoc)
			:CFIDDrawObj(szPathName,rect,pParentObj,pDoc)
{
	m_DrawShape = azimfreqobj;
	m_DrawDirect = VERDIR;
	m_AliasName="频率统计";
	m_NameFont.lfHeight = -20;
	ddir = NULL; dang = NULL; 
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
	m_FramePen.lopnWidth = CPoint(2,2);
	m_AzimNum = 36; m_DangNum = 18;
	m_DepLevel = 20;
	m_LeftVal = 0.f; m_RightVal=90.f;
	m_AziName ="DIR";
}

CAzimFreqObj::~CAzimFreqObj()
{
	if (ddir != NULL) delete ddir;
	if (dang != NULL) delete dang;
}    

void CAzimFreqObj::Serialize( CArchive& ar )
{
	CFIDDrawObj::Serialize( ar );
	if(ar.IsStoring() )
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
		ar << m_AziName;
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
		ar >> m_AziName;

		SerializeLogBrush(ar,m_PieBrush);
		m_PieBrush.lbColor =0xFFCA5B;
		bFlush = TRUE;
	}
}

void CAzimFreqObj::CreatePoint()
{
	bFlush = FALSE;
	if (ddir != NULL) delete ddir;
	if (dang != NULL) delete dang;
	ddir = NULL; dang = NULL; 
	m_PointNum = 0;	

	BOOL bOpen1=FALSE;
	BOOL bOpen2=FALSE;
	float SDep,EDep,dep;
	CGeoFIDIO* pDangFIDIO = new CGeoFIDIO(m_strPathName);
	CString StrAzi=m_AziName.Mid(m_AziName.ReverseFind('.')+1);
	StrAzi=m_strPathName.Left(m_strPathName.ReverseFind('.')+1)+StrAzi;
	CGeoFIDIO* pDdirFIDIO = new CGeoFIDIO(StrAzi);
	if(pDdirFIDIO->Open(pDoc->m_szSever)) 
		bOpen1=TRUE;
	if(pDangFIDIO->Open(pDoc->m_szSever)) 
		bOpen2=TRUE;
	if(bOpen1)
	{
		m_Rlev = pDdirFIDIO->GetDepthLevel();
		SDep=pDdirFIDIO->GetStartDepth(); 
		EDep=pDdirFIDIO->GetEndDepth();
	}
	else if(bOpen2) 
	{
		m_Rlev = pDangFIDIO->GetDepthLevel();
		SDep=pDangFIDIO->GetStartDepth(); 
		EDep=pDangFIDIO->GetEndDepth();
	}	
	else
	{
		delete pDangFIDIO; delete pDdirFIDIO; 
		return;
	}
	m_PointNum =(int)((m_PlotEdep-m_PlotSdep)/m_Rlev)+1;

	dang =  new float[m_PointNum];
	ddir =  new float[m_PointNum];
	memset(dang,-9999,m_PointNum);
	memset(ddir,-9999,m_PointNum);
	int k,k1,k2;
	if(bOpen1)
	{
		SDep=pDdirFIDIO->GetStartDepth(); 
		EDep=pDdirFIDIO->GetEndDepth();
		if(EDep>pDoc->m_PlotEdep)
			k2=m_PointNum;
		else
			k2=m_PointNum-(int)((m_PlotEdep-EDep)/m_Rlev);
		if(SDep>pDoc->m_PlotSdep)
		{
			k1=(int)((SDep-m_PlotSdep)/m_Rlev);
			for(k=k1;k<k2;k++)
			{
				dep=(float)(m_PlotSdep+k*m_Rlev);
				pDdirFIDIO->SeekToDepth(dep);
				pDdirFIDIO->Read(2,1,&ddir[k]);
			}
		}
		else
		{
			for(k=0;k<k2;k++)
			{
				dep=(float)(m_PlotSdep+k*m_Rlev);
				pDdirFIDIO->SeekToDepth(dep);
				pDdirFIDIO->Read(2,1,&ddir[k]);
			}
		}
	}
	if(bOpen2) 
	{
		SDep=pDangFIDIO->GetStartDepth(); 
		EDep=pDangFIDIO->GetEndDepth();
		if(EDep>pDoc->m_PlotEdep)
			k2=m_PointNum;
		else
			k2=m_PointNum-(int)((m_PlotEdep-EDep)/m_Rlev);
		if(SDep>pDoc->m_PlotSdep)
		{
			k1=(int)((SDep-m_PlotSdep)/m_Rlev);
			for(k=k1;k<k2;k++)
			{
				dep=(float)(m_PlotSdep+k*m_Rlev);
				pDangFIDIO->SeekToDepth(dep);
				pDangFIDIO->Read(2,1,&dang[k]);
			}
		}
		else
		{
			for(k=0;k<k2;k++)
			{
				dep=(float)(m_PlotSdep+k*m_Rlev);
				pDangFIDIO->SeekToDepth(dep);
				pDangFIDIO->Read(2,1,&dang[k]);
			}
		}
	}	
	delete pDangFIDIO; delete pDdirFIDIO; 
}

void CAzimFreqObj::SetClipRect(CDC* pDC , CRect rect)
{
	if (pDC->m_hDC != NULL) 
		::LPtoDP(pDC->m_hDC,(LPPOINT)(LPRECT)rect,2);
	CRgn rgn; rgn.CreateRectRgn(rect.left,rect.top,rect.right,rect.bottom);
	pDC->SelectClipRgn(&rgn);
}

void CAzimFreqObj::DrawClient(CDC* pDC)
{    
	CRect rect,m_ClipRect,m_AzimRect,m_DangRect;
	rect = m_Rect;
	pDoc->LPtoPP(rect);
	if (!pDC->RectVisible(rect)) return;
	CFIDDrawObj::DrawClient(pDC);   
	
	if(bFlush) CreatePoint();	
	if (m_PointNum == 0) return;

	pDC->GetClipBox(m_ClipRect);
	CRect rc=m_ClipRect;
	rc&= rect;
	rc.InflateRect(0,2);

	int m_TopDepth = (int)m_PlotSdep;
	int m_BottomDepth = (int)m_PlotEdep;

	int x = (rect.left + rect.right) / 2;
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
		
	double m_AzimLevel = 360.0/m_AzimNum;
	double m_DangLevel = (m_RightVal-m_LeftVal)/m_DangNum;
	SetClipRect(pDC,rc);
	while(m_AzimRect.top<rc.bottom)
	{                
		if(m_DangRect.bottom > rc.top)
		{
			int k1 = (int) ((m_TopDepth-pDoc->m_PlotSdep)/m_Rlev); 
			if (k1 < 0) k1 = 0;
			int k2 = (int)((m_TopDepth+m_DepLevel-pDoc->m_PlotSdep)/m_Rlev)+1; 
			if(k2 > m_PointNum) 
			k2 = m_PointNum;
			
			for (int i=0; i<m_AzimNum; i++) 
				m_AzimFreq[i] = 0;
			m_MaxAzimFreq = 0;

			for (i=0; i<m_DangNum; i++) 
				m_DangFreq[i] = 0;
			m_MaxDangFreq = 0;
			
			for (int k=k1; k<k2; k++)
			{
				if((ddir[k]==-9999.f)||(ddir[k]==-999.25f)) 
					continue;

				i = (int)(ddir[k]/m_AzimLevel)-1;
				if(i<0||i>m_AzimNum)
					continue;
				m_AzimFreq[i]++;
				if(m_MaxAzimFreq<m_AzimFreq[i]) 	
					m_MaxAzimFreq = m_AzimFreq[i];

				if((dang[k]==-9999.f)||(dang[k]==-999.25f)) 
					continue;
				if(!((dang[k]<= m_RightVal)&&
					(dang[k] >= m_LeftVal))) 
					continue;
				i = (int)(dang[k]/m_DangLevel)-1;
				if(i<0||i>m_DangNum)
					continue;
				m_DangFreq[i]++;
				if (m_MaxDangFreq<m_DangFreq[i]) 
					m_MaxDangFreq = m_DangFreq[i];
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
			CBrush brush;
			brush.CreateSolidBrush(m_PieColor);
			pDC->SelectObject(&brush);
			//绘制方位频率图
			x = (m_AzimRect.left + m_AzimRect.right) / 2;
			y = (m_AzimRect.top + m_AzimRect.bottom) / 2;			
			if (m_MaxAzimFreq>0)
			{
				CPoint *pt;
				pt=new CPoint[m_AzimNum*2+2];
				int x1,x2,y1,y2,x3,y3;
				for (int i=0; i<m_AzimNum; i++)
			    {
					double r = (double)m_CircleRadius  * m_AzimFreq[i] / m_MaxAzimFreq;
					x1 = x + Round(r*sin(i*m_AzimLevel/180.0*PI));
					y1 = y - Round(r*cos(i*m_AzimLevel/180.0*PI));
					if(i>0)
					{
						pt[i*2].x=x1;pt[i*2].y=y1;
					}
					else 
					{
						pt[i*2].x=x1;pt[i*2].y=y1;
						x3 = x1; y3 = y1;
						pt[m_AzimNum*2].x=x1;pt[m_AzimNum*2].y=y1;
					}
					x2 = x + Round(r*sin((i+1)*m_AzimLevel/180.0*PI));
					y2 = y - Round(r*cos((i+1)*m_AzimLevel/180.0*PI));
					pt[i*2+1].x=x2;pt[i*2+1].y=y2;
				}
				pDC->Polygon(pt,m_AzimNum*2+1);	
				delete pt;		
			}
			else
				pDC->Rectangle(x-5,y-5,x+5,y+5);		
			pDC->SelectObject(pOldBrush);
			//绘制倾角频率图
			if(m_DangBaseLineWidth>0&&m_DangFreqLineWidth>0&&m_MaxDangFreq >0)
			{	
				m_Pen.DeleteObject();
				m_Pen.CreatePen(0,m_DangBaseLineWidth,m_DangBaseLineColor);
				pDC->SelectObject(&m_Pen);
				pDC->MoveTo(m_DangRect.left,m_DangRect.bottom);
				pDC->LineTo(m_DangRect.right,m_DangRect.bottom);
				pDC->SelectObject(pOldPen);

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
			pDC->SelectObject(pOldPen);
		}
		m_TopDepth += m_DepLevel; 
		m_BottomDepth += m_DepLevel;
		yTop = DepthToYCoor(m_TopDepth);
		yTop -= pDoc->m_WinOrg.y;
		m_AzimRect.top = yTop + yLevel/2 - m_CircleRadius;
		m_AzimRect.bottom = m_AzimRect.top + 2 * m_CircleRadius;
		m_DangRect.top = m_AzimRect.bottom+10;
		m_DangRect.bottom = m_DangRect.top + m_DangFreqLineLength;
	}
	SetClipRect(pDC,m_ClipRect);
}


void CAzimFreqObj::DrawHead(CDC* pDC)
{
	CRect rect = m_HeadRect;
	pDoc->LPtoPP(rect);
	if (pDC->RectVisible(rect))
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

BOOL CAzimFreqObj::Properties(CView* pView)
{
	/*CString szHT;
	szHT.LoadString(IDS_DAZFREQUENCYPROPERTIES);
	CPropertySheet m_Sheet(szHT);
	CFreqBasePage m_pBasePage(this);
	CDangFreqPage m_pDangPage(this);
	CAzimFreqPage m_pAzimPage(this);
	TEXTFONT m_TextFont;
	CMinFontPage m_FontPage;

	m_Sheet.AddPage(&m_pBasePage);
	m_Sheet.AddPage(&m_pDangPage);
	m_Sheet.AddPage(&m_pAzimPage);

	CLineListPage m_LineList;
	szHT.LoadString(IDS_FRAMELINE);
	m_LineList.AddPen(&m_FramePen,szHT);
	m_Sheet.AddPage(&m_LineList);
	
	m_Sheet.AddPage(&m_FontPage);

	szHT.LoadString(IDS_ALIASFONT);
	m_TextFont.m_ItemName = szHT.GetBuffer(128);
	szHT.ReleaseBuffer();
	m_TextFont.m_Text = m_AliasName;
	m_TextFont.m_ForeGround = m_NameColor;
	m_TextFont.m_BackGround = RGB(255,255,255);
	m_TextFont.lf = m_NameFont;
	m_FontPage.AddFont(&m_TextFont);
	double m_PlotSdep1 = m_PlotSdep;
	double m_PlotEdep1 = m_PlotEdep;
	CString m_strSavePathName  = m_strPathName;
	if (m_Sheet.DoModal() == IDOK)
	{
		bFlush=TRUE;
		m_FontPage.OnOK();
		m_NameColor = m_TextFont.m_ForeGround;
		m_NameFont = m_TextFont.lf;
		m_NameFont.lfEscapement = 0;

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
				if (m_SaveRect.bottom<m_Rect.bottom) 
					m_SaveRect.bottom = m_Rect.bottom;
			}
		}
		pDoc->SetModifiedFlag();
		InvalidateRect(m_SaveRect);
		return TRUE;
	}*/
	return FALSE;
}

int CAzimFreqObj::HitTest(CView* pView,const CPoint& point)
{
	int nHandle = CFIDDrawObj::HitTest(pView,point); 
	if ((nHandle >= 0) || (!m_Rect.PtInRect(point))) return nHandle;
	if(m_Rect.PtInRect(point)) 
		return 9;
	else return -1;
}


