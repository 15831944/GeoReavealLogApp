#include "stdafx.h"
#include "style.h"
#include "Grid.h"
#include <math.h>

static long Round(double x)
{
	return (long) (x+0.5);
}


IMPLEMENT_SERIAL(CGridObj, CObject, 0)

CGridObj::CGridObj()
{
	m_bDrawDepLine = TRUE; m_DepLine1 = 1; m_DepLine2 = 5; m_DepLine3 = 10;
	m_bDrawVerLine = TRUE; m_VerLine1 = 10; m_VerLine2 = 5;	m_VerLine = 10; m_GridType = 0;
	m_DepPen1.lopnWidth = CPoint(1,1); 
	m_DepPen1.lopnColor = RGB(0xC0, 0xC0, 0xC0);
	m_DepPen1.lopnStyle = 0;
	m_DepPen2.lopnWidth = CPoint(1,1); 
	m_DepPen2.lopnColor = 0x0;//RGB(0,0x80,0); 
	m_DepPen2.lopnStyle = 0;
	m_DepPen3.lopnWidth = CPoint(2,2); 
	m_DepPen3.lopnColor = 0x0;//RGB(0,0x80,0); 
	m_DepPen3.lopnStyle = 0;
	m_VerPen1.lopnWidth = CPoint(1,1); 
	m_VerPen1.lopnColor = RGB(0xC0, 0xC0, 0xC0); 
	m_VerPen1.lopnStyle = 0;
	m_VerPen2.lopnWidth = CPoint(1,1); 
	m_VerPen2.lopnColor =  0x0;//RGB(0,0x80,0);
	m_VerPen2.lopnStyle = 0;
	m_Rect.SetRect(0,0,0,0);
	m_PlotSdep = 0.0;
	m_PlotEdep = 0.0;
	m_ZerX = NULL;
	m_AngX = NULL;
	m_bHoriOpaque = FALSE;
	m_bVertOpaque = TRUE;
	m_bDrawRuler=FALSE;
	if(m_GridType == DIPGRID)
	{
		m_LeftVal = 0;
		m_RightVal = 90;
	}
}           

CGridObj::~CGridObj()
{
	if (m_ZerX != NULL) delete m_ZerX;
	if (m_AngX != NULL) delete m_AngX;
}

CGridObj::CGridObj(const CRect& rect, double sdep, double edep, int nGridType)
{
	m_LeftVal = 1; m_RightVal = 100; m_GridType = nGridType;
	m_bDrawDepLine = TRUE; m_DepLine1 = 1; m_DepLine2 = 5; m_DepLine3 = 10;
	m_bDrawVerLine = TRUE; m_VerLine = m_VerLine1 = 10; m_VerLine2 = 5; 
	m_bDrawRuler=FALSE;
	m_DepPen1.lopnWidth = CPoint(1,1); 
	m_DepPen1.lopnColor = RGB(0xC0, 0xC0, 0xC0);
	m_DepPen1.lopnStyle = 0;
	m_DepPen2.lopnWidth = CPoint(1,1); 
	m_DepPen2.lopnColor = 0x0;//RGB(0,0x80,0); 
	m_DepPen2.lopnStyle = 0;
	m_DepPen3.lopnWidth = CPoint(2,2); 
	m_DepPen3.lopnColor = 0x0;//RGB(0,0x80,0); 
	m_DepPen3.lopnStyle = 0;
	m_VerPen1.lopnWidth = CPoint(1,1); 
	m_VerPen1.lopnColor = RGB(0xC0, 0xC0, 0xC0); 
	m_VerPen1.lopnStyle = 0;
	m_VerPen2.lopnWidth = CPoint(1,1); 
	m_VerPen2.lopnColor =  0x0;//RGB(0,0x80,0);
	m_VerPen2.lopnStyle = 0;
	if (m_GridType != LINEGRID) 
		m_VerPen2.lopnWidth = CPoint(2,2);
	m_Rect = rect;
 	m_PlotSdep = sdep;
 	m_PlotEdep = edep;
	m_ZerX = NULL;
	m_AngX = NULL;
	if(m_GridType == DIPGRID)
	{
		m_LeftVal = 0;
		m_RightVal = 90;
	}
	bDrawDepth=FALSE;	
	m_DepthTrackPosition=0;
	m_bDrawRuler=FALSE;
	CreateVerLine();
}                  

void CGridObj::Serialize(CArchive& ar)
{                
	WORD w;         
	if (ar.IsStoring())
	{
		ar << m_LeftVal; ar << m_RightVal; 
		ar << (WORD)m_GridType;
		
		ar << (WORD) m_bDrawDepLine;
		ar << (WORD) m_DepLine1;
		ar << (WORD) m_DepLine2;
		ar << (WORD) m_DepLine3;
		ar << (WORD) m_bDrawVerLine;
		ar << (WORD) m_VerLine;
		ar << (WORD) m_VerLine1;
		ar << (WORD) m_VerLine2;
		SerializeLogPen(ar,m_DepPen1);
		SerializeLogPen(ar,m_DepPen2);
		SerializeLogPen(ar,m_DepPen3);
		SerializeLogPen(ar,m_VerPen1);
		SerializeLogPen(ar,m_VerPen2);
		ar << m_PlotSdep;
		ar << m_PlotEdep;                   
		ar << m_Rect;
		ar << m_bHoriOpaque;
		ar << m_bVertOpaque;
	}
	else
	{
		// get the document back pointer from the archive
		ar >> m_LeftVal; ar >> m_RightVal; 
		ar >> w; m_GridType = (int)w;
		ar >> w; m_bDrawDepLine = (BOOL) w;
		ar >> w; m_DepLine1 = (int) w;
		ar >> w; m_DepLine2 = (int) w;
		ar >> w; m_DepLine3 = (int) w;
		ar >> w; m_bDrawVerLine = (BOOL) w;
		ar >> w; m_VerLine = (int) w;
		ar >> w; m_VerLine1 = (int) w;
		ar >> w; m_VerLine2 = (int) w;
		SerializeLogPen(ar,m_DepPen1);
		SerializeLogPen(ar,m_DepPen2);
		SerializeLogPen(ar,m_DepPen3);
		SerializeLogPen(ar,m_VerPen1);
		SerializeLogPen(ar,m_VerPen2);
		ar >> m_PlotSdep;
		ar >> m_PlotEdep;
		ar >> m_Rect;
		ar >> m_bHoriOpaque;
		ar >> m_bVertOpaque;
		CreateVerLine();
	}               
}

void CGridObj::SetRect(const CRect& rect)
{
	if (m_Rect == rect) return;
	m_Rect.top = rect.top;
	m_Rect.bottom = rect.bottom;
	if (m_Rect.left != rect.left || (m_Rect.right != rect.right)) 
	{
		m_Rect.left = rect.left;
		m_Rect.right = rect.right;
		CreateVerLine();
	}
}                 

void CGridObj::SetDepth(double sdep, double edep)
{
	m_PlotSdep = sdep;
	m_PlotEdep = edep;
}                     

void CGridObj::SetDepC(double depc)
{
	m_DepC = depc;
}

CRect CGridObj::GetRect()
{
	return m_Rect;	
}                 

double CGridObj::YCoorToDepth(long y)
{
	
	return ((y - m_Rect.top) * m_DepC + m_PlotSdep);
}

long CGridObj::DepthToYCoor(double depth)
{
	return (long) ((depth - m_PlotSdep) / m_DepC + m_Rect.top);
}

void CGridObj::CreateVerLine()
{
	if (!m_bDrawVerLine) return;
	if (m_ZerX != NULL) delete m_ZerX;
	m_ZerX = NULL;
	m_VerLine = 0;
	if(m_GridType == LOGGRID)
	{
		int xinc = 1;
		int x;  
		double xv,xvl,xvr;
		m_ZerX = new int[100];
		if (m_LeftVal > m_RightVal) xinc = -1;
		if (m_LeftVal < 0.0001) m_LeftVal = 0.0001;
		if (m_RightVal < 0.0001) m_RightVal = 0.0001;
		xv = (int)log10(m_LeftVal);
		xvl = log10(m_LeftVal);	xvr = log10(m_RightVal);
		if (xinc*(xvl-xv) < 0)  xv -= xinc;
		int k = 1;   
		if (xinc < 0) k = 10;
		x = Round(m_Rect.left + (xv - xvl) / (xvr - xvl) * m_Rect.Width());
		m_ZerX[0] = -m_Rect.left; 
		m_VerLine = 1;
		while (x < m_Rect.right)
		{            
			x = Round(m_Rect.left + (xv - xvl) / (xvr - xvl) * m_Rect.Width());
			if (x > m_Rect.left)
			{
				if ((k == 1) || (k == 10))
					m_ZerX[m_VerLine] = -x;   
				else
					m_ZerX[m_VerLine] = x;
				m_VerLine++;
			}
			if (xinc > 0)
			{
				k += xinc;
				xv += (log10((double)k) - log10((double)k-1)) * xinc;
				if (k == 10) k = 1;
			}
			else
			{	
				xv += (log10((double)k) - log10((double)k-1)) * xinc;
				k += xinc;
				if (k == 1) k = 10;
			}
		}
		m_ZerX[m_VerLine] = -m_Rect.right;
		m_VerLine++;
	}
	else if (m_GridType == DIPGRID)
	{
		div_t x;
		int i,j,k,n;
		int m_ScalePara = 234;
	 	int m_ScaleArray[10];
		i = m_ScalePara;
		int m_ScaleNum = 0;
		m_VerLine = 0; n = 0;
		while (1)
		{
			x = div(i,10);
	    	i = x.quot;
		    if(x.rem == 0) break;
			m_ScaleArray[m_ScaleNum] = x.rem;
			n += x.rem;
			m_ScaleNum ++;
			if (x.quot == 0) m_VerLine += x.rem*5; else m_VerLine += x.rem*2;
		}
		if (n==0) return;                     
		if (m_AngX != NULL) delete m_AngX;
		m_VerLine += 1;
		m_AngX = new double[m_VerLine];
		m_ZerX = new int[m_VerLine];
		m_ZerX[0] = -m_Rect.left; m_ZerX[m_VerLine-1] = -m_Rect.right;
		m_AngX[0] = m_LeftVal; m_AngX[m_VerLine-1] = m_RightVal;
		double angAdd = (m_RightVal - m_LeftVal) / n;
		int n1 = 0;		
		for (i=0; i<m_ScaleNum; i++)
		{
			n1 += Round(m_ScaleArray[i]*pow(2,i));			
		}                           
		float xAdd;
		float xx = (float) -m_ZerX[0];       
		int divNum;   
		n = 1;
		for (i=m_ScaleNum-1; i>=0; i--)
		{
			if (i == m_ScaleNum-1) divNum = 5; else divNum = 2;
			xAdd = (float)(pow(2,i) * m_Rect.Width() / n1 /divNum);
			for (j=0; j<m_ScaleArray[i]; j++)
			for (k=0; k<divNum; k++)	
			{              
				xx += xAdd;
				m_ZerX[n] = Round(xx);
				if (k == divNum-1) m_ZerX[n] = -m_ZerX[n];
				m_AngX[n] = m_AngX[n-1] + angAdd / divNum;
				n ++;
			}
		}
	}
	else
	{
		m_VerLine = m_VerLine1 + 1;
		m_ZerX = new int[m_VerLine];
		m_ZerX[0] = -m_Rect.left; m_ZerX[m_VerLine-1] = -m_Rect.right;
		for (int i=1;i<m_VerLine1;i++)
		{               
			int xx = m_Rect.left + Round(((float)m_Rect.Width())/m_VerLine1*i);
			if ((i % m_VerLine2) == 0) m_ZerX[i] = -xx;
			else m_ZerX[i] = xx;
		}
	}
}

void CGridObj::DrawVertLine(CDC* pDC)
{
	CPen m_Pen1,m_Pen2,m_Pen3,*pOldPen;
	CRect rect = m_Rect;
	CRect m_ClipRect;
	pDC->GetClipBox(m_ClipRect);
	m_ClipRect &= rect;
	if (!pDC->RectVisible(m_ClipRect)) return;
	if (m_bDrawVerLine)
	{
		m_Pen1.CreatePenIndirect(&m_VerPen1);
		m_Pen2.CreatePenIndirect(&m_VerPen2);
		int xx;
		for (int i=1;i<m_VerLine-1;i++)
		{               
			if(m_ZerX[i] < 0) 
			{ 
				xx = -m_ZerX[i]; 
				if((m_VerPen2.lopnWidth.x >0)&&(m_VerPen2.lopnWidth.y >0))
				{
					pOldPen = pDC->SelectObject(&m_Pen2);
					pDC->MoveTo(xx,m_ClipRect.top);
					pDC->LineTo(xx,m_ClipRect.bottom);
					pDC->SelectObject(pOldPen);
				}
			}
			else 
			{
				xx = m_ZerX[i]; 
				if((m_VerPen1.lopnWidth.x >0)&&(m_VerPen1.lopnWidth.y >0))
				{
					pOldPen = pDC->SelectObject(&m_Pen1);
					pDC->MoveTo(xx,m_ClipRect.top);
					pDC->LineTo(xx,m_ClipRect.bottom);
					pDC->SelectObject(pOldPen);
				}
			}
		}
		m_Pen1.DeleteObject();
		m_Pen2.DeleteObject();
	}	
}

void CGridObj::DrawHoriLine(CDC* pDC)
{
	CPen m_Pen1,m_Pen2,m_Pen3,m_Pen4,*pOldPen;
	CRect rect = m_Rect;
	CRect m_ClipRect;
	pDC->GetClipBox(m_ClipRect);
	m_ClipRect &= rect;
	if(!pDC->RectVisible(m_ClipRect)) return;
	int i,y0,y;
	unsigned int m_Depth,m_Sdep,m_Edep;
	if(m_bDrawDepLine) 
	{
    	m_Pen1.CreatePenIndirect(&m_DepPen1);
    	m_Pen2.CreatePenIndirect(&m_DepPen2);
    	m_Pen3.CreatePenIndirect(&m_DepPen3);
		m_Sdep = (unsigned int) (YCoorToDepth(m_ClipRect.top));
		m_Edep = (unsigned int) (YCoorToDepth(m_ClipRect.bottom));
		for(m_Depth = m_Sdep; m_Depth <= m_Edep; m_Depth++)
		{
			y = DepthToYCoor(m_Depth);
			if((y >= m_Rect.top) && (y <= m_Rect.bottom))
			{	
				if ((m_Depth % m_DepLine3) == 0)		
					pOldPen = pDC->SelectObject(&m_Pen3);
				else if ((m_Depth % m_DepLine2) == 0) 
					pOldPen = pDC->SelectObject(&m_Pen2);
				else if ((m_Depth%m_DepLine1) == 0) 
					pOldPen = pDC->SelectObject(&m_Pen1);
				else continue;
				pDC->MoveTo(m_Rect.left,y);
				pDC->LineTo(m_Rect.right,y);
				pDC->SelectObject(pOldPen);
			}
		}
		//处理深度间隔为1米的细格线 主要用于成像绘图
		if(m_DepLine3==1)
		{
			m_Sdep=(int)m_PlotSdep*10;
    		m_Edep=(int)m_PlotEdep*10;
			pOldPen = pDC->SelectObject(&m_Pen1);
			for(m_Depth = m_Sdep;m_Depth <= m_Edep; m_Depth++)
			{
				if((m_Depth%2==0)&&(m_Depth%10!=0))
				{
					y = DepthToYCoor(m_Depth/10.f);
					if((y >= m_Rect.top) && (y <= m_Rect.bottom))
					{
						pDC->MoveTo(m_Rect.left,y);
						pDC->LineTo(m_Rect.right,y);
					}
				}
			}
			pDC->SelectObject(pOldPen);
		}
		m_Pen1.DeleteObject();
		m_Pen2.DeleteObject();
		m_Pen3.DeleteObject();
	}
	if(bDrawDepth&&m_bDrawRuler)  //绘深度尺&&m_DepthTrackPosition==2
	{
    	m_Pen4.CreatePenIndirect(&m_DepPen1);
		pOldPen = pDC->SelectObject(&m_Pen4);
		//处理深度间隔为1米的细格线 主要用于成像绘图
		if(m_DepLine3==1)
		{
			m_Sdep=(int)m_PlotSdep*10;
    		m_Edep=(int)m_PlotEdep*10;
			if(m_DepthTrackPosition==0)
			{
				for(m_Depth = m_Sdep;m_Depth <= m_Edep; m_Depth++)
				{
					if(m_Depth%2==0)
					{
						y = DepthToYCoor(m_Depth/10.f);
						y0=DepthToYCoor(m_Depth/10.f+0.2f);
						if((y >= m_Rect.top) && (y0 <= m_Rect.bottom))
						{
							pDC->MoveTo(m_Rect.left,y);
							pDC->LineTo(m_Rect.left-7,y);
							for(i=1;i<5;i++)
							{
								pDC->MoveTo(m_Rect.left,y+(y0-y)*i/5);
								pDC->LineTo(m_Rect.left-4,y+(y0-y)*i/5);
							}
						}
					}
				}
			}
			else if(m_DepthTrackPosition==1)
			{
				for(m_Depth = m_Sdep;m_Depth <= m_Edep; m_Depth++)
				{
					if(m_Depth%2==0)
					{
						y = DepthToYCoor(m_Depth/10.f);
						y0=DepthToYCoor(m_Depth/10.f+0.2f);
						if((y >= m_Rect.top) && (y0 <= m_Rect.bottom))
						{
							pDC->MoveTo(m_Rect.right,y);
							pDC->LineTo(m_Rect.right+7,y);
							for(i=1;i<5;i++)
							{
								pDC->MoveTo(m_Rect.right,y+(y0-y)*i/5);
								pDC->LineTo(m_Rect.right+4,y+(y0-y)*i/5);
							}
						}
					}
				}
			}
			else
			{
				for(m_Depth = m_Sdep;m_Depth <= m_Edep; m_Depth++)
				{
					if(m_Depth%2==0)
					{
						y = DepthToYCoor(m_Depth/10.f);
						y0=DepthToYCoor(m_Depth/10.f+0.2f);
						if((y >= m_Rect.top) && (y0 <= m_Rect.bottom))
						{
							pDC->MoveTo(m_Rect.left,y);
							pDC->LineTo(m_Rect.left+7,y);
							pDC->MoveTo(m_Rect.right,y);
							pDC->LineTo(m_Rect.right-7,y);
							for(i=1;i<5;i++)
							{
								pDC->MoveTo(m_Rect.left,y+(y0-y)*i/5);
								pDC->LineTo(m_Rect.left+4,y+(y0-y)*i/5);
								pDC->MoveTo(m_Rect.right,y+(y0-y)*i/5);
								pDC->LineTo(m_Rect.right-4,y+(y0-y)*i/5);
							}
						}
					}
				}
			}
		}
		else
		{
			m_Sdep = (unsigned int) (YCoorToDepth(m_ClipRect.top));
			m_Edep = (unsigned int) (YCoorToDepth(m_ClipRect.bottom));
			if(m_DepthTrackPosition==0)
			{
				for (m_Depth = m_Sdep; m_Depth <= m_Edep; m_Depth++)
				{
					if((m_Depth%m_DepLine3)==0||
						(m_Depth%m_DepLine2)==0) 
					{
						y = DepthToYCoor(m_Depth);
						if((y>= m_Rect.top) && (y <= m_Rect.bottom))
						{	
							pDC->MoveTo(m_Rect.left,y);
							pDC->LineTo(m_Rect.left-7,y);
						}
					}
				}
				m_Sdep=(int)m_PlotSdep;
    			m_Edep=(int)m_PlotEdep;
				for(m_Depth = m_Sdep;m_Depth <= m_Edep; m_Depth++)
				{
					if((m_Depth%m_DepLine1) == 0) 
					{
						y=DepthToYCoor(m_Depth);
						y0=DepthToYCoor(m_Depth+m_DepLine1);
						pDC->MoveTo(m_Rect.left,y);
						pDC->LineTo(m_Rect.left-7,y);
						if((y>= m_Rect.top) && (y0 <= m_Rect.bottom))
						{	
							for(i=1;i<5;i++)
							{
								pDC->MoveTo(m_Rect.left,y+(y0-y)*i/5);
								pDC->LineTo(m_Rect.left-4,y+(y0-y)*i/5);
							}
						}
					}
				}
			}
			else if(m_DepthTrackPosition==1)
			{
				for (m_Depth = m_Sdep; m_Depth <= m_Edep; m_Depth++)
				{
					if((m_Depth%m_DepLine3)==0||
						(m_Depth%m_DepLine2) == 0) 
					{
						y = DepthToYCoor(m_Depth);
						if((y>= m_Rect.top) && (y <= m_Rect.bottom))
						{	
							pDC->MoveTo(m_Rect.right,y);
							pDC->LineTo(m_Rect.right+7,y);
						}
					}
				}
				m_Sdep=(int)m_PlotSdep;
    			m_Edep=(int)m_PlotEdep;
				for(m_Depth = m_Sdep;m_Depth <= m_Edep; m_Depth++)
				{
					if((m_Depth%m_DepLine1) == 0) 
					{
						y=DepthToYCoor(m_Depth);
						pDC->MoveTo(m_Rect.right,y);
						pDC->LineTo(m_Rect.right+7,y);
						y0=DepthToYCoor(m_Depth+m_DepLine1);
						if((y>= m_Rect.top) && (y0 <= m_Rect.bottom))
						{	
							for(i=1;i<5;i++)
							{
								pDC->MoveTo(m_Rect.right,y+(y0-y)*i/5);
								pDC->LineTo(m_Rect.right+4,y+(y0-y)*i/5);
							}
						}
					}
				}
			}
			else
			{
				for (m_Depth = m_Sdep; m_Depth <= m_Edep; m_Depth++)
				{
					if((m_Depth%m_DepLine3)==0||
						(m_Depth%m_DepLine2) == 0) 
					{
						y = DepthToYCoor(m_Depth);
						if((y>= m_Rect.top) && (y <= m_Rect.bottom))
						{	
							pDC->MoveTo(m_Rect.left,y);
							pDC->LineTo(m_Rect.left+7,y);
							pDC->MoveTo(m_Rect.right,y);
							pDC->LineTo(m_Rect.right-7,y);
						}
					}
				}
				m_Sdep=(int)m_PlotSdep;
    			m_Edep=(int)m_PlotEdep;
				for(m_Depth = m_Sdep;m_Depth <= m_Edep; m_Depth++)
				{
					if((m_Depth%m_DepLine1) == 0) 
					{
						y=DepthToYCoor(m_Depth);
						y0=DepthToYCoor(m_Depth+m_DepLine1);
						if((y>= m_Rect.top) && (y0 <= m_Rect.bottom))
						{	
							for(i=1;i<5;i++)
							{
								pDC->MoveTo(m_Rect.left,y+(y0-y)*i/5);
								pDC->LineTo(m_Rect.left+4,y+(y0-y)*i/5);
								pDC->MoveTo(m_Rect.right,y+(y0-y)*i/5);
								pDC->LineTo(m_Rect.right-4,y+(y0-y)*i/5);
							}
						}
					}
				}
			}
		}
		pDC->SelectObject(pOldPen);
		m_Pen4.DeleteObject();
	}
}