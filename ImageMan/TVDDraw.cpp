/******************************************************************
*   CCLDrw.cpp - implementation of the CCL draw object class  *           
*******************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "MainFrm.h"
#include "ImageMan.h"
#include "ImageManDoc.h"
#include "FIDIO.h"
#include "styledialog.h"
#include "TrackDraw.h"
#include "TVDDraw.h"
//#include "TVDPage.h"
//#include "TVDBasePage.h"
#include "MinFontDialog.h"
#include "CurveDraw.h"
IMPLEMENT_SERIAL(CTVDObj, CFIDDrawObj, 0)
        
CTVDObj::CTVDObj():CCurveObj()
{
	//起始垂深校正量
	m_DaltaDep=0.f;
	//显示深度间距
	m_nLevel=2;
	//细刻度线数
	m_nGrid=10;
}
    
CTVDObj::CTVDObj(CString szPathName, const CRect& rect,CDrawObj* pParentObj,CImageManDoc* pDoc)
			:CCurveObj(szPathName,rect,pParentObj,pDoc)
{
	//起始垂深校正量
	m_DaltaDep=0.f;
	//显示深度间距
	m_nLevel=2;
	//细刻度线数
	m_nGrid=10;
}

CTVDObj::~CTVDObj()
{
	if(m_Point != NULL) 
		delete m_Point;
	m_Point = NULL;
	if(m_Value != NULL) 
		delete m_Value;
	m_Value = NULL;
	m_ScaleFont.lfHeight=-16;
}    

void CTVDObj::Serialize( CArchive& ar )
{
	CFIDDrawObj::Serialize( ar );
	m_ScriptFont.lfUnderline=0;
	if(ar.IsStoring())
	{
		SerializeLogFont(ar,m_ScaleFont);
		ar << m_CalType;
		ar << m_Factor;
		ar << m_CurveStyleNo;
		ar << m_CurveWidth;
		ar << m_CurveColor;
		ar << m_DaltaDep;
		ar << m_nLevel;
		ar << m_nGrid;
	}
	else
	{
		SerializeLogFont(ar,m_ScaleFont);
		ar >> m_CalType;
		ar >> m_Factor;
		ar >> m_CurveStyleNo;
		ar >> m_CurveWidth;
		ar >> m_CurveColor;
		ar >> m_DaltaDep;
		ar >> m_nLevel;
		ar >> m_nGrid;
	}
}

BOOL CTVDObj::Properties(CView* pView)
{
	return FALSE;
}

void CTVDObj::DrawHead(CDC *pDC)
{
	CRect rect = m_HeadRect;
	pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;    
	CFont* pOldFont;
	
	CFont m_Font;
	m_Font.CreateFontIndirect(&m_ScaleFont);
	pOldFont = pDC->SelectObject(&m_Font);
	pDC->SetTextColor(m_CurveColor);
	pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
	pDC->TextOut((rect.left + rect.right)/2, rect.bottom - 7, m_UnitText, strlen(m_UnitText));

	CFont m_Font1;
	m_Font1.CreateFontIndirect(&m_NameFont);
    pDC->SelectObject(&m_Font1);
    pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
    CSize textsize;
	if (m_UnitText == "") textsize.cy = 0;
	else textsize = pDC->GetTextExtent("文本");  
    pDC->TextOut((rect.left + rect.right)/2, rect.bottom - textsize.cy -10, m_AliasName,strlen(m_AliasName));
	pDC->SelectObject(pOldFont);
	CPen Pen;
	Pen.CreatePen(0,m_CurveWidth,m_CurveColor);
	CPen *pOldPen = pDC->SelectObject(&Pen); 
	pDC->MoveTo(rect.left,rect.bottom);
	pDC->LineTo(rect.right,rect.bottom);
	CPen Pen1;
	Pen1.CreatePen(0,1,m_CurveColor);
	pDC->SelectObject(&Pen1); 
	int x;
	for(int i=9;i>0;i--)
	{
		x=rect.right-(int)(rect.Width()*log10(i*1.f));
		pDC->MoveTo(x,rect.bottom);pDC->LineTo(x,rect.bottom-8);
	}
	pDC->SelectObject(pOldPen);
}

int CTVDObj::CreatePoint(double sdep, double edep)
{
	if(m_Point != NULL) 
		delete m_Point;
	m_Point=NULL;
	if(m_Value != NULL)
		delete m_Value;
	m_Value=NULL;
	m_PointNum = 0;
	if(m_bErrorFID) return 0;
	m_bErrorFID = TRUE;
	m_EDep = edep;
	m_SDep = sdep;

	CFIDIO* pFIDIO = new CFIDIO(m_strPathName);
	if(pFIDIO->Open() && IsDepthUnitOK(pFIDIO))
	{
		m_Rlev = pFIDIO->GetDepthLevel();
		if (m_Rlev > 0.001) 
			m_bErrorFID = FALSE;
	}
	if(m_bErrorFID) 
	{
		m_SDep = m_EDep = m_SSdep = m_EEdep = 0;
		delete(pFIDIO);    
		m_bFlush = FALSE;
		return m_PointNum;
	}
	double StartDep = pFIDIO->GetStartDepth();
	double EndDep = pFIDIO->GetEndDepth();
	if((m_SDep>EndDep) ||(m_EDep<StartDep)) 
	{
		delete(pFIDIO);    
		m_bFlush = FALSE;
		return m_PointNum;
	}
	if(m_SDep < StartDep) m_SDep = StartDep;
	if(m_EDep > EndDep) m_EDep = EndDep;
	m_PointNum = (int)((m_EDep - m_SDep) / m_Rlev) + 1 ;
	if(m_PointNum < 0) 
		m_PointNum = 0;
	int i;
	double dep=StartDep+m_DaltaDep;
	if(m_PointNum > 0)
	{
		//显示井段以上的垂深校正
		if(m_SDep>StartDep)
		{
			int Num= (int)((m_SDep - StartDep) / m_Rlev) + 1;
			m_Value = new float[Num];
			memset(m_Value,-9999,Num);
			pFIDIO->SeekToDepth((float)StartDep);     
			Num = pFIDIO->Read(2,(unsigned int )Num,m_Value);
			for(i=0; i<Num; i++)
			{   
				if(m_Value[i]<=0.f)
					dep+=m_Rlev;
				else
					dep+=m_Rlev*cos(m_Value[i]*PI/180.f);
			}
			delete m_Value;m_Value=NULL;
		}
		m_Value = new float[m_PointNum];
		memset(m_Value,-9999,m_PointNum);

		m_Point = new CPoint[m_PointNum];
		pFIDIO->SeekToDepth((float)m_SDep);     
		m_PointNum = pFIDIO->Read(2,(unsigned int )m_PointNum,m_Value);
		m_EDep = m_SDep + (m_PointNum-1) * m_Rlev; 
		for(i=0; i<m_PointNum; i++)
		{   
			if(m_Factor==0)			//靠左边框
				m_Point[i].x =m_Rect.left;
			else if(m_Factor==1)	//靠右边框
				m_Point[i].x =m_Rect.right;
			else					//对称绘图
				m_Point[i].x =(m_Rect.left+m_Rect.right)/2;
			m_Point[i].y = DepthToYCoor(m_SDep+i*m_Rlev); 
			if(m_Value[i]<=0.f)
				dep+=m_Rlev;
			else
				dep+=m_Rlev*cos(m_Value[i]*PI/180.f);
			m_Value[i]=(float)dep;
		}
	}
	delete pFIDIO;    
	m_bFlush = FALSE;
	edep = m_EDep;
	sdep = m_SDep;
	return m_PointNum;
}
void CTVDObj::DrawClient(CDC *pDC)
{  
	if (m_bErrorFID) return;         
	if(IsGridCurve()) 
		m_bErrorFID = FALSE;
	CRect rect = m_Rect;
	pDoc->LPtoPP(&rect);
	if(!pDC->RectVisible(rect)) return;
	if((m_pParentObj == NULL) || (m_ParentType != 0)) 
		CFIDDrawObj::DrawClient(pDC);
	
	double sdep = YCoorToDepth(rect.top + pDoc->m_WinOrg.y);
	double edep = YCoorToDepth(rect.bottom  + pDoc->m_WinOrg.y);
	if(sdep < m_PlotSdep) 
		sdep = m_PlotSdep;
	if(edep > m_PlotEdep) 
		edep = m_PlotEdep;

	if(m_bFlush) CreatePoint(sdep,edep);
	if(m_PointNum > 2) 
	{
		pDoc->LPtoPP(m_Point,m_PointNum);                
		CPen Pen;
		Pen.CreatePen(0,m_CurveWidth,m_CurveColor);
		CPen *pOldPen = pDC->SelectObject(&Pen); 
		CPen Pen1;
		Pen1.CreatePen(0,1,m_CurveColor);

		CFont Font;
		Font.CreateFontIndirect(&m_ScaleFont);
	    CFont *pOldFont = pDC->SelectObject(&Font);
		pDC->SetTextColor(m_CurveColor);
	    pDC->SetTextAlign(TA_LEFT|TA_TOP);
		CSize sz=pDC->GetTextExtent("4680");
		int i,j,k,y,y0,y1,yy0;
		char ss[10]; 
		//录找显示第一个垂深点
		float dep=((int)(m_Value[0]/m_nLevel))*m_nLevel*1.f+m_nLevel;
		for(k=1;k<m_PointNum;k++)
		{
			if(m_Value[k]>dep)
			{
				y=m_Point[k].y;
				break;
			}
		}
		if(y>rect.top&&y<rect.bottom)
		{
			dtos(dep,ss);
			sz=pDC->GetTextExtent(ss);
			if(m_Factor==0)			//靠左边框
			{
				pDC->MoveTo(rect.left,y);
				pDC->LineTo(rect.left+10,y);
				pDC->TextOut(rect.left+15,y-sz.cy/2,ss);
			}
			else if(m_Factor==1)	//靠右边框
			{
				pDC->MoveTo(rect.right,y);
				pDC->LineTo(rect.right-10,y);
				pDC->TextOut(rect.right-sz.cx-15,y-sz.cy/2,ss);
			}
			else					//对称绘图
			{
				pDC->MoveTo(rect.left,y);
				pDC->LineTo(rect.left+10,y);
				pDC->MoveTo(rect.right,y);
				pDC->LineTo(rect.right-10,y);
				pDC->TextOut((rect.left+rect.right-sz.cx)/2,y-sz.cy/2,ss);
			}
			dep+=m_nLevel;
		}
		y0=y;yy0=y;
		for(i=k+1;i<m_PointNum;i++)
		{
			if(m_Value[i]>dep)
			{
				y=m_Point[i].y;
				if(y>rect.top&&y<rect.bottom)
				{
					dtos(dep,ss);
					sz=pDC->GetTextExtent(ss);
					if(m_Factor==0)			//靠左边框
					{
						pDC->MoveTo(rect.left,y);
						pDC->LineTo(rect.left+10,y);
						if(y-yy0>sz.cy)
						{
							pDC->TextOut(rect.left+15,y-sz.cy/2,ss);
							yy0=y;
						}
					}
					else if(m_Factor==1)	//靠右边框
					{
						pDC->MoveTo(rect.right,y);
						pDC->LineTo(rect.right-10,y);
						if(y-yy0>sz.cy)
						{
							pDC->TextOut(rect.right-sz.cx-15,y-sz.cy/2,ss);
							yy0=y;
						}
					}
					else					//对称绘图
					{
						pDC->MoveTo(rect.left,y);
						pDC->LineTo(rect.left+10,y);
						pDC->MoveTo(rect.right,y);
						pDC->LineTo(rect.right-10,y);
						if(y-yy0>sz.cy)
						{
							pDC->TextOut((rect.left+rect.right-sz.cx)/2,y-sz.cy/2,ss);
							yy0=y;
						}
					}
					dep+=m_nLevel;
				}
				if(y-y0>m_nGrid*2)
				{
					pDC->SelectObject(&Pen1); 
					if(m_Factor==0)			//靠左边框
					{
						for(j=0;j<m_nGrid;j++)
						{
							y1=y0+(y-y0)*j/m_nGrid;
							pDC->MoveTo(rect.left,y1);
							pDC->LineTo(rect.left+5,y1);
						}
					}
					else if(m_Factor==1)	//靠右边框
					{
						for(j=0;j<m_nGrid;j++)
						{
							y1=y0+(y-y0)*j/m_nGrid;
							pDC->MoveTo(rect.right,y1);
							pDC->LineTo(rect.right-5,y1);
						}
					}
					else					//对称绘图
					{
						for(j=0;j<m_nGrid;j++)
						{
							y1=y0+(y-y0)*j/m_nGrid;
							pDC->MoveTo(rect.left,y1);
							pDC->LineTo(rect.left+5,y1);
							pDC->MoveTo(rect.right,y1);
							pDC->LineTo(rect.right-5,y1);
						}
					}
				}
				y0=y;
			}
		}
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldFont);
		pDoc->PPtoLP(m_Point,m_PointNum);
	}
}