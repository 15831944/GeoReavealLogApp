/******************************************************************
*   CoreDrw.cpp - implementation of the Core draw object class    *           
*******************************************************************/

#include "stdafx.h" 
#include <math.h> 
#include "PreView.h"
#include "LogPlotDoc.h"
#include "GeoFIDio.h"			    
#include "TrackDraw.h"
#include "CoreDraw.h"
//#include "StyleDialog.h"


CCorePoint::CCorePoint(CPoint point, int nLthNo, int nOGLNo, int nDirect,CLogPlotDoc* pDocument)
{
	pDoc = pDocument;
	m_LthNo = nLthNo;
	m_OGLNo = nOGLNo;
	m_Direct = nDirect;
	m_Point = point;
	CSize size1,size2;
	size1 = GetCoreOGSize(m_OGLNo);
	size2 = GetCoreLthSize(m_LthNo);
	m_Rect.top = m_Point.y - size1.cy + 1;
	m_Rect.bottom = m_Point.y + size2.cy -1;
	int w = (size1.cx > size2.cx) ? size1.cx:size2.cx;
	if (m_Direct == IDM_LEFT)
	{
		m_Rect.left = m_Point.x;
		m_Rect.right = m_Rect.left + w -1;
	}
	else
	{
		m_Rect.right = m_Point.x;
		m_Rect.left = m_Rect.right - w + 1;
	}
}

void CCorePoint::SetPoint(const CPoint& point)
{
	CPoint delta = (CPoint)(point - m_Point);
	m_Rect += delta;
	m_Point = point;
}

void CCorePoint::Draw(CDC* pDC)
{
	CRect rect = m_Rect;
	pDoc->LPtoPP(rect);
	if (!pDC->RectVisible(rect)) return;
	CPoint point =  m_Point;
	pDoc->LPtoPP(&point);
	DrawCore(pDC,point,m_OGLNo,m_LthNo);
}

IMPLEMENT_SERIAL(CCoreObj, CFIDDrawObj, 0)

CCoreObj::CCoreObj()
{
	m_DrawShape = coreobj;
}

CCoreObj::CCoreObj(CString strPathName, const CRect& rect,CDrawObj* pParentObj,CLogPlotDoc* pDoc)
		:CFIDDrawObj(strPathName,rect,pParentObj,pDoc)
{
	m_HeadRect = rect;
	m_HeadRect.bottom = m_HeadRect.top + 100;
	m_HeadRect.right = m_HeadRect.left + 40;
	m_CoreLR = IDM_LEFT;
	m_Rect = m_HeadRect;
	m_Rect.top = m_HeadRect.bottom;
	m_DrawDirect = VERDIR;
	m_DrawShape = coreobj;
	m_AliasName = "ÑÒÐ¾ÆÊÃæ";
	m_NameFont.lfEscapement = 2700;
    m_NameFont.lfHeight = -20;
	m_Brush.lbStyle = BS_SOLID;
}

CCoreObj::~CCoreObj()
{
	POSITION pos = m_CorePoint.GetHeadPosition();
	while (pos != NULL) delete m_CorePoint.GetNext(pos);
	m_CorePoint.RemoveAll();
}    

void CCoreObj::CreatePoint()
{
	double m_CoreDep;
	int m_LthNo,m_OGLNo;
	FILE* m_CoreFile;    
	CPoint point;
	if (m_CoreLR == IDM_LEFT)
		point.x = m_Rect.left+3;
	else if (m_CoreLR == IDM_RIGHT)
		point.x = m_Rect.right-3;	
	else
		point.x = (m_Rect.left + m_Rect.right) / 2;
	POSITION pos = m_CorePoint.GetHeadPosition();
	while (pos != NULL) delete m_CorePoint.GetNext(pos);
	m_CorePoint.RemoveAll();
	char path[512];
	CGeoFIDIO::GetFileName(m_strPathName,path,'1');
	if ((m_CoreFile = fopen(path,"rb")) != NULL)
	{
		int no,color,length;
		while (!feof(m_CoreFile))
		{
			if(fread(&no,sizeof(int),1,m_CoreFile) != 1) break;
			fread(&m_CoreDep,sizeof(double),1,m_CoreFile);
			fread(&color,sizeof(int),1,m_CoreFile);
			fread(&m_LthNo,sizeof(int),1,m_CoreFile);
			fread(&m_OGLNo,sizeof(int),1,m_CoreFile);
			fread(&length,sizeof(int),1,m_CoreFile);
			if (length > 255) break;
			if (fseek(m_CoreFile,length,SEEK_CUR) != 0) break;
			if ((m_CoreDep >= m_PlotSdep) && (m_CoreDep <= m_PlotEdep))
			{
				point.y = DepthToYCoor(m_CoreDep);
				CCorePoint* pObj = new CCorePoint(point,m_LthNo,m_OGLNo, m_CoreLR,pDoc);
				POSITION pos = m_CorePoint.GetHeadPosition();
				CCorePoint* pObj1;
				CRect rect,rect1,tmpRect;
				rect = pObj->GetRect();
				while (pos != NULL)
				{
					pObj1 = (CCorePoint*) m_CorePoint.GetNext(pos);
					rect1 = pObj1->GetRect();
					if (tmpRect.IntersectRect(rect1,rect))
					{
						CPoint point = pObj->GetPoint();
						if (m_CoreLR == IDM_LEFT)
							point.x = rect1.right + 2;
						else
							point.x = rect1.left - 2;
						pObj->SetPoint(point);
						rect = pObj->GetRect();
					}                                         
				}
				m_CorePoint.AddTail(pObj);
			}
   		}  
		fclose(m_CoreFile);
	}
	bFlush = FALSE;
}

void CCoreObj::Serialize(CArchive& ar)
{                      
	CFIDDrawObj::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << m_CoreLR;
	}
	else
	{
		ar >> m_CoreLR;
	}
	CreatePoint();
}

/*
int CCoreObj::HitTest(CView* pView,const CPoint& point)
{
	CRect rect = m_HeadRect;
    rect.top = m_HeadRect.top - 3;
    rect.bottom = m_HeadRect.top + 3;
   	if (rect.PtInRect(point)) return 3;
	if (m_HeadRect.PtInRect(point)) return 0;
	return -1;
}
*/

void CCoreObj::SetRect(const CRect& rect)
{
	CPoint delta,point;    
	CCorePoint* pObj;
	delta.x = rect.right - m_Rect.right;
	delta.y = rect.top - m_Rect.top;
	POSITION pos = m_CorePoint.GetHeadPosition();
	while (pos != NULL)
	{
		pObj = (CCorePoint*) m_CorePoint.GetNext(pos);
		point = pObj->GetPoint();
		point.x += delta.x;
		point.y += delta.y;
		pObj->SetPoint(point);
	}                                         
	CFIDDrawObj::SetRect(rect);
}       

void CCoreObj::DrawHead(CDC* pDC)
{
	CFIDDrawObj::DrawHead(pDC);   
	CRect rect = m_HeadRect;
//	if (!WNT) 
		pDoc->LPtoPP(&rect);
	if (!pDC->RectVisible(rect)) return;

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
	CSize size = pDC->GetTextExtent("ÎÄ±¾");
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
		pDC->TextOut((rect.left+rect.right)/2, (rect.top + rect.bottom+size.cy)/2,m_AliasName);
	pDC->SelectObject(pOldFont);
}   

void CCoreObj::DrawClient(CDC* pDC)
{
	CRect rect = m_Rect;
	pDoc->LPtoPP(&rect);
	if (pDC->RectVisible(rect))
	{
		CFIDDrawObj::DrawClient(pDC);
		CRect m_ClipRect;
		pDC->GetClipBox(m_ClipRect);
		rect &= m_ClipRect;
		if (pDC->m_hDC != NULL) ::LPtoDP(pDC->m_hDC,(LPPOINT)(LPRECT)rect,2);
		CRgn rgn1; rgn1.CreateRectRgn(rect.left,rect.top,rect.right,rect.bottom);
		pDC->SelectClipRgn(&rgn1);

		if (bFlush) CreatePoint();
		CCorePoint* pObj;
		POSITION pos = m_CorePoint.GetHeadPosition();
		while (pos != NULL)
		{
			pObj = (CCorePoint*) m_CorePoint.GetNext(pos);
			pObj->Draw(pDC);
		}
	
		rect = m_ClipRect;
		if (pDC->m_hDC != NULL) ::LPtoDP(pDC->m_hDC,(LPPOINT)(LPRECT)rect,2);
		CRgn rgn2; rgn2.CreateRectRgn(rect.left,rect.top,rect.right,rect.bottom);
		pDC->SelectClipRgn(&rgn2);
	}
}

